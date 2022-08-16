#include "../driver/driver.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>


double get_secs(struct timeval start, struct timeval stop) {
    return (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
}

double get_secs_t(struct timeval t) {
    return (double)(t.tv_usec) / 1000000 + (double)(t.tv_sec);
}

struct timeval add_t(struct timeval t1, struct timeval t2) {
    return (struct timeval) {t1.tv_sec + t2.tv_sec, t1.tv_usec + t2.tv_usec};
}

struct timeval sub_t(struct timeval t1, struct timeval t2) {
    return (struct timeval) {t1.tv_sec - t2.tv_sec, t1.tv_usec - t2.tv_usec};
}

void timeit(void (*f)(int, unsigned char*, char*), int size, unsigned char *buf,char *indicator, char *filename) {
    struct timeval start, stop;
    printf("running: %s...\n", indicator);
    fflush(stdout);
    gettimeofday(&start, NULL);
    f(size, buf, filename);
    gettimeofday(&stop, NULL);
    printf("time taken: %f\n\n", get_secs(start, stop));
    fflush(stdout);
}

void write_direct(int size, unsigned char *buf, char *filename) {
    struct timeval tmp[2], total[3] = {{0, 0}, {0, 0}, {0, 0}};
    int fd = open(filename,  O_TRUNC | O_WRONLY | O_CREAT | __O_DIRECT);
    if (fd < 0) {
        perror("file open error");
        exit(1);
    }
    for (int i = 0; i < 1000; ++i) {
        gettimeofday(&tmp[0], NULL);
        lseek(fd, 0, SEEK_SET);
        gettimeofday(&tmp[1], NULL);
        total[0] = add_t(total[0], sub_t(tmp[1], tmp[0]));
        gettimeofday(&tmp[0], NULL);
        if (write(fd, buf, size) == -1) {
            perror("usr write direct");
            exit(1);
        }
        gettimeofday(&tmp[1], NULL);
        total[1] = add_t(total[1], sub_t(tmp[1], tmp[0]));
        gettimeofday(&tmp[0], NULL);
        fsync(fd);
        gettimeofday(&tmp[1], NULL);
        total[2] = add_t(total[2], sub_t(tmp[1], tmp[0]));
    }
    close(fd);
    printf("\tseek time:  %f\n", get_secs_t(total[0]));
    printf("\twrite time: %f\n", get_secs_t(total[1]));
    printf("\tfsync time: %f\n", get_secs_t(total[2]));
}

void write_fsync(int size, unsigned char *buf, char *filename) {
    struct timeval tmp[2], total[3] = {{0, 0}, {0, 0}, {0, 0}};
    int fd = open(filename,  O_TRUNC | O_WRONLY | O_CREAT | O_SYNC); // O_ASYNC O_SYNC
    if (fd < 0) {
        perror("file open error");
        exit(1);
    }
    for (int i = 0; i < 1000; ++i) {
        gettimeofday(&tmp[0], NULL);
        lseek(fd, 0, SEEK_SET);
        gettimeofday(&tmp[1], NULL);
        total[0] = add_t(total[0], sub_t(tmp[1], tmp[0]));
        gettimeofday(&tmp[0], NULL);
        if (write(fd, buf, size) == -1) {
            perror("usr write fsync");
            exit(1);
        }
        gettimeofday(&tmp[1], NULL);
        total[1] = add_t(total[1], sub_t(tmp[1], tmp[0]));
        gettimeofday(&tmp[0], NULL);
        fsync(fd);
        gettimeofday(&tmp[1], NULL);
        total[2] = add_t(total[2], sub_t(tmp[1], tmp[0]));
    }
    close(fd);
    printf("\tseek time:  %f\n", get_secs_t(total[0]));
    printf("\twrite time: %f\n", get_secs_t(total[1]));
    printf("\tsync time:  %f\n", get_secs_t(total[2]));
}

void write_fdatasync(int size, unsigned char *buf, char *filename) {
    struct timeval tmp[2], total[3] = {{0, 0}, {0, 0}, {0, 0}};
    int fd = open(filename,  O_TRUNC | O_WRONLY | O_CREAT | O_DSYNC); // O_ASYNC O_DSYNC
    if (fd < 0) {
        perror("file open error");
        exit(1);
    }
    for (int i = 0; i < 1000; ++i) {
        gettimeofday(&tmp[0], NULL);
        lseek(fd, 0, SEEK_SET);
        gettimeofday(&tmp[1], NULL);
        total[0] = add_t(total[0], sub_t(tmp[1], tmp[0]));
        gettimeofday(&tmp[0], NULL);
        if (write(fd, buf, size) == -1) {
            perror("usr write fdatasync");
            exit(1);
        }
        gettimeofday(&tmp[1], NULL);
        total[1] = add_t(total[1], sub_t(tmp[1], tmp[0]));
        gettimeofday(&tmp[0], NULL);
        fdatasync(fd);
        gettimeofday(&tmp[1], NULL);
        total[2] = add_t(total[2], sub_t(tmp[1], tmp[0]));
    }
    close(fd);
    printf("\tseek time:  %f\n", get_secs_t(total[0]));
    printf("\twrite time: %f\n", get_secs_t(total[1]));
    printf("\tdsync time: %f\n", get_secs_t(total[2]));
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