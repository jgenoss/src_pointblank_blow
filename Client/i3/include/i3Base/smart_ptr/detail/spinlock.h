#pragma once

extern "C" void _ReadWriteBarrier();
#pragma intrinsic(_ReadWriteBarrier)

#define COMPILER_FENCE _ReadWriteBarrier();

#if defined(_M_IX86) || defined(_M_X64)

extern "C" void _mm_pause();
#pragma intrinsic(_mm_pause)

#define SMT_PAUSE _mm_pause();

#endif

namespace i3
{
	namespace detail
	{
		inline void yield( unsigned k )
		{
			if ( k < 4) {}
			#if defined(SMT_PAUSE)
			else if (k < 16) SMT_PAUSE
			#endif
			else if (k < 32) Sleep(0);
			else Sleep(1);
		}

		class spinlock
		{
		public:
			long v_;

			bool try_lock()
			{
				long r = ::InterlockedExchange( &v_, 1 );
				// COMPILER_FENCE // 인터락함수가 메모리장벽역할을 해주지 않나??...
				return r == 0;
			}

			void lock() { for( unsigned k = 0; !try_lock() ; ++k) i3::detail::yield( k ); }

			void unlock()
			{
				COMPILER_FENCE 
				*const_cast< long volatile* >( &v_ ) = 0;
			}

			class scoped_lock
			{
			public:
				explicit scoped_lock( spinlock & sp ): sp_( sp ) { sp.lock(); }
				~scoped_lock() { sp_.unlock(); }
			private:
				spinlock & sp_;
		        scoped_lock( scoped_lock const & );
				scoped_lock & operator=( scoped_lock const & );
			};
		};
	}
}
