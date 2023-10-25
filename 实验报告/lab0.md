**目录**
[TOC]
---

# 思考题

## Thinking 0.1

前后三次```git status```的部分内容如下(省略了其他三个txt文件):

Untracked.txt:

```bash
未跟踪的文件:
  （使用 "git add <文件>..." 以包含要提交的内容）
        README.txt

提交为空，但是存在尚未跟踪的文件（使用 "git add" 建立跟踪）
```

Stage.txt:

```bash
要提交的变更：
  （使用 "git rm --cached <文件>..." 以取消暂存）
        新文件：   README.txt

未跟踪的文件:
  （使用 "git add <文件>..." 以包含要提交的内容）
```

Modified.txt:

```bash
位于分支 master
尚未暂存以备提交的变更：
  （使用 "git add <文件>..." 更新要提交的内容）
  （使用 "git restore <文件>..." 丢弃工作区的改动）
        修改：     README.txt

未跟踪的文件:
  （使用 "git add <文件>..." 以包含要提交的内容）

修改尚未加入提交（使用 "git add" 和/或 "git commit -a"）
```

```README```文件刚开始未跟踪, 在使用```git add README```之后被加入跟踪, 在使用```git commit```后被加入暂存区, 在修改内容后被标记为变更未暂存.



## Thinking 0.2

分别对应```git add```, ```git add ```和```git commit```.



## Thinking 0.3

1. ```git checkout -- print.c```

2. ```git reset HEAD print.c```
   ```git checkout -- print.c```
   
3. ``` git rm --cached print.c```



## Thinking 0.4

三次提交后日志内容如下:

```bash
commit e233546bd30a6b9303fc0695f085f3ca3cf14a60 (HEAD -> master)
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:29:05 2023 +0800

    3

commit 05d909b45fa2ac7950e3091eff9d674edd170bd5
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:48 2023 +0800

    2

commit c302e5a43875585baf7d3bda29322510ba99cf6d
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:15 2023 +0800

    1
```

执行```git reset --hard HEAD^```后:

```bash
commit 05d909b45fa2ac7950e3091eff9d674edd170bd5 (HEAD -> master)
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:48 2023 +0800

    2

commit c302e5a43875585baf7d3bda29322510ba99cf6d
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:15 2023 +0800

    1
```

执行```git reset --hard c302e5a43875585baf7d3bda29322510ba99cf6d```后:

```bash
commit c302e5a43875585baf7d3bda29322510ba99cf6d (HEAD -> master)
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:15 2023 +0800

    1
```

执行```git reset --hard e233546bd30a6b9303fc0695f085f3ca3cf14a60```后:

```bash
commit e233546bd30a6b9303fc0695f085f3ca3cf14a60 (HEAD -> master)
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:29:05 2023 +0800

    3

commit 05d909b45fa2ac7950e3091eff9d674edd170bd5
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:48 2023 +0800

    2

commit c302e5a43875585baf7d3bda29322510ba99cf6d
Author: Doxel <douyashadow@163.com>
Date:   Sun Mar 5 15:28:15 2023 +0800

    1
```

由此可见在git中可以使用```HEAD```和哈希码对版本进行回退。



## Thinking 0.5

执行结果如下:

```bash
git@21371136:~/os/thinkings/lab0 $ echo first
first
git@21371136:~/os/thinkings/lab0 $ echo second > output.txt
git@21371136:~/os/thinkings/lab0 $ echo third > output.txt
git@21371136:~/os/thinkings/lab0 $ echo forth >> output.txt 
git@21371136:~/os/thinkings/lab0 $ cat output.txt 
third
forth
```



## Thinking 0.6

```command```文件内容如下:

```bash
#!/bin/bash 

echo 'echo Shell Start...' > test.sh
echo 'echo set a = 1' >> test.sh
echo 'a=1' >> test.sh
echo 'echo set b = 2' >> test.sh
echo 'b=2' >> test.sh
echo 'echo set c = a+b' >> test.sh
echo 'c=$[$a+$b]' >> test.sh
echo 'echo save c to ./file1' >> test.sh
echo 'echo $c>file1' >> test.sh
echo 'echo save b to ./file2' >> test.sh
echo 'echo $b>file2' >> test.sh
echo 'echo save a to ./file3' >> test.sh
echo 'echo $a>file3' >> test.sh
echo 'echo save file1 file2 file3 to file4' >> test.sh
echo 'cat file1>file4' >> test.sh
echo 'cat file2>>file4' >> test.sh
echo 'cat file3>>file4' >> test.sh
echo 'echo save file4 to ./result' >> test.sh
echo 'cat file4>>result' >> test.sh
```

```result```文件内容如下:

```bash
3
2
1
```

```test.sh```脚本通过三个变量分别把1, 2, 3存入到了三个文件中, 最后又把这三个文件的内容存到了```result```中.

```echo echo Shell Start```与```echo 'echo Shell Start'```效果相同. 

```echo echo $c>file1```与```echo 'echo $c>file1'```效果不同. 前者将"echo \$c"写入了```file1```, 后者将"echo \$c > file1"输出到屏幕.



# 难点分析

1. git基础知识的理解与使用. 理解git里的概念, 比如工作区, 暂存区和远程仓库的关系; 使用简单的git命令, 比如```git add```, ```git commit```, ```git push```等.
2. Linux系统和命令行界面的使用. 熟悉Linux系统的文件结构, 包管理方式等; 不执拗于图形界面的使用逻辑, 养成在使用命令行界面的思维与习惯.



# 实验体会

在这节操作系统实验课中，我学习了如何认识实验环境并熟悉 Linux 操作系统，掌握控制终端和一些常用工具。这些技能是进行操作系统实验必不可少的基础。

通过本次实验，我对 Linux 操作系统有了更深入的了解。我学会了如何使用命令行界面进行文件操作、网络操作、进程管理等。这对我今后的工作学习都会有很大的帮助，尤其是在需要处理大量数据和进行复杂任务的情况下，命令行操作可以提高效率和减少出错的可能性。

除了 Linux 操作系统，我们还学习了控制终端和常用工具，例如 Vim、Git、Make 等。这些工具可以帮助我们更好地完成实验任务和开展项目工作。例如，使用 Vim 可以快速编辑代码，使用 Git 可以方便地管理代码版本，使用 Make 可以自动化编译和构建过程。

总的来说，这节操作系统实验课对我来说是非常有益的。它让我了解了操作系统实验所需的基本工具，并提供了一个良好的学习环境，让我能够更好地完成后续的实验任务。我相信这些技能和知识对我今后的学习和工作都会有很大的帮助。