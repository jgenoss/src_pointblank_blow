#pragma once

namespace i3
{
	
	namespace detail
	{
		struct get_name_from_non_class {		static const char* call(const char* name) { return name; }  };
		struct get_name_from_class {			static const char* call(const char* name) { name+=6; if (*name ==' ') ++name; return name; }   };
	}

	template<class T> inline
	const char*		get_class_name() 
	{
		const char* class_full_name = typeid(T).name();	
		typedef typename mpl::if_< i3::is_class<T>, 
			detail::get_name_from_class, detail::get_name_from_non_class>::type		fun_type;
		return fun_type::call(class_full_name);
	}
}

