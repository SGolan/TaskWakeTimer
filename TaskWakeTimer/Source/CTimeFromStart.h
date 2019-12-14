
#include <chrono> 


class CTimeFromStart
{

public:
	static void GetInstance()
	{
		GetTime();
	}
	static uint32_t GetTime()
	{
		static CTimeFromStart   *m_pCTimeFromStart = NULL;
		static std::chrono::time_point<std::chrono::system_clock>	m_StartTime;
		
		// CTimeFromStart is a singleton
		if (m_pCTimeFromStart == NULL)
		{
			m_pCTimeFromStart = new CTimeFromStart();
			m_StartTime = std::chrono::system_clock::now();
		}

		auto m_TimeFromStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_StartTime).count();
		return static_cast<uint32_t>(m_TimeFromStart);

	}

private:

	CTimeFromStart() {};
	CTimeFromStart(const CTimeFromStart &) {};
	CTimeFromStart& operator = (const CTimeFromStart &) {};
};