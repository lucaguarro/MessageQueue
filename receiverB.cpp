/*
    The second receiver gets messages from 257 & 997
    The second receiver terminates after it has received a total of 5000 messages
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
#include <signal.h>
using namespace std;

int main() {
    //throw 1;
    cout << "do u execute" << endl;// code that could cause exception
    
	int qid = msgget(ftok(".",'u'), 0);
    struct buf {
		long mtype; // required
		int greeting;
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);
    /*
        We need the pid of sender257 because we use it to kill that process right before this process does
    */
    msgrcv(qid, (struct msgbuf *)&msg, size, 300, 0);
    int pidOf257;
    pidOf257 = msg.greeting;
	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/

    int msgCount = 0;
    while(msgCount != 5000){
        msgrcv(qid, (struct msgbuf *)&msg, size, 2, 0);
        cout << getpid() << ": gets reply" << endl;
	    cout << "reply: " << msg.greeting << endl;
        if(msg.greeting % 997 == 0){
            //send ack message
            msg.greeting = 1;
            msg.mtype = 8;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
        }
        msgCount++;
    }

    /*
        When dying, send message to 997 with error message 0
        and end sender257
    */
    msg.greeting = 0;
    msg.mtype = 8;
    msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending   
    cout << endl << "The pid of 257 we have is: " << pidOf257 << endl;
    kill(pidOf257, SIGTERM);
    exit(0);
}