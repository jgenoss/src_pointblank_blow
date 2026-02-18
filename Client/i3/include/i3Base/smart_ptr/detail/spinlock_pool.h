#pragma once

#include "spinlock.h"

namespace i3
{
	namespace detail
	{
			
		template< int I > 
		class spinlock_pool
		{
		public:
			static spinlock & spinlock_for( void const * pv )
			{
				std::size_t i = reinterpret_cast< std::size_t >( pv ) % 41;
				return pool_[ i ];
			}

			class scoped_lock
			{
			public:
				explicit scoped_lock( void const * pv ): sp_( spinlock_for( pv ) ) { sp_.lock(); }
				~scoped_lock() { sp_.unlock(); }
			private:
				spinlock & sp_;
				scoped_lock( scoped_lock const & );
				scoped_lock & operator=( scoped_lock const & );
			};
		private:
			static spinlock pool_[ 41 ];
		};

		template< int I > 
		spinlock spinlock_pool< I >::pool_[ 41 ] =
		{
			{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
			{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
			{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
			{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
			{0}
		};
		
	}
}