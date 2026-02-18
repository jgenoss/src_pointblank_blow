#pragma once 

#include "../inst/ginst.h"
#include "../itl/functional.h"
//#include "stlfwd.h"
#include "../itl/unordered_set.h"
#include "../pool/cs.h"

namespace i3
{

	template <typename Ch, typename Traits, typename Alloc>
	class global_string_table : public shared_ginst< global_string_table<Ch, Traits, Alloc> >
	{
	public:

		global_string_table();
		~global_string_table();
		typedef global_string_table<Ch, Traits, Alloc>										my_type;
		typedef typename rc_basic_string<Ch, Traits, Alloc>									rc_string_type;
		typedef typename rc_string_type::storage_type										storage_type;
		
		typedef typename basic_string< Ch, Traits, Alloc >									ue_string_type;
		typedef typename std::basic_string< Ch, Traits, Alloc >								std_string_type;
		typedef typename pool_allocator<Ch>::size_type										size_type;
		
		
	
		rc_string_type					insert(const Ch* psz);
		rc_string_type					insert(const Ch* psz, size_type size);
		rc_string_type					insert(const rc_string_type& str);
/*
		template< template<class, class, class> class string_ctn, class T, class A>
		rc_string_type					insert(const string_ctn<Ch,T,A>& str);
*/
		void							erase(const storage_type* storage);

	private:
		struct hash //: public i3::unary_function< storage_type* , std::size_t >
		{
			std::size_t	operator()(const storage_type* v ) const 
			{
				return i3::detail::string_hash_value(v->ptr, v->size );
			}
			std::size_t operator()(const i3::pair<const Ch*, std::size_t>& pr) const 
			{
				return i3::detail::string_hash_value(pr.first, pr.second);
			}

		};

		struct equal_to //: public i3::binary_function<storage_type*, storage_type*, bool>
		{	
			bool operator()(const storage_type* _Left, const storage_type* _Right) const	
			{	
				if ( _Left->ptr == _Right->ptr) return true;
				if ( _Left->size != _Right->size) return false;
				return Traits::compare( _Left->ptr , _Right->ptr, _Left->size ) == 0;
			}
			bool operator()(const i3::pair<const Ch*, std::size_t>& _Left, const storage_type* _Right) const	
			{	
				if ( _Left.first == _Right->ptr) return true;
				if ( _Left.second != _Right->size) return false;
				return Traits::compare( _Left.first , _Right->ptr, _Left.second ) == 0;
			}
			bool operator()(const storage_type* _Left, const i3::pair<const Ch*, std::size_t>& _Right) const	
			{	
				if ( _Left->ptr == _Right.first) return true;
				if ( _Left->size != _Right.second) return false;
				return Traits::compare( _Left->ptr , _Right.first, _Left->size ) == 0;
			}
		};

		typedef unordered_set<const storage_type*, typename my_type::hash, typename my_type::equal_to  >   unordered_set_type;

		unordered_set_type		m_hashtable;			// 
		i3::cs					m_cs;
	};

	template<class C,class T, class A> 
	struct use_delayed_atexit_ginst< global_string_table<C,T,A> > : true_type {};

	template <typename Ch, typename T, typename A> 
	global_string_table<Ch, T, A>::global_string_table() : m_hashtable(521ul)
	{
				
	}
	template <typename Ch, typename T, typename A>
	global_string_table<Ch, T, A>::~global_string_table()
	{

	}

	template <typename Ch, typename T, typename A> inline
	typename global_string_table<Ch, T, A>::rc_string_type		global_string_table<Ch, T, A>::insert(const Ch* psz)
	{
		if ( psz[0] == 0 )
			return rc_string_type::get_null_storage();

		typedef unordered_set_type	table_type;

		i3::pair<const Ch*, std::size_t> key_pr(psz, T::length(psz) );

		i3::scoped_cs lock(m_cs);

		typename table_type::iterator it = m_hashtable.find_other( key_pr );
		if (it != m_hashtable.end())
			return *it;

		rc_string_type	v(psz, key_pr.second);
		v.get_storage()->set_fixed_storage();
		m_hashtable.insert( v.get_storage() );
		return v;
	}

	template <typename Ch, typename T, typename A> inline
	typename global_string_table<Ch, T, A>::rc_string_type		global_string_table<Ch, T, A>::insert(const Ch* psz, 
		typename global_string_table<Ch, T, A>::size_type size)
	{
		if ( size == 0 )
			return rc_string_type::get_null_storage();

		typedef unordered_set_type	table_type;
		i3::pair<const Ch*, std::size_t> key_pr(psz, size );

		i3::scoped_cs lock(m_cs);

		typename table_type::iterator it = m_hashtable.find_other( key_pr );
		if (it != m_hashtable.end())
			return *it;

		rc_string_type	v(psz, size);
		v.get_storage()->set_fixed_storage();
		m_hashtable.insert( v.get_storage() );
		return v;
	}

	template <typename Ch, typename T, typename A> inline
	typename global_string_table<Ch, T, A>::rc_string_type		global_string_table<Ch, T, A>::insert(const rc_string_type& str)
	{
		if ( str.size() == 0 )
			return rc_string_type::get_null_storage();

		if (str.get_storage()->is_fixed_storage())
			return str;

		typedef unordered_set<const storage_type*, typename my_type::hash, typename my_type::equal_to  >	table_type;
		typedef i3::pair<typename table_type::iterator, bool>		pair_type;

		i3::scoped_cs lock(m_cs);

		pair_type pair = m_hashtable.insert( str.get_storage() );	// 이건 find보다 그냥 넣어보는게 더 나을것 같다...
		if (pair.second == true )
			(*pair.first)->set_fixed_storage();
		return (*pair.first);
	}
	
/*
	template <typename Ch, typename T, typename A> 
	template< template<class, class, class> class string_ctn, class T2, class A2> inline
	typename global_string_table<Ch, T, A>::rc_string_type		global_string_table<Ch, T, A>::insert(const string_ctn<Ch,T2,A2>& str)
	{
		if ( str.size() == 0 )
			return rc_string_type::get_null_storage();

		rc_string_type	v(str.c_str(), str.size() );
		typedef unordered_set<const storage_type*, typename my_type::hash, typename my_type::equal_to  >	table_type;
		typedef i3::pair<typename table_type::iterator, bool>		pair_type;
		pair_type pair = m_hashtable.insert( v.get_storage() );
		if (pair.second == true )
			(*pair.first)->set_fixed_storage();

		return (*pair.first);
				
	}
*/
	
	template <typename Ch, typename T, typename A> inline
	void			global_string_table<Ch, T, A>::erase(const storage_type* storage)
	{
		i3::scoped_cs lock(m_cs);
		m_hashtable.erase(storage);
	}
	
	namespace
	{
		struct global_string_table_init
		{
			global_string_table_init()
			{
				global_string_table<char, i3::char_traits<char>, i3::pool_allocator<char> >::create_explicit();
				global_string_table<wchar_t, i3::char_traits<wchar_t>, i3::pool_allocator<wchar_t> >::create_explicit();
			}

		} _global_string_table_init_;
	}


}