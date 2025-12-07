#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>

void * thread_lettore(void *);
void * thread_scrittore(void *);

// Variabile globale
int id_coda_sensore;

int lettura(MonitorLS * m) {

    /* 
        NOTA: C'è un unico scrittore
    */
    pthread_mutex_lock(&m->mutex);
    while(m->scrittore==1){
        pthread_cond_wait(&m->cv_lettori,&m->mutex);
    }
    m->num_lettori++;

    pthread_cond_signal(&m->cv_lettori); 

    pthread_mutex_unlock(&m->mutex);
    int val;
    val=m->valore;
    // TODO: completare lettura
    pthread_mutex_lock(&m->mutex);
    m->num_lettori--;
    if(m->num_lettori==0) pthread_cond_signal(&m->cv_scrittori);
    pthread_mutex_unlock(&m->mutex);
    return val;
}

void scrittura(MonitorLS * m, int valore) {
    
    /* 
        NOTA: C'è un unico scrittore
    */
    pthread_mutex_lock(&m->mutex);
    while(m->num_lettori>0){
        pthread_cond_wait(&m->cv_scrittori,&m->mutex);
    }
    m->scrittore=1;
    pthread_mutex_unlock(&m->mutex);

    m->valore=valore;

    // TODO: completare scrittura
    pthread_mutex_lock(&m->mutex);
    m->scrittore=0;
    pthread_cond_broadcast(&m->cv_lettori);
    pthread_mutex_unlock(&m->mutex);
}

void main() {

	srand(time(NULL));

	int key_coda = ftok(".",'c');// TODO
    id_coda_sensore = msgget(key_coda,IPC_CREAT|0664);// TODO

    printf("Avvio processo smistatore...\n");

    pthread_t scrittore;
    pthread_t lettori[LETTORI];

    MonitorLS * m = malloc(sizeof(MonitorLS));// TODO

    // TODO: inizializzare variabili per la mutua esclusione
    pthread_mutex_init(&m->mutex,NULL);
    pthread_cond_init(&m->cv_scrittori,NULL);
    pthread_cond_init(&m->cv_lettori,NULL);
    m->num_lettori=0;
    // TODO: creare thread scrittore
    pthread_create(&scrittore,NULL,thread_scrittore,(void *)m);
    for(int i=0; i<3; i++) {
    	// TODO: creare thread lettori
        pthread_create(&lettori[i],NULL,thread_lettore,(void *)m);
    }

    // TODO: attendere la terminazione dei thread
    pthread_join(scrittore,NULL);
	// TODO: deallocare la struttura
    for(int i=0; i<3; i++) {
        pthread_join(lettori[i],NULL);
    }
}

void * thread_lettore(void * x) {

    MonitorLS * ls = (MonitorLS*)x;// TODO: recupero parametri
    int valore_old,valore;
    for(int i=0; i<10; i++) {

        sleep(1);

        // TODO: leggere il valore, moltiplicarlo per un intero tra 1 e 100 e stamparlo a video
        valore_old=lettura(ls);
        valore=(valore_old)*(rand() % 101);
        printf("Lettore: Stampo valore originale = %d e valore finale = %d\n", valore_old, valore);
    }

    pthread_exit(NULL);
}

void * thread_scrittore(void * x) {

    MonitorLS * ls = (MonitorLS*)x; // TODO: recupero parametri
    Messaggio msg;
    for(int i=0; i<10; i++) {

        printf("Smistatore: In attesa di messaggi...\n");

        // TODO: ricezione messaggio
        msgrcv(id_coda_sensore,&msg,sizeof(Messaggio)-sizeof(long),0,0);
        printf("Scrittore: Ricevuto valore = %d\n", msg.valore);

        // TODO: scrivere il messaggio nel monitor
        scrittura(ls,msg.valore);
    }

    pthread_exit(NULL);
}
