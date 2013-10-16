#include "ImageData.h"
#include "MyLine3D.h"

int W=0,H=0,S=0;
CImageData::CImageData(const char* imagefile,const char* vesselfile, int sx,int sy,int sz)
{
	pImage3D=new Image3D<short>(sx,sy,sz);
	pImage3D->loadImage(imagefile);

	pVessel3D=new Image3D<Vessel>(sx,sy,sz);
	pVessel3D->loadImage(vesselfile);

	pRealPos3D=new Image3D<int>(sx,sy,sz);

	W=pImage3D->getX();
	H=pImage3D->getY();
	S=pImage3D->getZ();
	//MyLine3D::setDimension(*this);

	foreNum=0;
	for(int z=0;z<getS();++z)
		for(int y=0;y<getH();++y)
			for(int x=0;x<getW();++x)
				pRealPos3D->get(x,y,z) =isVessel(x,y,z)?foreNum++:-1;
}


CImageData::~CImageData(void)
{
	if(pImage3D)	delete pImage3D;
	if(pVessel3D)	delete pVessel3D;
	if(pRealPos3D)	delete pRealPos3D;
}
osg::Node* CImageData::createMesh3D()
{
	osg::Geode* geode=new osg::Geode;
	osg::Geometry* geom=new osg::Geometry;
	osg::Vec3Array* coords = new osg::Vec3Array;
	osg::Vec3Array* colors=new osg::Vec3Array;

	int x1=0,y1=0,z1=0;
	int x2=pImage3D->getX();
	int y2=pImage3D->getY();
	int z2=pImage3D->getZ();

	for(int z=z1;z<z2;++z)
		for(int y=y1;y<y2;++y)
			for(int x=x1;x<x2;++x)
			{
				if(!isVessel(x,y,z))	continue;
				coords->push_back(osg::Vec3f(x,y,z));
				colors->push_back(osg::Vec3f(1,1,1));
			}
	printf("ALl Points=%d, Visible size=%d\n",x2*y2*z2,coords->size());
	geom->setVertexArray(coords);

	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,coords->size()));

	geode->addDrawable(geom);

	osg::Group* grp=new osg::Group;
	grp->addChild(geode);
	osg::Vec3d color(0,0,1);
	grp->addChild( create3DLine(osg::Vec3d(x1,y1,z1),osg::Vec3d(x2,y1,z1),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x1,y1,z1),osg::Vec3d(x1,y2,z1),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x1,y1,z1),osg::Vec3d(x1,y1,z2),color,4));

	grp->addChild( create3DLine(osg::Vec3d(x2,y2,z2),osg::Vec3d(x2,y2,z1),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x2,y2,z2),osg::Vec3d(x2,y1,z2),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x2,y2,z2),osg::Vec3d(x1,y2,z2),color,4));

	grp->addChild( create3DLine(osg::Vec3d(x2,y1,z2),osg::Vec3d(x1,y1,z2),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x2,y1,z2),osg::Vec3d(x2,y1,z1),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x1,y2,z1),osg::Vec3d(x2,y2,z1),color,4));
	grp->addChild( create3DLine(osg::Vec3d(x1,y2,z1),osg::Vec3d(x1,y2,z2),color,4));

	return grp;
}


