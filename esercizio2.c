
/*
Parte 2 del primo esercizio della prima esercitazione del corso di Sistemi Operativi e Virtualizzazione
                    --- TESTO DELL'ESERCIZIO ---
    Scrivere in C il programma mytimeout, versione semplificata del comando timeout
        ./mytimeout SECONDI COMANDO [ARGOMENTI...]
    mytimeout esegue COMANDO con i suoi argomenti. Se il comando termina entro SECONDI secondi, mytimeout termina subito dopo. 
    Se allo scadere il comando è ancora in esecuzione, mytimeout lo uccide con kill(), ne attende la terminazione e termina. 
    Prima di terminare, mytimeout stampa a video se la sua terminazione è dovuta al timeout o alla conclusione del comando. 
    Si possono usare solo le chiamate fork(), execvp(), waitpid(), kill() e usleep().
    Come COMANDO si usa il programma mysleep della prima parte. 
    Per esempio, 
    ./mytimeout 2 ./mysleep 5 termina dopo 2 secondi segnalando il timeout, 
    mentre ./mytimeout 5 ./mysleep 1 termina dopo 1 secondo segnalando la conclusione del comando.
*/

/*
Durante l'esercitazione è stato utile consultare il `man` delle varie syscall usate
*/

/*
    ========================================================
                    Come leggere il man
    ========================================================
    Una volta eseguito il comando bisogna guardare:
    1. SYNOPSIS -> serve per le librerie
    2. DESCRIPTION -> spiega il significato del parametro e i flag
    3. RETURN VALUE -> cosa ritorna in caso di successo e di errore
    4. ERRORS -> i valori possibili di errno quando ritorna -1
    BONUS -> NOTES/EXAMPLES: Sono casi particolari e piccoli esempi d'uso, utili a capire
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Nel man di fork, execvp, usleep è nella parte SYNOPSIS
#include <signal.h> // Nel man di kill, SIGKILL è nella parte SYNOPSIS
#include <sys/types.h>
#include <sys/wait.h> // Nel man di waitpid è nella parte SYNOPSIS

#define STEP_US 10000L //---Per il polling a 10ms ---- ESSENZIALE PER IL PADRE --> DEVE ASPETTARE DUE EVENTI

int main(int argc, char *argv[]){
    if(argc < 3) {
        fprintf(stderr, "UTILIZZO: %s SECONDI COMANDO [ARGOMENTI...]\n", argv[0]);
        return 1;
    }

    int sec = atoi(argv[1]);
    if (sec < 0) {
        fprintf(stderr, "SECONDI deve essere maggiore di 0\n");
        return 1;
    }


    /*
    Prendiamo il caso di fork dal man (man 2):
    SYNOPSIS:      #include <unistd.h>
                    pid_t fork(void);
    
    RETURN VALUE:  "On success, the PID of the child process is returned
                     in the parent, and 0 is returned in the child.
                      On failure, -1 is returned in the parent, no child
                      process is created [...]"
    
    Quindi, come si usa -> fork() ritorna 2 volte, una volta per processo.
    IL VALORE DI RITORNO È L'UNICO MODO PER SAPERE QUALE:
            ----------------------------------------------
            pid < 0 -> errore: Padre e nessun figlio
            pid == 0 -> è il figlio
            pid > 0 -> è il padre e pid è il PID del figlio
            -----------------------------------------------
    */

    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        return 1;
    }
    /* 
        Vediamo ora il caso del man di exec (man 3) 

        SYNOPSIS:      #include <unistd.h>
                        int execvp(const char *file, char *const argv[]);
        
        Le lettere nel nome dicono come si passano gli argomenti:
            v -> gli argomenti sono un vettore argv[]
                ("The array of pointers must be terminated by a null
                pointer"; per convenzione argv[0] è il nome del programma)
            p -> se "file" non contiene '/', il programma viene cercato
                nelle directory della variabile PATH (come fa la shell)
        
        RETURN VALUE: "The exec() functions return only if an error has
                        occurred. The return value is -1 [...]"
        
        Come si usa: se execvp ha successo, il codice del figlio viene
        sostituito dal nuovo programma, quindi le righe dopo non vengono
        mai eseguite. Tutto quello che segue execvp è gestione d'errore.
        
        nel nostror caso &argv[2] è già un vettore valido: argv[2] = "./mysleep",
        argv[3] = "5", argv[4] = NULL (argv termina sempre con NULL).
    */


    if (pid == 0) {
        execvp(argv[2], &argv[2]);
        perror("execvp");
        _exit(127); //Questa è una convenzione, significa comando non trovato
    }

    long total_us = (long)sec * 1000000L;
    long elapsed_us = 0;
    int status;
    pid_t r;

    /*
        Vediamo ora il caso di waitpid() (man 2)
    SYNOPSIS:      #include <sys/wait.h>
                     pid_t waitpid(pid_t pid, int *wstatus, int options);
    
      Parametro pid (DESCRIPTION):
          > 0  -> "wait for the child whose process ID is equal to [pid]"
           -1  -> "wait for any child process"
    
      Parametro options:
          0        -> si blocca finche' il figlio non cambia stato
          WNOHANG  -> "return immediately if no child has exited"
    
      RETURN VALUE:  "on success, returns the process ID of the child whose
                      state has changed; if WNOHANG was specified and one or
                      more child(ren) specified by pid exist, but have not
                      yet changed state, then 0 is returned.
                      On failure, -1 is returned."

        Il parametro wstatus è un intero e va letto con le macro, ecco quali:
        1. WIFEXITED(s) -> vero se il figlio è terminato normalmente
        2. WEXITSTATUS(s) -> valido solo se WIFEXITED è vera, il suo exit code
        3. WIFSIGNALED(s) -> vero se è terminato da un segnale
        4. WTERMSIG(s) -> valido solo se WIFISGNALED(s) è vera, quale segnale
    Come usarlo:
        in questo esercizio con WNOHANG la chiamata non blocca mai, quindi il ciclo può controllare il figlio e il tempo a ogni passo (polling).
        Il ciclo continua finchè waitpid torna a 0

    */

    /*
    Vediamo anche kill() (man 2)
        SYNOPSIS:      #include <signal.h>
                            int kill(pid_t pid, int sig);
            
            DESCRIPTION:   "If pid is positive, then signal sig is sent
                            to the process with the ID specified by pid."
            
            RETURN VALUE:  0 in caso di successo, -1 in caso di errore.

        Cosa fa: Invia un segnale, è il segnale scelto a decidere l'effetto (è possibile leggerli in man 7 signal).
        SIGKILL non può essere nè catturato nè ignorato -> termina il processo
    */

    while((r = waitpid(pid, &status, WNOHANG)) == 0) {
        if(elapsed_us >= total_us) {
            kill(pid, SIGKILL); 
            waitpid(pid, &status, 0);

            /*
            attende la morte del figlio e lo raccoglie liberando la voce nella tabella dei processi (sennò sarebbe zombie)
            */

            if(WIFSIGNALED(status) && (WTERMSIG(status) == SIGKILL)) {
                printf("my timeout: TIMEOUT dopo %d secondi, comando killato\n", sec);
                return 124;
            }
            break;
        }


        /*
        Vediamo usleep() (man 3)
        SYNOPSIS:      #include <unistd.h>
                        int usleep(useconds_t usec);
        
        Sospende il processo per (almeno) usec MICROsecondi:
            1 s = 1 000 000 us,   10 ms = 10 000 us
        
        RETURN VALUE:  0 in caso di successo, -1 in caso di errore.
        
        ERRORS:        EINVAL "usec is greater than or equal to 1000000.
                        (On systems where that is considered an error.)"

        
        */
        usleep(STEP_US);
        elapsed_us += STEP_US;
    }
    if(r < 0) {
        perror("waitpid");
        return 1;
    }
    printf("mytimeout: comando concluso entro %d sec\n", sec);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}