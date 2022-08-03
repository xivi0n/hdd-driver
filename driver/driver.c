#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include "driver.h"

void string_in_hex(void *in_string, int in_string_size, int fold) {
	if (fold) {
		printf("     | ");
		for (int i = 0; i < 16; ++i) {
			printf("%02x ", i & 0xFF);
		}
		printf("\n");
		printf("‒‒‒‒ ‒ ");
		for (int i = 0; i < 16; ++i) {
			printf("‒‒ ");
		}
		printf("\n");
	}
	int k = 0;
	for (int i = 0; i < in_string_size; ++i) {
		if (k == 0 && fold) printf("%04x | ", i & 0xFFFF);
		printf("%02x ", ((char *)in_string)[i] & 0xFF);
		k = k + 1;
		if (k == 16 && fold) {
			printf("\n");
			k = 0;
		}
	}
	printf("\n");
}

void string_in_char(void *in_string, int in_string_size, int fold) {
	int k = 0;
	for (int i = 0; i < in_string_size; ++i) {
		unsigned char c = ((char *)in_string)[i] & 0xFF;
		if (c >= 32 && c <= 126)
			printf("%c ", c);
		else
			printf("%c ", '.');
		k = k + 1;
		if (k == 16 && fold) {
			printf("\n");
			k = 0;
		}
	}
	printf("\n");
}

unsigned int string_to_int(void *in_string, int in_string_size) {
	unsigned int x = 0;
	for (int i = in_string_size - 1; i >= 0; --i) {
		x <<= 8;
		x += ((char *)in_string)[i] & 0xFF;
	}
	return x;
}

int dump_partition(struct partition *part, int partition_number) {
	if (part->sys_type == 0x00 || part->sys_type != 0x0c)
		return 0;
	printf("Partition /dev/sda%d\n", partition_number + 1);
	printf("boot_flag = %02X\n", part->boot_flag);
	printf("chs_begin = ");
	string_in_hex(part->chs_begin, 3, 0);
	printf("sys_type = %02X\n", part->sys_type);
	printf("chs_end = ");
	string_in_hex(part->chs_end, 3, 0);
	printf("start_sector = ");
	string_in_hex(part->start_sector, 4, 0);
	printf("nr_sector = ");
	string_in_hex(part->nr_sector, 4, 0);
}

void dump_boot_sector(struct boot_sector *bs) {
	printf("jump_instr = ");
	string_in_hex(bs->jump_instr, 3, 0);
	printf("oem_name = ");
	string_in_hex(bs->oem_name, 8, 0);
	printf("b_per_ls = ");
	string_in_hex(bs->b_per_ls, 2, 0);
	printf("ls_per_cl = ");
	string_in_hex(bs->ls_per_cl, 1, 0);
	printf("count_reserved = ");
	string_in_hex(bs->count_reserved, 2, 0);
	printf("n_fat = ");
	string_in_hex(bs->n_fat, 1, 0);
	printf("max_root_entry = ");
	string_in_hex(bs->max_root_entry, 2, 0);
	printf("total_ls = ");
	string_in_hex(bs->total_ls, 2, 0);
	printf("media_desc = ");
	string_in_hex(bs->media_desc, 1, 0);
	printf("ls_per_fat = ");
	string_in_hex(bs->ls_per_fat, 2, 0);
	printf("ps_per_track = ");
	string_in_hex(bs->ps_per_track, 2, 0);
	printf("n_heads = ");
	string_in_hex(bs->n_heads, 2, 0);
	printf("n_hidden = ");
	string_in_hex(bs->n_hidden, 4, 0);
	printf("total_ls_32 = ");
	string_in_hex(bs->total_ls_32, 4, 0);
	printf("ls_per_fat32 = ");
	string_in_hex(bs->ls_per_fat32, 4, 0);
	printf("drive_desc = ");
	string_in_hex(bs->drive_desc, 2, 0);
	printf("version = ");
	string_in_hex(bs->version, 2, 0);
	printf("root_start = ");
	string_in_hex(bs->root_start, 4, 0);
	printf("fs_info = ");
	string_in_hex(bs->fs_info, 2, 0);
	printf("copy = ");
	string_in_hex(bs->copy, 2, 0);
	printf("reserved = ");
	string_in_hex(bs->reserved, 12, 0);
	printf("phy_drive_numer = ");
	string_in_hex(bs->phy_drive_numer, 1, 0);
	printf("reserved_flag = ");
	string_in_hex(bs->reserved_flag, 1, 0);
	printf("ext_boot_sign = ");
	string_in_hex(bs->ext_boot_sign, 1, 0);
	printf("volume_id = ");
	string_in_hex(bs->volume_id, 4, 0);
	printf("volume_label = ");
	string_in_hex(bs->volume_label, 11, 0);
	// printf("boot code\n");
	// string_in_hex(bs->boot_code, 419, 1);
	printf("file_system_type = ");
	string_in_hex(bs->file_system_type, 8, 0);
	printf("drive_number = ");
	string_in_hex(bs->drive_number, 1, 0);
	printf("sign = ");
	string_in_hex(bs->sign, 2, 0);
}

