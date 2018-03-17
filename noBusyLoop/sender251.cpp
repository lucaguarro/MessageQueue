//g++ yoursender251.cpp patchxx.o
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>

#include "get_info.h"
using namespace std;
//MTYPE = 51
/*
	251 sender only sends its messages to one receiver. Make it receiverA.cpp.
	This sender terminates on a 'kill' command from a separate terminal.
*/
bool isReceiverAAlive = true;
bool isReceiverBAlive = true;
bool is257Alive = true;
bool is997Alive = true;

struct buf {
    long mtype; // required
    int message;
};

buf msg;

void sigintHandler(int sig_num)
{
    cout << "the sig_num is: " << sig_num << endl;
    int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
    int size = sizeof(msg)-sizeof(long);
    if(sig_num == SIGINT || sig_num == 10){
        msg.message = -3;
        msg.mtype = 100;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        msg.mtype = 2511;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    exit(0);
}

int main() {
    cout << "Sender251 starting..." << endl;// code that could cause exception
	cout << getpid() << endl;
    signal(SIGINT, sigintHandler);
    signal(10, sigintHandler);

	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);

    int pid = getpid();

    int numAlive = 4;

	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/
	int size = sizeof(msg)-sizeof(long);
	int randoInt;


    /*msg.message = -3;
	msg.mtype = 100;
	get_info(qid, (struct msgbuf *)&msg, size, 1);*/
	/*msg.mtype = 200;
	get_info(qid, (struct msgbuf *)&msg, size, 1);
	msg.mtype = 400;
	get_info(qid, (struct msgbuf *)&msg, size, 1);
	msg.mtype = 500;
	get_info(qid, (struct msgbuf *)&msg, size, 1);*/


    while(true){
        msg.message = 143;
        if(isReceiverAAlive){
            msg.mtype = 100;
			//cout << "mark E" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(isReceiverBAlive){
            msg.mtype = 200;
			//cout << "mark F" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is257Alive){
            msg.mtype = 400;
			//cout << "mark G" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is997Alive){
            msg.mtype = 500;
			//cout << "mark H" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
        }
        for(int i = 0; i < numAlive; i++){
			//cout << "mark I" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 300, 0);
            switch(msg.message) {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }
		msg.mtype = 2511;
		do
		{
			randoInt = rand() % RAND_MAX;
		}while(randoInt%251 != 0);
		msg.message = randoInt;
		//cout << "mark A" << endl;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
    }




    exit(0);
}