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

// ʹ�÷���
// start: ��ʱ��ʼ
// pause: ��ʱ��ͣ�����ص�ǰ��ʱʱ��(��λ:ms)
// clear: ��ʱ����
#include <stdio.h>

class BHTimer  
{
public:
	BHTimer();
	virtual ~BHTimer();

	// ��ʼ��ʱ
	void start();

	// ��ͣ��ʱ�����ص�ǰ�ۼƼ�ʱ���(ms)
	double pause();

	// ��ʱ����
	void clear();

	void reStart(){clear();start(); }

 	// ��ȡ��ǰ��ʱ���
 	double getTimeInterval();
	
	void show(const char* title="") 
	{ printf("%s cost time =%lf\n",title,pause()); }
private:
	LARGE_INTEGER m_startCount;
	LARGE_INTEGER m_frequency;
	double m_interval;

};

#endif // !defined(AFX_BHTIMER_H__BEE0C072_F822_4D6A_AED2_FE640C35EF45__INCLUDED_)
