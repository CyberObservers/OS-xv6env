// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  char *ref_page;
  int page_count;
  char *_end;
} kmem;

struct ref_stru {
  struct spinlock lock;
  int cnt[PHYSTOP / PGSIZE];  // 引用计数
} ref;


int
pgcount(void *pa_start, void *pa_end)
{
  char *p;
  int count=0;
  p=(char *)PGROUNDUP((uint64)pa_start);
  for(;p + PGSIZE <= (char *)pa_end; p+=PGSIZE)
    count++;
  return count;
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  kmem.page_count = pgcount(end, (void *) PHYSTOP);

  kmem.ref_page = end;
  for(int i=0; i < kmem.page_count; i++){
    kmem.ref_page[i] = 0;
  }
  kmem._end = kmem.ref_page + kmem.page_count;


  initlock(&kmem.lock, "kmem");
  initlock(&ref.lock, "ref");
  freerange(kmem._end, (void*)PHYSTOP);
}

int
page_index(uint64 pa)
{
  pa = PGROUNDDOWN(pa);
  int res = (pa - (uint64)kmem._end)/PGSIZE;
  if(res<0||res>=kmem.page_count){
    panic("page_index wrong");
  }

  return res;
}

void
incr(void *pa)
{
  int index = page_index((uint64)pa);
  acquire(&kmem.lock);
  kmem.ref_page[index]++;
  release(&kmem.lock);
}

void
desc(void *pa)
{
  int index = page_index((uint64)pa);
  acquire(&kmem.lock);
  kmem.ref_page[index]--;
  release(&kmem.lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
#if 1
void
kfree(void *pa)
{
  int index = page_index((uint64)pa);
  if(kmem.ref_page[index] > 1){
    desc(pa);
    return;
  }
  if(kmem.ref_page[index]==1){
    desc(pa);
  }

  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}
#else
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // 只有当引用计数为0了才回收空间
  // 否则只是将引用计数减1
  acquire(&ref.lock);
  if(--ref.cnt[(uint64)pa / PGSIZE] == 0) {
    release(&ref.lock);

    r = (struct run*)pa;

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  } else {
    release(&ref.lock);
  }
}
#endif

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
#if 1
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    incr(r);
  }
  
  return (void*)r;
}
#else
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    acquire(&ref.lock);
    ref.cnt[(uint64)r / PGSIZE] = 1;  // 将引用计数初始化为1
    release(&ref.lock);
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
#endif

void
print_free_pages_cnt()
{
  struct run *r;
  int cnt = 0;

  acquire(&kmem.lock);
  r = kmem.freelist;
  while(r->next){
    r = r->next;
    cnt++;
  }
  release(&kmem.lock);

  printf("print_free_pages_cnt: %d\n", cnt);

}