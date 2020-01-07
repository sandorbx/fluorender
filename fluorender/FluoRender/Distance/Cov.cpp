/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2018 Scientific Computing and Imaging Institute,
University of Utah.


Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
#include "Cov.h"
#include <FLIVR/VolumeRenderer.h>
#include <FLIVR/KernelProgram.h>
#include <FLIVR/VolKernel.h>
#include <FLIVR/TextureBrick.h>
#include <FLIVR/Texture.h>
#include <algorithm>

using namespace FL;

const char* str_cl_cov = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"__kernel void kernel_0(\n" \
"	__read_only image3d_t mask,\n" \
"	unsigned int ngx,\n" \
"	unsigned int ngy,\n" \
"	unsigned int ngz,\n" \
"	unsigned int gsxy,\n" \
"	unsigned int gsx,\n" \
"	__global unsigned int* count,\n" \
"	__global float* csum)\n" \
"{\n" \
"	int3 gid = (int3)(get_global_id(0),\n" \
"		get_global_id(1), get_global_id(2));\n" \
"	int3 lb = (int3)(gid.x*ngx, gid.y*ngy, gid.z*ngz);\n" \
"	int3 ub = (int3)(lb.x + ngx, lb.y + ngy, lb.z + ngz);\n" \
"	int4 ijk = (int4)(0, 0, 0, 1);\n" \
"	unsigned int lsum = 0;\n" \
"	float3 lcsum = (float3)(0.0, 0.0, 0.0);\n" \
"	for (ijk.x = lb.x; ijk.x < ub.x; ++ijk.x)\n" \
"	for (ijk.y = lb.y; ijk.y < ub.y; ++ijk.y)\n" \
"	for (ijk.z = lb.z; ijk.z < ub.z; ++ijk.z)\n" \
"	{\n" \
"		float mval = read_imagef(mask, samp, ijk).x;\n" \
"		if (mval > 0.0)\n" \
"		{\n" \
"			lsum++;\n" \
"			lcsum.x += (float)(ijk.x);\n" \
"			lcsum.y += (float)(ijk.y);\n" \
"			lcsum.z += (float)(ijk.z);\n" \
"		}\n" \
"	}\n" \
"	unsigned int index = gsxy * gid.z + gsx * gid.y + gid.x;\n" \
"	atomic_xchg(count+index, lsum);\n" \
"	atomic_xchg(csum+index*3, lcsum.x);\n" \
"	atomic_xchg(csum+index*3+1, lcsum.y);\n" \
"	atomic_xchg(csum+index*3+2, lcsum.z);\n" \
"}\n" \
"__kernel void kernel_1(\n" \
"	__read_only image3d_t mask,\n" \
"	unsigned int ngx,\n" \
"	unsigned int ngy,\n" \
"	unsigned int ngz,\n" \
"	unsigned int gsxy,\n" \
"	unsigned int gsx,\n" \
"	float3 center,\n" \
"	__global float* cov)\n" \
"{\n" \
"	int3 gid = (int3)(get_global_id(0),\n" \
"		get_global_id(1), get_global_id(2));\n" \
"	int3 lb = (int3)(gid.x*ngx, gid.y*ngy, gid.z*ngz);\n" \
"	int3 ub = (int3)(lb.x + ngx, lb.y + ngy, lb.z + ngz);\n" \
"	int4 ijk = (int4)(0, 0, 0, 1);\n" \
"	float xx = 0.0, xy = 0.0, xz = 0.0, yy = 0.0, yz = 0.0, zz = 0.0;\n" \
"	for (ijk.x = lb.x; ijk.x < ub.x; ++ijk.x)\n" \
"	for (ijk.y = lb.y; ijk.y < ub.y; ++ijk.y)\n" \
"	for (ijk.z = lb.z; ijk.z < ub.z; ++ijk.z)\n" \
"	{\n" \
"		float mval = read_imagef(mask, samp, ijk).x;\n" \
"		float3 fijk = (float3)(ijk.x, ijk.y, ijk.z) - center;\n" \
"		if (mval > 0.0)\n" \
"		{\n" \
"			xx += fijk.x * fijk.x;\n" \
"			xy += fijk.x * fijk.y;\n" \
"			xz += fijk.x * fijk.z;\n" \
"			yy += fijk.y * fijk.y;\n" \
"			yz += fijk.y * fijk.z;\n" \
"			zz += fijk.z * fijk.z;\n" \
"		}\n" \
"	}\n" \
"	unsigned int index = gsxy * gid.z + gsx * gid.y + gid.x;\n" \
"	atomic_xchg(cov+index*6, xx);\n" \
"	atomic_xchg(cov+index*6+1, xy);\n" \
"	atomic_xchg(cov+index*6+2, xz);\n" \
"	atomic_xchg(cov+index*6+3, yy);\n" \
"	atomic_xchg(cov+index*6+4, yz);\n" \
"	atomic_xchg(cov+index*6+5, zz);\n" \
"}\n" \
;

