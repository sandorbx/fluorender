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

#include <Fui/MeshPropAgent.h>
#include <Fui/MeshPropPanel.h>
#include <wx/valnum.h>

using namespace FUI;

MeshPropAgent::MeshPropAgent(MeshPropPanel &panel) :
	InterfaceAgent(),
	panel_(panel)
{
}

void MeshPropAgent::setObject(FL::MeshData* obj)
{
	InterfaceAgent::setObject(obj);
}

FL::MeshData* MeshPropAgent::getObject()
{
	return dynamic_cast<FL::MeshData*>(InterfaceAgent::getObject());
}

void MeshPropAgent::UpdateAllSettings()
{
	wxString str;
	FLTYPE::Color amb, diff, spec;
	double shine, alpha;
	//m_md->GetMaterial(amb, diff, spec, shine, alpha);
	getValue("mat amb", amb);
	getValue("color", diff);
	getValue("mat spec", spec);
	getValue("mat shine", shine);
	getValue("alpha", alpha);

	wxColor c;
	c = wxColor(diff.r()*255, diff.g()*255, diff.b()*255);
	panel_.m_diff_picker->SetColour(c);
	c = wxColor(spec.r()*255, spec.g()*255, spec.b()*255);
	panel_.m_spec_picker->SetColour(c);

	//lighting
	bool light_enable;
	getValue("light enable", light_enable);
	panel_.m_light_chk->SetValue(light_enable);
	//shine
	panel_.m_shine_sldr->SetValue(int(shine));
	str = wxString::Format("%.0f", shine);
	panel_.m_shine_text->ChangeValue(str);
	//alpha
	panel_.m_alpha_sldr->SetValue(int(alpha*255));
	str = wxString::Format("%.2f", alpha);
	panel_.m_alpha_text->ChangeValue(str);
	//scaling
	double sx, sy, sz;
	//m_md->GetScaling(sx, sy, sz);
	getValue("scale x", sx);
	getValue("scale y", sy);
	getValue("scale z", sz);
	panel_.m_scale_sldr->SetValue(int(sx*100.0+0.5));
	str = wxString::Format("%.2f", sx);
	panel_.m_scale_text->ChangeValue(str);
	//shadow
	double shadow_int;
	bool shadow_enable;
	getValue("shadow enable", shadow_enable);
	getValue("shadow int", shadow_int);
	panel_.m_shadow_chk->SetValue(shadow_enable);
	panel_.m_shadow_sldr->SetValue(int(shadow_int*100.0+0.5));
	str = wxString::Format("%.2f", shadow_int);
	panel_.m_shadow_text->ChangeValue(str);
	//size limiter
	bool limit_enable;
	long limit;
	getValue("limit enable", limit_enable);
	getValue("limit", limit);
	panel_.m_size_chk->SetValue(limit_enable);
	panel_.m_size_sldr->SetValue(limit);
	panel_.m_size_text->SetValue(wxString::Format("%d", limit));
}