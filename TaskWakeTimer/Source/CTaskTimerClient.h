#include <mutex> 
#include <chrono>
#include <condition_variable>
#include "ITaskTimerClient.h"


class CTaskTimerClient : public ITaskTimerClient
{
public:
	void Wake();
	void Sleep(std::chrono::steady_clock::time_point a_TimeDuration);
private:
	bool									m_bTimeElapsed;
	std::condition_variable					m_cvTimeElapsed;
	std::mutex								m_Mutex;
};



