#include<iostream>
#include<signal.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

class QueueWrapper{
	struct buf {
		long mtype; // required
		int message; // mesg content
	};
    private:
        msqid_ds qInfo;
        int size;
        int qid;
        int pid;
        buf msg;
    public:
        QueueWrapper();
        void controlMsgFreq(int qid, int size);
        void sendMsg(long mtype, int message);
        void receiveMsg(long mtype);
        int getMsg();
        void printReplyMsg(int pid);
};