# Lab4-challenge 实验报告

[TOC]

---

## I. 实现思路

### 流程图示

下图左侧展示了信号的注册与信号的发送，右侧展示了信号的处理。

![overview](lab4-challenge 实验报告/overview.svg)

### 数据结构

为了实现信号系统，我定义了一系列的数据结构用来描述信号、储存信号、组织信号。

下面是各类数据数据的代码，它们的作用在代码注释中给出。

#### 1. sigset与sigaction

```c
// 掩码结构体
typedef struct sigset_t {
	int sig[SIG_WORDS];
} sigset_t;

// 用于sigset初始化
static sigset_t init_sigset = {0};

// 信号特性描述符
struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
};
```

#### 2. 信号描述符

```c
// 信号描述符
struct signal {
	TAILQ_ENTRY(signal) sig_link;
	int signum;
};
```

#### 3. 信号挂起队列

```c
// 信号挂起队列
TAILQ_HEAD(Sig_pending, signal);
```

#### 4. 信号处理程序描述符

```c
typedef void (*sa_handler)(int);
// 信号处理程序(handler)描述符
struct sighand_struct {
	struct sigaction action[SIG_MAX];
};

// 用于sighand初始化
static struct sighand_struct init_sighand = {
    .action = {0},
};
```

#### 5. 修改Env结构体

```c
struct Env {
	...
	// lab4-challenge
	u_int env_user_sighand_entry;	// 用户态信号处理函数入口
	struct sighand_struct sighand; // 信号处理信息, 即64个信号的action
	sigset_t blocked;	       // 全局掩码
	struct Sig_pending sig_pending;	// 未处理信号队列
	u_int sig_pending_cnt;	// 未处理信号的数量
	u_int sig_is_handling; // 是否正在处理信号
};
```

### 用于修改sigset的函数

按照题目要求，为了方便用户程序修改储存信号掩码的数据结构sigset，我通过位运算实现了以下函数。

下面是各个函数的代码，它们的作用在代码注释中给出。

#### 1. 清空信号集

```c
// 清空信号集，即将所有位都设置为 0
void sigemptyset(sigset_t *set) {
	for (u_int i = 0; i < SIG_WORDS; i++) {
		set->sig[i] = 0;
	}
}
```

#### 2. 设置信号集

```C
// 设置信号集，即将所有位都设置为 1
void sigfillset(sigset_t *set) {
	for (u_int i = 0; i < SIG_WORDS; i++) {
		set->sig[i] = 0xFFFF;
	}
}
```

#### 3. 向信号集添加信号


```C
// 向信号集中添加一个信号，即将指定信号的位设置为 1
void sigaddset(sigset_t *set, int signum) {
	signum -= 1;
	set->sig[signum / SIG_B2W] |= 1 << (signum % SIG_B2W);
}
```

#### 4. 从信号集删除信号

```c
// 从信号集中删除一个信号，即将指定信号的位设置为 0
void sigdelset(sigset_t *set, int signum) {
	signum -= 1;
	set->sig[signum / SIG_B2W] &= ~(1 << (signum % SIG_B2W));
}
```

#### 5. 检查信号是否在信号集中

```c
// 检查一个信号是否在信号集中，如果在则返回 1，否则返回 0
int sigismember(const sigset_t *set, int signum) {
	signum -= 1;
	return 1 & (set->sig[signum / SIG_B2W] >> (signum % SIG_B2W));
}
```

### 系统调用

为了在用户态向进程注册信号处理函数，以及修改进程的信号掩码，我考虑新增系统调用在内核态修改Env结构体以达到相应的目的。

下面是各个系统调用对应的内核代码，它们的作用在代码注释中给出。

#### 1. 注册信号的处理函数

该函数对应题目要求中用户态的`sigaction`函数。

