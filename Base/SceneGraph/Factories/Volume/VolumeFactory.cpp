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

#include "VolumeFactory.hpp"
#include <Volume/VolumeGroup.hpp>

using namespace fluo;

VolumeFactory::VolumeFactory()
{
	m_name = "volume factory";
	default_object_name_ = "default volume";

	addValue("current", (VolumeData*)(0));//current volume data

	setValueChangedFunction(
		default_setting_filename_value_name_,
		std::bind(&VolumeFactory::OnSetDefault,
		this, std::placeholders::_1));
}

VolumeFactory::~VolumeFactory()
{

}

void VolumeFactory::createDefault()
{
	if (!getDefault())
	{
		VolumeData* vd = new VolumeData();
		vd->setName(default_object_name_);
		objects_.push_front(vd);

		//add default values here
		//output (2d) adjustments
		vd->addValue("gamma r", double(1));
		vd->addValue("gamma g", double(1));
		vd->addValue("gamma b", double(1));
		vd->addValue("brightness r", double(1));
		vd->addValue("brightness g", double(1));
		vd->addValue("brightness b", double(1));
		vd->addValue("equalize r", double(0));
		vd->addValue("equalize g", double(0));
		vd->addValue("equalize b", double(0));
		vd->addValue("sync r", bool(false));
		vd->addValue("sync g", bool(false));
		vd->addValue("sync b", bool(false));

		vd->addValue("bounds", FLTYPE::BBox());
		//clipping planes
		vd->addValue("clip planes", FLTYPE::PlaneSet(6));
		vd->addValue("clip bounds", FLTYPE::BBox());
		//save clip values individually
		//actual clipping planes are calculated after either
		//clip values or rotations are changed
		vd->addValue("clip x1", double(0));
		vd->addValue("clip x2", double(1));
		vd->addValue("clip y1", double(0));
		vd->addValue("clip y2", double(1));
		vd->addValue("clip z1", double(0));
		vd->addValue("clip z2", double(1));
		//clip distance (normalized)
		vd->addValue("clip dist x", double(1));
		vd->addValue("clip dist y", double(1));
		vd->addValue("clip dist z", double(1));
		//clip link
		vd->addValue("clip link x", bool(false));
		vd->addValue("clip link y", bool(false));
		vd->addValue("clip link z", bool(false));
		//clip rotation
		vd->addValue("clip rot x", double(0));
		vd->addValue("clip rot y", double(0));
		vd->addValue("clip rot z", double(0));
		//clip plane rendering
		vd->addValue("clip display", bool(false));
		vd->addValue("clip hold", bool(false));
		vd->addValue("clip mask", long(-1));
		vd->addValue("clip render mode", long(FLTYPE::PRMNormal));

		vd->addValue("data path", std::wstring());//path to original file
		vd->addValue("channel", long(0));//channel index of the original file
		vd->addValue("time", long(0));//time index of the original file

		//modes
		//blend mode
		vd->addValue("blend mode", long(0));//0: ignore; 1: layered; 2: depth; 3: composite
		//mip
		vd->addValue("mip mode", long(0));//0-normal; 1-MIP
		vd->addValue("overlay mode", long(0));//0-unset; 1-base layer; 2-white; 3-white mip
		//vd->addValue("stream mode", long(0));//0-normal; 1-MIP; 2-shading; 3-shadow, 4-mask
		vd->addValue("mask mode", long(0));//0-normal, 1-render with mask, 2-render with mask excluded,
											//3-random color with label, 4-random color with label+mask
		vd->addValue("use mask thresh", bool(false));// use mask threshold
		vd->addValue("mask thresh", double(0));//mask threshold
		vd->addValue("invert", bool(false));//invert intensity values

		//volume properties
		vd->addValue("soft thresh", double(0));//soft threshold
		vd->addValue("int scale", double(1));//scaling factor for intensity values
		vd->addValue("gm scale", double(1));//scaling factor for gradient magnitude values
		vd->addValue("max int", double(255));//max intensity value from integer reading
		vd->addValue("gamma 3d", double(1));
		vd->addValue("extract boundary", double(0));//previously called gm thresh
		vd->addValue("saturation", double(1));//previously called offset, low offset
		vd->addValue("low threshold", double(0));
		vd->addValue("high threshold", double(1));
		vd->addValue("alpha", double(1));
		vd->addValue("alpha enable", bool(true));
		vd->addValue("mat amb", double(1));//materials
		vd->addValue("mat diff", double(1));
		vd->addValue("mat spec", double(1));
		vd->addValue("mat shine", double(10));
		vd->addValue("noise redct", bool(false));//noise reduction
		vd->addValue("shading enable", bool(false));//shading
		vd->addValue("low shading", double(1));//low shading
		vd->addValue("high shading", double(10));//highg shading
		vd->addValue("shadow enable", bool(false));//shadow
		vd->addValue("shadow int", double(1));
		vd->addValue("sample rate", double(1));//sample rate
		//color
		vd->addValue("color", FLTYPE::Color(1.0));
		vd->addValue("hsv", FLTYPE::HSVColor(FLTYPE::Color(1.0)));
		vd->addValue("luminance", double(1.0));
		vd->addValue("sec color", FLTYPE::Color(1.0));//secondary color
		vd->addValue("sec color set", bool(false));
		vd->addValue("randomize color", bool(false));//set to change color

		//resolution
		vd->addValue("res x", long(0));
		vd->addValue("res y", long(0));
		vd->addValue("res z", long(0));
		vd->addValue("base res x", long(0));
		vd->addValue("base res y", long(0));
		vd->addValue("base res z", long(0));
		//scaling
		vd->addValue("scale x", double(1));
		vd->addValue("scale y", double(1));
		vd->addValue("scale z", double(1));
		//spacing
		vd->addValue("spc x", double(1));
		vd->addValue("spc y", double(1));
		vd->addValue("spc z", double(3));
		vd->addValue("spc from file", bool(false));//if spacing value are from original file, otherwise use default settings
		//added for multires data
		vd->addValue("base spc x", double(1));
		vd->addValue("base spc y", double(1));
		vd->addValue("base spc z", double(3));
		vd->addValue("spc scl x", double(1));
		vd->addValue("spc scl y", double(1));
		vd->addValue("spc scl z", double(1));

		//display control
		vd->addValue("bits", long(8));//bits
		vd->addValue("display", bool(true));
		vd->addValue("draw bounds", bool(false));
		vd->addValue("test wire", bool(false));

		//color map settings
		vd->addValue("colormap enable", bool(false));
		vd->addValue("colormap mode", long(0));
		vd->addValue("colormap type", long(0));
		vd->addValue("colormap low", double(0));
		vd->addValue("colormap high", double(1));
		vd->addValue("colormap proj", long(0));

		//texture ids will be removed later
		vd->addValue("2d mask id", (unsigned long)(0));//2d mask texture for segmentation
		//2d weight map for segmentation
		vd->addValue("2d weight1 id", (unsigned long)(0));//after tone mapping
		vd->addValue("2d weight2 id", (unsigned long)(0));//before tone mapping
		vd->addValue("2d dmap id", (unsigned long)(0));//2d depth map texture for rendering shadows

		//compression
		vd->addValue("compression", bool(false));

		//resize
		vd->addValue("resize", bool(false));
		vd->addValue("resize x", long(0));
		vd->addValue("resize y", long(0));
		vd->addValue("resize z", long(0));

		//brisk skipping
		vd->addValue("skip brick", bool(false));

		//shown in legend
		vd->addValue("legend", bool(true));

		//interpolate
		vd->addValue("interpolate", bool(true));

		//depth attenuation, also called fog previously
		vd->addValue("depth atten", bool(false));
		vd->addValue("da int", double(0.5));
		vd->addValue("da start", double(0));
		vd->addValue("da end", double(1));

		//valid brick number
		vd->addValue("brick num", long(0));

		//estimate threshold
		vd->addValue("estimate thresh", double(0));

		//parameters not in original class but passed to renderer
		vd->addValue("viewport", FLTYPE::GLint4());//viewport
		vd->addValue("clear color", FLTYPE::GLfloat4());//clear color
		vd->addValue("cur framebuffer", (unsigned long)(0));//current framebuffer

		//multires level
		vd->addValue("multires", bool(false));
		vd->addValue("level", long(0));
		vd->addValue("level num", long(1));

		//tex transform
		vd->addValue("tex transform", FLTYPE::Transform());

		//selected on the ui
		vd->addValue("selected", bool(false));
	}
}