void dump_fs_info(struct fs_info *fs) {
	printf("sign1 = ");
	string_in_hex(fs->sign1, 4, 0);
	// printf("reserved1\n");
	// string_in_hex(fs->reserved1, 480, 1);
	printf("sign2 = ");
	string_in_hex(fs->sign2, 4, 0);
	printf("free = ");
	string_in_hex(fs->free, 4, 0);
	printf("allocated = ");
	string_in_hex(fs->allocated, 4, 0);
	printf("reserved2 = ");
	string_in_hex(fs->reserved2, 12, 0);
	printf("sign3 = ");
	string_in_hex(fs->sign3, 4, 0);
}

void dump_dir_entry(struct dir_entry* de) {
	printf("name = ");
	string_in_char(de->name_1, 10, 0);
	string_in_char(de->name_2, 12, 0);
	string_in_char(de->name_3, 4, 0);
	printf("short_name = ");
	string_in_hex(de->short_name, 8, 0);
	string_in_char(de->short_name, 8, 0);
	printf("short_ext = ");
	string_in_char(de->short_ext, 3, 0);
	printf("first_cl_lfn = ");
	string_in_hex(de->first_cl, 2, 0);
	printf("hi_cl = ");
	string_in_hex(de->hi_cl, 2, 0);
	printf("lo_cl = ");
	string_in_hex(de->lo_cl, 2, 0);
	printf("size = ");
	string_in_hex(de->size, 4, 0);
	printf("size = %u bytes\n", string_to_int(de->size, 4));
}

int fat_start_address(struct boot_sector *bs) {
	return string_to_int(bs->count_reserved, 2) * string_to_int(bs->b_per_ls, 2);
}

int sectors_start_address(struct boot_sector *bs) {
	return (string_to_int(bs->count_reserved, 2) + 
		string_to_int(bs->n_fat, 1) * string_to_int(bs->ls_per_fat32, 4) + 
		ceil((32 * string_to_int(bs->max_root_entry, 2)) / string_to_int(bs->b_per_ls, 2))) *
		string_to_int(bs->b_per_ls, 2);
}

int get_ls_address(int ssa, int ls_per_cl, int cl_n, int b_per_ls) {
	return ssa + (cl_n - 2) * ls_per_cl * b_per_ls;
}

int get_root_dir_entry_address(int root_dir, int entry_num) {
	return root_dir + 32 + (entry_num - 1) * 64;
}

int read_from_cluster(int fd, void *buf, int buf_size, unsigned ssa, unsigned int cluster, unsigned int bls, unsigned int lsc) {
	int nr = 0;
	printf("Cluster number: %u\n", cluster);
	int pos = lseek(fd, get_ls_address(ssa, lsc, cluster, bls), SEEK_SET);
	printf("Position of pointer is: 0x%x\n\n", pos);
	if ((nr = read(fd, buf, buf_size)) == -1) {
		perror("read cluster");
		return -1;
	}
	return nr;
}

int write_to_cluster(int fd, void *buf, int buf_size, unsigned ssa, unsigned int cluster, unsigned int bls, unsigned int lsc) {
	int nw = 0;
	printf("Cluster number: %u\n", cluster);
	int pos = lseek(fd, get_ls_address(ssa, lsc, cluster, bls), SEEK_SET);
	printf("Position of pointer is: 0x%x\n\n", pos);
	if ((nw = write(fd, buf, buf_size)) == -1) {
		perror("write cluster");
		return -1;
	}
	return nw;
}

