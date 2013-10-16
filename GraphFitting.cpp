#include "GraphFitting.h"
CGraphFitting::CGraphFitting(CImageData* imgdata)
{
	pImg3D=imgdata;
	models=new MyLine3D[MAX_LABELS];
	printf("Begin Sample Line\n");
	for(int i=0;i<MAX_LINES;++i)
	{
		models[i].sampleLine(*pImg3D);
		models[i].realPos=i;
	}
	for(int i=MAX_LINES;i<MAX_LABELS;++i)
	{
		models[i].setValid(false);
		models[i].realPos=i;
	}
	pLabels=new int[pImg3D->getW()*pImg3D->getS()*pImg3D->getH()];
	printf("End Sample Line\n");
}

CGraphFitting::~CGraphFitting(void)
{
	delete []pLabels;
	delete []models;
}
int CGraphFitting::getValidNum()
{
	int num_labels=0;
	for(int i=0;i<MAX_LABELS;++i)
		if(models[i].isValid())
			++num_labels;
	return num_labels;
}
long long CGraphFitting::fastFitting()
{
	int num_pixels=pImg3D->foreNum+1;
	int num_labels=getValidNum();
	////////////////////////////////////////////////////////////
	int *data = new int[num_pixels*num_labels];
	int curIndex=0;

	int slice=pImg3D->getS();
	int width=pImg3D->getW();
	int height=pImg3D->getH();

	for(int z = 0;z<slice;++z)
		for(int y=0;y<height;++y)
			for(int x=0;x<width;++x)
			{
				bool vessel=pImg3D->isVessel(x,y,z);
				if(vessel)
				{
					for (int l=0; l < num_labels; l++ )
					{
						if(l==num_labels-1)
							data[curIndex*num_labels+l]=5000;//INFINIT;
						else
							data[curIndex*num_labels+l]=models[l].compEnergy(x,y,z,*pImg3D);
					}
					curIndex++;
				}
			}
	for (int l=0; l < num_labels; l++ )
	{
		if(l==num_labels-1)
			data[curIndex*num_labels+l]=0;
		else
			data[curIndex*num_labels+l]=INFINIT;
	}

	//////////////////////////////////////////////////////////////////
	int *label = new int[num_labels];
	for(int k=0;k<num_labels;++k)		label[k]=LABELCOST;
	/////////////////////////////////////////////////////////////////
	int *smooth = new int[num_labels*num_labels];
	memset(smooth,0,sizeof(int)*num_labels*num_labels);
	for( int i=0; i<num_labels; i++ ) 
		for ( int j=0; j<num_labels; j++ ) 
			smooth[ i*num_labels+j ] = smooth[ i+j*num_labels ] =SMOOTHCOST* int(i!=j);
	long long energy=0;
	try{
		GCoptimizationGeneralGraph *gc = new GCoptimizationGeneralGraph(num_pixels,num_labels);
		gc->setDataCost(data);
		gc->setLabelCost(label);

#ifdef USE_SMOOTHCOST
		gc->setSmoothCost(smooth);
		for(int z = 0;z<slice;++z)
			for(int y=0;y<height;++y)
				for(int x=0;x<width;++x)
				{
					if(!pImg3D->isVessel(x,y,z))	continue;

					int numHoles=0;
					//////////////////////////////////////////////////////////////////////////////////
					if(inRange(Voxel(x+1,y,z)))//right neighbour
					{
						if(pImg3D->isVessel(x+1,y,z))	
							gc->setNeighbors(pImg3D->getRealPos(x,y,z),pImg3D->getRealPos(x+1,y,z));
						else
							++numHoles;
					}
					//////////////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////
					if(inRange(Voxel(x,y+1,z)))//top neighbour
					{
						if(pImg3D->isVessel(x,y+1,z))	
							gc->setNeighbors(pImg3D->getRealPos(x,y,z),pImg3D->getRealPos(x,y+1,z));
						else
							++numHoles;
					}
					//////////////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////
					if(inRange(Voxel(x,y,z+1)))//front neighbour
					{
						if(pImg3D->isVessel(x,y,z+1))	
							gc->setNeighbors(pImg3D->getRealPos(x,y,z),pImg3D->getRealPos(x,y,z+1));
						else
							++numHoles;
					}
					//////////////////////////////////////////////////////////////////////////////////
					if(inRange(Voxel(x-1,y,z))&& !pImg3D->isVessel(x-1,y,z) )	++numHoles;//left hole
					if(inRange(Voxel(x,y-1,z))&& !pImg3D->isVessel(x,y-1,z) )	++numHoles;//down hole
					if(inRange(Voxel(x,y,z-1))&& !pImg3D->isVessel(x,y,z-1) )	++numHoles;//back hole

					if(numHoles>0)
						gc->setNeighbors(pImg3D->getRealPos(x,y,z),num_pixels-1,numHoles);
				}
#endif
		//printf("\nBefore optimization energy is %d",gc->compute_energy());
		//std::cout<<"\nBefore optimization energy is "<<gc->compute_energy();
		energy=gc->compute_energy();
		gc->expansion(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
		//printf("\nAfter optimization energy is %d",gc->compute_energy());
		//std::cout<<"\nAfter optimization energy is "<<gc->compute_energy();
		gc->compute_energy();

		for ( int  i = 0; i < num_pixels; i++ )
		{
			int tag = gc->whatLabel(i);
			models[tag].addSupport();
			pLabels[i]=tag;
			//if(result[i]!=num_labels-1)	printf("%d ",result[i]);
		}
		////////////////////////////////////////////////////////////////////////////////////
		for(int i=0;i<num_labels;++i)
		{
			int sp=models[i].getSupport();
			models[i].setValid(sp>0);
			if(i==num_labels-1)//last model ,must be valid
				models[i].setValid(true);
		}
		///////////////////////////////////////////////
		for(int i=0;i<num_labels-1;++i)
		{
			if(models[i].isValid())
			{
				fitLine(i,gc);
			}
		}
		delete gc;
	}
	catch (GCException e){
		e.Report();
	}

	delete [] smooth;
	delete []label;
	delete [] data;
	return energy;
}
void CGraphFitting::fitLine(int lineID,GCoptimizationGeneralGraph *gc)
{
	int slice=pImg3D->getS();
	int width=pImg3D->getW();
	int height=pImg3D->getH();

	MyLine3D& mline=models[lineID];
	std::vector<cv::Point3i> points; 
	int curp=0;
	for(int z = 0;z<slice;++z)
		for(int y=0;y<height;++y)
			for(int x=0;x<width;++x)
			{
				if(!pImg3D->isVessel(x,y,z))	continue;
				int type=gc->whatLabel(curp);
				if(type==lineID)
				{
					points.push_back(cv::Point3i(x,y,z));
				}
				++curp;
			}
	printf("Showing Line %d,Num=%d\n",lineID,points.size());
	mline.adjustLine(points,*pImg3D);
	if(splitLines)	mline.kmeanLines(points,*pImg3D,keepVoxels);//split lines by kmeans
	if(keepVoxels)  mline.supportPoints=points;//  store support points
	//mline.print();	
}
void CGraphFitting::saveLineVoxel()
{
	FILE* pFile=fopen("D:/c/visualize3D/linedata.txt","w");
	if(!pFile)	pFile=fopen("linedata.txt","w");

	int num=getValidNum()-1;
	fprintf(pFile,"%d\n",num);

	for(int i=0;i<num;++i)
	{
		Voxel vo=models[i].getStartPoint();
		fprintf(pFile,"%d %d %d ",vo.x,vo.y,vo.s);
		
		vo=models[i].getEndPoint();
		fprintf(pFile,"%d %d %d\n",vo.x,vo.y,vo.s);

		fprintf(pFile,"%d\n",models[i].supportPoints.size());
		for(int j=0;j<models[i].supportPoints.size();++j)
		{
			cv::Point3i vi=models[i].supportPoints[j];
			fprintf(pFile,"%d %d %d\n",vi.x,vi.y,vi.z);
		}
	}
	fclose(pFile);
}
void CGraphFitting::saveModels()
{
	FILE* pFile=fopen("D:/c/visualize3D/lines.txt","w");
	if(!pFile)	pFile=fopen("lines.txt","w");

	int num=getValidNum()-1;
	fprintf(pFile,"%d\n",num);

	for(int i=0;i<num;++i)
	{
		Voxel vo=models[i].getStartPoint();
		fprintf(pFile,"%d %d %d ",vo.x,vo.y,vo.s);
		
		vo=models[i].getEndPoint();
		fprintf(pFile,"%d %d %d\n",vo.x,vo.y,vo.s);
	}
	fclose(pFile);
}
void CGraphFitting::saveVoxels()
{
	FILE* pFile=fopen("D:/c/visualize3D/voxels.txt","w");
	if(!pFile)	pFile=fopen("voxels.txt","w");
	int num=W*H*S;
	fprintf(pFile,"%d\n",num);

	int labelNum=getValidNum();

	for(int z = 0;z<S;++z)
		for(int y=0;y<H;++y)
			for(int x=0;x<W;++x)
			{
				if(!pImg3D->isVessel(x,y,z))
					fprintf(pFile,"%d ",labelNum-1);
				else
				{
					int pos=pLabels[pImg3D->getRealPos(x,y,z)];
					fprintf(pFile,"%d ", models[pos].realPos);
				}
			}
	fclose(pFile);
}
bool CGraphFitting::adjustModels()
{
	//move all valid labels to the front
	////////////////////////////////////
	printf("Before Adjust %d valid lines\n",getValidNum());
	int i=0;
	while(models[i].isValid()) 
	{
		models[i].realPos=i;
		++i;
	}
	for(int j=i+1;j<MAX_LABELS;++j)
	{
		if(models[j].isValid())
		{
			models[j].realPos=models[i].realPos;//copy convinent
			models[i]=models[j];
			models[j].setValid(false);
			models[j].realPos=i++;
		}
	}
	for(int i=0;i<MAX_LABELS;++i)
	{
		models[i].clearSupport();
	}
	printf("After Adjust %d valid lines\n",getValidNum());
	return true;
}
void CGraphFitting::splitModels()
{
	CGraphFitting& gf=*this;

	int num=gf.getValidNum();
	int index=num-1;
	for(int i=0;i<num-1;++i)
	{
		MyLine3D& mLine=gf.getLine(i);
		for(int j=0;j<mLine.clusterLines.size();++j)
		{
			gf.setModel(mLine.clusterLines[j],index++);	
		}
		mLine.clusterLines.clear();
	}
	gf.getLine(index).setValid(true);//background model

	printf("Split from %d to %d **************",num,gf.getValidNum());
}
void CGraphFitting::appendModel(const MyLine3D& line)
{
	int num=getValidNum();
	setModel(line,num-1);	
	getLine(num).setValid(true);//background model move back
}
void CGraphFitting::mergeModels()
{
	printf("Start Merging\n");
	const int num=getValidNum();
	int index=num-1;
	for(int i=0;i<num-1;++i)
	{
		MyLine3D& mLine=getLine(i);
		double diff=99999;
		int curSel=i;
		//printf("for line %d ,there are ",i);
		for(int j=i+1;j<num-1;++j)
		{
			if(i==j)	continue;

			MyLine3D& nLine=getLine(j);
			if(mLine.isSimilar(nLine))
			{
				double cur=mLine.getSimilarity(nLine);
				if(diff>cur)	
				{
					diff=cur;
					curSel=j;
				}
			}
		}
		if(curSel!=i && diff<3)	
		{
			MyLine3D line = combineLines(mLine,getLine(curSel));
			if(line.isValid())
			{
				setModel(line,index++);	
			}
		}
	}
	getLine(index).setValid(true);//background model
	adjustModels();//readjust models
	printf("Merge from %d to %d lines\n",num,getValidNum());
}
MyLine3D CGraphFitting::combineLines(const MyLine3D& mLine,const MyLine3D& nLine)
{
	MyLine3D line;
	if(mLine.supportPoints.size()==0||nLine.supportPoints.size()==0)
	{
		line.setValid(false);
		return line;
	}
	std::vector<cv::Point3i> points=mLine.supportPoints;
	points.insert(points.end(),nLine.supportPoints.begin(),nLine.supportPoints.end());
	line=MyLine3D::getLineByPoints(points);
	line.adjustLine(points,*pImg3D);
	line.supportPoints=points;
	return line;
}
void CGraphFitting::resetLineGoodness()
{
	adjustModels();
	int numLines=getValidNum()-1;
	for(int i=0;i<numLines;++i)
		getLine(i).goodness=1.0;
}
void CGraphFitting::compFittingResult()
{
	int numLines=getValidNum()-1;
	for(int i=0;i<numLines;++i)
		getLine(i).tubePoints.clear();

	for(int z=0;z<S;++z)
		for(int y=0;y<H;++y)
			for(int x=0;x<W;++x)
			{
				for(int i=0;i<numLines;++i)
				{
					MyLine3D& line=getLine(i);
					if(line.inLineRange(x,y,z))
						line.tubePoints.push_back(cv::Point3i(x,y,z));
				}
			}
	/////////////////////////////////////////////////////////////////////////////
	for(int i=0;i<numLines;++i)
	{
		MyLine3D& line=getLine(i);
		std::vector<cv::Point3i> &  vt=line.tubePoints;
		double percent=line.supportPoints.size() /(vt.size()+0.0);
		printf("line %d ,percent=%.2f, foreground=%d, all=%d,radius=%.2f\n",i,percent,line.supportPoints.size(),vt.size(),line.mRadius);
		line.goodness=percent;
	}
}
void CGraphFitting::generateCandidates()
{
	double thres=0.25;
	int numLines=getValidNum()-1;
	for(int i=0;i<numLines;++i)
	{
		MyLine3D& line=getLine(i);
		if(line.goodness<thres)	
		{
			printf("%d line kmeans start\n",i);
			line.kmeanLines(line.supportPoints,*pImg3D,true);
			for(int j=0;j<line.clusterLines.size();++j)
				appendModel(line.clusterLines[j]);
			printf("%d line kmeans End\n",i);
		}
	}
	for(int i=0;i<numLines;++i)
	{
		MyLine3D& line=getLine(i);
		if(line.goodness<thres)	
			removeModel(i);
	}
	adjustModels();
}