// User-level IPC library routines

#include <drivers/dev_rtc.h>
#include <env.h>
#include <lib.h>
#include <mmu.h>

// lab5-1 exam
u_int get_time(u_int *us) {
    u_int temp = 0;
    panic_on(syscall_write_dev(&temp, DEV_RTC_ADDRESS + DEV_RTC_TRIGGER_READ, sizeof(temp)));
    panic_on(syscall_read_dev(&temp, DEV_RTC_ADDRESS + DEV_RTC_SEC, sizeof(temp)));
    panic_on(syscall_read_dev(us, DEV_RTC_ADDRESS + DEV_RTC_USEC, sizeof(temp)));
    return temp;
}

void usleep(u_int us) {
	// 读取进程进入 usleep 函数的时间
    u_int enter_us = 0;
	u_int enter_s = get_time(&enter_us);
    u_int limit_s = us / 1000000;
    u_int limit_us = us - limit_s;
    u_int add_s = (enter_us + limit_us) / 1000000;
    u_int left_us = (enter_us + limit_us) - add_s;
    u_int top_s = enter_s + limit_s + add_s;
    u_int top_us = left_us;
    // debugf("enter_s: %d\tenter_us: %d\n", enter_s, enter_us);
    while (1) {
		// 读取当前时间
        u_int now_us = 0;
        u_int now_s = get_time(&now_us);
		if (now_s > top_s) {
			return;
		}
        if ((now_s == top_s) && (now_us >= top_us)) {
            //debugf("%d -- %d\n", now_us, enter_us+us);
            return;
        }
        syscall_yield();
	}
}
// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
