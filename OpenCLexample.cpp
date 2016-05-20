#include "OpenCLexample.h"

#define DEBUG = true;

void paraSum() {
	
	// zistenie pouzivanej platformy ... CL_PLATFORM_NAME = NVIDIA CUDA
		std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Platform default_platform = all_platforms[1];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	//.....................................................................................

	// zistenie default hodnoty zariadenia ...CL_DEVICE_NAME = GeForce GTX 860M
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	std::cout << "Found " << all_devices.size() << " different devices." << std::endl;
	for (int i = 0; i < all_devices.size(); i++) {
		std::cout << i << ": " << all_devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
	}
	if (all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Device default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
	//.....................................................................................

	// Now we need to create a Context. Imagine the Context as the runtime link to the our device and platform
	cl::Context context({ default_device });

	// Next we need to create the program which we want to execute on our device
	cl::Program::Sources sources;

	// kernel calculates for each element C=A+B

	//nacitanie kernelov openCL do stringu ... FILE to STRING
	std::ifstream t("funkcie.cl");
	std::string kernel_code((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	// This code simply calculates C=A+B. As we want that one thread calculates sum of only one element, we use get_global_id(0). get_global_id(0) 
	// means get id of current thread. Id's can go from 0 to get_global_size(0) - 1. get_global_size(0) means number of threads. What is 0? 0 means first dimension. 
	//OpenCL supports running kernels on 1D, 2D and 3D problems. We will use 1D array! This means 1D problem.  


	// Next we need our kernel sources to build. We also check for the errors at building

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl::Program program(context, sources);
	if (program.build({ default_device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		//exit(1);
		std::cin.get();
	}

	int len = 100000000;
	// create buffers on the device
	cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * len);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * len);
	cl::Buffer buffer_R(context, CL_MEM_READ_WRITE, sizeof(int));
	cl::Buffer buffer_L(context, CL_MEM_READ_WRITE, sizeof(int));

	std::vector<int> A(len,1);
	
	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, default_device);

	// Now we can copy data from arrays A and B to buffer_A and buffer_B which represent memory on the device
	//write arrays A and B to the device
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * len, A.data());
	queue.enqueueWriteBuffer(buffer_L, CL_TRUE, 0, sizeof(int), &len);

	// run the Kernel
	cl::Kernel kernel_add = cl::Kernel(program, "par_sum");
	kernel_add.setArg(0, buffer_A);
	kernel_add.setArg(1, buffer_B);
	kernel_add.setArg(2, buffer_L);
	kernel_add.setArg(3, buffer_R);

	
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(500), cl::NullRange);
	
	DWORD start = GetTickCount();
	queue.finish();
	DWORD koniec = GetTickCount();

	// At the end we want to print memory B on our device.At first we need to transfer data from the device to our program(host)

	int R;
	//read result B from the device to B

	queue.enqueueReadBuffer(buffer_R, CL_TRUE, 0, sizeof(int), &R);

	std::cout << "cas paralelne  " << koniec - start << std::endl;
	std::cout << " result: " << R << std::endl;

	int vysl = 0;
	start = GetTickCount();
	for (int i = 0; i < len; i++) {
		vysl += A.at(i);
	}
	koniec = GetTickCount();

	std::cout << "cas CPU  " << koniec - start << std::endl;
	std::cout << " result: " << vysl << std::endl;
	
}

void paralellArraySumExample() {
	// zistenie pouzivanej platformy ... CL_PLATFORM_NAME = NVIDIA CUDA
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Platform default_platform = all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	//.....................................................................................

	// zistenie default hodnoty zariadenia ...CL_DEVICE_NAME = GeForce GTX 860M
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Device default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
	//.....................................................................................

	// Now we need to create a Context. Imagine the Context as the runtime link to the our device and platform
	cl::Context context({ default_device });

	// Next we need to create the program which we want to execute on our device
	cl::Program::Sources sources;

	// kernel calculates for each element C=A+B

	//nacitanie kernelov openCL do stringu ... FILE to STRING
	std::ifstream t("funkcie.cl");
	std::string kernel_code((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	// This code simply calculates C=A+B. As we want that one thread calculates sum of only one element, we use get_global_id(0). get_global_id(0) 
	// means get id of current thread. Id's can go from 0 to get_global_size(0) - 1. get_global_size(0) means number of threads. What is 0? 0 means first dimension. 
	//OpenCL supports running kernels on 1D, 2D and 3D problems. We will use 1D array! This means 1D problem.  


	// Next we need our kernel sources to build. We also check for the errors at building

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl::Program program(context, sources);
	if (program.build({ default_device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		//exit(1);
	}

	// For arrays A, B, C we need to allocate the space on the device

	// create buffers on the device
	cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * 10);

	// Arrays will have 10 element. We want to calculate sum of next arrays (A, B).
	int A[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int B[] = { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };

	// We need to copy arrays from A and B to the device. This means that we will copy arrays from the host to the device. 
	// Host represents our main. At first we need to create a queue which is the queue to the commands we will send to the our device

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, default_device);

	// Now we can copy data from arrays A and B to buffer_A and buffer_B which represent memory on the device
	//write arrays A and B to the device
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);

	// Now we can run the kernel which in parallel sums A and B and writes to C. We do this with KernelFunctor which runs the kernel on the device. 
	// Take a look at the "simple_add" this is the name of our kernel we wrote before. You can see the number 10. 
	// This corresponds to number of threads we want to run (our array size is 10)

	// run the Kernel
	cl::Kernel kernel_add = cl::Kernel(program, "simple_add");
	kernel_add.setArg(0, buffer_A);
	kernel_add.setArg(1, buffer_B);
	kernel_add.setArg(2, buffer_C);
	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(10), cl::NullRange);
	queue.finish();

	// At the end we want to print memory C on our device.At first we need to transfer data from the device to our program(host)

	int C[10];
	//read result C from the device to array C
	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 10, C);

	std::cout << " result: \n";
	for (int i = 0; i<10; i++) {
		std::cout << C[i] << " ";
	}
	std::cout << "\n";
}





