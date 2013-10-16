#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
class CFitModel
{
public:
	CFitModel(void):support(0),valid(true){}
	virtual ~CFitModel(void){}
public:
	bool isValid(){return valid;}
	void setValid(bool tag)
	{
		valid=tag;
	}
	void addSupport()
	{
		++support;
	}
	void clearSupport()
	{
		support=0;
	}
	int getSupport()
	{
		return support;
	}
	virtual double compDist(int x,int y,int z=0) const =0;
	virtual void print()=0;
///////////////////////////////////////
private:
	int support;
	bool valid;
};

