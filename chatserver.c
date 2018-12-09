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
	char buf[BUFSIZE];
	int i,j,num_clnt = 0;
	int strLen,clntsLen;
	char name[MAX][10],msg[BUFSIZE];
	int max = 0;

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

	while(1){
		FD_ZERO(&read_set);
		max = sock_id + 1;
		if(num_clnt>0){
			for(i=0;i<num_clnt;i++){
				if(clntsock[i]+1>max)
					max=clntsock[i]+1;
			}
		}

		FD_SET(sock_id,&read_set);

		for(i=0; i<num_clnt; i++)
			FD_SET(clntsock[i],&read_set);
		if(select(max,&read_set,(fd_set*)0,(fd_set*)0,(struct timeval*)0)<0){	
			printf("select error\n");
			exit(1);
		}
		if(FD_ISSET(sock_id,&read_set)!=0){
			clntsLen = sizeof(caddr);
			clntfd = accept(sock_id,(struct sockaddr *)&caddr,&clntsLen);
			if(clntfd != -1){
				clntsock[num_clnt] = clntfd;
				strLen=read(clntfd,name[num_clnt],10);
				name[num_clnt][strLen-1]='\0';
				num_clnt++;
				printf("number of user : %d\n",num_clnt);
			}
		}
		for(i = 0; i<num_clnt; i++){
			if(FD_ISSET(clntsock[i],&read_set)){
				if((strLen=read(clntsock[i],buf,BUFSIZE))<=0){
					close(clntsock[i]);
					if( i != num_clnt -1){
						clntsock[i] = clntsock[num_clnt - 1];
						strcpy(name[i],name[num_clnt -1]);
					}
					printf("number of user : %d\n",--num_clnt);
					continue;
				}
				buf[strLen] = '\0';
				for(j=0;j<num_clnt;j++){
					if(i!=j){
						sprintf(msg,"[%s] %s",name[i],buf);
						write(clntsock[j],msg,strlen(msg));
					}
				}
			}
		}

	}

}
