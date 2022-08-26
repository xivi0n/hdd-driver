#include "../driver/driver.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fs.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/ioctl.h>


#define CLAMPU(N,LIMIT) do { if ((N)>(LIMIT)) (N)=(LIMIT); } while(0)
int main() {
    char *fname = "/dev/sda";
    unsigned long long measure_rpm_time = 3000000000;
    int rpm_alternate = 1;
	unsigned int force_sector_size = 0;
	unsigned int sector_size=0;

	char *sector_buf=NULL;
	double revtime = 0;

    int fd = open(fname, __O_DIRECT | O_RDONLY);
	if (fd == -1)	{
		perror("Failed to open");
		return -1;
	}
    posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);

    unsigned long long sz = -1;
	sz = lseek(fd, 0, SEEK_END);
	ioctl(fd, BLKSSZGET, &sector_size);		// This is usually wrong for 4KB sector drives that emulate 512B. Manually force 4K when testing one of those drives.
	
	printf ("Sector size is %u\n", sector_size);

	if (force_sector_size) {
		printf("Forcing sector size to be %u bytes\n", force_sector_size);
		sector_size = force_sector_size;
	}
	if (sector_size == 0) {
		printf("Sector size can't be zero. Use --force-sector-size to choose a sector size if necessary.\n");
		return -1;
	}


	unsigned long long sz_sectors = sz / sector_size;
	printf ("File size is %llu bytes, %llu sectors\n", sz, sz_sectors);
	printf ("Timer claims to have a resolution of %llu ns\n", get_time_resolution_ns());

	sector_buf = (char*)mmap(NULL, sector_size*16, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
	if (revtime == 0) {
		revtime = measure_rev_period(fd, sector_buf, sector_size, measure_rpm_time, rpm_alternate);
	}
	else {
		printf("Assuming RPM = %.3f (%.3f us per revolution)\n", 60e9/revtime, revtime/1e3);
	}
    unsigned long long start = 0;
    unsigned long long end = sz_sectors;
    int fastmode = 1;

	FILE* fp = fopen("/home/syrmia/Desktop/disk/analysis/test.txt", "w");

	unsigned long long test_runtime_start = get_time_ns();
    track_bounds(fd, sector_buf, sector_size, start, end, revtime, fastmode, fp);
	printf ("Test runtime: %.3f s\n", 1.0*(get_time_ns()-test_runtime_start)/1.0e9);
	
	fclose(fp);
    close(fd);
    return 0;
}