#include "VStroj.h"

VStroj::VStroj() {
	this->platform = nullptr;
	this->device = nullptr;
	this->context = nullptr;
	//ctor
}

cl::Buffer* VStroj::getResultBuffer() {
	return &this->resultBuffer;
}

void VStroj::loadAvailablePlatforms(){
	cl::Platform::get(&this->all_platforms);
}

bool VStroj::setDefaultPlatform() {
	this->clearDevices();
	if (this->all_platforms.size() == 0) {
		this->logs.push_back("ERROR: No available platform. Try load available platforms !");
		return false;
	}
	this->platform = this->all_platforms[0];
	return true;
}

bool VStroj::setSpecificPlatform(const int id) {
	this->clearDevices();
	if (this->all_platforms.size() == 0 || id < 0 || id >= this->all_platforms.size()) {
		this->logs.push_back("ERROR: No available platform. Try load available platforms !");
		return false;
	}
	this->platform = this->all_platforms[id];
	return true;
}

std::string VStroj::getAvailablePlatforms() {
	std::string result = "";
	if (this->all_platforms.size() == 0) {
		cl::Platform::get(&this->all_platforms);
	}
	for (int i = 0; i < this->all_platforms.size(); i++) {
		result += std::to_string(i) + " : " + this->all_platforms[i].getInfo<CL_PLATFORM_NAME>() + "\n";
	}
	return result;
}

cl::Platform VStroj::getCurrentPlatform() {
	return this->platform;
}

std::string VStroj::getCurrentPlatformName() {
	return this->platform.getInfo<CL_PLATFORM_NAME>();
}

int VStroj::getNumberOfAvailablePlatforms() {
	return this->all_platforms.size();
}

bool VStroj::loadAvailableDevices() {
	if (this->platform.getInfo<CL_PLATFORM_NAME>().size() == 0) {
		return false;
	}
	this->platform.getDevices(CL_DEVICE_TYPE_ALL, &this->all_devices);
	return true;
}

bool VStroj::setDefaultDevice() {
	if (this->all_devices.size() == 0) {
		this->logs.push_back("ERROR: No available devices. Try load available devices or see if platform is supported in OpenCL !");
		return false;
	}
	this->device = this->all_devices[0];
	return true;
	
}

bool VStroj::setSpecificDevice(const int id) {
	if (this->all_devices.size() == 0 || id < 0 || id >= this->all_devices.size()) {
		this->logs.push_back("ERROR: No available devices. Try load available devices or see if platform is supported in OpenCL !");
		return false;
	}
	this->device = this->all_devices[id];
	return true;
}

std::string VStroj::getAvailableDevices() {
	std::string result = "";
	if (this->all_devices.size() == 0) {
		if (this->platform.getInfo<CL_PLATFORM_NAME>().size() == 0) {
			return result;
		}
		this->platform.getDevices(CL_DEVICE_TYPE_ALL, &this->all_devices);
	}
	for (int i = 0; i < this->all_devices.size(); i++) {
		result += std::to_string(i) + " : " + this->all_devices[i].getInfo<CL_DEVICE_NAME>() + "\n";
	}
	return result;
}

cl::Device VStroj::getCurrentDevice() {
	return this->device;
}

std::string VStroj::getCurrentDeviceName()
{
	return this->device.getInfo<CL_DEVICE_NAME>();
}

void VStroj::clearDevices() {
	this->device = nullptr;
	this->all_devices.clear();
}

int VStroj::getNumberOfAvailableDevices() {
	return this->all_devices.size();
}

cl_uint VStroj::getMaxComputeUnits() {
	//return this->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
	return this->device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
}

bool VStroj::setContextCurrentDevice() {
	if (this->device.getInfo<CL_DEVICE_NAME>() == "") {
		this->logs.push_back("ERROR: No device set as current. Try set device first !");
		return false;
	}
	this->context = cl::Context({ this->device });
	return true;
}

bool VStroj::setContextAllDevices() {
	if (this->all_devices.size() == 0) {
		this->logs.push_back("ERROR: No devices founded in current platform. Try load devices or see if current platform support OpenCL or some device !");
		return false;
	}
	this->context = cl::Context(*this->all_devices.data());
	return true;
}

cl_uint VStroj::getContextReferendeCount() {
	return this->context.getInfo<CL_CONTEXT_REFERENCE_COUNT>();
}

void VStroj::addStringCode(char* kernel_code, int len) {
	this->sources.clear();
	this->sources.push_back({ kernel_code, len });
}

