/*
	997 sender sends its messages to both receivers
	requires acknowledgement for each message from both receivers before it continues execution
	this sender terminates when it gets/observes a random number smaller than 100. 
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
	cout << "Sender 997 starting..." << endl;;

    /*
        Create the message queue and return the id. If it has already been created, just return its id.
    */
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);

    /*
        Message structure for IPC
    */
	struct buf {
		long mtype; // required
		int message; // mesg content
	};
	buf msg;
	/*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue. We store the number of bytes in size.
	*/
	int size = sizeof(msg)-sizeof(long);
	srand(time(NULL)); //seed so that different random values are generated each time program is ran
	int pid = getpid(); //program id of the process
	int randoInt; //will hold values of random integers

    /*
        Before any messages are received, all 4 of the other programs will be alive.
        We use int numAlive to keep track of how many alive/dead messages we expect to receive. 
        There should be 1 from each other process.
    */
	int numAlive = 4; 
	/*
		Sender 997 keeps track of the alive/dead state of ReceiverA, ReceiverB, Sender251, and Sender257
	*/
	bool isReceiverAAlive = true;
	bool isReceiverBAlive = true;
	bool is251Alive = true;
	bool is257Alive = true;

	/*
		Sender 997 sends a message once it has generated a random value that is divisible by 997.
		But it also terminates when it generates a random value that is less than 100.
		We need to check for both cases.
	*/
	do
	{
		randoInt = rand() % (RAND_MAX);
	}while(randoInt%997 != 0 && randoInt > 100);

	/*
		While a value that is greater than 100 has been generated
	*/
	while(randoInt > 100){
        /*
            The first thing we do is send an alive message to all the other 4 processes to receive.
            The mtypes for alive/dead messages follow the following schema:
            ReceiverA: 100
            ReceiverB: 200
            Sender251: 300
            Sender257: 400 
            Sender997: 500 SKIP because this is Sender 997
        */
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

        /*
            After we send the alive messages, we expect there to be one alive/dead messages from each 
            other process that is or was just alive sent to sender997 using mtype = 500. 
            If a process is alive, we do not do anything. We only need to check for the dead messages
            which we know happen if the message value is in range [-1,-5]. Each value denotes the dead
            message for a particular process, according to the following schema:
            ReceiverA: -1
            ReceiverB: -2
            Sender251: -3
            Sender257: -4 
            Sender997: -5 SKIP because this is Sender997
            
            When we detect a dead message, we decrement numAlive and set the corresponding boolean to false.
        */
        for(int i = 0; i < numAlive; i++){
            msgrcv(qid, (struct msgbuf *)&msg, size, 500, 0);
            switch(msg.message) {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -2: isReceiverBAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
            }
        }

		/*
			Here is where we perform the rest of Sender997's designated task. That is to send the random integer value generated to both
			receiver A and receiver B if they are alive. We also expect to receive a message back.
			One peculiarity is that ReceiverB can terminate after Sender997 sends its message but before ReceiverB can send an acknowledgement message
			back to it. Because of this we need to catch a termination message also for this mtype. This isn't the case with ReceiverA as it will always
			be executing as long as this sender is executing. But if we want to expand ReceiverA to handle Ctrl + C user termination, we would need it.
		*/
		msg.message = randoInt;
		if(isReceiverAAlive){
			msg.mtype = 9971;
			msg.message = randoInt;
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
			msgrcv(qid, (struct msgbuf *)&msg, size, 1997, 0);
			cout << pid << ": gets reply" << endl;
			cout << "reply: " << msg.message << endl << endl; 
		}
		if(isReceiverBAlive){
			msg.mtype = 9972;
			msg.message = randoInt;
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); 
			msgrcv(qid, (struct msgbuf *)&msg, size, 2997, 0);
			if(msg.message == -2){
				isReceiverBAlive = false;
			}
			else{
				cout << pid << ": gets reply" << endl;
				cout << "reply: " << msg.message << endl << endl; 
			}
		}
		/*
			Generate a new random value divisible by 997 or less than 100.
		*/
		do
		{
			randoInt = rand() % RAND_MAX; 
		}while(randoInt%997 != 0 && randoInt > 100);
	}

    /*
        When we exit the for loop, that means that Sender997 has completed its job and is about to die. But before it dies,
        it must send a termination message to each one of the other processes if those processes are still alive. Staying consistent
        with the schema provided above, it uses message value -5 to denote that it is dying and the mtypes that the other processes
        use to read dead/alive messages.
    */
	msg.message = -5;
    /*
        Sends error message to both mtypes of Receiver A because we do not know where it is in its execution
    */
    if(isReceiverAAlive){
        msg.mtype = 100;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		msg.mtype = 9971;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    /*
        Sends error message to both mtypes of Receiver B because we do not know where it is in its execution
    */
    if(isReceiverBAlive){
        msg.mtype = 200;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		msg.mtype = 9972;
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

    /*
        With current setup, this process will never be the last to die since in order for Receiver A to die, this sender must die first.
        Therefore we do not need to check to delete the message queue.
    */
	exit(0);
}