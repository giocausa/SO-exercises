			/*----HEADER FILE----*/
#ifndef __HEADER
#define __HEADER

#include "semafori.h"

#define DIM 4

// TODO: definire semafori
#define MUTEXP 0
#define MUTEXC 1
#define SPAZIO_DISPONIBILE 2
#define MESSAGGIO_DISPONIBILE 3
#define NPROD 4
#define NCONS 2

#define PRODS 4
#define CONS 8

#define LIBERO 0
#define INUSO 1
#define OC1 2
#define OC2 3


typedef struct {
	int buffer[DIM];
	int stato[DIM];
	// TODO: completare campi struct

} BufferMutuaEx;

void produci_tipo_1(BufferMutuaEx * buf, int semid, int val);
void produci_tipo_2(BufferMutuaEx * buf, int semid, int val);
void consuma_tipo_1(BufferMutuaEx * buf, int semid);
void consuma_tipo_2(BufferMutuaEx * buf, int semid);



#endif
