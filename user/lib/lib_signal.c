#include <lib.h>

// 输出信号信息
void print_sig_info(int signum) {
	debugf("🔴received special signal: ");
	switch (signum) {
	case SIGKILL:
		debugf("SIGKILL.\n");
		break;
	case SIGSEGV:
		debugf("SIGSEGV.\n");
		break;
	case SIGTERM:
		debugf("SIGTERM.\n");
		break;
	default:
		break;
	}
}

// 执行信号
static void __attribute__((noreturn))
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

	try(syscall_set_sig_is_handling(0, 0));
	int r = syscall_set_trapframe(0, tf);
	user_panic("sighand_entry returned %d", r);
}

// 发送信号
int kill(u_int envid, int sig) {
	if (sig == SIGKILL || sig == SIGSEGV) {
		debugf("ONLY OS can send SIGKILL or SIGSEGV signal.\n");
		return 0;
	}

	extern volatile struct Env *env;
	if (env->env_user_tlb_mod_entry != (u_int)sighand_entry) {
		try(syscall_set_sighand_entry(0, sighand_entry));
	}
	try(syscall_set_sighand_entry(envid, sighand_entry));
	return syscall_sendsig(envid, sig);
}

// 注册信号的处理函数
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	if (env->env_user_tlb_mod_entry != (u_int)sighand_entry) {
		try(syscall_set_sighand_entry(0, sighand_entry));
	}
	return syscall_sigaction(signum, act, oldact);
}

// 修改进程的信号掩码
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
	if (env->env_user_tlb_mod_entry != (u_int)sighand_entry) {
		try(syscall_set_sighand_entry(0, sighand_entry));
	}
	return syscall_sigprocmask(how, set, oldset);
}

// 清空信号集，即将所有位都设置为 0
void sigemptyset(sigset_t *set) {
	for (u_int i = 0; i < SIG_WORDS; i++) {
		set->sig[i] = 0;
	}
}

// 设置信号集，即将所有位都设置为 1
void sigfillset(sigset_t *set) {
	for (u_int i = 0; i < SIG_WORDS; i++) {
		set->sig[i] = 0xFFFF;
	}
}

// 向信号集中添加一个信号，即将指定信号的位设置为 1
void sigaddset(sigset_t *set, int signum) {
	signum -= 1;
	set->sig[signum / SIG_B2W] |= 1 << (signum % SIG_B2W);
}

// 从信号集中删除一个信号，即将指定信号的位设置为 0
void sigdelset(sigset_t *set, int signum) {
	signum -= 1;
	set->sig[signum / SIG_B2W] &= ~(1 << (signum % SIG_B2W));
}

// 检查一个信号是否在信号集中，如果在则返回 1，否则返回 0
int sigismember(const sigset_t *set, int signum) {
	signum -= 1;
	return 1 & (set->sig[signum / SIG_B2W] >> (signum % SIG_B2W));
}
