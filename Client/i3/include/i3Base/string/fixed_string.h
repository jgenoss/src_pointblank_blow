#pragma once

// header
#include "fixed_string_fwd.h"

#include "rc_string.h"
#include "global_string_table.h"

namespace i3
{
	// 복사생성자,대입연산자 제외한 나머지 수정을 모두 막은 것..
//	template<class Ch, class Traits = i3::char_traits<Ch>, class Alloc = i3::pool_allocator<Ch> >
//	class fixed_basic_string;


	template<class Ch, class Traits, class Alloc>
	class fixed_basic_string
	{
	public:
		typedef fixed_basic_string<Ch, Traits, Alloc>				my_type;
		typedef rc_basic_string<Ch, Traits, Alloc>					rc_string_type;
		typedef global_string_table<Ch, Traits, Alloc>				table_type;

		typedef	typename rc_string_type::traits_type				traits_type;
		typedef typename traits_type::char_type						value_type;
		typedef typename rc_string_type::allocator_type				allocator_type;
		typedef typename allocator_type::size_type					size_type;
		typedef typename allocator_type::difference_type			difference_type;

		typedef typename allocator_type::const_reference			reference;
		typedef typename allocator_type::const_reference			const_reference;
		typedef typename allocator_type::const_pointer				pointer;
		typedef typename allocator_type::const_pointer				const_pointer;

		typedef	typename const_pointer								iterator;				// vector와 마찬가지로 이터레이터 구현부는 제거..
		typedef typename const_pointer								const_iterator;
				
		typedef i3::reverse_iterator<iterator>						reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>				const_reverse_iterator;
		
		typedef typename allocator_type::pointer					non_const_pointer;

		static const size_type										npos = static_cast<size_type>(-1);

		fixed_basic_string() {}
		fixed_basic_string(const my_type& rhs ) : m_dyn_string( rhs.m_dyn_string) {}
		
		fixed_basic_string(const my_type& rhs, size_type pos, size_type count = my_type::npos ) : 
			m_dyn_string( table_type::i()->insert( rc_string_type(rhs.m_dyn_string, pos, count) ) ) { }
		
		fixed_basic_string(const Ch* psz, size_type count) : 
			m_dyn_string( table_type::i()->insert(psz, count) ) {}
		
		fixed_basic_string(const Ch* psz) : m_dyn_string( table_type::i()->insert(psz) ) {}
		
		fixed_basic_string(size_type count, Ch ch ) : m_dyn_string(table_type::i()->insert( rc_string_type(count,ch) ) ) {}

		template<class It>
		fixed_basic_string(It first, It last) : m_dyn_string(table_type::i()->insert(rc_string_type(first, last) ) ) {}

		fixed_basic_string(non_const_pointer first, non_const_pointer last )
			: m_dyn_string(table_type::i()->insert(rc_string_type(first, last) ) ) {}

		fixed_basic_string(const_iterator first, const_iterator last )
			: m_dyn_string(table_type::i()->insert(rc_string_type(first, last) ) ) {}
				
		fixed_basic_string( const rc_string_type& str ) : m_dyn_string(table_type::i()->insert(str) ) {}

		template< class T, class A>
		fixed_basic_string( const basic_string<Ch,T,A>& str ) : m_dyn_string(table_type::i()->insert(str.c_str(), str.size()) ) {}

		template<class BS, class T, class A>
		fixed_basic_string( const stack_basic_string<BS,Ch,T,A>& str ) : m_dyn_string(table_type::i()->insert(str.c_str(), str.size()) ) {}

		template< class T, class A>
		fixed_basic_string( const ::std::basic_string<Ch,T,A>& str ) : m_dyn_string(table_type::i()->insert(str.c_str(), str.size()) ) {}

		template< class It>
		fixed_basic_string( const i3::literal_base_range<It>& rng ) : m_dyn_string(table_type::i()->insert(rng.c_str(), rng.size()) ) {}



		~fixed_basic_string() {}
		
		my_type&	operator=(const my_type& rhs)	{	m_dyn_string = rhs.m_dyn_string;  return *this; }
		my_type&	operator=(const Ch*		psz )	{	m_dyn_string = table_type::i()->insert(psz);	 return *this; }
		my_type&	operator=(Ch ch)				{   m_dyn_string = table_type::i()->insert(rc_string_type(1,ch));  return *this; }
		

		my_type&	operator=( const rc_string_type& str ) { m_dyn_string = table_type::i()->insert(str);  return *this; }
		template<class T, class A>
		my_type&	operator=( const basic_string<Ch,T,A>& str ) { m_dyn_string = table_type::i()->insert(str.c_str(), str.size());  return *this; }
		template<class T, class A>
		my_type&	operator=( const ::std::basic_string<Ch,T,A>& str ) { m_dyn_string = table_type::i()->insert(str.c_str(), str.size());  return *this; }
		template<class BS, class T, class A>
		my_type&	operator=( const stack_basic_string<BS, Ch,T,A>& str ) { m_dyn_string = table_type::i()->insert(str.c_str(), str.size());  return *this; }
		template<class It>
		my_type&	operator=( const i3::literal_base_range<It>& rng ) { m_dyn_string = table_type::i()->insert(rng.c_str(), rng.size());  return *this; }


