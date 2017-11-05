#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"
void *thread(void *);
void service(int );
void initS(char []);
void listAllPid(char []);
void statusInfo(char [],char [],char []);
void childPid(char [],char []);
void cmdline(char [],char []);
void getAncestor(char [],char []);
void allAncestor(char [],char []);
#endif
