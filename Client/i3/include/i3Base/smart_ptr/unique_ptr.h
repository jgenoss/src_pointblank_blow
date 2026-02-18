#pragma once

#include "../itl/enable_if.h"
#include "../itl/compile_assert.h"
#include "../itl/mpl/mpl_if.h"
#include "../itl/type_traits/remove_reference.h"

#include "../itl/type_traits/has_pointer.h"
#include "../itl/type_traits/is_array.h"

#include "../itl/compressed_pair.h"

#include "../itl/move/move.h"

// TODO : 2015버전업에 의해 유니크포인터와 move 구현이 변경되어야 함... ( 2015.11.03.수빈)

namespace i3
{
	template<class T> struct default_delete;

	template <class T, class D = default_delete<T> >
	class unique_ptr;

	template <class T>
	struct default_delete
	{
		default_delete() {}
		template <class U>
        default_delete(const default_delete<U>&,
			typename i3::enable_if<is_convertible<U*, T*> >::type* = 0) {}
		void operator()(T* ptr) const { compile_assert(sizeof(T) > 0); NetworkDump_Free( ptr, __FILE__, __LINE__); delete ptr; }
	};
	
	template <class T>
	struct default_delete<T[]>
	{
		void operator()(T* ptr) const { compile_assert(sizeof(T) > 0); NetworkDump_Free( ptr, __FILE__, __LINE__); delete [] ptr; }
	private:
		template <class U> void operator()(U*) const;
	};

	namespace detail_up
	{
		template<class T, class D, bool has_ptr = i3::has_pointer<D>::value>
		struct pointer_type_impl { typedef typename D::pointer type; };

		template<class T, class D>
		struct pointer_type_impl<T,D,false> { typedef T* type; };

		template<class T, class D>
		struct pointer_type
		{
			typedef typename pointer_type_impl<T, typename remove_reference<D>::type>::type type;
		};

		template <class T> struct unique_ptr_error;

		template <class T, class D>
		struct unique_ptr_error<const unique_ptr<T, D> >
		{
			typedef unique_ptr<T, D> type;
		};


	}


	template <class T, class D >
	class unique_ptr
	{
	public:
		typedef T element_type;
		typedef D deleter_type;
		typedef typename detail_up::pointer_type<element_type, deleter_type>::type pointer;

	private:
		I3_MOVABLE(unique_ptr)
		
		template <class U, class E> unique_ptr(unique_ptr<U, E>&);
		template <class U, class E> unique_ptr& operator=(unique_ptr<U, E>&);

		template <class U> unique_ptr(U&, typename detail_up::unique_ptr_error<U>::type = 0);
		template <class U> typename detail_up::unique_ptr_error<U>::type operator=(U&);

		i3::compressed_pair<pointer, deleter_type> m_ptr;

		typedef typename add_reference<deleter_type>::type deleter_reference;
		typedef typename add_reference<const deleter_type>::type deleter_const_reference;

		struct nat {int for_bool_;};
		struct nat2 { int for_nullptr; };
		typedef int nat2::*		nullptr_t;

	public:

		unique_ptr() : m_ptr(pointer())
		{
			compile_assert(!is_reference<deleter_type>::value);
			compile_assert(!is_pointer<deleter_type>::value);
		}

		explicit unique_ptr(pointer p) : m_ptr(p)
		{
			compile_assert(!is_reference<deleter_type>::value);
			compile_assert(!is_pointer<deleter_type>::value);
		}


		unique_ptr(pointer p, typename mpl::if_<is_reference<D>,
			D, typename add_reference<const D>::type>::type d) : m_ptr(p, d) {} 

		unique_ptr(rv_ref(unique_ptr) r) : m_ptr(r.release(), i3::forward<deleter_type>(r.get_deleter())) {}
		
		template <class U, class E>
		unique_ptr( i3::rv< unique_ptr<U, E> >& u, typename i3::enable_if_c
			<
			!i3::is_array<U>::value &&
			i3::is_convertible<typename unique_ptr<U>::pointer, pointer>::value &&
			i3::is_convertible<E, deleter_type>::value && 
			( !is_reference<deleter_type>::value || is_same<deleter_type, E>::value)
			>::type* = 0)
			: m_ptr(const_cast<unique_ptr<U,E> >(u).release(), i3::move<D>(u.get_deleter())) {}


		~unique_ptr() {reset();}

		unique_ptr& operator=(rv_ref(unique_ptr) r) { reset(r.release()); m_ptr.second() = i3::move(r.get_deleter()); return *this; }

