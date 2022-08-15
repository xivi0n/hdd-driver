#include "../driver/driver.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

void timeit(void (*f)(int, unsigned char*, char*), int size, unsigned char *buf,char *indicator, char *filename) {
    struct timeval start, stop;
    double secs = 0;
    printf("running: %s...", indicator);
    fflush(stdout);
    gettimeofday(&start, NULL);
    f(size, buf, filename);
    gettimeofday(&stop, NULL);
    secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("\33[2K\r%s: time taken: %f\n", indicator, secs);
    fflush(stdout);
}

void write_direct(int size, unsigned char *buf, char *filename) {
    int fd = open(filename,  O_TRUNC | O_WRONLY | O_CREAT | __O_DIRECT);
    if (fd < 0) {
        perror("file open error");
        exit(1);
    }
    for (int i = 0; i < 1000; ++i) {
        lseek(fd, 0, SEEK_SET);
        if (write(fd, buf, size) == -1) {
            perror("usr write direct");
            exit(1);
        }
    }
    close(fd);
}

void write_fsync(int size, unsigned char *buf, char *filename) {
    int fd = open(filename,  O_TRUNC | O_WRONLY | O_CREAT | O_ASYNC);
    if (fd < 0) {
        perror("file open error");
        exit(1);
    }
    for (int i = 0; i < 1000; ++i) {
        lseek(fd, 0, SEEK_SET);
        if (write(fd, buf, size) == -1) {
            perror("usr write fsync");
            exit(1);
        }
        fsync(fd);
    }
    close(fd);
}

void write_fdatasync(int size, unsigned char *buf, char *filename) {
    int fd = open(filename,  O_TRUNC | O_WRONLY | O_CREAT | O_ASYNC);
    if (fd < 0) {
        perror("file open error");
        exit(1);
    }
    for (int i = 0; i < 1000; ++i) {
        lseek(fd, 0, SEEK_SET);
        if (write(fd, buf, size) == -1) {
            perror("usr write fdatasync");
            exit(1);
        }
        fdatasync(fd);
    }
    close(fd);
}

// sudo ./test3 filepath n_times_pagesize 
// sudo ./test3 /media/syrmia/KINGSTON/test3.txt 10
int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Invalid number of arguments!\n");
        return 1;
    }
    int size = sysconf(_SC_PAGESIZE) * atoi(argv[2]);
    unsigned char *buf = aligned_alloc(size, size);
    memset(buf, 'a', size);

    timeit(write_direct, size, buf, "DIRECT", argv[1]);
    timeit(write_fsync, size, buf, "FSYNC", argv[1]);
    timeit(write_fdatasync, size, buf, "FDATASYNC", argv[1]);

    free(buf);
    return 0;
}