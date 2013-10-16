// BHTimer.h: interface for the BHTimer class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>

#if !defined(AFX_BHTIMER_H__BEE0C072_F822_4D6A_AED2_FE640C35EF45__INCLUDED_)
#define AFX_BHTIMER_H__BEE0C072_F822_4D6A_AED2_FE640C35EF45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// created by weisheng
// modified [5/17/2007 weisheng]

// 使用方法
// start: 计时开始
// pause: 计时暂停，返回当前计时时间(单位:ms)
// clear: 计时清零
#include <stdio.h>

class BHTimer  
{
public:
	BHTimer();
	virtual ~BHTimer();

	// 开始计时
	void start();

	// 暂停计时，返回当前累计计时结果(ms)
	double pause();

	// 计时清零
	void clear();

	void reStart(){clear();start(); }

 	// 获取当前计时结果
 	double getTimeInterval();
	
	void show(const char* title="") 
	{ printf("%s cost time =%lf\n",title,pause()); }
private:
	LARGE_INTEGER m_startCount;
	LARGE_INTEGER m_frequency;
	double m_interval;

};

#endif // !defined(AFX_BHTIMER_H__BEE0C072_F822_4D6A_AED2_FE640C35EF45__INCLUDED_)
