#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define BUFSIZE 100

void errorHandling(char* msg);
void readRoutine(int sock, char* buf);
void writeRoutine(int sock, char* buf, char* myname);

int main(int ac, char* av[])
{
	int sock;
	pid_t pid;
	char buf[BUFSIZE];
	char myname[10],msg[BUFSIZE];
	struct sockaddr_in servAddr;
	struct hostent *hp;

	if( ac != 3 )
	{
		printf("Usage:%s <ip><port>\n", av[0]);
		exit(1);
	}

	/*sock = socket(PF_INET, SOCK_STREAM, 0);

	bzero(&servAddr,sizeof(servAddr));
	hp = gethostbyname(argv[1]);

	bcopy(hp->h_addr,(struct sockaddr*)&servAddr.sin_addr,hp->h_length);
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(atoi(argv[2]));

	connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr))*/

	sock = connect_to_server(av[1],atoi(av[2]));

	printf("NAME : ");
	scanf("%s",myname);

	pid = fork();

	if( pid == 0 )
		writeRoutine(sock, buf,myname);

	else
		readRoutine(sock, buf);

	while(wait(0)!=0);

	close(sock);

	return 0;

}
void readRoutine(int sock, char* buf)
{
	while(1)
	{
		int strLen = read(sock, buf, BUFSIZE);
		if( strLen == 0 )
			return ;

		buf[strLen] = 0;
		printf("%s", buf);
	}
}

void writeRoutine(int sock, char* buf,char* myname)
{
	char msg[BUFSIZE];
	while(1)

	{
		fgets(buf, BUFSIZE, stdin);
		if( !strcmp(buf, "q\n") || !strcmp(buf, "Q\n") )
		{
			shutdown(sock, SHUT_WR);
			return;
		}
		sprintf(msg,"[%s] %s",myname,buf);
		write(sock, msg, strlen(msg));

	}
}
