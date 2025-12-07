#include "header.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>



void inizializza(struct monitor* m){

	m->molo=0;
	m->id_nave=0;
    
    m->num_lett=0;
    m->num_lett_wait=0;
    m->num_scritt_wait=0;
    m->num_scritt=0;
    pthread_mutex_init(&m->mutex,NULL);
    pthread_cond_init(&(m->ok_aggiorna),NULL);
    pthread_cond_init(&(m->ok_leggi),NULL);
    /* TODO: Inizializzare le variabili dell'algoritmo, il mutex, e le variabili condition */

}

void rimuovi (struct monitor* m){
    /* TODO: Disattivare mutex e variabili condition */
    pthread_mutex_destroy(&(m->mutex));
    pthread_cond_destroy(&(m->ok_aggiorna));
    pthread_cond_destroy(&(m->ok_leggi));
}


//SCRITTURA. AGGIORNAMENTO DELLA POSIZIONE DEL TRENO
void scrivi_molo(struct monitor* m, int molo){
    pthread_mutex_lock(&(m->mutex));
    printf("inizio scrittura treni\n");
    while(m->num_lett>0 ||m->num_scritt>0){
    printf("sospensione scrittura\n");
    m->num_scritt_wait++;
    pthread_cond_wait(&m->ok_aggiorna,&m->mutex );
    m->num_scritt_wait--;
    }
    m->num_scritt++;
    pthread_mutex_unlock(&m->mutex);
    m->molo=molo;
    pthread_mutex_lock(&m->mutex);
    m->num_scritt--;
    if(m->num_scritt_wait>0){
    pthread_cond_signal(&m->ok_aggiorna);
    printf("ci sono scrittori in attesa,do precedenza a loro per evitare la starvation\n");
    }else if(m->num_lett_wait>0){
    printf("non ci sono scrittori in attesa,libero tutti i lettori\n");
    pthread_cond_signal(&m->ok_leggi);
    }
    m->num_scritt--;
    pthread_mutex_unlock(&m->mutex);
    /* TODO: Implementare qui lo schema dei lettori-scrittori con starvation di entrambi.
     * nella parte della SCRITTURA
     */
}


//LETTURA. RESTITUISCE LA POSIZIONE DEL TRENO
int leggi_molo(struct monitor* m){
pthread_mutex_lock(&m->mutex);
printf("entrata nei binari-inizio lettura\n");
while(m->num_scritt>0){
printf("sospensione lettura\n");
m->num_lett_wait++;
pthread_cond_wait(&m->ok_leggi,&m->mutex);
m->num_lett_wait--;
printf("riattivazione lettura della posizione del treno\n");
}
m->num_lett++;
pthread_mutex_unlock(&m->mutex);

int molo=m->molo;
pthread_mutex_lock(&m->mutex);
m->num_lett--;
if(m->num_lett==0){
printf("rilascio gli scrittori in attesa\n");
pthread_cond_signal(&m->ok_aggiorna);
}

pthread_mutex_unlock(&m->mutex);
return molo;
    /* TODO: Implementare qui lo schema dei lettori-scrittori con starvation di entrambi.
     * nella parte della LETTURA
     */

}

