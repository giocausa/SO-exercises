#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

	int key_coda = ftok(".",'c');// TODO
    int id_coda_sensore = msgget(key_coda,IPC_CREAT|0664);// TODO

    if(id_coda_sensore < 0) {
        perror("Errore msgget");
        exit(1);
    }

    pid_t pid;

    pid = fork();

    if(pid == 0) {
        // TODO: processo figlio sensore (eseguibile distinto)
        execl("./sensore","./sensore",NULL);
        exit(0);
    }

    pid = fork();

    if(pid == 0) {
       // TODO: processo figlio smistatore (eseguibile distinto)
       execl("./smistatore","./smistatore",NULL);
        exit(0);
    }


    // TODO: attendere completamento dei figli
    wait(NULL);
    wait(NULL);
    // TODO: rimuovere la coda
    msgctl(id_coda_sensore,IPC_RMID,0);
    return 0;

}
