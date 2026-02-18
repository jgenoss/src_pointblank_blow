#pragma once

//
// result_type 제거... 없으면 다른곳의 디폴트타입이 적용될수 있도록 알아서 처리되어야한다.
// 

namespace i3
{
	namespace sig				// 
	{
		template<class T>
		struct last_value
		{
			template<class It> inline
			T operator()(It f, It l) const
			{
				i3::optional<T> val;

				for ( ; f != l ; ++f)
					val = *f;
				
				return (val) ? val.get() : T();
			}
		};
		
		template<>
		struct last_value<void>
		{
			template<class It> inline
			void operator()(It f, It l) const
			{
				for ( ; f != l ; ++f )
					*f;
			}
		};

	}
}
