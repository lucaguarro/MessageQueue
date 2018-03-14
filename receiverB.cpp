/*
    The second receiver gets messages from 257 & 997
    The second receiver terminates after it has received a total of 5000 messages
    Send acknowledgement to 997
*/
//MTYPE = 2
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <signal.h>
using namespace std;

int main() {
    //throw 1;
    cout << "do u execute" << endl;// code that could cause exception
    
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
    bool isReceiverAAlive = true;
    int msgCount = 0;
    while(msgCount != 5000){
        cout << "before receiving message" << endl;
        msgrcv(qid, (struct msgbuf *)&msg, size, 2, 0); //HALTING
        cout << getpid() << ": gets reply" << endl;
	    cout << "reply: " << msg.message << endl;
        if(msg.message % 997 == 0){
            //send ack message
            msg.message = 143;
            msg.mtype = 8;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
        }
        else if(msg.message == 2){ //received this fatality code from receiverA
            isReceiverAAlive = false;
        }
        msgCount++;
    }

    /*
        When dying, send message to 997 with error message 0
        and end sender257
    */
    msg.message = 0;
    msg.mtype = 8;
    msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending   
    msg.message = 2;
    msg.mtype = 257;
    msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    if(!isReceiverAAlive){
        cout << "Deleting the message queue from Receiver B" << endl;
        msgctl (qid, IPC_RMID, NULL);
    }else{
        cout << "Attempting to message to Receiver A that I am dying" << endl;
        msg.message = 2; //fatality code
        msg.mtype = 1; //send to receiverB
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        cout << "terminating message sent" << endl;
    }
    exit(0);
}