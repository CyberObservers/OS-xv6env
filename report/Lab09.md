# Lab09 file system

## Large files

### 实验目的

本实验的目的是增加xv6文件的最大大小。由于目前一个xv6 inode包含12个“直接”块号和一个“间接”块号，“一级间接”块指一个最多可容纳256个块号的块，总共12+256=268个块，所以，xv6文件限制为`268*BSIZE`字节（在xv6中`BSIZE`为1024）。更改xv6文件系统代码，以支持每个inode中可包含256个一级间接块地址的“二级间接”块，每个一级间接块最多可以包含256个数据块地址。结果将是一个文件将能够包含多达65803个块，或256*256+256+11个块（11而不是12，因为我们将为二级间接块牺牲一个直接块号）。

### 实验内容

1. 在`fs.h`中添加宏定义：NDIRECT是直接块个数；NINDIRECT是一级块个数；NDINDIRECT是二级块个数。
2. 由于`NDIRECT`定义改变，其中一个直接块变为了二级间接块，需要修改inode结构体中`addrs`元素数量。
3. 修改`bmap`支持二级索引。
4. 修改`itrunc`释放所有块。

### 遇到的问题和解决方法

二级目录相较一级目录多了一次for循环，其他还好。就是需要追踪文件块被定义、访问和释放的全过程，有些繁琐。

### 实验心得

在真实的操作系统中实现了二级目录，很有意义。

### 实验截图

![bigfile](..\src\Lab09\bigfile.bmp)

## Symbolic links

### 实验目的

添加符号链接，以实现类似快捷方式的功能。实现`symlink(char *target, char *path)`系统调用，该调用在引用由`target`命名的文件的路径处创建一个新的符号链接。有关更多信息，请参阅`symlink`手册页（注：执行`man symlink`）。要进行测试，请将`symlinktest`添加到***Makefile***并运行它。

### 实验内容

1. 配置系统调用的常规操作，如在`user/usys.pl`、`user/user.h`中添加一个条目，在`kernel/syscall.c`、`kernel/syscall.h`中添加相关内容
2. 添加提示中的相关定义，`T_SYMLINK`以及`O_NOFOLLOW`
3. 在`kernel/sysfile.c`中实现`sys_symlink`，这里需要注意的是`create`返回已加锁的inode，此外`iunlockput`既对inode解锁，还将其引用计数减1，计数为0时回收此inode
4. 修改`sys_open`支持打开符号链接

### 遇到的问题和解决方法

这个系统调用应该在`kernel/sysfile.c`中实现，而我在`kernel/sysproc.c`中实现时发现无法调用create函数，因为它被声明为静态。

程序中`begin_op()`和`end_op()`令我费解。进行搜索后得知这两句话既声明了对文件系统操作的开始和结束，也让操作系统确定了执行操作的时机——在完成所有更新之前，不执行任何一个操作。

### 实验心得

在课堂上对软连接的讲解十分有限，实验补足了我在这方面的认知。

### 实验截图

![symlink](..\src\Lab09\symlink.bmp)

## Make grade

![grade-lab09](..\src\Lab09\grade-lab09.bmp)

## 代码位置

**Large files**

`kernel/fs.h`: 27, 34

`kernel/fs.c`: 378, 438

**Symbolic links**

`kernel/sys_file.c`: 289, 525