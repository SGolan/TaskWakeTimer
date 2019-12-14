#include <iostream>
#include <ctime>
#include <windows.h>
#include "CTaskTimerService.h"
#include "CThreadSafePrintf.h"

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

	void PrintTime(uint32_t a_ThreadIndex, const char* a_strState)
	{
		std::stringstream cstream;
		auto time_from_start = chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - start).count();
		cstream << "t = " << time_from_start << "[ms]: thread #" << a_ThreadIndex << a_strState << endl;
		CThreadSafePrintf::Print(&cstream);
	}

	void ThreadFunction()
	{
		PrintTime(m_ThreadIndex, " launched");
		// be idle for m_TimeBeforeSleep seconds
		Sleep(m_TimeBeforeSleep*1000);
		// wake up and "request" CTaskTimerServer to wake this thread again after m_TimeToSleep 
		PrintTime(m_ThreadIndex, " going to sleep");
		CTaskTimerService::GetInstance()->Sleep(m_TimeToSleep);
		PrintTime(m_ThreadIndex, " awakes");
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
	start = std::chrono::system_clock::now();

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

	cout << endl << "Press ENTER to exit..." << endl;
	getchar();

	return 0;
}
