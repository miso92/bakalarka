#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

extern int port;    // socket port number where we should be listening

extern int packetLength;   // length of the packet received
extern char *packetEnd;    // end of received packet (only for decoding)

int initialize_networking(int network_port = 51);
int listen_to_clients();
int sendPacket(SOCKET s, const char *p, size_t size);
char *receivePacket(SOCKET s);


#endif