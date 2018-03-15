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
	msqid_ds qInfo;
	cout << "Sender 251 starting..." << endl;
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
	int size = sizeof(msg)-sizeof(long);

	/*
		Before we do anything, we need to tell the patch-code what message to send on behalf of sender251
		to receiverA, when the user types in the kill command.
		us
	*/
	msg.mtype = 1;
	msg.message = 0;
	get_info(qid, (struct msgbuf *)&msg, size, 1);
	srand(time(NULL));

	int randoInt;
	
	//Generate message between 2 and int max. 0 and 1 are reserved to signal termination.
	
	while(true){
		do
		{
			randoInt = rand() % (RAND_MAX-3) + 3; //Generate message between 3 and int max. 0, 1, & 2 are reserved to signal termination.
		}while(randoInt%251 != 0);
		/*
			There is also a chance that the number can be divided by 997. This would cause issues because 997 requires acknowledgement
			messages and the receiver checks if the message is divisible by 997 to know whether or not to send an ack message.
			So we must ensure that it doesn't send invalid ack messages by subtracting 251 from this message if it is divisable by 997.
		*/
		if(randoInt % 997 == 0){
			randoInt = randoInt - 251;
		}
		msg.message = randoInt;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // HALTS

		//this do-while loop should make sure that the message queue does not get overfilled
		do{
			msgctl(qid, IPC_STAT, &qInfo);
		}while((qInfo.msg_qbytes/size)-2 <= qInfo.msg_qnum); //while the number of messages in the queue are greater than or equal the max - 2
		
		//cout << "max bytes: " << qInfo.msg_qbytes;
		//cout << "Num messages on queue: " << qInfo.msg_qnum << endl;

	}
	exit(0);
}