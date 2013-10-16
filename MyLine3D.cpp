#include "MyLine3D.h"

//int MyLine3D::W=0;
//int MyLine3D::H=0;
//int MyLine3D::S=0;
MyLine3D::MyLine3D()
{
	xa=ya=za=0;
	xb=yb=zb=10;
	theta=1;
	mRadius=-1;
	goodness=1.0;
}
MyLine3D::~MyLine3D(void)
{
}
void MyLine3D::initCrossBox()
{
	osg::Vec3f tn(xa-xb,ya-yb,za-zb);
	osg::Vec3f tu(tn.y(),-tn.x(),0);
	osg::Vec3f tv(tn.x()*tn.z(),tn.y()*tn.z(),-(tn.x()*tn.x()+tn.y()*tn.y()) );
	tu.normalize();
	tv.normalize();
	nu=tu;
	nv=tv;
}
bool MyLine3D::sameDirect(const Voxel& va,const Voxel& vb,const CImageData& img3D)
{
	Vessel vla=img3D.getVessel(va.x,va.y,va.s);
	Vessel vlb=img3D.getVessel(vb.x,vb.y,vb.s);

	const double minVal=cos(osg::DegreesToRadians(10.0));
	double val=getAngle(cv::Point3f(vla.data[1],vla.data[2],vla.data[3] ),cv::Point3f(vlb.data[1],vlb.data[2],vlb.data[3]));
	if(val<minVal)	return false;

	val=getAngle(cv::Point3f(vla.data[1],vla.data[2],vla.data[3] ),cv::Point3f(vb.x-va.x,vb.y-va.y,vb.s-va.s));
	if(val<minVal)	return false;

	val=getAngle(cv::Point3f(vlb.data[1],vlb.data[2],vlb.data[3] ),cv::Point3f(vb.x-va.x,vb.y-va.y,vb.s-va.s));
	if(val<minVal)	return false;

	return true;
}
void MyLine3D::sampleLine(CImageData& img3D)
{
	do{
		xa=rand()%W;
		ya=rand()%H;
		za=rand()%S;
		if(!img3D.isVessel(xa,ya,za))	continue;

		{
			int size=10;
			std::vector<Voxel> nearbyPoints; 
			for(int mz=-size;mz<=size;++mz)
				for(int my=-size;my<=size;++my)
					for(int mx=-size;mx<=size;++mx)
					{
						Voxel vb(xa+mx,ya+my,za+mz);
						if(inRange(vb) && img3D.isVessel(vb.x,vb.y,vb.s))
						{
							nearbyPoints.push_back(vb);
						}
					}
			/////////////////////////////////////////////////////////
			Voxel va(xa,ya,za);
			for(int m=0;m<1000;++m)
			{
				int id=rand()%nearbyPoints.size();
				Voxel vb=nearbyPoints[id];
				xb=vb.x; yb=vb.y; zb=vb.s;
				if(inRange(vb)&&validLine()&&img3D.isVessel(vb.x,vb.y,vb.s)&&getLength()>2&&getLength()<20&&sameDirect(va,vb,img3D))		
					return;
			}
		}
	}
	while(true);
}
double MyLine3D::compEnergy(int x,int y,int z,const CImageData& img3D) const
{
	Vessel vl=img3D.getVessel(x,y,z);
	double angle=compAngle(vl.data[1],vl.data[2],vl.data[3]);
	double bonus=log(angle+1)+1;//angle bonus

	double dis=compDist(x,y,z);

	dis*=dis;
//	return dis/(theta);//+(1-vl.data[0])*(50/bonus);

	angle=acos(angle);
	double dte=(1-vl.data[0])*30*angle*angle;
	if(dte>50)	dte=50;

	double energy=//dis/theta; 
					log(sqrt(2*osg::PI)*theta) +dis/(theta*theta);
	if(energy<-500)	energy=-500;
	if(energy>500)	energy=500;


	/*if(supportPoints.size()>0 && mRadius>0)	
	{
		energy=energy*getLength()*mRadius*mRadius*osg::PI/supportPoints.size();
	}
	return energy;*/
	return energy*getLength();
}
double MyLine3D::compDist(int xc,int yc,int zc) const
{
	double SAB=sqrt(0.0+ (xa-xb)*(xa-xb)+(ya-yb)*(ya-yb)+(za-zb)*(za-zb)  );
	double SAC=sqrt(0.0+ (xa-xc)*(xa-xc)+(ya-yc)*(ya-yc)+(za-zc)*(za-zc)  );
	double SBC=sqrt(0.0+ (xc-xb)*(xc-xb)+(yc-yb)*(yc-yb)+(zc-zb)*(zc-zb)  );
	double P=(SAB+SAC+SBC)/2.0;
	double SS=sqrt(P*(P-SBC)*(P-SAC)*(P-SAB));

	cv::Point3f v21(xc-xa,yc-ya,zc-za);
	cv::Point3f v23(xb-xa,yb-ya,zb-za);
	double angel=getAngle(v21,v23);
	if(angel<cos(ANGLE))//angel cab >90,then dist=ca
	{
		return sqrt(v21.dot(v21));
	}
	else 
	{
		v21=cv::Point3f(xc-xb,yc-yb,zc-zb);
		v23=cv::Point3f(xa-xb,ya-yb,za-zb);
		angel=getAngle(v21,v23);
		if(angel<cos(ANGLE))	//angel cba >90,then dist=cb
		{
			return sqrt(v21.dot(v21));
		}
	}
	return (SS+SS)/SAB;
}
bool MyLine3D::inLineRange(int x,int y,int z)
{
	cv::Point3f v21(x-xa,y-ya,z-za);
	cv::Point3f v23(xb-xa,yb-ya,zb-za);
	double angel=getAngle(v21,v23);
	if(angel<cos(ANGLE))//angel cab >90
		return false;

	v21=cv::Point3f(x-xb,y-yb,z-zb);
	v23=cv::Point3f(xa-xb,ya-yb,za-zb);
	angel=getAngle(v21,v23);
	if(angel<cos(ANGLE))//angel cba >90
		return false;

	return compDirectDist(x,y,z)<mRadius;
}
double MyLine3D::compDirectDist(int xc,int yc,int zc) const
{
	double SAB=sqrt(0.0+ (xa-xb)*(xa-xb)+(ya-yb)*(ya-yb)+(za-zb)*(za-zb)  );
	double SAC=sqrt(0.0+ (xa-xc)*(xa-xc)+(ya-yc)*(ya-yc)+(za-zc)*(za-zc)  );
	double SBC=sqrt(0.0+ (xc-xb)*(xc-xb)+(yc-yb)*(yc-yb)+(zc-zb)*(zc-zb)  );
	double P=(SAB+SAC+SBC)/2.0;
	double SS=sqrt(P*(P-SBC)*(P-SAC)*(P-SAB));
	return (SS+SS)/SAB;
}
double  MyLine3D::compAngle(const MyLine3D& line)
{
	Voxel vl=line.getDirect();
	return compAngle(vl.x,vl.y,vl.s);
}
double MyLine3D::compAngle(float tx,float ty,float tz) const
{
	double angle=getAngle(cv::Point3f(xa-xb,ya-yb,za-zb),cv::Point3f(tx,ty,tz));
	return abs(angle);
}
void MyLine3D::createLine(int x,int y,int z)
{
	setStartPoint(x,y,z);

	while(xa==xb&&ya==yb&&za==zb)
	{
		xb=rand()%W;
		yb=rand()%H;
		zb=rand()%S;
	}
}
osg::Node* MyLine3D::create3DBox(int id)
{
	initCrossBox();
	char name[100];
	sprintf(name,"%d",id);

	osg::Group* pGrp=new osg::Group;
	float radius=3;

	osg::Vec3f pos1=getBoxPos(radius,0,true);
	osg::Vec3f pos2=getBoxPos(radius,0,false);
	osg::Vec3f pos3=getBoxPos(radius,osg::PI,false);
	osg::Vec3f pos4=getBoxPos(radius,osg::PI,true);

	osg::Vec3d color(1,0,0);
	pGrp->addChild(createFace(pos1,pos2,pos3,pos4,color,name));

	pos1=getBoxPos(radius,osg::PI_2,true);
	pos2=getBoxPos(radius,osg::PI_2,false);
	pos3=getBoxPos(radius,-osg::PI_2,false);
	pos4=getBoxPos(radius,-osg::PI_2,true);
	pGrp->addChild(createFace(pos1,pos2,pos3,pos4,color,name));

	pGrp->addChild(create3DNode());
	return pGrp;
}
osg::Node* MyLine3D::create3DLinePoints(int id)
{
	osg::Group* pGrp=new osg::Group;
	pGrp->addChild(create3DNode());

	osg::Geode* geode=new osg::Geode;
	osg::Geometry* geom=new osg::Geometry;
	osg::Vec3Array* coords = new osg::Vec3Array;
	osg::Vec3Array* colors=new osg::Vec3Array;

			for(int i=0;i<supportPoints.size();++i)
			{
				cv::Vec3i cp=supportPoints[i];
				coords->push_back(osg::Vec3f(cp[0] ,cp[1],cp[2]));
				colors->push_back(osg::Vec3f(1,1,1));
			}
	printf("Support Points=%d, Percent =%.2f\n",coords->size(), coords->size()/(tubePoints.size()+0.0));
	geom->setVertexArray(coords);

	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,coords->size()));

	geode->addDrawable(geom);
	pGrp->addChild(geode);
	return pGrp;
}
osg::Vec3f MyLine3D::getBoxPos(float radius,float angle,bool up)
{
	osg::Vec3f ct(xa,ya,za);
	if(!up)	ct=osg::Vec3f(xb,yb,zb);

	float px=ct.x()+radius*(nu.x()*cos(angle)+nv.x()*sin(angle));
	float py=ct.y()+radius*(nu.y()*cos(angle)+nv.y()*sin(angle));
	float pz=ct.z()+radius*(nu.z()*cos(angle)+nv.z()*sin(angle));

	return osg::Vec3f(px,py,pz);
}
osg::Node* MyLine3D::create3DNode()
{
	return create3DLine(osg::Vec3d(xa,ya,za),osg::Vec3d(xb,yb,zb),osg::Vec3d(1,0,0));
}
int MyLine3D::guessRadius(const std::vector<cv::Point3i>& points)
{
	int rList[1000]={0};
	for(int i=0;i<points.size();++i)
	{
		int dist=compDirectDist(points[i].x,points[i].y,points[i].z);
		rList[dist]++;
	}
	int counter=0;
	for(int k=999;k>=1;--k)
	{
		counter+=rList[k];
		double percent=counter/(points.size()+0.0);
		if(percent>0.05)	return k+1;
	}
	return 1;
}
MyLine3D MyLine3D::getLineByPoints(const std::vector<cv::Point3i>& points)
{
	MyLine3D mline;
	mline.setValid(false);

	cv::Vec6f line;
	cv::fitLine(points,line,CV_DIST_L2,0,0.01,0.01);  
	
	if(points.size()<2)	return mline;

	mline=extendLineByPoints(points,line);

	return mline;
}
MyLine3D MyLine3D::extendLineByPoints(const std::vector<cv::Point3i>& points,const cv::Vec6f& line)
{
	MyLine3D mline;
	int minX=9999,minY=9999,minZ=9999;
	int maxX=-1,maxY=-1,maxZ=-1;
	for(int i=0;i<points.size();++i)
	{
		if(points[i].x<=minX)	minX=points[i].x;
		if(points[i].x>=maxX)	maxX=points[i].x;
		if(points[i].y<=minY)	minY=points[i].y;
		if(points[i].y>=maxY)	maxY=points[i].y;
		if(points[i].z<=minZ)	minZ=points[i].z;
		if(points[i].z>=maxZ)	maxZ=points[i].z;
	}
	minX--;maxX++;minY--;maxY++;minZ--;maxZ++;
	//printf("Adjust fitting points %d,%.2f,%.2f,%.2f,%.2f, minX=%d,maxX=%d,minY=%d,maxY=%d\n",points.size(),line[0],line[1],line[2],line[3],minX,maxX,minY,maxY);
	double px=line[3];
	double py=line[4];
	double pz=line[5];
	double qx=line[3];
	double qy=line[4];
	double qz=line[5];
	while(px>=minX&&px<=maxX&&py>=minY&&py<=maxY&&pz>=minZ&&pz<=maxZ)
	{
		px-=line[0];
		py-=line[1];
		pz-=line[2];
	}
	mline.setStartPoint(px,py,pz);
	while(qx>=minX&&qx<=maxX&&qy>=minY&&qy<=maxY&&qz>=minZ&&qz<=maxZ)
	{
		qx+=line[0];
		qy+=line[1];
		qz+=line[2];
	}
	mline.setEndPoint(qx,qy,qz);
	mline.setValid(true);	
	return mline;
}
void MyLine3D::adjustByPoints(const std::vector<cv::Point3i>& points)
{
	//adjust the angle and length
	cv::Vec6f line;
	cv::fitLine(points,line,CV_DIST_L2,0,0.01,0.01);  
	
	if(points.size()<2)	return;

	int minX=9999,minY=9999,minZ=9999;
	int maxX=-1,maxY=-1,maxZ=-1;
	for(int i=0;i<points.size();++i)
	{
		if(points[i].x<=minX)	minX=points[i].x;
		if(points[i].x>=maxX)	maxX=points[i].x;
		if(points[i].y<=minY)	minY=points[i].y;
		if(points[i].y>=maxY)	maxY=points[i].y;
		if(points[i].z<=minZ)	minZ=points[i].z;
		if(points[i].z>=maxZ)	maxZ=points[i].z;
	}
	minX--;maxX++;minY--;maxY++;minZ--;maxZ++;
	//printf("Adjust fitting points %d,%.2f,%.2f,%.2f,%.2f, minX=%d,maxX=%d,minY=%d,maxY=%d\n",points.size(),line[0],line[1],line[2],line[3],minX,maxX,minY,maxY);
	double px=line[3];
	double py=line[4];
	double pz=line[5];
	while(px>=minX&&px<=maxX&&py>=minY&&py<=maxY&&pz>=minZ&&pz<=maxZ)
	{
		px-=line[0];
		py-=line[1];
		pz-=line[2];
	}
	xa=px;ya=py;za=pz;

	px=line[3];py=line[4];pz=line[5];
	while(px>=minX&&px<=maxX&&py>=minY&&py<=maxY&&pz>=minZ&&pz<=maxZ)
	{
		px+=line[0];
		py+=line[1];
		pz+=line[2];
	}
	xb=px;yb=py;zb=pz;
}
void MyLine3D::adjustLine(const std::vector<cv::Point3i>& cpoints,const CImageData& img3D )
{
	//get purged points
	std::vector<cv::Point3i> points=cpoints;//purgePoints(cpoints);
	//adjust the theta
	double len=getLength();
	int sum=points.size();
	//theta=sqrt(sum/len)*SIGMA;
	///////////////////////////////////////////////////////////////////////////////
	MyLine3D newline=getLineByPoints(cpoints);

	if(newline.isValid())	setByLine(newline);//set by this new valid line
	//adjustByPoints(points);//first adjustment
	mRadius=guessRadius(points);//set by guessed radius
	theta=mRadius;
//	return;
	/////////////////////////////////////////////////////////////////////////////////
	theta=0;
	//mRadius=0;
	for(int i=0;i<points.size();++i)
	{
		double dis=compDirectDist(points[i].x,points[i].y,points[i].z);
		theta+=dis*dis;

		//if(dis>mRadius)	mRadius=dis;
	}
	theta=sqrt( theta/points.size());
	if(theta>10)	theta=10;
	////////////////////////////////////////////////////////////////////////////////
	return;
}
std::vector<cv::Point3i> MyLine3D::purgePoints(const std::vector<cv::Point3i>& cpoints)
{
	cv::Mat data(cpoints.size(),3, CV_32F),labels;
	int all_keypoints=cpoints.size();
	for(int i=0;i<all_keypoints;++i)
	{
		data.at<float>(i,0)=cpoints[i].x; 
		data.at<float>(i,1)=cpoints[i].y; 
		data.at<float>(i,2)=cpoints[i].z;
	}
	const int K=20;
	const int MAX_ITER=100;
	if(all_keypoints<=K)	return cpoints;
	printf("kmeans begin K=%d, Number=%d\n",K,all_keypoints);
	cv::Mat centers(K, 1,CV_32F);
	kmeans(data, K, labels,cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, MAX_ITER, 1.0),3, 2, centers);

	int counter[K]={0};
	for(int i=0;i<all_keypoints;++i)
		++counter[labels.at<int>(i,0)];

	int smallLabel=-1;
	int minNum=0x7fffffff;
	for(int i=0;i<K;++i)
	{
		if(counter[i]<minNum)
		{
			minNum=counter[i];
			smallLabel=i;
		}
	}

	double percent=minNum/(all_keypoints+0.0);
	if(percent<0.05)//too low
	{
		std::vector<cv::Point3i> points;
		for(int i=0;i<all_keypoints;++i)
		{
			if(smallLabel!=labels.at<int>(i,0))
				points.push_back(cpoints[i]);
		}
		printf("KMeans Purge From %d to %d\n",cpoints.size(),points.size());
		return points;
	}
	else	
		return cpoints;
}
bool MyLine3D::isSimilar(const MyLine3D& line)
{
	double angle=compAngle(line);
	if(angle<cos(osg::PI_4/2))	return false;

	Voxel v1=line.getStartPoint();
	Voxel v2=this->getStartPoint();
	double dis1=v1.compDist(v2);

	v1=line.getStartPoint();
	v2=this->getEndPoint();
	double dis2=v1.compDist(v2);

	v1=line.getEndPoint();
	v2=this->getEndPoint();
	double dis3=v1.compDist(v2);

	v1=line.getEndPoint();
	v2=this->getStartPoint();
	double dis4=v1.compDist(v2);

	double maxDis=std::max(std::max(dis1,dis2),std::max(dis3,dis4));

	double len1=line.getLength();
	double len2=this->getLength();

	return maxDis>=0.35*(len1+len2);
}
double MyLine3D::getSimilarity(const MyLine3D& line)
{
	Voxel v1=line.getStartPoint();
	Voxel v2=this->getStartPoint();
	double dis1=v1.compDist(v2);

	v1=line.getStartPoint();
	v2=this->getEndPoint();
	double dis2=v1.compDist(v2);

	v1=line.getEndPoint();
	v2=this->getEndPoint();
	double dis3=v1.compDist(v2);

	v1=line.getEndPoint();
	v2=this->getStartPoint();
	double dis4=v1.compDist(v2);

	return std::min(std::min(dis1,dis2),std::min(dis3,dis4));
}
void MyLine3D::kmeanLines(const std::vector<cv::Point3i>& cpoints,const CImageData& img3D,bool  keepVoxels)
{
	cv::Mat data(cpoints.size(),3, CV_32F);
	int all_keypoints=cpoints.size();
	for(int i=0;i<all_keypoints;++i)
	{
		data.at<float>(i,0)=cpoints[i].x; 
		data.at<float>(i,1)=cpoints[i].y; 
		data.at<float>(i,2)=cpoints[i].z;
	}
	for(int K=2;K<=12;K+=2)
	{
		if(cpoints.size()>K)
		{
			cv::Mat labels=doKmeansPoints(data,K);
			std::vector<cv::Point3i> pointsList[10000];

			for(int i=0;i<cpoints.size();++i)
			{
				pointsList[labels.at<int>(i,0)].push_back(cpoints[i]);
			}
			clusterLines.clear();//clear line list
			for(int i=0;i<K;++i)
			{
				MyLine3D newline=getLineByPoints(pointsList[i]);
				newline.adjustLine(pointsList[i],img3D);//set sigma, mRadius
				if(newline.isValid())
				{
					clusterLines.push_back(newline);
					if(keepVoxels)
						clusterLines.back().supportPoints=pointsList[i];
				}
			}
		}
	}
	printf("Original Line has %d points, after kmeans clustring,there are  %d lines\n",cpoints.size(),clusterLines.size());
}
cv::Mat MyLine3D::doKmeansPoints(const cv::Mat& data,const int K)
{
	cv::Mat labels;
	int all_keypoints=data.rows;
	const int MAX_ITER=100;

	if(all_keypoints<=K)	return labels;

	printf("kmeans begin K=%d, Number=%d\n",K,all_keypoints);
	cv::Mat centers(K, 1,CV_32F);
	kmeans(data, K, labels,cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, MAX_ITER, 1.0),3, 2, centers);

	return labels;
}