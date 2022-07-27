#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
	unsigned char dos_bpb[25];
	unsigned char lspf[4];
	unsigned char drive_desc[2];
	unsigned char version[2];
	unsigned char root_start[4];
	unsigned char fs_info[2];
	unsigned char copy[2];
	unsigned char reserved[12];
	unsigned char cfs[26];
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

void dump_bpb(unsigned char bpb[25]) {
	printf("b_per_ls = ");
	string_in_hex(bpb + 0x00, 2, 0);
	printf("ls_per_cl = ");
	string_in_hex(bpb + 0x02, 1, 0);
	printf("count_reserved = ");
	string_in_hex(bpb + 0x03, 1, 0);
	printf("n_fat = ");
	string_in_hex(bpb + 0x05, 1, 0);
	printf("max_root_entry = ");
	string_in_hex(bpb + 0x06, 2, 0);
	printf("total_ls = ");
	string_in_hex(bpb + 0x08, 2, 0);
	printf("media_desc = ");
	string_in_hex(bpb + 0x0a, 1, 0);
	printf("ls_per_fat = ");
	string_in_hex(bpb + 0x0b, 2, 0);
	printf("ps_per_track = ");
	string_in_hex(bpb + 0x0d, 2, 0);
	printf("n_heads = ");
	string_in_hex(bpb + 0x0f, 2, 0);
	printf("n_hidden = ");
	string_in_hex(bpb + 0x11, 4, 0);
	printf("total_ls_32 = ");
	string_in_hex(bpb + 0x15, 4, 0);
}

void dump_boot_sector(struct boot_sector *bs) {
	printf("jump_instr = ");
	string_in_hex(bs->jump_instr, 3, 0);
	printf("oem_name = ");
	string_in_hex(bs->oem_name, 8, 0);
	dump_bpb(bs->dos_bpb);
	printf("lspf = ");
	string_in_hex(bs->lspf, 4, 0);
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
	printf("cfs = \n");
	string_in_hex(bs->cfs, 26, 1);
}

int main() {
	int nr = 0, pos = -1;
	int fd = 0;
	char buf[512];

	if ((fd = open("/dev/sda", O_RDONLY | O_SYNC)) == -1) {
		perror("open");
		exit(1);
	}
	printf("fd = %d\n", fd);

	pos = lseek (fd, 0, SEEK_CUR);

	printf("Position of pointer is: %d\n", pos);
	if ((nr = read(fd, buf, sizeof(buf))) == -1) {
		perror("read");
		exit(1);
	}

	if (close(fd) == -1) {
		perror("close");
		exit(1);
	}

	// hexdump -n 512 -C /dev/sda
	string_in_hex(buf, 512, 1);
	printf("Size of buf = %ld - and number of bytes read are %d\n", sizeof(buf), nr);
	
	printf("\n************* THE MAIN PARTITIONS *************\n\n");
	for (int i = 0 ; i < 4 ; ++i) {
		struct partition *sp = (struct partition *)(buf + 446 + (16 * i));
		int ret = dump_partition(sp, i);
		// if (ret) {
		// 	printf("\n************* BOOT SECTOR *************\n\n");
		// 	struct boot_sector *bs = (struct boot_sector *)(buf + 0);
		// 	dump_boot_sector(bs);
		// }
	}
	printf("\n\n\n");

	return 0;
}