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
#ifndef _TREEPANEL_H_
#define _TREEPANEL_H_

#include <wx/wx.h>
#include <wx/dataview.h>
#include <Fui/TreeModel.h>

namespace FUI
{
	class TreePanel : public wxPanel
	{
	public:
		enum
		{
			ID_ToggleView = ID_TREE_PANEL2,
			ID_AddGroup,
			ID_AddMGroup,
			ID_RemoveData,
			ID_BrushAppend,
			ID_BrushDesel,
			ID_BrushDiffuse,
			ID_BrushErase,
			ID_BrushClear,
			ID_BrushCreate
		};

		TreePanel(wxWindow* frame,
			wxWindow* parent,
			wxWindowID id,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = 0,
			const wxString& name = "TreePanel");
		~TreePanel();

		void SetScenegraph(FL::Node* root);

		//seelction
		void UpdateSelection();
		wxString GetCurrentSel();
		void Select(wxString view, wxString name);

		//set the brush icon down
		void SelectBrush(int id);
		int GetBrushSelected();
		//control from outside
		void BrushAppend();
		void BrushDiffuse();
		void BrushDesel();
		void BrushClear();
		void BrushErase();
		void BrushCreate();
		void BrushSolid(bool state);

	private:
		wxWindow* m_frame;
		wxToolBar* m_toolbar;

		wxDataViewCtrl* m_tree_ctrl;
		wxObjectDataPtr<TreeModel> m_tree_model;

		DECLARE_EVENT_TABLE()
	};
}

#endif//_TREEPANEL_H_