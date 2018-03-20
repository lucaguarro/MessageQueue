/*
    The second receiver gets messages from 257 & 997
    The second receiver terminates after it has received a total of 5000 messages
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
#include <signal.h>
using namespace std;

bool is251Alive = true;
bool is997Alive = true;
bool isReceiverAAlive = true;
bool is257Alive = true;


int main()
{
    cout << "ReceiverB starting..." << endl; // code that could cause exception

    /*
        Create the message queue and return the id. If it has already been created, just return its id.
    */
    int qid = msgget(ftok(".", 'u'), IPC_CREAT | 0600);

    /*
        Message structure for IPC
    */
    struct buf
    {
        long mtype; // required
        int message;
    };
    buf msg;
    int pid = getpid();

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
    int size = sizeof(msg) - sizeof(long);

    /*
        Receiver B is in execution until it receives its 5000th "real" message (AKA not alive/dead messages).
        We therefore create and initialize the message counter to 0.
    */
    int msgCount = 0;
    while (msgCount != 5000)
    {
        /*
            The first thing we do is send an alive message to all the other 4 processes to receive.
            The mtypes for alive/dead messages follow the following schema:
            ReceiverA: 100
            ReceiverB: 200 SKIP because this is Receiver B
            Sender251: 300
            Sender257: 400 
            Sender997: 500
        */
        msg.message = 143;
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
        /*
            After we send the alive messages, we expect there to be one alive/dead messages from each 
            other process that is or was just alive sent to ReceiverB using mtype = 200. 
            If a process is alive, we do not do anything. We only need to check for the dead messages
            which we know happen if the message value is in range [-1,-5]. Each value denotes the dead
            message for a particular process, according to the following schema:
            ReceiverA: -1
            ReceiverB: -2 SKIP because this is Receiver B
            Sender251: -3
            Sender257: -4 
            Sender997: -5 
            
            When we detect a dead message, we decrement numAlive and set the corresponding boolean to false.
        */
        for (int i = 0; i < numAlive; i++)
        {
            msgrcv(qid, (struct msgbuf *)&msg, size, 200, 0);
            switch (msg.message)
            {
                case -1: isReceiverAAlive = false; numAlive--; break;
                case -3: is251Alive = false; numAlive--; break;
                case -4: is257Alive = false; numAlive--; break;
                case -5: is997Alive = false; numAlive--; break;
            }
        }
        /*
            If Sender257 is still alive, we need to read messages from it. mtype = 2572 is used for "real" messages from Sender257. "Real" meaning
            not dead/alive messages.
        */
        if (is257Alive)
        {
            msgrcv(qid, (struct msgbuf *)&msg, size, 2572, 0);
            msgCount++; //increment message count
            cout << pid << ": gets reply" << endl;
            cout << "reply: " << msg.message << endl
                 << endl;
        }
        /*
            If Sender997 is still alive, we need to read messages from it. mtype = 9972 is used for "real" messages from Sender997. However since
            Sender 997 could die when this Receiver expects a message from it, Sender997 also sends a termination message (-5) to this mtype so we must
            catch it. If we detect a "real" message (i.e. one that is not a termination message), we must send an acknowledgement message to 997.
        */
        if (is997Alive)
        {
            msgrcv(qid, (struct msgbuf *)&msg, size, 9972, 0);
            if(msg.message == -5){
                is997Alive = false;
            }
            else if(msgCount != 5000) //There is a chance that the message just received from Sender 257 was the 5000th message
            {
                msgCount++; //increment message count
                cout << pid << ": gets reply" << endl;
                cout << "reply: " << msg.message << endl
                     << endl;
                //Send acknowledgement message
                msg.mtype = 2997;
                msg.message = 143;
                msgsnd(qid, (struct msgbuf *)&msg, size, 0);
            }
        }
    }

    /*
        When we exit the for loop, that means that ReceiverB has completed its job and is about to die. But before it dies,
        it must send a termination message to each one of the other processes if those processes are still alive. Staying consistent
        with the schema provided above, it uses message value -2 to denote that it is dying and the mtypes that the other processes
        use to read dead/alive messages.
    */
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
    /*
        Sends error message to both mtypes of 997 because we do not know where it is in its execution
    */
    if (is997Alive)
    {
        msg.mtype = 500;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        msg.mtype = 2997;
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    }

    /*
        If this process is the last to die, we must delete the message queue.
    */
    if (!isReceiverAAlive && !is251Alive && !is257Alive && !is997Alive)
    {
        cout << "Deleting the message queue with qid: " << qid << " from Receiver A" << endl;
        msgctl(qid, IPC_RMID, NULL);
    }
    exit(0);
}