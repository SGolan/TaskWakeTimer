// File main.cpp
#include <iostream>
#include <windows.h>
#include <chrono>
#include "CTaskTimerService.h"
#include "CThreadSafePrintf.h"
#include "CTimeFromStart.h"

#define VERSION_MAJOR	1
#define VERSION_MINOR	0


#define STRINGIFY(x)	 _STRINGIFY(x)
#define _STRINGIFY(x)	#x

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

	void Join() { m_thread.join();  }

private:

	void PrintStatus(uint32_t a_ThreadIndex, const char* a_strState)
	{
		std::stringstream cstream;
		uint32_t time_from_start = CTimeFromStart::GetInstance()->GetTime();
		cstream << "t = " << time_from_start << "[ms]: thread #" << a_ThreadIndex << a_strState << endl;
		CThreadSafePrintf::GetInstance()->Print(&cstream);
	}

	void ThreadFunction()
	{
		PrintStatus(m_ThreadIndex, " launched");
		// be idle for m_TimeBeforeSleep seconds
		Sleep(m_TimeBeforeSleep*1000);
		// wake up and "request" CTaskTimerServer to wake this thread again after m_TimeToSleep 
		PrintStatus(m_ThreadIndex, " going to sleep");
		CTaskTimerService::GetInstance()->Sleep(m_ThreadIndex, m_TimeToSleep);
		PrintStatus(m_ThreadIndex, " awakes");
	}
	uint16_t			m_ThreadIndex;
	uint32_t			m_TimeBeforeSleep;
	uint32_t			m_TimeToSleep;
	// Notice: order is important, m_thread should be last to make it initialized/launched
	// after all other members were initialized
	std::thread			m_thread;
};


int main()
{
	
	cout << "Version: " << STRINGIFY(VERSION_MAJOR) << "." << STRINGIFY(VERSION_MINOR) << endl;

	CTimeFromStart::GetInstance();

	cout << endl << "t = 0[ms] launch timer-service thread ..." << endl << endl;
	CTaskTimerService::GetInstance();

	// invoke timer-client #0: goes to sleep @ t=2, requests wakeup after 6sec i.e. @ t=8
	CTaskTimerClientThread	thread0(0, 2, 6);
	// invoke timer-client #1: goes to sleep @ t=3, requests wakeup after 1sec i.e. @ t=4
	CTaskTimerClientThread	thread1(1, 3, 1);
	// invoke timer-client #2: goes to sleep @ t=1, requests wakeup after 2sec i.e. @ t=3
	CTaskTimerClientThread	thread2(2, 1, 2);
	
	thread0.Join();
	thread1.Join();
	thread2.Join();

	this_thread::sleep_for(chrono::seconds(2));
	cout << endl << "Notice: at this point, both lists handeled by CTaskTimerService (waiting-list \r\n\
                     \rand awaken-list) should be both empty !" << endl;

	cout << endl << "Press ENTER to exit..." << endl;
	getchar();

	return 0;
}
