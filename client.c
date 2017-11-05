#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
	while(1) {
		//socket的建立
		int sockfd = 0;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		if (sockfd == -1) {
			printf("Fail to create a socket.\n");
			exit(0);
		}

		//socket的連線

		struct sockaddr_in info;
		bzero(&info,sizeof(info));
		info.sin_family = PF_INET;

		//localhost test
		info.sin_addr.s_addr = inet_addr("127.0.0.1");
		info.sin_port = htons(59487);


		int err = connect(sockfd,(struct sockaddr *)&info,
		                  sizeof(info));
		if(err==-1) {
			printf("Connection error\n");
			exit(0);
		}


		//Send a message to server
		char message[1000]= {};
		char receiveMessage[5000] = {};
		ui(message);
		send(sockfd,message,sizeof(message),0);
		recv(sockfd,receiveMessage,sizeof(receiveMessage),
		     0);

		printf("%s\n",receiveMessage);

		close(sockfd);
	}
}
void ui(char s[])
{
	char buf[10] = {};
	while(1) {
		s[0] = '\0';
		buf[0] = '\0';
		printf("=======================================================\n");
		printf("(a)list all process ids\n");
		printf("(b)thread's IDs\n");
		printf("(c)child's PIDs\n");
		printf("(d)process name\n");
		printf("(e)state of progress(D,R,S,T,t,W,X,Z)\n");
		printf("(f)command line of excuting process(cmdline)\n");
		printf("(g)parent's PID\n");
		printf("(h)all ancestors of PIDs\n");
		printf("(i)virtual memory size(VmSize)\n");
		printf("(j)physical memory size(VmRSS)\n");
		printf("(k)exit\n");
		printf("which? ");
		scanf("%s",buf);
		strcat(s,buf);
		strcat(s,"\n");
		if(s[0]=='k'&&s[1]=='\n') {
			printf("bye\n");
			exit(0);
		} else if(s[0]=='a'&&s[1]=='\n') {
			strcat(s,"0");
			strcat(s,"\n");
			return;
		} else if(s[0]>'a' && s[0]<='j'&&s[1]=='\n') {
			printf("pid? ");
			scanf("%s",buf);
			strcat(s,buf);
			strcat(s,"\n");
			return;
		} else printf("unexpected input.\n");
	}
}

