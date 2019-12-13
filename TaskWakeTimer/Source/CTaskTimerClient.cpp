

#include "ITaskTimerClient.h"


/**********************************************************
  Timer client
***********************************************************/

#include "ITaskTimerServer.h"

/**
 * \class ITaskTimerClient
 *
 * \brief interface for a thread using a timer service
 */
class ITaskTimerClient
{
public:
	void Wake() { m_bTimeElapsed = true; };
	vois SetTimePoint(chrono::steady_clock::time_point a_TimePoin) { m_TimePoint = a_TimePoint; };

	void Sleep(uint32_t a_TimeMs)
	{
		// reset signal & call timer-server's function
		m_bTimeElapsed = false;
		CTaskTimerServer::GetInstance()->Sleep(a_TimeMs, this);
		// wait for timer-server to signal time to wake
		m_cvTimeElapsed.wait(m_Mutex, (m_bTimeElapsed == true));
	}

	bool								m_bTimeElapsed;
	condition_variable					m_cvTimeElapsed;
	mutex								m_Mutex;
	chrono::steady_clock::time_point	m_TimePoint;
};



#include "CTaskTimerClient.h"

class CTaskTimerClient : public ITaskTimerClient
{
public:
	void Wake() { m_bTimeElapsed = true; };
	vois SetTimePoint(chrono::steady_clock::time_point a_TimePoin) { m_TimePoint = a_TimePoint; };

	void Sleep(uint32_t a_TimeMs)
	{
		// reset signal & call timer-server's function
		m_bTimeElapsed = false;
		CTaskTimerServer::GetInstance()->Sleep(a_TimeMs, this);
		// wait for timer-server to signal time to wake
		m_cvTimeElapsed.wait(m_Mutex, (m_bTimeElapsed == true));
	}

	bool								m_bTimeElapsed;
	condition_variable					m_cvTimeElapsed;
	mutex								m_Mutex;
	chrono::steady_clock::time_point	m_TimePoint;
};