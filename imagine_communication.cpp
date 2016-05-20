#include "imagine_communication.h"
#define PACKET_FOR_CLIENT_MAX_SIZE 200
#define LOGIN_PACKET_CLIENT_NAME_OFFSET 3


static char packet4client[PACKET_FOR_CLIENT_MAX_SIZE];  // string buffer for constructing reply packet

static SOCKET clientSocket;

int handle_problem(char *problem_description)
{
	if (debug) printf(problem_description);
	closesocket(clientSocket);
	return 0;
}

int correct_login_packet(char *login_packet)
{
	return (strlen(login_packet) > 3) && (login_packet[0] == 'a') && (login_packet[2] == '!');
}

int receive_login_packet()
{
	char *login_packet = receivePacket(clientSocket);
	if (!login_packet) return handle_problem("Error reading login packet from the client, closing connection.\n");
	if (!correct_login_packet(login_packet)) return handle_problem("Incorrect login packet from client, closing connection.\n");
	free(login_packet);
	return 1;
}

int send_confirmation_packet(const char *confirmation_packet, int response_offset, char *response)
{
	int confirmation_packet_data_length = strlen(response) + response_offset;
	sprintf_s(packet4client, confirmation_packet, confirmation_packet_data_length, response);
	if (!sendPacket(clientSocket, packet4client, strlen(packet4client))) return 0;
	return 1;
}


static const char *login_confirmation_packet = "\007%d!b0!%s";

int send_login_confirmation_packet(short conn_id)
{
	return send_confirmation_packet(login_confirmation_packet, 3, "CL1");
}

static const char *response_data_packet = "\007%d!w1!3!OCL%s";
static const char *response_data_packet2 = "\007%d!w1!3!OCL";


int send_connect_confirmation_packet(short conn_id)
{
	char connid[12];
	sprintf_s(connid, "%d", conn_id);
	return send_confirmation_packet(response_data_packet, 8, connid);
}

static char *client_packet;

int handle_packet_problem(char *problem)
{
	if (debug) printf(problem);
	closesocket(clientSocket);
	if (client_packet) free(client_packet);
	client_packet = 0;
	return 0;
}

int correct_client_packet()
{
	return (strlen(client_packet) > 2);
}

char load_client_packet_type()
{
	return toupper(client_packet[0]);
}

const char *list_of_users_packet = "\00728!v2!3!OCL";

int handle_list_of_users()
{
	if (!sendPacket(clientSocket, list_of_users_packet, strlen(list_of_users_packet)))
		return handle_packet_problem("error sending list of users packet to client, closing connection\n");
	return 1;
}

static char *empty_list_packet = "\0078!l1!3!OCL";
static const int el_packet_length = 11;

int handle_list_message()
{

	if (!sendPacket(clientSocket, empty_list_packet, el_packet_length))
		return handle_packet_problem("error sending list message back to client, closing connection\n");
	return 1;
}


// message is encoded: chars with ascii < 48 are sent as #48#(a+48)
char *decode_low_characters(char *messageData)
{
	char *md = (char *)malloc(packetLength + 1), *rp = messageData;
	char *wp = md;
	while (rp < packetEnd)
	{
		if (*rp == '0')
		{
			rp++;
			*wp = (*rp) - 48;
		}
		else *wp = *rp;
		rp++;
		wp++;
	}
	return md;
}

static const char *ocl_ok_packet = "\00710!w1!3!OCLOK";
static const int ocl_ok_len = 14;
static const char *ocl_err_packet = "\00711!w1!3!OCLERR";
static const int ocl_err_len = 15;

static const char *response_packet;
static int response_len;

void determine_response(int success)
{
	if (success)
	{
		response_packet = ocl_ok_packet;
		response_len = ocl_ok_len;
	}
	else
	{
		response_packet = ocl_err_packet;
		response_len = ocl_err_len;
	}
}

int handle_connect_request(short conn_id, char *messageData)
{
	if (!send_connect_confirmation_packet(0)) return 0;
	return 1;
}

int handle_disconnect_request(short conn_id)
{
	return 1;
}

char *parse_message_packet()
{
	char *message_data = 0;
	int client_name_length;
	char *context;
	char *t = strtok_s(client_packet + 3, "!", &context);
	if (t) sscanf_s(t, "%d", &client_name_length);
	int recipient_name_length;
	t = strtok_s(0, "!", &context);
	if (t) sscanf_s(t + client_name_length, "%d", &recipient_name_length);
	t = strtok_s(0, "!", &context);
	if (t)
	{
		int rd = (recipient_name_length > MAX_RECIPIENT_LEN) ? MAX_RECIPIENT_LEN : recipient_name_length;
		message_data = t + recipient_name_length;
	}
	return message_data;
}

static int response_data_size;

char *pack_message_for_imagine(char *buf)
{
	char *pk = (char *)malloc(response_data_size + 20);
	if (!pk) return (char *)handle_packet_problem("Could not allocate input packet\n");

	//if (debug) printf("ocl -> %d bytes\n", toRead);

	// send the data read from port to the client
	sprintf_s(pk, response_data_size + 20, response_data_packet2, 8 + response_data_size);
	int el1 = strlen(pk);
	memcpy(pk + el1, buf, response_data_size);
	response_data_size += el1;
	return pk;
}

int pack_and_send_to_Imagine(char *buf)
{
	char *response_packet = pack_message_for_imagine(buf);
	
	if (!response_packet) return 0;

	int success = sendPacket(clientSocket, response_packet, response_data_size);
	free(response_packet);

	if (!success) return handle_packet_problem("Could not send back response packet\n");
	return 1;
}

int handle_regular_message()
{
	int success = 0;
	char *message_data = parse_message_packet();
	if (!message_data) return handle_packet_problem("incorrectly formatted W packet, closing connection\n");
	message_data++;

	success = input(message_data);
	return success;
}

const char LIST_OF_USERS = 'u';
const char LIST_MESSAGE = 'L';
const char GET_INFO_F = 'F';
const char GET_INFO_G = 'G';
const char REGULAR_MESSAGE = 'W';

int dispatch_client_packet()
{
	char client_packet_type = load_client_packet_type();
	int success = 0;
	if (debug) printf("msg type:%c\n", client_packet_type);

	switch (client_packet_type)
	{
	case LIST_OF_USERS: success = handle_list_of_users(); break;
	case LIST_MESSAGE: success = handle_list_message(); break;
	case GET_INFO_F:
	case GET_INFO_G: success = 1; break;
	case REGULAR_MESSAGE: success = handle_regular_message(); break;
	default: if (debug) printf("unexpected packet '%c' received and ignored.\n", client_packet_type);		
	}

	return success;
}

int handle_client_communication()
{
	while (1)
	{
		client_packet = receivePacket(clientSocket);
		if (!client_packet) return handle_packet_problem("connection with the client lost.\n");
		if (!correct_client_packet()) return handle_packet_problem("incorrect packet from client received, closing connection\n");
		dispatch_client_packet();
		if (client_packet) free(client_packet);
	}
}

int handle_client(SOCKET clSocket)
{
	clientSocket = clSocket;
	if (!receive_login_packet()) return 0;
	if (!send_login_confirmation_packet(1)) return 0;
	handle_client_communication();
	return 1;
}