```c
// 注册信号的处理函数
// 其中 signum 表示需要处理信号的编号，act 表示新的信号处理结构体，
// 旧的信号处理结构体则需要在 oldact != NULL 时保存该指针在对应的地址空间中。
// 当收到编号大于64的信号时直接返回异常码 -1。
int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	struct Env *e;
	struct sigaction *sig;

	if (signum < 1 || signum > SIG_MAX) {
		return -E_SIG;
	}

	e = curenv;
	sig = &e->sighand.action[signum - 1];
	if (oldact) {
		*oldact = *sig;
	}
	if (act) {
		*sig = *act;
	}
	return 0;
}
```

#### 2. 修改进程的信号掩码

该函数对应题目要求中用户态的`sigprocmask`函数。

```c
// 修改进程的信号掩码
// 其中 how 表明对于信号掩码的修改类型方式，包括下面三种方式：
// SIG_BLOCK（how 为 0）: 将 set 参数中指定的信号添加到当前进程的信号掩码中
// SIG_UNBLOCK（how 为 1）: 将 set 参数中指定的信号从当前进程的信号掩码中删除
// SIG_SETMASK（how 为 2）: 将当前进程的信号掩码设置为 set 参数中指定的信号集
// 当 oldset 不为 NULL 时，还需将原有的信号掩码放在 oldset 指定的地址空间中。
// 正常执行则返回 0，否则返回异常码 -1.
int sys_sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
	struct Env *e;
	sigset_t *sigset;
	e = curenv;
	sigset = &e->blocked;
	if (oldset) {
		*oldset = *sigset;
	}
	if (set) {
		switch (how) {
		case SIG_BLOCK:
			for (u_int i = 0; i < SIG_WORDS; i++) {
				sigset->sig[i] |= set->sig[i];
			}
			break;
		case SIG_UNBLOCK:
			for (u_int i = 0; i < SIG_WORDS; i++) {
				sigset->sig[i] &= ~set->sig[i];
			}
			break;
		case SIG_SETMASK:
			for (u_int i = 0; i < SIG_WORDS; i++) {
				sigset->sig[i] = set->sig[i];
			}
			break;
		default:
			return -E_SIG;
		}
	}
	return 0;
}
```

#### 3. 向特定进程发送信号

该函数对应题目要求中用户态的`kill`函数。

```c
// 向特定进程发送信号
// 向进程控制号编号为 envid 的进程发送 sig 信号，
// 注意当 envid = 0 时代表向自身发送信号。
// 该函数成功完成返回 0，如果信号编号超过限制或者进程编号不存在则返回 -1。
extern int sigsCnt;
extern struct signal sigs[];
int sys_sendsig(u_int envid, int sig) {
	struct Env *e;
	if (sig < 1 || sig > SIG_MAX || envid2env(envid, &e, 1) != 0) {
		return -E_SIG;
	};
	sigs[sigsCnt].signum = sig;
	TAILQ_INSERT_HEAD(&e->sig_pending, &sigs[sigsCnt], sig_link);
	sigsCnt = (sigsCnt + 1) % SIG_BUFFER;
	e->sig_pending_cnt++;
	return 0;
}

```

#### 4. 设置用户态信号处理函数入口

```c
// 设置用户态信号处理函数入口
int sys_set_sighand_entry(u_int envid, u_int func) {
	struct Env *env;
	try(envid2env(envid, &env, 1));
	env->env_user_sighand_entry = func;
	return 0;
}
```

#### 5. 标记进程是否正在处理信号

```c
// 标记进程是否正在处理信号
int sys_set_sig_is_handling(u_int envid, int is_handling) {
	struct Env *env;
	try(envid2env(envid, &env, 1));
	env->sig_is_handling = is_handling;
	return 0;
}
```

### 用户态下的信号处理入口`sighand_entry`

仿照`cow_entry`，我在用户态下实现了`sighand_entry`。在内核态修改进程上下文后，会返回到用户态处理信号，此时将通过此函数跳转至用户注册的处理函数。用户注册的处理函数执行完毕后，还会再通过此函数返回内核态，恢复进程的上下文。

下面是函数的代码，它的作用在代码注释中给出。

