#include "Controller.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>


void startCL() {
	//stroj = VStroj();
}

char* getPrefix(char *msg) {
	char *chpos;
	int index;
	char * prefix;
	chpos = strchr(msg, '#');
	index = (int)(chpos - msg);

	prefix = new char[index + 1];

	memcpy(prefix, msg, index);
	prefix[index] = '\0';

	return prefix;
	
}

char *getBody(char *msg) {
	char *chpos;
	int index;
	char * body;
	chpos = strchr(msg, '#');
	index = (int)(chpos - msg);
	int end_msg = strlen(msg);

	body = new char[end_msg -1];

	memcpy(body, &msg[index + 1], end_msg - index-1);
	body[end_msg - index - 1] = '\0';

	return body;
}

char *getBodyString(char *msg) {
	char *chpos;
	int index;
	char * data;
	chpos = strchr(msg, '[');
	index = (int)(chpos - msg);
	int end_msg = strlen(msg);

	data = new char[end_msg - 2];
	memcpy(data, &msg[index + 1], end_msg - index-2);
	data[end_msg - index - 2] = '\0';
	return data;
}

int getIndexOfChar(char* msg, char w, int id) {
	if (debug) std::cout << "getIndexOfChar - msg: " << msg << std::endl;
	int len = strlen(msg);
	int lid = 0;
	for (int i = 0; i < len; i++) {
		if (msg[i] == w && id == lid) {
			if (debug) std::cout << "getIndexOfChar - msg[" << i << "]: " << msg[i] << std::endl;
			return i;
		}
		if (msg[i] == w && id != lid) lid++;
	}

	return -1;
}

char *getBodyString(char *msg, int id) {
	int index1 = getIndexOfChar(msg, '[', id);
	int index2 = getIndexOfChar(msg, ']', id);
	char * data;
	data = new char[index2 - index1];
	memcpy(data, &msg[index1 + 1], index2-index1 -1);
	data[index2 - index1 - 1] = '\0';
	return data;
}

int getVariableData(char *body) {
	char *line = getBodyString(body);
	int data;
	
	std::stringstream lineStream(line);
	lineStream >> data;
	
	delete[] line;
	return data;
}

void allocVariable(char *body) {
	if (debug) std::cout << "PRISLA PREMENNA: " << body << std::endl;
	int data = getVariableData(body);
	
	stroj.addIntBuffer(1);
	int idBuffer = stroj.getBuffersSize()-1;
	if (debug) std::cout << "HODNOTA: " << data << std::endl;
	stroj.copyDataToBuffer(idBuffer, data);
	if (debug) std::cout << "BUFFER" << idBuffer << std::endl;
}

int getCountofChars(char *str, char ch) {
	int count = 0;
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == ch) count++;
	}
	return count;
}


std::pair<int, int*> getListData(char *body) {
	char *line = getBodyString(body);
	int *data;
	int num;
	int space_count = getCountofChars(line, ' ');
	if (debug) std::cout << "Pocet medzier: " << space_count << std::endl;
	data = new int[space_count + 1];
	std::stringstream lineStream(line);
	
	int index = 0;
	while (lineStream >> num) {
		data[index] = num;
		index++;
	}

	lineStream >> num;
	

	delete [] line;
	return std::make_pair(index, data);
}

void allocList(char *body) {
	if (debug) std::cout << "PRISLO POLE: " << body << std::endl;
	std::pair<int, int*> output = getListData(body);
	if (debug) {
		std::cout << "Prvky pola: ";
		for (int i = 0; i < output.first; i++) {
			std::cout << output.second[i];
		}
		std::cout << "" << std::endl;
	}
	
	stroj.addIntBuffer(output.first);
	int idBuffer = stroj.getBuffersSize()-1;
	stroj.copyDataToBuffer(idBuffer, output.second, output.first);

}

void parseProcedure(char *body) {
	if (debug) std::cout << "PRISLA PROCEDURA: " << body << std::endl;
}

void runKernel() {
	stroj.queueFinish();
}

void returnResult() {
	if (debug) std::cout << "RESULT REQUEST" << std::endl;
	pack_and_send_to_Imagine("RETURN");
}

void buildProgram() {
	stroj.connectSources();
	if (!stroj.buildProgram()) {
		if (debug) std::cout << "BUILD FAILED" << std::endl;
	// TODO nejaky return chyby z logu
	}
	
}

void clearMemory() {
	stroj.clearMemory();	
}

void returnVariableResult() {
	if (debug) {
		std::cout << "BUFFERS SIZES: " << std::endl;
		for (int i = 0; i < stroj.getBuffersSize(); i++) {
			std::cout << i << ": SIZE = " << stroj.getBufferLen(i) << std::endl;
		}
	}
	int result;
	if (debug) std::cout << "Selected device: " << stroj.getCurrentDeviceName() << std::endl;
	int index = stroj.getBuffersSize() - 1;
	stroj.readValueFromBuffer(index, 1, &result);
	if (debug || !debug) std::cout << "RESULT: " << result << std::endl;
	//return do Imaginu
}

