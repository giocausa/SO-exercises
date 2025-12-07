#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

void main() {

    printf("Avvio processo sensore...\n");
    
    int key_coda = ftok(".",'c');// TODO
    int id_coda_sensore = msgget(key_coda,IPC_CREAT|0664);// TODO

    srand(time(NULL));
    int val;
    Messaggio m;
    for(int i=0; i<10; i++) {

        // TODO: inviare interi casuali sulla coda
        val=rand()%11;
        m.type=TIPO;
        m.valore=val;
        msgsnd(id_coda_sensore,&m,sizeof(Messaggio)-sizeof(long),0);
        sleep(1);
    }

}
