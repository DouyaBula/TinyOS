#include <lib.h>

int main() {
	u_int s, us;
	s = get_time(&us);
	debugf("%d:%d\n", s, us);
    usleep(10000);
    s = get_time(&us);
    debugf("%d:%d\n", s, us);
	return 0;
}
