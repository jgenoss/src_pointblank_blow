#pragma once

#include "i3string_fwd.h"

#include "char_traits.h"
#include "../pool/pool_allocator.h"
#include "../itl/ebo.h"
#include "../itl/iterator.h"

#include "../itl/move/move.h"

#pragma pack(push, _CRT_PACKING)

namespace i3
{
	//
	// SSO를 제거한다.  (단문자 최적화를 사용하지 않음...-->컨테이너사이즈를 12로 유지하고 나머지는 동적으로 할당..)
	// 할당을 전제로 해야하므로, 4바이트 union을 구성하고...널값을 대신하도록 조절..
	// 문자열이 들어오면 바로 할당 시작..
	//

	template<class, class, class> class rc_basic_string;
	template<class, class, class> class fixed_basic_string;			
	template<class, class, class, class> class stack_basic_string;


	template <class Ch, class traits, class Alloc>
	class basic_string : private enable_ebo< typename rebind_wrap<Alloc, Ch>::type >
	{
		I3_COPYABLE_MOVABLE(basic_string);
	public:
		typedef basic_string<Ch, traits, Alloc>					my_type;
		typedef enable_ebo< typename rebind_wrap<Alloc, Ch>::type >	allocator_base;

		typedef	traits											traits_type;
		typedef typename traits::char_type						value_type;
		typedef typename Alloc::template rebind<Ch>::other		allocator_type;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;

		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer			const_pointer;

		typedef	typename pointer								iterator;				// vector와 마찬가지로 이터레이터 구현부는 제거..
		typedef typename const_pointer							const_iterator;
				
		typedef i3::reverse_iterator<iterator>					reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>			const_reverse_iterator;
		static const size_type									npos = static_cast<size_type>(-1);
		
	
		basic_string() : m_ptr( ms_null ), m_size(0), m_reserve(0) { eos();	}
		explicit basic_string(const Alloc& al) : allocator_base(al), m_ptr( ms_null ), m_size(0), m_reserve(0) { eos(); }

		basic_string(const my_type& rhs ) { assign_new(rhs, 0, my_type::npos ); }
		basic_string(const my_type& rhs, size_type pos, size_type count = my_type::npos ) {		assign_new(rhs, pos, count);	}
		basic_string(const my_type& rhs, size_type pos, size_type count, const Alloc& al) : allocator_base(al) {	assign_new(rhs, pos, count); }
		
		basic_string(rv_ref(my_type) rval) : m_ptr( rval.m_ptr), m_size( rval.m_size), m_reserve(rval.m_reserve) 
		{
			rval.m_ptr = ms_null;	rval.m_size = 0;	rval.m_reserve = 0;
		}
		
		template <class traits2, class Alloc2>
		basic_string(const ::std::basic_string<Ch, traits2, Alloc2>& rhs ) { assign_new( rhs.c_str(), rhs.size() ); }	
		template <class traits2, class Alloc2>
		basic_string(const rc_basic_string<Ch, traits2, Alloc2>& rhs ) { assign_new( rhs.c_str(), rhs.size() ); }	
		template <class traits2, class Alloc2>
		basic_string(const fixed_basic_string<Ch, traits2, Alloc2>& rhs ) { assign_new( rhs.c_str(), rhs.size() ); }	
		template <class BuffSize2, class traits2, class Alloc2>
		basic_string(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs ) { assign_new( rhs.c_str(), rhs.size() ); }	
		template<class It>
		basic_string(const i3::literal_base_range<It>& rng) { assign_new(rng.c_str(), rng.size()); }


		basic_string(const Ch* psz, size_type count) {	assign_new( psz, count);  }
		basic_string(const Ch* psz, size_type count, const Alloc& al) : allocator_base(al) {  assign_new( psz, count);  }
		basic_string(const Ch* psz) { assign_new(psz,  traits_type::length(psz) ); }
		basic_string(const Ch* psz, const Alloc& al) : allocator_base(al) { assign_new(psz,  traits_type::length(psz) ); }
		basic_string(size_type count, Ch ch )	{	assign_new(count, ch);	}
		basic_string(size_type count, Ch ch, const Alloc& al) : allocator_base(al)	{	assign_new(count, ch);	}
		template<class It>
		basic_string(It first, It last) { assign_new( first, last, typename i3::iterator_traits<It>::iterator_category() ); }
		template<class It>
		basic_string(It first, It last, const Alloc& al) : allocator_base(al) { assign_new( first, last, i3::iterator_traits<It>::iterator_category() ); }

		basic_string(iterator first, iterator last ) {	assign_new(&*first, last - first);	}	// const Ch* , count //
		basic_string(const_iterator first, const_iterator last ) {	assign_new(&*first, last - first);	}	// const Ch* , count //

