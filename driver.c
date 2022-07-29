#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

struct partition {
    unsigned char boot_flag;
    unsigned char chs_begin[3];
    unsigned char sys_type;
    unsigned char chs_end[3];
    unsigned char start_sector[4];
    unsigned char nr_sector[4];
};

struct boot_sector {
    unsigned char jump_instr[3];
    unsigned char oem_name[8];
	unsigned char b_per_ls[2];
	unsigned char ls_per_cl[1];
	unsigned char count_reserved[2];
	unsigned char n_fat[1];
	unsigned char max_root_entry[2];
	unsigned char total_ls[2];
	unsigned char media_desc[1];
	unsigned char ls_per_fat[2];
	unsigned char ps_per_track[2];
	unsigned char n_heads[2];
	unsigned char n_hidden[4];
	unsigned char total_ls_32[4];
	unsigned char ls_per_fat32[4];
	unsigned char drive_desc[2];
	unsigned char version[2];
	unsigned char root_start[4];
	unsigned char fs_info[2];
	unsigned char copy[2];
	unsigned char reserved[12];
	unsigned char phy_drive_numer[1];
	unsigned char reserved_flag[1];
	unsigned char ext_boot_sign[1];
	unsigned char volume_id[4];
	unsigned char volume_label[11];
	unsigned char file_system_type[8];
	unsigned char boot_code[419];
	unsigned char drive_number[1];
	unsigned char sign[2];
};

struct fs_info {
	unsigned char sign1[4];
	unsigned char reserved1[480];
	unsigned char sign2[4];
	unsigned char free[4];
	unsigned char allocated[4];
	unsigned char reserved2[12];
	unsigned char sign3[4];
};

struct dir_entry {
	// LFN
	unsigned char seq_num[1];
	unsigned char name_1[10];
	unsigned char attr[1];
	unsigned char type[1];
	unsigned char checksum[1];
	unsigned char name_2[12];
	unsigned char first_cl[2];
	unsigned char name_3[4];
	// 8.3 name
	unsigned char short_name[8];
	unsigned char short_ext[3];
	unsigned char file_attr[1];
	unsigned char user_attr[1];
	unsigned char fc_fine_t[1];
	unsigned char hash_t[2];
	unsigned char create_date[2];
	unsigned char owner_id[2];
	unsigned char hi_cl[2];
	unsigned char l_mod_t[2];
	unsigned char l_mod_d[2];
	unsigned char lo_cl[2];
	unsigned char size[4];
};


