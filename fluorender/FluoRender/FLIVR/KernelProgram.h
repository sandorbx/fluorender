#ifndef KernelProgram_h
#define KernelProgram_h

#include <GL/glew.h>
#if defined(_WIN32) || defined(__linux__)
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#endif
#ifdef _DARWIN
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLCurrent.h>
#endif
#include <string>
#include <vector>

namespace FLIVR
{
	class VolKernel;
	class KernelProgram
	{
	public:
		KernelProgram(const std::string& source);
		~KernelProgram();

		bool valid();
		void destroy();

		//create a kernel in the program
		//return kernel index; -1 unsuccessful
		int createKernel(std::string &name);
		int findKernel(std::string &name);
		//execute kernel
		bool executeKernel(int, cl_uint, size_t*, size_t*);
		bool executeKernel(std::string &name,
			cl_uint, size_t*, size_t*);

		//argument
		typedef struct
		{
			cl_uint index;
			size_t size;
			GLuint texture;
			cl_mem buffer;
		} Argument;
		bool matchArg(Argument*, unsigned int&);
		//set argument
		void setKernelArgConst(int, int, size_t, void*);
		void setKernelArgConst(std::string &name, int, size_t, void*);
		void setKernelArgBuf(int, int, cl_mem_flags, size_t, void*);
		void setKernelArgBuf(std::string &name, int, cl_mem_flags, size_t, void*);
		void setKernelArgBufWrite(int, int, cl_mem_flags, size_t, void*);
		void setKernelArgBufWrite(std::string &name, int, cl_mem_flags, size_t, void*);
		void setKernelArgTex2D(int, int, cl_mem_flags, GLuint);
		void setKernelArgTex2D(std::string &name, int, cl_mem_flags, GLuint);
		void setKernelArgTex3D(int, int, cl_mem_flags, GLuint);
		void setKernelArgTex3D(std::string &name, int, cl_mem_flags, GLuint);

		//read/write
		void readBuffer(int, void*);
		void writeBuffer(int, void*, size_t, size_t, size_t);

		//initialization
		static void init_kernels_supported();
		static bool init();
		static void clear();
		static void set_device_id(int id);
		static int get_device_id();
		static std::string& get_device_name();

		//info
		std::string &getInfo();

		friend class VolKernel;
#ifdef _DARWIN
		static CGLContextObj gl_context_;
#endif
	protected:
		std::string source_;
		cl_program program_;
		cl_command_queue queue_;

		//there can be multiple kernels in one program
		typedef struct
		{
			cl_kernel kernel;
			std::string name;
		} Kernel;
		std::vector<Kernel> kernels_;

		std::string info_;

		//memory object to release
		std::vector<Argument> arg_list_;

		static bool init_;
		static cl_device_id device_;
		static cl_context context_;
		static int device_id_;
		static std::string device_name_;
	};
}

#endif
