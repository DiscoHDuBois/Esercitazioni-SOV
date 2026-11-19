# Schemi per costruzione con System Calls

## descrizione file
| File | Cos'è |
|---|---|
| `ciclocopia.c` | Ciclo di copia usato in mycp, mycat, myhead. Varia solo da dove si legge il file, dove si scrive e dove ci si ferma |
| `chipar.c` | Prima tutte le fork, poi le attese waitpid(-1) restituisce il prossimo figlio che termina, con kill si fermano gli altri |
| `chifa.c` | Padre e Figlio non condividono variabili, restano due canali: exit status(0-255) o un file aperto dopo la waitpid |
| `exnwa.c` | La base di tutto ciò che lancia i programmi, figlio diventa comando, il padre ne legge l'esito |
| `redch.c` | Tra fork ed exec il figlio prepara i descrittori: per stdin close (0) + O_RDONLY, per >> O_APPEND |
| `wasc.c`  | Padre aspetta due eventi: Fine del figlio o scadere del tempo. Polling a passi di 10ms |


