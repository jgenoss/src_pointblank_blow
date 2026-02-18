#if !defined( __I3_POINTER_STACK_H)
#define __I3_POINTER_STACK_H

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

class I3_EXPORT_BASE i3PointerStack  : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PointerStack);
protected:
	void **	m_pBuffer;
	UINT32	m_Count;
	UINT32	m_MaxCount;
	UINT32	m_AllocUnit;

protected:
	void	AdjustBufferSize( UINT32 newsz);

public:
	i3PointerStack( UINT32 allocUnit = 16);
	virtual ~i3PointerStack(void);

	UINT32		getCount(void)		{ return m_Count; }
	void		setCount( UINT32 c);

	void *		getBuffer(void)		{ return m_pBuffer; }
	void *		getTop(void)		
	{ 
		if( m_Count <= 0)
			return NULL;

		return m_pBuffer[ m_Count - 1]; 
	}

	void		setTop( void * val)
	{
		if( (m_Count + 1) > m_MaxCount)
		{
			AdjustBufferSize( m_MaxCount + 1);
		}

		m_pBuffer[ m_Count - 1] = val;
	}

	void	Push( void *);
	void	PushToTop( void * p);
	bool	Pop( void * * pp);
	bool	IsEmpty(void)			{ return (m_Count == 0); }

	void	Reset( bool bFreeMem = true);
};

#endif
