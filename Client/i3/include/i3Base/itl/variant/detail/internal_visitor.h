#pragma once

#include "type_accessor.h"

namespace i3
{
	namespace detail
	{
	

		struct internal_clone_visitor : static_visitor<>
		{
			internal_clone_visitor(const void* src_data, void* dest_data) : src_data(src_data), dest_data(dest_data) {}
			const void* src_data; void* dest_data;

			template<class T>
			void operator()(const i3::identity<T>& ) const
			{
				detail::type_accessor<T>::construct( dest_data, detail::type_accessor<T>::get(src_data));
			}
		};

		struct internal_clone_type_visitor : static_visitor<>
		{
			internal_clone_type_visitor( void* dest_data) : dest_data(dest_data) {}
			void* dest_data;

			template<class T>
			void operator()(const i3::identity<T>& ) const
			{
				detail::type_accessor<T>::construct( dest_data);
			}
		};

		struct internal_copy_visitor : static_visitor<>
		{
			internal_copy_visitor(const void* src_data, void* dest_data) : src_data(src_data), dest_data(dest_data) {}
			const void* src_data; void* dest_data;

			template<class T>
			void operator()(const i3::identity<T>& ) const
			{
				detail::type_accessor<T>::copy(
					dest_data, detail::type_accessor<T>::get(src_data));
			}
		};

		struct internal_destroy_visitor : static_visitor<>
		{
			void* data;
			internal_destroy_visitor(void* data) : data(data) {}
			template<class T> void operator()(const i3::identity<T>&) const 
			{	
				detail::type_accessor<T>::destroy(data); 
			}
		};

		struct internal_swap_visitor : static_visitor<>
		{
			internal_swap_visitor(void* src_data, void* dest_data) : src_data(src_data), dest_data(dest_data) {}
			void* src_data; void* dest_data;

			template<class T>
			void operator()(const i3::identity<T>& ) const
			{
				detail::type_accessor<T>::swap(src_data, dest_data);
			}
		};


		struct internal_reset_visitor : static_visitor<>
		{
			internal_reset_visitor(void* data) : data(data) {} void* data;

			template<class T>
			void operator()(const i3::identity<T>&) const
			{
				type_accessor<T>::reset(data);
			}
		};

		struct internal_typeid_visitor : static_visitor<const std::type_info&>
		{
			template<class T>
			const std::type_info& operator()(const i3::identity<T>&) const
			{
				return typeid(T);
			}
		};

		struct internal_equal_visitor : static_visitor<bool>
		{
			internal_equal_visitor(const void* src_data, const void* dest_data) : src_data(src_data), dest_data(dest_data) {}
			const void* src_data,* dest_data;
			template<class T>
			bool operator()(const i3::identity<T>&) const
			{
				return detail::type_accessor<T>::get( src_data) == detail::type_accessor<T>::get(dest_data);
			}
		};

		struct internal_less_visitor : static_visitor<bool>
		{
			internal_less_visitor(const void* src_data, const void* dest_data) : src_data(src_data), dest_data(dest_data) {}
			const void* src_data,* dest_data;
			template<class T>
			bool operator()(const i3::identity<T>&) const
			{
				return detail::type_accessor<T>::get( src_data) < detail::type_accessor<T>::get(dest_data);
			}
		};

		template<class TargetType>
		struct internal_change_type_visitor : static_visitor<bool>
		{
			internal_change_type_visitor(void* data) : data(data) {}		void* data;

			template<class T> bool operator()(const i3::identity<T>& ) const 
			{  
				return detail::type_accessor<T>::convert<TargetType>(data );  
			}

			template<class T> bool operator()(const T& ) const { return false; }
		};


#pragma push_macro("new")
#undef new

		template<class Variant>
		struct construct_variant_visitor : static_visitor<>
		{
			construct_variant_visitor(Variant* dest) : m_dest(dest) {}
			Variant*									m_dest;		// 사본..
			template<class T> void operator()(const T& val) const {  new (m_dest) Variant(val);    }
			template<class T> void operator()(T& val) const {  new (m_dest) Variant(val);    }
		};
#pragma pop_macro("new")

		template<class Variant>
		struct copy_variant_visitor : static_visitor<>
		{
			copy_variant_visitor(Variant* dest) : m_dest(dest) {}
			Variant*									m_dest;		//사본..	
			template<class T> void	operator()(const T& val) const {  *m_dest = val;  }
			template<class T> void	operator()(T& val) const {  *m_dest = val;  }
		};		




	}

}
