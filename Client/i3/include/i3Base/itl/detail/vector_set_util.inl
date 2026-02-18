#include "../algorithm/find.h"

namespace i3
{
	namespace vector_set_util
	{
		template<class K, class P, class A, class U> inline
		typename i3::vector_set<K, P, A>::size_type	linear_index_of( const i3::vector_set<K, P, A>& ctn, const U& obj)
		{
			i3::vector_set<K, P, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			return static_cast<typename i3::vector_set<K, P, A>::size_type>(i3::distance(ctn.begin(), it));
		}

		template<class K, class P, class A, class U> inline
		typename i3::vector_multiset<K, P, A>::size_type	linear_1st_index_of( const i3::vector_multiset<K, P, A>& ctn, const U& obj)
		{
			i3::vector_multiset<K, P, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			return static_cast<typename i3::vector_multiset<K, P, A>::size_type>(i3::distance(ctn.begin(), it));
		}

		template<class K, class P, class A, class U> inline
		bool									linear_remove( i3::vector_set<K, P, A>& ctn, const U& obj)
		{
			i3::vector_set<K, P, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			if (it == ctn.end() ) return false;
			ctn.erase( it );
			return true;
		}

		template<class K, class P, class A, class U> inline
		bool									linear_remove_1st( i3::vector_multiset<K, P, A>& ctn, const U& obj)
		{
			i3::vector_multiset<K, P, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			if (it == ctn.end() ) return false;
			ctn.erase( it );
			return true;
		}

		template<class K, class P, class A, class U> inline
		typename i3::vector_set<K, P, A>::size_type	binary_index_of( const i3::vector_set<K, P, A>& ctn, const U& obj)
		{
			i3::vector_set<K, P, A>::const_iterator it = ctn.find(obj);
			return static_cast<typename i3::vector_set<K, P, A>::size_type>(i3::distance(ctn.begin(), it));
		}

		template<class K, class P, class A, class U> inline
		typename i3::vector_multiset<K, P, A>::size_type binary_1st_index_of( const i3::vector_multiset<K, P, A>& ctn, const U& obj)
		{
			i3::vector_multiset<K, P, A>::const_iterator it = ctn.find(obj);
			return static_cast<typename i3::vector_multiset<K, P, A>::size_type>(i3::distance(ctn.begin(), it));
		}

		template<class K, class P, class A, class U> inline
		bool									binary_remove( i3::vector_set<K, P, A>& ctn, const U& obj)
		{
			i3::vector_set<K, P, A>::const_iterator it = ctn.find(obj);
			if (it == ctn.end() ) return false;
			ctn.erase( it );
			return true;
		}

		template<class K, class P, class A, class U> inline
		bool									binary_remove_1st( i3::vector_multiset<K, P, A>& ctn, const U& obj)
		{
			i3::vector_multiset<K, P, A>::const_iterator it = ctn.find(obj);
			if (it == ctn.end() ) return false;
			ctn.erase( it );
			return true;
		}

		template<class K, class P, class A, class T, class Pred> inline
		i3::pair<typename i3::vector_set<K,P,A>::const_iterator, bool> binary_find(const i3::vector_set<K, P, A>& ctn, const T& Val, Pred Pr )
		{
			return i3::au::binary_find(ctn.begin(), ctn.end(), Val, Pr);
		}

		template<class K, class P, class A, class T> inline
		i3::pair<typename i3::vector_set<K,P,A>::const_iterator, bool> binary_find(const i3::vector_set<K, P, A>& ctn, const T& Val )
		{
			return i3::au::binary_find(ctn.begin(), ctn.end(), Val);
		}

		template<class K, class P, class A, class T, class Pred> inline
		i3::pair<typename i3::vector_multiset<K,P,A>::const_iterator, bool> binary_find(const i3::vector_multiset<K, P, A>& ctn, const T& Val, Pred Pr )
		{
			return i3::au::binary_find(ctn.begin(), ctn.end(), Val, Pr);
		}

		template<class K, class P, class A, class T> inline
		i3::pair<typename i3::vector_multiset<K,P,A>::const_iterator, bool> binary_find(const i3::vector_multiset<K, P, A>& ctn, const T& Val )
		{
			return i3::au::binary_find(ctn.begin(), ctn.end(), Val);
		}

	}

}