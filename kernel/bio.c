// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#if 0
#define NBUCKET 7
#define HASH(id) (id % NBUCKET)

struct hashbuf{
  struct buf head;
  struct spinlock lock;
};

struct {
  struct buf buf[NBUF];
  struct hashbuf buckets[NBUCKET];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
} bcache;

void
binit(void)
{
  struct buf *b;
  char lockname[16];

  for(int i=0;i<NBUCKET; i++){
    snprintf(lockname, sizeof(lockname), "bcache_%d", i);
    initlock(&bcache.buckets[i].lock, lockname);

    bcache.buckets[i].head.prev = &bcache.buckets[i].head;
    bcache.buckets[i].head.next = &bcache.buckets[i].head;
  }

  // Create linked list of buffers
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->next = bcache.buckets[0].head.next;
    b->prev = &bcache.buckets[0].head;
    initsleeplock(&b->lock, "buffer");
    bcache.buckets[0].head.next->prev = b;
    bcache.buckets[0].head.next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  int bid = HASH(blockno);
  acquire(&bcache.buckets[bid].lock);

  // Is the block already cached?
  for(b = bcache.buckets[bid].head.next; b != &bcache.buckets[bid].head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;

      // record uses of timestamp
      acquire(&tickslock);
      b->timestamp = ticks;
      release(&tickslock);

      release(&bcache.buckets[bid].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  b=0;
  struct buf *tmp;
  // Recycle the least recently used (LRU) unused buffer.
  for(int i = bid, cycle = 0; cycle != NBUCKET; i = (i + 1) % NBUCKET) {
    ++cycle;
    // hit hash bucket
    if(i != bid) {
      if(!holding(&bcache.buckets[i].lock))
        acquire(&bcache.buckets[i].lock);
      else
        continue;
    }

    for(tmp = bcache.buckets[i].head.next; tmp != &bcache.buckets[i].head; tmp = tmp->next)
      // use timestamp to operate lru
      if(tmp->refcnt == 0 && (b == 0 || tmp->timestamp < b->timestamp))
        b = tmp;

    if(b) {
      // other buckets
      if(i != bid) {
        b->next->prev = b->prev;
        b->prev->next = b->next;
        release(&bcache.buckets[i].lock);

        b->next = bcache.buckets[bid].head.next;
        b->prev = &bcache.buckets[bid].head;
        bcache.buckets[bid].head.next->prev = b;
        bcache.buckets[bid].head.next = b;
      }

      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;

      acquire(&tickslock);
      b->timestamp = ticks;
      release(&tickslock);

      release(&bcache.buckets[bid].lock);
      acquiresleep(&b->lock);
      return b;
    } 
    else {
      // doesnt hit
      if(i != bid)
        release(&bcache.buckets[i].lock);
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  int bid = HASH(b->blockno);
  releasesleep(&b->lock);

  //不再获取全局锁
  acquire(&bcache.buckets[bid].lock);
  b->refcnt--;

  // update timestamp
  acquire(&tickslock);
  b->timestamp = ticks;
  release(&tickslock);

  /*
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
  */
  release(&bcache.buckets[bid].lock);
}

void
bpin(struct buf *b) {
  int bid = HASH(b->blockno);
  acquire(&bcache.buckets[bid].lock);
  b->refcnt++;
  release(&bcache.buckets[bid].lock);
}

void
bunpin(struct buf *b) {
  int bid = HASH(b->blockno);
  acquire(&bcache.buckets[bid].lock);
  b->refcnt--;
  release(&bcache.buckets[bid].lock);
}
#else
#define NBUCKET 5 
#define time_stamp timestamp
struct {
  struct spinlock lock;
  struct buf buf[NBUF];//NBUF=30

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  // struct buf head;
  
} bcache[NBUCKET];

uint idx(uint blockno)
{
   return blockno % NBUCKET;
}

void
binit(void)
{
  struct buf *b;
  
  
  for(int i=0;i<NBUCKET;i++) 
  {  
      	for(b = bcache[i].buf; b < bcache[i].buf+NBUF; b++){ 
             initsleeplock(&b->lock, "buffer");
         }
      initlock(&bcache[i].lock, "bcache.bucket");
  }
}


static struct buf*
bget(uint dev, uint blockno)
{
  
  
  struct buf *b=0;

  int i= idx(blockno);
  uint min_time_stamp=-1;
  struct buf *min_b=0;
  

  acquire(&bcache[i].lock);  //只需要持有一个锁，避免竞争


  // Is the block already cached?
  for(b = bcache[i].buf; b < bcache[i].buf+NBUF; b++){  
          if(b->dev==dev && b->blockno == blockno)//hit
          {    
              b->refcnt++;
              release(&bcache[i].lock);
              acquiresleep(&b->lock);
              return b;
          }// 找到最小的时间戳对应buf
          if(b->refcnt==0 && b->time_stamp<min_time_stamp)
          {
	       min_time_stamp=b->time_stamp;
	       min_b=b;
          }
      
 }

  b=min_b;
  if(b!=0)
  {
	    b->dev = dev;
         b->blockno = blockno;
         b->valid = 0;
         b->refcnt = 1;
	     release(&bcache[i].lock);
         acquiresleep(&b->lock);
         return b;
  } 

  
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}


void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");
 
  releasesleep(&b->lock);
  int i=idx(b->blockno);

  acquire(&bcache[i].lock);
  b->refcnt--;
  
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->time_stamp=ticks;
  }
  release(&bcache[i].lock);
}

void
bpin(struct buf *b) {
  
  int i=idx(b->blockno);	
  acquire(&bcache[i].lock);	
  b->refcnt++;
  release(&bcache[i].lock);	
	      
}

void
bunpin(struct buf *b) {
  int i=idx(b->blockno);	
  acquire(&bcache[i].lock);	
  b->refcnt--;
  release(&bcache[i].lock);			  
}
#endif
