/*
* Pavitar Singh
* Course-Section: CS 360 01, Spring 2019
* Assignment: Project 2
* Collaborator: Dr. James Jerkins
* Resouses: https://tools.ietf.org/html/rfc867
			http://www.buildingthepride.com/faculty/jajerkins/cs360/signals.pdf
			https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
			https://www.programmingsimplified.com/c-program-concatenate-strings
			https://www.techonthenet.com/c_language/standard_library_functions/time_h/strftime.php
* Description: The Server will check for the errors on each step
				of Creating Socket, Binding, Listening then Using the Time function 
				get the each part of the time and then concatinate and send it 
				to the client.
*/
#include <netdb.h> 
#include <netinet/in.h> 

#include <sys/socket.h> 
#include <sys/types.h> 

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>

#define MAX 80
#define PORT 1113
#define SA struct sockaddr

void sig_handler(int);

volatile sig_atomic_t cleanup_exit;

int
main()
{
        int SockFrwd, ConnFrwd, Length;
		struct SockAddr ServerAddr, Client;
		
		//Socket created or not ??
		SockFrwd = socket(AF_INET, SOCKSTREAM, 0);
		if(SockFrwd == -1){
			printf("Creation of Socket Failed!!!\n");
			exit(0);
		}
		else
			printf("Socket Successfully created\n");
		bzero(&ServerAddr, sizeof(ServerAddr));
		
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		ServerAddr.sin_port = htons(PORT);
		
		//Checking Binding
		if((bind(SockFrwd, (SA*)&ServerAddr, sizeof(ServerAddr))) !=0){
			printf("Binding of Socket failed!!!\n");
			exit(0);
		}
		else
			printf("Socket Binding Successfull\n");
		
		//Checking Listening
		if((listen(SockFrwd, 5)) != 0){
			printf("Listening Failed!!!\n");
			exit(0);
		}
		else
			printf("Server is listening...\n");
		len = sizeof(Client);
		
		//Accept the packet
		ConnFrwd = accept(SockFrwd, (SA*)&Client, &Length);
		if(ConnFrwd < 0){
			printf("Server cannot accept!!!\n");
			exit(0);
		}
		else
			printf("Accepting...\n");
		cleanup_exit = false;
		
		signal(SIGINT, sig_handler);
		signal(SIGTERM, sig_handler);
		
		puts("Program running...\n");
		while(! cleanup_exit){
		char buff[MAX];
		char WeekDay[10], Month[10], Day[3], Year[5], Time[15], Zone[5];
		char Space = {" "};
		char Comma = {","};
		char Dash = {"-"};
		
		time_t t;
		t = time(NULL);
		// Get weekday, Month DAy, Year Time - Zone
		strftime(WeekDay, sizeof(WeekDay), "%A",localtime(&t));
		strftime(Month, sizeof(Month), "%B",localtime(&t));
		strftime(Day, sizeof(Day), "%d",localtime(&t));
		strftime(Year, sizeof(Year), "%Y",localtime(&t));
		strftime(Time, sizeof(Time), "%X",localtime(&t));
		strftime(Zone, sizeof(Zone), "%Z",localtime(&t));
		// Concatinate into one cstring
		strcat(buff, WeekDay);
		strcat(buff, Comma);
		strcat(buff, Space);
		strcat(buff, Month);
		strcat(buff, Day);
		strcat(buff, Comma);
		strcat(buff, Space);
		strcat(buff, Year);
		strcat(buff, Space);
		strcat(buff, Time);
		strcat(buff, Dash);
		strcat(buff, Zone);
		
		write(SockFrwd, buff, sizeof(buff));
		
		}
		puts("Cleaning and exiting\n")
		
		close(SockFrwd);
}

void
sig_handler(int sig)
{
		// save errno if you are doing anything that could trigger an error
		// i.e. anything that is not async-signal-safe
		int save_errno = errno;
		
		switch (sig) {
		case SIGINT :
			puts("CTRL-C pressed");
			break;
		case SIGTERM :
			puts("SIGTERM received");
			cleanup_exit = true;
			break;
		default :
			printf("%d not handled\n", sig);
		}
		errno = save_errno;
}