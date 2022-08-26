#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include "driver.h"
#include <time.h>

/**
 * Function for printing in_string buffer in hex format
 * @param in_string buffer we would like to print
 * @param in_string_size size of the buffer
 * @param fold boolean type of param to indicate should 
 * we word-wrap on 16 chars or not 	
 */
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

/**
 * Function for printing in_string buffer as chars
 * @param in_string buffer we would like to print
 * @param in_string_size size of the buffer
 * @param fold boolean type of param to indicate should 
 * we word-wrap on 16 chars or not 	
 */
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

/**
 * Helper function used to convert array of bytes from 
 * little endian format to a number
 * @param in_string byte array 
 * @param in_string_size byte array length
 * @return value calculated from little endian format
 */
unsigned int string_to_int(void *in_string, int in_string_size) {
	unsigned int x = 0;
	for (int i = in_string_size - 1; i >= 0; --i) {
		x <<= 8;
		x += ((char *)in_string)[i] & 0xFF;
	}
	return x;
}

/**
 * Printing partition information
 * @param part partition we would like to print
 * @param partition_number optional; used just as index
 * @return boolean information if partition is empty or not
 */
int dump_partition(struct partition *part, int partition_number) {
	if (part->sys_type == 0x00)
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

/**
 * Printing boot sector
 * @param bs boot sector we would like to print
 */
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

/**
 * Printing fs information sector
 * @param fs fs information sector we would like to print
 */
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

/**
 * Printing directory entry
 * @param de directory entry we would like to print
 */
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

/**
 * Get fat starting address from boot sector
 * @param bs boot sector
 * @return fat sector starting address
 */
int fat_start_address(struct boot_sector *bs) {
	return string_to_int(bs->count_reserved, 2) * string_to_int(bs->b_per_ls, 2);
}

/**
 * Get data sectors address from boot sector
 * @param bs boot sector
 * @return data sectors starting address
 */
int sectors_start_address(struct boot_sector *bs) {
	return (string_to_int(bs->count_reserved, 2) + 
		string_to_int(bs->n_fat, 1) * string_to_int(bs->ls_per_fat32, 4) + 
		ceil((32 * string_to_int(bs->max_root_entry, 2)) / string_to_int(bs->b_per_ls, 2))) *
		string_to_int(bs->b_per_ls, 2);
}

/**
 * Get logical sector address of a cluster number
 * @param ssa data sector starting address
 * @param ls_per_cl logical sectors per cluster
 * @param cl_n cluster number
 * @param b_per_ls bytes per logical sector
 * @return logical sector address
 */
int get_ls_address(int ssa, int ls_per_cl, int cl_n, int b_per_ls) {
	return ssa + (cl_n - 2) * ls_per_cl * b_per_ls;
}

/**
 * Get the root directory entry address
 * @param root_dir root directory address
 * @param entry_num root directory entry number
 * @return root directory entry address
 */
int get_root_dir_entry_address(int root_dir, int entry_num) {
	return root_dir + 32 + (entry_num - 1) * 32;
}

/**
 * Read cluster data to the given buffer
 * @param fd file descriptor 
 * @param buf buffer in which we would like to read data
 * @param buf_size size of the buffer
 * @param ssa data sector starting address
 * @param cluster cluster number
 * @param bls bytes per logical sector
 * @param lsc logical sectors per cluster
 * @return number of bytes read, -1 if error
 */
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

/**
 * Write buffer to the cluster
 * @param fd file descriptor
 * @param buf buffer we would like to write
 * @param buf_size size of the buffer
 * @param ssa data sector starting address
 * @param cluster cluster number
 * @param bls bytes per logical sector
 * @param lsc logical sectors per cluster
 * @return number of bytes written, -1 if error
 */
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

/**
 * Get the FAT entry
 * @param fd file descriptor
 * @param buf buffer in which we would like to read data
 * @param buf_size buffer size (ex.: FAT32 = 4)
 * @param fat fat starting address
 * @param n fat entry number
 * @return number of bytes read, -1 if error
 */
int get_fat_entry(int fd, void *buf, int buf_size, unsigned int fat, unsigned int n) {
	int pos = lseek(fd, fat + n * buf_size, SEEK_SET);
	int nr = 0;
	if ((nr = read(fd, buf, buf_size)) == -1) {
		perror("read fat entry");
		return -1;
	}
	return nr;
}

/**
 * Print file starting from given cluster number
 * @param fd file descriptor
 * @param cluster fat entry number
 * @param fat fat starting address
 * @param ssa data sector starting address
 * @param bls bytes per logical sector
 * @param lsc logical sector per cluster
 */
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

/**
 * Get the mbr 
 * @param fd file descriptor
 * @param mbr mbr poiner
 */
void get_mbr(int fd, struct mbr *mbr) {
	unsigned char *buf = aligned_alloc(sysconf(_SC_PAGESIZE), sysconf(_SC_PAGESIZE));
	int pos = lseek(fd, 0, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, buf, sysconf(_SC_PAGESIZE)) == -1) {
		perror("read mbr");
		exit(1);
	}
	memcpy(mbr, buf, sizeof(struct mbr));
	free(buf);
}