		template <class U, class E>
		unique_ptr& operator=( i3::rv<unique_ptr<U, E> >& u) { reset(u.release());	m_ptr.second() = i3::move(u.get_deleter());	return *this; }

		unique_ptr& operator=(nullptr_t) { reset(); return *this; }

		typename add_reference<T>::type operator*() const {return *m_ptr.first();}
		pointer operator->() const {return m_ptr.first();}
		pointer get() const {return m_ptr.first();}

		deleter_reference       get_deleter()       {return m_ptr.second();}
		deleter_const_reference get_deleter() const {return m_ptr.second();}

		operator int nat::*() const {return m_ptr.first() ? &nat::for_bool_ : 0;}

		pointer release()
		{
			pointer tmp = m_ptr.first();
			m_ptr.first() = pointer();
			return tmp;
		}

		void reset(pointer p = pointer())
		{
			pointer t = m_ptr.first();
			if (t != pointer()) m_ptr.second()(t);
			m_ptr.first() = p;
		}

		void swap(unique_ptr& u) { i3::swap(m_ptr, u.m_ptr); }

	};


	template <class T, class D, std::size_t N>
	class unique_ptr<T[N], D>
	{
	public:
		typedef T element_type;
		typedef D deleter_type;
		typedef typename detail_up::pointer_type<element_type, deleter_type>::type pointer;

	private:
		I3_MOVABLE(unique_ptr)

		i3::compressed_pair<pointer, deleter_type> m_ptr;

		typedef typename add_reference<deleter_type>::type deleter_reference;
		typedef typename add_reference<const deleter_type>::type deleter_const_reference;

		struct nat {int for_bool_;};

		template <class U> explicit 
		unique_ptr(U, typename i3::enable_if_c<i3::is_convertible<U, pointer>::value>::type* = 0);

		template <class U, class E>
		unique_ptr(U, E,
			typename i3::enable_if_c<i3::is_convertible<U, pointer>::value>::type* = 0);

		template <class U> unique_ptr(U&, typename detail_up::unique_ptr_error<U>::type = 0);
		template <class U> typename detail_up::unique_ptr_error<U>::type operator=(U&);

	public:

		unique_ptr( rv_ref(unique_ptr) r) : m_ptr(r.release(), i3::forward<deleter_type>(r.get_deleter())) {}

		unique_ptr& operator=( rv_ref(unique_ptr) r){ reset(r.release());	m_ptr.second() = i3::move(r.get_deleter()); return *this; }

		unique_ptr() : m_ptr(pointer())
        {
            compile_assert(!is_reference<deleter_type>::value);
            compile_assert(!is_pointer<deleter_type>::value);
        }

		explicit unique_ptr(pointer p) : m_ptr(p)
        {
            compile_assert(!is_reference<deleter_type>::value);
            compile_assert(!is_pointer<deleter_type>::value);
        }

		unique_ptr(pointer p, typename mpl::if_<is_reference<D>, D, typename i3::add_reference<const D>::type >::type d)
			: m_ptr(p, d) {}

		~unique_ptr() {reset();}

		T& operator[](size_t i) const {return m_ptr.first()[i];}
		pointer get() const {return m_ptr.first();}
		deleter_reference       get_deleter()       {return m_ptr.second();}
		deleter_const_reference get_deleter() const {return m_ptr.second();}

		operator int nat::*() const {return get() ? &nat::for_bool_ : 0;}

		void reset(pointer p = pointer())
		{
			pointer t = m_ptr.first();
			if (t != pointer())
				m_ptr.second()(t);
			m_ptr.first() = p;
		}

		pointer release()
		{
			pointer tmp = m_ptr.first();
			m_ptr.first() = pointer();
			return tmp;
		}

		void swap(unique_ptr& u) { i3::swap(m_ptr, u.m_ptr);}

	};

	template<class T, class D> inline
	void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) { x.swap(y); }

	template<class T1, class D1, class T2, class D2> inline
	bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {	return x.get() == y.get(); }

	template<class T1, class D1, class T2, class D2> inline
	bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return !(x == y); }

	template<class T1, class D1, class T2, class D2> inline
	bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return x.get() < y.get(); }

	template<class T1, class D1, class T2, class D2> inline
	bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return !(y < x); }

	template<class T1, class D1, class T2, class D2> inline
	bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return y < x; }

	template<class T1, class D1, class T2, class D2> inline
	bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return !(x < y); }
	

}