#if !defined( __I3_T_STACK_H)
#define __I3_T_STACK_H

template <class T>
class i3TStack  : public i3ElementBase
{
protected:
	T *		m_pBuffer = nullptr;
	UINT32	m_Count = 0;
	UINT32	m_MaxCount = 0;
	UINT32	m_AllocUnit;			// [initialize at constructor]

protected:
	void	AdjustBufferSize( UINT32 newsz);

public:
	i3TStack(UINT32 allocUnit = 16) : m_AllocUnit(allocUnit) {}
	virtual ~i3TStack(void);

	UINT32		GetCount(void)		{ return m_Count; }

	T *			GetBuffer(void)		{ return m_pBuffer; }
	T *			GetTop(void)		
	{ 
		if( m_Count <= 0)
			return nullptr;

		return &m_pBuffer[ m_Count - 1]; 
	}

	void		SetTop( T val)
	{
		if( (m_Count + 1) > m_MaxCount)
		{
			AdjustBufferSize( m_MaxCount + 1);
		}

		m_pBuffer[ m_Count - 1] = val;
	}

	void	Push( T);
	bool	Pop( T *);
	bool	IsEmpty(void)			{ return (m_Count == 0); }

	void	Reset( bool bFreeMem = true);
};

template <class T>
i3TStack<T>::~i3TStack(void)
{
	Reset();
}

template <class T>
void i3TStack<T>::AdjustBufferSize( UINT32 newsz)
{
	m_MaxCount = (newsz + (m_AllocUnit - 1)) / m_AllocUnit;
	m_MaxCount *= m_AllocUnit;

	if( m_pBuffer == nullptr)
		m_pBuffer = (T *)i3MemAlloc( m_MaxCount * sizeof(T));
	else
		m_pBuffer = (T *) i3MemReAlloc( m_pBuffer, m_MaxCount * sizeof(T));

	//m_pBuffer = (T *) i3MemLock( hMem);
}

template <class T>
void i3TStack<T>::Push( T val)
{
	if( (m_Count + 1) > m_MaxCount)
	{
		AdjustBufferSize( m_MaxCount + 1);
	}

	*(m_pBuffer + m_Count) = val;

	m_Count++;
}

template <class T>
bool i3TStack<T>::Pop( T * pVal)
{
	if( m_Count <= 0)
		return false;

	m_Count --;

	if( pVal != nullptr)
		*pVal = *(m_pBuffer + m_Count);

	return true;
}

template <class T>
void i3TStack<T>::Reset( bool bFreeMem)
{
	if( bFreeMem)
	{
		I3MEM_SAFE_FREE_POINTER(m_pBuffer);
		m_MaxCount = 0;
	}

	m_Count = 0;	
}

#endif
