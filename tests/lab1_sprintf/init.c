void sprintf_1_check() {
	char str[100];
    int n;
    n = sprintf(str, "%s%d%c", "abc", 123, 'h');
    printk("n:%d\t%s\n", n, str);
    n = sprintf(str, "%d&%d\n", 12, 34);
    printk("n:%d\t%s", n, str);
    n = sprintf(str, "%d\n", 1234);
    printk("n:%d\t%s", n, str);
    n = sprintf(str, "%c\n", 97);
    printk("n:%d\t%s", n, str);
}

void mips_init() {
	sprintf_1_check();
	halt();
}