void string_in_hex(void *in_string, int in_string_size, int fold) {
	int k = 0;
	for (int i = 0; i < in_string_size; ++i) {
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

void dump_dir_entry(struct dir_entry* de){
	printf("name = ");
	string_in_char(de->name_1, 10, 0);
	string_in_char(de->name_2, 12, 0);
	string_in_char(de->name_3, 4, 0);
	printf("short_name = ");
	string_in_char(de->short_name, 8, 0);
	printf("short_ext = ");
	string_in_char(de->short_ext, 3, 0);
	printf("first_cl_lfn = ");
	string_in_hex(de->first_cl, 2, 0);
	printf("hi_cl = ");
	string_in_hex(de->hi_cl, 2, 0);
	printf("lo_cl = ");
	string_in_hex(de->lo_cl, 2, 0);
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

int get_root_dir_entry(int root_dir, int entry_num) {
	return root_dir + 32 + (entry_num - 1) * 32;
}

void dump_file(int fd, unsigned char sector[4], unsigned int fat, unsigned int ssa, unsigned int bls, unsigned int lsc) {
	unsigned int n = string_to_int(sector, 4);
	int nr = 0;
	unsigned char buf[lsc * bls];
	printf("Cluster number: %u\n", n);
	int pos = lseek(fd, get_ls_address(ssa, lsc, n, bls), SEEK_SET);
	printf("Position of pointer is: 0x%x\n\n", pos);
	if ((nr = read(fd, buf, sizeof(buf))) == -1) {
		perror("read file");
		exit(1);
	}
	string_in_char(buf, 32, 1);
	string_in_hex(buf, 32, 1);

	pos = lseek(fd, fat + n * 4, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if ((nr = read(fd, buf, 4)) == -1) {
		perror("read file");
		exit(1);
	}
	printf("FAT entry is: ");
	string_in_hex(buf, 4, 0);
	if (string_to_int(buf, 4) != 0x0FFFFFFF) {
		dump_file(fd, buf, fat, ssa, bls, lsc);
	} else {
		printf("\n************* EOF *************\n\n");
	}
}

int main() {
	int nr = 0, pos = -1;
	int fd = 0;
	char buf[512];

	if ((fd = open("/dev/sda", O_RDONLY | O_SYNC)) == -1) {
		perror("open device");
		exit(1);
	}
	printf("fd = %d\n", fd);

	pos = lseek(fd, 0, SEEK_CUR);

	printf("Position of pointer is: 0x%x\n", pos);
	if ((nr = read(fd, buf, sizeof(buf))) == -1) {
		perror("read device");
		exit(1);
	}

	// hexdump -n 512 -C /dev/sda
	string_in_hex(buf, 512, 1);
	printf("Size of buf = %ld - and number of bytes read are %d\n", sizeof(buf), nr);
	
	printf("\n************* THE MAIN PARTITIONS *************\n\n");
	for (int i = 0 ; i < 4 ; ++i) {
		struct partition *sp = (struct partition *)(buf + 446 + (16 * i));
		int ret = dump_partition(sp, i);
		if (ret) {
			int partition_start = string_to_int(sp->start_sector, 4) * 512;
			printf("\nPartition starting address is: 0x%x\n\n", partition_start);
			printf("\n************* BOOT SECTOR *************\n\n");
			pos = lseek(fd, partition_start, SEEK_SET);
			printf("Position of pointer is: %x\n", pos);
			if ((nr = read(fd, buf, sizeof(buf))) == -1) {
				perror("read boot");
				exit(1);
			}
			struct boot_sector *bs = (struct boot_sector *) buf;
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
			unsigned int fs_info = partition_start +  fsls * bls;
			pos = lseek(fd, fs_info, SEEK_SET);
			printf("Position of pointer is: 0x%x\n", pos);
			if ((nr = read(fd, buf, sizeof(buf))) == -1) {
				perror("read fs");
				exit(1);
			}
			struct fs_info *fs = (struct fs_info *) buf;
			// string_in_hex(buf, 512, 1);
			dump_fs_info(fs);

			printf("\n************* FAT SECTOR *************\n\n");
			pos = lseek(fd, partition_start + fataddr, SEEK_SET);
			printf("Position of pointer is: 0x%x\n", pos);
			if ((nr = read(fd, buf, sizeof(buf))) == -1) {
				perror("read fat");
				exit(1);
			}
			string_in_hex(buf, 512, 1);

			printf("\n************* ROOT DIR SECTOR *************\n\n");
			unsigned int root_dir = partition_start + get_ls_address(ssa, lsc, rcl, bls);
			pos = lseek(fd, root_dir, SEEK_SET);
			printf("Position of pointer is: 0x%x\n", pos);
			if ((nr = read(fd, buf, sizeof(buf))) == -1) {
				perror("read root");
				exit(1);
			}
			// string_in_hex(buf, 512, 1);
			string_in_char(buf, 512, 1);

			printf("\n************* ROOT DIR ENTRY *************\n\n");
			unsigned int f_dir_entry = get_root_dir_entry(root_dir, 1);
			pos = lseek(fd, f_dir_entry, SEEK_SET);
			printf("Position of pointer is: 0x%x\n", pos);
			if ((nr = read(fd, buf, 64)) == -1) {
				perror("read entry");
				exit(1);
			}
			string_in_hex(buf, 64, 1);
			string_in_char(buf, 64, 1);
			struct dir_entry *de = (struct dir_entry *) buf;
			dump_dir_entry(de);

			printf("\n************* DUMP FILE *************\n\n");
			unsigned char arr[4] = {de->lo_cl[0], de->lo_cl[1], de->hi_cl[0], de->hi_cl[1]};
			dump_file(fd, arr, fataddr + partition_start, ssa + partition_start, bls, lsc);
		}
	}
	printf("\n\n\n");

	if (close(fd) == -1) {
		perror("close");
		exit(1);
	}

	return 0;
}