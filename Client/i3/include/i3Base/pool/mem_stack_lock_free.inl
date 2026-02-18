
namespace i3
{
	inline
	void	mem_stack_lock_free::push(void* data)
	{
		free_object* obj = reinterpret_cast<free_object*>(data);
		for ( ; ; )	
		{
			obj->next = m_head;

			if (i3::cas((void* volatile*)&m_head, obj->next, obj) )		// 
				break;
		}	
	}
	
	inline
	bool	mem_stack_lock_free::empty() const
	{
		return !m_head;
	}

	inline
	void*	mem_stack_lock_free::pop()
	{
		free_object* head;
		size_t		 pop_count;
		for ( ; ; )
		{
			head = m_head;
			pop_count = m_pop_count;
			if ( head == nullptr)
				return nullptr;
			
			if (i3::cas2((void* volatile*)&m_head, head, pop_count, head->next, pop_count + 1) )
				break;
		}

		return head;
	}
	
	
}