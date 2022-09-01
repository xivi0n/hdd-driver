#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fs.h>
#include "../driver/driver.h"

// sudo ./access-time -d /dev/sda -r 0,0,1,10 [-fs N][-f /output_file.txt]
// sudo ./access-time -d /dev/sda -l 0,5,6,7,8
int main(int argc, char* argv[]) {
    if (argc < 5) {
		perror("Invalid number of arguments! ./access-time -d device -r ref,start,step,end,err | -l ref,err,1st,2nd,3rd [-fs N][-f /output_file.txt][-ang]");
		return -1;
	}
    char *filename = NULL;
	char *dev = NULL;
	char *ss = NULL;
    char *range = NULL;
    char *list = NULL;
    int suppress_header = 1;
    int mode = 1;
    for(int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-f") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			filename = argv[i + 1];
		else if (strcmp(argv[i], "-d") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			dev = argv[i + 1];
		else if (strcmp(argv[i], "-fs") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			ss = argv[i + 1];
        else if (strcmp(argv[i], "-r") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			range = argv[i + 1];
        else if (strcmp(argv[i], "-l") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			list = argv[i + 1];
        else if (strcmp(argv[i], "-ang") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			mode = 0;
	}

    unsigned long long measure_rpm_time = 3000000000;
    int rpm_alternate = 1;
    unsigned int force_sector_size = 0;
	unsigned int sector_size = 0;
    FILE *fp = NULL;
    fp = !filename ? stdout : fopen(filename, "w");

    char *sector_buf = NULL;
	unsigned long long revtime = 0;

    if (ss) {
		force_sector_size = atoi(ss);
	}

    int fd = open(dev, __O_DIRECT | O_RDONLY);
	if (fd == -1)	{
		perror("Failed to open");
		return -1;
	}
    posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);
    unsigned long long sz = -1;
	sz = lseek(fd, 0, SEEK_END);
	ioctl(fd, BLKSSZGET, &sector_size);		// This is usually wrong for 4KB sector drives that emulate 512B. Manually force 4K when testing one of those drives.

	if (force_sector_size)
		sector_size = force_sector_size;
	if (sector_size == 0) {
		perror("Sector size can't be zero. Use -fs to choose a sector size if necessary.\n");
		return -1;
	}
    sector_buf = (char*)mmap(NULL, sector_size*16, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
    if (revtime == 0)
		revtime = measure_rev_period(fd, sector_buf, sector_size, measure_rpm_time, rpm_alternate, 1);

    if (range) {
        unsigned long long arr[5] = {0, 0, 0, 0, 0};
        char* token = strtok(range, ",");
        int i = 0;
        for(i = 0; token != NULL; ++i) {
            arr[i] = atoi(token);
            token = strtok(NULL, ",");
        }
        if (i != 5) {
            perror("Invalid range!\n");
            return -1;
        }
        angpos(fd, sector_buf, sector_size, sector_size, arr[0], arr[1], arr[2], arr[3], arr[4], mode, revtime, suppress_header, fp);
    } else if (list) {
        char* token = strtok(list, ",");
        unsigned long long ref = 0;
        unsigned long long err = 0;
        if (token) {
            ref = atoi(token);
            token = strtok(NULL, ",");
        } else {
            perror("Invalid list: reference missing!\n");
            return -1;
        }
        if (token) {
            err = atoi(token);
            token = strtok(NULL, ",");
        } else {
            perror("Invalid list: error missing!\n");
            return -1;
        }
        while(token != NULL) {
            unsigned long long t = atoi(token);
            angpos(fd, sector_buf, sector_size, sector_size, ref, t, 1, t + 1, err, mode, revtime, suppress_header, fp);
            token = strtok(NULL, ",");
        }
    }

    fclose(fp);
    close(fd);
    return 0;
}