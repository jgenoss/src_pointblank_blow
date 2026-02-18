#pragma once

//#include <xutility>

namespace i3 
{
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};	
	struct bidirectional_iterator_tag : public forward_iterator_tag {}; 
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};
	struct integral_iterator_tag {};
	struct floating_point_iterator_tag {};	
	
	struct undefined_inner_type {};

	//
	struct iterator_base
	{
		typedef undefined_inner_type	inner_type;
	};
	
	template<class Category, class T, class Diff = ptrdiff_t, class Pointer = T*, class Reference = T&, class InnerType = undefined_inner_type>
	struct iterator : public iterator_base
	{	
		typedef T			value_type;
		typedef Diff		difference_type;
		typedef Pointer		pointer;
		typedef Reference	reference;
		typedef Category	iterator_category;
		typedef InnerType	inner_type;				// 비표준.. (copy/move를 위해 MSVC쪽 참고)
	};
	
	template<class Iterator>
	struct iterator_traits
	{	
		typedef typename Iterator::value_type				value_type;
		typedef typename Iterator::difference_type			difference_type;
		typedef typename Iterator::pointer					pointer;
		typedef typename Iterator::reference				reference;
		typedef typename Iterator::iterator_category		iterator_category;
	};
	
	template<class T>
	struct iterator_traits<T*>
	{	
		typedef T								value_type;
		typedef ptrdiff_t						difference_type;
		typedef T*								pointer;
		typedef T&								reference;
		typedef random_access_iterator_tag		iterator_category;
	};
	
	template<class T>
	struct iterator_traits<const T*>
	{	
		typedef T								value_type;
		typedef ptrdiff_t						difference_type;
		typedef const T*						pointer;
		typedef const T&						reference;
		typedef random_access_iterator_tag		iterator_category;
	};
	
	template<> struct iterator_traits<bool>
	{	
		typedef bool							value_type;
		typedef bool							difference_type;
		typedef bool*							pointer;
		typedef bool&							reference;
		typedef integral_iterator_tag			iterator_category;
	};
	
	template<> struct iterator_traits<char>
	{	
		typedef char value_type;
		typedef char difference_type;
		typedef char * pointer;
		typedef char& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<signed char>
	{	
		typedef signed char value_type;
		typedef signed char difference_type;
		typedef signed char * pointer;
		typedef signed char& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<unsigned char>
	{	
		typedef unsigned char value_type;
		typedef unsigned char difference_type;
		typedef unsigned char * pointer;
		typedef unsigned char& reference;
		typedef integral_iterator_tag iterator_category;
	};

#ifdef _NATIVE_WCHAR_T_DEFINED
	template<> struct iterator_traits<wchar_t>
	{	
		typedef wchar_t value_type;
		typedef wchar_t difference_type;
		typedef wchar_t * pointer;
		typedef wchar_t& reference;
		typedef integral_iterator_tag iterator_category;
	};
#endif

	template<> struct iterator_traits<short>
	{	
		typedef short value_type;
		typedef short difference_type;
		typedef short * pointer;
		typedef short& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<unsigned short>
	{	
		typedef unsigned short value_type;
		typedef unsigned short difference_type;
		typedef unsigned short * pointer;
		typedef unsigned short& reference;
		typedef integral_iterator_tag iterator_category;
	};


	template<> struct iterator_traits<int>
	{	
		typedef int value_type;
		typedef int difference_type;
		typedef int * pointer;
		typedef int& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<unsigned int>
	{	
		typedef unsigned int value_type;
		typedef unsigned int difference_type;
		typedef unsigned int * pointer;
		typedef unsigned int& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<long>
	{	
		typedef long value_type;
		typedef long difference_type;
		typedef long * pointer;
		typedef long& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<unsigned long>
	{	
		typedef unsigned long value_type;
		typedef unsigned long difference_type;
		typedef unsigned long * pointer;
		typedef unsigned long& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<double>
	{
		typedef double value_type;
		typedef double difference_type;
		typedef double * pointer;
		typedef double& reference;
		typedef floating_point_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<float>
	{
		typedef float value_type;
		typedef float difference_type;
		typedef float * pointer;
		typedef float& reference;
		typedef floating_point_iterator_tag iterator_category;
	};
	
	template<> struct iterator_traits<long long>
	{	
		typedef long long value_type;
		typedef long long difference_type;
		typedef long long * pointer;
		typedef long long& reference;
		typedef integral_iterator_tag iterator_category;
	};

	template<> struct iterator_traits<unsigned long long>
	{	
		typedef unsigned long long value_type;
		typedef unsigned long long difference_type;
		typedef unsigned long long distance_type;	
		typedef unsigned long long * pointer;
		typedef unsigned long long& reference;
		typedef integral_iterator_tag iterator_category;
	}; 
	
	namespace detail
	{
		template <class category> struct advance;

		template <>
		struct advance< input_iterator_tag>
		{	
			template< class Iterator, class Diff > inline
			static void Do( Iterator& where, Diff diff)	{	for (; 0 < diff; --diff) ++where;	}
		};
		
		template <>
		struct advance< forward_iterator_tag>
		{	
			template< class Iterator, class Diff > inline
				static void Do( Iterator& where, Diff diff)	{	for (; 0 < diff; --diff) ++where;	}
		};

		template <>
		struct advance< bidirectional_iterator_tag >
		{	
			template< class Iterator, class Diff > inline
				static void Do( Iterator& where, Diff diff)
			{	
				if ( 0 < diff )
					for ( ++where ; 0 < diff; --diff) ++where;	
				else
				if ( diff < 0 )
					for ( --where ; diff < 0 ; ++diff ) --where;
			}
		};
		
		template <>
		struct advance< random_access_iterator_tag >
		{
			template< class Iterator, class Diff > inline
				static void Do( Iterator& where, Diff diff) {	where += diff; }
		};

		template <class category> struct distance;
		
		template <> 
		struct distance<input_iterator_tag>
		{
			template<class Iterator, class Diff> inline
				static void Do( Diff& dist, Iterator first, Iterator last) { for (; first != last ; ++first)	++dist;	}
		};
		
		template <> 
		struct distance<forward_iterator_tag>
		{
			template<class Iterator, class Diff> inline
				static void Do( Diff& dist, Iterator first, Iterator last) { for (; first != last ; ++first)	++dist;	}
		};

		template <> 
		struct distance<bidirectional_iterator_tag>
		{
			template<class Iterator, class Diff> inline
				static void Do( Diff& dist, Iterator first, Iterator last) { for (; first != last ; ++first)	++dist;	}
		};
		
		template <> 
		struct distance<random_access_iterator_tag>
		{
			template<class Iterator, class Diff> inline
				static void Do( Diff& dist, Iterator first, Iterator last) { dist += last - first;	}
		};

	}

	template <class Iterator, class Diff> inline
	void	advance( Iterator& where, Diff diff)
	{
		typedef typename iterator_traits<Iterator>::iterator_category	iterator_category;
		detail::advance<iterator_category>::Do(where, diff);
	}
		
	template <class Iterator> inline 
	typename iterator_traits<Iterator>::difference_type distance(Iterator first, Iterator last)
	{
		typedef typename iterator_traits<Iterator>::iterator_category	iterator_category;
		typename iterator_traits<Iterator>::difference_type dist = 0;
		detail::distance<iterator_category>::Do(dist, first, last);
		return dist;
	}
	
	template <class Iterator, class Diff> inline 
	void distance(Iterator first, Iterator last, Diff& dist_accum)
	{
		typedef typename iterator_traits<Iterator>::iterator_category	iterator_category;
		detail::distance<iterator_category>::Do(dist_accum, first, last);
	}


	template<class Iterator>
	class reverse_iterator
		: public iterator<	typename iterator_traits<Iterator>::iterator_category, typename iterator_traits<Iterator>::value_type,
		typename iterator_traits<Iterator>::difference_type, typename iterator_traits<Iterator>::pointer, typename iterator_traits<Iterator>::reference> 
	{	
	public:
		typedef reverse_iterator<Iterator>								my_type;
		typedef typename iterator_traits<Iterator>::iterator_category	iterator_category;
		typedef typename iterator_traits<Iterator>::value_type			value_type;
		typedef typename iterator_traits<Iterator>::difference_type		difference_type;
		typedef typename iterator_traits<Iterator>::pointer				pointer;
		typedef typename iterator_traits<Iterator>::reference			reference;
		typedef Iterator												iterator_type;

		reverse_iterator(){}
		explicit reverse_iterator(Iterator iter) : current(iter) {}
		template<class Other>
		reverse_iterator(const reverse_iterator<Other>& rhs) : current( rhs.base() ) {}

		Iterator	base() const { return current; }
		reference   operator*() const {	Iterator _Tmp = current;	return (*--_Tmp);	}
		pointer		operator->() const { return (&**this);	}
		my_type&	operator++()	{ --current; return *this; }
		my_type		operator++(int)	{	my_type _Tmp = *this;	--current;	return (_Tmp);	}
		my_type&	operator--()	{	++current;	return (*this); }
		my_type		operator--(int)	{	my_type _Tmp = *this;	++current;	return (_Tmp);	}
//
		my_type&	operator+=(difference_type _Off){	current -= _Off;	return (*this);	}
		my_type		operator+(difference_type _Off) const {	return (my_type(current - _Off));	}
		my_type&	operator-=(difference_type _Off) {		current += _Off;	return (*this);	}
		my_type		operator-(difference_type _Off) const {		return (my_type(current + _Off));	}

		reference   operator[](difference_type _Off) const {	return (*(*this + _Off));	}

	private:
		Iterator	current;		// 원본 이터레이터.. 이 클래스가 이걸 래핑..
	};

	template<class Iter1, class Iter2> inline
	bool operator==(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right){	return _Left.base() == _Right.base();  }
	template<class Iter1, class Iter2> inline
	bool operator!=(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right){	return (!(_Left == _Right)); }
	template<class Iter1, class Iter2> inline
	bool  operator<(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right){	return _Right.base() < _Left.base(); }
	
	template<class Iter1, class Iter2> inline
	bool  operator>(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right){	return (_Right < _Left);  }
	
	template<class Iter1, class Iter2> inline
	bool operator<=(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right) {	return (!(_Right < _Left)); }

	template<class Iter1, class Iter2> inline
	bool operator>=(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right) {	return (!(_Left < _Right));	}

	template<class Iter, class _Diff> inline
	reverse_iterator<Iter> operator+(_Diff _Off, const reverse_iterator<Iter>& _Right) {	return (_Right + _Off);  }

	template<class Iter1, class Iter2> inline
	typename reverse_iterator<Iter1>::difference_type operator-(const reverse_iterator<Iter1>& _Left, const reverse_iterator<Iter2>& _Right)
	{	
		return _Right.base() - _Left.base();
	}
	
	template<class Container>
	class back_insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
	{	
	public:
		typedef Container								container_type;
		
		explicit back_insert_iterator(Container& _Cont)	: container(&_Cont)	{ }
		back_insert_iterator<Container>& operator=(	typename Container::const_reference _Val) {	container->push_back(_Val); return (*this);	}
		back_insert_iterator<Container>& operator*() {	return (*this);	}
		back_insert_iterator<Container>& operator++() {	return (*this);	}
		back_insert_iterator<Container> operator++(int) { return (*this); }

	protected:
		Container *container;	
	};

	template<class Container> inline
	back_insert_iterator<Container> back_inserter(Container& _Cont) {	return (i3::back_insert_iterator<Container>(_Cont)); }
	

	template<class Container>
	class front_insert_iterator	: public iterator<output_iterator_tag, void, void, void, void>
	{	
	public:
		typedef Container container_type;

		explicit front_insert_iterator(Container& _Cont) : container(&_Cont) {}
		front_insert_iterator<Container>& operator=(typename Container::const_reference _Val) {	container->push_front(_Val); return (*this); }
		front_insert_iterator<Container>& operator*() {	return (*this);	}
		front_insert_iterator<Container>& operator++() { return (*this); }
		front_insert_iterator<Container> operator++(int) {	return (*this);	}
	protected:
		Container *container;	
	};

	template<class Container> inline front_insert_iterator<Container> front_inserter(Container& _Cont) 
		{ return (i3::front_insert_iterator<Container>(_Cont)); }
	
	
	template<class Container>
	class insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
	{	
	public:
		typedef Container container_type;
		
		insert_iterator(Container& _Cont, typename Container::iterator _Where)
			: container(&_Cont), iter(_Where){}

		insert_iterator<Container>& operator=(typename Container::const_reference _Val)
		{	
			iter = container->insert(iter, _Val);	++iter;		return (*this);
		}

		insert_iterator<Container>& operator*()	{	return (*this);	}
		insert_iterator<Container>& operator++(){	return (*this);	}
		insert_iterator<Container>& operator++(int)	{	return (*this);	}
	protected:
		Container *container;	
		typename Container::iterator iter;	
	};

	template<class Container, class Iter> inline
		insert_iterator<Container> inserter(Container& _Cont, Iter _Where)
	{	
		return (i3::insert_iterator<Container>(_Cont, _Where));
	}
	
	
	
	
}
