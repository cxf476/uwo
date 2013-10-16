#pragma once
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osg/Camera>
#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osg/Notify>

// Derive a class from NodeCallback to manipulate a
//   MatrixTransform object's matrix.
class LineCallBack : public osg::NodeCallback
{
public:
    LineCallBack():update(false){ pGrp=new osg::Group; }

    virtual void operator()( osg::Node* node,
            osg::NodeVisitor* nv )
    {
        // Normally, check to make sure we have an update
        //   visitor, not necessary in this simple example.
        osg::Switch* lineGrp = dynamic_cast<osg::Switch*>( node );
		if(lineGrp!=NULL)
		{
			if(update){
				lineGrp->removeChildren(0,lineGrp->getNumChildren());
				for(int i=0;i<pGrp->getNumChildren();++i)
					lineGrp->addChild(pGrp->getChild(i));
				pGrp->removeChildren(0,pGrp->getNumChildren());
				update=false;
			}
			//printf("%s\n",lineGrp->getName().c_str());
			traverse( node, nv );
		}
    }
	void addLine(osg::Node* pLine)
	{
		pGrp->addChild(pLine);
	}
	void setState(bool state){
		update=state;
	}
	bool getState(){	
		return update;	
	}
private:
	bool update;
	osg::ref_ptr<osg::Group> pGrp;
};