Cov::Cov(VolumeData* vd)
	: m_vd(vd),
	m_use_mask(false)
{
	std::memset(m_cov, 0, sizeof(float) * 9);
	std::memset(m_center, 0, sizeof(float) * 3);
}

Cov::~Cov()
{
}

bool Cov::CheckBricks()
{
	if (!m_vd)
		return false;
	if (!m_vd->GetTexture())
		return false;
	int brick_num = m_vd->GetTexture()->get_brick_num();
	if (!brick_num)
		return false;
	return true;
}

bool Cov::GetInfo(
	FLIVR::TextureBrick* b,
	long &bits, long &nx, long &ny, long &nz)
{
	bits = b->nb(0) * 8;
	nx = b->nx();
	ny = b->ny();
	nz = b->nz();
	return true;
}

void Cov::ReleaseData(void* val, long bits)
{
	if (bits == 8)
	{
		unsigned char* temp = (unsigned char*)val;
		delete[] temp;
	}
	else if (bits == 16)
	{
		unsigned short* temp = (unsigned short*)val;
		delete[] temp;
	}
}

long Cov::OptimizeGroupSize(long nt, long target)
{
	long loj, hij, res, maxj;
	//z
	if (nt > target)
	{
		loj = std::max(long(1), (target + 1) / 2);
		hij = std::min(nt, target * 2);
		res = 0; maxj = 0;
		for (long j = loj; j < hij; ++j)
		{
			long rm = nt % j;
			if (rm)
			{
				if (rm > res)
				{
					res = rm;
					maxj = j;
				}
			}
			else
			{
				return j;
			}
		}
		if (maxj)
			return maxj;
	}

	return target;
}

