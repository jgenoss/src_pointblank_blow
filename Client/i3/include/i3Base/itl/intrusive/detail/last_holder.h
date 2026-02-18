#pragma once

namespace i3
{
	namespace intrusive
	{
		template<bool CacheLast, class Node>
		struct last_holder
		{
			static const bool cache_last = CacheLast;
			typedef Node		node;
			
			last_holder() : m_last(nullptr) {}

			__forceinline node*		get_last() const { return m_last;  }
			__forceinline void		set_last( node* n) { m_last = n; }
			node*		m_last;
		};
		
		template<class Node>
		struct last_holder<false, Node>
		{
			static const bool cache_last = false;
			typedef Node		node;
			__forceinline node*		get_last() const { return nullptr;  }
			__forceinline void		set_last( node* n) {}
		};

	}


}