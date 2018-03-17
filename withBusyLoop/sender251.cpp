//g++ yoursender251.cpp patchxx.o
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
//MTYPE = 51
/*
	251 sender only sends its messages to one receiver. Make it receiverA.cpp.
	This sender terminates on a 'kill' command from a separate terminal.
*/
int main() {
	//Used to get information about the queue
	msqid_ds qInfo;
	cout << "Sender 251 starting..." << endl;

	//create the message queue
	int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
	cout << "Pid of sender251: " << getpid() << endl;
	// declare my message buffer
	
	/*
		Whatever message we send between programs will go into the int message.
		Very first field has to be a long data type and has to be listed FIRST. This is because the
		system calls make use from that struct.
	*/
	struct buf {
		long mtype; // required
		int message; // mesg content
	};
	buf msg;

	/*
		We need to tell the queue the size of the data passed into the queue
	*/
	int size = sizeof(msg)-sizeof(long);

	/*
		Before we do anything, we need to tell the patch-code what message to send on behalf of sender251
		to receiverA via the get_info function, when the user types in the kill command. Setup so that it sends a message of 0.
	*/
	msg.mtype = 1;
	msg.message = 0;
	get_info(qid, (struct msgbuf *)&msg, size, 1);

	//seed to generate random numbers
	srand(time(NULL));
	int randoInt;
	
	while(true){
		/*
			Generate message between 3 and int max that is divisibl by the marker. 0, 1, & 2 are reserved to signal termination.
			marker = 251 for this program.
		*/
		do
		{
			randoInt = rand() % (RAND_MAX-3) + 3;
		}while(randoInt%251 != 0);
		/*
			There is also a chance that the number can be divided by 997. This would cause issues because 997 requires acknowledgement
			messages and the receiver checks if the message is divisible by 997 to know whether or not to send an ack message.
			So we must ensure that it doesn't send invalid ack messages by subtracting 251 from this message if it is divisable by 997.
		*/
		if(randoInt % 997 == 0){
			randoInt = randoInt - 251;
		}

		//	Send message to receiver A
		msg.message = randoInt;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // HALTS

		/*
			This loop is to ensure that sender251 does not max out the queue. We need to ensure that receiverA can always send an acknowledgment
			message back to sender997.
		*/
		do{
			msgctl(qid, IPC_STAT, &qInfo);
		}while((qInfo.msg_qbytes/size)-2 <= qInfo.msg_qnum); //while the number of messages in the queue are greater than or equal the max - 2

	}
	exit(0);
}