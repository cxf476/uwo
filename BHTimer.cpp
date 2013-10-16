// BHTimer.cpp: implementation of the BHTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "BHTimer.h"
#include <iostream>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BHTimer::BHTimer()
{
	// 记录frequency
	QueryPerformanceFrequency(&m_frequency);
	m_interval = 0;
	m_startCount.QuadPart = 0;
}

BHTimer::~BHTimer()
{
	
}

// 开始计时
void BHTimer::start()
{
	QueryPerformanceCounter(&m_startCount);
}

// 暂停计时，返回当前累计计时结果(单位: ms)
double BHTimer::pause()
{
	if(m_startCount.QuadPart == 0)
		m_interval = 0;
	else
	{
		LARGE_INTEGER stopCount;
		QueryPerformanceCounter(&stopCount);

		m_interval += (double)(stopCount.QuadPart - m_startCount.QuadPart) / (double)m_frequency.QuadPart * 1000;

	}

	return m_interval;

}

// 计时清零
void BHTimer::clear()
{
	m_interval = 0;
}

// 获取当前计时结果(单位: ms)
double BHTimer::getTimeInterval()
{
	return m_interval;
}
