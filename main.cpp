#include "main.h"
#include "networking.h"


char debug = 1;   // should we print debugging output
VStroj stroj = VStroj();

void testy() {
	testyPlatformy();
	testyZariadenia();
	testyContextu();
	testyProgramu();
	testVypoctu1();
	std::cin.get();
}

int main(int argc, char **argv)
{
	testy();
	stroj = VStroj();
	frameworkSettings();
	printf("Daemon is runing ...\n");

	if (initialize_networking())
	{
		listen_to_clients();
	}
	else if (debug) printf("Could not initialize networking.\n");
	printf("OpenCL says bye.\n");
	return 0;
}
