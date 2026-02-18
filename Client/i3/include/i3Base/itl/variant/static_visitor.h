#pragma once

namespace i3
{
	namespace detail { struct is_static_visitor_tag {}; }

	template<class R = void>
	struct static_visitor : public detail::is_static_visitor_tag
	{
		typedef R result_type;		typedef R return_type;
	};

	template<class T> struct is_static_visitor : is_base_and_derived<detail::is_static_visitor_tag, T> {};

	template<class Visitor, class Visitable> __forceinline
	typename Visitor::result_type apply_visitor( Visitor& v, Visitable& obj) { return obj.apply_visitor(v); }
	
	template<class Visitor, class Visitable> __forceinline
	typename Visitor::result_type apply_visitor( const Visitor& v, Visitable& obj) { return obj.apply_visitor(v); }

	namespace detail
	{
		template<class Visitor, class Val1>
		struct apply_visitor_binary_invoke : noncopyable
		{
			typedef typename Visitor::result_type		result_type;
			apply_visitor_binary_invoke(Visitor& v, Val1& val1) : m_visit(v), m_val1(val1) {}
			
			template<class Val2>
			result_type operator()(Val2& val2) const { return m_visit(m_val1, val2); }
		private:
			Visitor& m_visit;	Val1& m_val1;
		};
		
		template<class Visitor, class Visitable2>
		struct apply_visitor_binary_unwrap : noncopyable
		{
			typedef typename Visitor::result_type	result_type;
			apply_visitor_binary_unwrap(Visitor& v, Visitable2& visitable2) : m_visit(v), m_visitable2(visitable2) {}

			template<class Val1>
			result_type operator()(Val1& val1) const
			{
				return i3::apply_visitor(apply_visitor_binary_invoke<Visitor, Val1>(m_visit, val1), m_visitable2);
			}
		private:
			Visitor& m_visit;
			Visitable2& m_visitable2;
		};
	}
	
	template<class Visitor, class Visitable1, class Visitable2> __forceinline
	typename Visitor::result_type apply_visitor( Visitor& visit, Visitable1& obj1, Visitable2& obj2)
	{
		return i3::apply_visitor(detail::apply_visitor_binary_unwrap<Visitor, Visitable2>(visit, obj2), obj1)	;
	}

	template<class Visitor, class Visitable1, class Visitable2> __forceinline
	typename Visitor::result_type apply_visitor(const Visitor& visit, Visitable1& obj1, Visitable2& obj2)
	{
		return i3::apply_visitor(detail::apply_visitor_binary_unwrap<const Visitor, Visitable2>(visit, obj2), obj1)	;
	}

	template<class Visitor>
	struct apply_visitor_t
	{
		typedef typename Visitor::result_type		result_type;
		explicit apply_visitor_t(Visitor& v) : m_visit(v) {}
		template<class Visitable>
		result_type operator()(Visitable& obj) { return i3::apply_visitor(m_visit, obj); }
		template<class Visitable1, class Visitable2>
		result_type operator()(Visitable1& obj1, Visitable2& obj2) { return i3::apply_visitor(m_visit, obj1, obj2); }
	private:
		Visitor&	m_visit;
	};

	template<class Visitor> __forceinline
	apply_visitor_t<Visitor>	apply_visitor(Visitor& v) { return apply_visitor_t<Visitor>(v); }

	

}