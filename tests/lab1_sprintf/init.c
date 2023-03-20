void sprintf_1_check() {
	char str[100];
    sprintf(str, "%d\n", 6543210);
    printk("%s", str);
    sprintf(str, "%c\n", 97);
    printk("%s", str);
}

void mips_init() {
	sprintf_1_check();
	halt();
}
