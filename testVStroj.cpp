#include "testVStroj.h"
#include <typeinfo>

void testBegin(std::string testName) {
	std::cout << "-------------TEST: " << testName << "-------------" << std::endl;
}
void testEnd() {
	std::cout << "---------------------------------------" << std::endl;
}

void testyPlatformy() {
	testBegin("Testy Platformy");
	VStroj stroj = VStroj();
	std::cout << stroj.getCurrentPlatform().getInfo<CL_PLATFORM_NAME>() << std::endl;
	
	if (stroj.getCurrentPlatformName() == "") std::cout << typeid(stroj.getCurrentPlatformName()).name() << std::endl;
	
	stroj.loadAvailablePlatforms();
	std::cout << stroj.getAvailablePlatforms() << std::endl;

	stroj.setDefaultPlatform();
	std::cout << stroj.getCurrentPlatformName() << std::endl;

	stroj.setSpecificPlatform(1);
	std::cout << stroj.getCurrentPlatformName() << std::endl;
	testEnd();
}

void testyZariadenia() {
	testBegin("Testy Zariadenia");
	VStroj stroj = VStroj();
	std::cout << stroj.getCurrentDevice().getInfo<CL_DEVICE_NAME>() << std::endl;

	if (stroj.getCurrentDeviceName() == "") std::cout << typeid(stroj.getCurrentDeviceName()).name() << std::endl;

	stroj.loadAvailablePlatforms();
	stroj.setDefaultPlatform();
	
	stroj.loadAvailableDevices();
	std::cout << stroj.getAvailableDevices() << std::endl;

	stroj.setSpecificPlatform(1);
	std::cout << stroj.getAvailableDevices() << std::endl;

	stroj.setDefaultDevice();
	std::cout << stroj.getCurrentDeviceName() << std::endl;

	stroj.setDefaultPlatform();
	stroj.loadAvailableDevices();
	stroj.setDefaultDevice();
	std::cout << stroj.getCurrentPlatformName() << std::endl;
	std::cout << stroj.getCurrentDeviceName() << std::endl;
	
	//std::cout << stroj.getCurrentDevice().getInfo<CL_DEVICE_PLATFORM>();

	testEnd();
}

void testyContextu() {
	testBegin("Testy Contextu");
	VStroj stroj = VStroj();

	stroj.loadAvailablePlatforms();
	stroj.setDefaultPlatform();

	stroj.loadAvailableDevices();

	std::cout << stroj.getAvailableDevices() << std::endl;

	stroj.setDefaultDevice();

	std::cout << stroj.getContextReferendeCount() << std::endl;
	stroj.setContextCurrentDevice();
	std::cout << stroj.getContextReferendeCount() << std::endl;
	stroj.setContextAllDevices();
	std::cout << stroj.getContextReferendeCount() << std::endl;
	testEnd();
}

void testyProgramu() {
	testBegin("Testy Programu");
	VStroj stroj = VStroj();

	stroj.loadAvailablePlatforms();
	stroj.setDefaultPlatform();

	stroj.loadAvailableDevices();

	stroj.setDefaultDevice();

	stroj.setContextCurrentDevice();
	char *source = "void kernel simple_add(global const int* A, global const int* B, global int* C){C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];}";
	int len = 0;
	len = strlen(source);
	stroj.addStringCode(source, len);
	stroj.connectSources();

	std::cout << stroj.buildProgram() << std::endl;
	std::cout << "**********************" << std::endl;
	std::cout << "void kernel simple_add(global const int* A, global const int* B, global int* C) { C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)]; }" << std::endl;
	std::cout << stroj.getLastSource() << std::endl;
	std::cout << "**********************" << std::endl;
	std::cout << stroj.getLastLog() << std::endl;
	testEnd();
}

void napln(VStroj &stroj) {
	int A[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int B[] = { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };
	stroj.addIntBuffer(10);
	stroj.addIntBuffer(10);
	stroj.addIntBuffer(10); //RESULT



	stroj.copyDataToBuffer(0, A, 10);
	stroj.copyDataToBuffer(1, B, 10);
}

void testVypoctu1() {
	testBegin("Testy Vypoctu1");
	
	int C[10];

	VStroj stroj = VStroj();
	std::cout << *C << std::endl;
	stroj.loadAvailablePlatforms();
	stroj.setSpecificPlatform(1);

	stroj.loadAvailableDevices();

	stroj.setSpecificDevice(0);

	stroj.setContextCurrentDevice();

	std::cout << stroj.getCurrentPlatformName() << std::endl;
	std::cout << stroj.getCurrentDeviceName() << std::endl;
	std::cout <<"POCET JEDNOTIEK:"<< stroj.getMaxComputeUnits() << std::endl;

	std::cout << stroj.createQueue() << std::endl;

	char *source = "void kernel simple_add(global const int* A, global const int* B, global int* C){C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];}";
	int len = 0;
	len = strlen(source);
	stroj.addStringCode(source, len);
	stroj.connectSources();
	

	napln(stroj);
	

	std::cout << stroj.buildProgram() << std::endl;
	
	//stroj.readValueFromBuffer(1, 10, &C);
	//std::cout << *C << std::endl;
	
	stroj.createKernel("simple_add");
	stroj.setArg(0, 0);
	stroj.setArg(1, 1);
	stroj.setArg(2, 2);


	stroj.setWorkgroupRange(10); //1D

	stroj.queueFinish();

	stroj.readValueFromBuffer(2, 10, &C);

	for (int i = 0; i < 10; i++)std::cout << C[i] << std::endl;

	testEnd();
}
