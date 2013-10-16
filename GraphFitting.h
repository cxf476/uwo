#pragma once
#include "MyLine3D.h"
#include "ImageData.h"
#include "graphcut/GCoptimization.h"

#define USE_SMOOTHCOST
#define MAX_LABELS 100000
const int MAX_LINES=400;
const int INFINIT=200000;
const int LABELCOST=5000;
const int SMOOTHCOST=120;
//ROI15  1500,50
//ROI16P 5000,120
class CGraphFitting
{
public:
	CGraphFitting(CImageData*);
	virtual ~CGraphFitting(void);

	long long fastFitting();
	int getValidNum();
	void fitLine(int lineID,GCoptimizationGeneralGraph *gc);
	MyLine3D& getLine(int pos) {return models[pos];}
	void saveModels();
	void saveVoxels();
	void saveLineVoxel();
	bool adjustModels();
	void splitModels();
	void mergeModels();
	void setModel(const MyLine3D& line,int pos){  models[pos]=line;models[pos].setValid(true); models[pos].realPos=pos; }
	void appendModel(const MyLine3D& line);
	void removeModel(int pos){ models[pos].setValid(false); }
	void setLineSplit(bool split){splitLines=split;}
	void keepSupVoxel(bool keep){keepVoxels=keep;}
	void clearVoxels(){
		for(int i=0;i<MAX_LABELS;++i)
			getLine(i).supportPoints.clear();
	}
	bool inRange(const Voxel& vo)
	{
		return vo.x>=0&&vo.x<W&&vo.y>=0&&vo.y<H&&vo.s>=0&&vo.s<S;
	}
	void resetLineGoodness();
	void compFittingResult();
	void generateCandidates();
private:
	MyLine3D CGraphFitting::combineLines(const MyLine3D& mLine,const MyLine3D& nLine);
private:
	MyLine3D* models;//[MAX_LABELS];
	CImageData* pImg3D;
	int * pLabels;
	bool splitLines;
	bool keepVoxels;
};

