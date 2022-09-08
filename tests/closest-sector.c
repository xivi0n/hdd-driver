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

// sudo ./closest-sector -d /dev/sda -f fat.txt -s 2400 [-t 1440][-c 12][-rev 8333395][-fs 512]
int main(int argc, char* argv[]) {
    if (argc < 7) {
        perror("Invalid number of arguments! ./closest-sector -d device -f fat.file -s N [-t N][-c N][-rev N][-fs N]\n");
		return -1;
    }
    int suppress_header = 0;
    char *filename = NULL;
    char *dev = NULL;

    unsigned long long revtime = 0;
    unsigned long long measure_rpm_time = 3000000000;
    unsigned long long jump_from = -1;
    unsigned long long track_size = 1440;
    unsigned long long cluster_size = 12;
    unsigned long long force_sector_size = 0;
    
    for(int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-f") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			filename = argv[i + 1];
		else if (strcmp(argv[i], "-d") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			dev = argv[i + 1];
		else if (strcmp(argv[i], "-fs") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			force_sector_size = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "-rev") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			revtime = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-s") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			jump_from = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-t") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			track_size = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-c") == 0 && (i != argc - 1) && (argv[i+1][0] != '-'))
			cluster_size = atoi(argv[i+1]);
	}

    if (jump_from == -1) {
        perror("Invalid starting sector\n");
		return -1;
    }

    int rpm_alternate = 1;
    unsigned int sector_size = 0;

    FILE *fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_SET);
    int fd = open(dev, __O_DIRECT | O_RDONLY);
    if (fd == -1)	{
        perror("Failed to open");
        return -1;
    }
    posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);
    unsigned long long sz = -1;
    sz = lseek(fd, 0, SEEK_END);
    ioctl(fd, BLKSSZGET, &sector_size);
    if (force_sector_size)
		sector_size = force_sector_size;
	if (sector_size == 0) {
		perror("Sector size can't be zero. Use -fs to choose a sector size if necessary.\n");
		return -1;
	}
    char *sector_buf = (char*)mmap(NULL, sector_size*16, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
    if (revtime == 0)
	    revtime = measure_rev_period(fd, sector_buf, sector_size, measure_rpm_time, rpm_alternate, 0);
    get_closest_free_cluster(fd, sector_buf, sector_size, jump_from, track_size, cluster_size, revtime, suppress_header, fp);
    close(fd);
    fclose(fp);
}