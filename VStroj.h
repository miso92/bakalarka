#include <iostream>
#include <CL/cl.hpp>

#include <string>
#include <fstream>
#include <streambuf>
class VStroj {
private:
	//result
	cl::Buffer resultBuffer;
	int result;
	void *res;

	//platformy
	std::vector<cl::Platform> all_platforms;
	cl::Platform platform;

	//zariadenia
	std::vector<cl::Device> all_devices;
	cl::Device device;

	//context
	cl::Context context;
	 
	//cl zdrojaky
	cl::Program::Sources sources;

	//program
	cl::Program program;

	//buffre
	std::vector<cl::Buffer> buffers;
	std::vector<size_t> buffersLen;

	//prikazovy rad
	cl::CommandQueue queue;

	//kernel
	cl::Kernel kernel;

	//build LOG
	std::vector<std::string> logs;


public:
	//ctor
	VStroj();

	//get adresy
	cl::Buffer* getResultBuffer();

	//platformy
	void loadAvailablePlatforms();
	bool setDefaultPlatform();
	bool setSpecificPlatform(const int id);
	std::string getAvailablePlatforms();
	cl::Platform getCurrentPlatform();
	std::string getCurrentPlatformName();
	int getNumberOfAvailablePlatforms();

	//zariadenia - zavisle na vybranej platforme
	bool loadAvailableDevices();
	bool setDefaultDevice();
	bool setSpecificDevice(const int id);
	std::string getAvailableDevices();
	cl::Device getCurrentDevice();
	std::string getCurrentDeviceName();
	void clearDevices();
	int getNumberOfAvailableDevices();
	cl_uint getMaxComputeUnits();

	//context
	bool setContextCurrentDevice();
	bool setContextAllDevices();
	cl_uint getContextReferendeCount();

	//kernel sources
	void addStringCode(char* kernel_code, int len);
	void addFileCode(std::string filename);
	const char* getLastSource();

	//program
	void connectSources();
	bool buildProgram();

	//get log
	std::string getLastLog();

	//buffre
	void addIntBuffer(unsigned int len);
	void createResultBuffer(unsigned int len);
	void loadResult();
	int getBuffersSize();
	void clearBuffers();
	size_t getBufferLen(int id);

	//command queue
	bool createQueue();
	bool copyDataToBuffer(int idBuffer, int value);
	bool copyDataToBuffer(int idBuffer, std::vector<int> arr, unsigned int len);
	bool copyDataToBuffer(int idBuffer, int arr[], unsigned int len);
	void queueFinish();
	bool readValueFromBuffer(int idBuffer, unsigned int len, void* result);

	//kernel
	void createKernel(char* clFunctionName);
	bool setArg(int idBuffer, int idArg);
	bool setArg(cl::Buffer &buffer, int idArg);

	//NDRange
	void setWorkgroupRange(size_t size);
	void setWorkgroupRange(size_t size1, size_t size2);
	void setWorkgroupRange(size_t size1, size_t size2, size_t size3);

	//clear
	void clearMemory();

protected:
};


//TODO
// pozret plenenie contextu viacerymi zariadeniami ... std::vector do normalneho pola ... .data nejde (pre viac zariadeni, PC, a pod)