void returnListResult() {
	if (debug) {
		std::cout << "BUFFERS SIZES: " << std::endl;
		for (int i = 0; i < stroj.getBuffersSize(); i++) {
			std::cout << i << ": SIZE = " << stroj.getBufferLen(i) << std::endl;
		}
	}

	int len = stroj.getBufferLen(stroj.getBuffersSize() - 1);
	int *C = new int[len];
	

	if (debug) std::cout << "Selected device: " << stroj.getCurrentDeviceName() << std::endl;
	int index = stroj.getBuffersSize() - 1;
	stroj.readValueFromBuffer(index, len, C);
	if (debug || !debug) {
	std::cout << "PRVKY RESULTU: ";
	for (int i = 0; i < len; i++) {
	std::cout << C[i] << " ";
	}
	std::cout << "" << std::endl;
	}
	//return do Imaginu

	delete[]C;
}

void makeInstruction(char *body) {
	if (debug) std::cout << "PRISLA INSTRUKCIA: " << body << std::endl;
	char *data = getBodyString(body);
	if (debug) std::cout << "DATA: " << data << std::endl;
	
	//instruction compare here
	if (strcmp(data, "RUN") == 0) runKernel();
	else if(strcmp(data, "BUILD") == 0) buildProgram();
	else if (strcmp(data, "GETRESULT") == 0) returnResult();
	else if (strcmp(data, "GETVRESULT") == 0) returnVariableResult();
	else if (strcmp(data, "GETLRESULT") == 0) returnListResult();
	else if (strcmp(data, "CLEAR") == 0) clearMemory();

	delete[] data;
}

void addKernelCode(char * body) {
	if (debug) std::cout << "PRISIEL KERNEL KOD: " << body << std::endl;
	char *data = getBodyString(body);
	if (debug) std::cout << "DATA: " << data << std::endl;
	stroj.addStringCode(data, strlen(data));

	delete[] data;
}

void setKernel(char* name) {
	stroj.createKernel(name);
	for (int i = 0; i < stroj.getBuffersSize(); i++) {
		stroj.setArg(i, i);
	}
}

void setNDRange(char * data) {
	if (debug) std::cout << "SETTING NDRANGE: " << data << std::endl;
	

	int *values;
	int space_count = getCountofChars(data, ' ');
	if (debug) std::cout << "Pocet medzier: " << space_count << std::endl;
	values = new int[space_count + 1];

	int index = 0;
	int num;

	std::stringstream lineStream(data);

	while (lineStream >> num) {
		values[index] = num;
		index++;
	}

	lineStream >> num;

	if (debug) {
		std::cout << "SIZE OF ARRAY: " << index << std::endl;
		std::cout << "DATA: ";
		for (int i = 0; i < index; i++) {
			std::cout << values[i];
		}
		std::cout << "" << std::endl;
	}
	switch (index) {
		case 1: stroj.setWorkgroupRange(values[0]); if(debug) std::cout << "SETTING 1D WORKGOUP: " << std::endl; break;
		case 2: stroj.setWorkgroupRange(values[0], values[1]); if (debug) std::cout << "SETTING 2D WORKGOUP: " << std::endl; break;
		case 3: stroj.setWorkgroupRange(values[0], values[1], values[2]); if (debug) std::cout << "SETTING 3D WORKGOUP: " << std::endl; break;
		default: break; //TODO Nejaky error
	}
	delete[] values;
}

void setParameter(char * body) {
	if (debug) std::cout << "PRISIEL PARAMETER: " << body << std::endl;
	char * line = getBodyString(body);
	char * type = getBodyString(line, 0);
	char * value = getBodyString(line, 1);
	if (debug)  std::cout << "STRING: " << line << std::endl;
	if (debug)  std::cout << "TYPE: " << type << std::endl;
	if (debug)  std::cout << "VALUE: " << value << std::endl;

	if (strcmp(type, "KERNEL_NAME") == 0) setKernel(value);
	else if (strcmp(type, "ND_RANGE") == 0) setNDRange(value);

	delete[] value;
	delete[] type;
	delete[] line;

}

void messageSwitch(char *msg) {
	char * prefix = getPrefix(msg);
	char * body = getBody(msg);

	if (debug) std::cout << "BODY MESSAGE: " << body << std::endl;

	switch (*prefix) {
	case 'V': allocVariable(body); break;
	case 'L': allocList(body); break;
	case 'P': parseProcedure(body); break;
	case 'Q': makeInstruction(body); break;
	case 'C': addKernelCode(body); break;
	case 'S': setParameter(body); break;
	default: break;	
	}
	if (debug) std::cout << "PREFIX: " << prefix << std::endl;
	delete[] prefix;
	delete[] body;
	
}

int input(char *msg) {
	if (debug) std::cout << "PRIJATA SPRAVA: " << msg << std::endl;
	if (msg[0] == 'V' || msg[0] == 'L' || msg[0] == 'P' || msg[0] == 'Q' || msg[0] == 'C' || msg[0] == 'S') messageSwitch(msg);
	
	//std::cout << "PAKET: " << msg << std::endl;
	//pack_and_send_to_Imagine("RETURN");
	return 1;
}