/**
 * Get the partition
 * @param fd file descriptor
 * @param n partition number
 * @param pp partition pointer
 */
void get_partition(int fd, int n, struct partition *pp) {
	int pos = lseek(fd, 446 + 16 * n, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, pp, sizeof(struct partition)) == -1) {
		perror("read partition");
		exit(1);
	}
}

/**
 * Get the partition from the mbr struct
 * @param n partition number
 * @param mbr mbr
 * @param pp partition pointer
 */
void get_partition_from_mbr(int n, struct mbr *mbr, struct partition *pp) {
	memcpy(pp, mbr->partition_entry[n], sizeof(struct partition));
}

/**
 * Get the partition starting address
 * @param pp partition 
 * @return partition starting address 
 */
unsigned int get_partition_start(struct partition *pp) {
	return string_to_int(pp->start_sector, 4) * 512;
}

/**
 * Get the boot sector of a partition
 * @param fd file descriptor
 * @param partition_start partition starting address
 * @param bs boot sector pointer
 */
void get_boot_sector(int fd, unsigned int partition_start, struct boot_sector *bs) {
	unsigned char *buf = aligned_alloc(sysconf(_SC_PAGESIZE), sysconf(_SC_PAGESIZE));
	int pos = lseek(fd, partition_start, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, buf, sysconf(_SC_PAGESIZE)) == -1) {
		perror("read boot_sector");
		exit(1);
	}
	memcpy(bs, buf, sizeof(struct boot_sector));
	free(buf);
}


/**
 * Get the FS information sector
 * @param fd file descriptor
 * @param partition_start partition starting address
 * @param fsls fs information logical sector number
 * @param bls bytes per logical sector
 * @param fs fs information sector pointer
 */
void get_fs_info(int fd, unsigned int partition_start, unsigned int fsls, unsigned int bls, struct fs_info *fs) {
	unsigned char *buf = aligned_alloc(sysconf(_SC_PAGESIZE), sysconf(_SC_PAGESIZE));
	unsigned int fs_info = partition_start +  fsls * bls;
	int pos = lseek(fd, fs_info, SEEK_SET);
	printf("Position of pointer is: 0x%x\n", pos);
	if (read(fd, buf, sysconf(_SC_PAGESIZE)) == -1) {
		perror("read fs");
		exit(1);
	}
	memcpy(fs, buf , sizeof(struct fs_info));
	free(buf);
}

/**
 * Get root directory entry
 * @param fd file descriptor
 * @param root_dir root directory starting address
 * @param n root directory entry number
 * @param de root directory entry pointer
 */
