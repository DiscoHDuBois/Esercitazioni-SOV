//...resto del codice
long t = 0;
while ((r = read(pid, &status, WHOHANG)) == 0) {
    if (t >= limite_us) {
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0);
        break;
    }
    usleep(10000);
    t += 10000;
}

// resto del codice...
