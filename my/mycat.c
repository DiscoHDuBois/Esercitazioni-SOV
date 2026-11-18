#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 4096

static int copia(int in, int out) {
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(in, buf, BUF_SIZE)) > 0) {
        ssize_t off;
        while (off < n) {
            ssize_t w = write(out, buf + off, n - off);
            if (w < 0) {
                perror("write");
                return 1;
            }
            off += w;
        }
    }
    if (n < 0) {
        perror("read");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return copia(STDIN_FILENO, STDOUT_FILENO) < 0
                   ? 1
                   : 0; // nessun file -> leggo da STDIN
    }

    int esito = 0;
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            perror(argv[i]);
            esito = 1;
            continue;
        }
        if (copia(fd, STDOUT_FILENO) < 0) {
            esito = 1;
        }
        close(fd);
    }
    return esito;
}
