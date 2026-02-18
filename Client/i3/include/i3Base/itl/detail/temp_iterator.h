#pragma once

#include "../iterator.h"

namespace i3
{
	template<class T>
	class temp_iterator : public i3::iterator<output_iterator_tag, void, void, void, void>			// 템포러리 버퍼함수 사용..
	{
	public:
		temp_iterator(ptrdiff_t count = 0) {	m_buff.begin = 0;	m_buff.current = 0;	m_buff.hiwater = 0;	m_buff.size = count;	m_pbuff = &m_buff;	}
		temp_iterator(const temp_iterator<T>& rhs) {	m_buff.begin = 0; m_buff.current = 0; m_buff.hiwater = 0; m_buff.size = 0; *this = rhs;	}
		~temp_iterator() 
		{
			if (m_buff.begin != 0)
			{
				i3::destroy( m_buff.begin, m_buff.hiwater);
				i3::return_temporary_buffer(m_buff.begin);
			}
		}
		temp_iterator<T>& operator=(const temp_iterator<T>& rhs) { m_pbuff = rhs.m_pbuff;	return *this;  }
		temp_iterator<T>& operator=( const T& val) 
		{      
			if (m_pbuff->current < m_pbuff->hiwater)
				*m_pbuff->current++ = val;
			else
			{
				T* ptr = &*m_pbuff->current;
				i3::construct(ptr, val);
				m_pbuff->hiwater = ++m_pbuff->current;
			}
			return *this;
		}

		temp_iterator<T>&	operator*() { return *this; }
		temp_iterator<T>&	operator++(){ return *this; }
		temp_iterator<T>&	operator++(int) { return *this; }
		temp_iterator<T>&	init() { m_pbuff->current = m_pbuff->begin; return *this; }
		T*					first() const { return m_pbuff->begin; }
		T*					last() const  { return m_pbuff->current; }

		ptrdiff_t			max_length()		// 할당과 사이즈조회를 동시담당..
		{
			if ( m_pbuff->begin == 0 && m_pbuff->size > 0 )
			{
				i3::pair<T*, ptrdiff_t> p = i3::get_temporary_buffer<T>( m_pbuff->size );
				m_pbuff->begin   = p.first;
				m_pbuff->current = p.first;
				m_pbuff->hiwater = p.first;
				m_pbuff->size    = p.second;
			}
			return m_pbuff->size;
		}

	private:
		struct buffer {	T*	begin;	T*  current;	T*  hiwater;	ptrdiff_t size;	};
		buffer		m_buff;
		buffer*		m_pbuff;
	};

	template<class T>
	class temp_pool_iterator : public i3::iterator<output_iterator_tag, void, void, void, void>			// 템포러리 버퍼함수 대신 static_pool사용..
	{
	public:
		temp_pool_iterator(ptrdiff_t count = 0) {	m_buff.begin = 0;	m_buff.current = 0;	m_buff.hiwater = 0;	m_buff.size = count;	m_pbuff = &m_buff;	}
		temp_pool_iterator(const temp_pool_iterator<T>& rhs) {	m_buff.begin = 0; m_buff.current = 0; m_buff.hiwater = 0; m_buff.size = 0; *this = rhs;	}
		~temp_pool_iterator() 
		{
			if (m_buff.begin != 0)
			{
				i3::destroy( m_buff.begin, m_buff.hiwater);
				i3::static_pool_free_no_dbg(m_buff.begin);
			}
		}

		temp_pool_iterator<T>& operator=(const temp_pool_iterator<T>& rhs) { m_pbuff = rhs.m_pbuff;	return *this;  }
		temp_pool_iterator<T>& operator=( const T& val) 
		{      
			if (m_pbuff->current < m_pbuff->hiwater)
				*m_pbuff->current++ = val;
			else
			{
				T* ptr = &*m_pbuff->current;
				i3::construct(ptr, val);
				m_pbuff->hiwater = ++m_pbuff->current;
			}
			return *this;
		}

		temp_pool_iterator<T>&	operator*() { return *this; }
		temp_pool_iterator<T>&	operator++(){ return *this; }
		temp_pool_iterator<T>&	operator++(int) { return *this; }
		temp_pool_iterator<T>&	init() { m_pbuff->current = m_pbuff->begin; return *this; }
		T*					first() const { return m_pbuff->begin; }
		T*					last() const  { return m_pbuff->current; }

		ptrdiff_t			max_length()		// 할당과 사이즈조회를 동시담당..
		{
			if ( m_pbuff->begin == 0 && m_pbuff->size > 0 )
			{
				m_pbuff->begin = static_cast<T*>(i3::static_pool_malloc_no_dbg( sizeof(T) * m_pbuff->size));
				m_pbuff->current = m_pbuff->begin;
				m_pbuff->hiwater = m_pbuff->begin;
			}
			return m_pbuff->size;
		}

	private:
		struct buffer {	T*	begin;	T*  current;	T*  hiwater;	ptrdiff_t size;	};
		buffer		m_buff;
		buffer*		m_pbuff;
	};





}