void get_root_dir_entry(int fd, unsigned int root_dir, unsigned int n, struct dir_entry *de) {
	unsigned int f_dir_entry = get_root_dir_entry_address(root_dir, n);
	int pos = lseek(fd, root_dir + (f_dir_entry - root_dir) / sysconf(_SC_PAGESIZE) * sysconf(_SC_PAGESIZE), SEEK_SET);
	unsigned char *buf = aligned_alloc(sysconf(_SC_PAGESIZE), sysconf(_SC_PAGESIZE));
	if (read(fd, buf, sysconf(_SC_PAGESIZE)) == -1) {
		perror("read entry");
		exit(1);
	}
	struct dir_entry tmp;
	memcpy(de, buf + f_dir_entry - root_dir, sizeof(struct dir_entry));
	free(buf);
}

/**
 * Get the part of FAT
 * @param fd file descriptor
 * @param buf buffer
 * @param buf_size size of buffer, must be % 4 (for FAT32)
 * @param fataddr fat starting address
 * @param offset number of entries offset from the start
 */
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

/**
 * Get the part of root directory
 * @param fd file descriptor
 * @param buf buffer
 * @param buf_size size of buffer
 * @param rootaddr root directory starting address
 * @param offset number of entries offset from the start
 */
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

/**
 * Get the root directory entry from short name
 * @param fd file descriptor
 * @param name short name of the file
 * @param len short name lenght
 * @param rootaddr root directory starting address
 * @return pointer of root directory entry
 */
struct dir_entry* get_dir_entry_by_shname(int fd, char *name, int len, unsigned int rootaddr) {
	struct dir_entry *de = malloc(sizeof(struct dir_entry));
	de->short_name[0] = 0xE5;
	unsigned int i = 0;
	for(unsigned int i = 1; ((((char *)de->short_name)[0] & 0xFF) != 0x00) && (strncmp(name, de->short_name, len) != 0); ++i){
		get_root_dir_entry(fd, rootaddr, i, de);
		// printf("---0x%x--\n", ((de->short_name[0] & 0xFF)));
	}
	if ((((char *)de->short_name)[0] & 0xFF) == 0x00) 
		return NULL;
	return de;
}

/**
 * Get the bad sector count
 * @param fd file descriptor
 * @param fat file allocation table start
 * @param ls_per_fat logical sectors per fat
 * @return bad sector count
 */
unsigned int get_bad_sector_count(int fd, unsigned int fat, unsigned int ls_per_fat) {
	unsigned int count = 0;
	unsigned int ps = sysconf(_SC_PAGESIZE);
	unsigned char *buf = aligned_alloc(ps, ps);
	for (int i = 0; i * ps < ls_per_fat; ++i) {
		int pos = lseek(fd, fat + i * ps, SEEK_SET);
		if (read(fd, buf, ps) == -1) {
			perror("read entry");
			exit(1);
		}
		for (int j = 0; j < ps / 4; ++j) {
			unsigned int entry = string_to_int(buf + j * 4, 4) & 0x0FFFFFFF;
			if (entry == 0x0FFFFFF7)
				count++;
		}
	}
	free(buf);
	return count;
}

unsigned long long get_time_resolution_ns() {
	struct timespec t;
	clock_getres(CLOCK_MONOTONIC_RAW, &t);
	return t.tv_sec*1000000000ULL + t.tv_nsec;
}

unsigned long long get_time_ns() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
	return t.tv_sec*1000000000ULL + t.tv_nsec;
}

unsigned long long time_ns_pread(int fd, void *buf, size_t nbyte, off_t offset) {
	struct timespec st, et;
	clock_gettime(CLOCK_MONOTONIC_RAW, &st);
	
	if (-1 == pread(fd, buf, nbyte, offset)) {
		perror("pread failed");
		return -1;
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &et);
	return (et.tv_sec - st.tv_sec)*1000000000ULL + (et.tv_nsec - st.tv_nsec);
}

unsigned long long time_ns_pread_abs(int fd, void *buf, size_t nbyte, off_t offset) {
	struct timespec et;
	
	if (-1 == pread(fd, buf, nbyte, offset)) {
		perror("pread failed");
		return -1;
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &et);
	return et.tv_sec*1000000000ULL + et.tv_nsec;
}

void sample_stat_reset(struct sample_stat *ss) {
	ss->sum = 0;
	ss->sumsq = 0;
	ss->count = 0;
}

