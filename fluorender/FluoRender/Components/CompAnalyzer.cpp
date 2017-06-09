/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2014 Scientific Computing and Imaging Institute,
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
#include "CompAnalyzer.h"
#include "DataManager.h"
#include <sstream>
#include <iostream>
#include <limits>

using namespace FL;

ComponentAnalyzer::ComponentAnalyzer(VolumeData* vd)
	: m_vd(vd),
	m_comp_list_dirty(true)
{
}

ComponentAnalyzer::~ComponentAnalyzer()
{
}

void ComponentAnalyzer::Analyze(bool sel)
{
	if (!m_vd || !m_vd->GetTexture())
		return;
	vector<TextureBrick*> *bricks = m_vd->GetTexture()->get_bricks();
	if (!bricks || bricks->size() == 0)
		return;

	size_t bn = bricks->size();
	//clear list and start calculating
	m_comp_list.clear();
	m_comp_graph.clear();

	if (sel)
		m_vd->GetVR()->return_mask();

	int bits;
	for (size_t bi = 0; bi < bn; ++bi)
	{
		void* data_data = 0;
		unsigned char* data_mask = 0;
		unsigned int* data_label = 0;
		int nx, ny, nz;
		TextureBrick* b = (*bricks)[bi];
		int c = 0;
		int nb = 1;
		if (bn > 1)
		{
			// get brick if ther are more than one brick
			nb = b->nb(0);
			nx = b->nx();
			ny = b->ny();
			nz = b->nz();
			bits = nb==1? nrrdTypeUChar: nrrdTypeUShort;
			//size
			unsigned long long mem_size = (unsigned long long)nx*
				(unsigned long long)ny*(unsigned long long)nz*nb;
			//data
			unsigned char* temp = new unsigned char[mem_size];
			unsigned char* tempp = temp;
			unsigned char* tp = (unsigned char*)(b->tex_data(0));
			unsigned char* tp2;
			for (unsigned int k = 0; k < nz; ++k)
			{
				tp2 = tp;
				for (unsigned int j = 0; j < ny; ++j)
				{
					memcpy(tempp, tp2, nx*nb);
					tempp += nx*nb;
					tp2 += b->sx()*nb;
				}
				tp += b->sx()*b->sy()*nb;
			}
			data_data = (void*)temp;
			//mask
			if (sel)
			{
				c = b->nmask();
				nb = b->nb(c);
				mem_size = (unsigned long long)nx*
					(unsigned long long)ny*(unsigned long long)nz*nb;
				temp = new unsigned char[mem_size];
				tempp = temp;
				tp = (unsigned char*)(b->tex_data(c));
				for (unsigned int k = 0; k < nz; ++k)
				{
					tp2 = tp;
					for (unsigned int j = 0; j < ny; ++j)
					{
						memcpy(tempp, tp2, nx*nb);
						tempp += nx*nb;
						tp2 += b->sx()*nb;
					}
					tp += b->sx()*b->sy()*nb;
				}
				data_mask = temp;
			}
			//label
			c = b->nlabel();
			nb = b->nb(c);
			mem_size = (unsigned long long)nx*
				(unsigned long long)ny*(unsigned long long)nz*nb;
			temp = new unsigned char[mem_size];
			tempp = temp;
			tp = (unsigned char*)(b->tex_data(c));
			for (unsigned int k = 0; k < nz; ++k)
			{
				tp2 = tp;
				for (unsigned int j = 0; j < ny; ++j)
				{
					memcpy(tempp, tp2, nx*nb);
					tempp += nx*nb;
					tp2 += b->sx()*nb;
				}
				tp += b->sx()*b->sy()*nb;
			}
			data_label = (unsigned int*)temp;
		}
		else
		{
			// get data if there is only one brick
			m_vd->GetResolution(nx, ny, nz);
			Nrrd* nrrd_data = m_vd->GetVolume(false);
			if (nrrd_data)
			{
				bits = nrrd_data->type;
				data_data = nrrd_data->data;
			}
			Nrrd* nrrd_mask = m_vd->GetMask(false);
			if (nrrd_mask)
				data_mask = (unsigned char*)(nrrd_mask->data);
			Nrrd* nrrd_label = m_vd->GetLabel(false);
			if (nrrd_label)
				data_label = (unsigned int*)(nrrd_label->data);
			if (!data_data || (sel && !data_mask) || !data_label)
				return;
		}

		unsigned int id = 0;
		unsigned int brick_id = b->get_id();
		double value;
		double scale;
		double delta;
		double ext;
		int i, j, k;
		//m_vd->GetResolution(nx, ny, nz);
		unsigned long long for_size = (unsigned long long)nx *
			(unsigned long long)ny * (unsigned long long)nz;
		unsigned long long index;
		CompUList comp_ulist;
		CompUListIter iter;
		for (index = 0; index < for_size; ++index)
		{
			value = 0.0;
			if (sel)
			{
				if (data_mask && !data_mask[index])
					continue;
			}
			if (data_label && !data_label[index])
				continue;

			if (data_label)
				id = data_label[index];

			if (bits == nrrdTypeUChar)
			{
				value = ((unsigned char*)data_data)[index] / 255.0;
				scale = 255.0;
			}
			else if (bits == nrrdTypeUShort)
			{
				value = ((unsigned short*)data_data)[index] / 65535.0;
				scale = 65535.0;
			}

			if (value <= 0.0)
				continue;

			k = index / (nx*ny);
			j = index % (nx*ny);
			i = j % nx;
			j = j / nx;
			ext = GetExt(data_label, index, id, nx, ny, nz, i, j, k);

			//find in list
			iter = comp_ulist.find(GetKey(id, brick_id));
			if (iter == comp_ulist.end())
			{
				//not found
				CompInfo info;
				info.id = id;
				info.brick_id = brick_id;
				info.sumi = 1;
				info.sumd = value;
				info.ext_sumi = ext;
				info.ext_sumd = value * ext;
				info.mean = 0.0;
				info.var = 0.0;
				info.m2 = 0.0;
				delta = value - info.mean;
				info.mean += delta / info.sumi;
				info.m2 += delta * (value - info.mean);
				info.min = value;
				info.max = value;
				info.pos = FLIVR::Point(i, j, k);
				comp_ulist.insert(pair<unsigned long long, CompInfo>
					(GetKey(id, brick_id), info));
			}
			else
			{
				iter->second.pos = FLIVR::Point((iter->second.pos * iter->second.sumi +
					FLIVR::Point(i, j, k)) / (iter->second.sumi + 1));
				//
				iter->second.sumi++;
				iter->second.sumd += value;
				iter->second.ext_sumi += ext;
				iter->second.ext_sumd += value * ext;
				//
				delta = value - iter->second.mean;
				iter->second.mean += delta / iter->second.sumi;
				iter->second.m2 += delta * (value - iter->second.mean);
				iter->second.min = value < iter->second.min ? value : iter->second.min;
				iter->second.max = value > iter->second.max ? value : iter->second.max;
			}
		}

		m_comp_list.min = std::numeric_limits<unsigned int>::max();
		m_comp_list.max = 0;
		for (iter = comp_ulist.begin();
			iter != comp_ulist.end(); ++iter)
		{
			if (!sel && iter->second.sumi < 2)
				continue;
			iter->second.var = sqrt(iter->second.m2 / (iter->second.sumi));
			iter->second.mean *= scale;
			iter->second.min *= scale;
			iter->second.max *= scale;
			m_comp_list.min = iter->second.sumi <
				m_comp_list.min ? iter->second.sumi :
				m_comp_list.min;
			m_comp_list.max = iter->second.sumi >
				m_comp_list.max ? iter->second.sumi :
				m_comp_list.max;
			m_comp_list.insert(std::pair<unsigned long long, CompInfo>
				(iter->first, iter->second));
		}

		if (bn > 1)
		{
			if (data_data) delete[] data_data;
			if (data_mask) delete[] data_mask;
			if (data_label) delete[] data_label;
		}
	}

	MatchBricks(sel);

	m_comp_list_dirty = false;
}

