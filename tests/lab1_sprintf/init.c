void sprintf_1_check() {
	char str[100];
    int n;
    n = sprintf(str, "");
    printk("n:%d\n",n);
    n = sprintf(str, "%d and %d\n", 12345, 6789);
    printk("n:%d\t%s", n, str);
    n = sprintf(str, "%d\n", 1234578);
    printk("n:%d\t%s", n, str);
    n = sprintf(str, "%c\n", 97);
    printk("n:%d\t%s", n, str);
}

void mips_init() {
	sprintf_1_check();
	halt();
}
