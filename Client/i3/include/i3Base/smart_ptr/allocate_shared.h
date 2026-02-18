#if !defined(pp_is_iterating)

#ifndef allocate_shared_h
#define allocate_shared_h

#include "detail/sp_aligned_storage.h"
#include "shared_ptr.h"
#include "../itl/preprocessor/ext/pp_enum_params_1_cat_params.h"

#define  allocate_shared_limit		10

namespace i3
{
	namespace detail
	{
		template<class, class> class sp_counted_impl_msa;
	}
	
// 아래는 dbg_util을 끄는 특화..
	template<class T, class A> 
	struct enable_pool_dbg< detail::sp_counted_impl_msa<T, A> > : enable_pool_dbg<T> {};
	
	namespace detail
	{
		// pda의 경우 얼로케이터가 따로 주어지고, 바깥에서 할당되고, 삭제는 내부에서 처리..
		template<class T, class A>
		class sp_counted_impl_msa : public select_sp_count_base<T>::type
		{
		public:
			typedef sp_counted_impl_msa<T, A>				this_type;
			typedef typename select_sp_count_base<T>::type	base_type;

			typedef typename rebind_wrap<A, this_type>::type A2;

			this_type(A a) : m_cpair(a) {}
				
			virtual void	dispose() { reinterpret_cast<T*>(storage_data())->~T(); }
			virtual void	destroy() {	this->~this_type();		alloc().deallocate(this, 1); }
			virtual void*	get_deleter(const std::type_info&) { return 0; }
			void*			address() { return storage_data(); }

		private:
			void*			storage_data()			{ return m_cpair.first().data; }
			const void*		storage_data() const	{ return m_cpair.first().data; }
			A2&				alloc()			{ return m_cpair.second(); }
			const A2&		alloc() const	{ return m_cpair.second(); }
			
			typedef typename sp_aligned_storage<sizeof(T), alignment_of<T>::value >::type storage_type;
			compressed_pair< storage_type, A2> m_cpair;			
		};
	}


	template<class T, class A>
	shared_ptr<T>	allocate_shared(const A& a)
	{
		typedef typename i3::rebind_wrap<A, detail::sp_counted_impl_msa<T,A> >::type		A2;
		detail::sp_counted_impl_msa<T, A>* counted_impl = ::new (A2().allocate(1)) detail::sp_counted_impl_msa<T,A>(a);
		T* pt = ::new (counted_impl->address()) T();
		return shared_ptr<T>(detail::sp_counted_base_tag(), counted_impl, pt);
	};

}


#define		pp_iteration_params_1	(3, (1, allocate_shared_limit, "allocate_shared.h"))		// 0 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()

#endif // allocate_shared_h

#else // pp_is_iterating

#define		i_			pp_frame_iteration(1)

namespace i3
{
	template<class T, class A, pp_enum_params_1(i_, class P) >
	shared_ptr<T>	allocate_shared(const A& a, pp_enum_params_1_cat_params(i_, const P,&, p) )
	{
		typedef typename i3::rebind_wrap<A, detail::sp_counted_impl_msa<T,A> >::type		A2;
		detail::sp_counted_impl_msa<T, A>* counted_impl = ::new (A2().allocate(1)) detail::sp_counted_impl_msa<T,A>(a);
		T* pt = ::new (counted_impl->address()) T( pp_enum_params_1(i_, p) );
		return shared_ptr<T>(detail::sp_counted_base_tag(), counted_impl, pt);
	}
}

#undef i_

#endif // pp_is_iterating