#define ADD_BEFORE_EVENT(obj, name, funct) \
	obj->setValueChangingFunction(name, std::bind(&VolumeData::funct, obj, std::placeholders::_1))

#define ADD_AFTER_EVENT(obj, name, funct) \
	obj->setValueChangedFunction(name, std::bind(&VolumeData::funct, obj, std::placeholders::_1))

void VolumeFactory::setEventHandler(VolumeData* vd)
{
	//handle before events
	//ADD_BEFORE_EVENT(vd, "mip mode", OnMipModeChanging);

	//handle after events
	ADD_AFTER_EVENT(vd, "mip mode", OnMipModeChanged);
	ADD_AFTER_EVENT(vd, "overlay mode", OnOverlayModeChanged);
	ADD_AFTER_EVENT(vd, "viewport", OnViewportChanged);
	ADD_AFTER_EVENT(vd, "clear color", OnClearColorChanged);
	ADD_AFTER_EVENT(vd, "cur framebuffer", OnCurFramebufferChanged);
	ADD_AFTER_EVENT(vd, "compression", OnCompressionChanged);
	ADD_AFTER_EVENT(vd, "invert", OnInvertChanged);
	ADD_AFTER_EVENT(vd, "mask mode", OnMaskModeChanged);
	ADD_AFTER_EVENT(vd, "noise redct", OnNoiseRedctChanged);
	ADD_AFTER_EVENT(vd, "2d dmap id", On2dDmapIdChanged);
	ADD_AFTER_EVENT(vd, "gamma 3d", OnGamma3dChanged);
	ADD_AFTER_EVENT(vd, "extract boundary", OnExtractBoundaryChanged);
	ADD_AFTER_EVENT(vd, "saturation", OnSaturationChanged);
	ADD_AFTER_EVENT(vd, "low threshold", OnLowThresholdChanged);
	ADD_AFTER_EVENT(vd, "high threshold", OnHighThresholdChanged);
	ADD_AFTER_EVENT(vd, "color", OnColorChanged);
	ADD_AFTER_EVENT(vd, "sec color", OnSecColorChanged);
	ADD_AFTER_EVENT(vd, "sec color set", OnSecColorSetChanged);
	ADD_AFTER_EVENT(vd, "luminance", OnLuminanceChanged);
	ADD_AFTER_EVENT(vd, "alpha", OnAlphaChanged);
	ADD_AFTER_EVENT(vd, "alpha enable", OnAlphaEnableChanged);
	ADD_AFTER_EVENT(vd, "mask thresh", OnMaskThreshChanged);
	ADD_AFTER_EVENT(vd, "use mask thresh", OnUseMaskThreshChanged);
	ADD_AFTER_EVENT(vd, "shading enable", OnShadingEnableChanged);
	ADD_AFTER_EVENT(vd, "mat amb", OnMaterialChanged);
	ADD_AFTER_EVENT(vd, "low shading", OnLowShadingChanged);
	ADD_AFTER_EVENT(vd, "mat diff", OnMaterialChanged);
	ADD_AFTER_EVENT(vd, "mat spec", OnMaterialChanged);
	ADD_AFTER_EVENT(vd, "mat shine", OnMaterialChanged);
	ADD_AFTER_EVENT(vd, "high shading", OnHighShadingChanged);
	ADD_AFTER_EVENT(vd, "sample rate", OnSampleRateChanged);
	ADD_AFTER_EVENT(vd, "colormap mode", OnColormapModeChanged);
	ADD_AFTER_EVENT(vd, "colormap low", OnColormapValueChanged);
	ADD_AFTER_EVENT(vd, "colormap high", OnColormapValueChanged);
	ADD_AFTER_EVENT(vd, "colormap type", OnColormapTypeChanged);
	ADD_AFTER_EVENT(vd, "colormap proj", OnColormapProjChanged);
	ADD_AFTER_EVENT(vd, "spc x", OnSpacingChanged);
	ADD_AFTER_EVENT(vd, "spc y", OnSpacingChanged);
	ADD_AFTER_EVENT(vd, "spc z", OnSpacingChanged);
	ADD_AFTER_EVENT(vd, "base spc x", OnBaseSpacingChanged);
	ADD_AFTER_EVENT(vd, "base spc y", OnBaseSpacingChanged);
	ADD_AFTER_EVENT(vd, "base spc z", OnBaseSpacingChanged);
	ADD_AFTER_EVENT(vd, "spc scl x", OnSpacingScaleChanged);
	ADD_AFTER_EVENT(vd, "spc scl y", OnSpacingScaleChanged);
	ADD_AFTER_EVENT(vd, "spc scl z", OnSpacingScaleChanged);
	ADD_AFTER_EVENT(vd, "level", OnLevelChanged);
	ADD_AFTER_EVENT(vd, "display", OnDisplayChanged);
	ADD_AFTER_EVENT(vd, "interpolate", OnInterpolateChanged);
	ADD_AFTER_EVENT(vd, "depth atten", OnDepthAttenChanged);
	ADD_AFTER_EVENT(vd, "skip brick", OnSkipBrickChanged);
	ADD_AFTER_EVENT(vd, "clip planes", OnClipPlanesChanged);
	ADD_AFTER_EVENT(vd, "sync r", OnSyncOutputChannels);
	ADD_AFTER_EVENT(vd, "sync g", OnSyncOutputChannels);
	ADD_AFTER_EVENT(vd, "sync b", OnSyncOutputChannels);
	ADD_AFTER_EVENT(vd, "clip x1", OnClipX1Changed);
	ADD_AFTER_EVENT(vd, "clip x2", OnClipX2Changed);
	ADD_AFTER_EVENT(vd, "clip y1", OnClipY1Changed);
	ADD_AFTER_EVENT(vd, "clip y2", OnClipY2Changed);
	ADD_AFTER_EVENT(vd, "clip z1", OnClipZ1Changed);
	ADD_AFTER_EVENT(vd, "clip z2", OnClipZ2Changed);
	ADD_AFTER_EVENT(vd, "clip rot x", OnClipRot);
	ADD_AFTER_EVENT(vd, "clip rot y", OnClipRot);
	ADD_AFTER_EVENT(vd, "clip rot z", OnClipRot);
	ADD_AFTER_EVENT(vd, "randomize color", OnRandomizeColor);
}

