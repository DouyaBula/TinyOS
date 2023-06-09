#ifndef _SIG_LIB_H
#define _SIG_LIB_H

// lab4-challenge
#include <types.h>
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

void sigemptyset(sigset_t *set);
void sigfillset(sigset_t *set);
void sigaddset(sigset_t *set, int signum);
void sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);

int kill(u_int envid, int sig);

int syscall_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int syscall_sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int syscall_sendsig(u_int envid, int sig);
int syscall_set_sighand_entry(u_int envid, u_int func);

#endif