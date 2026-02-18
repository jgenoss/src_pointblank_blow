#pragma once

//
//   void*를 대신할만한 자료구조가 일단 필요해서, 일단은 any를 씀...  
//
#include "i3ElementBase.h"
#include "itl/any.h"

class I3_EXPORT_BASE i3AnyData : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3AnyData, i3ElementBase);
public:
	i3AnyData() {}
	i3AnyData(const i3AnyData& rhs) : m_data(rhs.m_data) {}
	template<class T> 
	i3AnyData(const T& val) : m_data(val) {}
	
	i3AnyData& operator=(const i3AnyData& rhs) { m_data = rhs.m_data; return *this; }
	template<class T>
	i3AnyData& operator=(const T& val) { m_data = val; return *this; }
	const std::type_info& type() const { return m_data.type(); }

	i3::any&			get()	{ return m_data; }
	const i3::any&		get() const { return m_data; }
	void				clear() { m_data.clear();  }
private:
	i3::any			m_data;
};


template<class value_type> inline
value_type* AnyDataCast(i3AnyData* a) { return i3::any_cast<value_type>(&a->get()); }

template<class value_type> inline 
const value_type* AnyDataCast(const i3AnyData* a) {	return i3::any_cast<value_type>(&a->get()); }

template<class value_type> inline 
value_type AnyDataCast(i3AnyData& a) {  return i3::any_cast<value_type>(a.get());  }

template<class value_type> inline 
value_type AnyDataCast(const i3AnyData& a) {  return i3::any_cast<value_type>(a.get());  }


template<class value_type> inline 
value_type* UnsafeAnyDataCast(i3AnyData* a) {	return i3::unsafe_any_cast<value_type>( &a->get() );	}

template<class value_type> inline 
const value_type* UnsafeAnyDataCast(const i3AnyData* a) { return i3::unsafe_any_cast<value_type>( &a->get() );  }