void ComponentAnalyzer::MatchBricks(bool sel)
{
	if (!m_vd || !m_vd->GetTexture())
		return;
	Texture* tex = m_vd->GetTexture();
	vector<TextureBrick*> *bricks = tex->get_bricks();
	if (!bricks || bricks->size() <= 1)
		return;

	size_t bn = bricks->size();

	int bits;
	for (size_t bi = 0; bi < bn; ++bi)
	{
		void* data_data = 0;
		unsigned char* data_mask = 0;
		unsigned int* data_label = 0;
		int nx, ny, nz;
		int c = 0;
		int nb = 1;
		//get one brick
		TextureBrick* b = (*bricks)[bi];
		nb = b->nb(0);
		nx = b->nx();
		ny = b->ny();
		nz = b->nz();
		bits = nb * 8;
		//label
		c = b->nlabel();
		nb = b->nb(c);
		unsigned long long mem_size = (unsigned long long)nx*
			(unsigned long long)ny*(unsigned long long)nz*nb;
		unsigned char* temp = new unsigned char[mem_size];
		unsigned char* tempp = temp;
		unsigned char* tp = (unsigned char*)(b->tex_data(c));
		unsigned char* tp2;
		for (unsigned int k = 0; k < nz; ++k)
		{
			tp2 = tp;
			for (unsigned int j = 0; j < ny; ++j)
			{
				memcpy(tempp, tp2, nx*nb);
				tempp += nx*nb;
				tp2 += b->sx()*nb;
			}
			tp += b->sx()*b->sy()*nb;
		}
		data_label = (unsigned int*)temp;
		//check all 6 faces
		unsigned int l1, l2, index, b1, b2;
		//(x, y, 0)
		for (unsigned int j = 0; j < ny; ++j)
		for (unsigned int i = 0; i < nx; ++i)
		{
			index = j*nx + i;//diff
			l1 = data_label[index];
			if (!l1) continue;
			index += nx*ny;//diff
			l2 = data_label[index];
			if (!l2 || l1 == l2) continue;
			//get brick ids
			b2 = b->get_id();
			b1 = tex->negzid(b2);//diff
			if (b1 == b2) continue;
			//if l1 and l2 are different and already in the comp list
			//connect them
			auto i1 = m_comp_list.find(GetKey(l1, b1));
			auto i2 = m_comp_list.find(GetKey(l2, b2));
			if (i1 != m_comp_list.end() && i2 != m_comp_list.end())
				m_comp_graph.LinkComps(i1->second, i2->second);
		}
		//(x, y, nz-1)
		for (unsigned int j = 0; j < ny; ++j)
		for (unsigned int i = 0; i < nx; ++i)
		{
			index = nx*ny*(nz - 1) + j*nx + i;
			l1 = data_label[index];
			if (!l1) continue;
			index -= nx*ny;
			l2 = data_label[index];
			if (!l2 || l1 == l2) continue;
			//get brick ids
			b2 = b->get_id();
			b1 = tex->poszid(b2);
			if (b1 == b2) continue;
			//if l1 and l2 are different and already in the comp list
			//connect them
			auto i1 = m_comp_list.find(GetKey(l1, b1));
			auto i2 = m_comp_list.find(GetKey(l2, b2));
			if (i1 != m_comp_list.end() && i2 != m_comp_list.end())
				m_comp_graph.LinkComps(i1->second, i2->second);
		}
		//(x, 0, z)
		for (unsigned int k = 0; k < nz; ++k)
		for (unsigned int i = 0; i < nx; ++i)
		{
			index = nx*ny*k + i;
			l1 = data_label[index];
			if (!l1) continue;
			index += nx;
			l2 = data_label[index];
			if (!l2 || l1 == l2) continue;
			//get brick ids
			b2 = b->get_id();
			b1 = tex->negyid(b2);
			if (b1 == b2) continue;
			//if l1 and l2 are different and already in the comp list
			//connect them
			auto i1 = m_comp_list.find(GetKey(l1, b1));
			auto i2 = m_comp_list.find(GetKey(l2, b2));
			if (i1 != m_comp_list.end() && i2 != m_comp_list.end())
				m_comp_graph.LinkComps(i1->second, i2->second);
		}
		//(x, ny-1, z)
		for (unsigned int k = 0; k < nz; ++k)
		for (unsigned int i = 0; i < nx; ++i)
		{
			index = nx*ny*k + nx*(ny-1) + i;
			l1 = data_label[index];
			if (!l1) continue;
			index -= nx;
			l2 = data_label[index];
			if (!l2 || l1 == l2) continue;
			//get brick ids
			b2 = b->get_id();
			b1 = tex->posyid(b2);
			if (b1 == b2) continue;
			//if l1 and l2 are different and already in the comp list
			//connect them
			auto i1 = m_comp_list.find(GetKey(l1, b1));
			auto i2 = m_comp_list.find(GetKey(l2, b2));
			if (i1 != m_comp_list.end() && i2 != m_comp_list.end())
				m_comp_graph.LinkComps(i1->second, i2->second);
		}
		//(0, y, z)
		for (unsigned int k = 0; k < nz; ++k)
		for (unsigned int j = 0; j < ny; ++j)
		{
			index = nx*ny*k + nx*j;
			l1 = data_label[index];
			if (!l1) continue;
			index += 1;
			l2 = data_label[index];
			if (!l2 || l1 == l2) continue;
			//get brick ids
			b2 = b->get_id();
			b1 = tex->negxid(b2);
			if (b1 == b2) continue;
			//if l1 and l2 are different and already in the comp list
			//connect them
			auto i1 = m_comp_list.find(GetKey(l1, b1));
			auto i2 = m_comp_list.find(GetKey(l2, b2));
			if (i1 != m_comp_list.end() && i2 != m_comp_list.end())
				m_comp_graph.LinkComps(i1->second, i2->second);
		}
		//(nx-1, y, z)
		for (unsigned int k = 0; k < nz; ++k)
		for (unsigned int j = 0; j < ny; ++j)
		{
			index = nx*ny*k + nx*j + nx-1;
			l1 = data_label[index];
			if (!l1) continue;
			index -= 1;
			l2 = data_label[index];
			if (!l2 || l1 == l2) continue;
			//get brick ids
			b2 = b->get_id();
			b1 = tex->posxid(b2);
			if (b1 == b2) continue;
			//if l1 and l2 are different and already in the comp list
			//connect them
			auto i1 = m_comp_list.find(GetKey(l1, b1));
			auto i2 = m_comp_list.find(GetKey(l2, b2));
			if (i1 != m_comp_list.end() && i2 != m_comp_list.end())
				m_comp_graph.LinkComps(i1->second, i2->second);
		}

		if (data_data) delete[] data_data;
		if (data_mask) delete[] data_mask;
		if (data_label) delete[] data_label;
	}
}

