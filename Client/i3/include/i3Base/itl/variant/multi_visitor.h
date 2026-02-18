#if !defined(pp_is_iterating)

#ifndef multi_visitor_h
#define multi_visitor_h

#define multi_visitor_limit			10

#include "../bind/bind.h"
#include "../bind/placeholders.h"
#include "../utility/ref.h"

#define two_visitable_holder_flag			0x00
#define apply_visitor_flag					0x01

namespace i3
{
	namespace detail { namespace variant 
	{
		template<class Visitor, class Visitable1, class Visitable2>
		struct two_visitable_holder 
		{
		public:
			typedef typename Visitor::result_type	result_type;

			two_visitable_holder(Visitor& visit, Visitable1& obj1, Visitable2& obj2) : 
				m_visit(visit), m_obj1(obj1), m_obj2(obj2) {}
			

#define pp_iteration_params_1 (4, (3, multi_visitor_limit, "multi_visitor.h", two_visitable_holder_flag) )
#include pp_iterate()
						
		private:
			Visitor&  m_visit;
			Visitable1& m_obj1;
			Visitable2& m_obj2;

			two_visitable_holder& operator=(const two_visitable_holder&);
		};
		
		template<class Visitor, class Visitable1, class Visitable2> inline
		two_visitable_holder<Visitor, Visitable1, Visitable2> make_two_visitable_holder( Visitor& visit, Visitable1& obj1, Visitable2& obj2)
		{
			return two_visitable_holder<Visitor, Visitable1, Visitable2>(visit, obj1, obj2);
		}
		
		template<class Visitor, class Visitable1, class Visitable2> inline
		two_visitable_holder<const Visitor, Visitable1, Visitable2> make_two_visitable_holder( const Visitor& visit, Visitable1& obj1, Visitable2& obj2)
		{
			return two_visitable_holder<const Visitor, Visitable1, Visitable2>(visit, obj1, obj2);
		}

	} }

}

#define pp_iteration_params_1 (4, (3, multi_visitor_limit, "multi_visitor.h", apply_visitor_flag) )
#include pp_iterate()


#undef apply_visitor_flag
#undef two_visitable_holder_flag

#endif

#else // pp_is_iterating
	
#define i_		pp_frame_iteration(1)
#define i_inc	pp_inc(i_)

#if			pp_iteration_flags() == two_visitable_holder_flag

		template< pp_enum_params_from_to(3, i_inc, class VisitableUnwrapped)>
		result_type operator()( pp_enum_binary_params_from_to(3, i_inc, VisitableUnwrapped, & vis) ) const
		{
			
			return i3::apply_visitor( i3::bind( i3::ref(m_visit), i3::_1, i3::_2
	
#define	repeat_apply_visitor_params(z, n, d)	pp_comma() i3::ref( pp_cat(vis, n) )
				pp_repeat_from_to(3, i_inc, repeat_apply_visitor_params, ~)
#undef  repeat_apply_visitor_params
					
				), m_obj1, m_obj2);
		}
				

#elif			pp_iteration_flags() == apply_visitor_flag	

namespace i3
{
	template<class Visitor, pp_enum_params_1( i_, class T)> inline
	typename Visitor::result_type apply_visitor( const Visitor& visit, pp_enum_binary_params_1(i_, T, & obj) )
	{
		return i3::apply_visitor( detail::variant::make_two_visitable_holder(visit, obj1, obj2),
				pp_enum_params_from_to(3, i_inc, obj) );
	}

	template<class Visitor, pp_enum_params_1( i_, class T)> inline
	typename Visitor::result_type apply_visitor( Visitor& visit, pp_enum_binary_params_1(i_, T, & obj) )
	{
		return i3::apply_visitor( detail::variant::make_two_visitable_holder(visit, obj1, obj2),
			pp_enum_params_from_to(3, i_inc, obj) );
	}



}

#endif		// 	pp_iteration_flags() 

#undef i_inc
#undef i_

#endif // pp_is_iterating
