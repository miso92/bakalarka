#include "networking.h"
#include "imagine_communication.h"

int packetLength;   // length of the packet received
char *packetEnd;    // end of received packet (only for decoding)
static SOCKET sock;        // socket we are listening at
int port;    // socket port number where we should be listening

int initialize_networking(int network_port)
{
	port = network_port;

	// initialize windows sockets
	WSADATA wsaData;
	if (int err = WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		fprintf(stderr, "Error initializing windows sockets: %d\n", err);
		return 0;
	}

	// create socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		fprintf(stderr, "Error creating socket: %ld\n", WSAGetLastError());
		return 0;
	}

	// set up the sockaddr structure
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = ADDR_ANY;
	service.sin_port = htons((u_short)port);
	char enable = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	//bind the socket
	if (bind(sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		fprintf(stderr, "bind() failed, error=%ld.\n", WSAGetLastError());
		closesocket(sock);
		return 0;
	}

	// listen on the socket
	if (listen(sock, 1) == SOCKET_ERROR)
	{
		fprintf(stderr, "listen() failed, error=%ld.\n", WSAGetLastError());
		closesocket(sock);
		return 0;
	}
	return 1;
}

int listen_to_clients()
{
	// repeat forever:
	while (1)
	{
		sockaddr_in otherAddr;
		if (debug) printf("listening on port %d...\n", port);

		// accept a new connection
		int l = sizeof(otherAddr);
		SOCKET clientSocket = accept(sock, (SOCKADDR*)&otherAddr, &l);
		if (clientSocket == INVALID_SOCKET)
		{
			int er = WSAGetLastError();
			fprintf(stderr, "accept() with error %d\n", er);

			if (er == WSAEINTR)  // this indicates that somebody killed us, terminate
			{
				closesocket(sock);
				return 1;
			}
		}

		if (debug)
		{
			char ip[100];
			strncpy_s(ip, 100, inet_ntoa(otherAddr.sin_addr), 99);
			int peerPort = ntohs(otherAddr.sin_port);
			printf("accepted connection from %s:%d\n", ip, peerPort);
		}

		handle_client(clientSocket);

	} // while(1)
}

int sendPacket(SOCKET s, const char *p, size_t size)
{
	size_t bytesSent = 0;

	if (debug)
	{
		printf("--- %d bytes -> Imagine: \n", size);
		for (size_t i = 0; i < size; i++) printf("%d ", p[i]);
		printf("\t");
		for (size_t i = 0; i < size; i++) printf("%c", (p[i] >= 32) ? p[i] : '.');
		printf("\n");
	}
	do {
		int bs = send(s, p + bytesSent, size - bytesSent, 0);
		if (bs == SOCKET_ERROR)
		{
			if (debug) printf("socket error %ld in send() while sending login confirmation, closing connection\n", WSAGetLastError());
			closesocket(s);
			return 0;
		}
		bytesSent += bs;
	} while (bytesSent < size);

	return 1;
}

char *receivePacket(SOCKET s)
{
	char recvbuf[20];
	int delta = 0;
	int packetSize, wsaerr = 0;
	// receive packet header (<07><len><!>) 
	do {  //we have to loop for the case only part of the header will be received
		packetSize = recv(s, ((char *)recvbuf) + delta, 1, 0);
		if (packetSize > 0) delta++;
	} while ((delta < 20) && (recvbuf[delta - 1] != '!') && (packetSize > 0));

	if (packetSize < 0) wsaerr = WSAGetLastError();

	// if connection closed, return null
	if ((packetSize == 0) || (wsaerr == WSAECONNRESET))
		return 0;

	//if error occured, report and return null
	if (packetSize < 0)
	{
		fprintf(stderr, "error %ld reading socket\n", wsaerr);
		return 0;
	}

	//'!' did not arrive? bad header? faulty packet
	if ((delta >= 20) || (recvbuf[0] != 7))
	{
		fprintf(stderr, "Ignoring incorrect packet (%d,%c,%d)\n", delta, recvbuf[0], packetSize);
		return 0;
	}

	// retrieve packet length
	recvbuf[delta - 1] = 0;
	sscanf_s(recvbuf + 1, "%d", &packetLength);

	//if (debug) printf("Imagine -> %d bytes\n", packetLength);
	char *packet = (char *)malloc(packetLength + 1);
	if (!packet)
	{
		fprintf(stderr, "Could not allocate memory for packet of size %d\n", packetLength);
		return 0;
	}
	delta = 0;
	wsaerr = 0;
	// receive packet body  
	do {
		packetSize = ::recv(s, packet + delta, packetLength - delta, 0);
		if (packetSize > 0) delta += packetSize;
	} while ((delta < packetLength) && (packetSize > 0));

	if (packetSize < 0) wsaerr = WSAGetLastError();

	// if connection closed, return null
	if ((packetSize == 0) || (wsaerr == WSAECONNRESET))
	{
		if (debug) printf("recv(): packetSize=%d, wsaerr=%d\n", packetSize, wsaerr);
		return 0;
	}
	// if error occured, report and return null
	if (packetSize < 0)
	{
		fprintf(stderr, "error %ld while reading socket\n", wsaerr);
		return 0;
	}

	packet[packetLength] = 0;
	packetEnd = packet + packetLength;

	if (debug)
	{
/*		printf("%d bytes -> daemon\n", packetLength);
		for (int i = 0; i < packetLength; i++) printf("%d ", packet[i]);
		printf("\t");
		for (int i = 0; i < packetLength; i++) printf("%c", (packet[i] >= 32) ? packet[i] : '.');
		printf("\n"); */
	}

	return packet;
}