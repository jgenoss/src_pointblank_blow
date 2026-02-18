#pragma once 


//
//  std::string계열과는 달리 동적저장소만..reserve공간없이..딱 들어맞게..
//	
#include "rc_string_fwd.h"

#include "char_traits.h"
#include "../pool/pool_allocator.h"
#include "../itl/ebo.h"
#include "../itl/iterator.h"
#include "../atomic/atomic.h"

namespace i3 
{
	template<class Ch, class traits, class Alloc>
	class fixed_basic_string;
	template<class Ch, class traits, class Alloc>
	class basic_string;
	template<class BuffSize, class Ch, class traits, class Alloc>
	class stack_basic_string;
	template<class It>
	class literal_base_range;

		
	template <class Ch, class traits, class Alloc>
	class rc_basic_string : private enable_ebo< typename rebind_wrap<Alloc, Ch>::type>
	{
	public:
		typedef rc_basic_string<Ch, traits, Alloc>				my_type;
		typedef enable_ebo< typename rebind_wrap<Alloc, Ch>::type>		allocator_base;

		typedef	traits											traits_type;
		typedef typename traits::char_type						value_type;
		typedef typename Alloc::template rebind<Ch>::other		allocator_type;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;

		typedef typename allocator_type::const_reference		reference;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::const_pointer			pointer;
		typedef typename allocator_type::const_pointer			const_pointer;

		typedef	typename const_pointer							iterator;				// vector와 마찬가지로 이터레이터 구현부는 제거..
		typedef typename const_pointer							const_iterator;
				
		typedef i3::reverse_iterator<iterator>					reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>			const_reverse_iterator;

		typedef typename allocator_type::pointer				non_const_pointer;

		static const size_type									npos = static_cast<size_type>(-1);
		
		struct  storage 
		{	
			enum 
			{ 	
				DUMMY_SIZE = ( sizeof(size_type) > sizeof(unsigned long) )
							? ( sizeof(size_type) * 2) / sizeof(Ch)
							: ( sizeof(unsigned long) + sizeof(size_type)  ) / sizeof(Ch) 
					
			}; 
			enum {  REF_MASK = 0x7FFFFFFF, FIXED_MASK = 0x80000000 };
			
			storage() : refcount(1), size(0)   {  traits_type::assign( ptr[0] , Ch() );  }
			storage(size_type size) : refcount(1), size(size)  {	traits_type::assign( ptr[size] , Ch() ); }
			
			bool						is_fixed_storage() const {	return (refcount & FIXED_MASK) != 0; }
			void						set_fixed_storage() const {	refcount |= unsigned long(FIXED_MASK); }

		//	mutable unsigned long		refcount;
			mutable i3::atomic_ulong 	refcount;			// unsigned long
			size_type					size; 
			Ch							ptr[1];				// 당연이 1보다 클것임..
		};

		typedef storage											storage_type;
	

		rc_basic_string() : m_storage(&ms_null_storage) { add_ref(m_storage); }
		rc_basic_string(const my_type& rhs ) : m_storage ( rhs.m_storage) {	add_ref(m_storage); }

		template<class traits2, class Alloc2>
		rc_basic_string(const basic_string<Ch, traits2, Alloc2>& rhs)  { assign_new( rhs.c_str(), rhs.size() );  }
		template<class BuffSize2, class traits2, class Alloc2>
		rc_basic_string(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs)  { assign_new( rhs.c_str(), rhs.size() );  }

		template<class traits2, class Alloc2>
		rc_basic_string(const ::std::basic_string<Ch, traits2, Alloc2>& rhs)  { assign_new( rhs.c_str(), rhs.size() );  }

		rc_basic_string(const fixed_basic_string<Ch, traits, Alloc>& rhs) : m_storage(rhs.m_dyn_string.m_storage) { add_ref(m_storage); }

		template<class It>
		rc_basic_string(const i3::literal_base_range<It>& rng)  { assign_new( rng.c_str(), rng.size() );  }


		rc_basic_string(const my_type& rhs, size_type pos, size_type count = my_type::npos ) {		assign_new(rhs, pos, count);	}
		rc_basic_string(const Ch* psz, size_type count) {	assign_new( psz, count);  }
		rc_basic_string(const Ch* psz) { assign_new(psz,  traits_type::length(psz) ); }
		rc_basic_string(size_type count, Ch ch )	{	assign_new(count, ch);	}
		template<class It>
		rc_basic_string(It first, It last) { assign_new( first, last, typename i3::iterator_traits<It>::iterator_category() ); }

		rc_basic_string(non_const_pointer first, non_const_pointer last ) {	assign_new(&*first, last - first);	}	// const Ch* , count //
		rc_basic_string(const_iterator first, const_iterator last ) {	assign_new(&*first, last - first);	}	// const Ch* , count //

