#pragma once

#include "../type_traits/add_pointer.h"
#include "../type_traits/make_nonconst_referred.h"

namespace i3
{

	class any_function_return
	{
	public:
		template<class T> operator T& () const {	return *reinterpret_cast<T*>(get_data()); }
		virtual ~any_function_return()	{}			
	private:
		virtual void*		get_data() const = 0;
	};

	namespace detail
	{

		template <typename T>
		class any_function_return_template : public any_function_return
		{
		public:
		typedef typename i3::remove_cv<T>::type				held_type;
			any_function_return_template() : m_held() {}
			any_function_return_template(const T& rhs) : m_held(rhs)	{ }	
			any_function_return_template& operator=(const T& rhs){	m_held = rhs;  return *this; }
		private:
			virtual void*			get_data() const {  return const_cast<held_type*>(&m_held);   }
			held_type				m_held;
		};

	// 레퍼런스의 경우 미리 값을 확보하는게 불가능하지만...
	// 포인터를 써서 접근하면 가능하다..

		template <typename T>
		class any_function_return_template<T&> : public any_function_return
		{
		public:
			typedef	typename add_pointer<typename i3::make_nonconst_referred<T>::type>::type	held_type;

			any_function_return_template() : m_held(NULL) {}
			any_function_return_template(const T& rhs) : m_held(const_cast<held_type>(&rhs) ) {}	
			any_function_return_template& operator=(const T& rhs){	m_held = const_cast<held_type>(&rhs);  return *this; }
		private:
			virtual void*			get_data() const {  return	m_held;   }
			held_type				m_held;
		};

		template <>
		class any_function_return_template<void> : public any_function_return
		{
		public:
			any_function_return_template(){}
		private:
			virtual void*			get_data() const { return NULL; }
		};

		typedef any_function_return_template<void>		any_function_return_void;
	}

}