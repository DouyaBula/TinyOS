#include <blib.h>

size_t strlen(const char *s) {
	size_t n = 0;
	while(*s) {
		n++;
		s++;
	}
	return n;
}

char *strcpy(char *dst, const char *src) {
	char *res = dst;
	while(*src){
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	while(*dst){
		dst++;
	}
	while(*src){
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
	return res;
}

int strcmp(const char *s1, const char *s2) {
	while(*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char*) s1 - *(const unsigned char*) s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	unsigned char *p = (unsigned char*) s;
	while(n--) {
		*p++ = (unsigned char) c;
	}
	return s;
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	int reval = 0;
	while(n && !(reval = (*(unsigned char *)s1) - (*(unsigned char *)s2))) {
		s1 = (unsigned char *)s1 + 1;
		s2 = (unsigned char *)s2 + 1;
		n--;
	}
	return reval;
}