```c
void __attribute__((noreturn))
sighand_entry(struct Trapframe *tf, int signum, sa_handler handler) {
	// 如果产生了SIGSEGV但是进程使用掩码忽略, 那么直接panic.
	if (signum == SIGSEGV && sigismember(&env->blocked, signum)) {
		user_panic("SIGSEGV is blocked!\n");
	}

	if (signum == SIGKILL || signum == SIGSEGV || signum == SIGTERM) {
		print_sig_info(signum);
		if (!handler) {
			exit();
		}
	}

	if (handler) {
		handler(signum);
	} else {
		debugf("signum %d is ignored.\n", signum);
	}

	// 如果产生了SIGSEGV但是进程注册的函数没有处理, 那么直接panic.
	if (signum == SIGSEGV) {
		user_panic("SIGSEGV needs to be handled!\n");
	}

	panic_on(syscall_set_sig_is_handling(0, 0));
	int r = syscall_set_trapframe(0, tf);
	user_panic("sighand_entry returned %d", r);
}
```

### 规避内核态修改用户态时发生写时复制

对于可能修改用户态内容的系统调用，我在进入内核态之前先使用`memset`触发了一次写时复制，这样进入内核态后就不会产生写时复制问题。

```c
int syscall_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	if (oldact) {
		memset(oldact, 0, sizeof(oldact));
	}
	return msyscall(SYS_sigaction, signum, act, oldact);
}

int syscall_sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
	if (oldset) {
		memset(oldset, 0, sizeof(oldset));
	}
	return msyscall(SYS_sigprocmask, how, set, oldset);
}

```



### 修改`sys_exofork`以继承信号信息

为了使子进程继承必要的信号信息，我在`sys_exofork`中对子进程的`Env`结构体进行了必要的赋值。

```c
int sys_exofork(void) {
	...
	// lab4-challenge
	// 继承信号处理信息和全局掩码, 以及信号处理入口
	e->sighand = curenv->sighand;
	e->blocked = curenv->blocked;
	e->env_user_sighand_entry = curenv->env_user_sighand_entry;

	return e->env_id;
}
```



### 进程上下文的修改

#### 1. 进程被调度前的修改

在函数`env_run`的最后，我加入了一行代码以检查进程是否需要处理信号，如果需要，则修改进程的上下文以进入信号处理函数。

##### 修改`env_run`

```c
void env_run(struct Env *e) {
	...
	// lab4-1 challenge
	if (!e->sig_is_handling && curenv->sig_pending_cnt) {
		e->sig_is_handling = 1;
		do_signal(&curenv->env_tf);
	}
	env_pop_tf(&curenv->env_tf, curenv->env_asid);
}
```

##### 添加`do_signal`

```c
// lab4-challenge
void do_signal(struct Trapframe *tf) {
	struct signal *s = NULL;
	// 从进程的队列中取出未被阻塞的signal
	int t;
	TAILQ_FOREACH (s, &curenv->sig_pending, sig_link) {
		if ((!_sigismember(&curenv->blocked, s->signum) &&
		     !_sigismember(&curenv->sighand.action->sa_mask, s->signum)) ||
		    s->signum == SIGKILL) {
			t = s->signum;
			break;
		}
	}
	// 若存在未被阻塞的signal, 则修改进程的上下文, 进入用户态的handle_signal处理信号
	if (s) {
		curenv->sig_pending_cnt--;
		TAILQ_REMOVE(&curenv->sig_pending, s, sig_link);
		sig_setuptf(tf, t);
	} else { // 若signal全被阻塞, 当作无事发生
		curenv->sig_is_handling = 0;
	}
}

void sig_setuptf(struct Trapframe *tf, int signum) {
	struct Trapframe tmp_tf = *tf;

	if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
		tf->regs[29] = UXSTACKTOP;
	}
	tf->regs[29] -= sizeof(struct Trapframe);
	*(struct Trapframe *)tf->regs[29] = tmp_tf;

	tf->regs[4] = tf->regs[29];
	tf->regs[5] = signum;
	tf->regs[6] = curenv->sighand.action[signum - 1].sa_handler;
	tf->regs[29] -= 3 * sizeof(tf->regs[4]);
	tf->cp0_epc = curenv->env_user_sighand_entry;
}
```