void sample_stat_sample(struct sample_stat *ss, double value) {
	ss->sum += value;
	ss->sumsq += (value * value);
	ss->count++;
}

double sample_stat_mean(struct sample_stat *ss) {
	return ss->sum/ss->count;
}

double sample_stat_stdevp(struct sample_stat *ss) {
	return sqrt(ss->sumsq/ss->count - (ss->sum/ss->count)*(ss->sum/ss->count));
}

double sample_stat_stdev(struct sample_stat *ss) {
	return sqrt(ss->sumsq/(ss->count-1) - (ss->sum*ss->sum)/(ss->count*(ss->count-1)));
}

double sample_stat_stdevrr(struct sample_stat *ss) {
	return sqrt((ss->sumsq - ss->sum*ss->sum/ss->count) / (ss->count*(ss->count-1)));
}

unsigned long long measure_rev_period(int fd, void *buf, unsigned int sector_size, unsigned long long measure_time_ns, int alternate) {
	printf("Measuring RPM for %.1f seconds...\n", measure_time_ns/1e9);
	
	if (-1 == pread(fd, buf, sector_size, 0)) {
		perror("pread failed");
		return -1;
	}

	unsigned int alt_pos = alternate ? sector_size : 0;
	unsigned long long st, et, mt, mintime=0, maxtime=0;
	unsigned int its = 0;
	mt = et = st = get_time_ns();
	for (its=0; st + measure_time_ns >= et; its++) {
		// Maxtor 7405AV: Can't turn off look-ahead, but looks like simply requesting a different sector will not be served from the cache.
		et = time_ns_pread_abs(fd, buf, sector_size, (its&1) ? alt_pos : 0);
		if (its==0 || et-mt > maxtime) maxtime = et-mt;
		if (its==0 || et-mt < mintime) mintime = et-mt;
		mt = et;
	}
	double rpm = its *60.0 / ((et-st)/1e9);
	printf("    RPM: %.3f (%.3f us per revolution, %u revolutions in %f seconds, max %.3f, min %.3f)\n", rpm, (et-st)/its/1e3, its, (et-st)/1e9, maxtime*1e-3, mintime*1e-3 );
	if (rpm > 20000) {
		// Seagate 15K.7: RCD and DRA are sufficient to disable read cache and prefetch
		// WD S25: DPTL=0 is needed to stop prefetch. It doesn't seem to obey DRA.
		printf("RPM seems too high for a hard drive. Make sure disk read caching is disabled (Use hdparm -A0 -a0 /dev/<disk> (ATA/SATA) or sdparm -s RCD,DRA,DPTL=0  /dev/<disk>) (SCSI/SAS)\n");
	}
	return (et-st)/its + 0.5;
}

