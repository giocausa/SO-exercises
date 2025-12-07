#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

void inizializza_coda(CodaCircolare *coda) {
    /*
        TODO: completare
    */
    coda->count=coda->coda=coda->testa=0;
    pthread_mutex_init(&(coda->mutex), NULL);
    pthread_cond_init(&(coda->ok_consuma), NULL);
     pthread_cond_init(&(coda->ok_produci), NULL);
}

void distruggi_coda(CodaCircolare *coda) {
    /*
        TODO: completare
    */
    pthread_mutex_destroy(&(coda->mutex));
    pthread_cond_destroy(&(coda->ok_produci));
    pthread_cond_destroy(&(coda->ok_consuma));
}


void produci(CodaCircolare *coda, int giocattolo) {
    /*
        TODO: produzione
    */
    pthread_mutex_lock(&(coda->mutex));
    printf("PRODUTTORE:inizio produzione\n");
    while(coda->count==DIM){
    pthread_cond_wait(&(coda->ok_produci), &(coda->mutex));
    }
    

    coda->buffer[coda->coda]=giocattolo;
    coda->coda=(coda->coda+1)%DIM;
    coda->count++;
    printf("Prodotto giocattolo: %d (count=%d)\n", giocattolo, coda->count);
    
    pthread_cond_signal(&(coda->ok_consuma));
    pthread_mutex_unlock(&(coda->mutex));
}

void consuma(CodaCircolare *coda) {
    pthread_mutex_lock(&(coda->mutex));
    printf("CONSUMATORE:inizio consumazione\n");
    while(coda->count<3){
    pthread_cond_wait(&(coda->ok_consuma),&(coda->mutex));
    }
    for(int i=0;i<3;i++){
    int giocattolo=coda->buffer[coda->testa];
    coda->testa=(coda->testa)%DIM;
    coda->count--;
    printf("Prelevato giocattolo: %d (count=%d)\n", giocattolo, coda->count);
    }
    pthread_cond_signal(&(coda->ok_produci));
    pthread_mutex_unlock(&(coda->mutex));
}

void inizializza_buffer(BufferBabbo *buffer) {
    /*
        TODO: completare
    */
    buffer->consegne_effettuate=0;
    buffer->regali_completati=0;
    buffer->buffer_regali=0;
    pthread_mutex_init(&(buffer->mutex),NULL);
    pthread_cond_init(&(buffer->ok_babbo),NULL);
}

void distruggi_buffer(BufferBabbo *buffer) {
    /*
        TODO: completare
    */
    pthread_cond_destroy(&(buffer->ok_babbo));
    pthread_mutex_destroy(&(buffer->mutex));
}

void aggiorna_conteggio(BufferBabbo *buffer) {
    /*
        TODO: incremento buffer una volta consumati 3 giocattoli
    */
    pthread_mutex_lock(&(buffer->mutex));

    buffer->buffer_regali++;
    printf("Regalo pronto: %d\n", buffer->buffer_regali);
    pthread_cond_signal(&(buffer->ok_babbo));
    pthread_mutex_unlock(&(buffer->mutex));
    
}

void consegna(BufferBabbo *buffer) {

    /*
        TODO: consegna dei regali una volta impacchettati 5 regali
    */
    pthread_mutex_lock(&(buffer->mutex));
    while(buffer->buffer_regali<5){
    pthread_cond_wait(&(buffer->ok_babbo),&(buffer->mutex));
    }
    buffer->buffer_regali -= 5;
    buffer->consegne_effettuate++;
    printf("Babbo Natale consegna effettuata #%d\n", buffer->consegne_effettuate);
    pthread_mutex_unlock(&(buffer->mutex));
}