#### 2. 触发SIGSEGV的修改

当用户程序触发`SIGSEGV`时，同样需要修改用户上下文以进入处理函数。我修改了`do_tlb_refill`和`_do_tlb_refill`，以实现这个目的。

##### 修改`do_tlb_refill`

原有的`do_tlb_refill`函数丢弃了进程上下文作为参数，在这里我给补上了。

```assembly
NESTED(do_tlb_refill, 0, zero)
	move 	a2, a0		# lab4-challenge: we need to modify trapframe
	...
END(do_tlb_refill)
```

##### 修改`_do_tlb_refill`

```c
Pte _do_tlb_refill(u_long va, u_int asid, struct Trapframe *tf) {
	Pte *pte;
	// SIGSEGV 抢占式处理
	if (va < UTEMP) {
		printk("kernel: SIGSEGV triggered!\n");
		if (!curenv->env_user_sighand_entry) {
			env_destroy(curenv);
		}
		struct Trapframe tmp_tf = *tf;
		if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
			tf->regs[29] = UXSTACKTOP;
		}
		tf->regs[29] -= sizeof(struct Trapframe);
		*(struct Trapframe *)tf->regs[29] = tmp_tf;
		
		tf->regs[4] = tf->regs[29];
		tf->regs[5] = SIGSEGV;
		tf->regs[6] = curenv->sighand.action[SIGSEGV - 1].sa_handler;
		tf->regs[29] -= 3 * sizeof(tf->regs[4]);
		tf->cp0_epc = curenv->env_user_sighand_entry;
		env_pop_tf(tf, curenv->env_asid);
		panic("SIGSEGV returned!\n");
	}
    ...
}
```



## II. 测试

在`user/sigtest.c`中，我编写了多个测试函数。在`sigtest.c`中启用想要的测试函数后，使用`make && make run`命令即可进行测试。

### 测试程序`user/sigtest.c`

