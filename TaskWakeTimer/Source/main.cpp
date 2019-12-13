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

	void Join() { m_thread.join();  }

	static std::mutex	m_coutMutex;

private:

	void PrintTime(uint32_t a_ThreadIndex, const char* a_strState)
	{
		m_coutMutex.lock();
		cout << "t = " << chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now() - start).count() << ": thread # " << a_ThreadIndex << a_strState << endl;
		m_coutMutex.unlock();
	}

	void ThreadFunction()
	{
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
	std::thread			m_thread;
};

std::mutex	CTaskTimerClientThread::m_coutMutex;


int main()
{
	start = std::chrono::system_clock::now();

	// thread #0: goes to sleep @ t=2 and asks to be awaken after 6sec, i.e. @ t=8
	CTaskTimerClientThread	thread0(0, 2, 6);
	// thread #1: goes to sleep @ t=3 and asks to be awaken after 1sec, i.e. @ t=4
	CTaskTimerClientThread	thread1(1, 3, 1);
	// thread #2: goes to sleep @ t=1 and asks to be awaken after 2sec, i.e. @ t=3
	CTaskTimerClientThread	thread2(2, 1, 2);


	thread0.Join();
	thread1.Join();
	thread2.Join();

	cout << endl << "Press ENTER to exit..." << endl;
	getchar();

	return 0;
}
