# Lab04 Traps

## RISC-V assembly

### 实验目的

回答问题。

### 实验内容

编译一个程序，回答问题。

### 遇到的问题

无

### 实验心得

通过观察汇编代码了解了高级语言向汇编的转换，更进一步接近底层了。

## Back trace

### 实验目的

在`kernel/printf.c`中实现一个函数`backtrace()`。在`sys_sleep`中插入这个函数，然后执行`bttest`以调用`sys_sleep`。输出应该如下：

```bash
backtrace:
0x0000000080002cda
0x0000000080002bb6
0x0000000080002898
```

这个函数实际就是实现曾经调用函数地址的回溯。

### 实验内容

1. 在`kernel/defs.h`中加入函数声明，并在`kernel/printf.c`中加入`void backtrace()`函数。
2. 在`kernel/riscv.h`中加入实验给出的`r_fp()`函数用于读取寄存器fp中的值。
3. 在`sys_sleep()`中调用`backtrace()`
4. 实现backtrace：
   1. 将fp传入`PGROUNDUP`和`PGROUNDDOWN`并做差。当差等于页表大小时做while循环
   2. 在循环内，返回地址保存在-8偏移的位置
   3. 前一个页帧指针保存在-16偏移的位置

### 遇到的问题

不太理解操作系统栈空间和栈分布。后来经过查阅资料与观看视频后对“函数调用栈”有了一定的认知：它是由高地址向低地址增长，且大小为一页的栈。另外，栈指针保存在sp寄存器中，且初始在高地址。另外，一开始我担心指针越界的问题，后来了解到用`PGROUNDUP`和`PGROUNDDOWN`可以获取栈帧所在页的页地址的范围。

### 实验心得

在完成这个实验时，我遇到了一些挑战，尤其是在理解操作系统栈空间和栈分布方面。最初，我对函数调用栈的工作原理不太了解，包括栈指针的位置以及栈帧的保存方式。这导致我在实现`backtrace()`函数时感到困惑。通过查阅资料和观看相关视频，我逐渐理解了其工作原理。每个函数调用都会在栈上创建一个栈帧，用于保存函数的返回地址、参数和局部变量等信息。理解了这些概念后，我开始着手实现`backtrace()`函数。做完这个实验我对调试中的追踪功能产生了深深的敬意，挺难实现的。

### 实验截图

![backtrace](..\src\Lab04\backtrace.bmp "backtrace截图")

## Alarm

### 实验目的



### 实验内容



### 遇到的问题



### 实验心得



### 实验截图



## Make grade截图



## 代码位置

##### backtrace

`kernel/printf.c` col: 137

`kernel/riscv.h` col: 326



## 附：gdb调试xv6代码

### 调试步骤：

1. 在一个终端输入`make qemu-gdb`，可以指定某一CPU执行，如`make CPUS=1 qemu-gdb`，在make qemu后会提示去另一个终端启动gdb，并给出此终端的端口号。

   ![gdb-port](..\src\Lab04\gdb-port.bmp)

2. 新建一个终端，输入`gdb-multiarch kernel/kernel`，输入以下代码，注意端口号输自己的。

   ```bash
   (gdb) set architecture riscv:rv64
   (gdb) target remote localhost:26000
   ```

   

3. 加载文件。如果调试的是用户态文件，则需要加一个`user/`，如：

   ```
   (gdb) file user/mkdir
   ```

   

4. 打断点，用`b xxx`命令。一般会打在main开头，`b main`；或打在某一行，如`b trace.c:20`表示在`mkdir.c`的第20行打断点。

5. 继续执行，用`c`命令。

6. 触发断点后，按步执行即可，以下给出gdb常用指令。

   | 指令名       | 指令作用                        |
   | ------------ | ------------------------------- |
   | b            | 打断点                          |
   | c            | 继续执行                        |
   | layout split | 显示源代码和汇编代码            |
   | ni           | 单步汇编执行，不进函数          |
   | si           | 单步汇编执行，进入函数          |
   | n            | 单步C语言执行，对应VS中“逐过程” |
   | s            | 单步C语言执行，对应VS中“逐语句” |
   | p $a0        | 打印a0寄存器中的值              |
   | p/x 1536     | 以16进制格式打印1536            |
   | i r a0       | a0寄存器信息                    |
   | x/i 0x630    | 查看0x630地址处的指令           |

### 调试效果

![gdb](..\src\Lab04\gdb.bmp)

