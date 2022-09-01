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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        perror("Invalid number of arguments! ./rev-time -d device [-fs N]\n");
		return -1;
    }
    char *dev = NULL;
	char *ss = NULL;
    for(int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			dev = argv[i + 1];
		else if (strcmp(argv[i], "-fs") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			ss = argv[i + 1];
	}

    unsigned long long measure_rpm_time = 3000000000;
    unsigned long long revtime = 0;
    int rpm_alternate = 1;
    unsigned int force_sector_size = 0;
	unsigned int sector_size = 0;
    char *sector_buf = NULL;
    if (ss)
		force_sector_size = atoi(ss);

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
    if (revtime == 0) {
		revtime = measure_rev_period(fd, sector_buf, sector_size, measure_rpm_time, rpm_alternate, 1);
        printf("rev_time\t%llu\n", revtime);
		printf("rpm\t%.3f\n", 60e9/revtime);
    }
    return 0;
}