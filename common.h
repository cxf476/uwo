#ifndef COMMON_H
#define COMMON_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include <osg/Config>
#include <iostream>
#include <windows.h>
#include <GL/glut.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/Optimizer>
#include <osg/LineWidth>

typedef std::string String;
const int INFTY=1000000;
const double ANGLE=osg::DegreesToRadians(91.0);
enum Label {NONE=128, OBJ=255, BKG=0};

extern int W,H,S;//golobal 
static float myround(float m)
{
	float num=abs(m);
	int nn=num;
	if(num-nn>0.4)	nn++;
	return m>0?nn:-nn;
}
template<typename T>
static void  on_mouse(int event,int x,int y,int flags,void *param)
{
	cv::Mat* pMat=(cv::Mat*)param;
	if (event==CV_EVENT_LBUTTONDOWN )
	{
		int val=pMat->at<T>(cv::Point(x,y));
		printf("(%d %d)=%d\n",x,y,val);
	}
}
struct Vessel
{
	float data[5];// x,y,z,vessness,size
};
struct Voxel
{
	Voxel(int _x,int _y,int _s)
	{
		x=_x;y=_y;s=_s;
	}
	double compDist(const Voxel& vl)
	{
		return sqrt((x-vl.x)*(x-vl.x)+(y-vl.y)*(y-vl.y)+(s-vl.s)*(s-vl.s)+0.0);
	}
	int x,y,s;
};
namespace{
	double getAngle(const cv::Point3f& v21,const cv::Point3f& v23)//get angle between v21 and v23
	{
		double dot=v21.dot(v23);
		double d21=sqrt(v21.dot(v21));
		double d23=sqrt(v23.dot(v23));
		return dot/(d21*d23);
	}
	osg::Group* createLight(osg::Group* _root)
	{	
		osg::Group* lightGroup=new osg::Group;
		osg::StateSet* rootSet=_root->getOrCreateStateSet();
		float cutof=30,expo=0;
		float ambient=0.330f,diffuse=0.770f,specular=0.770f;
		osg::PositionAttitudeTransform* pPATLight=new osg::PositionAttitudeTransform;
		{
			osg::Light* headLight=new osg::Light();//设置前上方的光源,改为第一区间光
			headLight->setLightNum(0);
			headLight->setPosition(osg::Vec4(0.f,0.f,0.f,1.f));
			headLight->setAmbient(osg::Vec4(ambient,ambient,ambient,1.0f));//设置环境光
			headLight->setDiffuse(osg::Vec4(diffuse,diffuse,diffuse,1.0f));//设置漫射光成分
			headLight->setSpecular(osg::Vec4(specular,specular,specular,1.0));//设置镜面光成分
			headLight->setDirection(-osg::Z_AXIS);
			headLight->setSpotCutoff(cutof);
			headLight->setSpotExponent(expo);
			osg::LightSource* lightSource=new osg::LightSource();
			lightSource->setLight(headLight);
			lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
			lightSource->setStateSetModes(*rootSet,osg::StateAttribute::ON);
			pPATLight->addChild(lightSource);
			lightGroup->addChild(pPATLight);
		}
		{
			float ambient=0.0f,diffuse=0.67f,specular=0.770f;
			osg::Light* headLight=new osg::Light();//设置前上方的光源,改为第一区间光
			headLight->setLightNum(1);
			headLight->setPosition(osg::Vec4(0.f,0.f,1.f,0.f));
			headLight->setAmbient(osg::Vec4(ambient,ambient,ambient,1.0f));//设置环境光
			headLight->setDiffuse(osg::Vec4(diffuse,diffuse,diffuse,1.0f));
			osg::LightSource* lightSource=new osg::LightSource();
			lightSource->setLight(headLight);
			lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
			lightSource->setStateSetModes(*rootSet,osg::StateAttribute::ON);
			lightGroup->addChild(lightSource);
		}
		return lightGroup;
	}
	osg::Node* create3DLine(const osg::Vec3d& s1,const osg::Vec3d& s2,const osg::Vec3d& color,float width=2.0)
	{
		osg::Geode* geode=new osg::Geode;
		osg::Geometry* geom=new osg::Geometry;

		osg::Vec3Array* coords = new osg::Vec3Array;
		coords->push_back(s1);
		coords->push_back(s2);
		geom->setVertexArray(coords);

		osg::Vec3Array* norms = new osg::Vec3Array(1);
		(*norms)[0] = osg::Z_AXIS;

		osg::Vec3Array* colors=new osg::Vec3Array;
		colors->push_back(color);//(osg::Vec3(1.0,0.0,0.0));
		geom->setColorArray(colors);
		geom->setColorBinding(osg::Geometry::BIND_OVERALL);

		geom->setNormalArray(norms);
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,coords->size()));
		geode->addDrawable(geom);

		osg::StateSet* state=geode->getOrCreateStateSet();
	
		static osg::ref_ptr<osg::StateSet> m_LineSet;
		if (!m_LineSet.valid())	m_LineSet=new osg::StateSet;

		osg::LineWidth* lineWidth=new osg::LineWidth;
		lineWidth->setWidth(width);
		m_LineSet->setAttribute(lineWidth);

		geode->setStateSet(m_LineSet.get());
		return geode;
	}
	 osg::Node* createFace(const osg::Vec3d& v1,const osg::Vec3d& v2,const osg::Vec3d& v3,const osg::Vec3d& v4,const osg::Vec3d& color,const char* name)
	{
		osg::Geode* geode=new osg::Geode;
		osg::Geometry* geom=new osg::Geometry;

		osg::Vec3Array* coords = new osg::Vec3Array;
		coords->push_back(v1);
		coords->push_back(v2);
		coords->push_back(v3);
		coords->push_back(v4);
		geom->setVertexArray(coords);

		osg::Vec3Array* norms = new osg::Vec3Array(1);
		osg::Vec3f norm(v1.x()-v2.x(),v1.y()-v2.y(),0.0);
		norm.normalize();
		(*norms)[0]= norm;
		//	(*norms)[0] = osg::Z_AXIS;

		osg::Vec4Array* colors=new osg::Vec4Array;
		colors->push_back(osg::Vec4d(color,0));
		geom->setColorArray(colors);
		geom->setColorBinding(osg::Geometry::BIND_OVERALL);

		geom->setNormalArray(norms);
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coords->size()));

		geode->addDrawable(geom);
		geode->addDescription("3DLINE");
		geode->setName(name);

		osg::StateSet* stateset = geom->getOrCreateStateSet();
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		return geode;
	}
}
#endif
