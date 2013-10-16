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
	// ��¼frequency
	QueryPerformanceFrequency(&m_frequency);
	m_interval = 0;
	m_startCount.QuadPart = 0;
}

BHTimer::~BHTimer()
{
	
}

// ��ʼ��ʱ
void BHTimer::start()
{
	QueryPerformanceCounter(&m_startCount);
}

// ��ͣ��ʱ�����ص�ǰ�ۼƼ�ʱ���(��λ: ms)
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

// ��ʱ����
void BHTimer::clear()
{
	m_interval = 0;
}

// ��ȡ��ǰ��ʱ���(��λ: ms)
double BHTimer::getTimeInterval()
{
	return m_interval;
}
