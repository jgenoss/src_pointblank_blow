#if !defined( __I3_STACK_MEMORY_POOL_H)
#define __I3_STACK_MEMORY_POOL_H

#include "i3ElementBase.h"

class I3_EXPORT_BASE i3StackMemoryPool : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3StackMemoryPool, i3ElementBase);
protected:
	char *		m_pBuff = nullptr;
	char *		m_pTail = nullptr;

	UINT32		m_MaxSize = 0;
	UINT32		m_AllocedSize = 0;

public:
	virtual ~i3StackMemoryPool(void);

	bool	Create( UINT32 maxSize);

	void *	Alloc( UINT32 sz);
	void	Free( void * pMem);
	void	Reset(void);

	bool	isPointer( void * p)		{ return (m_pBuff <= p) && ((m_pBuff + m_MaxSize) >= p); }

	void	Dump(void);
};

#endif
