#pragma once

#include "priority_queue_fwd.h"
#include "vector.h"
#include "algorithm/make_heap.h"
#include "algorithm/push_heap.h"
#include "algorithm/pop_heap.h"


#pragma pack(push, _CRT_PACKING)

namespace i3
{
	template<class T, class Ctn, class Pred>
	class priority_queue : private enable_ebo< Pred >
	{
	public:
		typedef				enable_ebo<Pred>					base_type;
		typedef				Ctn									container_type;
		typedef typename	container_type::value_type			value_type;
		typedef typename	container_type::size_type			size_type;
		typedef typename	container_type::reference			reference;
		typedef typename	container_type::const_reference		const_reference;
		
		priority_queue() {}
		explicit priority_queue(const Pred& pr) : base_type(pr){}
		priority_queue(const Pred& pr, const container_type& ctn) : m_ctn(ctn), base_type(pr) { make_heap(m_ctn.begin(), m_ctn.end(), comp() ); }
		template<class Iter> priority_queue(Iter first, Iter last) : m_ctn(first, last)	{	make_heap(m_ctn.begin(), m_ctn.end(), comp() ); }
		template<class Iter> priority_queue(Iter first, Iter last, const Pred& pr) : m_ctn(first, last), base_type(pr) { make_heap(m_ctn.begin(), m_ctn.end(), comp() ); }
		template<class Iter> priority_queue(Iter first, Iter last, const Pred& pr, const container_type& ctn) : m_ctn(ctn), base_type(pr)
		{	
			m_ctn.insert(m_ctn.end(), first, last);		make_heap(m_ctn.begin(), m_ctn.end(), comp() );
		}

		bool			empty() const		{	return m_ctn.empty();	}
		size_type		size() const		{	return m_ctn.size();	}
		const_reference top() const			{	return m_ctn.front();	}
		reference		top()				{	return m_ctn.front();	}
		void			push(const value_type& pr) {	m_ctn.push_back(pr);	push_heap(m_ctn.begin(), m_ctn.end(), comp() );	}
		void			pop()				{	pop_heap(m_ctn.begin(), m_ctn.end(), comp() );	m_ctn.pop_back();	}
	private:
		Pred&			comp()		{ return  base_type::get(); }
		const Pred&		comp() const  { return base_type::get(); }
		container_type		m_ctn;					
	};
	
	
	
}


#pragma pack(pop)

