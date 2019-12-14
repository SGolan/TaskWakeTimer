
#include <chrono>

class CTimeFromStart
{

public:
	static CTimeFromStart* GetInstance()
	{
		// CTimeFromStart is a singleton
		if (m_pCTimeFromStart == NULL)
		{
			m_pCTimeFromStart = new CTimeFromStart();
		}

		return m_pCTimeFromStart;
	}

	uint32_t GetTime()
	{
		auto TimeFromStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_StartTime).count();
		return static_cast<uint32_t>(TimeFromStart);

	}

	static CTimeFromStart   *m_pCTimeFromStart;

private:
	CTimeFromStart() { m_StartTime = std::chrono::system_clock::now(); };
	CTimeFromStart(const CTimeFromStart &) {};
	CTimeFromStart& operator = (const CTimeFromStart &) {};

	std::chrono::time_point<std::chrono::system_clock>	m_StartTime;
};