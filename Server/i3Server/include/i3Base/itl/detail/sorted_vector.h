#pragma once

//
// vector  -> ebo덕분에 --> private상속이 어려움 --> 멤버로 빼고..
// compare -> ebo처리..
// map계열에서 따로 erase함수를 하나 더 만들어야됨...erase( const key_type* f, const key_type* l) 형태..(set은 함수가 중복된다..)
//
#include "sorted_vector_impl.h"

namespace i3
{

	template<class Val, class Pred, class Alloc>						// 여기서 Val는 value_type
	class sorted_vector_base : private i3::sorted_vector_key<Val, Pred>
	{
	public:
		typedef sorted_vector_base<Val, Pred, Alloc>					my_type;
		typedef i3::sorted_vector_key<Val, Pred>						base_type;
		
		typedef typename base_type::key_type						key_type;
		typedef typename base_type::value_type						value_type;		
		typedef typename base_type::key_compare						key_compare;
		typedef typename base_type::value_compare					value_compare;
		typedef typename rebind_wrap<Alloc, Val>::type				allocator_type;
		typedef typename allocator_type::reference					reference;
		typedef typename allocator_type::const_reference			const_reference;
		
		typedef i3::vector<Val, allocator_type>						vector_type;
		typedef typename vector_type::iterator						iterator;
		typedef typename vector_type::const_iterator				const_iterator;
		typedef typename vector_type::size_type						size_type;
		typedef typename vector_type::difference_type				difference_type;

		typedef typename allocator_type::pointer					pointer;
		typedef typename allocator_type::const_pointer				const_pointer;
		
		typedef typename vector_type::reverse_iterator				reverse_iterator;
		typedef typename vector_type::const_reverse_iterator		const_reverse_iterator;
	
		typedef typename base_type::base_type						compare_base;
		typedef typename base_type::my_compare						my_compare;
			
		my_type() {}
		explicit my_type(const key_compare& pr) : base_type(pr) {}
		explicit my_type(const key_compare& pr, const allocator_type& a) : base_type(pr), m_vector(a) {}

		template<class input_iterator>
		my_type(input_iterator f, input_iterator l) : m_vector(f, l) 
		{
			i3::sort( m_vector.begin(), m_vector.end(), comp());		// 우선 정렬..유니크의 경우..유닉처리를 더해야함..
		}

		template<class input_iterator>
		my_type(input_iterator f, input_iterator l, const key_compare& pr) : base_type(pr), m_vector(f, l) 
		{
			i3::sort( m_vector.begin(), m_vector.end(), comp());		// 우선 정렬..유니크의 경우..유닉처리를 더해야함..
		}


		template<class input_iterator>
		my_type(input_iterator f, input_iterator l, const key_compare& pr, const allocator_type& a) : base_type(pr), m_vector(f, l, a) 
		{
			i3::sort( m_vector.begin(), m_vector.end(), comp());		// 우선 정렬..유니크의 경우..유닉처리를 더해야함..
		}
		
		my_type(const my_type& rhs) : base_type(rhs.comp()), m_vector(rhs.m_vector) { }
				
		my_type&	operator=( const my_type& rhs) {	comp() = rhs.comp();	m_vector = rhs.m_vector;	return *this; }	
		
		allocator_type			get_allocator() const { return m_vector.get_allocator(); } 

		iterator				begin()			{ return m_vector.begin(); }
		const_iterator			begin() const	{ return m_vector.begin(); }
		iterator				end()			{ return m_vector.end();   }
		const_iterator			end() const		{ return m_vector.end(); }

		reverse_iterator		rbegin()			{ return m_vector.rbegin(); }
		const_reverse_iterator	rbegin() const		{ return m_vector.rbegin(); }
		reverse_iterator		rend()				{ return m_vector.rend();   }
		const_reverse_iterator	rend() const		{ return m_vector.rend(); }

		bool					empty() const		{ return m_vector.empty(); }
		size_type				size() const		{ return m_vector.size(); }
		size_type				max_size() const	{ return m_vector.max_size(); }
		void					clear()				{ m_vector.clear(); }
		
		size_type				capacity() const   { return m_vector.capacity(); }		// 벡터쪽 함수 추가..
		void					reserve(size_type n) { m_vector.reserve(n); }			// 벡터쪽 함수 추가..
		

