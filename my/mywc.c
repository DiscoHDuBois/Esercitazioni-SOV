#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s FILE\n", argv[0]);
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        return 1;
    }
    char buf[BUF_SIZE];
    ssize_t n;
    long righe = 0;
    long byte = 0;
    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        byte += n;
        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                righe++;
            }
        }
    }
    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    close(fd);

    printf("%ld %ld %s\n", righe, byte, argv[1]);
    return 0;
}
