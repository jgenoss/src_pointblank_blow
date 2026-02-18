#ifndef  __WBANetwork_Mutex_H
#define  __WBANetwork_Mutex_H


namespace WBANetwork
{
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

	private:
		CRITICAL_SECTION		m_cs;

	public:
		bool	TryLock();
		void	Lock();
		void	Unlock();

	};
}

#endif