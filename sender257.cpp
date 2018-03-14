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

/*
	257 sender only sends its messages to one receiver. Make it receiverB.cpp.
	terminates when its receiver stops receiving event notification 
*/
int main() {
	cout << "Do you execute?" << endl;
	int qid = msgget(ftok(".",'u'), 0);
	int pid = getpid();
	// declare my message buffer
	cout << "PID257: " << pid << endl;
	struct buf {
		long mtype; // required
		int message; // mesg content
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);

	/*
		Because this process ends when its receiver ends, the first thing we do is give its receiver the pid.
		This way right before the receiver process ends, it can kill this sender using the pid given to it in the beginning.
	*/
	msg.mtype = 300; //this mtype is only used for this message
	msg.message = pid;
	cout << "before sending" << endl;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
	cout << "after sending" << endl;
	srand(time(NULL));
	
	/*
		This sender gets killed from another process so we loop indefinitely
	*/
	msg.mtype = 2; //2 for receiverB
	cout << "entering loop" << endl;
	int randoInt;
	while(true){
		do
		{
			randoInt = rand() % (RAND_MAX-2) + 2; //Generate message between 2 and int max. 0 and 1 are reserved to signal termination.
		}while(randoInt%257 != 0);
		/*
			There is also a chance that the number can be divided by 997. This would cause issues because 997 requires acknowledgement
			messages and the receiver checks if the message is divisible by 997 to know whether or not to send an ack message.
			So we must ensure that it doesn't send invalid ack messages by subtracting 257 from this message if it is divisable by 997.
		*/
		if(msg.message % 997 == 0){
			msg.message -= 257;
		}
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
	}
	exit(0);
}