```c
#include <lib.h>
void handler(int num) {
	debugf("🤡Reach handler, now the signum is %d!\n", num);
}

int global = 0;
void handler1(int num) {
    debugf("😋Reach handler1, now the signum is %d!\n", num);
    global = 1;
}

void handler2(int num) {
	debugf("🥶Reach handler2, now the signum is %d!\n", num);
}

int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int *test = NULL;
void sgv_handler(int num) {
    debugf("Segment fault appear!\n");
    test = &a[0];
    debugf("test = %d.\n", *test);
    exit();
}

void test1();
void test2();
void test3();
void multSigTest();
void blockedSigTest();
void handBlockedSigTest();
void multProcTest();

int main(int argc, char **argv) {
    // test1(); // 测试点1: 基本信号测试
    // test2(); // 测试点2: 空指针测试(SIGSEGV测试)
    // test3(); // 测试点3: 写时复制测试
	// multSigTest();   // 多信号与SIGTERM测试
    // blockedSigTest();   // 进程掩码测试
    // handBlockedSigTest();   // 处理函数掩码测试
    // multProcTest();  // 跨进程发送信号测试
	return 0;
}

#define TEST_NUM 2
void test1(){
    sigset_t set;
    sigemptyset(&set);
    struct sigaction sig;
    sig.sa_handler = handler1;
    sig.sa_mask = set;
    panic_on(sigaction(TEST_NUM, &sig, NULL));
    sigaddset(&set, TEST_NUM);
    panic_on(sigprocmask(0, &set, NULL));
    kill(0, TEST_NUM);
    panic_on(sigprocmask(1, &set, NULL));
    int ans = 0;
    for (int i = 0; i < 10000000; i++) {
        ans += i;
    }
    debugf("global = %d.\n", global);
}

void test2(){
    sigset_t set;
    sigemptyset(&set);
    struct sigaction sig;
    sig.sa_handler = sgv_handler;
    sig.sa_mask = set;
    panic_on(sigaction(11, &sig, NULL));
    *test = 10;
    debugf("test = %d.\n", *test);
}

sigset_t set2;
void test3(){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 1);
    sigaddset(&set, 2);
    panic_on(sigprocmask(0, &set, NULL));
    sigdelset(&set, 2);
    int ret = fork();
    if (ret != 0) {
        panic_on(sigprocmask(0, &set2, &set));
        debugf("Father: %d.\n", sigismember(&set, 2));
    } else {
        debugf("Child: %d.\n", sigismember(&set, 2));
    }
}

void multSigTest(){
    sigset_t set;
	sigemptyset(&set);
	struct sigaction sig;
	sig.sa_handler = handler;
	sig.sa_mask = set;
	panic_on(sigaction(2, &sig, NULL));
	sig.sa_handler = handler2;
	panic_on(sigaction(3, &sig, NULL));
    panic_on(sigaction(5, &sig, NULL));

	sigaddset(&set, 2);
	sigaddset(&set, 3);
	panic_on(sigprocmask(0, &set, NULL));
	kill(0, 2);
	kill(0, 3);
    panic_on(sigprocmask(1, &set, NULL));
	kill(0, SIGTERM);
	kill(0, 5);
	int ans = 0;
	for (int i = 0; i < 10000000; i++) {
		ans += i;
	}
}

void blockedSigTest(){
    sigset_t set;
	sigemptyset(&set);
	struct sigaction sig;
	sig.sa_handler = handler;
	sig.sa_mask = set;
    panic_on(sigaction(1, &sig, NULL));
	panic_on(sigaction(2, &sig, NULL));
	panic_on(sigaction(3, &sig, NULL));
    sig.sa_handler = handler2;
	panic_on(sigaction(4, &sig, NULL));
    panic_on(sigaction(5, &sig, NULL));
    panic_on(sigaction(6, &sig, NULL));

    
	sigaddset(&set, 1);
	sigaddset(&set, 3);
	sigaddset(&set, 5);
	panic_on(sigprocmask(0, &set, NULL));
    kill(0, 1);
	kill(0, 2);
	kill(0, 3);
	kill(0, 4);
	kill(0, 5);
    kill(0, 6);
	int ans = 0;
	for (int i = 0; i < 10000000; i++) {
		ans += i;
	}
}

void handBlockedSigTest() {
    sigset_t set;
	sigemptyset(&set);
	struct sigaction sig;
    sigaddset(&set, 2);
	sigaddset(&set, 4);
	sigaddset(&set, 6);
	sig.sa_handler = handler;
	sig.sa_mask = set;
    panic_on(sigaction(1, &sig, NULL));
	panic_on(sigaction(2, &sig, NULL));
	panic_on(sigaction(3, &sig, NULL));
    sig.sa_handler = handler2;
	panic_on(sigaction(4, &sig, NULL));
    panic_on(sigaction(5, &sig, NULL));
    panic_on(sigaction(6, &sig, NULL));

    kill(0, 1);
	kill(0, 2);
	kill(0, 3);
	kill(0, 4);
	kill(0, 5);
    kill(0, 6);
	int ans = 0;
	for (int i = 0; i < 10000000; i++) {
		ans += i;
	}
}

void multProcTest() {
	sigset_t set;
	sigemptyset(&set);
	struct sigaction sig;
	sig.sa_handler = handler;
	sig.sa_mask = set;
	panic_on(sigaction(5, &sig, NULL));
	panic_on(sigaction(6, &sig, NULL));
    int ans = 0;
	int ret = fork();
	if (ret != 0) {
		kill(0, 5);
		kill(ret, 6);
		for (int i = 0; i < 10000000; i++) {
			ans += i;
		}
	} else {
		for (int i = 0; i < 10000000; i++) {
			ans += i;
		}
	}
}

```



