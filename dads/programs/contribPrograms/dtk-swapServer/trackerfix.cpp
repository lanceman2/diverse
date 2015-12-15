#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <dtk.h>
//#include <expect.h>
#include "lswap.h"
using namespace std;

//Signal handler
void sigchild_handler (int s)
{
	while (wait(NULL) > 0)
		signal( SIGCHLD, sigchild_handler );
}

//The function to connect to the tracker server
//void connectToIO();

//The main function;
int main(int argc, char** argv)
{
	//Check to see if the server is already running by writing a specific
	//value to shared memory
	dtkSharedMem* test = new dtkSharedMem(sizeof(int), "tracker-server");
	int Dead=1337;
	test->write(&Dead);
	delete test;
	//Now fork off so that I can become a daemon
	int pid=fork();
	//Checks to see what is going on
	//Error check
	if (pid <0)
	{
		printf("Error, could not fork\n");
		exit(1);
	}
	//Parent goes bye bye
	else if (pid > 0)
	{
		exit(0);
	}
	//Become a daemon
	if (setsid() < 0)
	{
		printf("Error, could not become a daemon\n");
		exit(1);
	}
	//Setup proper signal handeling
/*	struct sigaction sa;
	sa.sa_handler = sigchild_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}*/
	signal( SIGCHLD, sigchild_handler );
	//Run system commands to reset the state of the DTK server
	system("dtk-shutdownServer");
	system("killall -9 dtk-server");
	system("dtk-destroySharedMem -r");
	system("dtk-destroySharedMem -r");
	system("dtk-server -d");
	//Create a linux swap class that handles mapping the shared memory
	//etc... This is reused from the old implementation, go reuse!
	lswap mySwap;
	//Connect to io.sv.vt.edu using libexpect, don't look at the code
	//it will make your head hurt
//	connectToIO();
	//Shared memory segment that listens for the kill command
	dtkSharedMem readSeg(sizeof(int),"tracker-server");
	//Make sure I get the kill command
	readSeg.queue();
	//Value I read into for DTK
	int Val;
	
	//Connect the two systems
	mySwap.setup();
	//Main loop
	while (1)
	{
		//Update devices and such
		mySwap.serve();

		//Check to see if the server needs to die
		if (readSeg.read(&Val))
			exit(0);
		if (Val == 1337)
			exit(0);
		//Sleep so we dont' eat the cpu
		usleep(10000);
	}
	return 0;
}

/*void connectToIO()
{
	int MyFD;
	exp_loguser = 1;
	exp_timeout = 10;
	//exp_timeout = 100000000000;
*/
	/*
	string password="34a$@711";
	string user="root";
	string connectCommand="ssh";
	string clientCommand="resetis900";
	*/
/*
	//exp_is_debugging=1;
	//exp_debugfile = fdopen(0, "w");
	MyFD = exp_spawnl("ssh", "ssh", "root@io.sv.vt.edu", (char*)0);
	if ( 0 > MyFD)
	{
		printf("Error connecting to io.sv.vt.edu\n");
	}
	//exp_expectl(MyFD, exp_glob, "password:\r\n",0,exp_end);
*/
	/*
	char BUFF[200];
	int numRead = read(MyFD, BUFF, 30);
	BUFF[numRead]='\0';
	//Add in a blasted gui for the users later
	if (strcmp("root@io.sv.vt.edu's password: ", BUFF) !=0)
	{
		printf("ERROR:  Please type ssh root@io.sv.vt.edu\n");
		printf("in a terminal and then type yes when prompted\n");
		printf("Next press control + c then re-run the script again");
	}
	printf("Num read = %d\n",numRead);
	printf("|%s|\n", BUFF);
	char* password="34a$@711";
	char* command="resetis900";
	write(MyFD, password, strlen(password));
	write(MyFD, command, strlen(command));
	FILE* writeFP;
	writeFP=fdopen(MyFD, "w");
	fprintf(writeFP,password);
	fprintf(writeFP,command);
	*/
/*
	//printf("STARTING\n");
	int result = exp_expectl(MyFD, exp_glob, "yes", 0, exp_end);
	fprintf(stderr, "Result is %d\n", result);
	if (result != -1 && result != -2)
	{
		//printf("Need to say yes AND DID \n");
		result = exp_expectl(MyFD, exp_glob, "/no)?", 0, exp_end);
		char* accept="yes\r\n";
		write(MyFD, accept, strlen(accept));
		result = exp_expectl(MyFD, exp_glob, "yes\r\n", 0, exp_end);
		result = exp_expectl(MyFD, exp_glob, "\r\nWarning: Permanently added 'io.sv.vt.edu,128.173.49.145' (RSA) to the list of known hosts.\r\r\n", 0, exp_end);
	}

	result = exp_expectl(MyFD, exp_glob, "root@io.sv.vt.edu's password: ", 0, exp_end);

	//fprintf(stderr, "INFO IS %d\n", result);
*/
/*
	if (-1 == result)
		printf("Ruh roh\n");
	else
		printf("YES READY\n");
*/
/*
	char* command="34a$@711\r\n";
	write(MyFD, command, strlen(command));
	result = exp_expectl(MyFD, exp_glob, ".*", 0,exp_end);
*/
	/*
	char BUFF[200];
	int numRead = read(MyFD, BUFF, 200);
	BUFF[numRead]='\0';
	printf("%s\n", BUFF);
	*/
/*
	char* resetcommand="resetis900&\r\n";
	write(MyFD, resetcommand, strlen(resetcommand));

	result = exp_expectl(MyFD, exp_glob, ".*", 0,exp_end);

	char* diecommand="exit\r\n";
	write(MyFD, diecommand, strlen(diecommand));
*/
	/*
	numRead = read(MyFD, BUFF, 200);
	BUFF[numRead]='\0';
	printf("%s\n", BUFF);
	*/
/*
	sleep(1);
}
*/
