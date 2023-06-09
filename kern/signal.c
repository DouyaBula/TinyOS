#include <signal.h>

// 向信号集中添加一个信号，即将指定信号的位设置为 1
void _sigaddset(sigset_t *set, int signum) {
	signum -= 1;
	set->sig[signum / SIG_B2W] |= 1 << (signum % SIG_B2W);
}

// 从信号集中删除一个信号，即将指定信号的位设置为 0
void _sigdelset(sigset_t *set, int signum) {
	signum -= 1;
	set->sig[signum / SIG_B2W] &= ~(1 << (signum % SIG_B2W));
}

// 检查一个信号是否在信号集中，如果在则返回 1，否则返回 0
int _sigismember(const sigset_t *set, int signum) {
	signum -= 1;
	return 1 & (set->sig[signum / SIG_B2W] >> (signum % SIG_B2W));
}