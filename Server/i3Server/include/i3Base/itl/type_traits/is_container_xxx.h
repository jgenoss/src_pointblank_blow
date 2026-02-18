
// 컨테이너의 경우 컨테이너 이름을 걸고 직접 만듬.....

// no include guard

#define		pp_trait_name				pp_tuple_elem(3, 0, is_container_xxx)
#define		pp_full_container_name		pp_tuple_elem(3, 1, is_container_xxx)
#define		pp_type_arity				pp_tuple_elem(3, 2, is_container_xxx)

namespace i3
{
	template<class T>
	struct pp_trait_name : false_type {};

	template< pp_enum_params(pp_type_arity, class T) >
	struct pp_trait_name< pp_full_container_name< pp_enum_params(pp_type_arity, T) > > : true_type {};
}

#undef pp_type_arity
#undef pp_full_container_name
#undef pp_trait_name
#undef is_container_xxx

