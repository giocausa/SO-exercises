#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "header.h"
#include "monitor_signal_continue.h"


void generatore_produttore(struct ProdConsGen *pc){
        
    /* TODO: utilizzare il costrutto monitor per la produzione del messaggio */
    enter_monitor(&pc->m);
    printf("inizio produzione-entrata nel monitor\n");
    while(pc->num_elem==DIM_QUEUE){
    printf("sospensione produzione\n");
    wait_condition(&pc->m,VARCOND_GEN_PRODUTTORI);
    printf("riattivazione produzione\n");
    }
    pc->msg[pc->testa].tipo=MSG_TYPE;

    for(int i=0;i<STRING_MAX_DIM-1;i++){
    pc->msg[pc->testa].stringa[i]='a'+(rand()%26);
    }

    pc->msg[pc->testa].stringa[STRING_MAX_DIM-1]='\0';

    for(int i=0;i<INT_MAX_DIM;i++){
    pc->msg[pc->testa].array[i]=rand()%10;//valore da 0 a 10
    }

    pc->msg[pc->testa].var=0;

    printf("[generatore_produttore] Messaggio generato!\n");
    printf("[generatore_produttore] ...............msg.stringa: %s\n", pc->msg[pc->testa].stringa);
    printf("[generatore_produttore] ...............msg.array[0]: %d\n",pc->msg[pc->testa].array[0]);
    printf("[generatore_produttore] ...............msg.array[1]: %d\n",pc->msg[pc->testa].array[1]);
    printf("[generatore_produttore] ...............msg.var: %d\n",pc->msg[pc->testa].var);
    pc->testa=(pc->testa+1)%DIM_QUEUE;
    pc->num_elem++;
    signal_condition(&pc->m,VARCOND_GEN_CONSUMATORI);
    leave_monitor(&(pc->m));
    /* TODO: ... */
}

void generatore_consumatore(struct ProdConsGen *pc, int ds_queue_gen_filter){

    /* TODO: utilizzare il costrutto monitor per la consumazione del messaggio e l'invio verso il processo filter */
    enter_monitor(&pc->m);
    while(pc->num_elem==0){
    printf("sospensione consumazione\n");
    wait_condition(&pc->m,VARCOND_GEN_CONSUMATORI);
    printf("riattivazione consumazione\n");
    }
    printf("[generatore_consumatore] Messaggio CONSUMATO!\n");
    printf("[generatore_consumatore] ...............msg.stringa: %s\n", pc->msg[pc->coda].stringa);
    printf("[generatore_consumatore] ...............msg.array[0]: %d\n", pc->msg[pc->coda].array[0]);
    printf("[generatore_consumatore] ...............msg.array[1]: %d\n", pc->msg[pc->coda].array[1]);
    printf("[generatore_consumatore] ...............msg.var: %d\n", pc->msg[pc->coda].var);
    
    int ret =msgsnd(ds_queue_gen_filter,&(pc->msg[pc->coda]), sizeof(message)-sizeof(long),0);
    
    if (ret<0){
            perror("msgsnd del messaggio on ds_queue_gen_filter FALLITA!");
            exit(-1);
    }
    pc->coda=(pc->coda+1)%DIM_QUEUE;
    pc->num_elem--;
    signal_condition(&pc->m,VARCOND_GEN_PRODUTTORI);
    printf("[generatore_consumatore] Messaggio INVIATO!\n");
    leave_monitor(&pc->m);
    /* TODO: ... */
}

