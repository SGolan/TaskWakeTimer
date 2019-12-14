#include <iostream>
#include <sstream>
#include <string>  
#include <mutex> 


class CThreadSafePrintf
{

public:

	static void Print(std::stringstream *a_pstringstream)
	{
		static CThreadSafePrintf   *m_pCThreadSafePrintf = NULL;
		static std::mutex		    m_Mutex;

		// CThreadSafePrintf is a singleton
		if (m_pCThreadSafePrintf == NULL)
		{
			m_pCThreadSafePrintf = new CThreadSafePrintf();
		}

		m_Mutex.lock();
		std::cout << a_pstringstream->str();
		m_Mutex.unlock();
	}

private:

	CThreadSafePrintf() {};
	CThreadSafePrintf(const CThreadSafePrintf &) {};
	CThreadSafePrintf& operator = (const CThreadSafePrintf &) {};
};