void ComponentAnalyzer::OutputFormHeader(std::string &str)
{
	if (m_vd && m_vd->GetBrickNum() > 1)
		str = "BRICK_ID\tID\tPosX\tPosY\tPosZ\tSumN\tSumI\tSurfaceN\tSurfaceI\tMean\tSigma\tMin\tMax\n";
	else
		str = "ID\tPosX\tPosY\tPosZ\tSumN\tSumI\tSurfaceN\tSurfaceI\tMean\tSigma\tMin\tMax\n";
}

void ComponentAnalyzer::OutputCompList(std::string &str, int verbose, std::string comp_header)
{
	int bn = m_vd->GetBrickNum();

	ostringstream oss;
	if (verbose == 1)
	{
		oss << "Statistics on the selection:\n";
		oss << "A total of " <<
			m_comp_list.size() <<
			" component(s) selected\n";
		std::string header;
		OutputFormHeader(header);
		oss << header;
	}
	for (auto i = m_comp_list.begin();
		i != m_comp_list.end(); ++i)
	{
		if (comp_header != "")
		{
			if (i == m_comp_list.begin())
				oss << comp_header << "\t";
			else
				oss << "\t";
		}

		std::list<unsigned int> ids;
		std::list<unsigned int> brick_ids;
		unsigned int sumi;
		double sumd;
		unsigned int ext_sumi;
		double ext_sumd;
		double mean;
		double var;
		double min;
		double max;
		FLIVR::Point pos;

		if (bn > 1)
		{
			if (m_comp_graph.Visited(i->second))
				continue;

			CompUList list;
			if (m_comp_graph.GetLinkedComps(i->second, list))
			{
				sumi = 0;
				sumd = 0.0;
				ext_sumi = 0;
				ext_sumd = 0.0;
				mean = 0.0;
				var = 0.0;
				min = std::numeric_limits<double>::max();
				max = 0.0;

				for (auto iter = list.begin();
					iter != list.end(); ++iter)
				{
					ids.push_back(iter->second.id);
					brick_ids.push_back(iter->second.brick_id);
					sumi += iter->second.sumi;
					sumd += iter->second.sumd;
					ext_sumi += iter->second.ext_sumi;
					ext_sumd += iter->second.ext_sumd;
					//mean
					//var
					min = iter->second.min < min ? iter->second.min : min;
					max = iter->second.max > max ? iter->second.max : max;
					//pos
				}
			}
			else
			{
				ids.push_back(i->second.id);
				brick_ids.push_back(i->second.brick_id);
				sumi = i->second.sumi;
				sumd = i->second.sumd;
				ext_sumi = i->second.ext_sumi;
				ext_sumd = i->second.ext_sumd;
				mean = i->second.mean;
				var = i->second.mean;
				min = i->second.min;
				max = i->second.max;
				pos = i->second.pos;
			}
		}
		else
		{
			ids.push_back(i->second.id);
			brick_ids.push_back(i->second.brick_id);
			sumi = i->second.sumi;
			sumd = i->second.sumd;
			ext_sumi = i->second.ext_sumi;
			ext_sumd = i->second.ext_sumd;
			mean = i->second.mean;
			var = i->second.mean;
			min = i->second.min;
			max = i->second.max;
			pos = i->second.pos;
		}

		if (bn > 1)
			oss << brick_ids.front() << "\t";
		oss << ids.front() << "\t";
		oss << pos.x() << "\t";
		oss << pos.y() << "\t";
		oss << pos.z() << "\t";
		oss << sumi << "\t";
		oss << sumd << "\t";
		oss << ext_sumi << "\t";
		oss << ext_sumd << "\t";
		oss << mean << "\t";
		oss << var << "\t";
		oss << min << "\t";
		oss << max << "\n";
	}
	str = oss.str();
}

