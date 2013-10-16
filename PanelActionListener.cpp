#include "PanelActionListener.h"
#include "GraphFitting.h"

extern CGraphFitting *pGf;
CPanelActionListener::CPanelActionListener(void)
{
	curLine=-1;
}

CPanelActionListener::~CPanelActionListener(void)
{
}
void CPanelActionListener::Accept(osgViewer::Viewer * _viewer,osg::Switch* _switch,osg::Switch* pGrp)
{
	pViewer=_viewer;
	pViewer->addEventHandler(this);
	pSwitch=_switch;
	pDetailGrp=pGrp;
}
bool CPanelActionListener::handle(const GUIEventAdapter& ea,GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
	case GUIEventAdapter::KEYDOWN:
		KeyDownEvent(ea,aa);
		break;
	case GUIEventAdapter::PUSH:
		//MousePressEvent(ea,aa);
		break;
	case GUIEventAdapter::DRAG:
		//MouseDragEvent(ea,aa);
		break;
	case GUIEventAdapter::DOUBLECLICK:
		MouseClickEvent(ea,aa);
		break;
	case GUIEventAdapter::MOVE:
		//MouseMoveEvent(ea,aa);
		break;
	//case GUIEventAdapter::FRAME:
		//displayEachLine();
		//break;
	default:
		break;
	}
	return false;
}
void CPanelActionListener::MousePressEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa)
{
}	
void CPanelActionListener::MouseMoveEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa)
{
}
void CPanelActionListener::MouseClickEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa)
{
	if (ea.getButton()==GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if(pViewer->computeIntersections(ea.getX(),ea.getY(),intersections))
		{
			for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();hitr!=intersections.end();++hitr)
			{
				osg::Node* node = hitr->nodePath.empty() ? NULL : hitr->nodePath.back();
				if(node->getNumDescriptions()>0 && node->getDescription(0)=="3DLINE")//单击了广告牌
				{
					curLine=atoi(node->getName().c_str());
					//pSwitch->setSingleChildOn(curLine);
					pSwitch->setAllChildrenOff();
					pDetailGrp->removeChildren(1,pDetailGrp->getNumChildren());
					osg::Node* pLinePoints=pGf->getLine(curLine).create3DLinePoints(curLine);
					pDetailGrp->addChild(pLinePoints);
					pDetailGrp->setSingleChildOn(1);
					//printf("CLICK LINE %s, support points=%d\n",node->getName().c_str(),0);
					break;
				}
			}
		}
	}
}
void CPanelActionListener::MouseDragEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa)
{
}
void CPanelActionListener::KeyDownEvent(const GUIEventAdapter& ea,GUIActionAdapter& aa)
{
	switch(ea.getKey())
	{
	case 'n':
	case 'N':
		pSwitch->setAllChildrenOn();
		pDetailGrp->setSingleChildOn(0);
		break;
	case 'm':
	case 'M':
		pSwitch->setAllChildrenOff();
		pDetailGrp->setSingleChildOn(0);
		break;
	}
}