unsigned int get_min_step(int fd, void *buf, unsigned int size, unsigned long long start, double revtime, unsigned int *min_step_time) {
	double min_step = 1;
	double min_step_time_avg = 0;
	double lr = 0.25;
	unsigned long long pos = start*size;
	unsigned long long st = get_time_ns();
	int phase = 0;
	for (unsigned int n = 0,lim=0; n < 200 && lr > 0.0000001 && lim < 10000;n++) {
		unsigned long long et = time_ns_pread_abs(fd, buf, size, pos);
		
		unsigned int t = et - st;
		min_step_time_avg = min_step_time_avg * (1.0-lr) + t*lr;
		if (t > 0.8*revtime) {
			if (phase)
				lr *= 0.8;
			phase = 0;
			min_step = min_step * (1.0+lr);
			n=0;
		}
		else {
			min_step = min_step * (1.0-0.01*lr);
			phase = 1;
		}
		pos += ((unsigned int)min_step)*size;
		//printf("%f: %.2f %.2f  %llu %.1f\n", lr, min_step, min_step_time_avg, t, 0.9*revtime);
		st = et;
	}
	
	// Add a bit of safety margin
	min_step_time_avg *= ceil(min_step*1.4)/min_step;
	min_step = ceil(min_step*1.4);

	//printf("Debug: min_step_time_avg = %f\n", min_step_time_avg);
	
	// Get a more accurate estimate of min_step_time
	struct sample_stat min_step_stat = {0, 0, 0};
	for (unsigned int i = 0;i < 400;i++) {
		unsigned long long et = time_ns_pread_abs(fd, buf, size, pos);
		pos += ((unsigned int)min_step)*size;
		if (et-st < min_step_time_avg*1.2) {		// Reject samples that look obviously wrong.
			sample_stat_sample(&min_step_stat, et-st);
			// min_step_stat.sample(et-st);
		}
		st = et;		
	}
	if (min_step_time) {
		if (min_step_stat.count > 200) {
			// *min_step_time = (unsigned int)(min_step_stat.mean() + min_step_stat.stdev()*2.0);
			*min_step_time = (unsigned int)(sample_stat_mean(&min_step_stat) + sample_stat_stdev(&min_step_stat)*2.0);
		}
		else {
			printf("Calibrate min_step: Too many bad samples?\n");
			printf("  Count: %u, min_step: %.0f, min_step_time_avg: %.0f, mean+2*stdev time: %.0f\n", min_step_stat.count, min_step, min_step_time_avg, (sample_stat_mean(&min_step_stat) + sample_stat_stdev(&min_step_stat)*2.0));
			*min_step_time = (unsigned int)min_step_time_avg;
		}
	}
	return min_step;
}

