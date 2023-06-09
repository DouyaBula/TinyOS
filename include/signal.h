#ifndef _SIG_H
#define _SIG_H

#include <queue.h>
#include <types.h>

// 信号掩码控制宏 (how)
#define SIG_BLOCK (0)	// 将 set 参数中指定的信号添加到当前进程的信号掩码中
#define SIG_UNBLOCK (1) // 将 set 参数中指定的信号从当前进程的信号掩码中删除
#define SIG_SETMASK (2) // 将当前进程的信号掩码设置为 set 参数中指定的信号集

// 信号编号
#define SIGKILL (9)
#define SIGSEGV (11)
#define SIGTERM (15)

// 信号数量
#define SIG_MAX (64)
#define SIG_B2W (8 * sizeof(int))
#define SIG_WORDS (SIG_MAX / SIG_B2W)
#define SIG_BUFFER (64)

// 掩码结构体
typedef struct sigset_t {
	int sig[SIG_WORDS];
} sigset_t;

// 信号描述符
struct signal {
	TAILQ_ENTRY(signal) sig_link;
	int signum;
};

// 信号挂起队列
TAILQ_HEAD(Sig_pending, signal);

// 信号特性描述符
struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
};

typedef void (*sa_handler)(int);

// 信号处理程序(handler)描述符
struct sighand_struct {
	struct sigaction action[SIG_MAX];
};

// 用于sighand初始化
static struct sighand_struct init_sighand = {
    .action = {0},
};

// 用于sigset初始化
static sigset_t init_sigset = {0};

void _sigaddset(sigset_t *set, int signum);
void _sigdelset(sigset_t *set, int signum);
int _sigismember(const sigset_t *set, int signum);

#endif