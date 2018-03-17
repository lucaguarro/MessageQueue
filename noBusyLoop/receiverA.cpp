/*
    Each receiver repeatedly gets a message & displays the value and sender's identity
    Accepts messages from 251 and 997
    This receiver only terminates after both of its senders had terminated.
    Send acknowledgement to 997 
*/
//MTYPE = 1
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;
bool is251Alive = true;
bool is997Alive = true;
bool isReceiverBAlive = true;
bool is257Alive = true;
int qid;

int main() {
    cout << "Starting receiverA..." << endl;
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
    struct buf {
		long mtype; // required
		int message;
	};

	buf msg;
	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/
	int size = sizeof(msg)-sizeof(long);


    int numAlive = 4;

    int pid = getpid();
    /*DETECTING IF PROCESSES HAVE ENDED
        //set two boolean values one for each sender; is251Alive, is997Alive
        //when we read a message from one that matches the error code, set XXXAlive to false
    }*/
    while(is251Alive || is997Alive){ //While either or both the senders are alive
        msg.message = 143;
        if(isReceiverBAlive){
            msg.mtype = 200;
            cout << "mark E" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is251Alive){
            msg.mtype = 300;
            cout << "mark F" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is257Alive){
            msg.mtype = 400;
            cout << "mark G" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(is997Alive){
            msg.mtype = 500;
            cout << "mark H" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
        }
        cout << "num alive: " << numAlive << endl;
        for(int i = 0; i < numAlive; i++){
            cout << "mark I" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 100, 0);
            switch(msg.message) {
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }
        if(is251Alive){
            cout << "mark A" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 2511, 0);
            if(msg.message == -3){
                is251Alive = false;
            }
            else{
                cout << pid << ": gets reply" << endl;
                cout << "reply: " << msg.message << endl << endl; 
            }
        }
        if(is997Alive){
            cout << "mark B" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 9971, 0);
            if(msg.message == 143){
                is997Alive = false;
            }
            else{
                cout << pid << ": gets reply" << endl;
                cout << "reply: " << msg.message << endl << endl; 
                msg.mtype = 1997;
                msg.message = 143;
                cout << "mark C" << endl;
                msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
            }
        }
        
    }

    msg.message = -1;
    if(isReceiverBAlive){
        msg.mtype = 200;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(is251Alive){
        msg.mtype = 300;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(is257Alive){
        msg.mtype = 400;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if(is997Alive){
        msg.mtype = 500;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);  
    }

    if(!isReceiverBAlive && !is251Alive && !is257Alive && !is997Alive){
        cout << "Deleting the message queue with qid: " << qid << " from Receiver A" << endl;
        msgctl (qid, IPC_RMID, NULL);
    }
    exit(0);
}