int get_fat_entry(int fd, void *buf, int buf_size, unsigned int fat, unsigned int n) {
	int pos = lseek(fd, fat + n * buf_size, SEEK_SET);
	int nr = 0;
	if ((nr = read(fd, buf, buf_size)) == -1) {
		perror("read fat entry");
		return -1;
	}
	return nr;
}

void dump_file(int fd, unsigned int cluster, unsigned int fat, unsigned int ssa, unsigned int bls, unsigned int lsc) {
	int nr = 0;
	unsigned char buf[lsc * bls];
	read_from_cluster(fd, buf, 32, ssa, cluster, bls, lsc);
	string_in_char(buf, 32, 1);
	string_in_hex(buf, 32, 1);

	get_fat_entry(fd, buf, 4, fat, cluster);
	printf("FAT entry is: ");
	string_in_hex(buf, 4, 0);
	printf("--------------------------\n");
	unsigned int cluster_val = string_to_int(buf, 4) & 0x0FFFFFFF;
	if (cluster_val == 0x00000000) {
		perror("reading free cluster");
		exit(1);
	}
	if (cluster_val != 0x0FFFFFFF) {
		dump_file(fd, cluster_val, fat, ssa, bls, lsc);
	} else {
		printf("\n************* EOF *************\n\n");
	}
}

void get_mbr(int fd, struct mbr *mbr) {
	int pos = lseek(fd, 0, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, mbr, sizeof(struct mbr)) == -1) {
		perror("read mbr");
		exit(1);
	}
}

void get_partition(int fd, int n, struct partition *pp) {
	int pos = lseek(fd, 446 + 16 * n, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, pp, sizeof(struct partition)) == -1) {
		perror("read partition");
		exit(1);
	}
}

void get_partition_from_mbr(int n, struct mbr *mbr, struct partition *pp) {
	memcpy(pp, mbr->partition_entry[n], sizeof(struct partition));
}

unsigned int get_partition_start(struct partition *pp) {
	return string_to_int(pp->start_sector, 4) * 512;
}

void get_boot_sector(int fd, unsigned int partition_start, struct boot_sector *bs) {
	int pos = lseek(fd, partition_start, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, bs, sizeof(struct boot_sector)) == -1) {
		perror("read boot_sector");
		exit(1);
	}
}

void get_fs_info(int fd, unsigned int partition_start, unsigned int fsls, unsigned int bls, struct fs_info *fs) {
	unsigned int fs_info = partition_start +  fsls * bls;
	int pos = lseek(fd, fs_info, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, fs, sizeof(struct fs_info)) == -1) {
		perror("read fs");
		exit(1);
	}
}

void get_root_dir_entry(int fd, unsigned int root_dir, unsigned int n, struct dir_entry *de) {
	unsigned int f_dir_entry = get_root_dir_entry_address(root_dir, n);
	int pos = lseek(fd, f_dir_entry, SEEK_SET);
	if (read(fd, de, sizeof(struct dir_entry)) == -1) {
		perror("read entry");
		exit(1);
	}
}

void get_fat(int fd, void *buf, int buf_size, unsigned int fataddr, unsigned int offset) {
	if (buf_size % 4 != 0) {
		perror("read fat");
		exit(1);
	}
	unsigned char tmp[4];
	for (int i = 0; i < buf_size / 4; ++i) {
		get_fat_entry(fd, tmp, 4, fataddr + offset * 4, i);
		memcpy(buf + i * 4, tmp, 4);
	}
}

void get_root_dir(int fd, void *buf, int buf_size, unsigned int rootaddr, unsigned int offset) {
	int size = sizeof(struct dir_entry);
	if (buf_size % size != 0) {
		perror("read root_dir");
		exit(1);
	}
	struct dir_entry de;
	for (int i = 0; i < buf_size / size; ++i) {
		get_root_dir_entry(fd, rootaddr + offset * size, i + 1, &de);
		memcpy(buf + i * size, &de, size);
	}
}



struct dir_entry* get_dir_entry_by_shname(int fd, char *name, int len, unsigned int rootaddr) {
	struct dir_entry *de = malloc(sizeof(struct dir_entry));
	unsigned int i = 0;
	for(unsigned int i = 0; ((((char *)de->short_name)[0] & 0xFF) != 0xE5) && (strncmp(name, de->short_name, len) != 0); ++i){
		get_root_dir_entry(fd, rootaddr, i, de);
	}
	if ((((char *)de->short_name)[0] & 0xFF) == 0xE5) 
		return 0;
	return de;
}

