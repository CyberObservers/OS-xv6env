# Lab00 Tools 环境搭建
## 激活WSL服务
控制面板->启动或关闭Windows功能->选择"Windows虚拟机监控程序平台", "适用于Linux的Windows子系统"和"虚拟机平台", 接着重启电脑.

## 安装操作系统
在终端输入以下指令(以Ubuntu 20.04为例):
```
wsl --install -d Ubuntu-20.04
```
## VS code远程连接Linux
安装Remote-WSL插件, 然后在Linux目录下输入: 
```
$ code .
```
即可自动连接VS code, 也可以通过VS code连接

## xv6环境配置: 安装RISC-V等工具
在Linux下输入以下命令即可
```bash
$ sudo apt-get update && sudo apt-get upgrade
$ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

测试安装:
```bash
# in the xv6 directory
$ make qemu
# ... lots of output ...
init: starting sh
$
```

## 克隆实验仓库
若无git, 请先安装git. 执行以下代码: 
```bash
$ git clone git://g.csail.mit.edu/xv6-labs-2021
```

## 配置远程仓库
非本实验重点, 仅提及.
1.  在GitHub上创建一个仓库
2.  在WSL中填写邮箱与姓名信息
3.  在WSL中生成并获取ssh密钥
4.  在GitHub中添加ssh密钥
5.  复制仓库的SSH key
6.  在本地仓库目录下更改远程仓库url
7.  将本地分支push到远程仓库
常用指令:
```bash
$ git add . # 将修改添加到暂存区
$ git commit -m "msg" # 保存所有本地修改, 并添加注解(msg)
$ git push origin branch_name # 将某一分支push到远程仓库
$ git remote -v # 查看push和pull的远程仓库名
$ git status # 查看状态
```

**配置完成!**

## 实验中的git命令

若要切换到其他分支且本分支有修改，则需要：

```bash
$ git add .	# 将修改添加到文件树
$ git commit -m "msg" # 确认修改，并注解
$ git push # 推送到远程仓库
$ git checkout <branch_name> # 切换分支
```

**若不保存切换不了分支，若切换了则不会保存修改！！！**