void VStroj::addFileCode(std::string filename) {
	std::ifstream file(filename);
	std::string kernel_code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	//this->addStringCode(kernel_code.c_str(), kernel_code.length());
}

const char* VStroj::getLastSource() {
	if (this->sources.size() > 0) return this->sources.back().first;
	return "EMPTY STACK";
}

void VStroj::connectSources() {
	this->program = cl::Program(this->context, this->sources);
}

bool VStroj::buildProgram() {
	if (this->program.build({ this->device }) != CL_SUCCESS) { //TODO nejak z kontextu vytiahnut devices
		//error log
		this->logs.push_back(this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device));
		return false;
	}
	return true;
}

std::string VStroj::getLastLog(){
	if (this->logs.size() > 0) return this->logs.back();
	return "EMPTY STACK";
}

void VStroj::addIntBuffer(unsigned int len) {
	this->buffers.push_back(cl::Buffer(this->context, CL_MEM_READ_WRITE, sizeof(int) * len));
	this->buffersLen.push_back(len);
}

void VStroj::createResultBuffer(unsigned int len) {
	this->resultBuffer = cl::Buffer(this->context, CL_MEM_READ_WRITE, sizeof(int) * len);
}

void VStroj::loadResult() {
	this->queue.enqueueReadBuffer(this->resultBuffer, CL_TRUE, 0, sizeof(int), &this->result);
}

int VStroj::getBuffersSize() {
	return this->buffers.size();
}

void VStroj::clearBuffers() {
	this->buffers.clear();
}

size_t VStroj::getBufferLen(int id) {
	return this->buffersLen[id];
}

bool VStroj::createQueue() {
	if (this->device.getInfo<CL_DEVICE_NAME>() == "") {
		this->logs.push_back("ERROR: No device set as current. Try set device first !");
		return false;
	}
	this->queue = cl::CommandQueue(this->context, this->device);
	return true;
}

bool VStroj::copyDataToBuffer(int idBuffer, int value) {
	if (idBuffer < 0 || idBuffer >= this->buffers.size()) return false;
	this->queue.enqueueWriteBuffer(this->buffers[idBuffer], CL_TRUE, 0, sizeof(int), &value);
	return true;
}

bool VStroj::copyDataToBuffer(int idBuffer, std::vector<int> arr, unsigned int len) {
	if (idBuffer < 0 || idBuffer >= this->buffers.size()) return false;
	this->queue.enqueueWriteBuffer(this->buffers[idBuffer], CL_TRUE, 0, sizeof(int) * len, arr.data());
	return true;
}

bool VStroj::copyDataToBuffer(int idBuffer, int arr[], unsigned int len) {
	if (idBuffer < 0 || idBuffer >= this->buffers.size()) return false;
	this->queue.enqueueWriteBuffer(this->buffers[idBuffer], CL_TRUE, 0, sizeof(int) * len, arr);
	return true;
}

void VStroj::queueFinish() {
	this->queue.finish();
}

bool VStroj::readValueFromBuffer(int idBuffer, unsigned int len, void* result) {
	if (idBuffer < 0 || idBuffer >= this->buffers.size()) return false;
	this->queue.enqueueReadBuffer(this->buffers[idBuffer], CL_TRUE, 0, sizeof(int)*len, result);
	
	return true;
}

void VStroj::createKernel(char* clFunctionName) {
	this->kernel = cl::Kernel(this->program, clFunctionName);
}

bool VStroj::setArg(int idBuffer, int idArg) {
	if (idBuffer < 0 || idBuffer >= this->buffers.size()) return false;
	this->kernel.setArg(idArg, this->buffers[idBuffer]);
	return true;
}

bool VStroj::setArg(cl::Buffer &buffer, int idArg) {
	this->kernel.setArg(idArg, &buffer);
	return true;
}

void VStroj::setWorkgroupRange(size_t size) {
	this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, cl::NDRange(size), cl::NullRange);
}

void VStroj::setWorkgroupRange(size_t size1, size_t size2) {
	this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, cl::NDRange(size1, size2), cl::NullRange);
}

void VStroj::setWorkgroupRange(size_t size1, size_t size2, size_t size3) {
	this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, cl::NDRange(size1, size2, size3), cl::NullRange);
}

void VStroj::clearMemory() {
	this->buffers.clear();
	this->buffersLen.clear();
	this->sources.clear();
}

