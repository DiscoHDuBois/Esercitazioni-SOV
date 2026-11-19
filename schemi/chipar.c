// resto del codice...
for (int i = 0; i < n; i++) {
    pids[i] = fork();
    if (pids[i] == 0) {
        /* exec o lavoro */
        _exit(0);
    }
}
while ((r = waitpid(-1, &status, 0)) > 0) {
    /* r = PID di chi ha finito */
}

// resto del codice...
