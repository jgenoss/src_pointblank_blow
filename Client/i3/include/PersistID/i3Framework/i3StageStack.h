#if !defined( __I3_STAGE_STACK_H)
#define __I3_STAGE_STACK_H

class I3_EXPORT_FRAMEWORK i3StageStack  : public i3ElementBase
{
	I3_CLASS_DEFINE( i3StageStack);
protected:
	INT32 *		m_pBuffer;
	UINT32	m_Count;
	UINT32	m_MaxCount;
	UINT32	m_AllocUnit;

protected:
	void	AdjustBufferSize( UINT32 newsz);

public:
	i3StageStack( UINT32 allocUnit = 16);
	virtual ~i3StageStack(void);

	UINT32		GetCount(void)		{ return m_Count; }
	void		SetCount( UINT32 c);

	INT32 *		GetBuffer(void)		{ return m_pBuffer; }
	INT32 *		GetTop(void)		
	{ 
		if( m_Count <= 0)
			return NULL;

		return &m_pBuffer[ m_Count - 1]; 
	}

	void		SetTop( INT32 val)
	{
		m_pBuffer[ m_Count - 1] = val;
	}

	void	Push( INT32);
	bool	Pop( INT32 *);
	bool	IsEmpty(void)			{ return (m_Count == 0); }

	void	Reset(void);
};


#endif
