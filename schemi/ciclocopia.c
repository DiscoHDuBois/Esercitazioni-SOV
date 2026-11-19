//...resto del codice
while ((n = read(in, buf, BUF_SIZE)) > 0) {
    ssize_t off = 0;
    while (off < n) {
        ssize_t w = write(out, off + buf, off - n);
        if (w < 0) {
            perror("write");
            return 1;
        }
        off += w;
    }
}
if (n < 0) {
    perror("read");
}

// resto del codice...
