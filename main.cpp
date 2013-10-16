#include <stdio.h>
#include<iostream>
#include<process.h>
#include "ImageData.h"
#include "LineCallBack.h"
#include "BHTimer.h"
#include "MyLine3D.h"
#include "GraphFitting.h"
#include "PanelActionListener.h"

CImageData* pImg3D=NULL;
CGraphFitting *pGf=NULL;

void showIn3D(LineCallBack* pLCB,CGraphFitting& gf)
{
	int numLines=gf.getValidNum()-1;
	for(int i=0;i<numLines;++i)
	{
		MyLine3D& mLine=gf.getLine(i);
		if(mLine.goodness>0.1)
			pLCB->addLine(mLine.create3DBox(i));
	}
	pLCB->setState(true);
	while(pLCB->getState())
	{
		//printf("Sleep for 1 Second\n");
		Sleep(1000);
	}
}
void optimize(LineCallBack* pLCB,CGraphFitting& gf)
{
	gf.keepSupVoxel(true);
	gf.setLineSplit(false);
	long long prev=-1;
	for(int k=0;k<15;++k)
	{
		long long energy=gf.fastFitting();
		gf.adjustModels();
		showIn3D(pLCB,gf);
		if(energy>0)
		{
			if(energy!=prev)
				prev=energy;
			else
			{
				printf("Converged after %d interations ",k);
				std::cout<<energy<<std::endl;

				break;
			}
		}
		std::cout<<energy<<"   ";
	}
	gf.keepSupVoxel(true);
	gf.fastFitting();
	gf.adjustModels();
	gf.saveLineVoxel();
	gf.saveModels();
	gf.saveVoxels();
	return;
//	printf("Not Converged after 1000 interations\n");
}
void threadGC(void* param)
{
	BHTimer timer;
	LineCallBack* pLCB=(LineCallBack*)param;
	CGraphFitting gf(pImg3D);
	pGf=&gf;
	showIn3D(pLCB,gf);

	while(true)
	{
		timer.start();
		gf.resetLineGoodness();
		optimize(pLCB,gf);
		showIn3D(pLCB,gf);
		gf.compFittingResult();
		printf("-----------GC Time= %.3f---------------\n",timer.pause()/1000.0);
		system("pause");
		showIn3D(pLCB,gf);
		system("pause");
		gf.generateCandidates();
		showIn3D(pLCB,gf);
		system("pause");
	}
	delete pImg3D;
}

int main()
{
	srand((unsigned int)time(0));

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x =  100;
	traits->y = 100;
	traits->width = 600;
	traits->height = 480;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
    //osg::observer_ptr<osgViewer::GraphicsWindow> window = 
	viewer->setUpViewerAsEmbeddedInWindow(100,100,800,600);
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    viewer->addEventHandler(new osgViewer::StatsHandler);

	osg::Camera* camera = viewer->getCamera();
	camera->setGraphicsContext(gc.get());
	camera->setViewport(new osg::Viewport(0,0, traits->width, traits->height));
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);

    // create the view of the scene.
	osg::Group* pRoot=new osg::Group;
	pRoot->addChild(createLight(pRoot));
	osg::ClearNode* backdrop = new osg::ClearNode;
    backdrop->setClearColor(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
    pRoot->addChild(backdrop);

	osg::Switch* pLineGrp=new osg::Switch;
	pLineGrp->setName("LineGroup");
	LineCallBack* pLCB=new LineCallBack;
	pLineGrp->setUpdateCallback(pLCB);
	pRoot->addChild(pLineGrp);

	osg::Switch *pGrp=new osg::Switch;
	pRoot->addChild(pGrp);
//	pImg3D=new CImageData("roi15.partial.data","roi15.partial.float5.vesselness",88,67,70);//"roi15.partial.edge_tracing.vesselness");
//	pImg3D=new CImageData("D:/data/roi16.partial.data","D:/data/roi16.partial.float5.vesselness",151,84,151);
	pImg3D=new CImageData("D:/data/roi16.partial.partial.data","D:/data/roi16.partial.partial.float5.vesselness",111,44,111);
	osg::Node* pMesh=pImg3D->createMesh3D();
	pGrp->addChild(pMesh);

	_beginthread(threadGC,0,pLCB);
	viewer->setSceneData(pRoot);
    
	CPanelActionListener *pAL=new CPanelActionListener;
	pAL->Accept(viewer.get(),pLineGrp,pGrp);

	viewer->realize();
	return viewer->run();
}

