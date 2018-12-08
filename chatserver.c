#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFSIZE 100
#define MAX 100

void main(int ac, char *av[]){
	struct sockaddr_in saddr,caddr;
	struct hostent* hp;
	int sock_id, sockNum, clntsock[MAX],clntfd;
	char hostname[256];
	fd_set read_set;
	char buf[BUFSIZE],welcome[BUFSIZE] = "WELCOME!\n";
	int i,j,num_clnt = 0;
	int strLen,clntsLen;
	struct timeval timeout;

/*	if((sock_id = socket(PF_INET, SOCK_STREAM,0))== -1)
			exit(1);

	bzero((void*)&saddr, sizeof(saddr));
	gethostname(hostname, 256);
	hp = gethostbyname(hostname);
	bcopy((void*)hp->h_addr,(void*)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = atoi(av[1]);
	saddr.sin_family = AF_INET;

	if(bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr))!= 0 )
		exit(1);
	if(listen(sock_id,MAX) != 0)
		exit(1);
*/
	sock_id = make_server_socket(atoi(av[1]));
	if(sock_id == -1)
		exit(1);
	sockNum = sock_id + 1;

	FD_ZERO(&read_set);
	while(1){
		timeout.tv_sec = 1;
		timeout.tv_usec = 1000;
		if(num_clnt>0)
			sockNum = clntsock[num_clnt-1]+1;
		FD_SET(sock_id,&read_set);

		for(i=0; i<num_clnt; i++)
			FD_SET(clntsock[i],&read_set);
		if(select(sockNum,&read_set,(fd_set*)0,(fd_set*)0,(struct timeval*)0)<0){	
			printf("select error\n");
			exit(1);
		}
		if(FD_ISSET(sock_id,&read_set)!=0){
			clntsLen = sizeof(caddr);
			clntfd = accept(sock_id,(struct sockaddr *)&caddr,&clntsLen);
			if(clntfd != -1){
				clntsock[num_clnt] = clntfd;
				num_clnt++;
				write(clntfd,welcome,strlen(welcome));
				printf("%d clnt join\n",num_clnt);
			}
		}
		for(i = 0; i<num_clnt; i++){
			if(FD_ISSET(clntsock[i],&read_set)){

				if((strLen=read(clntsock[i],buf,BUFSIZE))> 0){
					buf[strLen] = '\0';
				}
				else{
					printf("shutdown\n");
					close(clntsock[i]);
					if( i != num_clnt -1)
						clntsock[i] = clntsock[num_clnt - 1];
					num_clnt--;
					continue;
					
				}
				for(j=0;j<num_clnt;j++){
					write(clntsock[j],buf,strlen(buf));
				}
			}
		}

	}

}
