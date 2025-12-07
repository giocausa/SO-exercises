
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

typedef struct {
    int value;
    //TODO: aggiunta delle variabili necessarie alla sincronizzazione
    // con lettori-scrittori con staravtion di entrambi
    pthread_mutex_t mutex;
    pthread_cond_t ok_scrivi;
    pthread_cond_t ok_leggi;
    int num_scrittori;
    int num_lettori;
    int num_scrittori_wait;
    int num_lettori_wait;
} data_str;

typedef struct {
    int queue_res;
    int sender;
   //TODO: struttura
  data_str* d;
} reader_param;

int read_data(data_str* data) {
    int value;
    pthread_mutex_lock(&(data->mutex));

    while(data->num_scrittori>0){
        data->num_lettori_wait++;
    pthread_cond_wait(&(data->ok_leggi),&(data->mutex));
    data->num_lettori_wait--;
    }
    data->num_lettori++;

    pthread_mutex_unlock(&(data->mutex));

    //TODO: lettura valore da "data" attraverso
    // lettori-scrittori con starvation di entrambi
    value=data->value;
    
pthread_mutex_lock(&(data->mutex));
data->num_lettori--;

if(data->num_lettori==0){
pthread_cond_signal(&(data->ok_scrivi));
}
pthread_mutex_unlock(&(data->mutex));
    return value;
}

void write_data(data_str* data, int new_value) {
    //TODO: scrittura "new_value" in "data" attraverso
    // lettori-scrittori con starvation di entrambi
   pthread_mutex_lock(&(data->mutex));
   while(data->num_lettori>0||data->num_scrittori>0){
    data->num_scrittori_wait++;
    pthread_cond_wait(&(data->ok_scrivi),&(data->mutex));
    data->num_scrittori_wait--;
   }
    data->num_scrittori++;
    pthread_mutex_unlock(&(data->mutex));
    data->value=new_value;
    pthread_mutex_lock(&(data->mutex));
    data->num_scrittori--;
    if(data->num_scrittori_wait>0){
        pthread_cond_signal(&(data->ok_scrivi));
    }else{
    pthread_cond_broadcast(&(data->ok_leggi));
    }
    pthread_mutex_unlock(&(data->mutex));
   
}

void * updater(void* par) {
    data_str* data = (data_str*) par;
    int new_value;
    int i;
    for (i = 0; i < NUM_UPDATES; i++) {
        new_value = rand()%10+1;
        printf("UPDATER: aggiornamento dato al valore: %d\n",new_value);
        write_data(data,new_value);
        sleep(rand()%3+1);
    }
    pthread_exit(NULL);
}

void * reader(void* par) {
    reader_param* p =(reader_param*)par;//TODO: ricostruzione parametro

    res risp;
    //TODO: Costruzione risposta con prelievo valore dalla funzione read_data
    risp.value=read_data(p->d);
    risp.sender=p->sender;
    printf("READER_SERV: invio valore al client %d\n",risp.value);
    //TODO: Invio risposta al reader client
    msgsnd(p->queue_res,&risp, sizeof(res)-sizeof(long),0);
    pthread_exit(NULL);
}

int main() {

    int pid;
    key_t msg_req_key =ftok(".",'a'); //TODO: definire chiave
	key_t msg_res_key =ftok(".",'b');//TODO: definire chiave

    //TODO: INIZIALIZZAZIONE CODE
    int queue_req=msgget(msg_req_key,IPC_CREAT|0664);
	int queue_res=msgget(msg_res_key,IPC_CREAT|0664);
    data_str* d =malloc(sizeof(data_str)); //TODO: inizializzazione struttura condivisa tra i thread
   
    //TODO: inizializzazione mutex e condition variables
    pthread_mutex_init(&(d->mutex), NULL);
    pthread_cond_init(&(d->ok_leggi),NULL);
     pthread_cond_init(&(d->ok_scrivi),NULL);
     pthread_attr_t attr;
     pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
     d->num_lettori=0;
     d->num_lettori_wait=0;
     d->num_scrittori_wait=0;
     d->num_scrittori=0;
    
    pthread_t upd, rd[NUM_READS*NUM_READERS];

    reader_param p[NUM_READS*NUM_READERS];

   

    srand(time(NULL));
    
    //TODO: creazione thread updater
     pthread_create(&upd,&attr,updater, (void *)d);
    req msg;
    int i;
    for (i = 0; i < NUM_READS*NUM_READERS; i++) {
        //TODO: ricezione messaggio di richiesta
       msgrcv(queue_req,&msg,sizeof(msg)-sizeof(long), 1, 0);
        printf("READER:SERV: ricevuta richiesta di lettura da: %d\n",msg.pid);
        //TODO: preparazione parametro p[i]
       p[i].queue_res=queue_res;
       p[i].sender=msg.pid;
        p[i].d=d;
        //TODO: creazione thread reader
      pthread_create(&rd[i],&attr, reader, (void *)&p[i]);
    }
    //TODO: join dei thread
    for (i = 0; i < NUM_READS*NUM_READERS; i++) {
    pthread_join(rd[i],NULL);
    }
    pthread_join(upd, NULL);
   free(d);
   msgctl(queue_req, IPC_RMID,0);
     msgctl(queue_res, IPC_RMID,0);
    //TODO: deallocazione memoria e rimozione coda
   
    return 0;
}
