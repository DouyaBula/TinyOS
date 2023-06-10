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
