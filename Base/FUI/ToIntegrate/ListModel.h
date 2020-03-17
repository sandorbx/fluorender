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
#ifndef _LISTMODEL_H_
#define _LISTMODEL_H_

#include <wx/dataview.h>
#include <Fui/InterfaceAgent.h>
#include <Scenegraph/Node.h>

namespace FUI
{
	class AgentFactory;
	class ListModel : public wxDataViewModel, public InterfaceAgent
	{
	public:
		ListModel();

		int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2,
			unsigned int column, bool ascending) const override;

		//model definition
		virtual unsigned int GetColumnCount() const override;

		virtual wxString GetColumnType(unsigned int col) const override;

		virtual void GetValue(wxVariant &variant,
			const wxDataViewItem &item, unsigned int col) const override;

		virtual bool SetValue(const wxVariant &variant,
			const wxDataViewItem &item, unsigned int col) override;

		virtual bool IsEnabled(const wxDataViewItem &item,
			unsigned int col) const override;

		virtual bool IsContainer(const wxDataViewItem &item) const override;

		virtual bool HasContainerColumns(const wxDataViewItem & item) const override;

		virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override;

		virtual unsigned int GetChildren(const wxDataViewItem &parent,
			wxDataViewItemArray &array) const override;

		virtual bool isSameKindAs(const Object* obj) const
		{
			return dynamic_cast<const ListModel*>(obj) != NULL;
		}

		virtual const char* className() const { return "ListModel"; }

		//interface agent functions
		virtual void setObject(FL::Node* root);
		virtual FL::Node* getObject()
		{
			return dynamic_cast<FL::Node*>(InterfaceAgent::getObject());
		}

		friend class AgentFactory;

	protected:
		void OnItemAdded(FL::Event& event);
		void OnItemRemoved(FL::Event& event);
	};
}

#endif//_LISTMODEL_H_