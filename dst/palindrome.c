#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	int temp = n;
	int cmp = 0;
	while (temp > 0) {
		cmp = 10 * cmp + temp % 10;
		temp /= 10;
	}
	if (n == cmp) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
