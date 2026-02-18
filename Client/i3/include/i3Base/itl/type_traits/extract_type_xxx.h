
#ifndef extract_type_xxx_h
#define extract_type_xxx_h

namespace i3
{
	namespace detail_extract_type
	{
		template<class T> struct exists { typedef void type; };	
	}
}

#endif  // extract_type_xxx_h

// no include guard
#define		pp_trait_name		pp_tuple_elem(2, 0, extract_type_xxx)
#define		pp_type_name		pp_tuple_elem(2, 1, extract_type_xxx)

#define		pp_trait_name_impl	pp_cat(pp_trait_name, _impl)

namespace i3
{

	template<class T, class Enable = void> 
	struct pp_trait_name_impl {};

	template<class T>
	struct pp_trait_name_impl<T, typename detail_extract_type::exists<typename T::pp_type_name>::type >
	{ 
		typedef typename T::pp_type_name type;
	}
	
	template<class T> struct pp_trait_name : pp_trait_name_impl<T> {};
}

#undef pp_trait_name_impl
#undef pp_type_name
#undef pp_trait_name
#undef extract_type_xxx
