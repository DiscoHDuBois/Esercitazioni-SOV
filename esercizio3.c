/*
Secondo esercizio della prima esercitazione del corso di Sistemi Operativi e Virtualizzazione
                    --- TESTO DELL'ESERCIZIO ---
        Scrivere in C il programma mycp, che copia un file in un altro usando solo le chiamate di sistema open(), read(), write() e close():
        SINTASSI/mycp ORIGINE DESTINAZIONE
        Se la destinazione non esiste va creata con permessi 0644; se esiste va sovrascritta. 
        La copia avviene a blocchi, attraverso un buffer di 4096 byte.
*/

/*
Durante l'esercitazione è stato utile consultare il `man` delle varie syscall usate
*/

/*
In questo esercizio si fa uso del FILE DESCRIPTOR, è un intero per identificare il file aperto
Le chiamate read, write, close lavorano su quell'intero NON SUL NOME DEL FILE
*/



#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> // read, write, close nel SYNOPSIS
#include <fcntl.h> // open, O_RDONLY, O_WRONLY nel SYNOPSIS
#define BUF_SIZE 4096


int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "USO: %s ORIGINE DESTINAZIONE\n", argv[0]);
        return 1;
    }

    /*
    Vediamo open() (man 2)
    SYNOPSIS:      #include <fcntl.h>
                    int open(const char *pathname, int flags, + terzo argomento opzionale)
                        [mode_t mode] );
    
    Parametro flags (DESCRIPTION): "must include one of the following
    access modes: O_RDONLY, O_WRONLY, or O_RDWR", a cui si possono
    aggiungere altri flag con l'OR bit a bit ( | ).
    
    RETURN VALUE:  "return the new file descriptor (a nonnegative
                    integer). On error, -1 is returned [...]"
    */

    int src = open(argv[1], O_RDONLY);
    if(src < 0) {
        perror(argv[1]); //classico no such file or directory
        return 1;
    }

    /*
    Nell'esercizio ogni flag corrisponde a un punto da risolvere dell'esercizio (descrizioni prese dal man):
        - O_WRONLY: apertura in sola scrittuta
        - O_CREAT: il terzo argomento (quello descritto prima come opzionale) diventa obbligatorio
        - O_TRUNC: realizza la sovrascrittura, se copiassi un file corto sopra uno lungo resterebbe la parte avanzata del vecchio
        - mode 0644: permessi
    */

    int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(dst < 0) {
        perror(argv[2]);
        close(src);
        return 1;
    }
    char buf[BUF_SIZE];
    ssize_t n; //Intero con segno

    /*
    Vediamo read() (man 2)
    SYNOPSIS:      #include <unistd.h>
                    ssize_t read(int fd, void buf[.count], size_t count);
    
    Legge al massimo count byte dal descrittore fd e li mette in buf.
    
    RETURN VALUE:  "the number of bytes read is returned (zero indicates
                    end of file), and the file position is advanced by
                    this number. It is not an error if this number is
                    smaller than the number of bytes requested [...]"
                    "On error, -1 is returned"
    Come usarlo: i casi del valore di ritorno stabiliscono l'andamento del ciclo:
    1. n > 0: ho n bit validi in buf
    2. n == 0: fine, fine del ciclo
    3. n < 0: errore, esco dal ciclo e lo segnalo dopo
    */

    /*
    Vediamo ora write() (man 2)
    SYNOPSIS:      #include <unistd.h>
                        ssize_t write(int fd, const void buf[.count],
                            size_t count);
        
    RETURN VALUE: "the number of bytes written is returned. On error,
                    -1 is returned [...] Note that a successful
                    write() may transfer fewer than count bytes."
                    "In the event of a partial write, the caller can
                    make another write() call to transfer the
                    remaining bytes."
    Come usarlo: si scrivono n byte, il ciclo interno riparte da buf + off con i byte rimanenti per gestire scritture parziali(n - off)   
    */

    /*
    Vediamo poi close() (man 2)
    SYNOPSIS:      #include <unistd.h>
                    int close(int fd);
    
    RETURN VALUE:  "returns zero on success. On error, -1 is returned"
    Libera il file descriptor (il numero può essere riutilizzato in una open successiva)
    il ritorno di close è bene conntrollarlo sul file SCRITTO, alcuni errori posso essere segnalati più tardi
    */


    while ((n = read(src, buf, BUF_SIZE)) > 0) {
        ssize_t off = 0;
        while (off < n) {
            ssize_t w = write(dst, buf + off, n - off);
            if (w < 0) {
                perror("write");
                close(src);
                close(dst);
                return 1;
            }
            off += w;
        }
    }

    if (n < 0) {
        perror("read");
    }

    close(src);
    if (close(dst) < 0) {
        perror("close");
        return 1;
    }

    return n < 0 ? 1 : 0;

}