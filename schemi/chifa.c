//...resto del codice
if (pid == 0) {
    int k = cont(argv[1]);
    _exit(k > 254 ? 254 : k);
}
waitpid(pid, &status, 0);
int k = WEXITSTATUS(status);
// resto del codice...