unsigned long long find_next_track_boundary (int fd, void *buf, unsigned int size, unsigned long long lb1, unsigned long long ub1, unsigned int min_step, unsigned int min_step_time, double revtime) {
	unsigned long long ub = ub1, lb = lb1;
	unsigned int retries = 5;
	
	while (ub - lb > 1 && retries) {
		if (ub - lb > min_step) {
			unsigned int next_track_partition1 = 0;
			unsigned int next_track_partition2 = 0;
			unsigned int it_limit = 64;
			do {
				next_track_partition1 = next_track_partition2;
				//printf("Finding partition: %u %llu-%llu %u %u\n", next_track_partition1, lb, ub, min_step, min_step_time);
				for (unsigned int j = 0; lb+min_step*j < ub+min_step; j++) {
					unsigned long long s = lb+j*min_step;					
					unsigned int t = time_ns_pread(fd, buf, size, s*size);

					if (j == 0) continue;
					if (t > 1.7 * revtime) {
						next_track_partition2 = next_track_partition1+1;	// Force fail
						break;		// Try again...
					}
					else {
						if (t > revtime + min_step_time*0.55) t -= revtime;
						if (t > revtime + min_step_time*0.55) {
							min_step *= 1.02;		// If min_step suddenly becomes too small for this region, try slowly increasing it.
							min_step_time *= 1.02;
							next_track_partition2 = next_track_partition1+1;	// Force fail
							break;		// Try again...
						}
						else if (t > (min_step_time + (0.3+0.1) * revtime)) {		// step + skew(0.3rev) + guardband(0.1rev)
							next_track_partition2 = s;
							break;
						}
						else if (t > min_step_time) {
							next_track_partition2 = s;
							break;
						}
					}
				}
			} while (next_track_partition1 != next_track_partition2 && --it_limit);
			if (next_track_partition2 == 0) return -1;		// No boundary found.
			lb = next_track_partition2-min_step;
			ub = next_track_partition2;

		}
		else {
			// ub-lb is never bigger than min_step.
			unsigned long long mp = (ub + lb) >> 1;
			unsigned int tl, tu;
			if (mp >= min_step + lb1) {
				int pread_res = pread(fd, buf, size, (mp-min_step)*size);		// This is always below lb
				tl = time_ns_pread(fd, buf, size, mp*size);
				tu = time_ns_pread(fd, buf, size, (mp+min_step)*size);	// This is always above ub
				if (tl > 1.7 * revtime || tu > 1.7*revtime) {		// Assumption that a correct read does not take more than this amount of time.
					//printf("Bad: Took too long, trying again.\n");			
					retries--;	
				}
				else {
					if (retries < 3) {
						// Desperation: if min_step became too small, it could cause tu/tl to exceed one revolution.
						if (tu > revtime) tu -= revtime;
						if (tl > revtime) tl -= revtime;
					}
					if (tu < min_step_time && tl > min_step_time) {
						ub = mp;		// time < min_step_time means no track boundary in that partition
					}
					else if (tl < min_step_time && tu > min_step_time) {
						lb = mp;	
					}
					else if (tu < min_step_time && tl < min_step_time) {
						retries = 0;		// Confused: Both were fast, so there is no track boundary at all?
						//printf("fast: %u - %u: %u %u, %u\n", lb, ub, tl, tu, min_step_time);
						break;
					}
					else
					{
						// Can become confused if the skew is too small and a read across the track
						// boundary causes a full revolution instead of a decrease in latency.
						if (retries < 3)
							printf("Confused, trying again %u: (%llu - %llu) %u %u\n", retries, lb, ub, tl, tu);		
						lb = lb1;
						ub = ub1;		
						retries--;
					}
				}
			}
			else {
				int pread_res = pread(fd, buf, size, lb*size);
				tl = time_ns_pread(fd, buf, size, mp*size);
				tu = time_ns_pread(fd, buf, size, ub*size);
				unsigned int low_time_limit = ((mp-lb)*min_step_time/min_step + 0.47*revtime) * 1.1;		// max_skew + time to read half partition if on same track + guardband

				// If controller/something is too slow, then even two sequential sector accesses that
				// cross a track boundary (including skew) will take > 1 revolution to complete.
				// The completion time should be exactly one rev + track skew. But set the lower bound
				// to 1.05*rev time so we don't misclassify accesses that take exactly one rev + 1 sector
				// (consecutive sectors on the same track). We're assuming that the variation in a measured revolution
				// is less than 5%, and that the track skew is significantly over 5% of a revolution.
				tu = fmod(tu-0.05*revtime, revtime) + 0.05*revtime;
				tl = fmod(tl-0.05*revtime, revtime) + 0.05*revtime;


				if (tu > 0.9 * revtime && tl < low_time_limit) {
					ub = mp;		// One revolution = track boundary isn't in the region we tested.
				}
				else if (tl > 0.9 * revtime && tu < low_time_limit) {
					lb = mp;	
				}
				else if (tu > 0.9 * revtime && tl > 0.9 * revtime) {
					retries = 0;
					break;
				}
				else
				{
					// Can become confused if a track switch takes too long (e.g., zone switch)
					// Eventually, the search range will increase enough so that the alternative 
					// algorithm (using min_step-spaced reads) can be used, which is more robust
					// to weird track switch times.
					//if (retries < 3) printf("Confused, trying again %u: (%llu - %llu) %u %u\n", retries, lb, ub, tl, tu);		
					lb = lb1;
					ub = ub1;		
					retries--;
				}
			}
		}
	}
	if (retries == 0) {
		return -1ULL;
	}
	return ub;
}

