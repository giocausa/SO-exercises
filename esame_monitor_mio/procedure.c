#include "procedure.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <time.h>


int stampa_risultato(MonitorRisultati * ls) {

	int risultato;

	// TODO: consumo il valore
	enter_monitor(&ls->m);
		if(ls->num_elementi == 0){
			printf("[%d]Sospensione Consumatore\n",getpid());
			wait_condition(&ls->m,VARCOND_CONSUMATORI);
			printf("[%d]Riattivazione Consumatore\n",getpid());
		}
		
		risultato = ls->buffer[ls->coda];
		//printf("[%d]Consumatore: Il valore consumato = %d\n",getpid(),risultato);
		ls->coda = (ls->coda+1)%DIM;
		ls->num_elementi--;
		signal_condition(&ls->m,VARCOND_PRODUTTORI);
	leave_monitor(&ls->m);

	return risultato;

}


void inserisci_risultato(MonitorRisultati * ls, int valore) {

	// TODO: produco il valore
	enter_monitor(&ls->m);
		if(ls->num_elementi == DIM){
			printf("[%d]Sospensione Produttore\n",getpid());
			wait_condition(&ls->m,VARCOND_PRODUTTORI);
			printf("[%d]Riattivazione Produttore\n",getpid());
		}
		
		ls->buffer[ls->testa] = valore;
		printf("[%d]Produttore: Il valore produtto = %d\n",getpid(),ls->buffer[ls->testa]);
		ls->testa = (ls->testa+1)%DIM;
		ls->num_elementi++;
		signal_condition(&ls->m,VARCOND_CONSUMATORI);
	leave_monitor(&ls->m);
}

void filtro(MonitorRisultati * ls, int coda){
	Messaggio m;
	int valore = 0;
	for (int i = 0 ; i < 4; i++){
		// TODO: filtro i valori prelevati dalla mailbox: se sono pari, li inserisco, altrimenti inserisco 0
		msgrcv(coda,(void*)&m,sizeof(Messaggio)-sizeof(long),MESSAGGIO,0);
		valore = m.valore;
		printf("[%d]Messaggio ricevuto: %d\n",getpid(),valore);
		if(valore%2 == 0){
			//pari
			inserisci_risultato(ls,valore);
		}else{
			//dispari
			inserisci_risultato(ls,0);
		}
	}
}
void generazione(int coda) {
	srand(getpid()*time(NULL));
	Messaggio m;
	sleep(2);
	for (int i = 0 ; i < 4; i++) {
		
		// TODO: genero dei numeri casuali tra 1 e 20 e li invio alla mailbox
		int valore = rand()%20 + 1;
		m.type = MESSAGGIO;
		m.valore = valore;
		msgsnd(coda,(void*)&m,sizeof(Messaggio)-sizeof(long),0);
		printf("[%d]Messaggio inviato.: %d\n",getpid(),m.valore);
	}
}
void stampa(MonitorRisultati * ls) {
	int val;
	for (int i = 0 ; i < 4; i++) {
		
		// TODO: stampo il valore appena consumato
		val = stampa_risultato(ls);
		
		printf("[%d] Ho letto il valore: %d \n", getpid(), val);
	}
}
