#include "queueWrapper.h"


void QueueWrapper::controlMsgFreq(int qid, int size) {
		do{
				msgctl(qid, IPC_STAT, &qInfo);
		}while((qInfo.msg_qbytes/size)-2 <= qInfo.msg_qnum); //while the number of messages in the queue are greater than or equal the max - 2
}

QueueWrapper::QueueWrapper(){
		this->size = sizeof(this->msg)-sizeof(int);
		this->qid = msgget(ftok(".",'u'), IPC_CREAT|0600);
		this->pid = getpid();
}

void QueueWrapper::sendMsg(long mtype, int message){
		this->msg.mtype = mtype;
		this->msg.message = message;
		msgsnd(qid, (struct buf*)&msg, size, 0);
}

void QueueWrapper::receiveMsg(long mtype){
		msgrcv(qid, (struct buf*)&msg, size, mtype, 0);
}

int QueueWrapper::getMsg(){
		return msg.message;
}

void QueueWrapper::printReplyMsg(int pid){
		std::cout << pid << ": gets reply" << std::endl;
		std::cout << "reply: " << msg.message << std::endl << std::endl; 
}