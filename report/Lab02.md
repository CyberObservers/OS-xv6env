# Lab02 Sys call

## System call tracing

#### 1. 实验目的

在本作业中，一个系统调用跟踪功能将被实现。实验人员将创建一个新的`trace`系统调用来控制跟踪。它应该有一个参数，这个参数是一个整数“掩码”（mask），它的比特位指定要跟踪的系统调用。例如，要跟踪`fork`系统调用，程序调用`trace(1 << SYS_fork)`，其中`SYS_fork`是***kernel/syscall.h***中的系统调用编号。如果在掩码中设置了系统调用的编号，则必须修改xv6内核，以便在每个系统调用即将返回时打印出一行。该行应该包含进程id、系统调用的名称和返回值；不需要打印系统调用参数。`trace`系统调用应启用对调用它的进程及其随后派生的任何子进程的跟踪，但不应影响其他进程。

#### 2. 实验内容

1. 阅读`user/trace.c`中的内容。发现它首先调用`trace(int)`，再将`argv`除了前两个参数外的其他参数写入新的`nargv`中，再调用`exec`函数（util-xargs中也有）从而执行指令。
2. 在`kernel/proc.h`文件的`proc`结构体中加入一个新的字段`trace_mask`用于保存`trace`的参数。并在`sys_trace()`的实现中实现参数的保存，具体方法是调用`argint(int, (int *))`函数。
3. 根据题目提示，由于`struct proc`中增加了一个新的变量,当`fork`的时候我们也需要将这个变量传递到子进程中。具体做法是访问子进程的PCB，将相应字段拷贝过去。
4. 接下来考虑如何实现系统调用`trace`了，具体需要更改`kernel/sysproc.c syscall()`。需要注意的是，`trace_mask`是比特掩码，需要用位运算检查有哪些需要追踪。更改后的` syscall()`具体步骤是：
   1. 获取当前进程的PCB，`struct proc *p = myproc()`
   2. 获取该进程的系统调用编号，在`p->trapframe->a7`字段（寄存器）中，类型为`int`。
   3. 执行系统调用，将结果保存在`p->trapframe->a0`中。
   4. 通过`(1<<num) & p->trace_mask`检查是否匹配。
5. 在`kernel/syscall.c`中注册`sys_trace`。具体有三步：
   1. `extern uint64 sys_trace(void)`
   2. 注册函数：在`static uint64 (*syscalls[])(void)`中加入`[SYS_trace]   sys_trace,`
   3. 注册系统调用名：在`static char *syscalls_name[]`中加入`[SYS_trace]   "trace",`
6. 在***Makefile***的**UPROGS**中添加`$U/_trace`
7. 在***user/user.h***中加入`int trace(int);`
8. 在***user/usys.pl***中加入`entry("sysinfo");`

#### 3. 遇到的问题及解决方法

第一次在内核态下编程，对系统调用的分层架构和实现原理不太理解，导致不知道怎样注册系统调用，怎样获取进程控制块等操作。在阅读内核态其他函数的源码后，我对这些操作有了一定的认知。在编写syscall核心代码时，由于对位运算不太了解，我在列出系统调用时花费了一些时间。另外，由于需要获取进程的系统调用，我还去了解了一下对应寄存器的作用。

#### 4. 实验心得

lab01在用户态下通过系统调用实现内核态的操作。而lab02开始就是从内核态编写一个新的系统调用。我学习到了xv6下实现系统调用的全流程操作，包括注册、编写、调用。阅读了表驱动的系统调用注册表，包括名称、函数指针等。

#### 5. 实验截图

![trace](E:\大二下\操作系统\课设文档\src\Lab02\trace.bmp "trace实验截图")

## Sys info

#### 1. 实验目的

在这个作业中，您将添加一个系统调用`sysinfo`，它收集有关正在运行的系统的信息。系统调用采用一个参数：一个指向`struct sysinfo`的指针（参见***kernel/sysinfo.h***）。内核应该填写这个结构的字段：`freemem`字段应该设置为空闲内存的字节数，`nproc`字段应该设置为`state`字段不为`UNUSED`的进程数。我们提供了一个测试程序`sysinfotest`；如果输出“**sysinfotest: OK**”则通过。

#### 2. 实验内容

1. 在`kernel/kalloc.c`中添加一个函数`getFreeMem(void)`用于获取空闲内存量。由于内存是使用链表进行管理的，因此遍历`kmem`中的空闲链表就能够获取所有空闲内存，函数内是遍历链表的基本操作。
2. 在`kernel/proc.c`中添加一个函数`procnum(uint64 *dst)`获取当前系统进程数。具体方法为遍历`proc`数组，检查正在使用的PCB以获取进程数。
3. 实现`sys_sysinfo`，将数据写入结构体并传递到用户空间。
4. 注册该系统调用（同trace）
5. 注册用户态`sysinfotest`测试

#### 3. 遇到的问题及解决方法

本实验有三个板块：一、获取空闲块数；二、获取进程数；三、内核态向用户态传输数据。而这些都需要通过查阅xv6内核态源码获得。这需要理解其代码逻辑与对应的数据结构，有些耗时。

#### 4. 实验心得

在这个实验中，我学习了如何添加一个系统调用`sysinfo`，它可以收集有关系统的一些信息，比如空闲内存和进程数。这个实验让我对xv6的内存管理和进程管理有了更深入的了解，也锻炼了我的代码阅读和编写能力。通过这个实验，我实现了系统调用函数`sys_sysinfo`，它接受一个指向用户空间的结构体指针作为参数。这个结构体定义在`kernel/sysinfo.h`中，包含两个字段：`freemem`和`nproc`。我学习到可以使用`copyout()`函数将数据从内核空间复制到用户空间，从而实现内核态向用户态的数据传输。从本质上理解了用户态和内核态的不同——地址空间不同。只要可以通过指针写，就可以访问。

#### 5. 实验截图



## make grade 截图

![grade-lab02](E:\大二下\操作系统\课设文档\src\Lab02\grade-lab02.bmp "lab02成绩")

### 代码位置



`kernel/proc.h` col: 109

`kernel/sysproc.c` col: 101, 117

`kernel/proc.c` col: 274

`kernel/syscall.c` col: 158, 159, 162

`kernel/kalloc.c` col: 85

`kernel/proc.c` col: 664