unsigned int ComponentAnalyzer::GetExt(unsigned int* data_label,
	unsigned long long index,
	unsigned int id,
	int nx, int ny, int nz,
	int i, int j, int k)
{
	if (!data_label)
		return 0;
	bool surface_vox, contact_vox;
	unsigned long long indexn;
	//determine the numbers
	if (i == 0 || i == nx - 1 ||
		j == 0 || j == ny - 1 ||
		k == 0 || k == nz - 1)
	{
		//border voxel
		surface_vox = true;
		//determine contact
		contact_vox = false;
		if (i > 0)
		{
			indexn = index - 1;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				contact_vox = true;
		}
		if (!contact_vox && i < nx - 1)
		{
			indexn = index + 1;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				contact_vox = true;
		}
		if (!contact_vox && j > 0)
		{
			indexn = index - nx;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				contact_vox = true;
		}
		if (!contact_vox && j < ny - 1)
		{
			indexn = index + nx;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				contact_vox = true;
		}
		if (!contact_vox && k > 0)
		{
			indexn = index - nx*ny;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				contact_vox = true;
		}
		if (!contact_vox && k < nz - 1)
		{
			indexn = index + nx*ny;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				contact_vox = true;
		}
	}
	else
	{
		surface_vox = false;
		contact_vox = false;
		//i-1
		indexn = index - 1;
		if (data_label[indexn] == 0)
			surface_vox = true;
		if (data_label[indexn] &&
			data_label[indexn] != id)
			surface_vox = contact_vox = true;
		//i+1
		if (!surface_vox || !contact_vox)
		{
			indexn = index + 1;
			if (data_label[indexn] == 0)
				surface_vox = true;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				surface_vox = contact_vox = true;
		}
		//j-1
		if (!surface_vox || !contact_vox)
		{
			indexn = index - nx;
			if (data_label[indexn] == 0)
				surface_vox = true;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				surface_vox = contact_vox = true;
		}
		//j+1
		if (!surface_vox || !contact_vox)
		{
			indexn = index + nx;
			if (data_label[indexn] == 0)
				surface_vox = true;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				surface_vox = contact_vox = true;
		}
		//k-1
		if (!surface_vox || !contact_vox)
		{
			indexn = index - nx*ny;
			if (data_label[indexn] == 0)
				surface_vox = true;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				surface_vox = contact_vox = true;
		}
		//k+1
		if (!surface_vox || !contact_vox)
		{
			indexn = index + nx*ny;
			if (data_label[indexn] == 0)
				surface_vox = true;
			if (data_label[indexn] &&
				data_label[indexn] != id)
				surface_vox = contact_vox = true;
		}
	}

	return surface_vox ? 1 : 0;
}

