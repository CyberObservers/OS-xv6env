# Lab03 Page table

## Speed up system calls

### 1. 实验目的

创建每个进程时，在 USYSCALL（一个 VA 定义的 在`memlayout.h`中）。在此页的开头，存储一个`结构 usyscall`（也在`memlayout.h`中定义），并将其初始化以存储 当前进程的 PID。对于这个实验室，`ugetpid（）` 已经 在用户空间端提供，并将自动使用 USYSCALL 映射。 如果`pgtbltest`中的`ugetpid`测试通过，实验完成。

### 2. 实验内容

1. 在`krenel/proc.c`中的`proc_pagetable(struct proc *p)`声明一个共享内存。映射`USYSCALL`，授权用户态与读，具体做法是`PTE_R | PTE_U`。并对内存初始化。
2. 如果第一步失败则解除与`USYSCALL`和`TRAMPOLINE`的映射关系，并释放内存。
3. 在`krenel/proc.c`中的`allocproc(void)`函数中分配一个`USYSCALL`页。
4. 在`krenel/proc.c`中的`freeproc(void)`函数中释放该页。
5. 在在`krenel/proc.c`中的`proc_freepagetable(void)`函数中解除与物理内存的映射关系。

### 3. 遇到的问题及解决方法

由于对用户态和内核态的区别和联系不太了解，不清楚如何添加这块共享内存，也不清楚如何设置共享内存的权限设置。另外，对内核态如何释放内存、如何解除映射关系花了一些时间阅读源码。

### 4. 实验心得

通过这个实验，我进一步深入了解了操作系统的内核编程和页表管理，了解了提高系统运行性能 的一种方法是在用户态和内核态之间建立只读的共享内存。我学会了在内核中为进程创建共享内存，设置页表映射关系和权限，以及在释放资源时解除映射关系并释放内存。同时，我对于调试和测试内核代码有了更多的经验。这让我对操作系统的内部工作原理有了更深入的理解，对于我未来的系统编程和操作系统研究有了更好的基础。

### 5. 实验截图

![getpid](E:\大二下\操作系统\课设文档\src\Lab03\getpid.bmp "getpid实验截图")

## Print a page table

### 1. 实验目的

定义一个名为 `vmprint（）` 的函数。 它应该有 一个`pagetable_t`参数，并打印该页表 采用下述格式。 在`exec.c `的`exec`函数中，在`return argc` 之前插入 `if（p->pid==1） vmprint（p->pagetable）`  ， 以打印第一个进程的页表。 如果通过 `Make Grade` 的 `PTE printout`测试，则实验完成。

### 2. 实验内容

1.  在`exec.c `的`exec`函数中，在`return argc` 之前插入 `if（p->pid==1） vmprint（p->pagetable）`  ， 以打印第一个进程的页表。
2. 早`kernel/vm.c`中声明一个`vmprint`函数，该函数是一个递归函数。
3. 阅读参考`freewallk`函数部分代码，可知一页有512个页表项。遍历它们，如果是页表地址则递归调用。利用了DFS的思想。
4. 限制递归深度为2（系统资源有限，页表深度也不大）。
5. 
6. 用`%p`打印地址。

### 3. 遇到的问题及解决方法

虽然直到采用DFS的思想遍历页表，但一页有多少页表项一开始不太清楚。后来根据实验提示查阅了`freewalk`函数后获取了页表项。第二个问题是一开始在printf的格式化字符串中没有用%p打印地址，造成了一些困扰。

### 4. 实验心得

通过解决上述问题，我成功地实现了`vmprint`函数，并且能够在`exec`函数中打印第一个进程的页表。这个实验让我更深入地理解了页表的结构和页表项的数量，以及如何使用递归来遍历页表。在完成这个试验后，我对虚拟内存与多级页表有了更深入的理解。

### 5. 实验截图

![print](E:\大二下\操作系统\课设文档\src\Lab03\print.bmp "print pgtbl实验截图")

## Detecting which pages have been accessed

### 1. 实验目的

本实验中，一个名为 `pgaccess()`的系统调用函数将被实现，它返回已被访问的页。该系统调用接收三个参数。首先，它需要 要检查的第一个用户页面的起始虚拟地址。其次，它需要 要检查的页数。最后，它将用户地址带到缓冲区进行存储 结果进入位掩码（一种每页使用一位的数据结构，其中 第一页对应于最低有效位）。

### 2. 实验内容

1. 在`kernel/sysproc.c`中补全`sys_pgaccess()`函数：
2. 在`sys_pgaccess()`中，用`argaddr()`和`argint()`接收参数（内核态下接收参数与用户态不同）。接收页表个数、掩码、和入口地址（虚拟）。
3. 在`sys_pgaccess()`中，建立一个中间变量保存那些页表被访问过。这里用的是一个int变量。
4. 设置页表个数上限，这里设置了32.
5. 在`kernel/riscv.h`中定义`PTE_A`，值为`1 << 6`.
6. 在`pgaccess()`中，从入口地址开始遍历，调用`vmpgaccess()`（在`kernel/vm.c`中实现）。并按位与到中间变量对应位。
7. 在`kernel/vm.c`中，定义`vmpgaccess(pagetable_t pagetable, uint64 va)`。在该函数中判断一个页是否被访问。具体方法为调用walk函数返回页表物理入口，再将`PTE_A`置为0，返回1.若入口为0，就返回0.

### 3. 遇到的问题及解决方法

不了解虚拟地址、物理地址的读取方法与转换方法；对多级页表的认识只停留于纸上，面对真正的操作系统无法调用系统函数实现逻辑。主要是不了解内核态下文件的意义以及其中函数实现的功能，加之语言障碍对我产生了非常大的困难。挣扎一番后决定看实验提示并结合多级页表的图思考。一条条看过去后逐渐有了思路。

### 4. 实验心得

初看这个实验是有一种无从下手的感觉，内核态仍然是黑箱子的存在。在看实验提示后对标志位有了新的认识。在解决问题的过程中，我学会了如何在内核态下接收参数，如何进行虚拟地址和物理地址的转换，以及如何操作多级页表来判断页是否被访问过。同时，我也更加熟悉了操作系统的内核编程环境和文件结构。

### 5. 实验截图

![detect](E:\大二下\操作系统\课设文档\src\Lab03\detect.bmp "detect实验截图")

## Make grade 截图

![grade-lab03](E:\大二下\操作系统\课设文档\src\Lab03\grade-lab03.bmp "lab03 grade")

## 代码位置

##### speed up system calls

`kernel/proc.c` col: 130，159，227，211

##### print a page table

`kernel/exec.c` col: 121

`kernel/vm.c` col: 444

##### detecting which pages have been accessed

`kernel/sysproc.c`  col: 81

`kernel/vm.c` col: 466

`kernel/riscv.h` col: 346

