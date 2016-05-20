#ifndef __IMAGINE_COMMUNICATION_H__
#define __IMAGINE_COMMUNICATION_H__

#include "Controller.h"
#include "networking.h"

#define MAX_RECIPIENT_LEN 10

int handle_client(SOCKET clientSocket);
int pack_and_send_to_Imagine(char *buf);

#endif