#pragma once

#include "i3memory.h"

// 쓰레드 옵션은 정책을 세워야 한다...(반드시)

namespace i3
{
	namespace detail
	{
		class sp_counted_root : private noncopyable
		{
		public:			
			virtual ~sp_counted_root() {}
			virtual void	dispose() = 0;
			virtual void	destroy() { NetworkDump_Free(this,__FILE__,__LINE__); delete this; }
			virtual void*	get_deleter(const std::type_info& ti) = 0;
			virtual long	use_count() const = 0;
		protected:	
			sp_counted_root() : m_use_count(1), m_weak_count(1) {}
			long m_use_count;
			long m_weak_count;	// use_count존재시 1이 붙음...
		};

		class sp_counted_base_st : public sp_counted_root
		{
		public:
			void add_ref_copy() { ++m_use_count; }
			bool add_ref_lock() { if (m_use_count == 0) return false; ++m_use_count; return true; }
			void release()		{ if (--m_use_count == 0) { dispose(); weak_release(); } }
			void weak_add_ref() { ++m_weak_count; }
			void weak_release() { if (--m_weak_count == 0) destroy(); }
			virtual long use_count() const { return m_use_count; }
		};
		
		class sp_counted_base_mt : public sp_counted_root
		{
		public:
			void add_ref_copy() { ::InterlockedIncrement(&m_use_count); }
			bool add_ref_lock() 
			{
				for(;;)
				{
					long tmp = static_cast<const volatile long&>(m_use_count);
					if (tmp == 0) return false;
					if (::InterlockedCompareExchange(&m_use_count, tmp + 1, tmp) == tmp) return true;
				}
			}
			void release() { if ( ::InterlockedDecrement(&m_use_count) == 0 ) { dispose(); weak_release(); } }
			void weak_add_ref() { ::InterlockedIncrement(&m_weak_count); }
			void weak_release() { if ( ::InterlockedDecrement(&m_weak_count) == 0) destroy(); }
			virtual long use_count() const { return static_cast<const volatile long&>(m_use_count); }
		};
		
	}
}
