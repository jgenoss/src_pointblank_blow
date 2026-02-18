#if !defined(pp_is_iterating)

#ifndef user_data_h
#define user_data_h

#include "any.h"
#include "tuple/tuple.h"
#include "move/move.h"
#include "type_traits/make_reference_result.h"
#include "type_traits/make_const_reference_result.h"

namespace i3
{

#define user_data_arity_limit		10

	typedef i3::shared_ptr< class user_data >	user_data_ptr;

	class user_data 
	{
		I3_COPYABLE_MOVABLE(user_data)
	public:
		user_data() {}
		
		user_data(const user_data& rhs) : m_any(rhs.m_any) {}

		user_data(rv_ref(user_data) rval) 
		{
			m_any.move(rval.m_any);
		}

		user_data&	operator=(const_assign_ref(user_data) rhs) 
		{
			m_any = static_cast<const user_data&>(rhs);	return *this;
		}
		
		user_data&	operator=(rv_ref(user_data) rval) 
		{
			m_any.move(rval.m_any);	return *this;
		}
		
		void		move(user_data& rhs)
		{
			m_any.move(rhs.m_any);
		}

		void swap(user_data& rhs) { m_any.swap(rhs.m_any); }
		void clear() { m_any.clear(); }
		bool empty() const { return m_any.empty(); }

		template<class P1>
		user_data(const P1& p1 ) : m_any(p1) {}			// 1개에 대해서는 튜플을 쓰지 않는다.
		
		template<class P1, class P2>
		user_data(const P1& p1, const P2& p2) : m_any( i3::make_tuple(p1, p2) ) {}

// 나머지 반복은 프리프로세서+매크로조합으로 한다..		
		#define repeat_user_data_cons(z, n, d) \
		template<pp_enum_params_1(n, class P)> \
		user_data( pp_enum_params_1_cat_params(n, const P, &, p) );
		pp_repeat_from_to(3, pp_inc(user_data_arity_limit), repeat_user_data_cons, ~)
		#undef	repeat_user_data_cons
		

		template<class P1>
		void assign(const P1& p1) { m_any = p1; }

		template<class P1, class P2>
		void assign(const P1& p1, const P2& p2) { m_any = i3::make_tuple(p1, p2); }

		#define repeat_user_data_assign(z, n, d) \
		template<pp_enum_params_1(n, class P)> \
		void assign( pp_enum_params_1_cat_params(n, const P, &, p) );
		pp_repeat_from_to(3, pp_inc(user_data_arity_limit), repeat_user_data_assign, ~)
		#undef	repeat_user_data_assign
		

		template<class P1> 
		void		extract(P1& p1) const {  p1 = *i3::unsafe_any_cast<P1>(&m_any); }
		
		template<class P1>
		void		extract_ptr(P1*& p1) { p1 = i3::unsafe_any_cast<P1>(&m_any); }
		
		template<class P1, class P2>
		void		extract(P1& p1, P2& p2) const 
		{ 
			const i3::tuple<P1, P2>& casted = *i3::unsafe_any_cast< i3::tuple<P1, P2> >(&m_any); 
			p1 = casted.get<0>();
			p2 = casted.get<1>();
		}
		
		template<class P1, class P2>
		void		extract_ptr(P1*& p1, P2*& p2) 
		{ 
			const i3::tuple<P1, P2>& casted = *i3::unsafe_any_cast< i3::tuple<P1, P2> >(&m_any); 
			p1 = &casted.get<0>();
			p2 = &casted.get<1>();
		}
	
		#define repeat_user_data_extract(z, n, d) \
		template<pp_enum_params_1(n, class P)> \
		void		extract( pp_enum_params_1_cat_params(n, P, &, p) ) const; \
		template<pp_enum_params_1(n, class P)> \
		void		extract_ptr( pp_enum_params_1_cat_params(n, P, *&, p) ) const;

		pp_repeat_from_to(3, pp_inc(user_data_arity_limit), repeat_user_data_extract, ~)
		#undef	repeat_user_data_extract

		template<class P1>
		typename make_const_reference_result<P1>::type value_cast() const
		{
			return *i3::unsafe_any_cast<P1>(&m_any);
		}
		
		template<class P1>
		typename make_reference_result<P1>::type value_cast() 
		{
			return *i3::unsafe_any_cast<P1>(&m_any);
		}
		
		template<class P1, class P2>
		typename make_const_reference_result< i3::tuple<P1,P2> >::type tuple_cast() const
		{
			return *i3::unsafe_any_cast<i3::tuple<P1,P2> >(&m_any);
		}
		template<class P1, class P2>
		typename make_reference_result< i3::tuple<P1,P2> >::type tuple_cast() 
		{
			return *i3::unsafe_any_cast<i3::tuple<P1,P2> >(&m_any);
		}

