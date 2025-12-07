#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#include "procedure.h"


int main() {

	int id_queue, id_shm;
	key_t chiave_shm = IPC_PRIVATE;// TODO
	key_t chiave_mbx = IPC_PRIVATE;// TODO

	id_shm = shmget(chiave_shm,sizeof(MonitorRisultati),IPC_CREAT|0664);// TODO
	id_queue = msgget(chiave_mbx,IPC_CREAT|0664);// TODO


	MonitorRisultati * ls = (MonitorRisultati*)shmat(id_shm,0,0);// TODO
	if(ls==(void*)-1){
		perror("Errore SHMAT");
		exit(1);
	}

	// TODO: inizializzare struttura
	ls->testa = 0;
	ls->coda = 0;
	ls->num_elementi = 0;
	init_monitor(&ls->m,2);
	
	pid_t pid;

	int j;
	for(j=0; j < N_STAMPA ; j++) {
		
		// TODO
		pid = fork();
		if(pid == 0){
			printf("[%d]Inizio Stampa\n",getpid());
			stampa(ls);
			exit(0);
		}
	}

	
	for(j=0; j < N_FILTRI ; j++) {
		
		// TODO
		pid = fork();
		if(pid == 0){
			printf("[%d]Inizio Filtro\n",getpid());
			filtro(ls,id_queue);
			exit(0);
		}
	}

	for(j=0; j < N_GENERATORI ; j++) {
	
		// TODO
		pid = fork();
		if(pid == 0){
			printf("[%d]Inizio Generatore\n",getpid());
			generazione(id_queue);
			exit(0);
		}
	}

	// TODO: attendere terminazione processi figli
	for(j=0;j<N_STAMPA+N_FILTRI+N_GENERATORI;j++){
		wait(NULL);
		printf("Processo %d ha terminato\n",j);
	}

	// TODO: deallocazione strutture
	shmctl(id_shm,IPC_RMID,0);
	msgctl(id_queue,IPC_RMID,0);

}




