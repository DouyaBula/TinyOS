/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

#define N (32)
#define X (0xFFFFFFFF)
int map[N];
int w[N];
int cnt[N];

int zero[N];
int buf[N];
// lab5-1 extra
void erase(int logicno);
int mapp(u_int logicno) {
    int secno = map[logicno];
    if(secno!=X){
        return secno;
    }
    int cntt, sel;
    for(int i=0;i<N;i++){
        if(w[i]){
            cntt=cnt[i];
            sel=i;
        }
    }
    for(int i=sel+1;i<N;i++){
        if(w[i]&&cnt[i]<cntt){
            cntt=cnt[i];
            sel=i;
        }
    }
    int flag=0;
    int sel2;
    int cntt2;
    if(cntt>=5){
        flag=1;
        for(int i=0;i<N;i++){
            if(!w[i]){
                sel2=i;
                cntt2=cnt[i];
            }
        }
        for(int i=sel2+1;i<N;i++){
            if(!w[i]&&cnt[i]<cntt){
                sel2=i;
                cntt=cnt[i];
            }
        }
        ide_read(0,sel2,buf,1);
        ide_write(0,sel,buf,1);
        w[sel]=0;
        for(int i=1;i<N;i++){
            if(map[i]==sel2){
                erase(i);
                map[i]=sel;
                break;
            }
        }
    }
    if (flag==1){
    map[logicno]=sel2;
    }else{
        map[logicno]=sel;
    }
    return map[logicno];
}

void erase(int logicno){
    int secno = map[logicno];
    cnt[secno]++;
    w[secno]=1;
    ide_write(0, secno, zero, 1);
    map[logicno]=X;
}
void ssd_init() {
    for(int i=0;i<N;i++){
        map[i]=X;
        w[i]=1;
        cnt[i]=0;
        zero[i]=0;
    }
}

int ssd_read(u_int logicno, void *dst){
    int secno = map[logicno];
    if(secno == X){
        return -1;
    }
    ide_read(0, secno, dst, 1);
    return 0;
}

void ssd_write(u_int logicno, void *src) {
    int secno = map[logicno];
    if(secno != X){
        erase(logicno);
        secno=mapp(logicno);
    }else{
    secno = mapp(logicno);
    }
    ide_write(0, secno, src, 1);
    w[secno]=0;
}

void ssd_erase(u_int logicno){
    if(map[logicno] == X){
        return;
    }
    erase(logicno);
}
// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(temp)));
        temp = begin + off;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(temp)));
        temp = DEV_DISK_OPERATION_READ;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(temp)));
        panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(temp)));
        if (temp) {
            panic_on(syscall_read_dev(dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
        } else {
            user_panic("ide.c: read FAIL.\n");
        }
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(temp)));
        temp = begin + off;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(temp)));
        panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
        temp = DEV_DISK_OPERATION_WRITE;
        panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(temp)));
        panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(temp)));
        if (!temp) {
            user_panic("ide.c: write FAIL.\n");
        }
	}
}
