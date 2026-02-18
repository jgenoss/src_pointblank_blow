#if !defined( __I3_STAGE_STACK_H)
#define __I3_STAGE_STACK_H

class I3_EXPORT_FRAMEWORK i3StageStack  : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3StageStack, i3ElementBase);
protected:
	INT32 *		m_pBuffer = nullptr;
	UINT32	m_Count = 0;
	UINT32	m_MaxCount = 0;
	UINT32	m_AllocUnit;			// [initialize at constructor]

protected:
	void	AdjustBufferSize( UINT32 newsz);

public:
	i3StageStack( UINT32 allocUnit = 16);
	virtual ~i3StageStack(void);

	UINT32		GetCount(void)		{ return m_Count; }

	INT32 *		GetBuffer(void)		{ return m_pBuffer; }
	INT32 *		GetTop(void)		
	{ 
		if( m_Count == 0)
			return nullptr;

		return &m_pBuffer[ m_Count - 1]; 
	}

	// revision 2116
	void		SetTop( INT32 val)
	{
		if( (0 < m_Count) && (m_Count <= m_MaxCount) )
			m_pBuffer[ m_Count - 1] = val;
	}

	void	Push( INT32);
	bool	Pop( INT32 *);
	bool	IsEmpty(void)			{ return (m_Count == 0); }

	void	Reset(void);
};


#endif
