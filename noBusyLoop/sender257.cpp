/*
    Sender 257 only sends its messages to Receiver B and terminates once Receiver B has
*/
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



int main() {
    cout << "Sender257 starting..." << endl;

    /*
        Create the message queue and return the id. If it has already been created, just return its id.
    */
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
    /*
        Message structure for IPC
    */
    struct buf {
		long mtype; //message type
		int message;
	};
	buf msg;
    
    /*
        Before any messages are received, all 4 of the other programs will be alive.
        We use int numAlive to keep track of how many alive/dead messages we expect to receive. 
        There should be 1 from each other process.
    */
    int numAlive = 4;
    /*
        Sender 257 keeps track of the alive/dead state of ReceiverA, ReceiverB, Sender251, and Sender997.
        Sender 257 dies, when Receiver B stops receiving messages.
    */
    bool isReceiverAAlive = true;
    bool isReceiverBAlive = true;
    bool is251Alive = true;
    bool is997Alive = true;

	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue. We store the number of bytes in size.
	*/
	int size = sizeof(msg)-sizeof(long);
	int randoInt; //will hold values of random integers
    srand(time(NULL)); //seed so that different random values are generated each time program is ran
    /*
        sender257 executes as long as its receiver is alive (ReceiverB)
    */
    while(isReceiverBAlive){
        /*
            The first thing we do is send an alive message to all the other 4 processes to receive.
            The mtypes for alive/dead messages follow the following schema:
            ReceiverA: 100
            ReceiverB: 200
            Sender251: 300
            Sender257: 400 SKIP because this is Sender 257
            Sender997: 500
        */
        msg.message = 143;
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
        /*
            After we send the alive messages, we expect there to be one alive/dead messages from each 
            other process that is or was just alive sent to sender257 using mtype = 400. 
            If a process is alive, we do not do anything. We only need to check for the dead messages
            which we know happen if the message value is in range [-1,-5]. Each value denotes the dead
            message for a particular process, according to the following schema:
            ReceiverA: -1
            ReceiverB: -2
            Sender251: -3
            Sender257: -4 SKIP because this is Sender257
            Sender997: -5
            
            When we detect a dead message, we decrement numAlive and set the corresponding boolean to false.
        */
        for(int i = 0; i < numAlive; i++){
            msgrcv(qid, (struct msgbuf *)&msg, size, 400, 0);
            switch(msg.message) {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }
        /*
            The rest of the code in this while loop performs Sender257's designated task. That is to generate a random
            int value that is divisible by 257 and send it to ReceiverB. It uses 2572 for these type of messages, that
            program ReceiverB will use to read it.
        */
		msg.mtype = 2572;
		do
		{
			randoInt = rand() % RAND_MAX;
		}while(randoInt%257 != 0);
		msg.message = randoInt;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
    }

    /*
        When we exit the for loop, that means that Sender257 has completed its job and is about to die. But before it dies,
        it must send a termination message to each one of the other processes if those processes are still alive. Staying consistent
        with the schema provided above, it uses message value -4 to denote that it is dying and the mtypes that the other processes
        use to read dead/alive messages.
    */
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

    /*
        With current setup, this process will never be the last to die since in order for Receiver B to die, this sender must die first.
        Therefore we do not need to check to delete the message queue.
    */
    exit(0);
}