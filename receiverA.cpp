/*
    Each receiver repeatedly gets a message & displays the value and sender's identity
    Accepts messages from 251 and 997
    This receiver only terminates after both of its senders had terminated.
    Send acknowledgement to 997 
*/

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
	int qid = msgget(ftok(".",'u'), 0);
    struct buf {
		long mtype; // required
		int greeting;
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

    /*DETECTING IF PROCESSES HAVE ENDED
        //set two boolean values one for each sender; is251Alive, is997Alive
        //when we read a message from one that matches the error code, set XXXAlive to false
    }*/
    while(is251Alive || is997Alive){ //While either or both the senders are alive
        msgrcv(qid, (struct msgbuf *)&msg, size, 1, 0);
        if(msg.greeting == 0){ //this message code is reserved for when 251 dies
            is251Alive = false;
            cout << "251 died confirmed";
        }
        if(msg.greeting == 1){ //this message code is reserved for when 997 dies
            is997Alive = false;
            cout << "997 died confirmed";
        }
        cout << getpid() << ": gets reply" << endl;
        cout << "reply: " << msg.greeting << endl;        
    }
    msgctl (qid, IPC_RMID, NULL);
    exit(0);
}