bool ComponentAnalyzer::GenAnnotations(Annotations &ann)
{
	if (!m_vd)
		return false;

	Texture* tex = m_vd->GetTexture();
	if (!tex)
		return false;
	Nrrd* nrrd_data = tex->get_nrrd(0);
	if (!nrrd_data)
		return false;
	int bits = nrrd_data->type;
	double scale;
	if (bits == nrrdTypeUChar)
		scale = 255.0;
	else if (bits == nrrdTypeUShort)
		scale = 65535.0;
	double spcx, spcy, spcz;
	m_vd->GetSpacings(spcx, spcy, spcz);
	int nx, ny, nz;
	m_vd->GetResolution(nx, ny, nz);

	if (m_comp_list.empty() ||
		m_comp_list_dirty)
		Analyze(true);

	double total_int = 0.0;
	unsigned int sum = 0;
	std::string sinfo;
	ostringstream oss;
	for (auto i = m_comp_list.begin();
		i != m_comp_list.end(); ++i)
	{
		oss.str("");
		oss << i->second.sumi << "\t";
		oss << double(i->second.sumi)*spcx*spcy*spcz << "\t";
		oss << i->second.mean;
		sinfo = oss.str();
		ann.AddText(std::to_string(i->second.id),
			FLIVR::Point(i->second.pos.x()/nx,
			i->second.pos.y()/ny, i->second.pos.z()/nz),
			sinfo);
		total_int += i->second.sumd * scale;
		sum += i->second.sumi;
	}
	return true;
}

