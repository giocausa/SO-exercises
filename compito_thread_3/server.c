
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

typedef struct {
    int buffer[MAX_VALUES];
    int testa;
    int coda;
    int nelem;
    pthread_mutex_t mutex;
    pthread_cond_t ok_produci;
    pthread_cond_t ok_consuma;
    // TODO aggiungere le variabili per la gestione circolare
    // e per la sincronizzazione dei thread
} data_str;

typedef struct {
    int somma;
    int num_valori_sommati;
    pthread_cond_t ok_somma;
    pthread_mutex_t mutex;
    // TODO aggiungere le variabili per la sincronizzazione dei thread
} somma_valori;

// VARIABILI GLOBALI:
// la struttura
data_str shared_buf;
// le code
int queue_req;
int queue_res;

int consuma() {
    int value;
    pthread_mutex_lock(&shared_buf.mutex);
    printf("CONSUMATORE:inizio consumazione\n");
    while(shared_buf.nelem==0){
        printf("CONSUMATORE:sospensione consumazione\n");
    pthread_cond_wait(&shared_buf.ok_consuma,&shared_buf.mutex);
    }
    value=shared_buf.buffer[shared_buf.testa];
    shared_buf.testa=(shared_buf.testa+1)%MAX_VALUES;
    shared_buf.nelem--;
    pthread_cond_signal(&shared_buf.ok_produci);
    pthread_mutex_unlock(&shared_buf.mutex);
    //TODO consumatore, con gestione coda circolare

    return value;
}

void produci(int new_value) {
pthread_mutex_lock(&shared_buf.mutex);
printf("PRODUTTORE:inizio produzione\n");
while(shared_buf.nelem==MAX_VALUES){
printf("PRODUTTORE:sospensione produzione\n");
pthread_cond_wait(&shared_buf.ok_produci,&shared_buf.mutex);
printf("PRODUTTORE:riattivazione produzione\n");
}
shared_buf.buffer[shared_buf.coda]=new_value;
shared_buf.coda=(shared_buf.coda+1)%MAX_VALUES;
shared_buf.nelem++;
pthread_cond_signal(&shared_buf.ok_consuma);
pthread_mutex_unlock(&shared_buf.mutex);
    //TODO produttore, con gestione coda circolare
}


void * stampa_somma(void* par) {
    somma_valori *s = (somma_valori*)par;  // Recupero del parametro passato al thread

    pthread_mutex_lock(&(s->mutex)); // Acquisisco il mutex per l'accesso alla struttura condivisa

    while (s->num_valori_sommati < NUM_CONS) {
        printf("STAMPA_SOMMA: in attesa di dodici valori...\n");
        pthread_cond_wait(&s->ok_somma, &s->mutex); // Attesa condizione: somma completata
    }

    // Una volta verificata la condizione, stampo la somma
    printf("STAMPA_SOMMA: La somma dei valori è: %d\n", s->somma);

    pthread_mutex_unlock(&(s->mutex)); // Rilascio il mutex
    pthread_exit(NULL); // Termine del thread
}

void * produttore(void* par) {
    // par qui non serve, può essere ignorato
    int new_value;
    int i;
    for (i = 0; i < NUM_UPDATES*MAX_VALUES; i++) {
        new_value = rand()%10+1;
        printf("PRODUTTORE: inserimento nuovo dato: %d\n",new_value);
        produci(new_value);
        sleep(rand()%3+1);
    }
    pthread_exit(NULL);
}


void * consumatore(void* par) {
    somma_valori *s = (somma_valori*)par; // Recupero del parametro passato al thread

    int i;
    for (i = 0; i < NUM_CONS; i++) {
        req msg;
        res risp;

        // Ricezione del messaggio di richiesta
        if (msgrcv(queue_req, &msg, sizeof(req) - sizeof(long), 0, 0) == -1) {
            perror("CONSUMATORE_SERV: Errore nella ricezione del messaggio");
            pthread_exit(NULL); // Uscita dal thread in caso di errore
        }

        printf("CONSUMATORE_SERV: ricevuta richiesta di consumo\n");

        // Consuma un valore dalla coda condivisa
        risp.type = 1;
        risp.value = consuma(); // Chiamata alla funzione consuma

        // Invio del messaggio di risposta
        if (msgsnd(queue_res, &risp, sizeof(res) - sizeof(long), 0) == -1) {
            perror("CONSUMATORE_SERV: Errore nell'invio del messaggio");
            pthread_exit(NULL); // Uscita dal thread in caso di errore
        }

        printf("CONSUMATORE_SERV: inviato valore al consumatore client: %d\n", risp.value);

        // Aggiornamento della somma e notifica condizione
        pthread_mutex_lock(&s->mutex);
        s->somma += risp.value;
        s->num_valori_sommati++;
        if (s->num_valori_sommati == NUM_CONS) {
            printf("CONSUMATORE_SERV: sommati %d valori, segnalo stampa somma\n", NUM_CONS);
            pthread_cond_signal(&s->ok_somma); // Notifico al thread `stampa_somma`
        }
        pthread_mutex_unlock(&s->mutex);
    }

    pthread_exit(NULL); // Termine del thread
}

int main() {

    //TODO: completare l'inizializzazione code
    key_t msg_req_key =ftok(".",'a'); /*..*/
	key_t msg_res_key = ftok(".",'b');/*..*/

        queue_req =msgget(msg_req_key, 0); /*..*/
	queue_res =msgget(msg_res_key,0); /*..*/

    //TODO inizializzazione di "shared_buf"
    shared_buf.coda=0;
    shared_buf.testa=0;
    shared_buf.nelem=0;
    
    pthread_mutex_init(&shared_buf.mutex,NULL);
    pthread_cond_init(&shared_buf.ok_consuma,NULL);
     pthread_cond_init(&shared_buf.ok_produci,NULL);
    pthread_t prod, cons, sum;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    srand(time(NULL));

    somma_valori* s =(somma_valori*)malloc(sizeof(somma_valori)); /*TODO allocazione struttura*/;
    s->num_valori_sommati = 0;
s->somma = 0;
pthread_mutex_init(&s->mutex, NULL);
pthread_cond_init(&s->ok_somma, NULL);
    // TODO e inizializzazione valori
    pthread_create(&prod,&attr,produttore,(void*)prod);
    pthread_create(&cons,&attr,consumatore, (void *)s);
    pthread_create(&sum,&attr,stampa_somma, (void *)s);
    //TODO creazione dei tre thread prod cons e sum
    // nota: ai thread cons e sum bisogna passare la struttura s

    //TODO join 
    pthread_join(prod,NULL);
    pthread_join(cons,NULL);
    pthread_join(sum,NULL);
    printf("i thread produttore,consumatore e somma sono morti\n");
    //TODO rimozione code e struttura s
    free(s);
   
    msgctl(queue_req,IPC_RMID,0);
    msgctl(queue_res,IPC_RMID,0);
    return 0;
}
