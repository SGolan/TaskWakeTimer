// File: CThreadSafePrintf.h
#pragma once
#include <iostream>
#include <sstream>
#include <string>  
#include <mutex> 

class CThreadSafePrintf
{

public:

	static CThreadSafePrintf* GetInstance()
	{
		// CThreadSafePrintf is a singleton
		if (m_pCThreadSafePrintf == NULL)
		{
			m_pCThreadSafePrintf = new CThreadSafePrintf();
		}
		return m_pCThreadSafePrintf;
	}

	void Print(std::stringstream *a_pstringstream)
	{
		m_Mutex.lock();
		std::cout << a_pstringstream->str();
		m_Mutex.unlock();
	}

	static CThreadSafePrintf   *m_pCThreadSafePrintf;

private:

	CThreadSafePrintf() {};
	CThreadSafePrintf(const CThreadSafePrintf &) {};
	CThreadSafePrintf& operator = (const CThreadSafePrintf &) {};

	std::mutex	m_Mutex;
};