void Cov::Compute()
{
	if (!CheckBricks())
		return;
	if (!m_vd->GetMask(false))
		return;

	//create program and kernels
	FLIVR::KernelProgram* kernel_prog = FLIVR::VolumeRenderer::
		vol_kernel_factory_.kernel(str_cl_cov);
	if (!kernel_prog)
		return;
	int kernel_index0 = -1;
	int kernel_index1 = -1;
	string name0 = "kernel_0";
	string name1 = "kernel_1";
	if (kernel_prog->valid())
	{
		kernel_index0 = kernel_prog->findKernel(name0);
		kernel_index1 = kernel_prog->findKernel(name1);
	}
	else
	{
		kernel_index0 = kernel_prog->createKernel(name0);
		kernel_index1 = kernel_prog->createKernel(name1);
	}

	size_t brick_num = m_vd->GetTexture()->get_brick_num();
	vector<FLIVR::TextureBrick*> *bricks = m_vd->GetTexture()->get_bricks();

	//get center
	std::memset(m_center, 0, sizeof(float)*3);
	unsigned long long sum = 0;
	for (size_t i = 0; i < brick_num; ++i)
	{
		FLIVR::TextureBrick* b = (*bricks)[i];
		long nx, ny, nz, bits;
		if (!GetInfo(b, bits, nx, ny, nz))
			continue;
		//get tex ids
		GLint mid = m_vd->GetVR()->load_brick_mask(bricks, i);

		//compute workload
		size_t ng;
		kernel_prog->getWorkGroupSize(kernel_index0, &ng);
		//try to make gsxyz equal to ng
		//ngx*ngy*ngz = nx*ny*nz/ng
		//z
		long targetz = std::ceil(double(nz) / std::pow(double(ng), 1 / 3.0));
		//optimize
		long ngz = OptimizeGroupSize(nz, targetz);
		//xy
		long targetx;
		long targety;
		if (ngz == 1)
		{
			targetx = std::ceil(double(nx) / std::sqrt(double(ng)));
			targety = std::ceil(double(ny) / std::sqrt(double(ng)));
		}
		else
		{
			targetx = std::ceil(double(nx) * targetz / nz);
			targety = std::ceil(double(ny) * targetz / nz);
		}
		//optimize
		long ngx = OptimizeGroupSize(nx, targetx);
		long ngy = OptimizeGroupSize(ny, targety);

		long gsx = nx / ngx + (nx%ngx ? 1 : 0);
		long gsy = ny / ngy + (ny%ngy ? 1 : 0);
		long gsz = nz / ngz + (nz%ngz ? 1 : 0);
		long gsxyz = gsx * gsy * gsz;
		long gsxy = gsx * gsy;

		size_t local_size[3] = { 1, 1, 1 };
		size_t global_size[3] = { size_t(gsx), size_t(gsy), size_t(gsz) };

		//set
		unsigned int* count = new unsigned int[gsxyz];
		float *csum = new float[gsxyz*3];
		kernel_prog->setKernelArgTex3D(kernel_index0, 0,
			CL_MEM_READ_ONLY, mid);
		kernel_prog->setKernelArgConst(kernel_index0, 1,
			sizeof(unsigned int), (void*)(&ngx));
		kernel_prog->setKernelArgConst(kernel_index0, 2,
			sizeof(unsigned int), (void*)(&ngy));
		kernel_prog->setKernelArgConst(kernel_index0, 3,
			sizeof(unsigned int), (void*)(&ngz));
		kernel_prog->setKernelArgConst(kernel_index0, 4,
			sizeof(unsigned int), (void*)(&gsxy));
		kernel_prog->setKernelArgConst(kernel_index0, 5,
			sizeof(unsigned int), (void*)(&gsx));
		kernel_prog->setKernelArgBuf(kernel_index0, 6,
			CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(unsigned int)*(gsxyz), (void*)(count));
		kernel_prog->setKernelArgBuf(kernel_index0, 7,
			CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(float)*(gsxyz*3), (void*)(csum));

		//execute
		kernel_prog->executeKernel(kernel_index0, 3, global_size, 0);
		//read back
		kernel_prog->readBuffer(sizeof(unsigned int)*(gsxyz), count, count);
		kernel_prog->readBuffer(sizeof(float)*(gsxyz*3), csum, csum);

		//compute center
		for (int i = 0; i < gsxyz; ++i)
		{
			sum += count[i];
			m_center[0] += csum[i * 3];
			m_center[1] += csum[i * 3 + 1];
			m_center[2] += csum[i * 3 + 2];
		}

		//release buffer
		kernel_prog->releaseAll();
	}
	if (sum)
	{
		m_center[0] /= sum;
		m_center[1] /= sum;
		m_center[2] /= sum;
	}

	//get cov
	for (size_t i = 0; i < brick_num; ++i)
	{
		FLIVR::TextureBrick* b = (*bricks)[i];
		long nx, ny, nz, bits;
		if (!GetInfo(b, bits, nx, ny, nz))
			continue;
		//get tex ids
		GLint mid = m_vd->GetVR()->load_brick_mask(bricks, i);

		//compute workload
		size_t ng;
		kernel_prog->getWorkGroupSize(kernel_index0, &ng);
		//try to make gsxyz equal to ng
		//ngx*ngy*ngz = nx*ny*nz/ng
		//z
		long targetz = std::ceil(double(nz) / std::pow(double(ng), 1 / 3.0));
		//optimize
		long ngz = OptimizeGroupSize(nz, targetz);
		//xy
		long targetx;
		long targety;
		if (ngz == 1)
		{
			targetx = std::ceil(double(nx) / std::sqrt(double(ng)));
			targety = std::ceil(double(ny) / std::sqrt(double(ng)));
		}
		else
		{
			targetx = std::ceil(double(nx) * targetz / nz);
			targety = std::ceil(double(ny) * targetz / nz);
		}
		//optimize
		long ngx = OptimizeGroupSize(nx, targetx);
		long ngy = OptimizeGroupSize(ny, targety);

		long gsx = nx / ngx + (nx%ngx ? 1 : 0);
		long gsy = ny / ngy + (ny%ngy ? 1 : 0);
		long gsz = nz / ngz + (nz%ngz ? 1 : 0);
		long gsxyz = gsx * gsy * gsz;
		long gsxy = gsx * gsy;

		size_t local_size[3] = { 1, 1, 1 };
		size_t global_size[3] = { size_t(gsx), size_t(gsy), size_t(gsz) };

		//set
		unsigned int* count = new unsigned int[gsxyz];
		float *cov = new float[gsxyz * 6];
		kernel_prog->setKernelArgTex3D(kernel_index1, 0,
			CL_MEM_READ_ONLY, mid);
		kernel_prog->setKernelArgConst(kernel_index1, 1,
			sizeof(unsigned int), (void*)(&ngx));
		kernel_prog->setKernelArgConst(kernel_index1, 2,
			sizeof(unsigned int), (void*)(&ngy));
		kernel_prog->setKernelArgConst(kernel_index1, 3,
			sizeof(unsigned int), (void*)(&ngz));
		kernel_prog->setKernelArgConst(kernel_index1, 4,
			sizeof(unsigned int), (void*)(&gsxy));
		kernel_prog->setKernelArgConst(kernel_index1, 5,
			sizeof(unsigned int), (void*)(&gsx));
		kernel_prog->setKernelArgConst(kernel_index1, 6,
			sizeof(cl_float3), (void*)(m_center));
		kernel_prog->setKernelArgBuf(kernel_index1, 7,
			CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(float)*(gsxyz * 6), (void*)(cov));

		//execute
		kernel_prog->executeKernel(kernel_index1, 3, global_size, 0);
		//read back
		kernel_prog->readBuffer(sizeof(float)*(gsxyz * 6), cov, cov);

		//compute center
		for (int i = 0; i < gsxyz; ++i)
		{
			m_cov[0] += cov[i * 6];
			m_cov[1] += cov[i * 6 + 1];
			m_cov[2] += cov[i * 6 + 2];
			m_cov[3] += cov[i * 6 + 3];
			m_cov[4] += cov[i * 6 + 4];
			m_cov[5] += cov[i * 6 + 5];
		}

		//release buffer
		kernel_prog->releaseAll();
	}

	//set to correct spacings
	double spcx, spcy, spcz;
	m_vd->GetSpacings(spcx, spcy, spcz);
	m_center[0] *= spcx;
	m_center[1] *= spcy;
	m_center[2] *= spcz;
	m_cov[0] *= spcx * spcx;
	m_cov[1] *= spcx * spcy;
	m_cov[2] *= spcx * spcz;
	m_cov[3] *= spcy * spcy;
	m_cov[4] *= spcy * spcz;
	m_cov[5] *= spcz * spcz;
}
