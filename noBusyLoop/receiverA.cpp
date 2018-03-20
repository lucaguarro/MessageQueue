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
bool is251Alive = true;
bool is997Alive = true;
bool isReceiverBAlive = true;
bool is257Alive = true;
int qid;

int main() {
    cout << "Starting receiverA..." << endl;

    /*
        Create the message queue and return the id. If it has already been created, just return its id.
    */
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);

    /*
        Message structure for IPC
    */
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

    /*
        Before any messages are received, all 4 of the other programs will be alive.
        We use int numAlive to keep track of how many alive/dead messages we expect to receive. 
        There should be 1 from each other process.
    */
    int numAlive = 4;

    int pid = getpid();
    /*
        ReceiverA executes as long as either Sender251 or Sender995 is still in execution AKA still alive
    */
    while(is251Alive || is997Alive){
        /*
            The first thing we do is send an alive message to all the other 4 processes to receive.
            The mtypes for alive/dead messages follow the following schema:
            ReceiverA: 100  SKIP because this is Receiver A
            ReceiverB: 200
            Sender251: 300
            Sender257: 400 
            Sender997: 500
        */
        msg.message = 143;
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
        /*
            After we send the alive messages, we expect there to be one alive/dead messages from each 
            other process that is or was just alive sent to Sender251 using mtype = 100. 
            If a process is alive, we do not do anything. We only need to check for the dead messages
            which we know happen if the message value is in range [-1,-5]. Each value denotes the dead
            message for a particular process, according to the following schema:
            ReceiverA: -1 SKIP because this is Receiver A
            ReceiverB: -2
            Sender251: -3
            Sender257: -4 
            Sender997: -5 
            
            When we detect a dead message, we decrement numAlive and set the corresponding boolean to false.
        */
        for(int i = 0; i < numAlive; i++){
            msgrcv(qid, (struct msgbuf *)&msg, size, 100, 0);
            switch(msg.message) {
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }
        /*
            If Sender251 is still alive, we need to read messages from it. mtype = 2511 is used for "real" messages from Sender257. "Real" meaning
            not dead/alive messages.
        */
        if(is251Alive){
            msgrcv(qid, (struct msgbuf *)&msg, size, 2511, 0);
            if(msg.message == -3){
                is251Alive = false;
            }
            else{
                cout << pid << ": gets reply" << endl;
                cout << "reply: " << msg.message << endl << endl; 
            }
        }
        /*
            If Sender997 is still alive, we need to read messages from it. mtype = 9971 is used for "real" messages from Sender997. However since
            Sender 997 could die when this Receiver expects a message from it, Sender997 also sends a termination message (-5) to this mtype so we must
            catch it. If we detect a "real" message (i.e. one that is not a termination message), we must send an acknowledgement message to 997.
        */
        if(is997Alive){
            msgrcv(qid, (struct msgbuf *)&msg, size, 9971, 0);
            if(msg.message == -5){
                is997Alive = false;
            }
            else{
                cout << pid << ": gets reply" << endl;
                cout << "reply: " << msg.message << endl << endl; 
                msg.mtype = 1997;
                msg.message = 143;
                msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
            }
        }
        
    }

    /*
        When we exit the for loop, that means that ReceiverA has completed its job and is about to die. But before it dies,
        it must send a termination message to each one of the other processes if those processes are still alive. Staying consistent
        with the schema provided above, it uses message value -1 to denote that it is dying and the mtypes that the other processes
        use to read dead/alive messages.
    */
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
    
    /*
        If this process is the last to die, we must delete the message queue.
    */
    if(!isReceiverBAlive && !is251Alive && !is257Alive && !is997Alive){
        cout << "Deleting the message queue with qid: " << qid << " from Receiver A" << endl;
        msgctl (qid, IPC_RMID, NULL);
    }
    exit(0);
}