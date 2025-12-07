#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "header.h"

CodaCircolare coda;

int main() {
    srand(time(NULL));
    CodaCircolare*coda=malloc(sizeof(CodaCircolare));
    BufferBabbo* buf=malloc(sizeof(BufferBabbo));
    inizializza_coda(coda);
    inizializza_buffer(buf);
    /*
        TODO: allocazione buffer e inizializzazione strutture
    */   

    /*
        TODO: creazione thread produttori, consumatori e Babbo_Natale
    */
    pthread_t prod[NPROD],cons[NCONS],babbo;
    for(int i=0;i<NPROD;i++){
    pthread_create(&prod[i], 0, Produttore, (void *)coda);
    }
    for(int i=0;i<NCONS;i++){
    pthread_create(&cons[i], 0, Consumatore, (void *)buf);
    }
    pthread_create(&babbo, 0, Babbo_Natale, (void *)buf);
     

      for(int i=0;i<NPROD;i++){
      pthread_join(prod[i], NULL);

      }

       for(int i=0;i<NCONS;i++){
       pthread_join(cons[i], NULL);
       }
       pthread_join(babbo, NULL);

       free(coda);
       free(buf);
       distruggi_coda(coda);
       distruggi_buffer(buf);
    /*
        TODO: attesa terminazione thread produttori, consumatori e Babbo_Natale
    */

    /*
        TODO: rimozione strutture
    */

    return 0;
}

void* Produttore(void* p) {
    for (int i = 0; i < GIOCATTOLI_PER_PROD; i++) {
        sleep(2);
        int giocattolo = rand() % 100 + 1;
        produci(&coda,  giocattolo);
        /*
            TODO: produzione e terminazione
        */
    }
pthread_exit(NULL);
}

// Thread consumatore
void* Consumatore(void* p) {
 BufferBabbo* buf=(BufferBabbo*)p;

    for (int i=0; i<PRELIEVI_PER_CONS; i++) {
        /*
            TODO: consumo, aggiornamento e terminazione
        */
        consuma(&coda);
        aggiorna_conteggio(buf);
    }
pthread_exit(NULL);

}

// Thread Babbo Natale
void* Babbo_Natale(void* p) {
 BufferBabbo* buf=(BufferBabbo*)p;
    for (int i=0; i<CONSEGNE; i++) {
        /*
            TODO: consegna e terminazione
        */
        consegna(buf);
    }

pthread_exit(NULL);
}