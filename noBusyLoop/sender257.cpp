#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>
using namespace std;
//MTYPE = 257
/*
	257 sender only sends its messages to one receiver. Make it receiverB.cpp.
	terminates when its receiver stops receiving event notification 
*/
bool isReceiverAAlive = true;
bool isReceiverBAlive = true;
bool is251Alive = true;
bool is997Alive = true;
int qid;

int main() {
    cout << "Sender257 starting..." << endl;// code that could cause exception
    
	qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
    struct buf {
		long mtype; // required
		int message;
	};

	buf msg;
    int pid = getpid();

    int numAlive = 4;

	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/
	int size = sizeof(msg)-sizeof(long);
	int randoInt;
    while(isReceiverBAlive){
        msg.message = 143;
        if(isReceiverAAlive){
            msg.mtype = 100;
			cout << "mark E" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(isReceiverBAlive){
            msg.mtype = 200;
			cout << "mark F" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is251Alive){
            msg.mtype = 300;
			cout << "mark G" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is997Alive){
            msg.mtype = 500;
			cout << "mark H" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
        }
        for(int i = 0; i < numAlive; i++){
			cout << "mark I" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 400, 0);
            switch(msg.message) {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }
		msg.mtype = 2572;
		do
		{
			randoInt = rand() % RAND_MAX;
		}while(randoInt%257 != 0);
		msg.message = randoInt;
		cout << "mark A" << endl;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
    }

    msg.message = -4;
    if(isReceiverAAlive){
        msg.mtype = 100;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(isReceiverBAlive){
        msg.mtype = 200;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(is251Alive){
        msg.mtype = 300;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(is997Alive){
        msg.mtype = 500;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
    }

    if(!isReceiverAAlive && !isReceiverBAlive && !is251Alive && !is997Alive){
        cout << "Deleting the message queue with qid: " << qid << " from Receiver A" << endl;
        msgctl (qid, IPC_RMID, NULL);
    }
    exit(0);
}