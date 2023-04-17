#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <queue.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
    static int user_time[5];
    
    int user_ok[5];
    for (int k = 0; k < 5; k++) {
        user_ok[k] = 0;
    }
    struct Env *temp;
    TAILQ_FOREACH(temp, &env_sched_list, env_sched_link) {
        user_ok[temp->env_user] = 1;
    }
    
    // #define DEBUG 0
    if (yield != 0 || count == 0 || e == NULL || e->env_status != ENV_RUNNABLE) {
        if (e != NULL && e->env_status == ENV_RUNNABLE) {
            TAILQ_REMOVE(&env_sched_list, e, env_sched_link);
            TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);
            user_time[e->env_user] += e->env_pri;
        }

        if (TAILQ_EMPTY(&env_sched_list)) {
            panic("schedule: no runnable envs");
        }
        
        int minUser = -1;
        for (int t = 0; t < 5; t++) {
            if (user_ok[t]) {
                if (minUser == -1) {
                    minUser = t;
                    continue;
                }
                if (user_time[t] < user_time[minUser]) {
                    minUser = t;
                }
            }
        }

        TAILQ_FOREACH(e, &env_sched_list, env_sched_link) {
            if (e->env_user == minUser) {
                break;
            }
        }

        count = e->env_pri;
    }
    count--;
    env_run(e);
}
