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
#ifndef FL_Cell_h
#define FL_Cell_h

#include <FLIVR/Point.h>
#include <FLIVR/BBox.h>
#include <FLIVR/Color.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>

namespace FL
{
	class Vertex;
	typedef boost::shared_ptr<Vertex> pVertex;
	typedef boost::weak_ptr<Vertex> pwVertex;
	class Cell;
	typedef boost::shared_ptr<Cell> pCell;
	typedef boost::weak_ptr<Cell> pwCell;

	struct IntraEdgeData
	{
		//contact size
		unsigned int size_ui;//voxel count
		float size_f;//voxel count weighted by intensity
		//distance
		float dist_v;//distance calculated from voxel grid
		float dist_s;//distance calculated from xyz spatial coordinates
	};

	struct IntraCellData
	{
		unsigned int id;
		pwCell cell;
	};

	typedef boost::adjacency_list<boost::listS,
		boost::listS, boost::undirectedS,
		IntraCellData, IntraEdgeData> IntraGraph;
	typedef IntraGraph::vertex_descriptor IntraVert;
	typedef IntraGraph::edge_descriptor IntraEdge;
	typedef boost::graph_traits<IntraGraph>::adjacency_iterator IntraAdjIter;
	typedef boost::graph_traits<IntraGraph>::edge_iterator IntraEdgeIter;

	class Cell
	{
	public:
		Cell(unsigned int id) :
			m_id(id), m_brick_id(0),
			m_size_ui(0), m_size_f(0.0f),
			m_external_ui(0), m_external_f(0.0f),
			m_count0(0), m_count1(0),
			m_intra_vert(IntraGraph::null_vertex())
		{}
		~Cell() {}

		unsigned int Id();
		unsigned int BrickId();
		unsigned long long GetKey();
		IntraVert GetIntraVert();
		void SetIntraVert(IntraVert intra_vert);
		void Set(pCell &cell);
		void Inc(size_t i, size_t j, size_t k, float value);
		void Inc(pCell &cell);
		void Inc();
		void IncExternal(float value);
		void AddVertex(pVertex &vertex);
		pwVertex GetVertex();
		unsigned int GetVertexId();

		//get
		FLIVR::Point &GetCenter();
		FLIVR::BBox &GetBox();
		unsigned int GetSizeUi();
		float GetSizeF();
		unsigned int GetExternalUi();
		float GetExternalF();
		//set
		void SetBrickId(unsigned int id);
		void SetCenter(const FLIVR::Point &center);
		void SetBox(const FLIVR::BBox &box);
		void SetSizeUi(unsigned int size_ui);
		void SetSizeF(float size_f);
		void SetExternalUi(unsigned int external_ui);
		void SetExternalF(float external_f);
		//count
		unsigned int GetCount0();
		unsigned int GetCount1();
		void SetCount0(unsigned int val);
		void SetCount1(unsigned int val);

	private:
		unsigned int m_id;
		unsigned int m_brick_id;
		FLIVR::Point m_center;
		FLIVR::BBox m_box;
		//size
		unsigned int m_size_ui;
		float m_size_f;
		//external size
		unsigned int m_external_ui;
		float m_external_f;
		//count
		unsigned int m_count0;
		unsigned int m_count1;
		IntraVert m_intra_vert;
		pwVertex m_vertex;//parent
	};

	inline unsigned int Cell::Id()
	{
		return m_id;
	}

	inline unsigned int Cell::BrickId()
	{
		return m_brick_id;
	}

	inline unsigned long long Cell::GetKey()
	{
		unsigned long long temp = m_brick_id;
		return (temp << 32) | m_id;
	}

	inline IntraVert Cell::GetIntraVert()
	{
		return m_intra_vert;
	}

	inline void Cell::SetIntraVert(IntraVert intra_vert)
	{
		m_intra_vert = intra_vert;
	}

	inline void Cell::Set(pCell &cell)
	{
		m_center = cell->GetCenter();
		m_size_ui = cell->GetSizeUi();
		m_size_f = cell->GetSizeF();
		m_box = cell->GetBox();
	}

	inline void Cell::Inc(size_t i, size_t j, size_t k, float value)
	{
		m_center = FLIVR::Point(
			(m_center*m_size_ui + FLIVR::Point(double(i),
			double(j), double(k))) / (m_size_ui + 1));
		m_size_ui++;
		m_size_f += value;
		FLIVR::Point p(i, j, k);
		m_box.extend(p);
	}

	inline void Cell::Inc(pCell &cell)
	{
		m_center = FLIVR::Point(
			(m_center * m_size_ui + cell->GetCenter() *
			cell->GetSizeUi()) / (m_size_ui +
			cell->GetSizeUi()));
		m_size_ui += cell->GetSizeUi();
		m_size_f += cell->GetSizeF();
		m_box.extend(cell->GetBox());
	}

	inline void Cell::Inc()
	{
		m_size_ui++;
	}

	inline void Cell::IncExternal(float value)
	{
		m_external_ui++;
		m_external_f += value;
	}

	inline void Cell::AddVertex(pVertex &vertex)
	{
		m_vertex = vertex;
	}

	inline pwVertex Cell::GetVertex()
	{
		return m_vertex;
	}

	inline FLIVR::Point &Cell::GetCenter()
	{
		return m_center;
	}

	inline FLIVR::BBox &Cell::GetBox()
	{
		return m_box;
	}

	inline unsigned int Cell::GetSizeUi()
	{
		return m_size_ui;
	}

	inline float Cell::GetSizeF()
	{
		return m_size_f;
	}

	inline unsigned int Cell::GetExternalUi()
	{
		return m_external_ui;
	}

	inline float Cell::GetExternalF()
	{
		return m_external_f;
	}

	inline void Cell::SetBrickId(unsigned int id)
	{
		m_brick_id = id;
	}

	inline void Cell::SetCenter(const FLIVR::Point &center)
	{
		m_center = center;
	}

	inline void Cell::SetBox(const FLIVR::BBox &box)
	{
		m_box = box;
	}

	inline void Cell::SetSizeUi(unsigned int size_ui)
	{
		m_size_ui = size_ui;
	}

	inline void Cell::SetSizeF(float size_f)
	{
		m_size_f = size_f;
	}

	inline void Cell::SetExternalUi(unsigned int external_ui)
	{
		m_external_ui = external_ui;
	}

	inline void Cell::SetExternalF(float external_f)
	{
		m_external_f = external_f;
	}

	//count
	inline unsigned int Cell::GetCount0()
	{
		return m_count0;
	}

	inline unsigned int Cell::GetCount1()
	{
		return m_count1;
	}

	inline void Cell::SetCount0(unsigned int val)
	{
		m_count0 = val;
	}

	inline void Cell::SetCount1(unsigned int val)
	{
		m_count1 = val;
	}

}//namespace FL

#endif//FL_Cell_h