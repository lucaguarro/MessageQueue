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
//MTYPE = 257
/*
	257 sender only sends its messages to one receiver. Make it receiverB.cpp.
	terminates when its receiver stops receiving event notification 
*/
int main() {
	cout << "Do you execute?" << endl;
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
	int pid = getpid();
	// declare my message buffer
	cout << "PID257: " << pid << endl;
	struct buf {
		long mtype; // required
		int message; // mesg content
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);

	srand(time(NULL));
	
	msg.mtype = 2; //2 for receiverB
	cout << "entering loop" << endl;
	int randoInt;
	bool receiverBAlive = true;

	msg.mtype = 257;
	msg.message = 257;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	while(receiverBAlive){
		cout << "before receiving message from self" << endl;
		msgrcv(qid, (struct msgbuf *)&msg, size, 257,0); // read mesg
		cout << "after receiving message from self" << endl;
		if(msg.message==2){
			receiverBAlive = false;
		}
		do
		{
			randoInt = rand() % (RAND_MAX-3) + 3; //Generate message between 3 and int max. 0, 1, & 2 are reserved to signal termination.
		}while(randoInt%257 != 0);
		
		/*
			There is also a chance that the number can be divided by 997. This would cause issues because 997 requires acknowledgement
			messages and the receiver checks if the message is divisible by 997 to know whether or not to send an ack message.
			So we must ensure that it doesn't send invalid ack messages by subtracting 257 from this message if it is divisable by 997.
		*/
		if(msg.message % 997 == 0){
			randoInt -= 257;
		}
		//msgsnd()
		/*
			We must always make sure that there is a message in the queue for this sender to read so that it does not halt when trying to 
			receive the fatality code from receiverB
		*/
		msg.mtype = 257;
		msg.message = 257;
		cout << "before sending message to self" << endl;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); //HALTING
		cout << "after sending message to self" << endl;
		msg.mtype = 2;
		msg.message = randoInt;
		if(receiverBAlive){
			cout << "before sending message to receiverB" << endl;
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
			cout << "after sending message to receiverB" << endl;
		}
	}
	exit(0);
}