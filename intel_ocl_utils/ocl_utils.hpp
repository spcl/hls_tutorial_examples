/**


  This file contains a set of basic utilities to interact with Intel FPGA OpenCL
  ecosystem.
  It has been derived from Intel FPGA coding examples.

  Author: Tiziano De Matteis
*/


#ifndef OCL_UTILS_HPP
#define OCL_UTILS_HPP
#include <string>
#include <algorithm>
#include <stdarg.h>
#include <unistd.h>

#include "CL/cl.hpp"

/**
 * @brief The IntelFPGAOCLUtils class contains a set of basic utilities for interacting
 * with the Intel FPGA ecosystem
 */

class IntelFPGAOCLUtils{
public:

    /**
     * @brief initOpenCL instantiate platform, device and context and build the program using the program path passed as argument
     *			NOTE: it will use the first Intel device that it found
     * @return true if everything went ok
     */
    static bool initOpenCL(cl::Platform &platform, cl::Device &device, cl::Context &context,  cl::Program &program, const std::string &program_path)
    {
	cl_int status;

	bool found=findPlatform("Intel(R) FPGA SDK for OpenCL(TM)",platform);
	if(!found) {
	    std::cerr<< "ERROR: Unable to find Intel(R) FPGA OpenCL platform" <<std::endl;
	    return false;
	}
	//get the first device of type accelerator
	std::vector<cl::Device> devices;
	status=platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR,&devices);
	checkError(status, __FILE__,__LINE__, "Query for device failed");
	device=devices[0];

	// Create the context
	context=cl::Context({device});
	//create the program
	createProgramFromBinary(context, program,program_path.c_str(),device);
	return true;

    }

    /**
     * @brief initEnvironment instantiate platform, device, context command queues and kernels for a given set of kernels
     */
    static bool initEnvironment(cl::Platform &platform, cl::Device &device, cl::Context &context,  cl::Program &program,
				const std::string &program_path, std::vector<std::string> &kernel_names, std::vector<cl::Kernel> &kernels,
				std::vector<cl::CommandQueue> &queues)
    {
	bool init_ocl=initOpenCL(platform,device,context,program,program_path);
	if(!init_ocl)
	    return false;
	//create kernels and queues
	int num_kernels=kernel_names.size();
	createKernels(program,kernel_names,kernels);
	createCommandQueues(context,device,queues,num_kernels);
        return true;
    }


    /**
     * @brief listAllDevices list all the devices attached to the host
     */
    static void listAllDevices()
    {

	cl_int status;
	int i, j;

	// get all platforms
	std::vector<cl::Platform> all_platforms;
	status = cl::Platform::get(&all_platforms);
	checkError(status, __FILE__,__LINE__, "Query for platform failed");

	for (i = 0; i < all_platforms.size(); i++) {

	    // get all devices
	    std::vector<cl::Device> devices;
	    status=all_platforms[i].getDevices(CL_DEVICE_TYPE_ALL,&devices);
	    checkError(status, __FILE__, __LINE__, "Query for device failed");

	    // for each device print critical attributes
	    for (j = 0; j < devices.size(); j++) {

		// print device name
		std::string str=devices[i].getInfo<CL_DEVICE_NAME>(&status);
		checkError(status,__FILE__,__LINE__, "Query Device failed!");
		std::cout << j+1<<"." << " Device: "<<str<<std::endl;
		std::cout << "\t" <<j+1<<"."<< "1. Hardware Version: "<<devices[i].getInfo<CL_DEVICE_VERSION>()<<std::endl;
		std::cout << "\t" <<j+1<<"."<< "2. Driver Version: "<<devices[i].getInfo<CL_DRIVER_VERSION>()<<std::endl;
		//std::cout << "\t" <<j+1<<"."<< "3. OpenCl Version: "<<devices[i].getInfo<CL_DEVICE_OPENCL_C_VERSION>()<<std::endl;
		std::cout << "\t" <<j+1<<"."<< "4. Parallel Compute Units: "<<devices[i].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>()<<std::endl;
		std::cout << "\t" <<j+1<<"."<< "5. Global Memory size (MB): "<<devices[i].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()/(1024*1024)<<std::endl;
		std::cout << "\t" <<j+1<<"."<< "6. Local Memory size (KB): "<<devices[i].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()/(1024)<<std::endl;
		//print memory info
	    }

	    devices.clear();

	}

	all_platforms.clear();

    }

    /**
     * @brief createCommandQueue create a command queue for a given context and device
     */
    static void createCommandQueue(cl::Context &context, cl::Device &device, cl::CommandQueue &queue)
    {
	cl_int status;
	queue = std::move(cl::CommandQueue(context,device,0,&status));
	checkError(status,__FILE__,__LINE__, "Failed to create queue");

    }

    /**
     * @brief createCommandQueues creates a number of command queus
     */
    static void createCommandQueues(cl::Context &context, cl::Device &device, std::vector<cl::CommandQueue> &queues, int num_queue)
    {
	cl_int status;
	for(int i=0;i<num_queue;i++)
	{
	    queues.push_back(std::move(cl::CommandQueue(context,device,0,&status)));
	    checkError(status,__FILE__,__LINE__, "Failed to create queue");
	}
    }


    /**
     * @brief createKernel create a single kernel, whose name is passed as argument
     */
    static void createKernel(cl::Program program, std::string kernel_name, cl::Kernel &kernel)
    {
	cl_int status;
	kernel = std::move(cl::Kernel(program,kernel_name.c_str(), &status));
	checkError(status, __FILE__,__LINE__,"Failed to create kernel");
    }



    /**
     * @brief createKernels given a program, creates the kernels with a given name
     */
    static void createKernels(cl::Program program, std::vector<std::string> &kernel_names, std::vector<cl::Kernel> &kernels)
    {
	cl_int status;
	for(std::string k:kernel_names)
	{
	    kernels.push_back(std::move(cl::Kernel(program,k.c_str(),&status)));
	    checkError(status, __FILE__,__LINE__, "Failed to create kernel");
	}
    }

    /**
     * @brief checkError check for errors and prints messages (if any)
     *  If an error occured, the program is terminated
     */
    static void checkError(cl_int error , const char *file,int line,const char *msg)
    {
	// If not successful
	if(error != CL_SUCCESS) {
	    // Print line and file
	    std::cerr << "ERROR: ";
	    printError(error);
	    std::cerr << "\nLocation: "<<file<<":"<<line << " :"<<msg<< std::endl;
	    exit(error);
	}
    }

    static const unsigned int AOCL_ALIGNMENT=64;


