// File: CTaskTimerService.cpp
#include <iterator> 
#include "CTaskTimerService.h"
#include "CThreadSafePrintf.h"
#include "CTimeFromStart.h"


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

void CTaskTimerService::ScanWaitingThreadsSortedList()
{
	m_ListCTimerItemMutex.lock();

	// scan sorted-list (from early wake-time to late) : signal threads having expired sleep time and remove their entries
	list<CTimerItem*>::iterator iter;
	for (iter = m_ListpCTimerItemWaitingThreads.begin(); iter != m_ListpCTimerItemWaitingThreads.end(); )
	{
		if (m_CurrentTimeSec < (*iter)->m_TimeToAwakeSec)
		{
			break;
		}

		// signal waiting thread to awake
		(*iter)->m_CSemaphore.Signal();
		// move its CTimerItem from to-be-awaken-list to monitor-thread-to-awake list
		// TODO: this list should be further managed by timeout and size-limit in order
		// handle stray threads (not signaled they were awaked) !!!
		m_ListpCTimerItemAwakenThreads.push_back(*iter);
		iter = m_ListpCTimerItemWaitingThreads.erase(iter);
		PrintStatus();
	}

	m_ListCTimerItemMutex.unlock();
}

void CTaskTimerService::ScanAwakenThreadsSortedList()
{
	// remove CTimerItem of awaken-threads from monitor-thread-awaken list
	list<CTimerItem*>::iterator iter;
	for (iter = m_ListpCTimerItemAwakenThreads.begin(); iter != m_ListpCTimerItemAwakenThreads.end(); )
	{
		if ((*iter)->m_WaitingThreadAwaken)
		{
			iter = m_ListpCTimerItemAwakenThreads.erase(iter);
			PrintStatus();
		}
		else
		{
			++iter;
		}
	}
}



void CTaskTimerService::ThreadFunction()
{

	for (; ; )
	{
		// scan waiting-threads list and release threads with expired wait-time
		ScanWaitingThreadsSortedList();
		
		// scan awaken-threads list and release resource after thread signaled it resumed its run
		ScanAwakenThreadsSortedList();		

		// sleep 1sec (simulates IRQ every 1sec)
		this_thread::sleep_for(chrono::seconds(1));
		// update current time
		m_CurrentTimeSec++;
	}

}


void CTaskTimerService::AddToWaitingThreadsSortedList(CTimerItem* a_pCTimerItem)
{

	m_ListCTimerItemMutex.lock();

	// insert item to waiting threads ordered-list

	if (m_ListpCTimerItemWaitingThreads.empty())
	{
		// case list is empty 
		m_ListpCTimerItemWaitingThreads.push_back(a_pCTimerItem);
	}
	else
	{
		list<CTimerItem*>::iterator iter;
		// list not empty : look for thread with later wakeup-time
		for (iter = m_ListpCTimerItemWaitingThreads.begin(); iter != m_ListpCTimerItemWaitingThreads.end(); ++iter)
		{
			if ((*iter)->m_TimeToAwakeSec > a_pCTimerItem->m_TimeToAwakeSec)
			{
				// if found, insert just before 
				m_ListpCTimerItemWaitingThreads.insert(iter, a_pCTimerItem);
				break;
			}
		}
		if (iter == m_ListpCTimerItemWaitingThreads.end())
		{
			// case thread with later wakeup-time not found, insert at end
			m_ListpCTimerItemWaitingThreads.push_back(a_pCTimerItem);
		}
	}

	m_ListCTimerItemMutex.unlock();
}


void CTaskTimerService::Sleep(uint32_t a_ThreadIndex, uint32_t a_TimeToSleepSec)
{
	// calculate time to wake
	uint32_t TimeSToWakeSec = m_CurrentTimeSec + a_TimeToSleepSec;
	// create timer list-item
	CTimerItem *pCTimerItem = new CTimerItem(a_ThreadIndex, TimeSToWakeSec);

	AddToWaitingThreadsSortedList(pCTimerItem);

	PrintStatus();
	
	// calling thread blocks till wakeup signal
	pCTimerItem->m_CSemaphore.Wait();
	
	// calling thread signals it's awake
	pCTimerItem->m_WaitingThreadAwaken = true;
}


void CTaskTimerService::PrintStatus()
{

	uint32_t time_from_start = CTimeFromStart::GetInstance()->GetTime();
	std::stringstream cstream;

	// print waiting-threads list
	cstream << "t = " << time_from_start << "[ms]: waiting list: /" ;
	list<CTimerItem*>::iterator iter = m_ListpCTimerItemWaitingThreads.begin();
	for(iter = m_ListpCTimerItemWaitingThreads.begin(); iter != m_ListpCTimerItemWaitingThreads.end(); ++iter)
	{
		cstream << "-> t" << (*iter)->m_Threadindex << " ";
	}
	
	// print already-awaken-threads list
	cstream <<  " awaken list: /";
	iter = m_ListpCTimerItemAwakenThreads.begin();
	for (iter = m_ListpCTimerItemAwakenThreads.begin(); iter != m_ListpCTimerItemAwakenThreads.end(); ++iter)
	{
		cstream << "-> t" << (*iter)->m_Threadindex << " ";
	}
	cstream << endl;

	CThreadSafePrintf::GetInstance()->Print(&cstream);

}
