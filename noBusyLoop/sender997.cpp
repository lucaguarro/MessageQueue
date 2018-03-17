//princesethi13@yahoo.com
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
//MTYPE = 97
/*
	997 sender sends its messages to both receivers
	requires acknowledgement for each message from both receivers before it continues execution
	this sender terminates when it gets/observes a random number smaller than 100. 
*/
bool isReceiverAAlive = true;
bool isReceiverBAlive = true;
bool is251Alive = true;
bool is257Alive = true;
int qid;

int main() {
	cout << "Sender 997 starting..." << endl;;
	qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
	struct buf {
		long mtype; // required
		int message; // mesg content
	};
	buf msg;
	int size = sizeof(msg)-sizeof(long);
	srand(time(NULL));
	int pid = getpid();
	int randoInt;
	int numAlive = 4; 
	do
	{
		randoInt = rand() % (RAND_MAX-3) + 3; //Generate message between 3 and int max. 0, 1, & 2 are reserved to signal termination.
	}while(randoInt%997 != 0 && randoInt > 100);
	while(randoInt > 100){
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
        if(is257Alive){
            msg.mtype = 400;
			cout << "mark H" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
        }
        for(int i = 0; i < numAlive; i++){
			cout << "mark I" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 500, 0);
            switch(msg.message) {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
            }
        }
		msg.message = randoInt;
		if(isReceiverAAlive){
			msg.mtype = 9971;
			msg.message = randoInt;
			cout << "mark A" << endl;
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
			cout << "mark B" << endl;
			msgrcv(qid, (struct msgbuf *)&msg, size, 1997, 0);
            cout << pid << ": gets reply" << endl;
            cout << "reply: " << msg.message << endl << endl; 
		}
		if(isReceiverBAlive){
			msg.mtype = 9972;
			msg.message = randoInt;
			cout << "mark C" << endl;
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
			cout << "mark D" << endl;
			msgrcv(qid, (struct msgbuf *)&msg, size, 2997, 0);
            cout << pid << ": gets reply" << endl;
            cout << "reply: " << msg.message << endl << endl; 
		}
		do
		{
			randoInt = rand() % RAND_MAX; 
		}while(randoInt%997 != 0 && randoInt > 100);
	}

    
    if(isReceiverAAlive){
		msg.message = -5;
        msg.mtype = 100;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		msg.message = 143;
		msg.mtype = 9971;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(isReceiverBAlive){
		msg.message = -5;
        msg.mtype = 200;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		msg.message = 143;
		msg.mtype = 9972;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
	msg.message = -5;
    if(is251Alive){
        msg.mtype = 300;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(is257Alive){
        msg.mtype = 400;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
    }

    if(!isReceiverAAlive && !isReceiverBAlive && !is251Alive && !is257Alive){
        cout << "Deleting the message queue with qid: " << qid << " from Sender 997" << endl;
        msgctl (qid, IPC_RMID, NULL);
    }
	exit(0);
}