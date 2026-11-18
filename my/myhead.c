#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 4096

static int conta(int fd, const char *buf, ssize_t n) {
    ssize_t off = 0;
    while (off < n) {
        ssize_t w = write(fd, buf + off, n - off);
        if (w < 0) {
            perror("write");
            return -1;
        }
        off += w;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "USO: %s N FILE\n", argv[0]);
        return 1;
    }
    long max = atol(argv[1]);
    int fd = open(argv[2], O_RDONLY);
    if (fd < 0) {
        perror(argv[2]);
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t n;
    long righe = 0;
    while (righe < max && (n = read(fd, buf, BUF_SIZE)) > 0) {
        ssize_t fino = n;
        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\n' && ++righe == max) {
                fino = i + 1;
                break;
            }
        }
        if (conta(STDOUT_FILENO, buf, fino) < 0) {
            close(fd);
            return 1;
        }
    }
    close(fd);
    return 0;
}
