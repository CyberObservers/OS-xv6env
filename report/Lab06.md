# Lab06 Multitreading

## Uthread: switching between threads

### 实验目的

本实验是在给定的代码基础上实现用户级线程切换。因为是用户级线程，不需要设计用户栈和内核栈，用户页表和内核页表等等切换。

### 实验内容

1. 在`user/uthread.c`中定义存储上下文的结构体context_t
2. 在`user/uthread.c`中修改`thread`结构体，添加`context`字段。
3. 模仿`kernel/swtch.S`，在`kernel/uthread_switch.S`中写入代码用于上下文切换
4. 在`user/uthread.c`中修改`thread_schedule()`，添加线程切换语句
5. 在`user/uthread.c`中，在`thread_create`中对`thread`结构体做一些初始化设定，主要是`ra`返回地址和`sp`栈指针。

### 遇到的问题和解决方法

对上下文切换具体原理不是很了解，参考***switch.S***后对代码的写法有了了解。

### 实验心得

实现了用户态线程，可以感觉到开销比内核态线程小，因为不用费劲的维护例如`trapframe`的数据。

### 实验截图

![uthread](..\src\Lab06\uthread.bmp)

## Using threads

### 实验目的

在探索使用哈希表的线程和锁的并行编程中，解决多线程读写数据时散列桶数据丢失的问题。

### 实验内容

1. 在`ph.c`中为每个散列桶定义一个锁，将五个锁放在一个数组中，并进行初始化。
2. 在`put()`中对`insert()`上锁。

### 遇到的问题和解决方法

无

### 实验心得

将同步互斥的知识运用到实践中。

### 实验截图

![ph_safe](..\src\Lab06\ph_safe.bmp)

## Barrier

### 实验目的

实现一个屏障：应用程序中的一个点，所有参与的线程在此点上必须等待，直到所有其他参与线程也达到该点。在实验中，使用pthread条件变量。这是一种序列协调技术，类似于xv6的`sleep`和`wakeup`。

### 实验内容

完成`notxv6/barrier.c`中`barrier(void)`函数。具体操作为：

1. 申请锁
2. 调用barrier的线程+1
3. 判断是否所有线程调用barrier
   1. 是：轮次+1，线程数清零
   2. 否：等待未到达的线程
4. 释放锁。

### 遇到的问题和解决方法

与信号量隔离不同，这里用的是一个函数进行隔离的实现。学到了用函数隔离的方法。一开始以为允许线程在两个轮次执行，但是总是失败。后来参考别人的思路了解到应当等所有线程都调用barrier后再开启新的轮次。

### 实验心得

了解了UNIX下`barrier(void)`的实现。

### 实验截图

![uthread](..\src\Lab06\barrier.bmp)

## Make grade截图

![uthread](..\src\Lab06\grade-lab06.bmp)



## 代码位置

#### Uthread: switching between threadsd

`user/uthread.c` col: 14, 33

`user/uthread_switch.S` col: 14

`user/thread.c` col: 89, 104

#### Using threads

`notxv6/ph.c` col: 58

#### Barrier

`notxv6/barrier.c` col: 33

