#pragma once
#include "data3d.h"
#include"common.h"
template<typename T> class Data3D;

template<typename T=short>
class Image3D :public Data3D<T>
{
public:
	Image3D(int w,int h, int s):Data3D(w,h,s){};
	virtual ~Image3D(void){}
public:
	void loadImage(const char* filename,bool bigendian=false);
	void saveImage(const char*);
	int dI(int m,int n){return abs(0+m_pData[m]-m_pData[n]); }
	double lapLacian(int x,int y,int z){ return -(dXX(x,y,z)+dYY(x,y,z)+dZZ(x,y,z)); }
	void showMipByZ();
	void showMipByX();
	void showMipByY();
private:
};
template<typename T>
void Image3D<T>::loadImage(const char* filename,bool bigendian)
{
	int filesize=getDataSize();
	//read data file
	FILE* pFile=fopen(filename,"rb");
	smart_assert(pFile,filename);
	//load data from file
	int readsize=fread(m_pData,sizeof(T),filesize,pFile);
	assert(readsize==filesize);
	//printf("readsize=%lf, filesize=%d\n",readsize/(512.0),filesize);

	//change data format form big endian to small endian 
	if(bigendian&&sizeof(T)==2)
	{
		for(int i=0;i<filesize;++i)
		{
			T temp=m_pData[i];
			char* pH=(char*)(&temp);char* pL=pH+1;std::swap(*pH,*pL);
			m_pData[i]=temp;
		}
	}
}
template<typename T>
void Image3D<T>::saveImage(const char* filename)
{
	FILE* pFile=fopen(filename,"wb");
	smart_assert(pFile,filename);
	fwrite(m_pData,sizeof(T),getDataSize(),pFile);
	fclose(pFile);
}
template<typename T>
void Image3D<T>::showMipByZ()
{
	{
		cv::Mat mat=cv::Mat(getY(),getX(),CV_16S,cv::Scalar(0));
		for(int y=0;y<getY();++y)
			for(int x=0;x<getX();++x)
			{
				int maxVal=-1000000;
				for(int z=0;z<getZ();++z) 
				{
					T val=get(x,y,z);
					if(val>maxVal)	maxVal=val;
				}
				mat.at<short>(cv::Point(x,y))=maxVal;
			}
		normalize(mat,mat,0x7fff,-0x7fff,CV_MINMAX);
		cv::namedWindow("MIP",0);
		cv::imshow("MIP",mat);
		cv::setMouseCallback("MIP",on_mouse<short>,&mat);
	}
}
template<typename T>
void Image3D<T>::showMipByY()
{
	cv::Mat mat=cv::Mat(getZ(),getX(),CV_16S,cv::Scalar(0));
	for(int x=0;x<getX();++x)
		for(int z=0;z<getZ();++z){
			int maxVal=-1000000, posy=0;
			for(int y=0;y<getY();++y){
				T val=get(x,y,z);
				if(val>maxVal)		{maxVal=val;	posy=y;}
			}
			mat.at<short>(cv::Point(x,z))=maxVal;
		}
	normalize(mat,mat,0x7fff,-0x7fff,CV_MINMAX);
	cv::imshow("MIP",mat);
	cv::setMouseCallback("MIP",on_mouse<short>,&mat);
}
template<typename T>
void Image3D<T>::showMipByX()
{
	cv::Mat mat=cv::Mat(getZ(),getY(),CV_16S,cv::Scalar(0));
	for(int y=0;y<getY();++y)
		for(int z=0;z<getZ();++z){
			int maxVal=-1000000, posx=0;
			for(int x=getX()-1;x>=0;--x){
				T val=get(x,y,z);
				if(val>maxVal)		{maxVal=val;	posx=x;}
			}
			mat.at<short>(cv::Point(y,z))=maxVal;
		}
	normalize(mat,mat,0x7fff,-0x7fff,CV_MINMAX);
	cv::imshow("MIP",mat);
	cv::setMouseCallback("MIP",on_mouse<short>,&mat);
}

