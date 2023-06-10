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

int main(int argc, char **argv) {
	multiSigTest();
    // _debug();
	return 0;
}

#define TEST_NUM 2
#define TEST_NUM2 3
void _debug() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, TEST_NUM);
    int boolean = sigismember(&set, TEST_NUM);
    debugf("TEST_NUM is in set: %d\n", boolean);
	panic_on(sigprocmask(0, &set, NULL));
    debugf("TEST_NUM is in set: %d\n", boolean);
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
	kill(0, SIGTERM);
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

void sigsegvTest() {
	char *low = 0x3F0000;
	*low = 'a';
}