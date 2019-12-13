#include <iostream>

using namespace std;

#include "CTaskTimerService.h"


class CTaskTimerClientThread
{

public:

	CTaskTimerClientThread(	uint16_t aThreadIndex		, 
							uint32_t aTimeBeforeSleep	,
							uint32_t aTimeToSleep		) : std::thread([this] { this->ThreadFunction(); })
	{
		m_ThreadIndex     = aThreadIndex	;
		m_TimeBeforeSleep = aTimeBeforeSleep;
		m_TimeToSleep     = aTimeToSleep	;
	}

private:

	void CTaskTimerClientThread::ThreadFunction()
	{
		cout << chrono::system_clock::now() << " thread #" << m_ThreadIndex << " invoked..." << endl;
		for (; ; )
		{
			// sleep for a while
			cout << chrono::system_clock::now() << " thread #" << m_ThreadIndex << " going to sleep" << endl;
			this_thread::sleep_for(std::chrono::seconds(m_TimeBeforeSleep));
			// wake up and "request" CTaskTimerServer to wake this thread again after m_TimeToSleep 
			CTaskTimerServer::GetInstance()->Sleep(m_TimeToSleep);
			cout << chrono::system_clock::now() << " thread #" << m_ThreadIndex << " awaken" << endl;

			return;
		}

	}
	uint16_t m_ThreadIndex;
	uint32_t m_TimeBeforeSleep;
	uint32_t m_TimeToSleep;
};


int main()
{
	// thread #0: goes to sleep @ t=2 and asks to be awaken after 4sec, i.e. @ t=6
	CTaskTimerClientThread	thread0(0, 2, 4);
	// thread #1: goes to sleep @ t=3 and asks to be awaken after 5sec, i.e. @ t=8
	CTaskTimerClientThread	thread0(1, 3, 5);
	// thread #2: goes to sleep @ t=1 and asks to be awaken after 2sec, i.e. @ t=3
	CTaskTimerClientThread	thread0(2, 1, 2);

	return 0;
}
