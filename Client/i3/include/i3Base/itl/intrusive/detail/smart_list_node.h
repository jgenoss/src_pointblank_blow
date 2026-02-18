#pragma once

namespace i3
{
	namespace intrusive
	{
		struct list_node
		{
			list_node*		next;
			list_node*		prev;
		};
		
		template<class SmartPtr>
		struct list_node_with_smart_ptr : list_node
		{
			SmartPtr	m_smart_ptr;							// 레퍼런스타입 획득을 위해서 통으로 들고 있는게 좋겠다..

			void		init_smart_ptr(const SmartPtr& p) { m_smart_ptr = p; }
			void		destroy_smart_ptr() { m_smart_ptr.reset(); }
			SmartPtr&	get_smart_ptr() { return m_smart_ptr; }
			const SmartPtr& get_smart_ptr() const { return m_smart_ptr; }
		};
		
		// 부분특화.. Raw 포인터도 들어갈수 있도록 하고 딱히 delete처리를 하지 않도록 부분 구현...
		// 실데이터 타입은 T*에 해당한다..

		template<class RawElement>
		struct list_node_with_smart_ptr<RawElement*> : list_node
		{
			RawElement*	m_ptr;

			void		init_smart_ptr( RawElement* p) { m_ptr = p; }
			void		destroy_smart_ptr() { m_ptr = nullptr; }
			RawElement*&	get_smart_ptr() { return m_ptr; }
			RawElement* const & get_smart_ptr() const { return m_ptr; }
		};

	}
}