VolumeData* VolumeFactory::build(VolumeData* vd)
{
	unsigned int default_id = 0;
	return clone(default_id);
}

VolumeData* VolumeFactory::clone(VolumeData* vd)
{
	if (!vd)
		return 0;

	incCounter();

	VolumeData* new_vd = dynamic_cast<VolumeData*>(
		vd->clone(CopyOp::DEEP_COPY_ALL));
	if (!new_vd)
		return 0;
	new_vd->setId(global_id_);
	std::string name = "volume" + std::to_string(local_id_);
	new_vd->setName(name);

	objects_.push_front(new_vd);
	setValue("current", new_vd);

	setEventHandler(new_vd);

	//notify observers
	Event event;
	event.init(Event::EVENT_NODE_ADDED,
		this, new_vd);
	notifyObservers(event);

	return new_vd;
}

VolumeData* VolumeFactory::clone(const unsigned int id)
{
	Object* object = find(id);
	if (object)
	{
		VolumeData* vd = dynamic_cast<VolumeData*>(object);
		if (vd)
			return clone(vd);
	}
	return 0;
}

VolumeGroup* VolumeFactory::buildGroup(VolumeData* vd)
{
	VolumeGroup* group;
	if (vd)
		group = new VolumeGroup(*vd, CopyOp::DEEP_COPY_ALL);
	else
		group = new VolumeGroup(*getDefault(), CopyOp::DEEP_COPY_ALL);
	if (group)
	{
		group->setName("Group");
		group->setValueChangedFunction(
			"randomize color", std::bind(
				&VolumeGroup::OnRandomizeColor,
				group, std::placeholders::_1));
	}
	return group;
}

void VolumeFactory::OnSetDefault(Event& event)
{
	if (!readDefault())
		createDefault();
}