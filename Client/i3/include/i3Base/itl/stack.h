#pragma once

#include "stack_fwd.h"
#include "vector.h"

#pragma pack(push, _CRT_PACKING)

namespace i3
{
	template<class T, class Ctn>
	class stack
	{
	public:
		typedef Ctn											container_type;
		typedef typename container_type::value_type			value_type;
		typedef typename container_type::size_type			size_type;
		typedef typename container_type::reference			reference;
		typedef typename container_type::const_reference	const_reference;

		stack() : m_ctn() {}
		explicit stack(const container_type& ctn) : m_ctn(ctn) {}
		bool					empty() const { return m_ctn.empty(); }
		size_type				size() const  { return m_ctn.size(); }
		reference				top()			{ return m_ctn.back(); }
		const_reference			top() const		{ return m_ctn.back(); }
		void					push(const value_type& v) { m_ctn.push_back(v);  }
		void					pop() { m_ctn.pop_back(); }
		const container_type&	get_container() const { return m_ctn; }
	private:
		container_type		m_ctn;
	};

	template<class T, class C> inline bool operator==(const stack<T, C>& left, const stack<T, C>& right)
	{	
		return left.get_container() == right.get_container();
	}

	template<class T, class C> inline bool operator!=(const stack<T, C>& left, const stack<T, C>& right)
	{	
		return !(left == right);
	}

	template<class T, class C> inline bool operator<(const stack<T, C>& left, const stack<T, C>& right)
	{	
		return left.get_container() < right.get_container();
	}

	template<class T,  class C> inline bool operator>(const stack<T, C>& left, const stack<T, C>& right)
	{	
		return right < left;
	}

	template<class T, class C> inline bool operator<=(const stack<T, C>& left, const stack<T, C>& right)
	{	
		return !(right < left);
	}

	template<class T, class C> inline bool operator>=(const stack<T, C>& left, const stack<T, C>& right)
	{	
		return !(left < right);
	}


}

#pragma pack(pop)