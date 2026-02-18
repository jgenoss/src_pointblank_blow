#pragma once

#include "../itl/noncopyable.h"

namespace i3
{
	class cs 
	{
	public:
		// spincount는 0x00FFFFFF범위이며, 상위 바이트값은 현 OS에서 무시됨.
		cs(DWORD dwSpinCount = 1024)	{  ::InitializeCriticalSectionAndSpinCount( &m_cs, dwSpinCount ); }
		~cs()			{  ::DeleteCriticalSection( &m_cs ); }
		void lock()		{ ::EnterCriticalSection( &m_cs );  }
		void unlock()	{ ::LeaveCriticalSection( &m_cs ); }
	private:
		::CRITICAL_SECTION m_cs;
	};

	class scoped_cs : public i3::noncopyable
	{
	public:
		scoped_cs(cs& c) : m_cs(c) {  c.lock(); }
		~scoped_cs() { m_cs.unlock(); }
	private:
		cs&				m_cs;
	};
}
