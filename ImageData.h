#pragma once
#include "common.h"
#include "image3D.h"
class CImageData
{
friend class MyLine3D;
public:
	CImageData(const char* imagefile,const char* vesselfile, int sx,int sy,int sz);
	virtual ~CImageData(void);

	void saveData();
	bool isVessel(int x,int y,int z) const
	{
		return pVessel3D->get(x,y,z).data[0]>0.2;
		//return pImage3D->get(x,y,z)>3500;
	}
	bool isVessel(int id) const
	{
		return pVessel3D->get(id).data[0]>0.2;
		//return pImage3D->get(id)>3500;
	}
	Vessel getVessel(int x,int y ,int z) const
	{
		return pVessel3D->get(x,y,z);
	}
	short getIntensity(int x,int y ,int z) const
	{
		return pImage3D->get(x,y,z);
	}
	int getRealPos(int x,int y,int z)const
	{
		return pRealPos3D->get(x,y,z);
	}
	int getW(){return pImage3D->getX(); }
	int getH(){return pImage3D->getY(); }
	int getS(){return pImage3D->getZ(); }
///////////////////3D display//////////////////////////
	osg::Node* createMesh3D();
//////////////////////////////////////////////////////
public:
	Image3D<short>* pImage3D;
	Image3D<int>* pRealPos3D;
	Image3D<Vessel>* pVessel3D;
	int foreNum;
};