bool ComponentAnalyzer::GenMultiChannels(std::list<VolumeData*>& channs, int color_type)
{
	if (!m_vd)
		return false;

	if (m_comp_list.empty() ||
		m_comp_list_dirty)
		Analyze(true);

	Texture* tex = m_vd->GetTexture();
	if (!tex)
		return false;
	Nrrd* nrrd_data = tex->get_nrrd(0);
	if (!nrrd_data)
		return false;
	int bits = 8;
	if (nrrd_data->type == nrrdTypeUChar)
		bits = 8;
	else if (nrrd_data->type == nrrdTypeUShort)
		bits = 16;
	void* data_data = nrrd_data->data;
	if (!data_data)
		return false;
	//get label
	Nrrd* nrrd_label = tex->get_nrrd(tex->nlabel());
	if (!nrrd_label)
		return false;
	unsigned int* data_label = (unsigned int*)(nrrd_label->data);
	if (!data_label)
		return false;
	double spcx, spcy, spcz;
	m_vd->GetSpacings(spcx, spcy, spcz);
	int nx, ny, nz;
	m_vd->GetResolution(nx, ny, nz);
	double amb, diff, spec, shine;
	m_vd->GetMaterial(amb, diff, spec, shine);

	unsigned int count = 1;
	unsigned long long for_size = (unsigned long long)nx *
		(unsigned long long)ny * (unsigned long long)nz;
	unsigned long long index;
	unsigned int value_label;
	for (auto i = m_comp_list.begin();
		i != m_comp_list.end(); ++i)
	{
		VolumeData* vd = new VolumeData();
		vd->AddEmptyData(bits,
			nx, ny, nz,
			spcx, spcy, spcz);
		vd->SetSpcFromFile(true);
		vd->SetName(m_vd->GetName() +
			wxString::Format("_COMP%d_SIZE%d", count++, i->second.sumi));

		//populate the volume
		//the actual data
		Texture* tex_vd = vd->GetTexture();
		if (!tex_vd) continue;
		Nrrd* nrrd_vd = tex_vd->get_nrrd(0);
		if (!nrrd_vd) continue;
		void* data_vd = nrrd_vd->data;
		if (!data_vd) continue;
		for (index = 0; index < for_size; ++index)
		{
			value_label = data_label[index];
			if (value_label == i->second.id)
			{
				if (bits == 8)
					((unsigned char*)data_vd)[index] = ((unsigned char*)data_data)[index];
				else
					((unsigned short*)data_vd)[index] = ((unsigned short*)data_data)[index];
			}
		}

		//settings
		Color c = GetColor(i->second, m_vd, color_type);
		vd->SetColor(c);
		vd->SetEnableAlpha(m_vd->GetEnableAlpha());
		vd->SetShading(m_vd->GetShading());
		vd->SetShadow(false);
		//other settings
		vd->Set3DGamma(m_vd->Get3DGamma());
		vd->SetBoundary(m_vd->GetBoundary());
		vd->SetOffset(m_vd->GetOffset());
		vd->SetLeftThresh(m_vd->GetLeftThresh());
		vd->SetRightThresh(m_vd->GetRightThresh());
		vd->SetAlpha(m_vd->GetAlpha());
		vd->SetSampleRate(m_vd->GetSampleRate());
		vd->SetMaterial(amb, diff, spec, shine);

		channs.push_back(vd);
	}
	return true;
}

