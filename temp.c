
int make_shared(void *va) {
        int r;
            u_int perm = (*vpt)[VPN(va)] & 0xfff;
                if (va >= (void *) UTOP)
                        return -1;

                            if ((perm & PTE_V) == 0 || ((*vpd)[PDX(va)] & PTE_V) == 0) {
                                        if ((r = syscall_mem_alloc(syscall_getenvid(), ROUNDDOWN(va, BY2PG), PTE_V | PTE_R | PTE_LIBRARY)) < 0)
                                                    return -1;
                                                            return ROUNDDOWN((*vpt)[VPN(va)] & 0xfffff000, BY2PG);
                            }

                                if ((perm & PTE_R) == 0) {
                                            return -1;
                                }

                                    (*vpt)[VPN(va)] |= PTE_LIBRARY;
                                        return ROUNDDOWN((*vpt)[VPN(va)] & 0xfffff000, BY2PG);
}
                                }
                            }
}
