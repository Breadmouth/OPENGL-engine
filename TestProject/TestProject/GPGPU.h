#pragma once

#include "Application.h"
#include <CL/cl.h>

class GPGPU : public Application
{
public:
	GPGPU(){};
	virtual void Create();
	virtual void Destroy(){ 
		delete[] m_vectors;
		clReleaseMemObject(m_buffer);
		clReleaseKernel(m_kernel);
		clReleaseProgram(m_program);
		clReleaseCommandQueue(m_queue);
		clReleaseContext(m_context);
	};
	virtual void Update(float dt){};
	virtual void Draw(){};

protected:
	glm::vec4 *m_vectors;

	cl_platform_id m_platform;
	cl_device_id m_device;
	cl_context m_context;
	cl_command_queue m_queue;
	cl_program m_program;
	cl_kernel m_kernel;
	cl_mem m_buffer;
};