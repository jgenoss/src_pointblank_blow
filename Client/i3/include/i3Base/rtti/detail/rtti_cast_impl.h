#pragma once

namespace i3
{
	namespace detail
	{
	

		template<class T, class Tag>
		struct get_static_rtti_info;

		template<class T>
		struct get_static_rtti_info<T, rtti_available_tag>
		{
			typedef	i3::rtti_info*			result_type;
			static result_type	call() { return T::static_rtti(); }	
		};
		
		template<class T>
		struct get_static_rtti_info<T, class_meta_available_tag>
		{
			typedef	i3::class_meta*			result_type;
			static result_type	call() { return T::static_meta(); }
		};

		template<class T>
		struct get_static_rtti_info<T, rtti_na_tag>
		{
			typedef i3::null_type*			result_type;
			static result_type	call() { compile_assert_msg(false, compare_type_has_not_rtti_or_meta);	return nullptr; }
		};
		
		template<class T, class Tag>
		struct get_dynamic_rtti_info;
		
		template<class T>
		struct get_dynamic_rtti_info<T, rtti_available_tag>
		{
			typedef	i3::rtti_info*			result_type;
			static result_type	call(const T* p) { return p->get_rtti(); }	
		};

		template<class T>
		struct get_dynamic_rtti_info<T, class_meta_available_tag>
		{
			typedef	i3::class_meta*			result_type;
			static result_type	call(const T* p) { return p->meta(); }
		};

		template<class T>
		struct get_dynamic_rtti_info<T, rtti_na_tag>
		{
			typedef i3::null_type*			result_type;
			static result_type	call(const T* ) { compile_assert_msg(false, src_type_has_not_rtti_or_meta);	return nullptr; }
		};
		
		
	}
}