### 期望输出

其中测试点1、2、3为教程网站给出的样例，其余为自行编写的测试点。

#### 测试点1

```bash
Reach handler, now the signum is 2!
global = 1.
```

#### 测试点2

```bash
Segment fault appear!
test = 1.
```

#### 测试点3

```bash
Father: 1.
Child: 0.
```

#### 多信号与SIGTERM测试

```c
🥶Reach handler2, now the signum is 5!
🔴received special signal: SIGTERM.
```



#### 进程掩码测试

```bash
🥶Reach handler2, now the signum is 6!
🥶Reach handler2, now the signum is 4!
🤡Reach handler, now the signum is 2!
```



#### 处理函数掩码测试

```bash
🥶Reach handler2, now the signum is 5!
🤡Reach handler, now the signum is 3!
🤡Reach handler, now the signum is 1!
```

#### 跨进程发送信号测试

```c
🤡Reach handler, now the signum is 6!
🤡Reach handler, now the signum is 5!
```

## III. 遇到的问题以及解决方案

### 一个十分诡异的bug：对于新建的结构体，其内容会莫名其妙的变化。

这个诡异的现象如下图所示。其中`s`是指向`struct signal`结构体的指针。

<img src="lab4-challenge 实验报告/image-20230612104040707.png" alt="image-20230612104040707" style="zoom:80%;" />

明明没有修改`s->signum`的值，为什么输出结果会发生变化呢？难道是`sigx`自减时修改了它的值？还是说`printk`修改了它的值？这都不可能啊。

单看这一段代码，我实在是摸不着头脑。



抓耳挠腮了很久，我终于找到了问题所在，在新建信号结构体时，我是在一个函数内部新建了一个`struct signal`结构体，再将链表里的指针指向这个结构体。代码如下：

```c
int sys_sendsig(u_int envid, int sig) {
	...
	struct signal s = {.signum = sig};
	TAILQ_INSERT_HEAD(&e->sig_pending, &s, sig_link);
	...
}

```

我在用java写oo作业时，经常会写出这样的业务逻辑，并且不会出现bug。这种写法很直观，很容易理解，就是新建一个东西再放到链表里嘛。

但是在系统内核里就不能这么写，这么写就会出bug。

问题就出在**"在一个函数内部新建结构体"**，这只是在栈上申请了一个局部变量，当这段空间释放后被重新使用时，这段空间原有的内容就会被覆盖。

为了解决这个问题，我联想到了`Env`结构体的组织方式：先使用全局变量申请固定的空间，这样结构体的内容就不会被更改了。每次需要申请信号结构体时，只需要取出一份来用就可以了。

```c
// lab4-challenge
struct signal sigs[SIG_BUFFER] __attribute__((aligned(BY2PG)));
```

解决这个bug的经历，是我这次挑战性任务收获最大的部分。

我深刻体会到，编写系统内核时，对内存的操作要极其敏感，要熟练掌握底层知识，并让自己的思维更加缜密。



### 已有的`do_tlb_refill`无法修改进程上下文，难以实现信号`SIGSEGV`.

通过阅读`tlb_asm.S`，我发现`do_tlb_refill`函数与其它的异常处理函数不同，它把保存上下文地址的`a0`寄存器给覆盖了。为此，我将上下文地址重新存入`a2`寄存器中，并为c语言撰写的`_do_tlb_refill`添加了第三个参数`struct Trapframe* tf`，从而能够在`_do_tlb_refill`中修改上下文。

```assembly
NESTED(do_tlb_refill, 0, zero)
	move 	a2, a0		# lab4-challenge: we need to modify trapframe
	mfc0    a0, CP0_BADVADDR
	mfc0    a1, CP0_ENTRYHI
	...
END(do_tlb_refill)
```

```c
Pte _do_tlb_refill(u_long va, u_int asid, struct Trapframe *tf){
    if (va < UTEMP) {
		// modify tf
	}
}
```

