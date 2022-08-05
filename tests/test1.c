#include "../driver/driver.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {
    int fd = 0;
    if ((fd = open("/dev/sda", O_RDWR | O_SYNC)) == -1) {
		perror("open device");
		exit(1);
	}
	printf("fd = %d\n", fd);
    struct mbr mmbr;
    get_mbr(fd, &mmbr);
    struct partition p;
    get_partition_from_mbr(0, &mmbr, &p);
    unsigned int p_start = get_partition_start(&p);
    struct boot_sector bs;
    get_boot_sector(fd, p_start, &bs);

    unsigned int fsls = string_to_int(bs.fs_info, 2);       //  fs info logical sector
    unsigned int rcl = string_to_int(bs.root_start, 4);     //  root cluster number
    unsigned int lsc = string_to_int(bs.ls_per_cl, 1);      //  logical sectors per cluster
    unsigned int bls = string_to_int(bs.b_per_ls, 2);       //  bytes per logical sector
    unsigned int ssa = sectors_start_address(&bs);          //  sector starting address
    unsigned int fataddr = p_start + fat_start_address(&bs);//  (first) fat starting address 
    unsigned int rootaddr = p_start + get_ls_address(ssa, lsc, rcl, bls); //  root address
    close(fd);

    printf("FAT starting at: %x\n\n\n", fataddr);

    int done = 1;
    if ((fd = open("/dev/sda", O_RDONLY)) == -1) {
        perror("open device");
        exit(1);
    }
    struct dir_entry *de;// = get_dir_entry_by_shname(fd, "TEST123", 7, rootaddr);
    close(fd);
    int fp;
    int size = 0;
    int buf_size = 2;
    unsigned char *buf = malloc(sizeof(char) * buf_size);
    memset(buf, 'a', buf_size);
    while (done) {
        if ((fp = open("/media/syrmia/KINGSTON/test123.txt", O_TRUNC | O_WRONLY | O_CREAT | O_SYNC)) == -1)
            printf("cant open a file");

        lseek(fp, 0, SEEK_END);
        if (write(fp, buf, buf_size) == -1) {
            perror("usr write");
            exit(1);
        }
        fsync(fp);
        fdatasync(fp);
        close(fp);

        if ((fd = open("/dev/sda", O_RDONLY | __O_DIRECT | O_SYNC)) == -1) {
            perror("open device");
            exit(1);
        }
        size = size + buf_size;
        de = get_dir_entry_by_shname(fd, "TEST123", 7, rootaddr);
        if (de && string_to_int(de->size, 4) == size)
            done = 0;
        fprintf(stdout, "Buffer size: %d, Entry size: %d\n", size, string_to_int(de->size, 4));
        
        close(fd);
    }
    printf("\n\n");
    dump_dir_entry(de);
    printf("Size written: %d\n", size);
    free(buf);
    // free(de);

    return 0;
}