#include "GPGPU.h"

void GPGPU::Create()
{
	m_vectors = new glm::vec4[5000000];

	for (unsigned int i = 0; i < 5000000; ++i)
	{
		m_vectors[i] = glm::vec4(1);
	}

	//gte the first platform
	cl_int result = clGetPlatformIDs(1, &m_platform, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetPlatformIDs failed: %i\n", result);

	result = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_DEFAULT, 1, &m_device, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetDeviceIDs failed: %i\n", result);

	//create a context for a device on the specified platform
	cl_context_properties contextProperties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)m_platform, 0
	};
	m_context = clCreateContext(contextProperties, 1, &m_device, nullptr, nullptr, &result);

	if (result != CL_SUCCESS)
		printf("clCreateContext failed: %i\n", result);

	//create a command queue to process commands
	m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, &result);

	if (result != CL_SUCCESS)
		printf("clCreateCommandQueue failed: %i\n", result);

	const char* kernelSource = "__kernel void normalize_vec4( \
								__global float4* vectors) { \
								int i = get_global_id(0); \
								vectors[i] = normalize(vectors[i]); \
								}";
	unsigned int kernelSize = strlen(kernelSource);

	m_program = clCreateProgramWithSource(m_context, 1, (const char**)&kernelSource, &kernelSize, &result);
	if (result != CL_SUCCESS)
		printf("clCreateProgramWithSource failed: %i\n, result");

	clBuildProgram(m_program, 1, &m_device, nullptr, nullptr, nullptr);
	if (result != CL_SUCCESS)
		printf("clBuildProgram failed: %i\n", result);

	//extract the kernel
	m_kernel = clCreateKernel(m_program, "normalize_vec4", &result);
	if (result != CL_SUCCESS)
		printf("clCreateKernel failed: %i\n", result);

	m_buffer = clCreateBuffer(m_context,
			CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(glm::vec4) * 5000000, m_vectors,
			&result);

	if (result != CL_SUCCESS)
		printf("clCreateBuffer failed: %i\n", result);

	//set the buffer as the first argument for our kernel
	result = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_buffer);
	if (result != CL_SUCCESS)
		printf("clSetKernelArg failed: %i\n", result);

	//process the kernel and give it an event ID
	cl_event eventID = 0;

	//we'll give it a size equal to the number of vec4's to process
	size_t globalWorkSize[] = { 5000000 };

	result = clEnqueueNDRangeKernel(m_queue, m_kernel, 1, nullptr, globalWorkSize, nullptr, 0, nullptr, &eventID);

	if (result != CL_SUCCESS)
		printf("clEnqueueNDRangeKernel failed: %i\n", result);

	result = clEnqueueReadBuffer(m_queue, m_buffer, CL_TRUE, 0, sizeof(glm::vec4) * 5000000, m_vectors, 1, &eventID, nullptr);

	if (result != CL_SUCCESS)
		printf("clEnqueueuRedBuffer failed: %i\n", result);

	clFlush(m_queue);
	clFinish(m_queue);

	cl_ulong clstartTime = 0;
	result = clGetEventProfilingInfo(eventID, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &clstartTime, nullptr);

	if (result != CL_SUCCESS)
		printf("clGetEventProfilingInfo failed: %i\n", result);

	cl_ulong clendTime = 0;
	result = clGetEventProfilingInfo(eventID, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &clendTime, nullptr);

	if (result != CL_SUCCESS)
		printf("clGetEventProfilingInfo failed: %i\n", result);

	//return time in nanoseconds, so convert to seconds
	double clTime = (clendTime - clstartTime) * 1.0e-9;
	printf("GPU duration: %f\n", clTime);

	//double startTime = glfwGetTime();
	//
	//for (unsigned int i = 0; i < 5000000; ++i)
	//{
	//	m_vectors[i] = glm::normalize(m_vectors[i]);
	//}
	//
	//double endTime = glfwGetTime();
	//
	//printf("CPU duration: %f\n", endTime - startTime);

}