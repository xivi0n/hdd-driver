#include "../driver/driver.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#ifndef NOT_TESTING
// sudo ./test short_name filepath size_in_bytes device
// sudo ./test TEST0 /media/syrmia/KINGSTON/test0.txt 2450 /dev/sdb
int main(int argc, char **argv) {
    if (argc != 5) {
        perror("Invalid number of arguments!\n");
    }
    char shname[20];
    strcpy(shname, argv[1]);
    int size = atoi(argv[3]);
    printf("Size: %d\n", size);
    int fp;
    if ((fp = creat(argv[2], S_IWUSR | S_IRUSR)) == -1)
        printf("cant open a file");

    for (int i = 0; i < size; ++i)
        if (write(fp, "a", 1) == -1) {
            perror("usr write");
            exit(1);
        }
    
    fsync(fp);
    printf("Sync done!\n");
    close(fp);
    printf("File closed!\n");

    char command[50];
    strcpy(command, "sudo hdparm -f ");
    strcpy(command + 15, argv[4]);
    if (system(command) != 0 ) {
        perror("hdparm");
        exit(1);
    }
    printf("Reset completed!\n");

    int fd = 0;
    unsigned char *buf = malloc(sizeof(char) * 512);
    if ((fd = open("/dev/sdb", O_RDWR | O_SYNC)) == -1) {
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

    struct fs_info fs;
    get_fs_info(fd, p_start, fsls, bls, &fs);
    dump_fs_info(&fs);

    printf("FAT starting at: %x\n", fataddr);

    struct dir_entry *de = get_dir_entry_by_shname(fd, shname, strlen(shname), rootaddr);
    
    dump_dir_entry(de);

    unsigned char sector[4];
    memcpy(sector, de->lo_cl, 2);
    memcpy(sector + 2, de->hi_cl, 2);
    unsigned int offset = string_to_int(sector, 4);
    printf("Sector: %x\n", offset);
    printf("Address: 0x%x\n", p_start + get_ls_address(ssa, lsc, offset, bls));
    get_fat(fd, buf, 512, fataddr, offset);
    string_in_hex(buf, 512, 1);

    close(fd);
    return 0;
}
#endif