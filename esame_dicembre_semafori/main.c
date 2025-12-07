#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#include "header.h"


int main(){
	pid_t pid_ex,pid;
	int st,i,value;
	int shmid,semid;
     BufferMutuaEx* buf;
	key_t key_shm,key_sem;
	key_shm =IPC_PRIVATE; // TODO 
	key_sem =IPC_PRIVATE; // TODO 
     
	shmid =shmget(key_shm,sizeof( BufferMutuaEx),IPC_CREAT|0664); // TODO 
	semid =semget(key_sem,4,IPC_CREAT|0664); // TODO 
	if(shmid<=0){perror("shered fallita!");exit(1);}
	printf("[MASTER] - Shmid : %d, Semid: %d\n",shmid,semid);
    if(semid<0){perror("semaforo fallito");exit(1);}
	
	// TODO: inizializzare semafori
     
    semctl(semid,MUTEXP, SETVAL,1);
	semctl(semid,MUTEXC, SETVAL,1);
	semctl(semid,SPAZIO_DISPONIBILE, SETVAL,DIM);
	semctl(semid,MESSAGGIO_DISPONIBILE, SETVAL,0);


    


	buf =(BufferMutuaEx*)shmat(shmid,NULL, 0); // TODO 

if(buf==(void*)-1){
perror("attacht a sto cazz");
exit(1);
}
	 for (i = 0; i < DIM; i++) {
        buf->stato[i] = LIBERO;
    }
	// TODO: inizializzazione della struttura
	
	srand(time(NULL));
    
	for(i = 0; i < NPROD; i++){
		pid = fork();
		if(pid==0){
	for(int j=0;j<PRODS;j++){
		value = rand() % 100;
		if(i%2==0){
		produci_tipo_1(buf, semid,value);
		
	
		// TODO: ogni figlio produttore (1 e 2) fa PRODS produzioni
		}else{
		produci_tipo_2(buf, semid,value);
		
		}
	}
	exit(0);
		}else if(pid<0){
		perror("processo fallito\n");
		exit(1);
		}
	
	}	
		
	
		
		
	

	for(i = 0; i < NCONS; i++){
		pid_ex = fork();
	
	if(pid_ex==0){
	for(int j=0;j<CONS;j++){
	if(i%2==0){
    consuma_tipo_1(buf, semid);
	
	
	}else{
	consuma_tipo_2(buf,  semid);
	
	}
	}
	exit(0);
	}else if(pid_ex<0){
	perror("processo fallito\n");
	exit(1);
	}
	}
		// TODO: ogni figlio consumatore (1 e 2) fa CONS consumi
	for(i = 0; i < NPROD; i++){
	wait(&st);
	printf("figlio produttore n.ro %d morto con status%d\n",i+1,st>>8);
	}
	for(i = 0; i < NCONS; i++){
	wait(&st);
	printf("figlio produttore n.ro %d morto con status%d\n",i+1,st>>8);
	}
	// TODO: attesa completamento figli
    shmctl(shmid, IPC_RMID,NULL);
    semctl(semid, 0,IPC_RMID);
	// TODO: rimozione strutture 

	return 0;
}

