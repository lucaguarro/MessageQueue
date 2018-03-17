/*
    The second receiver gets messages from 257 & 997
    The second receiver terminates after it has received a total of 5000 messages
    Send acknowledgement to 997
*/
//MTYPE = 2
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

bool is251Alive = true;
bool is997Alive = true;
bool isReceiverAAlive = true;
bool is257Alive = true;
int qid;

void my_handler(int s)
{
    //printf("Caught signal %d\n",s);
    if (!isReceiverAAlive && !is251Alive && !is257Alive && !is997Alive)
    {
        cout << "Deleting the message queue with qid: " << qid << " from Receiver A" << endl;
        msgctl(qid, IPC_RMID, NULL);
    }
    exit(1);
}
int main()
{
    //Handle potential ctrl+c event
    /*struct sigaction sigIntHandler;
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;*/
    //--------------------------------------

    cout << "ReceiverB starting..." << endl; // code that could cause exception

    qid = msgget(ftok(".", 'u'), IPC_CREAT | 0600);
    struct buf
    {
        long mtype; // required
        int message;
    };

    buf msg;
    int pid = getpid();

    int numAlive = 4;

    /*
		When we send a message into a queue. The OS needs to know how much memory to allocate for the
		message queue.
	*/
    int size = sizeof(msg) - sizeof(long);
    int msgCount = 0;
    while (msgCount != 5000)
    {
        msg.message = 143;
        if (isReceiverAAlive)
        {
            msg.mtype = 100;
            cout << "mark E" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if (is251Alive)
        {
            msg.mtype = 300;
            cout << "mark F" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if (is257Alive)
        {
            msg.mtype = 400;
            cout << "mark G" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        if (is997Alive)
        {
            msg.mtype = 500;
            cout << "mark H" << endl;
            msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        }
        for (int i = 0; i < numAlive; i++)
        {
            cout << "mark I" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 200, 0);
            switch (msg.message)
            {
            case -1:
                isReceiverAAlive = false;
                numAlive--;
                break;
            case -3:
                is251Alive = false;
                numAlive--;
                break;
            case -4:
                is257Alive = false;
                numAlive--;
                break;
            case -5:
                is997Alive = false;
                numAlive--;
                break;
            }
        }
        if (is257Alive)
        {
            cout << "mark A" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 2572, 0);
            cout << pid << ": gets reply" << endl;
            cout << "reply: " << msg.message << endl
                 << endl;
        }
        if (is997Alive)
        {
            cout << "mark B" << endl;
            msgrcv(qid, (struct msgbuf *)&msg, size, 9972, 0);
            if(msg.message == 143){
                //ignore
            }
            else
            {
                cout << pid << ": gets reply" << endl;
                cout << "reply: " << msg.message << endl
                     << endl;
                msg.mtype = 2997;
                msg.message = 143;
                cout << "mark C" << endl;
                msgsnd(qid, (struct msgbuf *)&msg, size, 0);
            }
        }
        msgCount++;
    }

    msg.message = -2;
    if (isReceiverAAlive)
    {
        msg.mtype = 100;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if (is251Alive)
    {
        msg.mtype = 300;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if (is257Alive)
    {
        msg.mtype = 400;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }
    if (is997Alive)
    {
        msg.mtype = 500;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }

    if (!isReceiverAAlive && !is251Alive && !is257Alive && !is997Alive)
    {
        cout << "Deleting the message queue with qid: " << qid << " from Receiver A" << endl;
        msgctl(qid, IPC_RMID, NULL);
    }
    exit(0);
}