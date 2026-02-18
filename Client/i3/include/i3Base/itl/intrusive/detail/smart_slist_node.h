#pragma once

namespace i3
{
	namespace intrusive
	{
		struct slist_node
		{
			slist_node*	next;
		};
		
		template<class SmartPtr>
		struct slist_node_with_smart_ptr : slist_node
		{
			SmartPtr m_smart_ptr;

			void		init_smart_ptr(const SmartPtr& p) { m_smart_ptr = p; }
			void		destroy_smart_ptr() { m_smart_ptr.reset(); }
			SmartPtr&	get_smart_ptr() { return m_smart_ptr; }
			const SmartPtr& get_smart_ptr() const { return m_smart_ptr; }
		};

		template<class RawElement>
		struct slist_node_with_smart_ptr<RawElement*> : slist_node
		{
			RawElement*	m_ptr;

			void		init_smart_ptr(const RawElement* p) { m_ptr = p; }
			void		destroy_smart_ptr() { m_ptr = nullptr; }
			RawElement*&	get_smart_ptr() { return m_ptr; }
			RawElement* const & get_smart_ptr() const { return m_ptr; }
		};

	}
}