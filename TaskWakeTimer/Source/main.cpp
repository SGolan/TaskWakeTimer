#include <iostream>
#include <ctime>
#include <windows.h>
#include "CTaskTimerService.h"

using namespace std;

std::chrono::system_clock::time_point start;

class CTaskTimerClientThread
{

public:

	CTaskTimerClientThread(	uint16_t a_ThreadIndex		, 
							uint32_t a_TimeBeforeSleep	,
							uint32_t a_TimeToSleep		) : m_ThreadIndex		(a_ThreadIndex), 
															m_TimeBeforeSleep	(a_TimeBeforeSleep),
															m_TimeToSleep		(a_TimeToSleep),
															m_thread			([this] { this->ThreadFunction(); })
	{
	}

private:

	void ThreadFunction()
	{

		cout << "t = " << chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now()-start).count()     << ": thread #" << m_ThreadIndex << " invoked..." << endl;

		// sleep for a while
		cout << "t = " << chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now()-start).count()  << ": thread #" << m_ThreadIndex << " going to sleep" << endl;
		Sleep(m_TimeBeforeSleep*1000);
		// wake up and "request" CTaskTimerServer to wake this thread again after m_TimeToSleep 
		CTaskTimerService::GetInstance()->Sleep(m_TimeToSleep);
		cout << "t = " << chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now() - start).count() << ": thread #" << m_ThreadIndex << " awaken" << endl;
		
		for (; ; )
		{
			Sleep(1000);
		}

	}
	uint16_t	m_ThreadIndex;
	uint32_t	m_TimeBeforeSleep;
	uint32_t	m_TimeToSleep;
	std::thread	m_thread;
};


int main()
{
	start = std::chrono::system_clock::now();

	// thread #0: goes to sleep @ t=2 and asks to be awaken after 4sec, i.e. @ t=6
	CTaskTimerClientThread	thread0(0, 2, 6);
	// thread #1: goes to sleep @ t=3 and asks to be awaken after 5sec, i.e. @ t=8
	//CTaskTimerClientThread	thread0(1, 3, 5);
	// thread #2: goes to sleep @ t=1 and asks to be awaken after 2sec, i.e. @ t=3
	//CTaskTimerClientThread	thread0(2, 1, 2);

	return 0;
}