private:
    static void checkError(cl_int error ,int line, const char *file,const char *msg) ;

    /**
     * @brief createProgramFromBinary loads the program froma  binary file
     */
    static void createProgramFromBinary(cl::Context &context, cl::Program &program, const char *binary_file_name, cl::Device &device) {
	// Early exit for potentially the most common way to fail: AOCX does not exist.
	if(!fileExists(binary_file_name)) {
	    std::cerr<< "AOCX file '"<<binary_file_name<<"' does not exist"<<std::endl;
	    checkError(CL_INVALID_PROGRAM,__FILE__,__LINE__, "Failed to load binary file");
	}

	// Load the binary.
	size_t binary_size;
	unsigned char *binary=loadBinaryFile(binary_file_name, &binary_size);
	if(binary == NULL) {
	    checkError(CL_INVALID_PROGRAM,__FILE__,__LINE__, "Failed to load binary file");
	}

	std::vector<cl::Device> dev;
	dev.push_back(device);
	//create the vector with the binaries to pass to the constructor of cl::Program
	std::vector<std::pair<const void*, size_t>> binaries;
	binaries.push_back(std::make_pair(binary,binary_size));

        program=cl::Program(context,{device},binaries);

        //checkError(status, __FILE__,__LINE__, "Failed to create program with binary");
    }

    static bool fileExists(const char *file_name) {
	return access(file_name, R_OK) != -1;
    }

    // Loads a file in binary form.
    static unsigned char *loadBinaryFile(const char *file_name, size_t *size) {
	// Open the File
	FILE* fp;
	fp = fopen(file_name, "rb");
	if(fp == 0) {
	    return NULL;
	}

	// Get the size of the file
	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);

	// Allocate space for the binary
	unsigned char *binary = new unsigned char[*size];

	// Go back to the file start
	rewind(fp);

	// Read the file into the binary
	if(fread((void*)binary, *size, 1, fp) == 0) {
	    delete[] binary;
	    fclose(fp);
	    return NULL;
	}
	fclose(fp);

	return binary;
    }

    /**
     * @brief findPlatformSearches all platforms for the first platform whose name
     * @return On success the platform reference will contains  the desired platform
     */
    static bool findPlatform(std::string search, cl::Platform &platform) {
	cl_int status;

	//lower case
	std::transform(search.begin(), search.end(), search.begin(), tolower);

	//get all the platforms
	std::vector<cl::Platform> all_platforms;
	status=(cl::Platform::get(&all_platforms));
	checkError(status, __FILE__,__LINE__, "Failed to get platform");


	// For each platform, get name and compare against the search string.
	bool found=false;
	int found_id=0;
	for(unsigned i = 0; i < all_platforms.size(); ++i) {
	    std::string name = all_platforms[i].getInfo<CL_PLATFORM_NAME>();

	    // Convert to lower case.
	    std::transform(name.begin(), name.end(), name.begin(), tolower);

	    if(name.find(search) != std::string::npos) {
		// Found!
		found_id=i;
		found=true;
		break;
	    }
	}

	if(found)
	{
	    platform=all_platforms[found_id];
	    return true;
	}
	else
	    return false;
    }





    /**
     * @brief printError prints error messages
     * @param error
     */
    static void printError(cl_int error) {
	// Print error message
	switch(error)
	{
	    case -1:
		std::cerr << "CL_DEVICE_NOT_FOUND ";
		break;
	    case -2:
		std::cerr << "CL_DEVICE_NOT_AVAILABLE ";
		break;
	    case -3:
		std::cerr << "CL_COMPILER_NOT_AVAILABLE ";
		break;
	    case -4:
		std::cerr << "CL_MEM_OBJECT_ALLOCATION_FAILURE ";
		break;
	    case -5:
		std::cerr << "CL_OUT_OF_RESOURCES ";
		break;
	    case -6:
		std::cerr << "CL_OUT_OF_HOST_MEMORY ";
		break;
	    case -7:
		std::cerr << "CL_PROFILING_INFO_NOT_AVAILABLE ";
		break;
	    case -8:
		std::cerr << "CL_MEM_COPY_OVERLAP ";
		break;
	    case -9:
		std::cerr << "CL_IMAGE_FORMAT_MISMATCH ";
		break;
	    case -10:
		std::cerr << "CL_IMAGE_FORMAT_NOT_SUPPORTED ";
		break;
	    case -11:
		std::cerr << "CL_BUILD_PROGRAM_FAILURE ";
		break;
	    case -12:
		std::cerr << "CL_MAP_FAILURE ";
		break;
	    case -13:
		std::cerr << "CL_MISALIGNED_SUB_BUFFER_OFFSET ";
		break;
	    case -14:
		std::cerr << "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST ";
		break;

	    case -30:
		std::cerr << "CL_INVALID_VALUE ";
		break;
	    case -31:
		std::cerr << "CL_INVALID_DEVICE_TYPE ";
		break;
	    case -32:
		std::cerr << "CL_INVALID_PLATFORM ";
		break;
	    case -33:
		std::cerr << "CL_INVALID_DEVICE ";
		break;
	    case -34:
		std::cerr << "CL_INVALID_CONTEXT ";
		break;
	    case -35:
		std::cerr << "CL_INVALID_QUEUE_PROPERTIES ";
		break;
	    case -36:
		std::cerr << "CL_INVALID_COMMAND_QUEUE ";
		break;
	    case -37:
		std::cerr << "CL_INVALID_HOST_PTR ";
		break;
	    case -38:
		std::cerr << "CL_INVALID_MEM_OBJECT ";
		break;
	    case -39:
		std::cerr << "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR ";
		break;
	    case -40:
		std::cerr << "CL_INVALID_IMAGE_SIZE ";
		break;
	    case -41:
		std::cerr << "CL_INVALID_SAMPLER ";
		break;
	    case -42:
		std::cerr << "CL_INVALID_BINARY ";
		break;
	    case -43:
		std::cerr << "CL_INVALID_BUILD_OPTIONS ";
		break;
	    case -44:
		std::cerr << "CL_INVALID_PROGRAM ";
		break;
	    case -45:
		std::cerr << "CL_INVALID_PROGRAM_EXECUTABLE ";
		break;
	    case -46:
		std::cerr << "CL_INVALID_KERNEL_NAME ";
		break;
	    case -47:
		std::cerr << "CL_INVALID_KERNEL_DEFINITION ";
		break;
	    case -48:
		std::cerr << "CL_INVALID_KERNEL ";
		break;
	    case -49:
		std::cerr << "CL_INVALID_ARG_INDEX ";
		break;
	    case -50:
		std::cerr << "CL_INVALID_ARG_VALUE ";
		break;
	    case -51:
		std::cerr << "CL_INVALID_ARG_SIZE ";
		break;
	    case -52:
		std::cerr << "CL_INVALID_KERNEL_ARGS ";
		break;
	    case -53:
		std::cerr << "CL_INVALID_WORK_DIMENSION ";
		break;
	    case -54:
		std::cerr << "CL_INVALID_WORK_GROUP_SIZE ";
		break;
	    case -55:
		std::cerr << "CL_INVALID_WORK_ITEM_SIZE ";
		break;
	    case -56:
		std::cerr << "CL_INVALID_GLOBAL_OFFSET ";
		break;
	    case -57:
		std::cerr << "CL_INVALID_EVENT_WAIT_LIST ";
		break;
	    case -58:
		std::cerr << "CL_INVALID_EVENT ";
		break;
	    case -59:
		std::cerr << "CL_INVALID_OPERATION ";
		break;
	    case -60:
		std::cerr << "CL_INVALID_GL_OBJECT ";
		break;
	    case -61:
		std::cerr << "CL_INVALID_BUFFER_SIZE ";
		break;
	    case -62:
		std::cerr << "CL_INVALID_MIP_LEVEL ";
		break;
	    case -63:
		std::cerr << "CL_INVALID_GLOBAL_WORK_SIZE ";
		break;
	    default:
		std::cerr << "UNRECOGNIZED ERROR CODE ("<<error<<")";
	}
    }


};
#endif // OCL_UTILS_HPP
