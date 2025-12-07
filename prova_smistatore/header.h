#ifndef _HEADER_H_
#define _HEADER_H_

#include <pthread.h>

#define TIPO 1
#define LETTORI 3

typedef struct {

    // TODO: completare campi messaggio
    long type;
    int valore;
} Messaggio;

typedef struct {
    int valore;
    // TODO: completare inserendo i campi necessari a gestire il problema dei lettori-scrittori (unico scrittore)
    pthread_mutex_t mutex;
    pthread_cond_t cv_scrittori;
    pthread_cond_t cv_lettori;
    int num_lettori;
    int scrittore;

} MonitorLS;

#endif
