#include <iostream> 
#include <iterator> 
#include <list> 
#include <mutex> 
#include <cstdint>
#include <thread>
#include <chrono>
#include <condition_variable>

using namespace std;

class CTaskTimerServerThread;


/**********************************************************
  Timer client
***********************************************************/

/**
 * \class ITaskTimerClient
 *
 * \brief interface for a thread using a timer service
 */
class ITaskTimerClient
{
public:
	void Wake() { m_bTimeElapsed = true; };
	vois SetTimePoint(chrono::steady_clock::time_point a_TimePoin) { m_TimePoint = a_TimePoint ; };

	void Sleep(uint32_t a_TimeMs)
	{
		// reset signal & call timer-server's function
		m_bTimeElapsed = false;
		CTaskTimerServer::GetInstance()->Sleep(a_TimeMs, this);
		// wait for timer-server to signal time to wake
		m_cvTimeElapsed.wait(m_Mutex, (m_bTimeElapsed == true));
	}

	bool								m_bTimeElapsed	;
	condition_variable					m_cvTimeElapsed	;
	mutex								m_Mutex			;
	chrono::steady_clock::time_point	m_TimePoint		;
};


class CTaskTimerClientThread : public ITaskTimerClient
{
public:

	CTaskTimerClientThread(uint16_t aThreadIndex, uint32_t aTimeBeforeSleep, uint32_t aTimeToSleep) : std::thread([this] { this->ThreadFunction(); })
    {
		m_ThreadIndex     = aThreadIndex     ;
		m_TimeBeforeSleep = aTimeBeforeSleep ;
		m_TimeToSleep     = aTimeToSleep     ;
	}

	void ThreadFunction()
	{
		cout << chrono::system_clock::now() << " thread #" << m_ThreadIndex << "invoked..." << endl;
		for (; ; )
		{
			this_thread::sleep_for(std::chrono::seconds(m_TimeBeforeSleep));
			CTaskTimerServer::GetInstance()->Sleep(m_TimeToSleep);
			
			return;
		}

	}
	uint16_t m_ThreadIndex		;
	uint32_t m_TimeBeforeSleep	;
	uint32_t m_TimeToSleep		;
};


/**********************************************************
  Timer server
***********************************************************/

// class of timer-server thread (used by timer server class)
class CTaskTimerServerThread
{
public:

};

// singleton class : timer server
class CTaskTimerServer
{
public:

	// singleton get-instance 
	static CTaskTimerServer* GetInstance()
	{
		if (m_pCTaskTimerServer)
		{
			return m_pCTaskTimerServer;
		}
		else
		{
			m_pCTaskTimerServer = new CTaskTimerServer();
			return m_pCTaskTimerServer;
		}
	}
	// Sleep interface function (to be called by client threads)
	void Sleep(ITaskTimerClient *a_pITaskTimerClient, uint32_t a_TimeMs)
	{
		chrono::steady_clock::time_point CurrentTimePoint = chrono::steady_clock::now();
		a_pITaskTimerClient->SetTimePoint( CurrentTimePoint + chrono::seconds(a_TimeMs) );
		m_listWaitingTasks.push_back(a_pITaskTimerClient);
	}


	void ThreadFunction()
	{
		cout << "CTaskTimerServerThread:: thread invoked" << endl;
		for (; ; )
		{
			// sleep 1 sec
			this_thread::sleep_for(chrono::seconds(1));
			// read time and scan 
			chrono::steady_clock::time_point CurrentTimePoint = chrono::steady_clock::now();
			for (list<ITaskTimerClient>::iterator iter = m_listWaitingTasks.begin(); iter != m_listWaitingTasks.end(); ++iter)
			{
				if (iter->GetTimetoWake() <= CurrentTimePoint)
				{
					iter->Wake() = true;
				}
			}
		}

	}

private:

	CTaskTimerServer() : std::thread([this] { this->ThreadFunction(); }) {  };

	static CTaskTimerServer*	m_pCTaskTimerServer;
	list <ITaskTimerClient*>	m_listWaitingTasks;
	thread						m_thread;

};
