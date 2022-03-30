#pragma once

class LC_SequenceManager
{
public:
	static LC_SequenceManager& Instance()
	{
		static LC_SequenceManager s_hInstance;
		return s_hInstance;
	}
	
	LONG GetSequenceID()
	{
#ifdef WIN32
		InterlockedIncrement(&m_lCounter);
#else
		__sync_fetch_and_add(&m_lCounter,1);
#endif

		return m_lCounter;
	}

private:
	LC_SequenceManager() : m_lCounter(0) {}

private:
	LONG		m_lCounter;
};