void track_bounds(int fd, void *buf, unsigned int size, unsigned long long start, unsigned long long end, double revtime, int fastmode, FILE* output) {
	// fastmode = 1 is similar to MIMD Zone-finding algorithm: http://www.esos.hanyang.ac.kr/files/publication/journals/international/a6-gim.pdf
	// -- Needs extra paranoia because track sizes can change frequently and the MIMD algorithm can produce incorrect answers.
	// fastmode = 0 searches for the next track boundary. It searches one track at a time.
	//   This algorithm is a O(lg n) algorithm for finding track boundaries, with a prediction of the expected track size to make n small.
	//   If the prediction is wrong, exponentially expand the search window until the track boundary is found.
	// fastmode = 1 is the MIMD algorithm with some extra checks added for better reliability.
	//   It can search multiple tracks ahead, assuming that if both N*track_size and (N-1)*track_size are both
	//   track boundaries, then it is highly likely that there are exactly N tracks with the same track_size in this region.
	//   If the track_size estimate needs to change, it falls back to the baseline fastmode=0 algorithm to compute the next track size.
	printf("Track boundaries%s. (%llu - %llu)\n", fastmode? " (fast)": "",start, end);

	unsigned long long pos = 0;
	unsigned int track_size;
	unsigned int zone_size_estimate = 1;

	unsigned int min_step = 10;
	unsigned int min_step_time = 0;
	
	// Find minimum step that doesn't require a complete revolution, then add a guardband.
	for (int calibrate_retries = 3; calibrate_retries >= 0; calibrate_retries--) {
		min_step = get_min_step (fd, buf, size, pos, revtime, &min_step_time);
		if (min_step_time < revtime / 2) break;
		if (calibrate_retries == 0) {
			printf("Calibrate min_step failed. Too big: %u, t=%u\n", min_step, min_step_time);
			return;
		}
	}
	
	printf("Using min_step %u, t=%u\n", min_step, min_step_time);

	pos = start;
	track_size = 100;		// Initial estimate of track size, doesn't have to be particularly accurate. Should be less than 2x the real track size, or this algorithm might find track pairs instead of tracks.
	unsigned int c = 0;
	while(pos < end) {

		if (fastmode) {
			unsigned long long new_pos;
			int allow_increase = 1;
			unsigned int zone_size = 0;
			unsigned int track_step = zone_size_estimate * 0.9;
			int first_iteration = 1;
			if (track_step < 4) track_step = 4;
			while (1) {
				const unsigned long long target = pos + track_step*track_size;			
				if (target < end) {
					// Large jumps: Check two adjacent tracks bounds to avoid an incorrect track_size guess from
					// being accepted as correct if track_step * wrong_track_size happens to be a multiple of correct_track_size.
					// This is less likely to happen for small jumps, because it requires a bigger error in the track size guess.
					if (track_step > 4) {
						new_pos = find_next_track_boundary(fd, buf, size, target-track_size-1, target-track_size+1, min_step, min_step_time, revtime);
					}
					if (track_step <= 4 || new_pos == target-track_size)
						new_pos = find_next_track_boundary(fd, buf, size, target-1, target+1, min_step, min_step_time, revtime);
					else
						new_pos = -1ULL;
				}
				else
					new_pos = -1ULL;

				if (new_pos == target) {		// The prediction was exactly correct
					for (unsigned long long p = pos + track_size; p <= new_pos; p+= track_size)
							// printf("%llu\t%u\n", p, track_size);
							fprintf(output, "%llu\t%u\n", p, track_size);
					zone_size += track_step;
					if (allow_increase)
						track_step <<= 1;
					else {
						allow_increase = 1;
					}
					pos = new_pos;
					if (first_iteration) {
						// Performance optimization: If the zone estimate (new_pos == target) is good, there are likely very few tracks left in the zone,
						// so shrink the track_step estimate to save one (likely-failing) query.
						track_step = 4;
					}					
				}
				else {
					if (track_step == 1)
						break;		// Fast mode fail, go back to finding one track boundary.
					track_step >>= 2;
					if (track_step < 1) track_step = 1;
					allow_increase = 0;
				}
				first_iteration = 0;
				// printf("track_step = %u\n", track_step);
			}
			if (zone_size > zone_size_estimate * 1.04 + 10)
				zone_size_estimate = zone_size_estimate * 1.04 + 10;
			else if (zone_size > zone_size_estimate)
				zone_size_estimate = (3*zone_size_estimate + zone_size) >> 2;
			else if (zone_size > zone_size_estimate * 0.8)
				zone_size_estimate = zone_size;
			else if (zone_size > 0)
				zone_size_estimate *= 0.8;
		}
		
		// change_retries: Keep retrying up to a few times if the track boundary keeps changing the track size.
		// Measure the track boundary at least twice, and accept it only if we get the same answer twice in a row.
		// Normally, a track size change results in only one retry to confirm the change.
		// The purpose is to reduce the chance that random noise can make us choose an incorrect
		// position for the next track boundary. It would be very expensive to retry and confirm
		// every track boundary, so only confirm those that change the track size. 
		// Track sizes don't tend to change frequently, so I'm more skeptical when it changes than when it doesn't.
		int change_retries = 10;
		unsigned int new_track_size = track_size;
		unsigned long long new_pos = -1ULL;
		while (change_retries--) {
			unsigned int tolerance = 1;
			while (new_pos == -1ULL) {
				if (tolerance >= new_track_size)
					break;
				new_pos = find_next_track_boundary(fd, buf, size, pos+new_track_size-tolerance, pos+new_track_size+tolerance, min_step, min_step_time, revtime);
				//if (new_pos != -1ULL)
				//	printf("  found: tolerance = %u, diff = %d\n", tolerance, new_pos - (pos + new_track_size));
				tolerance *= 4;
			}
			// We're desperate. Try 10 times before giving up.
			for (int i=0;i<10 && new_pos == -1ULL;i++) {
				if (i > 1) {
					// Recalibrate min_step too?
					unsigned int new_min_step_time;
					unsigned int new_min_step = get_min_step (fd, buf, size, pos, revtime, &new_min_step_time);
					if (new_min_step_time < revtime/2) {
						min_step = new_min_step;
						min_step_time = new_min_step_time;
						if (i > 2) {
							printf("Desperate: Increasing min_step_time by %.0f%%\n", (i-2)*7.0);
							min_step_time *= 1+(i-2)*0.07;
						}
						printf("Recalibrate: Using min_step %u, t=%u\n", min_step, min_step_time);
					}
					else
						printf("Recalibrate attempt failed: Continuing to use min_step %u, t=%u. Attempt was %u, t=%u\n", min_step, min_step_time, new_min_step, new_min_step_time);				
				}
				// The next track boundary must be between 1 and 20000 sectors away. Practically unconstrained because no disk has anywhere near 20K sectors per track. Yet.
				new_pos = find_next_track_boundary(fd, buf, size, pos+1, pos+20000, min_step, min_step_time, revtime);
			}
			
			if (new_pos == -1ULL)		// Boundary not found despite 10 retries. Skip ahead and continue. There will be incorrect answers here.
				break;
			if ((new_pos - pos) == new_track_size)	// Boundary found. Track size didn't change or was the same as the last retry attempt. Done.
				break;
			new_track_size = new_pos - pos;	// Boundary found, track size changed, so record the tentative new track size and try again to confirm.
		}
		
		if (new_pos == -1ULL) {
			pos += track_size;
			printf("Confused: Can't find next track. Giving up, skipping ahead to sector %llu, and trying again.\n", pos);
			continue;
		}
		
		// Limit growth of track size estimate. If we make an error on one track, this error can
		// persist indefinitely if the estimated track size is an integer multiple of the real track size.
		
		if ((new_pos - pos) > track_size * 1.2 + 10)
			track_size = track_size * 1.2 + 10;
		else if ((new_pos - pos) < track_size * 0.9)
			track_size = track_size * 0.9;
		else
			track_size = new_pos - pos;
		
		if (new_pos > end)
			new_pos = end;		// find_next_track_boundary can overrun the end of disk, so clamp the result then quit.
		// fprintf(output, "%llu\t%lld\n", new_pos, (new_pos - pos));
		if (c++ == 32) {
			c = 0;
			fflush(output);
		}
		pos = new_pos;
	}

	return;
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
			printf("Sectors starting address is: 0x%x\n", ssa + partition_start);
			

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
			printf("Root directory address is: 0x%x\n\n", rootaddr);
			get_root_dir(fd, buf, 512, partition_start + rootaddr, 0);
			// string_in_hex(buf, 512, 1);
			string_in_char(buf, 512, 1);

			
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

			struct dir_entry *fde = get_dir_entry_by_shname(fd, "TEST1", 5, rootaddr + partition_start);
			if (fde != NULL) {
				string_in_hex(fde, 64, 1);
				string_in_char(fde, 64, 1);
				dump_dir_entry(fde);
			}
			printf("Bad sector count: %u\n", get_bad_sector_count(fd, fataddr + partition_start, string_to_int(bs->ls_per_fat32, 4)));
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