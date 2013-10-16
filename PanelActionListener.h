#pragma once
#include "common.h"
#include "ImageData.h"
#include "BHTimer.h"
using namespace osgGA;
class CPanelActionListener :public osgGA::GUIEventHandler
{
public:
	CPanelActionListener(void);
	virtual ~CPanelActionListener(void);

	void Accept(osgViewer::Viewer * _viewer,osg::Switch* _switch,osg::Switch* pGrp);
//////////////////////////////////////////////////////////////////////////
public:
	virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& aa);
protected:
	void KeyDownEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa);
	void MouseClickEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa);
	void MousePressEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa);
	void MouseDragEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa);
	void MouseMoveEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa);
	//////////////////////////////////////////////////////////////////////////
protected:
	osgViewer::Viewer *pViewer;
	int curLine;
	osg::Switch* pSwitch;
	osg::Switch* pDetailGrp;
};