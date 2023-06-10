#include <lib.h>

int global = 0;
void handler(int num) {
	debugf("😄Reach handler, now the signum is %d!\n", num);
	global = 1;
}

void handler2(int num) {
	debugf("😊Reach handler2, now the signum is %d!\n", num);
	global = 2;
}

int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int *test = NULL;
void sgv_handler(int num) {
    debugf("Segment fault appear!\n");
    test = &a[0];
    debugf("test = %d.\n", *test);
    // exit();
}

int main(int argc, char **argv) {
	sigsegvTest();
	return 0;
}

void sigsegvTest() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGSEGV);
    struct sigaction sig;
    sig.sa_handler = sgv_handler;
    sig.sa_mask = set;
    debugf("user-handler: %x\n", sig.sa_handler);
    panic_on(sigaction(11, &sig, NULL));
    *test = 10;
    debugf("test = %d.\n", *test);
    return 0;
}


#define TEST_NUM 2
#define TEST_NUM2 3
void _debug() {
    sigset_t set;
    sigset_t oldset;
    sigemptyset(&set);
    sigaddset(&set, TEST_NUM);
    int boolean = sigismember(&set, TEST_NUM);
	panic_on(sigprocmask(0, &set, &oldset));
}


void multiSigTest() {
	sigset_t set;
	sigemptyset(&set);
	struct sigaction sig;
	sig.sa_handler = handler;
	sig.sa_mask = set;
	panic_on(sigaction(TEST_NUM, &sig, NULL));
	sig.sa_handler = handler2;
	panic_on(sigaction(TEST_NUM2, &sig, NULL));

	sigaddset(&set, TEST_NUM);
	sigaddset(&set, TEST_NUM2);
	sigaddset(&set, SIGKILL);
	panic_on(sigprocmask(0, &set, NULL));
	kill(0, TEST_NUM);
	kill(0, TEST_NUM2);
    panic_on(sigprocmask(1, &set, NULL));
	kill(0, SIGKILL);
	kill(0, 5);
	int ans = 0;
	for (int i = 0; i < 10000000; i++) {
		ans += i;
	}
	ans = 0;
	for (int i = 0; i < 10000000; i++) {
		ans += i;
	}
	debugf("global = %d.\n", global);
}

