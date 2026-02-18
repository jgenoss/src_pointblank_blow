#if !defined(pp_is_iterating)

#ifndef make_shared_h
#define make_shared_h

//
//  boost현 버전에 이런것도 존재...(생성할때, 타입생성과 스마트포인터내부데이터를 합친 메모리를 할당해 생성하고 소거함)
//  문서상으로는 명시적인 new의 사용을 제거하는 목적도 있음..(그럴만한 가치는 각자의 판단)
//  preprocessor 처리를 해야함...(이건 내일...)

#include "detail/sp_aligned_storage.h"
#include "shared_ptr.h"
#include "../itl/preprocessor/ext/pp_enum_params_1_cat_params.h"

#define  make_shared_limit		10

#pragma push_macro("new")
#undef new

namespace i3
{
	namespace detail
	{
		template<class> class sp_counted_impl_ms;
	}


// 아래는 dbg_util을 끄는 특화..
	template<class T> 
	struct enable_pool_dbg< detail::sp_counted_impl_ms<T> > : enable_pool_dbg<T> {};


	namespace detail
	{
	
	
		// 아래의 정규 메모리할당은 static_pool로 정함..(i3컨테이너의 표준과 같으며...dbg_util의 온오프가 가능하도록 처리...)

		template<class T> 
		class sp_counted_impl_ms : public select_sp_count_base<T>::type
		{
		public:
			typedef sp_counted_impl_ms<T> this_type;
			typedef typename select_sp_count_base<T>::type	base_type;

			this_type() {}		// 아무것도 초기화하지 않음..

			virtual void	dispose() { reinterpret_cast<T*>(m_storage.data)->~T(); }
			virtual void*	get_deleter(const std::type_info&) { return 0; }
			void*			address() { return m_storage.data; }

			void* operator new(std::size_t) { return static_pool_type_malloc( identity<this_type>(), sizeof(this_type) ); }
			void  operator delete(void* p) { static_pool_type_free(identity<this_type>(), p); }
			
		private:
			typedef typename sp_aligned_storage<sizeof(T), alignment_of<T>::value >::type storage_type;
			storage_type m_storage;		// 선형메모리..
		};
				
	}
	
	template<class T>
	shared_ptr<T>	make_shared()
	{
		detail::sp_counted_impl_ms<T>* counted_impl = new detail::sp_counted_impl_ms<T>();
		NetworkDump_Malloc( counted_impl, sizeof( detail::sp_counted_impl_ms<T>), __FILE__, __LINE__);
		T* pt = ::new (counted_impl->address()) T;
		return shared_ptr<T>( detail::sp_counted_base_tag(), counted_impl, pt);
	}
}


#define		pp_iteration_params_1	(3, (1, make_shared_limit, "make_shared.h"))		// 0 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()

#pragma pop_macro("new")

#endif // make_shared_h

#else // pp_is_iterating

#define		i_			pp_frame_iteration(1)

namespace i3
{
	template<class T, pp_enum_params_1(i_, class P) >
	shared_ptr<T>	make_shared( pp_enum_params_1_cat_params(i_, const P,&, p) )
	{
		detail::sp_counted_impl_ms<T>* counted_impl = new detail::sp_counted_impl_ms<T>();	
		T* pt = ::new (counted_impl->address()) T( pp_enum_params_1(i_, p) );
		return shared_ptr<T>(detail::sp_counted_base_tag(), counted_impl, pt);
	}
}

#undef i_

#endif // pp_is_iterating

