// File: CTaskTimerService.h
#pragma once
#include <cstdint>
#include <iterator> 
#include <list> 
#include <mutex> 
#include <thread> 
#include "ITaskTimerService.h"

class CTaskTimerService : public ITaskTimerService
{

	class CSemaphore
	{
	public:
		CSemaphore() { };
		std::condition_variable			m_ConditionVariable;
		std::mutex						m_Mutex;
		void Signal()
		{
			m_ConditionVariable.notify_one();
		};
		void Wait()
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_ConditionVariable.wait(lock);		//sharon.golan
		};
	};

	class CTimerItem
	{
	public:
		CTimerItem(uint32_t m_Threadindex, uint32_t a_TimeToWakeSec) : m_Threadindex(m_Threadindex)		,
																	   m_WaitingThreadAwaken(false)		,
																	   m_TimeToAwakeSec(a_TimeToWakeSec)	{};
		uint32_t	m_TimeToAwakeSec;
		CSemaphore	m_CSemaphore;
		bool        m_WaitingThreadAwaken;
		uint32_t	m_Threadindex;
	};

public:

	static ITaskTimerService*	GetInstance();
	virtual void				Sleep(uint32_t a_ThreadIndex, uint32_t a_TimeToSleepSec);

	static ITaskTimerService   *m_pCTaskTimerService;

private:
	CTaskTimerService();
	CTaskTimerService(const CTaskTimerService &)			 {};
	CTaskTimerService& operator = (const CTaskTimerService &) {};
	void	ThreadFunction();
	void	AddToWaitingThreadsSortedList(CTimerItem* a_pCTimerItem);
	void    ScanWaitingThreadsSortedList();
	void    ScanAwakenThreadsSortedList();
	void    PrintStatus();

	uint32_t					m_CurrentTimeSec;
	std::list<CTimerItem *>		m_ListpCTimerItemWaitingThreads;
	std::list<CTimerItem *>		m_ListpCTimerItemAwakenThreads;
	std::mutex					m_ListCTimerItemMutex;
	// Notice: order is important, m_thread should be last to make it initialized/launched
	// after all other members were initialized
	std::thread					m_thread;

};