bool ComponentAnalyzer::GenRgbChannels(std::list<VolumeData*> &channs, int color_type)
{
	if (!m_vd)
		return false;

	if (m_comp_list.empty() ||
		m_comp_list_dirty)
		Analyze(true);

	Texture* tex = m_vd->GetTexture();
	if (!tex)
		return false;
	Nrrd* nrrd_data = tex->get_nrrd(0);
	if (!nrrd_data)
		return false;
	int bits = 8;
	if (nrrd_data->type == nrrdTypeUChar)
		bits = 8;
	else if (nrrd_data->type == nrrdTypeUShort)
		bits = 16;
	void* data_data = nrrd_data->data;
	if (!data_data)
		return false;
	//get label
	Nrrd* nrrd_label = tex->get_nrrd(tex->nlabel());
	if (!nrrd_label)
		return false;
	unsigned int* data_label = (unsigned int*)(nrrd_label->data);
	if (!data_label)
		return false;
	double spcx, spcy, spcz;
	m_vd->GetSpacings(spcx, spcy, spcz);
	int nx, ny, nz;
	m_vd->GetResolution(nx, ny, nz);
	double amb, diff, spec, shine;
	m_vd->GetMaterial(amb, diff, spec, shine);

	//red volume
	VolumeData* vd_r = new VolumeData();
	vd_r->AddEmptyData(bits,
		nx, ny, nz,
		spcx, spcy, spcz);
	vd_r->SetSpcFromFile(true);
	vd_r->SetName(m_vd->GetName() +
		wxString::Format("_CH_R"));
	//green volume
	VolumeData* vd_g = new VolumeData();
	vd_g->AddEmptyData(bits,
		nx, ny, nz,
		spcx, spcy, spcz);
	vd_g->SetSpcFromFile(true);
	vd_g->SetName(m_vd->GetName() +
		wxString::Format("_CH_G"));
	//blue volume
	VolumeData* vd_b = new VolumeData();
	vd_b->AddEmptyData(bits,
		nx, ny, nz,
		spcx, spcy, spcz);
	vd_b->SetSpcFromFile(true);
	vd_b->SetName(m_vd->GetName() +
		wxString::Format("_CH_B"));

	//get new data
	//red volume
	Texture* tex_vd_r = vd_r->GetTexture();
	if (!tex_vd_r) return false;
	Nrrd* nrrd_vd_r = tex_vd_r->get_nrrd(0);
	if (!nrrd_vd_r) return false;
	void* data_vd_r = nrrd_vd_r->data;
	if (!data_vd_r) return false;
	//green volume
	Texture* tex_vd_g = vd_g->GetTexture();
	if (!tex_vd_g) return false;
	Nrrd* nrrd_vd_g = tex_vd_g->get_nrrd(0);
	if (!nrrd_vd_g) return false;
	void* data_vd_g = nrrd_vd_g->data;
	if (!data_vd_g) return false;
	//blue volume
	Texture* tex_vd_b = vd_b->GetTexture();
	if (!tex_vd_b) return false;
	Nrrd* nrrd_vd_b = tex_vd_b->get_nrrd(0);
	if (!nrrd_vd_b) return false;
	void* data_vd_b = nrrd_vd_b->data;
	if (!data_vd_b) return false;

	unsigned long long for_size = (unsigned long long)nx *
		(unsigned long long)ny * (unsigned long long)nz;
	unsigned long long index;
	unsigned int value_label;
	Color color;
	for (index = 0; index < for_size; ++index)
	{
		value_label = data_label[index];
		//auto i = std::find(m_comp_list.begin(),
		//	m_comp_list.end(), CompInfo(value_label, 0));
		auto i = m_comp_list.find(GetKey(value_label, 0));//to add bricks
		if (i != m_comp_list.end())
		{
			color = GetColor(i->second, m_vd, color_type);
			if (bits == 8)
			{
				double value = ((unsigned char*)data_data)[index];
				((unsigned char*)data_vd_r)[index] = (unsigned char)(color.r()*value);
				((unsigned char*)data_vd_g)[index] = (unsigned char)(color.g()*value);
				((unsigned char*)data_vd_b)[index] = (unsigned char)(color.b()*value);
			}
			else
			{
				double value = ((unsigned short*)data_data)[index];
				((unsigned short*)data_vd_r)[index] = (unsigned short)(color.r()*value);
				((unsigned short*)data_vd_g)[index] = (unsigned short)(color.g()*value);
				((unsigned short*)data_vd_b)[index] = (unsigned short)(color.b()*value);
			}
		}
	}

	FLIVR::Color red = Color(1.0, 0.0, 0.0);
	FLIVR::Color green = Color(0.0, 1.0, 0.0);
	FLIVR::Color blue = Color(0.0, 0.0, 1.0);
	vd_r->SetColor(red);
	vd_g->SetColor(green);
	vd_b->SetColor(blue);

	bool bval = m_vd->GetEnableAlpha();
	vd_r->SetEnableAlpha(bval);
	vd_g->SetEnableAlpha(bval);
	vd_b->SetEnableAlpha(bval);
	bval = m_vd->GetShading();
	vd_r->SetShading(bval);
	vd_g->SetShading(bval);
	vd_b->SetShading(bval);
	vd_r->SetShadow(false);
	vd_g->SetShadow(false);
	vd_b->SetShadow(false);
	//other settings
	double dval = m_vd->Get3DGamma();
	vd_r->Set3DGamma(dval);
	vd_g->Set3DGamma(dval);
	vd_b->Set3DGamma(dval);
	dval = m_vd->GetBoundary();
	vd_r->SetBoundary(dval);
	vd_g->SetBoundary(dval);
	vd_b->SetBoundary(dval);
	dval = m_vd->GetOffset();
	vd_r->SetOffset(dval);
	vd_g->SetOffset(dval);
	vd_b->SetOffset(dval);
	dval = m_vd->GetLeftThresh();
	vd_r->SetLeftThresh(dval);
	vd_g->SetLeftThresh(dval);
	vd_b->SetLeftThresh(dval);
	dval = m_vd->GetRightThresh();
	vd_r->SetRightThresh(dval);
	vd_g->SetRightThresh(dval);
	vd_b->SetRightThresh(dval);
	dval = m_vd->GetAlpha();
	vd_r->SetAlpha(dval);
	vd_g->SetAlpha(dval);
	vd_b->SetAlpha(dval);
	dval = m_vd->GetSampleRate();
	vd_r->SetSampleRate(dval);
	vd_g->SetSampleRate(dval);
	vd_b->SetSampleRate(dval);
	vd_r->SetMaterial(amb, diff, spec, shine);
	vd_g->SetMaterial(amb, diff, spec, shine);
	vd_b->SetMaterial(amb, diff, spec, shine);

	channs.push_back(vd_r);
	channs.push_back(vd_g);
	channs.push_back(vd_b);

	return true;
}

FLIVR::Color ComponentAnalyzer::GetColor(CompInfo &comp_info,
	VolumeData* vd, int color_type)
{
	FLIVR::Color color;
	switch (color_type)
	{
	case 1:
	default:
		color = FLIVR::Color(HSVColor(comp_info.id % 360, 1.0, 1.0));
		break;
	case 2:
		if (vd)
		{
			double value;
			if (m_comp_list.min == m_comp_list.max)
				value = 1.0;
			else
				value = double(comp_info.sumi - m_comp_list.min) /
				double(m_comp_list.max - m_comp_list.min);
			color = vd->GetColorFromColormap(value);
		}
		break;
	}
	return color;
}