		key_compare				key_comp() const	{  return compare_base::get(); }
		value_compare			value_comp() const	{  return value_compare(key_comp()); }
		

		iterator				erase(const_iterator i)	{ return m_vector.erase(i); }
		iterator				erase(const_iterator f, const_iterator l) { return m_vector.erase(f, l); }
		
		void					swap(my_type& rhs) {	i3::swap( comp(), rhs.comp() );	i3::swap( m_vector, rhs.m_vector);	}

		iterator				find(const key_type& k)	
		{ 
			iterator i (lower_bound(k));	
			if ( i != end() && comp()(k, *i) ) i = end();	
			return i;
		}
		const_iterator			find(const key_type& k) const	
		{ 
			const_iterator i (lower_bound(k));	
			if ( i != end() && comp()(k, *i) ) i = end();	
			return i;
		}
				
		iterator				lower_bound(const key_type& k) { return i3::lower_bound(m_vector.begin(), m_vector.end(), k, comp() ); }
		iterator				upper_bound(const key_type& k) { return i3::upper_bound(m_vector.begin(), m_vector.end(), k, comp() ); }
		const_iterator			lower_bound(const key_type& k) const { return i3::lower_bound(m_vector.begin(), m_vector.end(), k, comp() ); }
		const_iterator			upper_bound(const key_type& k) const { return i3::upper_bound(m_vector.begin(), m_vector.end(), k, comp() ); }
		
		bool					operator==( const my_type& rhs) const { return m_vector == rhs.m_vector; }
		bool					operator<(const my_type& rhs)   const { return m_vector < rhs.m_vector; }
		

	protected:
		__forceinline const my_compare&	comp() const { return compare_base::get(); }
		__forceinline my_compare&		comp()		 { return compare_base::get(); }
		__forceinline const vector_type&	vec() const { return m_vector; }
		__forceinline vector_type&			vec()		{ return m_vector; }
	private:
		vector_type							m_vector;			// 메인 멤버..
	};

	template<class Val, class Pred, class Alloc>		
	class sorted_vector : protected sorted_vector_base<Val, Pred, Alloc>
	{
	public:
		typedef sorted_vector<Val, Pred, Alloc>			my_type;
		typedef sorted_vector_base<Val, Pred, Alloc>	base_type;
		
		my_type() {}
		explicit my_type( const key_compare& pr ) : base_type(pr) {}
		explicit my_type(const key_compare& pr, const allocator_type& a) : base_type(pr, a) {}
		
		template<class input_iterator>
		my_type(input_iterator f, input_iterator l) : base_type(f, l) 
		{ 
			vec().erase(i3::unique(vec().begin(), vec().end(), i3::not2(comp())), vec().end());		// 이미 정렬된상태에선 반대함수만 취급하면 된다..
		}

		template<class input_iterator>
		my_type(input_iterator f, input_iterator l, const key_compare& pr) : base_type(f, l, pr) 
		{ 
			vec().erase(i3::unique(vec().begin(), vec().end(), i3::not2(comp())), vec().end());		// 이미 정렬된상태에선 반대함수만 취급하면 된다..
		}

		template<class input_iterator>
		my_type(input_iterator f, input_iterator l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) 
		{ 
			vec().erase(i3::unique(vec().begin(), vec().end(), i3::not2(comp())), vec().end());		// 이미 정렬된상태에선 반대함수만 취급하면 된다..
		}
		
		template<class Iter> void insert(Iter f, Iter l)	
		{
			if ( empty() )
			{
				vec().insert(vec().begin(), f, l);
				i3::sort( vec().begin(), vec().end(), comp());
				vec().erase(i3::unique(vec().begin(), vec().end(), i3::not2(comp())), vec().end());		// 유닉처리가 필요함..
			}
			else
			{
				for (; f != l; ++f) insert(*f);
			}
		}
		
		i3::pair<iterator, bool> insert(const value_type& val)
		{
			iterator i(lower_bound(key(val) ) );
			if (i == end() || comp()(val, *i ) )
				return i3::pair<iterator, bool>( vec().insert(i, val), true );
			return	i3::pair<iterator, bool>(i, false); 
		}

		//Section [23.1.2], Table 69
		//http://developer.apple.com/documentation/DeveloperTools/gcc-3.3/libstdc++/23_containers/howto.html#4
		iterator insert(iterator pos, const value_type& val)
		{
			if ( ( pos == begin() || comp()( *(pos-1), val ) ) && ( pos == end() || comp()( val, *pos ) ) )
				return vec().insert(pos, val);
			return insert(val).first;
		}

