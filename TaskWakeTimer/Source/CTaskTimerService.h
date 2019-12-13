#include <cstdint>
#include <iterator> 
#include <list> 
#include <mutex> 
#include "ITaskTimerService.h"

class CTaskTimerService : public ITaskTimerService
{

	class CSemaphore
	{
	public:
		CSemaphore() { };
		std::condition_variable			m_ConditionVariable;
		std::mutex						m_Mutex;
		void signal()
		{
			m_ConditionVariable.notify_one();

		};
		void wait()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_ConditionVariable.wait(lock);
		};
	};

	class CTimerItem
	{
	public:
		CTimerItem(uint32_t a_TimeToWakeSec) : m_TimeToAwakeSec(a_TimeToWakeSec);
		uint32_t	m_TimeToAwakeSec;
		CSemaphore	m_CSemaphore;
	};

public:

	static CTaskTimerService*	GetInstance();
	virtual void				Sleep(uint32_t a_TimeToSleepSec);

private:
	CTaskTimerService();
	CTaskTimerService(const CTaskTimerService &)			 {};
	CTaskTimerService operator = (const CTaskTimerService &) {};

	CTaskTimerService		   *m_pCTaskTimerService;
	std::thread					m_thread;
	uint32_t					m_CurrentTimeSec;
	std::list<*CTimerItem>		m_ListCTimerItem;
	std::mutex					m_ListCTimerItemMutex;

};