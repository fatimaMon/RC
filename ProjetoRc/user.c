#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <conio.h>
#include <string.h>
#define IPSIZE 20
#define PORTSIZE 5 
#define PORT "58011"
#define MAXSIZE 300
#define FILESIZE 50

int main(int argc, char* argv[]){

int fd,errcode,num, uid, rid;
ssize_t n;
socklen_t addrlen;
char asIP[IPSIZE], asPort[PORTSIZE], fsIP[IPSIZE], fsPort[PORTSIZE];
char message[MAXSIZE], command[MAXSIZE], pass[MAXSIZE];
char fop, fname[FILESIZE];
struct addrinfo hints,*res;
struct sockaddr_in addr;
char buffer[MAXSIZE];



if ( argc <9 && argc> 10)
	{
		printf(" Ivalid Arguments\n" );
		exit(1);
	}
	else
	{
		if( strcmp(argv[1], "-n") != 0 || strcmp(argv[3], "-p") != 0
			|| strcmp(argv[5], "-m" )!= 0 || strcmp(argv[7], "-q")!=0)
		{
			printf("Invalid Arguments");
			exit(1);
		}
		else
		{
			strcpy(asIP, argv[2]);
			strcpy(asPort, argv[4]);
			strcpy(fsIP, argv[6]);
			strcpy(fsPort, argv[8]);
			
		}
	}


fd=socket(AF_INET,SOCK_STREAM,0);

if (fd==-1)
{
	printf("Error: Creating user socket.\n");
	exit(1);
} 

memset(&hints,0,sizeof hints);
hints.ai_family=AF_INET;
hints.ai_socktype=SOCK_STREAM;


errcode= getaddrinfo ("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);

if(errcode!=0)
{
	printf("Error:Trying to get server address.\n");
	exit(1);

}

n= connect (fd,res->ai_addr,res->ai_addrlen);
if(n==-1)
{	
	printf("Error: Trying to connect.\n");
	close(fd);
	exit(1);
}

while(fgets(message, sizeof(message), stdin) != NULL)
{
	sscanf(message,"%s", command);
	printf("command:%s\n", command);
	if(strcmp(command, "login") == 0)
	{

		sscanf(message,"%s %d %s", command, &uid, pass);
		sprintf(buffer, "LOG %d %s", uid, pass);
		n=write (fd,buffer,18);
	    n= read(fd,buffer, 128);
		if(strcmp(buffer, "RLO OK\n") == 0)
		{
			printf("You are now logged in.\n");

		}
		else if (strcmp(buffer, "RLO NOK\n") == 0)
		{
			printf("Your Password is incorrect.\n");
		}
				
		else
		{
			printf("Invalid input.\n");
		}
	}

	else if (strcmp(command,"req") == 0)
	{
		
		num = sscanf(message,"%s %c %s", command,fop, fname);
		rid = 1000 + (rand()%9999);
		if(fop == 'R' || fop == 'U' || fop == 'D') 
		{  /*verifica a operacao para mandar para o As o fname as well*/

			if(num == 3)
			{
				sprintf(buffer,"REQ %d %d %c %s",uid, rid, fop, fname);
				write(fd,buffer, 128);
			}
			else
			{
				printf("Missing the File Name.\n");
				continue;
			}
			
			
		}
		else
		{
			sprintf(buffer,"REQ %d %d %c",uid, rid, fop);
			write(fd,buffer,128);

		}

		memset(buffer,0, MAXSIZE);
		read(fd,buffer,128);	

		if(strcmp(buffer, "RRQ OK") == 0)
		{
			continue; // request coul be processed
		}	

		else if (strcmp(buffer, "RRQ ELOG") == 0)
		{
			printf("You are no log in.\n");
			continue;
		}

		else if (strcmp(buffer, "RRQ EPD") == 0)
		{
			printf("Message coul not be sent by the AS to PD.\n");
			continue;
		}

		else if (strcmp(buffer,"EUSER") == 0)
		{
			printf("Incorret uid.\n");
			continue;
		}

		else if (strcmp(buffer, "EFOP") == 0)
		{
			printf("File Operation is Invalid.\n");
			continue;
		}
		
		else
		{
			printf("Incorrect formatted request message.\n");
			continue;
		}
		

	}
	

}

write(1,buffer,n);
freeaddrinfo(res);
close (fd);

return 0;

}
