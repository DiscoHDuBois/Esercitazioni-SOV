//...resto del codice
pid_t rc = fork();
if (rc < 0) {
    perror("fork failed");
    return 1;
}
if (rc == 0) {
    execvp(argv[1], &argv[1]);
    perror("execvp");
    _exit(127);
}

int status;
waitpid(pid, &status, 0);
if (WIFEXITED(status)) {
    printf("codice: %d\n", WIFEXITED(status));
}
// resto del codice...
