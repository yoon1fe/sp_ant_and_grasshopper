#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

#define BUFSIZE 100

void main(int ac,char *av[]){
	struct sockaddr_in saddr;
	int sock_id;
	char myname[20];
	char buf[BUFSIZE],msg[BUFSIZE];
	fd_set read_set;
	struct hostent *hp;
	int strLen,n;

	if(ac != 3){
		printf("usage: %s IPaddress PORTNUM\n",av[0]);
		exit(1);
	}

	if((sock_id = socket(PF_INET, SOCK_STREAM, 0))==-1)
		exit(1);

	bzero((void*)&saddr,sizeof(saddr));
	hp = gethostbyname(av[1]);

	bcopy(hp->h_addr,(struct sockaddr*)&saddr.sin_addr,hp->h_length);
	saddr.sin_port = htons(atoi(av[2]));
	saddr.sin_family = AF_INET;

	if(connect(sock_id,(struct sockaddr*)&saddr,sizeof(saddr))!=0){
		printf("connect error\n");
		exit(1);
	}

	printf("connet server\n");
	printf("NAME : ");
	scanf("%s",myname);

	FD_ZERO(&read_set);
	while(1){
		FD_SET(sock_id,&read_set);
		FD_SET(0,&read_set);

		if(select(sock_id+1,&read_set,0,0,0)<0){
			printf("select error\n");
			exit(1);
		}
		if(FD_ISSET(sock_id,&read_set)){
			if((strLen = recv(sock_id,buf,BUFSIZE,0))>0){
				buf[strLen] = '\0';
				printf("%s\n",buf);
			}
		}
		if(FD_ISSET(0,&read_set)){
			if((fgets(buf,BUFSIZE,stdin))>0){
				if(strcmp(buf,"quit")==0){
					printf("bye\n");
					close(sock_id);
					exit(0);
				}
				sprintf(msg,"[%s] %s",myname,buf);
				if(send(sock_id,msg,strlen(msg),0)<0)
					printf("send error\n");
			}
		}
	}
}
