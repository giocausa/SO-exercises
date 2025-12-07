			/*----IMPLEMENTAZIONE DELLE PROCEDURE----*/
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "semafori.h"
#include <math.h>

void produci_tipo_1(BufferMutuaEx * buf, int semid, int value) {

	// TODO: completare produzione 1
   int indice=0;
	Wait_Sem(semid,SPAZIO_DISPONIBILE);
    Wait_Sem(semid,MUTEXP);
	while(indice<=DIM&&buf->stato[indice]!=LIBERO){
	indice++;
	}
	buf->stato[indice]=INUSO;
	Signal_Sem(semid,MUTEXP);
	buf->buffer[indice]=value;
	printf("[Produttore %d] ho prodotto %d di tipo 1, in posizione %d\n", getpid(), value, indice);
	buf->stato[indice]=OC1;
	Signal_Sem(semid,MESSAGGIO_DISPONIBILE);

}

void produci_tipo_2(BufferMutuaEx * buf, int semid, int value) {

	// TODO: completare produzione 2 

	 int indice=0;
	Wait_Sem(semid,SPAZIO_DISPONIBILE);
    Wait_Sem(semid,MUTEXP);
	while(indice<=DIM&&buf->stato[indice]!=LIBERO){
	indice++;
	}
	buf->stato[indice]=INUSO;
	Signal_Sem(semid,MUTEXP);
	buf->buffer[indice]=value;
	printf("[Produttore %d] ho prodotto %d di tipo 2, in posizione %d\n", getpid(), value, indice);
	buf->stato[indice]=OC2;
	Signal_Sem(semid,MESSAGGIO_DISPONIBILE);

}

void consuma_tipo_1(BufferMutuaEx * buf, int semid) {
int indice=0;
Wait_Sem(semid, MESSAGGIO_DISPONIBILE);
Wait_Sem(semid, MUTEXC);
while(indice<=DIM&&buf->stato[indice]!=OC1){
indice++;
}
buf->stato[indice]=INUSO;
Signal_Sem(semid, MUTEXC);
	// TODO: completare consumo 1
	printf("[Consumatore %d] consumo elemento %d di tipo 1 in posizione %d\n",getpid(), buf->buffer[indice], indice);	
	buf->stato[indice]=LIBERO;
    Signal_Sem(semid, SPAZIO_DISPONIBILE);
}

void consuma_tipo_2(BufferMutuaEx * buf, int semid) {
	int indice=0;
Wait_Sem(semid, MESSAGGIO_DISPONIBILE);
Wait_Sem(semid, MUTEXC);
while(indice<=DIM&&buf->stato[indice]!=OC2){
indice++;
}
buf->stato[indice]=INUSO;
Signal_Sem(semid, MUTEXC);
	// TODO: completare consumo 1
	
	printf("[Consumatore %d] consumo elemento %d di tipo 2 in posizione %d\n",getpid(),buf->buffer[indice], indice);	
	buf->stato[indice]=LIBERO;
    Signal_Sem(semid, SPAZIO_DISPONIBILE);
	// TODO: completare consumo 2

	}