		using base_type::erase;

		size_type				erase(const key_type& k) 
		{   
			iterator i(find(k));
			if ( i == end()) return 0;
			erase(i);
			return 1;
		}


		i3::pair<const_iterator, const_iterator> equal_range(const key_type& k) const	
		{
			const_iterator i1 = lower_bound(k);		const_iterator i2 = i1;
			if ( i1 != end() && !comp()( k, key(*i1) ) ) ++i2;
			return i3::pair<const_iterator, const_iterator>(i1, i2);	
		}
		
		i3::pair<iterator, iterator> equal_range(const key_type& k) 	
		{
			iterator i1 = lower_bound(k);		iterator i2 = i1;
			if ( i1 != end() && !comp()( k, key(*i1) ) ) ++i2;
			return i3::pair<iterator, iterator>(i1, i2);	
		}

		size_type	count(const key_type& k) const { return find(k) != end(); }
	};

	template<class Val, class Pred, class Alloc>		
	class multi_sorted_vector : protected sorted_vector_base<Val, Pred, Alloc>
	{
	public:
		typedef multi_sorted_vector<Val, Pred, Alloc>			my_type;
		typedef sorted_vector_base<Val, Pred, Alloc>			base_type;

		my_type() {}
		explicit my_type( const key_compare& pr ) : base_type(pr) {}
		explicit my_type(const key_compare& pr, const allocator_type& a) : base_type(pr, a) {}
		
		template<class input_iterator>
		my_type(input_iterator f, input_iterator l) : base_type(f, l) { }

		template<class input_iterator>
		my_type(input_iterator f, input_iterator l, const key_compare& pr) : base_type(f, l, pr) { }

		template<class input_iterator>
		my_type(input_iterator f, input_iterator l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) { }
		
		template<class Iter> void insert(Iter f, Iter l)	
		{
			if ( empty() )
			{
				vec().insert(vec().begin(), f, l);
				i3::sort( vec().begin(), vec().end(), comp());
			}
			else
			{
				for (; f != l; ++f) insert(*f);
			}
		}

		iterator insert(const value_type& val) {	return vec().insert(lower_bound( key(val) ), val);	}

		iterator insert(iterator pos, const value_type& val)		// 힌트값에 대해 어케 처리해야되나..
		{
			if ( ( pos == begin() || !comp()( val, *(pos-1) ) ) && ( pos == end() || !comp()( *pos, val ) ) )	// 왼,오른쪽 뒤집은뒤 not연산하면 됨..
				return vec().insert(pos, val);
			return insert(val);
		}

		using base_type::erase;

		size_type				erase(const key_type& k) 
		{  
			i3::pair<iterator, iterator> where = equal_range(k);
			size_type num = static_cast<size_type>(i3::distance( where.first, where.second ));
			erase(where.first, where.second);
			return num;
		}

		i3::pair<const_iterator, const_iterator> equal_range(const key_type& k) const{	return i3::equal_range(m_vector.begin(), m_vector.end(), k, comp()); }	
		i3::pair<iterator, iterator> equal_range(const key_type& k) {	return i3::equal_range(m_vector.begin(), m_vector.end(), k, comp()); }	

		size_type	count(const key_type& k) const
		{
			i3::pair<const_iterator, const_iterator> res = equal_range(k);
			return	static_cast<size_type>( i3::distance(res.first, res.second) );	
		}


	};

	template<class V, class P, class A> inline
	bool	operator!=(const sorted_vector_base<V, P, A>& lhs, const sorted_vector_base<V, P, A>& rhs) { return !(lhs == rhs); }
	
	template<class V, class P, class A> inline
	bool	operator> (const sorted_vector_base<V, P, A>& lhs, const sorted_vector_base<V, P, A>& rhs) { return  rhs < lhs  ; }
	
	template<class V, class P, class A> inline
	bool	operator<= (const sorted_vector_base<V, P, A>& lhs, const sorted_vector_base<V, P, A>& rhs) { return  !(rhs < lhs); }
	
	template<class V, class P, class A> inline
	bool	operator>= (const sorted_vector_base<V, P, A>& lhs, const sorted_vector_base<V, P, A>& rhs) { return  !(lhs < rhs); }

	

}