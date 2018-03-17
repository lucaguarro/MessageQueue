#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

int main() {
    cout << "randmax: " << RAND_MAX << endl;
    srand(time(NULL));
    for(int i = 0; i < 10; i++){
        cout << rand() % RAND_MAX + 2 << endl;
    }
    /*int qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
    cout << "the qid " << qid << endl;
    cout << "errno: " << errno << endl;
    cout << "EEXIST: " << EEXIST << endl;
    struct buf {
        long mtype; // required
        int message; // mesg content
    };
    buf msg;
    int size = sizeof(msg)-sizeof(long);

    msg.mtype = 1;
    msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sending
    cout << msg.message << endl;
    //Get acknowledgement from receiverA
    cout << "message received: " << msgrcv(qid, (struct msgbuf *)&msg, size, 4, 0) << endl; // read mesg
    cout << "errno: " << errno << ", EIDRM: " << EIDRM;
    cout << msg.message << endl;
    cout << "ipc no wait: " << IPC_NOWAIT << endl;
    cout << "Errors:" << endl;
    cout << "EINVAL " << EINVAL << endl;
    cout << "EACCES " << EACCES << endl;
    cout << "E2BIG " << E2BIG << endl;
    cout << "ENOMSG " << ENOMSG << endl;
    cout << "EFAULT " << EFAULT << endl;
    cout << "EINTR " << EINTR << endl;*/
}