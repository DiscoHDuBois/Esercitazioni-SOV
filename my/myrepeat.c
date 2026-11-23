#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "USO: %s N MILLISECONDI COMANDO [ARGOMENTI...]\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    long ms = atol(argv[2]);
    int falliti = 0;

    for (int i = 0; i < n; i++) {
        fflush(stdout); /* evita che il buffer di printf venga copiato */
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            execvp(argv[3], &argv[3]);
            perror("execvp");
            _exit(127);
        }
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            falliti++;
        printf("--- esecuzione %d/%d terminata ---\n", i + 1, n);

        for (long resto = ms; resto > 0; resto -= 500) /* usleep < 1 s per volta */
            usleep((resto >= 500 ? 500 : resto) * 1000);
    }
    printf("myrepeat: %d esecuzioni fallite su %d\n", falliti, n);
    return falliti ? 1 : 0;
}