		~basic_string() 
		{	
			if ( m_ptr != ms_null)
				alloc().deallocate(m_ptr, m_reserve + 1);	
		}	// 할당된 것 이외에는 처리할것 없음..
		
//		my_type&	operator=(const my_type& rhs)	{ return assign(rhs); }		// rvalue_ref_emul을 쓸것이므로 다르게 처리..

		my_type&	operator=(const_assign_ref(my_type) rhs) { return assign(rhs); }

		my_type&	operator=(rv_ref(my_type) rval) 
		{  
		//	if ( this == &rval ) return *this;
			if ( m_ptr != ms_null) 
				alloc().deallocate(m_ptr, m_reserve + 1);	
			m_ptr = rval.m_ptr; m_size = rval.m_size; m_reserve = rval.m_reserve; 
			rval.m_ptr = ms_null;	rval.m_size = 0;	rval.m_reserve = 0;
			return *this;
		}
		
		my_type&	operator=(const Ch*		psz )	{ return assign(psz); }
		my_type&	operator=(Ch ch)				{ return assign(1, ch); }
		
		template < class traits2, class Alloc2>
		my_type&	operator=(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type&	operator=(const rc_basic_string<Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type&	operator=(const fixed_basic_string<Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template < class BuffSize2, class traits2, class Alloc2>
		my_type&	operator=(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template<class It>
		my_type&	operator=(const i3::literal_base_range<It>& rng) { return assign(rng.c_str(), rng.size());   }


		my_type&	operator+=(const my_type& rhs)	{ return append(rhs); }
		my_type&	operator+=(const Ch*	psz)	{ return append(psz); }
		my_type&	operator+=(Ch ch)				{ return append((size_type)1, ch); }

//		template < template<class, class, class> class string_ctn, class traits2, class Alloc2>
//		my_type&	operator+=(const string_ctn<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
	
		template < class traits2, class Alloc2>
		my_type&	operator+=(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type&	operator+=(const rc_basic_string<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type&	operator+=(const fixed_basic_string<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class BuffSize2, class traits2, class Alloc2>
		my_type&	operator+=(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class It>
		my_type&	operator+=(const i3::literal_base_range<It>& rng) {  return append(rng.c_str(), rng.size());   }

		allocator_type			get_allocator() const { return alloc(); }
		
		void					clear()			{	eos(0);	}		// 이렇게만 하면 충분할것 같은데..
		iterator				begin()			{ return m_ptr; }
		const_iterator			begin() const	{ return m_ptr; }
		iterator				end()			{ return m_ptr + m_size; }
		const_iterator			end() const		{ return m_ptr + m_size; }
		reverse_iterator		rbegin()		{ return reverse_iterator(end()); }
		const_reverse_iterator	rbegin() const	{ return const_reverse_iterator(end()); }
		reverse_iterator		rend()			{ return reverse_iterator(begin()); }
		const_reverse_iterator	rend() const	{ return const_reverse_iterator(begin()); }
		
		reference				at(size_type pos)	{ return m_ptr[pos]; }
		const_reference			at(size_type pos) const { return m_ptr[pos]; }
		reference				operator[](size_type pos) { return m_ptr[pos]; }
		const_reference			operator[](size_type pos) const { return m_ptr[pos]; }

		void					push_back(Ch ch)	{	insert( end(), ch); }
		const Ch*				c_str() const		{ return m_ptr; }
		const Ch*				data() const		{ return m_ptr; }
		size_type				size() const		{ return m_size; }
		size_type				length() const		{ return m_size; }
		size_type				max_size() const	{ return alloc().max_size() - 1; }

		void					resize(size_type newsize) { resize(newsize, Ch()); }
		void					resize(size_type newsize, Ch ch)
		{
			if ( newsize <= m_size )
				erase(newsize);
			else
				append(newsize - m_size, ch);
		}
		size_type				capacity() const { return m_reserve; }
		void					reserve(size_type capa = 0)	{	grow_up_append(capa);		eos();	}
		bool					empty() const { return m_size == 0; }
		
		size_type				copy(Ch* dest, size_type count, size_type pos = 0 ) const
		{
			return				_Copy_s(dest, count, count, pos);
		}
		
		size_type				_Copy_s(Ch* dest, size_type destsize, size_type count, size_type pos = 0 ) const
		{
			if ( count > m_size - pos )
				count  = m_size - pos;
			traits_type::_Copy_s(dest, destsize, m_ptr + pos, count);
			return count;
		}
		
		void					swap( my_type& rhs)
		{
			if ( this == &rhs) return;

			if ( alloc() == rhs.alloc() )
			{	// 스웝 구현.... 더 좋은 방법이 있을지도..
			
				Ch*			ptemp = m_ptr;		m_ptr = rhs.m_ptr;		rhs.m_ptr = ptemp;						
				size_type	temp_size = m_size;	m_size = rhs.m_size;	rhs.m_size = temp_size;
				size_type	temp_reserve = m_reserve;	m_reserve = rhs.m_reserve;	rhs.m_reserve = temp_reserve;
			}
			else
			{
				my_type Tmp = *this;
				*this = rhs;
				rhs		= Tmp;	
			}	
		}
		
		size_type				find(const my_type& rhs, size_type pos = 0) const {	return	find(rhs.m_ptr, pos, rhs.size() );	}
		size_type				find(const Ch* psz, size_type pos, size_type count) const
		{
			if ( count == 0 && pos <= m_size )
				return pos;
			
			size_type nm;
			if ( pos < m_size && count <= ( nm = m_size - pos ) )
			{
				const Ch* pU, * pV;
				for ( nm -= count - 1, pV = m_ptr + pos ; ( pU = traits_type::find(pV, nm, *psz) ) != 0; 
					nm -= pU - pV + 1, pV = pU + 1 )
					if ( traits_type::compare(pU, psz, count ) == 0 )
						return pU - m_ptr;
			}
			return my_type::npos;
		}

		size_type				find(const Ch* psz, size_type pos = 0) const {	return find(psz, pos, traits_type::length(psz)); }
		size_type				find(Ch ch, size_type pos ) const { return find_count_1(ch, pos); }
		size_type				find(Ch ch) const { return find_pos_0_count_1(ch); }


		size_type				rfind(const my_type& rhs, size_type pos = my_type::npos) const {	return rfind( rhs.m_ptr, pos, rhs.size());	}
		size_type				rfind(const Ch* psz, size_type pos, size_type count) const
		{
			if (count == 0) 
				return ( pos < m_size ? pos : m_size );
			if ( count <= m_size )
			{
				const Ch* pU = m_ptr + ( pos < m_size - count ? pos : m_size - count);
				for ( ; ; --pU)
					if ( traits_type::eq(*pU, *psz) && traits_type::compare(pU, psz, count) == 0 )
						return pU - m_ptr;
					else if ( pU == m_ptr )
						break;
			}
			return my_type::npos;
		}

//	
		size_type				rfind(const Ch* psz, size_type pos = my_type::npos) const { return rfind(psz, pos, traits_type::length(psz)); }
		size_type				rfind(Ch ch, size_type pos ) const { return rfind_count_1( ch, pos); }
		size_type				rfind(Ch ch) const { return rfind_npos_count_1(ch);  }


		size_type	find_first_of( const my_type& rhs, size_type pos = 0) const { return find_first_of(rhs.m_ptr, pos, rhs.size()); }
		size_type	find_first_of( const Ch* psz, size_type pos, size_type count) const
		{
			if ( count > 0 && pos < m_size )
			{
				const Ch* const pV = m_ptr + m_size;
				for ( const Ch* pU = m_ptr + pos ; pU < pV ; ++pU )
					if ( traits_type::find(psz, count, *pU) != 0 )
						return pU - m_ptr;
			}
			return my_type::npos;
		}
		size_type	find_first_of(const Ch* psz, size_type pos = 0) const { return find_first_of(psz, pos, traits_type::length(psz)); }
		size_type	find_first_of(Ch ch, size_type pos ) const { return find_count_1(ch, pos); }
		size_type	find_first_of(Ch ch ) const { return find_pos_0_count_1(ch); }


		size_type	find_last_of(const my_type& rhs, size_type pos = my_type::npos) const { return find_last_of(rhs.m_ptr, pos, rhs.size()); }
		size_type	find_last_of(const Ch* psz, size_type pos, size_type count) const
		{
			if ( count > 0 && m_size > 0)
			{
				for ( const Ch* pU = m_ptr + ( pos < m_size ? pos : m_size - 1); ; --pU)
					if (traits_type::find(psz, count, *pU) != 0 )
						return pU - m_ptr;
					else if ( pU == m_ptr )
						break;
			}
			return my_type::npos;
		}
		size_type	find_last_of(const Ch* psz, size_type pos = my_type::npos) const { return find_last_of(psz, pos, traits_type::length(psz)); }
		size_type	find_last_of(Ch ch, size_type pos) const { return rfind_count_1(ch, pos); }
		size_type	find_last_of(Ch ch) const { return rfind_npos_count_1(ch); }

		size_type	find_first_not_of(const my_type& rhs, size_type pos = 0) const { return find_first_not_of(rhs.m_ptr, pos, rhs.size());	}
		size_type	find_first_not_of(const Ch* psz, size_type pos, size_type count) const
		{	
			if ( pos < m_size)
			{	
				const Ch* const pV = m_ptr + m_size;
				for (const Ch* pU = m_ptr + pos; pU < pV ; ++pU )
					if (traits_type::find(psz, count, *pU) == 0)
						return pU - m_ptr;
			}
			return my_type::npos;
		}

		size_type	find_first_not_of(const Ch* psz,	size_type pos = 0) const {	return find_first_not_of(psz, pos, traits_type::length(psz));	}
		size_type	find_first_not_of(Ch ch, size_type pos = 0) const	{	return find_first_not_of((const Ch*)&ch, pos, 1); }

		size_type	find_last_not_of(const my_type& rhs, size_type pos = my_type::npos) const {	return find_last_not_of(rhs.m_ptr, pos, rhs.size());	}

		size_type	find_last_not_of(const Ch* psz, size_type pos, size_type count) const
		{	
 			if ( m_size > 0 ) 
				for (const Ch* pU  = m_ptr + ( pos < m_size ? pos : m_size - 1); ; --pU)
					if ( traits_type::find(psz, count, *pU) == 0)
						return pU - m_ptr; 
					else if ( pU == m_ptr )
						break;
			return my_type::npos;
		}

		size_type	find_last_not_of(const Ch* psz, size_type pos = my_type::npos) const {	return find_last_not_of(psz, pos, traits_type::length(psz)); }
		size_type	find_last_not_of(Ch ch, size_type pos = my_type::npos) const {	return find_last_not_of((const Ch*)&ch, pos, 1); }

		my_type		substr(size_type pos = 0, size_type count = my_type::npos) const { return my_type(*this, pos, count, get_allocator()); }
		

		int			compare( const my_type& rhs) const { return compare(0, m_size, rhs.m_ptr, rhs.size()); }
		int			compare(size_type pos, size_type n0, const my_type& rhs) const { return compare(pos, n0, rhs, 0, my_type::npos); }
		int			compare(size_type pos, size_type n0, const my_type& rhs, size_type rpos, size_type count) const
		{
			if ( count > rhs.m_size - rpos )
				count = rhs.m_size - rpos;
			return compare(pos, n0, rhs.m_ptr + rpos, count);
		}
		
		int			compare(const Ch* psz) const { return compare(0, m_size, psz, traits_type::length(psz)); }
		int			compare(size_type pos, size_type n0, const Ch* psz) const { return compare(pos, n0, psz, traits_type::length(psz)); }
		int			compare(size_type pos, size_type n0, const Ch* psz, size_type count) const 
		{
			if ( n0 > m_size - pos )
				n0 = m_size - pos;
			size_type result = traits_type::compare( m_ptr + pos, psz, n0 < count ? n0 : count );

			return	( result != 0 ) ? (int) result :
					( n0 < count ) ? -1 :
					( n0 > count ) ? +1 : 0;
		}
		
		my_type&	assign(const my_type& rhs) { return assign(rhs, 0, npos); }			// 일반함수는 에뮬레이션 되지 않아서 구현복구한다..

		/*
		my_type&	assign(const_assign_ref(my_type) rhs) { return assign(rhs, 0, npos); }
		my_type&	assign(rv_ref(my_type) rval)
		{
			if ( this == &rval ) return *this;
			if ( m_ptr != ms_null) 
				alloc().deallocate(m_ptr, m_reserve + 1);	
			m_ptr = rval.m_ptr; m_size = rval.m_size; m_reserve = rval.m_reserve; 
			rval.m_ptr = ms_null;	rval.m_size = 0;	rval.m_reserve = 0;
			return *this;
		}
		*/

		my_type&	assign(const my_type& rhs, size_type pos, size_type count)
		{
			size_type num = rhs.size() - pos;
			if ( num > count )
				num = count;
			
			if ( num > 0 )
			{
				if ( this == &rhs)
					erase((size_type)(pos + num)), erase(0, pos);
				else
				{
					grow_up_assign(num); 
					traits_type::copy(m_ptr, rhs.m_ptr + pos, num );
					eos(num);
				}
			}
			else
				eos(0);

			return *this;
		}
		my_type&	assign(const Ch* psz) { return assign(psz, traits_type::length(psz) ); }
		my_type&	assign(const Ch* psz, size_type count)
		{
			if ( count > 0 )
			{
				if ( is_inside(psz) )
				{
					erase( (size_type) (psz + count - m_ptr) );
					erase( 0, psz - m_ptr );
				}
				else
				{
					grow_up_assign(count);
					traits_type::copy( m_ptr, psz, count );
					eos(count);
				}
			}
			else 
				eos(0);

			return *this;
		}

		template < class traits2, class Alloc2>
		my_type&	assign(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type&	assign(const rc_basic_string<Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type&	assign(const fixed_basic_string<Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template < class BuffSize2, class traits2, class Alloc2>
		my_type&	assign(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) {  return assign(rhs.c_str(), rhs.size());   }
		template<class It>
		my_type&	assign(const i3::literal_base_range<It>& rng) { return assign(rhs.c_str(), rhs.size());   }


		my_type&	assign(size_type count, Ch ch )
		{
			if ( count > 0 )
			{
				grow_up_assign(count);

				ch_assign( 0, count, ch );
			}
			eos(count);
			return *this;
		}
		
		template<class It>
		my_type&	assign(It first, It last)
		{
			return assign_tag(first, last, typename i3::iterator_traits<It>::iterator_category() );
		}
		
		my_type&	assign(iterator first, iterator last)
		{
			return replace( begin(), end(), first, last );
		}			

		my_type&	assign(const_iterator first, const_iterator last)
		{
			return replace( begin(), end(), first, last );
		}
		

		my_type&	append(const my_type& rhs) {	return append(rhs, 0, my_type::npos);	}
		my_type&	append(const my_type& rhs, size_type pos, size_type count)
		{
			size_type num = rhs.size() - pos;
			if ( count > num )
				count  = num;
			if ( count > 0 )
			{
				grow_up_append( m_size + count );
				traits_type::copy( m_ptr + m_size, rhs.m_ptr + pos, count );
				eos( m_size + count );
			}
			return *this;
		}
		my_type&	append(const Ch* psz) { return append(psz, traits_type::length(psz) ); }
		my_type&	append(const Ch* psz, size_type count)
		{
			if ( count > 0 )
			{
				if ( is_inside(psz) )
					return append(*this, psz - m_ptr, count);
				
				grow_up_append( m_size + count );
				traits_type::copy( m_ptr + m_size, psz, count );
				eos( m_size + count );
			}
			return *this;
		}

		template < class traits2, class Alloc2>
		my_type& append(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type& append(const rc_basic_string<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class traits2, class Alloc2>
		my_type& append(const fixed_basic_string<Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class BuffSize2, class traits2, class Alloc2>
		my_type& append(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) {  return append(rhs.c_str(), rhs.size());   }
		template < class It>
		my_type& append(const i3::literal_base_range<It>& rng) {  return append(rng.c_str(), rng.size());   }
				
		
		my_type&	append( size_type count, Ch ch )
		{
			if (count > 0 )
			{
				grow_up_append( m_size + count );

				ch_assign(m_size, count, ch );
				
				eos(m_size + count );
			}
			return *this;
		}

		template<class It>
		my_type&	append( It first, It last ) {	return append(first, last, typename i3::iterator_traits<It>::iterator_category() );	}
		
		my_type&	append(iterator first, iterator last){	return replace(end(), end(), first, last);	}
		my_type&	append(const_iterator first, const_iterator last){	return replace(end(), end(), first, last);	}

		my_type&	insert(size_type pos, const my_type& rhs){ 	return insert(pos, rhs, 0, my_type::npos );	}
		my_type&	insert(size_type pos, const my_type& rhs, size_type rpos, size_type count)
		{
			size_type num = rhs.size() - rpos;
			if (count > num )
				count = num;
			
			if (count > 0)
			{
				grow_up_append( m_size + count);
				
				traits_type::move( m_ptr + pos + count, m_ptr + pos, m_size - pos );

				if ( this == &rhs)
					traits_type::move( m_ptr + pos, m_ptr + ( pos < rpos ? rpos + count : rpos ), count );
				else
					traits_type::copy( m_ptr + pos, rhs.m_ptr + rpos, count );
								
				eos(m_size + count);
			}
			return *this;
		}
		my_type&	insert(size_type pos, const Ch* psz, size_type count)
		{
			if ( is_inside(psz) )
				return insert(pos, *this, psz - m_ptr, count);
			
			if ( count > 0 )
			{
				grow_up_append( m_size + count);
				traits_type::move( m_ptr + pos + count, m_ptr + pos, m_size - pos);
				traits_type::copy( m_ptr + pos, psz, count );
				eos(m_size + count);
			}
			return *this;
		}
		my_type&	insert(size_type pos, const Ch* psz) {	return insert(pos, psz, traits_type::length(psz)); }
		
		my_type&	insert(size_type pos, size_type count, Ch ch)
		{
			if ( count > 0 )
			{
				grow_up_append( m_size + count);
				traits_type::move( m_ptr + pos + count, m_ptr + pos, m_size - pos);
				ch_assign(pos, count, ch);
				eos(m_size + count);
			}
			return *this;
		}
		
		iterator	insert(const_iterator where) {	return insert(where, Ch());	}
		iterator	insert(const_iterator where, Ch ch)
		{
			size_type pos = ptr_diff(where, begin());
			insert(pos, 1, ch);
			return begin() + pos;
		}
		void		insert(const_iterator where, size_type count, Ch ch)
		{
			size_type pos = ptr_diff(where, begin());
			insert( pos, count, ch);
		}
		template<class It>
		void		insert(const_iterator where, It first, It last)
		{
			insert_impl(where, first, last, typename i3::iterator_traits<It>::iterator_category());
		}
		
		void		insert(const_iterator where, iterator first, iterator last)
		{
			replace(where, where, first, last);
		}

		void		insert(const_iterator where, const_iterator first, const_iterator last)
		{
			replace(where, where, first, last);
		}
		
		my_type&	erase( size_type pos = 0, size_type count = my_type::npos)
		{
			if ( count < m_size - pos )
			{
				if (count)
				{
					traits_type::move( m_ptr + pos, m_ptr + pos + count, m_size - pos - count );
					eos( m_size - count );
				}
			}
			else
				eos(pos);

			return *this;
		}
		
		iterator	erase(const_iterator where)
		{
			size_type count = ptr_diff(where, begin());
			erase( count, 1);
			return m_ptr + count;
		}
		iterator	erase(const_iterator first, const_iterator last)
		{
			size_type count = ptr_diff(first, begin());
			erase( count, ptr_diff(last , first) );
			return m_ptr + count;
		}



		my_type&	replace( size_type pos, size_type n0, const my_type& rhs)
		{
			return replace(pos, n0, rhs, 0, my_type::npos);
		}

		my_type&	replace( size_type pos, size_type n0, const my_type& rhs, size_type rpos, size_type count)
		{
			if ( n0 > m_size - pos )
				n0 = m_size - pos;
			
			size_type num = rhs.size() - rpos;
			if ( count > num )
				count  = num;
			
			size_type nm = m_size - n0 - pos;
			size_type newsize = m_size + count - n0;
			
			grow_up_append(newsize);

			if ( this != &rhs )
			{	// no overlap, just move down and copy in new stuff
				traits_type::move( m_ptr + pos + count , m_ptr + pos + n0, nm ); 
				traits_type::copy( m_ptr + pos, rhs.m_ptr + rpos, count );
			}
			else 
			if ( count <= n0 )
			{	// hole doesn't get larger, just copy in substring
				traits_type::move( m_ptr + pos, m_ptr + rpos, count );
				traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm);
			}
			else 
			if ( rpos <= pos )
			{	// hole gets larger, substring begins before hole
				traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm );
				traits_type::move( m_ptr + pos, m_ptr + rpos, count);
			}
			else
			if ( pos + n0 <= rpos )
			{	// hole gets larger, substring begins after hole
				traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm);
				traits_type::move( m_ptr + pos, m_ptr + rpos + count - n0, count);
			}
			else
			{	// hole gets larger, substring begins in hole
				traits_type::move( m_ptr + pos, m_ptr + rpos, n0);
				traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm);
				traits_type::move( m_ptr + pos + n0, m_ptr + rpos + count, count - n0);
			}
			eos(newsize);
			return *this;
		}
		
		my_type&	replace( size_type pos, size_type n0, const Ch* psz, size_type count)
		{
			if ( is_inside(psz) )
				return replace( pos, n0, *this, psz - m_ptr, count);
			
			if ( n0 > m_size - pos )
				n0 = m_size - pos;

			size_type nm = m_size - n0 - pos;

			if ( count < n0 )
				traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm);

			
			if ( count > 0 || n0 > 0 )
			{
				size_type newsize = m_size + count - n0;
				grow_up_append( newsize);
				
				if ( n0 < count )
					traits_type::move(m_ptr + pos + count, m_ptr + pos + n0, nm );
				traits_type::copy( m_ptr + pos, psz, count );
				eos(newsize);
			}

			return *this;
		}
		my_type&		replace(size_type pos, size_type n0, const Ch* psz )
		{
			return replace(pos, n0, psz, traits_type::length(psz));
		}
		my_type&		replace(size_type pos, size_type n0, size_type count, Ch ch)
		{	
			if (n0 > m_size - pos)
				n0 = m_size - pos;
			size_type nm = m_size - n0 - pos;

			if (count < n0 )
				traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm );
			if ( count > 0 || n0 > 0 )
			{
				size_type newsize = m_size + count - n0;
				grow_up_append( newsize);
				
				if ( n0 < count )
					traits_type::move( m_ptr + pos + count, m_ptr + pos + n0, nm);
				
				ch_assign( pos, count, ch);

				eos(newsize);
			}
			return *this;
		}

		my_type&		replace(const_iterator first, const_iterator last, const my_type& rhs)
		{
			return replace(ptr_diff(first, begin()), ptr_diff(last, first), rhs);			
		}
		my_type&		replace(const_iterator first, const_iterator last, const Ch* psz, size_type count)
		{
			return replace(ptr_diff(first, begin()), ptr_diff(last, first), psz, count);
		}
		my_type&		replace(const_iterator first, const_iterator last, const Ch* psz)
		{
			return replace(ptr_diff(first, begin()), ptr_diff(last, first), psz);
		}
		my_type&		replace(const_iterator first, const_iterator last, size_type count, Ch ch)
		{
			return replace(ptr_diff(first, begin()), ptr_diff(last, first), count, ch);
		}
		template<class It>
		my_type&		replace(const_iterator first, const_iterator last, It first2, It last2)
		{
			return replace(first, last, first2, last2, typename i3::iterator_traits<It>::iterator_category() );
		}

		my_type&		replace(const_iterator first, const_iterator last, iterator first2, iterator last2)
		{
			return replace(first, last, const_iterator(first2), const_iterator(last2) );
		}

		my_type&		replace(const_iterator first, const_iterator last, const_iterator first2, const_iterator last2)
		{
			if ( first2 == last2 )
				erase( ptr_diff(first, begin()), ptr_diff( last, first));
			else
				replace(ptr_diff(first, begin()), ptr_diff( last, first), &*first2, last2 - first2);
			return *this;
		}

	private:
		enum 
		{
			ALLOC_MASK = sizeof(Ch) <= 1 ? 0xF :
						 sizeof(Ch) <= 2 ? 0x7 :
						 sizeof(Ch) <= 4 ? 0x3 :
						 sizeof(Ch) <= 8 ? 1 : 0
		};
		
		void					assign_new( const my_type& rhs, size_type pos, size_type count )
		{
			size_type num = rhs.size() - pos;
			if ( count < num )
				num = count;
			grow_up_new(num);			//  m_reserve, m_ptr 초기화..
			if (num)
				traits_type::copy(m_ptr, rhs.m_ptr + pos, num );
			eos(num);					//  m_size 초기화..
		}
		void					assign_new( const Ch* psz, size_type count )
		{
			grow_up_new(count);
			if (count)
				traits_type::copy(m_ptr, psz, count);
			eos(count);
		}
		void					assign_new( size_type count, Ch ch)
		{
			grow_up_new(count);
			
			ch_assign(0, count, ch);

			eos(count);
		}
		template<class It>
		void					assign_new(It first, It last, i3::integral_iterator_tag )
		{
			assign_new((size_type)first, (Ch) last );
		}
		template<class It>
		void					assign_new(It first, It last, i3::input_iterator_tag )
		{
			size_type dist = (size_type) i3::distance( first, last);
			grow_up_new(dist);
			for ( Ch* ptr = m_ptr  ; first != last ; ++first , ++ptr )
				traits_type::assign(*ptr, (Ch)*first );
			eos(dist);
		}

		template<class It>
		my_type&					assign_tag(It count, It ch, i3::integral_iterator_tag)
		{
			return assign((size_type)count, (Ch) ch);
		}
		template<class It>
		my_type&					assign_tag(It first, It last, i3::input_iterator_tag)
		{
			return replace( begin(), end(), first, last );
		}	
		

		template<class It>
		my_type&					append(It first, It last, i3::integral_iterator_tag )
		{
			return append((size_type)first, (Ch)last);
		}
		
		template<class It>
		my_type&					append(It first, It last, i3::input_iterator_tag )
		{
			return replace(end(), end(), first, last);
		}
		

		void					grow_up_new( size_t newsize)			// newsize가 0인 것을 허용..
		{
			if ( newsize < 1 )
			{
				m_ptr		= ms_null;			// alloc().allocate( BUF_SIZE );		
				m_reserve	= 0;
			}
			else
			{
				size_type newReserve = newsize | ALLOC_MASK;
				m_ptr		= alloc().allocate(newReserve + 1);
				m_reserve	= newReserve;
			}
		}


		void					grow_up_assign( size_t newsize)
		{
			if ( newsize > m_reserve )
			{
				size_type newReserve = newsize | ALLOC_MASK;		// (16 or 8) 정렬에 알맞도록 크기를 좀더 늘림..
				if ( m_reserve / 2 > newReserve / 3 )				// 많이 늘어난 추가할당량이 아니면 50%가 늘어나도록 처리..
					newReserve = m_reserve + m_reserve / 2;

				Ch* ptr = alloc().allocate( newReserve + 1 );
				
				if (m_ptr != ms_null)
					alloc().deallocate(m_ptr, m_reserve + 1);
				m_ptr = ptr;
				m_reserve = newReserve;
			}
		}

		void					grow_up_append( size_t newsize)
		{
			if ( newsize > m_reserve )
			{
				size_type newReserve = newsize | ALLOC_MASK;		// (16 or 8) 정렬에 알맞도록 크기를 좀더 늘림..
				if ( m_reserve / 2 > newReserve / 3 )				// 많이 늘어난 추가할당량이 아니면 50%가 늘어나도록 처리..
					newReserve = m_reserve + m_reserve / 2;				
				Ch* ptr = alloc().allocate( newReserve + 1 );
				traits_type::copy( ptr, m_ptr, m_size );
				
				if (m_ptr != ms_null)
					alloc().deallocate(m_ptr, m_reserve + 1);
				m_ptr = ptr;
				m_reserve = newReserve;
			}
		}
		
		void					ch_assign(size_type pos, size_type count, Ch ch)
		{
			if ( count == 1 )
				traits_type::assign( m_ptr[pos], ch );
			else
				traits_type::assign( m_ptr + pos, count, ch);
		}
		
		template<class It>
		void					insert_impl(const_iterator where, It count, It ch, i3::integral_iterator_tag )
		{
			insert(where, (size_type)count, (Ch)ch);
		}
		
		template<class It>
		void					insert_impl(const_iterator where, It first, It last, i3::input_iterator_tag )
		{
			replace(where, where, first, last);
		}

		bool					is_inside(const Ch* psz) {	return  ( m_ptr <= psz && psz < m_ptr + m_size) ; }
		
		static size_type		ptr_diff(const_iterator p2, const_iterator p1) { assert(p2);	return p2 - p1; }
		
		template<class It>
		my_type&				replace(const_iterator first, const_iterator last, It count, It ch, i3::integral_iterator_tag)
		{
			return replace(first, last, (size_type) count, (Ch) ch);
		}
		template<class It>
		my_type&				replace(const_iterator first, const_iterator last, It first2, It last2, i3::input_iterator_tag)
		{
			my_type	rhs(first2, last2);
			replace(first, last, rhs);
			return *this;
		}

		void					eos()				{	traits_type::assign( m_ptr[m_size], Ch() ); }
		void					eos(size_type num)	{	traits_type::assign( m_ptr[m_size = num], Ch()); }

		//
		size_type				find_count_1(Ch ch, size_type pos) const
		{
			size_type nm;
			if ( pos < m_size && 1 <= ( nm = m_size - pos ) )
			{
				const Ch* pU = traits_type::find(m_ptr + pos, nm, ch );
				if (pU != 0)
					return pU - m_ptr;
			}
			return my_type::npos;
		}
		size_type				find_pos_0_count_1(Ch ch) const
		{
			if ( 0 < m_size )
			{
				const Ch* pU = traits_type::find(m_ptr, m_size, ch );
				if (pU != 0)
					return pU - m_ptr;
			}
			return my_type::npos;
		}
		size_type				rfind_count_1(Ch ch, size_type pos) const
		{
			if ( 0 < m_size )
			{
				const Ch* pU = m_ptr + ( pos < m_size - 1 ? pos : m_size - 1);
				for ( ; ; --pU)
					if ( traits_type::eq(*pU, ch) )
						return pU - m_ptr;
					else if ( pU == m_ptr )
						break;
			}
			return my_type::npos;
		}
		size_type				rfind_npos_count_1(Ch ch) const
		{
			if ( 0 < m_size )
			{
				const Ch* pU = m_ptr + m_size - 1;
				for ( ; ; --pU)
					if ( traits_type::eq(*pU, ch) )
						return pU - m_ptr;
					else if ( pU == m_ptr )
						break;
			}
			return my_type::npos;
		}

		__forceinline const allocator_type&		alloc() const { return allocator_base::get(); }
		__forceinline		 allocator_type&	alloc()  { return allocator_base::get(); }
		
		Ch*							m_ptr;
		size_type					m_size;				
		size_type					m_reserve;	
		
		static Ch*					ms_null;

		template<class C2, class T2, class A2> friend
		void force_initialize_i3string( i3::identity< basic_string<C2,T2,A2> > );
	};
	
	template<class Ch>
	class i3_basic_string_null_storage : public shared_ginst<i3_basic_string_null_storage<Ch> > 
	{	
	public:
		static const size_t array_size = (sizeof(Ch*) > sizeof(Ch)  )  ? sizeof(Ch*) / sizeof(Ch) : 1;
		i3_basic_string_null_storage() {  ::memset(data, 0, sizeof(Ch) * array_size ); }
		static Ch* get_data()			{ create_explicit();	return i()->data;   }
	private:
		Ch	data[  array_size   ]; 
	};

	namespace
	{
		struct _init_i3_basic_string_null_storage_ 
		{
			_init_i3_basic_string_null_storage_()
			{
				i3_basic_string_null_storage<char>::create_explicit();	i3_basic_string_null_storage<wchar_t>::create_explicit();
			}
		} _init_i3_basic_string_null_storage_var_;
	}

	template<class C, class T, class A>
	C*				basic_string<C, T, A>::ms_null = i3::i3_basic_string_null_storage<C>::get_data();

	template<class Ch,	class traits, class Alloc> inline
	void swap(basic_string<Ch, traits, Alloc>& Left, basic_string<Ch, traits, Alloc>& Right) {	Left.swap(Right);  }
	
	
	template<class C, class T, class A> inline
	void force_initialize_i3string( i3::identity< basic_string<C,T,A> > )
	{
		basic_string<C,T,A>::ms_null = i3::i3_basic_string_null_storage<C>::get_data();
	}

}

#pragma pack(pop)

#include "i3string_operator.h"


