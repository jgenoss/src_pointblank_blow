#pragma once

#include "../ismb.h"

namespace i3
{
	//
	// boyer_moore와 기타 유사 알고리즘에 대한 스킵테이블이 분리됨.. 
	// ( char와 wchar_t의 메모리할당차이와 인덱스 접근문제로 인한것이고, 특별한것은 없음)
	//
	
	namespace detail
	{
		struct bm_skiptable_char_tag;
		struct bm_skiptable_wchar_tag;

		template<class Key, class Val, class Tag>
		class bm_skiptable;

		template<class Key, class Val>
		class bm_skiptable<Key, Val, bm_skiptable_wchar_tag>
		{
			typedef i3::unordered_map<Key, Val> map_type;
		public:
			bm_skiptable(std::size_t len, Val default_val) : m_map(len), m_default(default_val) {}
			void insert(Key k, Val v) {  m_map[k] = v;  }		
			Val  operator[](Key k) const 
			{
				typename map_type::const_iterator it = m_map.find(k);
				return it == m_map.end() ? m_default : it->second;
			}
		private:
			map_type		m_map;
			const Val		m_default;
		};
		
		//
		// 멀티바이트는 순수알고리즘이 작동이 잘 안된다고 하는데 테스트해보니..아주 잘되서..일단 놔둔다..
		// 

		template<class Key, class Val>
		class bm_skiptable<Key, Val, bm_skiptable_char_tag>
		{
			typedef typename i3::make_unsigned<Key>::type		ukey_type;
			static const size_t array_size = 1U << ( CHAR_BIT * sizeof(ukey_type) );

		public:
			bm_skiptable(std::size_t, Val default_val) 
			{
				i3::fill_n( m_array, array_size, default_val);			// memset으로 작동될것이다.
			}
			
			// DBCS/MBCS 호환될수 있도록 조정..
			void insert(Key k, Val v) 
			{ 
				m_array[ static_cast<ukey_type>(k) ] = v; 
			}

			Val  operator[](Key k) const { return m_array[static_cast<ukey_type>(k)]; }
		private:
			Val				m_array[ array_size ];
		};
				
		template<class It>
		struct get_bm_skiptable_type
		{
			typedef typename i3::iterator_traits<It>::difference_type	value_type;
			typedef typename i3::iterator_traits<It>::value_type		key_type;
			typedef i3::mpl::size_t< sizeof(key_type) >					sizeof_key;

			typedef typename mpl::if_<	mpl::and_< is_integral<key_type>, is_same< sizeof_key, mpl::size_t<1> > >, 
				bm_skiptable_char_tag, bm_skiptable_wchar_tag >::type	tag;

			typedef bm_skiptable<key_type, value_type, tag>				type;
		};
	}
}
