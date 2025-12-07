#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int pid;
   key_t msg_req_key =ftok(".",'a'); //TODO: definire chiave
	key_t msg_res_key =ftok(".",'b');//TODO: definire chiave

    //TODO: INIZIALIZZAZIONE CODE
	 int queue_req=msgget(msg_req_key,IPC_CREAT|0664);
	int queue_res=msgget(msg_res_key,IPC_CREAT|0664);
    pid = fork();//TODO
    if (pid==0) {
        //TODO: eseguibile server
        execl("./server","./server",NULL);
        _exit(0);
    }

    pid = fork();//TODO
    if (pid==0) {
        //TODO: eseguibile reader
        execl("./reader","./reader",NULL);
        _exit(0);
    }

    //TODO: terminazione processi
    wait(NULL);
    wait(NULL);
    // TODO: eliminazione code
    msgctl(queue_req, IPC_RMID,0);
     msgctl(queue_res, IPC_RMID,0);

    return 0;
}