		unsigned long			ref_count() const	{ return m_dyn_string.ref_count(); }
		bool					is_null_string() const { return m_dyn_string.is_null_string(); }
		allocator_type			get_allocator() const { return m_dyn_string.get_allocator(); }
		void					clear()			{	m_dyn_string.clear(); }		// 이렇게만 하면 충분할것 같은데..


		const_iterator			begin() const	{ return m_dyn_string.begin(); }
		const_iterator			end() const		{ return m_dyn_string.end(); }
		const_reverse_iterator	rbegin() const	{ return const_reverse_iterator(end()); }
		const_reverse_iterator	rend() const	{ return const_reverse_iterator(begin()); }

		const_reference			at(size_type pos) const { return m_dyn_string.at(pos); }
		const_reference			operator[](size_type pos) const { return m_dyn_string.operator[](pos); }
			
	
		const Ch*				c_str() const		{ return m_dyn_string.c_str(); } 
		const Ch*				data() const		{ return m_dyn_string.data(); }
		size_type				size() const		{ return m_dyn_string.size(); }
		size_type				length() const		{ return m_dyn_string.length(); }
		size_type				max_size() const	{ return allocator_type().max_size() - 1; }
		
		bool					empty() const		{ return m_dyn_string.empty(); }

		
		size_type				copy(Ch* dest, size_type count, size_type pos = 0 ) const{		return	_Copy_s(dest, count, count, pos);	}
		
		size_type				_Copy_s(Ch* dest, size_type destsize, size_type count, size_type pos = 0 ) const
		{
			return m_dyn_string._Copy_s(dest, destsize, count, pos);
		}
		
		void					swap( my_type& rhs)	{	m_dyn_string.swap(rhs.m_dyn_string);	}
		
		size_type				find(const my_type& rhs, size_type pos = 0) const { return m_dyn_string.find(rhs.m_dyn_string, pos); }
		size_type				find(const Ch* psz, size_type pos, size_type count) const { return m_dyn_string.find(psz, pos, count); }
		size_type				find(const Ch* psz, size_type pos = 0) const {	return m_dyn_string.find(psz, pos); }
		size_type				find(Ch ch, size_type pos ) const { return m_dyn_string.find(ch, pos); }
		size_type				find(Ch ch) const { return m_dyn_string.find(ch); }
		
		size_type				rfind(const my_type& rhs, size_type pos = my_type::npos) const {	return m_dyn_string.rfind(rhs.m_dyn_string, pos); }
		size_type				rfind(const Ch* psz, size_type pos, size_type count) const { return m_dyn_string.rfind(psz, pos, count); }
		size_type				rfind(const Ch* psz, size_type pos = my_type::npos) const { return m_dyn_string.rfind(psz, pos); }
		size_type				rfind(Ch ch, size_type pos ) const { return m_dyn_string.rfind(ch, pos); }
		size_type				rfind(Ch ch) const { return m_dyn_string.rfind(ch); }
		
		
		size_type	find_first_of( const my_type& rhs, size_type pos = 0) const { return m_dyn_string.find_first_of(rhs.m_dyn_string, pos); }
		size_type	find_first_of( const Ch* psz, size_type pos, size_type count) const { return m_dyn_string.find_first_of(psz, pos, count); }
		size_type	find_first_of(const Ch* psz, size_type pos = 0) const { return m_dyn_string.find_first_of(psz, pos); }
		size_type	find_first_of(Ch ch, size_type pos ) const { return m_dyn_string.find_first_of(ch, pos); }
		size_type	find_first_of(Ch ch) const { return m_dyn_string.find_first_of(ch); }


		size_type	find_last_of(const my_type& rhs, size_type pos = my_type::npos) const { return m_dyn_string.find_last_of(rhs.m_dyn_string, pos); }
		size_type	find_last_of(const Ch* psz, size_type pos, size_type count) const { return m_dyn_string.find_last_of(psz, pos, count); }
		size_type	find_last_of(const Ch* psz, size_type pos = my_type::npos) const { return m_dyn_string.find_last_of(psz, pos); }
		size_type	find_last_of(Ch ch, size_type pos ) const { return m_dyn_string.find_last_of(ch, pos); }
		size_type	find_last_of(Ch ch) const { return m_dyn_string.find_last_of(ch); }

		size_type	find_first_not_of(const my_type& rhs, size_type pos = 0) const { return m_dyn_string.find_first_not_of(rhs.m_dyn_string, pos); }
		size_type	find_first_not_of(const Ch* psz, size_type pos, size_type count) const { return m_dyn_string.find_first_not_of(psz, pos, count); }
		size_type	find_first_not_of(const Ch* psz,	size_type pos = 0) const {	return m_dyn_string.find_first_not_of(psz, pos); }
		size_type	find_first_not_of(Ch ch, size_type pos = 0) const	{	return m_dyn_string.find_first_not_of(ch, pos); }

