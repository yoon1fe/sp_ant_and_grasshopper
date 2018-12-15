#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <curses.h>
#include <pthread.h>

#define BUFSIZE 100

void *interface(void*);
int sock_id;
void main(int ac,char *av[]){
	pthread_t t1,t2;
	struct sockaddr_in saddr;
	char buf[BUFSIZE];
	fd_set read_set;
	struct hostent *hp;
	int strLen,i=0;
	int c_y,c_x;
	char myname[10];
/*	if(ac != 3){
		printf("usage: %s IPaddress PORTNUM\n",av[0]);
		exit(1);
	}

	if((sock_id = socket(AF_INET, SOCK_STREAM, 0))==-1){
		printf("socket error\n");
		exit(1);
	}

	bzero((void*)&saddr,sizeof(saddr));
	hp = gethostbyname(av[1]);

	bcopy(hp->h_addr,(struct sockaddr*)&saddr.sin_addr,hp->h_length);
	saddr.sin_port = htons(atoi(av[2]));
	saddr.sin_family = AF_INET;

	if(connect(sock_id,(struct sockaddr*)&saddr,sizeof(saddr))==-1){
		printf("connect error\n");
		exit(1);
	}*/
	sock_id = connect_to_server(av[1],atoi(av[2]));

	printf("NAME : ");
	fgets(myname,10,stdin);
	write(sock_id,myname,strlen(myname));

	initscr();
	pthread_create(&t1,NULL,interface,(void*)&myname);
	while(1){
		FD_ZERO(&read_set);
		FD_SET(sock_id,&read_set);

		if(select(sock_id+1,&read_set,0,0,0)<0){
			printf("select error\n");
			exit(1);
		}
		if(FD_ISSET(sock_id,&read_set)){
			strLen = read(sock_id,buf,BUFSIZE);

			if(strLen==0){
				endwin();
				pthread_join(t1,NULL);
				close(sock_id);
				return;
			}
			else{
				buf[strLen] = '\0';
				if(i == LINES-1){
					move(0,0);
					deleteln();
					i--;
				}
				getyx(stdscr,c_y,c_x);
				move(i++,0);
				addstr(buf);
				move(LINES-1,c_x);
				refresh();
			}
		}
	}
}


void *interface(void* a)
{
	char msg[BUFSIZE],c;
	
	while(1){
		msg[0] = '\0';
		move(LINES-1,0);
		getnstr(msg,BUFSIZE);
		if(!strcmp(msg,"QUIT")){
			endwin();
			close(sock_id);
			exit(0);
		}
		write(sock_id,msg,strlen(msg));
		move(LINES-1,0);
		deleteln();
		refresh();
	}
}

