#if !defined( __I3_T_BAND_BUFFER_H)
#define __I3_T_BAND_BUFFER_H

#include "i3Platform.h"

template <class T>
class I3_EXPORT_TEMPLATE i3TBandBuffer  : public i3ElementBase
{
protected:
	T *		m_pBuffer;
	UINT32	m_Count;
	UINT32	m_MaxCount;
	UINT32	m_AllocUnit;
	bool	m_bNoFreeMem;

protected:
	void	AdjustBufferSize( UINT32 newsz);

public:
	i3TBandBuffer( UINT32 allocUnit = 32);
	virtual ~i3TBandBuffer(void);

	UINT32		GetCount(void)		{ return m_Count; }
	void		SetCount( UINT32 c);

	T *			GetBuffer(void)		{ return m_pBuffer; }

	bool		getFreeMemState(void)			{ return m_bNoFreeMem; }
	void		setFreeMemState( bool bFree)	{ m_bNoFreeMem = !bFree; }

	void	Set( UINT32 idx, T);
	T		Get( UINT32 idx);
	void	Add( T);
	void	Remove( UINT32 idx);
	void	Reset(void);
};

template <class T>
i3TBandBuffer<T>::i3TBandBuffer( UINT32 allocUnit)
{
	m_pBuffer = NULL;
	m_Count = 0;
	m_MaxCount = 0;
	m_AllocUnit = allocUnit;
	m_bNoFreeMem = false;
}

template <class T>
i3TBandBuffer<T>::~i3TBandBuffer(void)
{
	Reset();
}

template <class T>
void i3TBandBuffer<T>::AdjustBufferSize( UINT32 newsz)
{
	

	m_MaxCount = (newsz + (m_AllocUnit - 1)) / m_AllocUnit;
	m_MaxCount *= m_AllocUnit;

	/*
	INT32 hMem;
	if( m_pBuffer == NULL)	hMem = i3MemAlloc( m_MaxCount * sizeof(T));
	else					hMem = i3MemReAlloc( m_pBuffer, m_MaxCount * sizeof(T));
	m_pBuffer = (T *) i3MemLock( hMem);
	*/
	if( m_pBuffer == NULL)	m_pBuffer = (T *)i3MemAlloc( m_MaxCount * sizeof(T));
	else					m_pBuffer = (T *)i3MemReAlloc( m_pBuffer, m_MaxCount * sizeof(T));


}

template <class T>
void i3TBandBuffer<T>::SetCount( UINT32 c)
{
	m_Count = c;

	AdjustBufferSize( c);
}

template <class T>
void i3TBandBuffer<T>::Set( UINT32 idx, T val)
{
	*(m_pBuffer + idx) = val;
}

template <class T>
T i3TBandBuffer<T>::Get( UINT32 idx)
{
	return *(m_pBuffer + idx);
}

template <class T>
void i3TBandBuffer<T>::Add( T val)
{
	if( (m_Count + 1) > m_MaxCount)
	{
		AdjustBufferSize( m_MaxCount + 1);
	}

	*(m_pBuffer + m_Count) = val;

	m_Count++;
}

template <class T>
void i3TBandBuffer<T>::Remove( UINT32 idx)
{
	INT32 MoveLength;

	if( m_Count <= 0)
		return;

	MoveLength = m_Count - idx - 1;
	if( MoveLength > 0)
	{
		memcpy( m_pBuffer + idx, m_pBuffer + idx + 1, MoveLength * sizeof(T));
	}
	m_Count--;
}

template <class T>
void i3TBandBuffer<T>::Reset(void)
{
	if( m_bNoFreeMem == false)
	{
		if( m_pBuffer != NULL)
		{
			i3MemFree( m_pBuffer );
			m_pBuffer = NULL;
		}

		m_MaxCount = 0;
	}

	m_Count = 0;
}

#endif
