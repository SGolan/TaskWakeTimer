#include <iostream> 
#include <iterator> 
#include <thread>
#include "CTaskTimerService.h"

using namespace std;

CTaskTimerService* CTaskTimerService::m_pCTaskTimerService = NULL;

CTaskTimerService* CTaskTimerService::GetInstance()
{
	// CTaskTimerService is a singleton
	if (m_pCTaskTimerService != NULL)
	{
		return m_pCTaskTimerService;
	}
	else
	{
		m_pCTaskTimerService = new CTaskTimerService();
		return m_pCTaskTimerService;
	}
}

CTaskTimerService::CTaskTimerService() : std::thread([this] { this->ThreadFunction(); })
{
	m_CurrentTimeSec = 0;
}

void CTaskTimerService::ThreadFunction()
{

	for (; ; )
	{
		// sleep 1sec (simulates IRQ every 1sec)
		this_thread::sleep_for(chrono::seconds(1));
		
		// update current time
		m_CurrentTimeSec++;
		
		// scan list and signal all threads having their sleep time expired
		for (list<CTimerItem*>::iterator iter = m_ListCTimerItem.begin(); iter != m_ListCTimerItem.end(); ++iter)
		{
			if (iter->m_TimeToAwakeSec <= m_CurrentTimeSec)
			{
				iter->m_CSemaphore.Signal();
			}
		}
	}

}


void CTaskTimerService::Sleep(uint32_t a_TimeToSleepSec)
{
	// calculate time to wake
	uint32_t TimeSToWakeSec = m_CurrentTimeSec + a_TimeToSleepSec;
	// create timer list-item
	CTimerItem *pCTimerItem    = new CTimerItem(TimeSToWakeSec);
	
	// insert item to waiting threads list
	m_ListCTimerItemMutex.lock();
	m_ListCTimerItem.push_back(pCTimerItem);
	m_ListCTimerItemMutex.unlock();
	
	// block calling thread till wakeup signal
	pCTimerItem->m_CSemaphore.wait();
}
