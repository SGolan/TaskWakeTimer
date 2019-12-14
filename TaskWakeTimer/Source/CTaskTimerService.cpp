#include <iterator> 
#include "CTaskTimerService.h"

using namespace std;

ITaskTimerService* CTaskTimerService::m_pCTaskTimerService = NULL;

ITaskTimerService* CTaskTimerService::GetInstance()
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

CTaskTimerService::CTaskTimerService() : m_thread([this] { this->ThreadFunction(); })
{
	m_CurrentTimeSec = 0;
}

void CTaskTimerService::ThreadFunction()
{

	for (; ; )
	{
			
		// scan the list, signal threads having expired sleep time and remove their entries
		list<CTimerItem*>::iterator iter = m_ListpCTimerItemWaitingThreads.begin();
		while( iter != m_ListpCTimerItemWaitingThreads.end() )
		{
			if ((*iter)->m_TimeToAwakeSec <= m_CurrentTimeSec)
			{
				// signal waiting thread to awake
				(*iter)->m_CSemaphore.Signal();
				// move its CTimerItem from to-be-awaken-list to monitor-thread-to-awake list
				m_ListpCTimerItemAwakenThreads.push_back(*iter);
				iter = m_ListpCTimerItemWaitingThreads.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		// delete CTimerItem of threads signaled they are awake, from monitor-thread-to-awake list
		iter = m_ListpCTimerItemAwakenThreads.begin();
		while (iter != m_ListpCTimerItemAwakenThreads.end())
		{
			if ((*iter)->m_WaitingThreadAwaken)
			{
				iter = m_ListpCTimerItemAwakenThreads.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		// sleep 1sec (simulates IRQ every 1sec)
		this_thread::sleep_for(chrono::seconds(1));
		// update current time
		m_CurrentTimeSec++;
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
	m_ListpCTimerItemWaitingThreads.push_back(pCTimerItem);
	m_ListCTimerItemMutex.unlock();
	
	// calling thread blocks till wakeup signal
	pCTimerItem->m_CSemaphore.Wait();
	
	// calling thread signals it's awake
	pCTimerItem->m_WaitingThreadAwaken = true;
}