		size_type	find_last_not_of(const my_type& rhs, size_type pos = my_type::npos) const {	return m_dyn_string.find_last_not_of(rhs.m_dyn_string, pos); }
		size_type	find_last_not_of(const Ch* psz, size_type pos, size_type count) const { return m_dyn_string.find_last_not_of(psz, pos, count); }
		size_type	find_last_not_of(const Ch* psz, size_type pos = my_type::npos) const {	return m_dyn_string.find_last_not_of(psz, pos); }
		size_type	find_last_not_of(Ch ch, size_type pos = my_type::npos) const {	return m_dyn_string.find_last_not_of(ch, pos); }
		
		
		my_type		substr(size_type pos = 0, size_type count = my_type::npos) const { 	return my_type(*this, pos, count); }
		

		bool		equal_to( const my_type& rhs ) const {	return c_str() == rhs.c_str(); }
		bool		equal_to( const rc_string_type& str) const { return m_dyn_string.equal_to(str); }
		bool		equal_to(const Ch* psz) const	{	return m_dyn_string.equal_to(psz); }				
		template< template<class, class, class> class string_ctn, class C,class T, class A>
		bool		equal_to(const string_ctn<C,T,A>& str ) {  return m_dyn_string.equal_to(str); }

		
		int			compare( const my_type& rhs) const { return m_dyn_string.compare(rhs.m_dyn_string);	}
		int			compare( const rc_string_type& str) const { return m_dyn_string.compare(str); }
		int			compare( size_type pos, size_type n0, const my_type& rhs) const { return m_dyn_string.compare(pos, n0, rhs.m_dyn_string); }
		int			compare( size_type pos, size_type n0, const my_type& rhs, size_type rpos, size_type count) const
			{	return m_dyn_string.compare(pos, n0, rhs.m_dyn_string, rpos, count); 	}
		int			compare( const Ch* psz) const { return m_dyn_string.compare(psz); }
		int			compare(size_type pos, size_type n0, const Ch* psz) const { return m_dyn_string.compare(pos, n0, psz); }
		int			compare(size_type pos, size_type n0, const Ch* psz, size_type count) const { return m_dyn_string.compare(pos, n0, psz, count); }

		
		my_type&	assign(const my_type& rhs) { m_dyn_string = rhs.m_dyn_string;	return *this;	}
		my_type&	assign(const my_type& rhs, size_type pos, size_type count)
		{
			m_dyn_string = table_type::i()->insert( rc_string_type(rhs.m_dyn_string, pos, count) );
			return *this;
		}

		my_type&	assign(const Ch* psz) { m_dyn_string = table_type::i()->insert(psz);	return *this; }
		my_type&	assign(const Ch* psz, size_type count) { m_dyn_string = table_type::i()->insert(psz, count);	return *this; }


		my_type&	assign( const rc_string_type& str ) { m_dyn_string = table_type::i()->insert(str);  return *this; }
		template<class T, class A>
		my_type&	assign( const basic_string<Ch,T,A>& str ) { m_dyn_string = table_type::i()->insert(str.c_str(), str.size());  return *this; }
		template<class T, class A>
		my_type&	assign( const ::std::basic_string<Ch,T,A>& str ) { m_dyn_string = table_type::i()->insert(str.c_str(), str.size());  return *this; }
		template<class BS, class T, class A>
		my_type&	assign( const stack_basic_string<BS, Ch,T,A>& str ) { m_dyn_string = table_type::i()->insert(str.c_str(), str.size());  return *this; }
		template<class It>
		my_type&	assign( const i3::literal_base_range<It>& rng ) { m_dyn_string = table_type::i()->insert(rng.c_str(), rng.size());  return *this; }


		my_type&	assign(size_type count, Ch ch )	{ m_dyn_string = table_type::i()->insert( rc_string_type(count,ch) );		return *this;	}


		template<class It>
		my_type&	assign(It first, It last)
		{
			assign_tag(first, last, typename i3::iterator_traits<It>::iterator_category() );	return *this;
		}

		my_type&	assign(non_const_pointer first, non_const_pointer last) {	m_dyn_string = table_type::i()->insert(rc_string_type(first, last) );		return *this; }
		my_type&	assign(const_iterator first, const_iterator last) {	m_dyn_string = table_type::i()->insert(rc_string_type(first, last) );		return *this; }
		
	private:
		
		template<class It>
		void	assign_tag(It count, It ch, i3::integral_iterator_tag)	{		assign((size_type)count, (Ch) ch);	}

		template<class It>
		void	assign_tag(It first, It last, i3::input_iterator_tag)	{	m_dyn_string = table_type::i()->insert(rc_string_type(first, last) ); }	

		rc_string_type							m_dyn_string;	
		
		friend class rc_basic_string<Ch, Traits, Alloc>;

	};


	template<class Ch, class T, class A> inline
	void swap(fixed_basic_string<Ch, T, A>& Left, fixed_basic_string<Ch, T, A>& Right) {	Left.swap(Right);  }
	



}


#include "fixed_string_operator.h"
