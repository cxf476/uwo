#pragma once
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include<utility>
#include"smart_assert.h"
using std::endl;
using std::cout;
template<typename T>
class Data3D
{
public:
	Data3D(int w,int h, int s);
	virtual ~Data3D(void);
	
	T& get(int x,int y, int s){return m_pData[s*mDims[0]*mDims[1]+y*mDims[0]+x]; }
	T& get(int id){return m_pData[id];}
	int getID(int x,int y, int s){return s*mDims[0]*mDims[1]+y*mDims[0]+x;}
	int getDataSize(){return mDims[0]*mDims[1]*mDims[2];}
public:
	int getZ(){return mDims[2];}
	int getY(){return mDims[1];}
	int getX(){return mDims[0];}
	int getDim(int dim)
	{
		smart_assert(dim>=0&&dim<3,"DIM>=0 && DIM<3");
		return mDims[dim];
	}
	void printDims()
	{
		printf("%d %d %d\n",getDim(0),getDim(1),getDim(2));
	}
	void printData()
	{
		for(int z=0;z<mDims[2];++z)
		{
			for(int y=0;y<mDims[1];++y)
			{
				for(int x=0;x<mDims[0];++x)
				{
					std::cout<<get(x,y,z)<<" ";
				}
				cout<<"\n";
			}
			cout<<"*******************\n";
		}
	}
	bool inRange(int x,int y,int z)
	{
		return x>=0&&x<mDims[0]&&y>=0&&y<mDims[1]&&z>=0&&z<mDims[2];
	}
	double getValue(int x,int y,int s)
	{
		if(!inRange(x,y,s))	return 0;
		return get(x,y,s);
	}
protected:
	int mDims[3];
	T *m_pData;//image data pointer,
};
template<typename T>
inline Data3D<T>::Data3D(int w,int h, int s)
{
	mDims[0]=w;mDims[1]=h;mDims[2]=s;
	m_pData=new T[w*h*s];
	memset(m_pData,0,sizeof(T)*w*h*s);
}
template<typename T>
inline Data3D<T>::~Data3D(void)
{
	if(m_pData)	delete []m_pData;
	fprintf(stderr,"Release Data of Data3D\n");
}