#if !defined( __I3_T_OBJECT_ARRAY_H)
#define __I3_T_OBJECT_ARRAY_H

#include "i3ElementBase.h"

template <class T>
class i3TObjectArray : public i3ElementBase
{

protected:
	T *		m_pArray = nullptr;
	INT32	m_Max = 0;
	INT32	m_Count = 0;

public:
	virtual ~i3TObjectArray(void);

	bool	Create( INT32 MaxCount);

	INT32	GetCount(void);
	T *		GetObject( INT32 idx);
	T *		GetUnusedObject(void);
	void	Reset(void);

	#if defined( I3_DEBUG)
	static  i3TObjectArray<T> * _NewObject( const char * /*pszFile*/, int /*line*/)
	#else
	static  i3TObjectArray<T> * _NewObject(void)
	#endif
	{
		return new i3TObjectArray<T>;
	}
};


template <class T>
i3TObjectArray<T>::~i3TObjectArray(void)
{
	I3_SAFE_DELETE_ARRAY( m_pArray);
}

template <class T>
bool i3TObjectArray<T>::Create( INT32 MaxCount)
{
	m_Max = MaxCount;
	m_Count = 0;

	m_pArray = new T[MaxCount];
	I3ASSERT( m_pArray != nullptr);
	NetworkDump_Malloc( m_pArray, sizeof(T) * MaxCount, __FILE__, __LINE__);

	return true;
}

template <class T>
INT32 i3TObjectArray<T>::GetCount(void)
{
	return m_Count;
}

template <class T>
T * i3TObjectArray<T>::GetObject( INT32 idx)
{
	return m_pArray[idx];
}

template <class T>
T * i3TObjectArray<T>::GetUnusedObject(void)
{
	m_Count++;

	// Exceed capacity of object array (max:%d)
	I3ASSERT( m_Count < m_Max);

	return & m_pArray[ m_Count - 1];
}

template <class T>
void i3TObjectArray<T>::Reset(void)
{
	m_Count = 0;
}


#endif
