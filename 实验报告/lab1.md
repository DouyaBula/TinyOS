**目录**
[TOC]
---

# 思考题

## Thinking 1.1

### (1) MIPS交叉编译工具链重复过程 

附录里已经用gcc演示过一次了，这里用mips交叉编译器重复一遍过程。

`hello.c`:

```c
#include <stdio.h>
int main(){
    printf("hello warudo!\n");
    return 0;
}
```

预处理 (部分) : 

```bash
git@21371136:~/os/thinkings/lab1 $ mips-linux-gnu-gcc -E hello.c > temp
git@21371136:~/os/thinkings/lab1 $ cat temp
extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
# 885 "/usr/mips-linux-gnu/include/stdio.h" 3
extern int __uflow (FILE *);
extern int __overflow (FILE *, int);
# 902 "/usr/mips-linux-gnu/include/stdio.h" 3
# 2 "hello.c" 2
# 2 "hello.c"
int main(){
    printf("hello warudo!\n");
    return 0;
}
```

只编译不链接后的反汇编 (部分) :

```asm
00000000 <main>:
   0:   27bdffe0        addiu   sp,sp,-32
   4:   afbf001c        sw      ra,28(sp)
   8:   afbe0018        sw      s8,24(sp)
   c:   03a0f025        move    s8,sp
  10:   3c1c0000        lui     gp,0x0
  14:   279c0000        addiu   gp,gp,0
  18:   afbc0010        sw      gp,16(sp)
  1c:   3c020000        lui     v0,0x0
  20:   24440000        addiu   a0,v0,0
  24:   8f820000        lw      v0,0(gp)
  28:   0040c825        move    t9,v0
  2c:   0320f809        jalr    t9
  30:   00000000        nop
  34:   8fdc0010        lw      gp,16(s8)
  38:   00001025        move    v0,zero
  3c:   03c0e825        move    sp,s8
  40:   8fbf001c        lw      ra,28(sp)
  44:   8fbe0018        lw      s8,24(sp)
  48:   27bd0020        addiu   sp,sp,32
  4c:   03e00008        jr      ra
  50:   00000000        nop
```

正常编译后的反汇编 (部分) :

```asm
004006e0 <main>:
  4006e0:       27bdffe0        addiu   sp,sp,-32
  4006e4:       afbf001c        sw      ra,28(sp)
  4006e8:       afbe0018        sw      s8,24(sp)
  4006ec:       03a0f025        move    s8,sp
  4006f0:       3c1c0042        lui     gp,0x42
  4006f4:       279c9010        addiu   gp,gp,-28656
  4006f8:       afbc0010        sw      gp,16(sp)
  4006fc:       3c020040        lui     v0,0x40
  400700:       24440830        addiu   a0,v0,2096
  400704:       8f828030        lw      v0,-32720(gp)
  400708:       0040c825        move    t9,v0
  40070c:       0320f809        jalr    t9
  400710:       00000000        nop
  400714:       8fdc0010        lw      gp,16(s8)
  400718:       00001025        move    v0,zero
  40071c:       03c0e825        move    sp,s8
  400720:       8fbf001c        lw      ra,28(sp)
  400724:       8fbe0018        lw      s8,24(sp)
  400728:       27bd0020        addiu   sp,sp,32
  40072c:       03e00008        jr      ra
  400730:       00000000        nop
```

可以看到反汇编结果的第10行和第11行发生了变化, 这说明在链接时printf被插入到了最终的可执行文件中. 

### (2) objdump传入参数的含义

`-D`: 对所有section进行反汇编.

`-S`: 源代码与反汇编混合.



## Thinking 1.2

### (1) 解析内核文件

```bash
git@21371136:~/os/21371136/tools/readelf (lab1)$ ./readelf ~/os/21371136/target/mos
0:0x0
1:0x80010000
2:0x80012470
3:0x80012488
4:0x800124a0
5:0x0
6:0x0
7:0x0
8:0x0
9:0x0
10:0x0
11:0x0
12:0x0
13:0x0
14:0x0
15:0x0
16:0x0
```

### (2) 自己编写的readelf与正牌readelf的异同

使用`readelf -h`分别解析自己编写的`readelf`和`hello`:

```bash
git@21371136:~/os/21371136/tools/readelf (lab1)$ readelf -h readelf
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN (Position-Independent Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x1180
  Start of program headers:          64 (bytes into file)
  Start of section headers:          14488 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         13
  Size of section headers:           64 (bytes)
  Number of section headers:         31
  Section header string table index: 30
git@21371136:~/os/21371136/tools/readelf (lab1)$ readelf -h hello
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 03 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - GNU
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x8049600
  Start of program headers:          52 (bytes into file)
  Start of section headers:          746256 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         8
  Size of section headers:           40 (bytes)
  Number of section headers:         35
  Section header string table index: 34
```

可以看出`readelf`和`hello`的OS/ABI、Type和Machine不同. 通过`Makefile`可以看出编译`hello`时加入了`-m32 -static -g`参数, 这会生成特定的文件结构. 

## Thinking 1.3

> ​	操作系统的启动是一个非常复杂的过程。不过，由于 MOS 操作系统的目标是在 GXemul仿真器上运行，这个过程被大大简化了。GXemul 仿真器支持直接加载 ELF 格式的内核，也就是说，GXemul 已经提供了 bootloader 的引导（启动）功能。MOS 操作系统不需要再实现bootloader 的功能。在 MOS 操作系统的运行第一行代码前，我们就已经拥有一个正常的程序运行环境，内存和一些外围设备都可以正常使用。
>
> ​	GXemul 支持加载 ELF 格式内核，所以启动流程被简化为加载内核到内存，之后跳转到内核的入口，启动就完成了。这里要注意，之所以简单还有一个原因就在于 GXemul 本身是仿真器，是一种模拟硬件的软件而不是真正的硬件，所以就不需要面对传统的 bootloader 面对的那种非常纠结的情况了。

# 难点分析

这次的难点主要是实现printk。学会阅读课程组提供的代码，领会使用c语言实现库函数的方法和思维。

# 实验体会

在这次实验中，我了解了 MIPS 体系结构的基本组成和特点，并通过实验代码的填写，深入了解了操作系统启动的基本流程，从而能够更好地理解操作系统内部的工作机制。我还学习了 ELF 文件的结构和功能，这对于我今后的软件开发和调试工作都非常重要。

在这次实验中，我还完成了 printk 函数的编写。这个过程中，我深入理解了操作系统启动的基本流程，同时也学会了如何在 MIPS 体系结构下编写操作系统代码。这对于我今后在嵌入式开发和底层软件开发方面都非常有帮助。

总的来说，这次实验让我从操作系统角度深入理解了 MIPS 体系结构，掌握了操作系统启动的基本流程，了解了 ELF 文件的结构和功能，同时也学会了在 MIPS 体系结构下编写操作系统代码。这些技能和知识对我今后的学习和工作都将有很大的帮助。