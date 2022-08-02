#include "driver/driver.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#ifndef NOT_TESTING
int main() {
    char shname[5] = "TEST1";
    int size = 4096;
    printf("Size: %d\n", size);
    int fp;
    if ((fp = creat("/media/syrmia/KINGSTON/test1.txt", S_IWUSR | S_IRUSR)) == -1)
        printf("cant open a file");

    for (int i = 0; i < size; ++i)
        if (write(fp, "a", 1) == -1) {
            perror("usr write");
            exit(1);
        }
    
    fsync(fp);
    close(fp);

    int fd = 0;
    unsigned char *buf = malloc(sizeof(char) * 512);
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

    unsigned int fsls = string_to_int(bs.fs_info, 2);		//	fs info logical sector
    unsigned int rcl = string_to_int(bs.root_start, 4);	    //	root cluster number
    unsigned int lsc = string_to_int(bs.ls_per_cl, 1);		//	logical sectors per cluster
    unsigned int bls = string_to_int(bs.b_per_ls, 2);		//	bytes per logical sector
    unsigned int ssa = sectors_start_address(&bs);			//	sector starting address
    unsigned int fataddr = p_start + fat_start_address(&bs);//	(first) fat starting address 

    printf("FAT starting at: %x\n", fataddr);

    unsigned int root_dir = p_start + get_ls_address(ssa, lsc, rcl, bls);
    struct dir_entry *de = get_dir_entry_by_shname(fd, shname, strlen(shname), root_dir);
    
    dump_dir_entry(de);

    unsigned char sector[4];
    memcpy(sector, de->lo_cl, 2);
    memcpy(sector + 2, de->hi_cl, 2);
    unsigned int offset = string_to_int(sector, 4);
    printf("Sector: %d\n", offset);
    get_fat(fd, buf, 512, fataddr, offset);
    string_in_hex(buf, 512, 1);

    close(fd);
    return 0;
}
#endif