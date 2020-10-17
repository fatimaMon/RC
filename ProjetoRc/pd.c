#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#define IPSIZE 20
#define PORTSIZE 5 
#define MAXSIZE 300
#define PORT "58011"


int fd,errcode, num, uid;
ssize_t n;
char message[MAXSIZE], msg[2], password[8], command[MAXSIZE];
char str_value[MAXSIZE];

socklen_t addrlen, i;
char pdIP[IPSIZE], pdPort[PORTSIZE], asIP[IPSIZE], asPort[PORTSIZE];
struct addrinfo hints,*res;
struct sockaddr_in addr;
char buffer[128];


int main(int argc, char* argv[])
{

	if ( argc <8 && argc>9)
	{
		printf(" Ivalid Arguments\n" );
		exit(1);
	}
	else
	{
		if( strcmp(argv[4], "-n") != 0 || strcmp(argv[6], "-p") != 0)
		{
			printf("Invalid Arguments");
			exit(1);
		}
		else
		{
			strcpy(pdIP, argv[1]);
			strcpy(pdPort, argv[3]);
			strcpy(asIP, argv[5]);
			strcpy(asPort, argv[7]);
			
		}
	}


fd = socket(AF_INET,SOCK_DGRAM,0);
if(fd == -1) exit(1);

memset(&hints,0,sizeof hints);
hints.ai_family=AF_INET;
hints.ai_socktype=SOCK_DGRAM;

errcode = getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);

if(errcode != 0)
{
	exit(1); // error
}
  
memset(message, 0, 18);
while(fgets(message, MAXSIZE, stdin) != NULL)
{
	num = sscanf(message, "%s %d %s", command, &uid, password);
	if(strcmp(command, "reg") == 0)
	{
		
		sprintf(message, "REG %d %s 193.136.128.108 57060\n",uid, password);
	}
	else if ( strcmp(command,"exit") == 0 )
	{
		sprintf(message, "UNR %d %s", uid, password);
	}
	else
	{
		printf("Error: Command not valid\n");
	}

	n=sendto(fd,message,41,0,res->ai_addr,res->ai_addrlen);
	//n=sendto(fd,message,sizeof(message),0,res->ai_addr,res->ai_addrlen);	
	memset(message, 0, MAXSIZE);

	if(n == -1)
	{
		printf("Error: Sending Message to AS Server.\n");
		exit(1)
	}

	addrlen=sizeof(addr);
	n = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
	if( n == -1)
	{
		printf("Error: Receiving Message From AS Server.\n");
		close(fd);
		exit(1);
	}
	else
	{
		if(strcmp(message,"RRG OK\n") == 0)
		{
		printf("Registration successful\n");
		}

		else if (strcmp(message, "RUN OK") == 0)
		{
			//freeaddrinfo(res);
			close(fd);
		}

		else if(strcmp(message, "RUN NOK") == 0)
		{
			printf("Unregister Failed\n");
		}
	
	
	}

}

write(1,buffer,n);

freeaddrinfo(res);
close(fd);

return 0;
}




