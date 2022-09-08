#ifndef driver_h_
#define driver_h_

#include <stdio.h>

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

struct mbr {
	unsigned char bootstrap_code[446];
	unsigned char partition_entry[4][16];
	unsigned char boot_sign[2];
};

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

struct sample_stat {
	double sum, sumsq;
	unsigned int count;
};

typedef struct {
	struct sample_stat s;
	struct sample_stat s_nodelta;
	int first;
	int min;
} angpos_data_t;

void sample_stat_reset(struct sample_stat *ss);
void sample_stat_sample(struct sample_stat *ss, double value);
double sample_stat_mean(struct sample_stat *ss);
double sample_stat_stdevp(struct sample_stat *ss);
double sample_stat_stdev(struct sample_stat *ss);
double sample_stat_stdevrr(struct sample_stat *ss);

void string_in_hex(void *in_string, int in_string_size, int fold);
void string_in_char(void *in_string, int in_string_size, int fold);
unsigned int string_to_int(void *in_string, int in_string_size);

int fat_start_address(struct boot_sector *bs);
int sectors_start_address(struct boot_sector *bs);
int get_ls_address(int ssa, int ls_per_cl, int cl_n, int b_per_ls);
int get_root_dir_entry_address(int root_dir, int entry_num);

int read_from_cluster(int fd, void *buf, int buf_size, unsigned ssa, unsigned int cluster, unsigned int bls, unsigned int lsc);
int write_to_cluster(int fd, void *buf, int buf_size, unsigned ssa, unsigned int cluster, unsigned int bls, unsigned int lsc);

void get_mbr(int fd, struct mbr *mbr);
void get_partition(int fd, int n, struct partition *pp); 
void get_partition_from_mbr(int n, struct mbr *mbr, struct partition *pp);
unsigned int get_partition_start(struct partition *pp);
void get_boot_sector(int fd, unsigned int partition_start, struct boot_sector *bs); 
void get_fs_info(int fd, unsigned int partition_start, unsigned int fsls, unsigned int bls, struct fs_info *fs);
void get_root_dir_entry(int fd, unsigned int root_dir, unsigned int n, struct dir_entry *de);
void get_root_dir(int fd, void *buf, int buf_size, unsigned int rootaddr, unsigned int offset);
int get_fat_entry(int fd, void *buf, int buf_size, unsigned int fat, unsigned int n);
void get_fat(int fd, void *buf, int buf_size, unsigned int fataddr, unsigned int offset);

struct dir_entry* get_dir_entry_by_shname(int fd, char *name, int len, unsigned int rootaddr);
unsigned int get_bad_sector_count(int fd, unsigned int fat, unsigned int ls_per_fat);

int dump_partition(struct partition *part, int partition_number);
void dump_boot_sector(struct boot_sector *bs);
void dump_fs_info(struct fs_info *fs);
void dump_dir_entry(struct dir_entry* de);
void dump_file(int fd, unsigned int cluster, unsigned int fat, unsigned int ssa, unsigned int bls, unsigned int lsc);

unsigned long long get_time_ns();
unsigned long long get_time_resolution_ns();
unsigned long long time_ns_pread(int fd, void *buf, size_t nbyte, off_t offset);
unsigned long long time_ns_pread_abs(int fd, void *buf, size_t nbyte, off_t offset);

unsigned long long measure_rev_period(int fd, void *buf, unsigned int sector_size, unsigned long long measure_time_ns, int alternate, int suppress_header);

unsigned int get_min_step(int fd, void *buf, unsigned int size, unsigned long long start, double revtime, unsigned int *min_step_time);
unsigned long long find_next_track_boundary (int fd, void *buf, unsigned int size, unsigned long long lb1, unsigned long long ub1, unsigned int min_step, unsigned int min_step_time, double revtime);
void track_bounds(int fd, void *buf, unsigned int size, unsigned long long start, unsigned long long end, double revtime, int fastmode, FILE* output);
double angpos(int fd, void *buf, unsigned int size, unsigned int readsize, unsigned long long jump_from, unsigned long long start, unsigned int step, unsigned long long end, double max_error, int measure_absolute_time, double revtime, int suppress_header, FILE *fp);
unsigned long long get_closest_free_cluster(int fd, void *buf, unsigned int size, unsigned long long jump_from, unsigned long long track_size, unsigned long long cluster_size, double revtime, int suppress_header, FILE *fp);
#endif
