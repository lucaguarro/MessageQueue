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

int main() {
    cout << "r u executing boi" << endl;
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
    struct buf {
		long mtype; // required
		int message;
		//char greeting[50]; // mesg content
	};

	buf msg;
	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/
	int size = sizeof(msg)-sizeof(long);

    bool is251Alive = true;
    bool is997Alive = true;
    bool isReceiverBAlive = true;

    /*DETECTING IF PROCESSES HAVE ENDED
        //set two boolean values one for each sender; is251Alive, is997Alive
        //when we read a message from one that matches the error code, set XXXAlive to false
    }*/
    while(is251Alive || is997Alive){ //While either or both the senders are alive
        cout << "Call before msgrcv! " << endl;
        msgrcv(qid, (struct msgbuf *)&msg, size, 1, 0);
        cout << "Call after msgrcv! " << endl;
        if(msg.message == 0){ //this message code is reserved for when 251 dies
            is251Alive = false;
            cout << "251 died confirmed" << endl;
        }
        else if(msg.message == 1){ //this message code is reserved for when 997 dies
            is997Alive = false;
            cout << "997 died confirmed" << endl;
        }
        else if(msg.message == 2){
            isReceiverBAlive = false;
            cout << "Received confirmation that receiverB has died" << endl;
        }
        else{ //If we get here it means that we have received a message from either 251 or 997
            cout << getpid() << ": gets reply" << endl;
            cout << "reply: " << msg.message << endl; 
            //Need to check if the message was sent from 997
            if(msg.message % 997 == 0){
                msg.message = 143;
                msg.mtype = 4;
                cout << endl << "before sending message to 997" << endl;
                msgsnd(qid, (struct msgbuf *)&msg, size, 0); //HANGING HERE
                cout << "after sending message to 997" << endl << endl;
            }
        } 
        cout << endl;
    }
    //If receiverB is already dead, we need to delete the message queue
    if(!isReceiverBAlive){
        cout << "Deleting the message queue from Receiver A" << endl;
        msgctl (qid, IPC_RMID, NULL);
    }else{ //If not, we need to tell receiverB that this receiver is about to die
        cout << "Attempting to message to Receiver B that I am dying" << endl;
        msg.message = 2; //fatality code
        msg.mtype = 2; //send to receiverB
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        cout << "terminating messsage sent" << endl;
    }
    exit(0);
}