		rc_basic_string(const storage* pstorage) : m_storage(pstorage) { add_ref(pstorage); }
		
		~rc_basic_string(); 
		
		my_type&	operator=(const my_type& rhs)	{ return assign(rhs); }
		my_type&	operator=(const Ch*		psz )	{ return assign(psz); }

		template<class traits2, class Alloc2>
		my_type&	operator=(const basic_string<Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.size() );  }

		template<class BuffSize2, class traits2, class Alloc2>
		my_type&	operator=(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.size() );  }

		template<class traits2, class Alloc2>
		my_type&	operator=(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.size() );  }
		
		my_type&	operator=(const fixed_basic_string<Ch, traits, Alloc>& rhs) {	return assign(rhs.m_dyn_string.m_storage);  }

		template<class It>
		my_type&	operator=(const i3::literal_base_range<It>& rng) { return assign(rng.c_str(), rng.size() );  }


		my_type&	operator=(Ch ch)				{ return assign(1, ch); }
		my_type&	operator=(const storage_type* pstorage) { return assign(pstorage);    }

		my_type&	operator+=(const my_type& rhs)	{ return append(rhs); }
		my_type&	operator+=(const Ch*	psz)	{ return append(psz); }
		my_type&	operator+=(Ch ch)				{ return append((size_type)1, ch); }

		template<class traits2, class Alloc2>
		my_type&	operator+=(const basic_string<Ch, traits2, Alloc2>& rhs) { return append(rhs.c_str(), rhs.size() );  }

		template<class BuffSize2, class traits2, class Alloc2>
		my_type&	operator+=(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) { return append(rhs.c_str(), rhs.size() );  }

		template<class traits2, class Alloc2>
		my_type&	operator+=(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) { return append(rhs.c_str(), rhs.size() );  }

		my_type&	operator+=(const fixed_basic_string<Ch, traits, Alloc>& rhs) {	return append(rhs.c_str, rhs.size());  }

		template<class It>
		my_type&	operator+=(const i3::literal_base_range<It>& rng) { return append(rng.c_str(), rng.size() );  }


		static const storage*	get_null_storage()  { return &ms_null_storage; }
		const storage*			get_storage() const			{ return m_storage; }
		unsigned long			ref_count() const	{ return (m_storage->refcount & storage::REF_MASK); }
		bool					is_null_string() const { return m_storage == &ms_null_storage; }	

		allocator_type			get_allocator() const { return alloc(); }
		void					clear()			{	set_null_storage();	}		// 이렇게만 하면 충분할것 같은데..

		const_iterator			begin() const	{ return m_storage->ptr; }
		const_iterator			end() const		{ return m_storage->ptr + size(); }
		const_reverse_iterator	rbegin() const	{ return const_reverse_iterator(end()); }
		const_reverse_iterator	rend() const	{ return const_reverse_iterator(begin()); }
		const_reference			at(size_type pos) const { return m_storage->ptr[pos]; }
		const_reference			operator[](size_type pos) const { return m_storage->ptr[pos]; }

		void					push_back(Ch ch)	{	insert( end(), ch); }

		const Ch*				c_str() const		{ return m_storage->ptr; }
		const Ch*				data() const		{ return m_storage->ptr; }
		size_type				size() const		{ return m_storage->size; }
		size_type				length() const		{ return m_storage->size; }
		size_type				max_size() const	{ return allocator_type().max_size() - 1; }

		void					resize(size_type newsize) { resize(newsize, Ch()); }
		void					resize(size_type newsize, Ch ch)
		{
			if ( newsize <= size() )
				erase(newsize);
			else
				append(newsize - size(), ch);
		}
		bool					empty() const { return size() == 0; }

		size_type	copy(Ch* dest, size_type count, size_type pos = 0 ) const{		return	_Copy_s(dest, count, count, pos);	}
		
		size_type	_Copy_s(Ch* dest, size_type destsize, size_type count, size_type pos = 0 ) const
		{
			if ( count > size() - pos )
				count  = size() - pos;
			traits_type::_Copy_s(dest, destsize, c_str() + pos, count);
			return count;
		}
		
		my_type&	assign(const storage* pstorage) { set_storage_addref(pstorage); return *this; }

		my_type&	assign(const my_type& rhs) { set_storage_addref(rhs.m_storage);	return *this; }

		my_type&	assign(const my_type& rhs, size_type pos, size_type count)
		{
			size_type num = rhs.size() - pos;
			if ( num > count )
				num = count;
			
			if ( num > 0 )
			{
				if ( num == rhs.size() ) 
				{
					set_storage_addref( rhs.m_storage );		return *this; 
				}
				
				storage* pNew = create_storage(num);
				traits_type::copy(m_ptr(pNew), rhs.c_str() + pos, num );
				set_storage(pNew);
			}
			else
			{
				set_null_storage();
			}
			return *this;
		}

		my_type&	assign(const Ch* psz) { assign(psz, traits_type::length(psz) ); return *this; }
		
		template<class traits2, class Alloc2>
		my_type&	assign(	const rc_basic_string<Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.length() );   }

		my_type&	assign(const Ch* psz, size_type count)
		{
			if ( count > 0 )
			{
				if ( c_str() == psz && count == size() )
					return *this;

				storage* pNew = create_storage(count);
				traits_type::copy( m_ptr(pNew), psz, count );
				set_storage(pNew);
			}
			else 
			{
				set_null_storage();
			}

			return *this;
		}

		template<class traits2, class Alloc2>
		my_type&	assign(const basic_string<Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.size() );  }

		template<class BuffSize2, class traits2, class Alloc2>
		my_type&	assign(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.size() );  }

		template<class traits2, class Alloc2>
		my_type&	assign(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) { return assign(rhs.c_str(), rhs.size() );  }

		my_type&	assign(const fixed_basic_string<Ch, traits, Alloc>& rhs) {	return assign(rhs.m_dyn_string.m_storage);  }

		template<class It>
		my_type&	assign(const i3::literal_base_range<It>& rng) { return assign(rng.c_str(), rng.size() );  }


		my_type&	assign(size_type count, Ch ch )
		{
			if ( count > 0 )
			{
				storage* pNew = create_storage(count);
				ch_assign(m_ptr(pNew), 0, count, ch);
				set_storage(pNew);
			}
			else
			{
				set_null_storage();
			}
			return *this;
		}
		
		template<class It>
		my_type&	assign(It first, It last)
		{
			assign_tag(first, last, typename i3::iterator_traits<It>::iterator_category() );	return *this;
		}
		
		my_type&	assign(non_const_pointer first, non_const_pointer last)
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
				storage* pNew = create_storage(size() + count);
				traits_type::copy( m_ptr(pNew), c_str(), size() );
				traits_type::copy( m_ptr(pNew) + size(), rhs.c_str() + pos, count );
				set_storage(pNew);
			}
			return *this;
		}

		my_type&		append(const Ch* psz) { return append(psz, traits_type::length(psz) ); }
		my_type&		append(const Ch* psz, size_type count)
		{
			if ( count > 0 )
			{
				storage* pNew = create_storage(size() + count);
				traits_type::copy( m_ptr(pNew), c_str(), size() );
				traits_type::copy( m_ptr(pNew) + size(), psz, count );
				set_storage(pNew);
			}
			return *this;
		}

		template<class traits2, class Alloc2>
		my_type&	append(const basic_string<Ch, traits2, Alloc2>& rhs) { return append(rhs.c_str(), rhs.size() );  }

		template<class BuffSize2, class traits2, class Alloc2>
		my_type&	append(const stack_basic_string<BuffSize2, Ch, traits2, Alloc2>& rhs) { return append(rhs.c_str(), rhs.size() );  }

		template<class traits2, class Alloc2>
		my_type&	append(const ::std::basic_string<Ch, traits2, Alloc2>& rhs) { return append(rhs.c_str(), rhs.size() );  }

		my_type&	append(const fixed_basic_string<Ch, traits, Alloc>& rhs) {	return append(rhs.c_str, rhs.size());  }

		template<class It>
		my_type&	append(const i3::literal_base_range<It>& rng) { return append(rng.c_str(), rng.size() );  }


		my_type&	append( size_type count, Ch ch )
		{
			if (count > 0 )
			{
				storage* pNew = create_storage(size() + count);
				traits_type::copy( m_ptr(pNew), c_str(), size() );
				ch_assign( m_ptr(pNew), size(), count, ch);
				set_storage(pNew);
			}
			return *this;
		}

		template<class It>
		my_type&	append( It first, It last ) {	return append(first, last, typename i3::iterator_traits<It>::iterator_category() );	}

		my_type&	append(non_const_pointer first, non_const_pointer last){	return replace(end(), end(), first, last);	}
		my_type&	append(const_iterator first, const_iterator last){	return replace(end(), end(), first, last);	}


		my_type&	insert(size_type pos, const my_type& rhs){ return	insert(pos, rhs, 0, my_type::npos );	}

		my_type&	insert(size_type pos, const my_type& rhs, size_type rpos, size_type count)
		{
			size_type num = rhs.size() - rpos;
			if (count > num )
				count = num;
			
			if (count > 0)
			{
				storage* pNew = create_storage(size() + count );
				
				traits_type::copy( m_ptr(pNew) , c_str(), pos );
				traits_type::copy( m_ptr(pNew) + pos, rhs.c_str() + rpos, count);
				traits_type::copy( m_ptr(pNew) + pos + count, c_str() + pos, size() - pos);
				set_storage(pNew);
			}
			return *this;
		}


		my_type&	insert(size_type pos, const Ch* psz, size_type count)
		{
			if ( count > 0 )
			{
				storage* pNew = create_storage( size() + count );
				traits_type::copy( m_ptr(pNew) , c_str(), pos );
				traits_type::copy( m_ptr(pNew) + pos, psz, count);
				traits_type::copy( m_ptr(pNew) + pos + count, c_str() + pos, size() - pos);
				set_storage(pNew);
			}
			return *this;
		}

		my_type&	insert(size_type pos, const Ch* psz) {	return insert(pos, psz, traits_type::length(psz)); }
		
		my_type&	insert(size_type pos, size_type count, Ch ch)
		{
			if ( count > 0 )
			{
				storage* pNew = create_storage(	size() + count);
				traits_type::copy( m_ptr(pNew), c_str(), pos );
				ch_assign( m_ptr(pNew), pos, count, ch);
				traits_type::copy( m_ptr(pNew) + pos + count, c_str() + pos, size() - pos);
				set_storage(pNew);
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
	
		void		insert(const_iterator where, non_const_pointer first, non_const_pointer last)
		{
			replace(where, where, first, last);
		}

		void		insert(const_iterator where, const_iterator first, const_iterator last)
		{
			replace(where, where, first, last);
		}
		

		void					swap( my_type& rhs)
		{
			if ( this->m_storage == rhs.m_storage) 
				return;

			const storage* pTmp = m_storage;
			m_storage = rhs.m_storage;
			rhs.m_storage = pTmp;			// 카운팅 자체는 변하지 않음..
		}
		
		size_type				find(const my_type& rhs, size_type pos = 0) const {	return	find(rhs.c_str(), pos, rhs.size() );	}
		size_type				find(const Ch* psz, size_type pos, size_type count) const
		{
			if ( psz == c_str() && count == size() )			// 예외적인 얼리아웃..그외에는 통하지 않는다..
				return 0;

			if ( count == 0 && pos <= size() )
				return pos;
			
			size_type nm;
			if ( pos < size() && count <= ( nm = size() - pos ) )
			{
				const Ch* pU, * pV;
				for ( nm -= count - 1, pV = c_str() + pos ; ( pU = traits_type::find(pV, nm, *psz) ) != 0; 
					nm -= pU - pV + 1, pV = pU + 1 )
					if ( traits_type::compare(pU, psz, count ) == 0 )
						return pU - c_str();
			}
			return my_type::npos;
		}
		size_type				find(const Ch* psz, size_type pos = 0) const {	return find(psz, pos, traits_type::length(psz)); }
//		size_type				find(Ch ch, size_type pos = 0) const { return find((const Ch*)&ch, pos, 1); }
		size_type				find(Ch ch, size_type pos) const { return find_count_1(ch, pos); }
		size_type				find(Ch ch) const { return find_pos_0_count_1(ch); }

		size_type				rfind(const my_type& rhs, size_type pos = my_type::npos) const {	return rfind( rhs.m_ptr, pos, rhs.size());	}
		size_type				rfind(const Ch* psz, size_type pos, size_type count) const
		{
			if ( psz == c_str() && count == size() )		// 예외적인 얼리아웃..그외에는 통하지 않는다..
				return 0;

			if (count == 0) 
				return ( pos < size() ? pos : size() );

			if ( count <= size() )
			{
				const Ch* pU = c_str() + ( pos < size() - count ? pos : size() - count);
				for ( ; ; --pU)
					if ( traits_type::eq(*pU, *psz) && traits_type::compare(pU, psz, count) == 0 )
						return pU - c_str();
					else if ( pU == c_str() )
						break;
			}
			return my_type::npos;
		}
		size_type				rfind(const Ch* psz, size_type pos = my_type::npos) const { return rfind(psz, pos, traits_type::length(psz)); }
//		size_type				rfind(Ch ch, size_type pos = my_type::npos) const { return rfind((const Ch*)&ch, pos, 1); }
		size_type				rfind(Ch ch, size_type pos) const { return rfind_count_1(ch, pos); }
		size_type				rfind(Ch ch) const { return rfind_npos_count_1(ch); }


		size_type	find_first_of( const my_type& rhs, size_type pos = 0) const { return find_first_of(rhs.m_ptr, pos, rhs.size()); }
		size_type	find_first_of( const Ch* psz, size_type pos, size_type count) const
		{
			if ( count > 0 && pos < size() )
			{
				const Ch* const pV = c_str() + size();
				for ( const Ch* pU = c_str() + pos ; pU < pV ; ++pU )
					if ( traits_type::find(psz, count, *pU) != 0 )
						return pU - c_str();
			}
			return my_type::npos;
		}

		size_type	find_first_of(const Ch* psz, size_type pos = 0) const { return find_first_of(psz, pos, traits_type::length(psz)); }
//		size_type	find_first_of(Ch ch, size_type pos = 0 ) const { return find((const Ch*)&ch, pos, 1); }
		size_type	find_first_of(Ch ch, size_type pos) const { return find_count_1(ch, pos); }
		size_type	find_first_of(Ch ch) const { return find_pos_0_count_1(ch); }

		size_type	find_last_of(const my_type& rhs, size_type pos = my_type::npos) const { return find_last_of(rhs.m_ptr, pos, rhs.size()); }
		size_type	find_last_of(const Ch* psz, size_type pos, size_type count) const
		{
			if ( count > 0 && size() > 0)
			{
				for ( const Ch* pU = c_str() + ( pos < size() ? pos : size() - 1); ; --pU)
					if (traits_type::find(psz, count, *pU) != 0 )
						return pU - c_str();
					else if ( pU == c_str() )
						break;
			}
			return my_type::npos;
		}
		size_type	find_last_of(const Ch* psz, size_type pos = my_type::npos) const { return find_last_of(psz, pos, traits_type::length(psz)); }
//		size_type	find_last_of(Ch ch, size_type pos = my_type::npos) const { return rfind((const Ch*)&ch, pos, 1); }
		size_type	find_last_of(Ch ch, size_type pos) const { return rfind_count_1(ch, pos); }
		size_type	find_last_of(Ch ch) const { return rfind_npos_count_1(ch); }


		size_type	find_first_not_of(const my_type& rhs, size_type pos = 0) const { return find_first_not_of(rhs.m_ptr, pos, rhs.size());	}
		size_type	find_first_not_of(const Ch* psz, size_type pos, size_type count) const
		{	
			if ( pos < size() )
			{	
				const Ch* const pV = c_str() + size();
				for (const Ch* pU = c_str() + pos; pU < pV ; ++pU )
					if (traits_type::find(psz, count, *pU) == 0)
						return pU - c_str();
			}
			return my_type::npos;
		}

		size_type	find_first_not_of(const Ch* psz,	size_type pos = 0) const {	return find_first_not_of(psz, pos, traits_type::length(psz));	}
		size_type	find_first_not_of(Ch ch, size_type pos = 0) const	{	return find_first_not_of((const Ch*)&ch, pos, 1); }

		size_type	find_last_not_of(const my_type& rhs, size_type pos = my_type::npos) const {	return find_last_not_of(rhs.m_ptr, pos, rhs.size());	}

		size_type	find_last_not_of(const Ch* psz, size_type pos, size_type count) const
		{	
 			if ( size() > 0 ) 
				for (const Ch* pU  = c_str() + ( pos < size() ? pos : size() - 1); ; --pU)
					if ( traits_type::find(psz, count, *pU) == 0)
						return pU - c_str(); 
					else if ( pU == c_str() )
						break;
			return my_type::npos;
		}

		size_type	find_last_not_of(const Ch* psz, size_type pos = my_type::npos) const {	return find_last_not_of(psz, pos, traits_type::length(psz)); }
		size_type	find_last_not_of(Ch ch, size_type pos = my_type::npos) const {	return find_last_not_of((const Ch*)&ch, pos, 1); }

		my_type		substr(size_type pos = 0, size_type count = my_type::npos) const { 	return my_type(*this, pos, count); }

		bool		equal_to( const my_type& rhs ) const 
		{	
			if ( c_str() == rhs.c_str() ) 
				return true;
			if ( size() != rhs.size() )
				return false;
			return traits_type::compare( c_str(), rhs.c_str(), size() ) == 0;
		}

		bool		equal_to(const Ch* psz) const	
		{	
			if ( psz == c_str() ) 
				return true;
			if ( size() != traits_type::length(psz) )
				return false;

			return traits_type::compare( c_str(), psz, size() ) == 0;
		}				

		template< template<class, class, class> class string_ctn, class C,class T, class A>
		bool		equal_to(const string_ctn<C,T,A>& str ) 
		{  
			if ( c_str() == str.c_str() ) 
				return true;

			if ( size() != str.size() )
				return false;

			return traits_type::compare( c_str(), str.c_str(), size() ) == 0; 
		}


		int			compare( const my_type& rhs) const 
		{ 
			if ( m_storage == rhs.m_storage )
				return 0;
			return compare(0, size(), rhs.c_str(), rhs.size()); 
		}

		int			compare(size_type pos, size_type n0, const my_type& rhs) const 
		{ 
			if ( m_storage == rhs.m_storage && n0 == size() )
				return 0;
			return compare(pos, n0, rhs, 0, my_type::npos); 
		}

		int			compare(size_type pos, size_type n0, const my_type& rhs, size_type rpos, size_type count) const
		{
			if ( count > rhs.size() - rpos )
				count = rhs.size() - rpos;
			if ( m_storage == rhs.m_storage && pos == rpos && n0 == count )
				return 0;

			return compare(pos, n0, rhs.c_str() + rpos, count);
		}
		
		int			compare(const Ch* psz) const 
		{ 
			if ( psz == c_str() )
				return 0;
			return compare(0, size(), psz, traits_type::length(psz)); 
		}

		int			compare(size_type pos, size_type n0, const Ch* psz) const 
		{ 
			if ( c_str() + pos == psz && pos + n0 == size() )
				return 0;

			return compare(pos, n0, psz, traits_type::length(psz)); 
		}
		int			compare(size_type pos, size_type n0, const Ch* psz, size_type count) const 
		{
			if ( n0 > size() - pos )
				n0 = size() - pos;

			if ( c_str() + pos == psz && n0 == count )
				return 0;

			size_type result = traits_type::compare( c_str() + pos, psz, n0 < count ? n0 : count );

			return	( result != 0 ) ? (int) result :
					( n0 < count ) ? -1 :
					( n0 > count ) ? +1 : 0;
		}
//////////////////////////////////////////////////////////////////		

		my_type&	erase( size_type pos = 0, size_type count = my_type::npos)
		{
			size_type num = size() - pos;
			if ( count > num )
				count  = num;

			if ( count )
			{
				if (count == size())
					set_null_storage();
				else
				{
					storage* pNew = create_storage( size() - count );
					traits_type::copy( m_ptr(pNew), c_str() , pos);
					traits_type::copy( m_ptr(pNew) + pos , c_str() + pos + count, size() - pos - count);
					set_storage( pNew );
				}
			}
			return *this;
		}
					
		iterator	erase(const_iterator where)
		{
			size_type count = ptr_diff(where, begin());
			erase( count, 1);
			return c_str() + count;
		}

		iterator	erase(const_iterator first, const_iterator last)
		{
			size_type count = ptr_diff(first, begin());
			erase( count, ptr_diff(last , first) );
			return c_str() + count;
		}
///////////////////
		my_type&	replace( size_type pos, size_type n0, const my_type& rhs) {	return replace(pos, n0, rhs, 0, my_type::npos);	}

		my_type&	replace( size_type pos, size_type n0, const my_type& rhs, size_type rpos, size_type count)
		{
			// n0는 삭제, count는 삽입이라고 생각하면 됨..

			if ( n0 > size() - pos )
				n0 = size() - pos;
			
			size_type num = rhs.size() - rpos;
			if ( count > num )
				count  = num;
			
			if ( n0 == 0 &&	count == 0 )					// 아무런일없음..
				return *this;

			if ( n0 == size() && count == rhs.size() )		// 전체 치환..
			{
				set_storage_addref(rhs.m_storage);
				return *this;
			}
			
			size_type newsize = size() + count - n0;
			if ( newsize == 0)								// 전체삭제인경우..
			{
				set_null_storage();
				return *this;
			}

			size_type nm = size() - n0 - pos;

			storage* pNew = create_storage(newsize);
			traits_type::copy( m_ptr(pNew), c_str(), pos );
			traits_type::copy( m_ptr(pNew) + pos, rhs.c_str() + rpos, count);
			traits_type::copy( m_ptr(pNew) + pos + count, c_str() + pos + n0, nm );
			set_storage(pNew);
			return *this;
		}

		my_type&	replace( size_type pos, size_type n0, const Ch* psz, size_type count)
		{
			if ( is_inside(psz) )
			{
				return replace( pos, n0, *this, psz - c_str(), count);	
			}
			
			if ( n0 > size() - pos )
				n0 = size() - pos;

			if ( n0 == 0 && count == 0 )
				return *this;

			size_type newsize = size() + count - n0;
			if (newsize == 0)
			{
				set_null_storage();		return *this;
			}

			size_type nm = size() - n0 - pos;
			
			storage* pNew = create_storage(newsize);
			traits_type::copy( m_ptr(pNew), c_str(), pos );
			traits_type::copy( m_ptr(pNew) + pos, psz, count);
			traits_type::copy( m_ptr(pNew) + pos + count, c_str() + pos + n0, nm );
			set_storage(pNew);
			return *this;
		}

		my_type&		replace(size_type pos, size_type n0, const Ch* psz ) {	return replace(pos, n0, psz, traits_type::length(psz)); }
		my_type&		replace(size_type pos, size_type n0, size_type count, Ch ch)
		{	
			if (n0 > size() - pos)
				n0 = size() - pos;
			
			if ( n0 == 0 && count == 0 )
				return *this;

			size_type newsize = size() + count - n0;
			
			if ( newsize == 0 )
			{
				set_null_storage();		return *this;
			}
			
			size_type nm = size() - n0 - pos;

			storage* pNew = create_storage(newsize);
			traits_type::copy( m_ptr(pNew), c_str(), pos );
			ch_assign( m_ptr(pNew), pos, count, ch);
			traits_type::copy( m_ptr(pNew) + pos + count, c_str() + pos + n0, nm );
			set_storage(pNew);
			return *this;
		}

		my_type&		replace(const_iterator first, const_iterator last, const my_type& rhs) {	return replace(ptr_diff(first, begin()), ptr_diff(last, first), rhs);	}
		my_type&		replace(const_iterator first, const_iterator last, const Ch* psz, size_type count) { return replace(ptr_diff(first, begin()), ptr_diff(last, first), psz, count); }
		my_type&		replace(const_iterator first, const_iterator last, const Ch* psz) { return replace(ptr_diff(first, begin()), ptr_diff(last, first), psz); }
		my_type&		replace(const_iterator first, const_iterator last, size_type count, Ch ch) { return replace(ptr_diff(first, begin()), ptr_diff(last, first), count, ch); }
		template<class It>
		my_type&		replace(const_iterator first, const_iterator last, It first2, It last2)	{ return replace(first, last, first2, last2, typename i3::iterator_traits<It>::iterator_category() ); }


		my_type&		replace(const_iterator first, const_iterator last, non_const_pointer first2, non_const_pointer last2)
		{
			return replace(first, last, const_iterator(first2), const_iterator(last2) );
		}

		my_type&	replace(const_iterator first, const_iterator last, const_iterator first2, const_iterator last2)
		{
			if ( first2 == last2 )
				erase( ptr_diff(first, begin()), ptr_diff( last, first));
			else
				replace(ptr_diff(first, begin()), ptr_diff( last, first), &*first2, last2 - first2);

			return *this;
		}



	private:
	
		bool				dec_ref_my_storage()   {  return ( (--m_storage->refcount) & storage::REF_MASK ) == 0 ; }
		static bool			dec_ref_storage(const storage* pStorage) { return ( (--pStorage->refcount) & storage::REF_MASK ) == 0 ; }

		void				deallocate_my_storage()  {  alloc().deallocate( reinterpret_cast<Ch*>(const_cast<storage*>(m_storage) ), storage::DUMMY_SIZE + m_storage->size + 1 ); }
		void				deallocate_storage(const storage* pStorage) 
			{  alloc().deallocate( reinterpret_cast<Ch*>(const_cast<storage*>(pStorage) ), storage::DUMMY_SIZE + pStorage->size + 1 ); }

		bool				is_fixed_storage() const { return m_storage->is_fixed_storage(); }
		static bool			is_fixed_storage(const storage* pStorage) { return pStorage->is_fixed_storage(); }

		static void			add_ref(const storage* pStorage) {  ++pStorage->refcount; }
		void				release(const storage* pStorage);


		static Ch*			m_ptr(storage* pNew) { return pNew->ptr; }
		
		storage*			create_storage(size_type num) 
		{	
			Ch* pNew = alloc().allocate( storage::DUMMY_SIZE + num + 1 );
			return new (pNew) storage(num);
		}

		void				set_null_storage()
		{
			add_ref(&ms_null_storage);		release(m_storage);		m_storage = &ms_null_storage;
		}
		void				set_storage(const storage* pNew) {	release(m_storage);		m_storage = pNew;	}
		
		void				set_storage_addref(const storage* pRhs) { add_ref(pRhs);	release(m_storage);	m_storage = pRhs; }

	
		void		assign_new( const my_type& rhs, size_type pos, size_type count )
		{
			size_type num = rhs.size() - pos;
			if ( count < num )
				num = count;
			
			if ( num == rhs.size() )
			{
				m_storage = rhs.m_storage;	add_ref(m_storage);
			}
			else
			if ( !num )
			{
				m_storage = &ms_null_storage;	add_ref(m_storage);
			}
			else
			{
				storage* pNew = create_storage(num);
				traits_type::copy( m_ptr(pNew), rhs.c_str() + pos, num );
				m_storage = pNew;
			}
		}

		void		assign_new( const Ch* psz, size_type count )
		{
			if (!count)
			{
				m_storage = &ms_null_storage;	add_ref(m_storage);
			}
			else
			{
				storage* pNew = create_storage(count);
				traits_type::copy( m_ptr(pNew), psz, count);
				m_storage = pNew;
			}
		}

		void					assign_new( size_type count, Ch ch)
		{
			if (!count)
			{
				m_storage = &ms_null_storage;	add_ref(m_storage);
			}
			else
			{
				storage* pNew =  create_storage(count);
				ch_assign(m_ptr(pNew), 0, count, ch);
				m_storage = pNew;
			}
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
			
			if (dist == 0 )
			{
				m_storage = &ms_null_storage;		add_ref(m_storage);
			}
			else
			{
				storage* pNew = create_storage(dist);

				for ( Ch* ptr = m_ptr(pNew)  ; first != last ; ++first , ++ptr )
					traits_type::assign(*ptr, (Ch)*first );
				
				m_storage = pNew;
			}
				
		}

		template<class It>
		void	assign_tag(It count, It ch, i3::integral_iterator_tag)	{		assign((size_type)count, (Ch) ch);	}
		template<class It>
		void	assign_tag(It first, It last, i3::input_iterator_tag)	{	replace( begin(), end(), first, last );	}	
		

		template<class It>
		void	append(It first, It last, i3::integral_iterator_tag ) { append((size_type)first, (Ch)last);	}
		
		template<class It>
		void	append(It first, It last, i3::input_iterator_tag )	{	replace(end(), end(), first, last);	}
		
		
		static void				ch_assign(Ch* ptr, size_type pos, size_type count, Ch ch)
		{
			if (count == 1)
				traits_type::assign( ptr[pos], ch );
			else
				traits_type::assign( ptr + pos, count, ch);
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

		bool					is_inside(const Ch* psz) {	return  ( c_str() <= psz && psz < c_str() + size() ) ; }
		
		static size_type		ptr_diff(const_iterator p2, const_iterator p1) { assert(p2);	return p2 - p1; }
		
		template<class It>
		void		replace(const_iterator first, const_iterator last, It count, It ch, i3::integral_iterator_tag){	replace(first, last, (size_type) count, (Ch) ch); }
		template<class It>
		void		replace(const_iterator first, const_iterator last, It first2, It last2, i3::input_iterator_tag)
		{
			my_type	rhs(first2, last2);
			replace(first, last, rhs);
		}
		
		size_type				find_count_1(Ch ch, size_type pos) const
		{
			size_type nm;
			if ( pos < size() && 1 <= ( nm = size() - pos ) )
			{
				const Ch* pU = traits_type::find(c_str() + pos, nm, ch );
				if (pU != 0)
					return pU - c_str();
			}
			return my_type::npos;
		}
		size_type				find_pos_0_count_1(Ch ch) const
		{
			if ( 0 < size() )
			{
				const Ch* pU = traits_type::find(c_str(), size(), ch );
				if (pU != 0)
					return pU - c_str();
			}
			return my_type::npos;
		}
		size_type				rfind_count_1(Ch ch, size_type pos) const
		{
			if ( 0 < size() )
			{
				const Ch* pU = c_str() + ( pos < size() - 1 ? pos : size() - 1);
				for ( ; ; --pU)
					if ( traits_type::eq(*pU, ch) )
						return pU - c_str();
					else if ( pU == c_str() )
						break;
			}
			return my_type::npos;
		}
		size_type				rfind_npos_count_1(Ch ch) const
		{
			if ( 0 < size() )
			{
				const Ch* pU = c_str() + size() - 1;
				for ( ; ; --pU)
					if ( traits_type::eq(*pU, ch) )
						return pU - c_str();
					else if ( pU == c_str() )
						break;
			}
			return my_type::npos;
		}

		__forceinline	const allocator_type&	alloc() const { return allocator_base::get(); }
		__forceinline			allocator_type&	alloc()  { return allocator_base::get(); }

//		allocator_type				m_alloc;
		const storage*				m_storage;			// 레퍼런스카운팅..
		
		static const storage		ms_null_storage;


	};
	
	template <class Ch, class traits, class Alloc>
	const typename rc_basic_string<Ch, traits, Alloc>::storage			rc_basic_string<Ch, traits, Alloc>::ms_null_storage;

	

	template<class Ch,	class traits, class Alloc> inline
	void swap(rc_basic_string<Ch, traits, Alloc>& Left, rc_basic_string<Ch, traits, Alloc>& Right) {	Left.swap(Right);  }
	

	
}

#include "global_string_table.h"

namespace i3
{
	template<class Ch, class T, class A> inline
		rc_basic_string<Ch, T, A>::~rc_basic_string() 
	{	
		typedef global_string_table<Ch, T, A>			table_type;

		if ( dec_ref_my_storage() )
		{
			if ( is_fixed_storage() )
				table_type::i()->erase(m_storage);
			deallocate_my_storage();
		}
	}	// 할당된 것 이외에는 처리할것 없음..


	template<class Ch, class T, class A> inline
		void rc_basic_string<Ch, T, A>::release(const storage* pStorage) 
	{  
		typedef global_string_table<Ch, T, A>			table_type;

		if ( dec_ref_storage(pStorage) )
		{
			if ( is_fixed_storage(pStorage) )
				table_type::i()->erase(pStorage);
			deallocate_storage(pStorage);
		}
	}

}


#include "rc_string_operator.h"


