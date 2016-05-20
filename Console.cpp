#include "Console.h"
#include <iostream>

void frameworkSettings() {
	std::cout << " --------------- Settings --------------- " << std::endl;

	int input = 0;
	std::cout << "Select Platform: (Write number and submit, default is Platform 0)" << std::endl;
	std::cout << stroj.getAvailablePlatforms() << std::endl;
	input = std::cin.get() - '0';
	if (input >= stroj.getNumberOfAvailablePlatforms() || input < 0) stroj.setDefaultPlatform();
	else stroj.setSpecificPlatform(input);
	std::cout << "Selected platform: " << stroj.getCurrentPlatformName() << std::endl;
	std::cin.get();

	input = 0;
	std::cout << "Select Device for" << stroj.getCurrentPlatformName() << ": (Write number and submit, default is Device 0)" << std::endl;
	std::cout << stroj.getAvailableDevices() << std::endl;
	input = std::cin.get() - '0';
	if (input >= stroj.getNumberOfAvailableDevices() || input < 0) stroj.setDefaultDevice();
	else stroj.setSpecificDevice(input);
	std::cout << "Selected device: " << stroj.getCurrentDeviceName() << std::endl;
	std::cout << "Max compute units for this device: " << stroj.getMaxComputeUnits() << std::endl;
	std::cout << " ---------- Settings completed ---------- " << std::endl;

	stroj.setContextCurrentDevice();
	stroj.createQueue();
}