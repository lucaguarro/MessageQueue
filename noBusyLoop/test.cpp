/* A C program that does not terminate when Ctrl+C is pressed */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;
//g++ -o test.out test.cpp
/* Signal Handler for SIGINT */
void sigintHandler(int sig_num)
{
    cout << "the sig_num is: " << sig_num << endl;
    switch (sig_num) {
    case SIGSEGV:
        cout << "give out a backtrace or something...\n" << endl;
        abort();
    case SIGINT:
        cout << "\n Cannot be terminated using Ctrl+C \n" << endl;
        abort();  
    default:
        cout << "wasn't expecting that!\n" << endl;
        abort();
    }
}
 
int main ()
{
    cout << getpid() << endl;
    /* Set the SIGINT (Ctrl-C) signal handler to sigintHandler 
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    signal(SIGSEGV, sigintHandler);
    signal(10, sigintHandler);
 
    /* Infinite loop */
    while(1)
    {        
    }
    return 0;
}