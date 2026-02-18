

// no include guard
#define		bind_pp_name			pp_tuple_elem(2, 0, bind_operator_xxx)
#define		bind_pp_op				pp_tuple_elem(2, 1, bind_operator_xxx)

namespace i3
{
	namespace bi
	{
		struct bind_pp_name
		{
			template<class V, class W> bool operator()(const V& v, const W& w) const { return v bind_pp_op w; }
		};
		
		template<class R, class F, class L, class A2>
		bind_t<bool, bind_pp_name, list2<bind_t<R,F,L>, typename add_value<A2>::type > >
		operator bind_pp_op(const bind_t<R,F,L>& f, A2 a2)
		{
			typedef typename add_value<A2>::type B2;
			typedef list2< bind_t<R,F,L>, B2> list_type;
			return bind_t<bool, bind_pp_name, list_type>( bind_pp_name(), list_type(f, a2) ); 
		}
	}
}

#undef bind_pp_op
#undef bind_pp_name
#undef bind_operator_xxx
