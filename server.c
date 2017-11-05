#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <dirent.h>
#include "server.h"

int main(int argc, char *argv[])
{
	//socket的建立
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		printf("Fail to create a socket.");
	}

	//socket的連線
	struct sockaddr_in serverInfo;
	bzero(&serverInfo,sizeof(serverInfo));

	serverInfo.sin_family = PF_INET;
	serverInfo.sin_addr.s_addr = INADDR_ANY;
	serverInfo.sin_port = htons(59487);
	bind(sockfd,(struct sockaddr *)&serverInfo,
	     sizeof(serverInfo));
	listen(sockfd,10);

	int forClientSockfd = 0;
	struct sockaddr_in clientInfo;
	int addrlen = sizeof(clientInfo);


	while(1) {

		forClientSockfd = accept(sockfd,
		                         (struct sockaddr*) &clientInfo, &addrlen);
		service(forClientSockfd);
	}
	return 0;
}
void *thread(void *argu)
{
	int i;
	int *csfd = argu;
	int forClientSockfd = *csfd;
	char msg[100] = {};
	char ans[5000] = {};
	char pid[100] = {};
	recv(forClientSockfd,msg,sizeof(msg),0);
	//do something to msg
	printf("get:%s",msg);
	for(i=2; i<100; i++) {
		if(msg[i]<'0' || msg[i]>'9') break;
		pid[i-2] = msg[i];
	}
	printf("pid:%s\n",pid);
	if(msg[0]=='a') {
		listAllPid(ans);
	} else if(msg[0]=='b') {
		statusInfo(ans,pid,"Tgid");
	} else if(msg[0]=='c') {
		childPid(ans,pid);
	} else if(msg[0]=='d') {
		statusInfo(ans,pid,"Name");
	} else if(msg[0]=='e') {
		statusInfo(ans,pid,"State");
	} else if(msg[0]=='f') {
		cmdline(ans,pid);
	} else if(msg[0]=='g') {
		statusInfo(ans,pid,"PPid");
	} else if(msg[0]=='h') {
		allAncestor(ans,pid);
	} else if(msg[0]=='i') {
		statusInfo(ans,pid,"VmSize");
	} else if(msg[0]=='j') {
		statusInfo(ans,pid,"VmRss");
	} else {}

	//
	printf("send:%s\n",ans);
	send(forClientSockfd,ans,sizeof(ans),0);
	pthread_exit(NULL);
	return NULL;
}

void service(int csfd)
{
	pthread_t t;
	pthread_create(&t,NULL,&thread,(void*) &csfd);
}

void initS(char s[])
{
	int i;
	for(i=0; i<5000; i++) {
		s[i] = '\0';
	}
}
void listAllPid(char s[])            //for a
{
	initS(s);
	strcat(s,"All Pids:\n");
	DIR * dir;
	struct dirent * ptr;
	dir =opendir("/proc/");
	while((ptr = readdir(dir))!=NULL) {
		if(atoi(ptr->d_name)!=0) {
			strcat(s,ptr->d_name);
			strcat(s," ");
		}
	}
	strcat(s,"\n");
	closedir(dir);
}

void statusInfo(char s[],char pid[],
                char info[])    //for b,d,e,g,i,j
{
	initS(s);
	char path[100] = {"/proc/"};
	strcat(path,pid);
	strcat(path,"/status");
	FILE *fp = fopen(path,"r");
	if(fp==NULL) {
		strcpy(s,"info does not exist");
		return;
	}
	char c;
	char status[1000] = {};
	int i = 0;
	while(c != EOF) {
		c = fgetc(fp);
		status[i] = c;
		i++;
	}
	i = 0;
	char *p = strstr(status,info);
	if(p == NULL) {
		strcpy(s,"info does not exist");
		return;
	}
	for(i=0; i<5000; i++) {
		if(p[i]=='\n') break;
		s[i] = p[i];
	}
}
void childPid(char s[],
              char pid[])               //for c
{
	initS(s);
	strcat(s,"childen's Pids:\n");
	char path[100] = {"/proc/"};
	strcat(path,pid);
	strcat(path,"/task/");
	strcat(path,pid);
	strcat(path,"/children");
	FILE *fp = fopen(path,"r");
	if(fp==NULL) {
		strcpy(s,"info does not exist");
		return;
	}
	char c;
	char child[1000] = {};
	int i = 0;
	while(c != EOF) {
		c = fgetc(fp);
		child[i] = c;
		i++;
	}
	i = (int)strlen(child);
	child[i-1] = '\0';
	strcat(s,child);
}
void cmdline(char s[],
             char pid[])               //for f
{
	initS(s);
	strcat(s,"command line of excuting process(cmdline):\n");
	char path[100] = {"/proc/"};
	strcat(path,pid);
	strcat(path,"/cmdline");
	FILE *fp = fopen(path,"r");
	if(fp==NULL) {
		strcpy(s,"info does not exist");
		return;
	}
	char c;
	char cmd[1000] = {};
	int i = 0;
	while(c != EOF) {
		c = fgetc(fp);
		cmd[i] = c;
		i++;
	}
	i = (int)strlen(cmd);
	cmd[i-1] = '\0';
	strcat(s,cmd);
}
void getAncestor(char s[],char pid[])
{
	if(atoi(pid)==0) return;
	char path[100] = {"/proc/"};
	strcat(path,pid);
	strcat(path,"/status");
	FILE *fp = fopen(path,"r");
	if(fp==NULL) {
		strcpy(s,"info does not exist");
		return;
	}
	char c;
	char status[1000] = {};
	int i = 0;
	while(c != EOF) {
		c = fgetc(fp);
		status[i] = c;
		i++;
	}
	i = 0;
	char *p = strstr(status,"PPid");
	char a[100] = {};
	int j = 0;
	for(i=0; i<100; i++) {
		if(p[i]=='\n') break;
		if(p[i]>='0'&&p[i]<='9') {
			a[j] = p[i];
			j++;
		}
	}
	strcat(s,a);
	strcat(s," ");
	getAncestor(s,a);
}
void allAncestor(char s[],char pid[])    //for h
{
	initS(s);
	strcat(s,"All ancestor of Pids:\n");
	getAncestor(s,pid);
}

