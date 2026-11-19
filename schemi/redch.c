//...resto del codice
if (pid == 0) {
    close(STDOUT_FILENO);
    open(file, O_WRONLY | O_CREAT | O_TRUNC, S_RWIXU);
    execvp(argv[2], &argv[2]);
    _exit(127);
}
// resto del codice...
