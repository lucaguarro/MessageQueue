/*
	251 sender only sends its messages to one receiver. Make it receiverA.cpp.
	This sender terminates on a 'kill' command from a separate terminal.
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

#include "get_info.h"
using namespace std;

/*
    These need to all be global because arguments cannot be passed to the sigintHandler function
    Sender 251 keeps track of the alive/dead state of ReceiverA, ReceiverB, Sender257, Sender997
*/
bool isReceiverAAlive = true;
bool isReceiverBAlive = true;
bool is257Alive = true;
bool is997Alive = true;
int qid;
int size;

/*
    Message structure for IPC
*/
struct buf {
    long mtype; // required
    int message;
};

buf msg;

/*
    Our version of the patch code that extends the functionality to be able to send more than 1 message.
    Essentially, if it gets the designated kill message (10) from the user or a Ctrl + C, it will send termination messages
    to each one of the other processes that are stil alive.
*/
void sigintHandler(int sig_num)
{
    if(sig_num == SIGINT || sig_num == 10){ //If ctrl+c or SIGUSR1 is set to 10
        msg.message = -3;
        /*
            Because we do not know where ReceiverA or ReceiverB are in their execution, we need to send a termination message to both of
            their mtypes for each one.
        */
        if(isReceiverAAlive){
            msg.mtype = 100; 
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
            msg.mtype = 2511;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if(isReceiverBAlive){
            msg.mtype = 200;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
            msg.mtype = 2512;
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
    }
    exit(0);
}

int main() {
    cout << "Sender251 starting..." << endl;// code that could cause exception
	cout << "Pid of Sender251: " << getpid() << endl; //print the pid of the process so its easy to terminate from the terminal

    /*
        Create the message queue and return the id. If it has already been created, just return its id.
    */
	qid = msgget(ftok(".",'u'), IPC_CREAT|0600);

    /*
        Before any messages are received, all 4 of the other programs will be alive.
        We use int numAlive to keep track of how many alive/dead messages we expect to receive. 
        There should be 1 from each other process.
    */
    int numAlive = 4;

	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/
	size = sizeof(msg)-sizeof(long);

    /*
        In order for the Sender251 to end execution, it must be terminated from the terminal. It can either be done using Ctrl + C
        or via SIGUSR1 signal handling.
        *Note because in our setup - each program keeps track of the alive, dead state of each other process it does not suffice to only send one
        termination message so we opted to not use the patch code and write our own "version" that allows us to send more than one message.
    */
    signal(SIGINT, sigintHandler);
    signal(10, sigintHandler);

	int randoInt; //will hold values of random integers

    while(true){ //251 is always in execution until user terminates it
        /*
            The first thing we do is send an alive message to all the other 4 processes to receive.
            The mtypes for alive/dead messages follow the following schema:
            ReceiverA: 100  
            ReceiverB: 200
            Sender251: 300 SKIP because this is Sender 251
            Sender257: 400 
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
            other process that is or was just alive sent to Sender251 using mtype = 300. 
            If a process is alive, we do not do anything. We only need to check for the dead messages
            which we know happen if the message value is in range [-1,-5]. Each value denotes the dead
            message for a particular process, according to the following schema:
            ReceiverA: -1
            ReceiverB: -2
            Sender251: -3 SKIP because this is Sender 251
            Sender257: -4 
            Sender997: -5 
            
            When we detect a dead message, we decrement numAlive and set the corresponding boolean to false.
        */
        for(int i = 0; i < numAlive; i++){
            msgrcv(qid, (struct msgbuf *)&msg, size, 300, 0);
            switch(msg.message) {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }

        /*
            The rest of the code in this while loop performs Sender251's designated task. That is to generate a random
            int value that is divisible by 251 and send it to ReceiverA. It uses 2511 for these type of messages, that
            program ReceiverA will use to read it.        
        */
		msg.mtype = 2511;
		do
		{
			randoInt = rand() % RAND_MAX;
		}while(randoInt%251 != 0);
		msg.message = randoInt;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
    }
    exit(0);
}