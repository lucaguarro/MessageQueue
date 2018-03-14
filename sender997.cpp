//princesethi13@yahoo.com
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
	997 sender sends its messages to both receivers
	requires acknowledgement for each message from both receivers before it continues execution
	this sender terminates when it gets/observes a random number smaller than 100. 
*/
int main() {
	cout << "do something" << endl;;
	int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);

	// declare my message buffer
	/*
		Whatever message we send between programs will go into the char array.
		Very first field has to be a long data type and has to be listed FIRST. This is because the
		system calls make use from that struct.
	*/
	struct buf {
		long mtype; // required
		int message; // mesg content
	};
	buf msg;
	int size = sizeof(msg)-sizeof(long);

	srand(time(NULL));
	bool receiverBAlive = true;
	//Generate message between 2 and int max. 0 and 1 are reserved to signal termination.
    msg.message = rand() % RAND_MAX + 2;
	int randoInt; 
	do
	{
		randoInt  = rand() % RAND_MAX + 2;
	}while(randoInt%997 != 0 || randoInt < 100);
	while(randoInt > 100){
		msg.mtype = 1;
		msg.message = rand() % RAND_MAX + 2;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
		//Get acknowledgement from receiverA
		msgrcv(qid, (struct msgbuf *)&msg, size, 4,0); // read mesg
		cout << getpid() << ": gets reply" << endl;
		cout << "reply: " << msg.message << endl;
		
		//Check if receiverB is still alive. It could be dead because it ends once it gets 5000 messages.
		if(receiverBAlive){
			//send message to receiver B
			msg.mtype = 2;
			msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
			//Get acknowledgement from receiverB
			msgrcv(qid, (struct msgbuf *)&msg, size, 8, 0); // read mesg
			cout << "wtf" << getpid() << ": gets reply" << endl;
			cout << "reply: " << msg.message << endl;
			if(msg.message == 0){
				receiverBAlive = false;
				cout << "Sender 997 has recognized that receiverB has terminated" << endl;
			}
		}
		//Generate message between 2 and int max. 0 and 1 are reserved to signal termination.
		do
		{
			randoInt  = rand() % RAND_MAX + 2;
		}while(randoInt%997 != 0 || randoInt < 100);
	}

	msg.message = 1;
	msg.mtype = 1;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	exit(0);
}