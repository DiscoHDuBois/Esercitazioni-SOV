# Esercizi di programmazione di sistema in C

Tre piccoli programmi C che riproducono versioni semplificate di comandi Unix, usando direttamente le system call POSIX.

| File | Cosa fa | Chiamate consentite |
|---|---|---|
| `mysleep.c` | Dorme per N secondi (comando di prova) | — |
| `mytimeout.c` | Esegue un comando con un limite di tempo | `fork`, `execvp`, `waitpid`, `kill`, `usleep` |
| `mycp.c` | Copia un file in un altro a blocchi | `open`, `read`, `write`, `close` |

I sorgenti sono commentati in dettaglio. Prima di ogni system call, il commento riporta la sinossi, il valore di ritorno citato dalla pagina `man` (man-pages 6.7, Linux) e il modo in cui quel valore viene usato nel codice.

---

## Compilazione

```sh
gcc esercizio1.c -o mysleep
gcc esercizio2.c -o mytimeout
gcc esercizio3.c -o mycp
```

---

## Consultare le pagine man utili allo svolgimento

```sh
man 2 fork   man 3 exec    man 2 waitpid   man 2 kill   man 3 usleep
man 2 open   man 2 read    man 2 write     man 2 close  man 7 signal
```

Il numero indica la sezione:

- **2**: system call;
- **3**: funzioni della libreria C (`execvp` e `usleep` stanno qui);
- **7**: panoramiche, come l'elenco dei segnali.

Se le pagine di sezione 2 e 3 mancano, vanno installate... `man-pages` su Arch, `manpages-dev` su Debian e Ubuntu.