		#define repeat_user_data_tuple_cast(z, n, d) \
		template<pp_enum_params_1(n, class P)> \
		typename make_const_reference_result< i3::tuple<pp_enum_params_1(n, P)> >::type tuple_cast() const; \
		template<pp_enum_params_1(n, class P)> \
		typename make_reference_result<i3::tuple<pp_enum_params_1(n, P)> >::type tuple_cast();
		pp_repeat_from_to(3, pp_inc(user_data_arity_limit), repeat_user_data_tuple_cast, ~)
		#undef	repeat_user_data_tuple_cast



	private:
		i3::any		m_any;	
	};

	inline void swap(user_data& lhs, user_data& rhs) 
	{
		lhs.swap(rhs);
	}

	template<class P1> inline
	typename make_const_reference_result<P1>::type value_cast(const user_data& d)
	{
		return	d.value_cast<P1>();
	}
		
	template<class P1> inline
	typename make_reference_result<P1>::type value_cast(user_data& d) 
	{
		return d.value_cast<P1>();
	}

	template<class P1, class P2> inline
	typename make_const_reference_result< i3::tuple<P1,P2> >::type tuple_cast(const user_data& d)
	{
		return d.tuple_cast<P1,P2>();
	}
	
	template<class P1, class P2> inline
	typename make_reference_result< i3::tuple<P1,P2> >::type tuple_cast(user_data& d) 
	{
		return d.tuple_cast<P1,P2>();
	}
	
}

#define		user_data_extract_implementation			0x01
#define		user_data_extract_ptr_implementation		0x02	

#define		pp_iteration_params_1	(3, (3, user_data_arity_limit, "user_data.h"))				// 1단계 파일반복..
#include	pp_iterate()

#endif		

#else		// pp_is_iterating

#if			pp_iteration_depth() == 1

#define i_				pp_frame_iteration(1)
	
namespace i3 
{
	template< pp_enum_params_1(i_, class P)> inline
	user_data::user_data( pp_enum_params_1_cat_params(i_, const P, &, p) ) : m_any( i3::make_tuple( pp_enum_params_1(i_, p) ) )
	{
	}
	
	template< pp_enum_params_1(i_, class P)> inline
	void user_data::assign( pp_enum_params_1_cat_params(i_, const P, &, p) )
	{
		m_any = i3::make_tuple( pp_enum_params_1(i_, p) );
	}

	template<pp_enum_params_1(i_, class P)> inline
	void	user_data::extract(pp_enum_params_1_cat_params(i_, P, &, p)) const 
	{ 
		const i3::tuple<pp_enum_params_1(i_, P)>& casted = 
			*i3::unsafe_any_cast< i3::tuple<pp_enum_params_1(i_, P)> >(&m_any); 

#define		pp_iteration_params_2  (4, (1, i_, "user_data.h", user_data_extract_implementation) )
#include	pp_iterate()

	}
	
	template<pp_enum_params_1(i_, class P)> inline
	void	user_data::extract_ptr(pp_enum_params_1_cat_params(i_, P, *&, p)) const 
	{ 
		const i3::tuple<pp_enum_params_1(i_, P)>& casted = 
			*i3::unsafe_any_cast< i3::tuple<pp_enum_params_1(i_, P)> >(&m_any); 

#define		pp_iteration_params_2  (4, (1, i_, "user_data.h", user_data_extract_ptr_implementation) )
#include	pp_iterate()
	}

	template<pp_enum_params_1(i_, class P)> inline
	typename make_const_reference_result< i3::tuple<pp_enum_params_1(i_, P)> >::type user_data::tuple_cast() const
	{
		return *i3::unsafe_any_cast<i3::tuple<pp_enum_params_1(i_, P)> >(&m_any);
	}
	template<pp_enum_params_1(i_, class P)> inline
	typename make_reference_result< i3::tuple<pp_enum_params_1(i_, P)> >::type user_data::tuple_cast() 
	{
		return *i3::unsafe_any_cast<i3::tuple<pp_enum_params_1(i_, P)> >(&m_any);
	}

	template<pp_enum_params_1(i_, class P)> inline
	typename make_const_reference_result< i3::tuple<pp_enum_params_1(i_, P)> >::type tuple_cast(const user_data& d)
	{
		return d.tuple_cast<pp_enum_params_1(i_, P)>();
	}
	
	template<pp_enum_params_1(i_, class P)> inline
	typename make_reference_result< i3::tuple<pp_enum_params_1(i_, P)> >::type tuple_cast(user_data& d) 
	{
		return d.tuple_cast<pp_enum_params_1(i_, P)>();
	}


}
	
#undef i_

#elif			pp_iteration_depth() == 2

#define	j_			pp_frame_iteration(2)
	
#if			pp_iteration_flags() == user_data_extract_implementation

	pp_cat(p, j_) = casted.get<pp_dec(j_)>();

#elif		pp_iteration_flags() == user_data_extract_ptr_implementation
	
	pp_cat(p, j_) = &casted.get<pp_dec(j_)>();

#endif

#undef j_

#endif // pp_iteration_depth()

#endif // pp_is_iterating
