#pragma once

#if defined(WIN32)
#	define _WIN32_THREADS
#	include <windows.h>
#elif defined(__GNUG__) || defined(_USE_POSIX_THREADS)
#	define _HAS_POSIX_THREADS
#	include <pthread.h>
#else
#	define _SINGLE_THREADED
#endif

#ifndef DEFAULT_MT_POLICY
#	ifdef _SINGLE_THREADED
#		define _DEFAULT_MT_POLICY single_threaded
#	else
#		define _DEFAULT_MT_POLICY multi_threaded_local
#	endif
#endif

class single_threaded
{
public:
	single_threaded()
	{
		;
	}

	virtual ~single_threaded()
	{
		;
	}

	virtual void lock()
	{
		;
	}

	virtual void unlock()
	{
		;
	}
};

// The multi threading policies only get compiled in if they are enabled.
class multi_threaded_global
{
public:
	multi_threaded_global()
	{
		static bool isinitialised = false;

		if (!isinitialised)
		{
			InitializeCriticalSection(get_critsec());
			isinitialised = true;
		}
	}

	multi_threaded_global(const multi_threaded_global&)
	{
		;
	}

	virtual ~multi_threaded_global()
	{
		;
	}

	virtual void lock()
	{
		EnterCriticalSection(get_critsec());
	}

	virtual void unlock()
	{
		LeaveCriticalSection(get_critsec());
	}

private:
	CRITICAL_SECTION* get_critsec()
	{
		static CRITICAL_SECTION g_critsec;
		return &g_critsec;
	}
};

class multi_threaded_local
{
public:
	multi_threaded_local()
	{
		InitializeCriticalSection(&m_critsec);
	}

	multi_threaded_local(const multi_threaded_local&)
	{
		InitializeCriticalSection(&m_critsec);
	}

	virtual ~multi_threaded_local()
	{
		DeleteCriticalSection(&m_critsec);
	}

	virtual void lock()
	{
		EnterCriticalSection(&m_critsec);
	}

	virtual void unlock()
	{
		LeaveCriticalSection(&m_critsec);
	}

private:
	CRITICAL_SECTION m_critsec;
};

#ifdef _HAS_POSIX_THREADS
// The multi threading policies only get compiled in if they are enabled.
class multi_threaded_global
{
public:
	multi_threaded_global()
	{
		pthread_mutex_init(get_mutex(), nullptr);
	}

	multi_threaded_global(const multi_threaded_global&)
	{
		;
	}

	virtual ~multi_threaded_global()
	{
		;
	}

	virtual void lock()
	{
		pthread_mutex_lock(get_mutex());
	}

	virtual void unlock()
	{
		pthread_mutex_unlock(get_mutex());
	}

private:
	pthread_mutex_t* get_mutex()
	{
		static pthread_mutex_t g_mutex;
		return &g_mutex;
	}
};

class multi_threaded_local
{
public:
	multi_threaded_local()
	{
		pthread_mutex_init(&m_mutex, nullptr);
	}

	multi_threaded_local(const multi_threaded_local&)
	{
		pthread_mutex_init(&m_mutex, nullptr);
	}

	virtual ~multi_threaded_local()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	virtual void lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	virtual void unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}

private:
	pthread_mutex_t m_mutex;
};
#endif // _HAS_POSIX_THREADS

template<class mt_policy>
class lock_block
{
public:
	mt_policy *m_mutex;

	lock_block(mt_policy *mtx)
		: m_mutex(mtx)
	{
		m_mutex->lock();
	}

	~lock_block()
	{
		m_mutex->unlock();
	}
};