#ifdef NOT_TESTING
int main() {
	int nr = 0, pos = -1;
	int fd = 0;
	char buf[512];

	if ((fd = open("/dev/sda", O_RDWR | O_SYNC)) == -1) {
		perror("open device");
		exit(1);
	}
	printf("fd = %d\n", fd);

	struct mbr *mbr = malloc(sizeof(struct mbr));
	get_mbr(fd, mbr);

	// hexdump -n 512 -C /dev/sda
	string_in_hex(mbr, sizeof(struct mbr), 1);
	printf("Size of buf = %ld - and number of bytes read are %d\n", sizeof(buf), nr);
	
	printf("\n************* THE MAIN PARTITIONS *************\n\n");
	for (int i = 0 ; i < 1 ; ++i) {
		struct partition *sp = malloc(sizeof(struct partition));
		get_partition_from_mbr(i, mbr, sp);
		// get_partition(fd, i, sp);
		int ret = dump_partition(sp, i);
		if (ret) {
			unsigned int partition_start = get_partition_start(sp);
			printf("\nPartition starting address is: 0x%x\n\n", partition_start);
			printf("\n************* BOOT SECTOR *************\n\n");
			pos = lseek(fd, partition_start, SEEK_SET);
			printf("Position of pointer is: %x\n", pos);
			if ((nr = read(fd, buf, sizeof(buf))) == -1) {
				perror("read boot");
				exit(1);
			}
			struct boot_sector *bs = malloc(sizeof(struct boot_sector));
			get_boot_sector(fd, partition_start, bs);
			// string_in_hex(buf, 512, 1);
			dump_boot_sector(bs);

			unsigned int fsls = string_to_int(bs->fs_info, 2);		//	fs info logical sector
			unsigned int rcl = string_to_int(bs->root_start, 4);	//	root cluster number
			unsigned int lsc = string_to_int(bs->ls_per_cl, 1);		//	logical sectors per cluster
			unsigned int bls = string_to_int(bs->b_per_ls, 2);		//	bytes per logical sector
			unsigned int ssa = sectors_start_address(bs);			//	sector starting address
			unsigned int fataddr = fat_start_address(bs);			//	(first) fat starting address 

			printf("\nFAT starting address is: 0x%x\n", fataddr + partition_start);
			printf("Sectors starting address is: 0x%x\n\n", ssa + partition_start);

			printf("\n************* FS INFO SECTOR *************\n\n");
			
			struct fs_info *fs = malloc(sizeof(struct fs_info));
			get_fs_info(fd, partition_start, fsls, bls, fs);
			// string_in_hex(buf, 512, 1);
			dump_fs_info(fs);

			printf("\n************* FAT SECTOR *************\n\n");
			get_fat(fd, buf, 512, partition_start + fataddr, 0);
			string_in_hex(buf, 512, 1);

			printf("\n************* ROOT DIR SECTOR *************\n\n");
			unsigned int rootaddr = get_ls_address(ssa, lsc, rcl, bls);
			get_root_dir(fd, buf, 512, partition_start + rootaddr, 0);
			// string_in_hex(buf, 512, 1);
			string_in_char(buf, 512, 1);

			break;
			printf("\n************* ROOT DIR ENTRY *************\n\n");
			struct dir_entry *de = malloc(sizeof(struct dir_entry));
			get_root_dir_entry(fd, rootaddr + partition_start, 1, de);
			
			string_in_hex(de, 64, 1);
			string_in_char(de, 64, 1);
			
			dump_dir_entry(de);

			printf("\n************* DUMP FILE *************\n\n");
			unsigned char arr[4] = {de->lo_cl[0], de->lo_cl[1], de->hi_cl[0], de->hi_cl[1]};
			unsigned int sector = string_to_int(arr, 4);
			dump_file(fd, sector, fataddr + partition_start, ssa + partition_start, bls, lsc);

			struct dir_entry *fde = get_dir_entry_by_shname(fd, "B", 1, rootaddr + partition_start);
			if (fde != NULL)
				dump_dir_entry(fde);
		}
	}
	printf("\n\n\n");

	if (close(fd) == -1) {
		perror("close");
		exit(1);
	}

	return 0;
}
#endif