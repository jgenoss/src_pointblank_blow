#if !defined( __I3_MEMORY_BUFFER_H)
#define __I3_MEMORY_BUFFER_H

#include "i3PersistantElement.h"

class I3_EXPORT_BASE i3MemoryBuffer : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3MemoryBuffer);
protected:
	void	*	m_pPointer;
	UINT32		m_Size;
	bool		m_bOwnBuffer;

public:
	i3MemoryBuffer(void);
	virtual ~i3MemoryBuffer(void);

	BOOL			Create( UINT32 sz);
	BOOL			Bind( void * pPointer, UINT32 sz);

	UINT32			GetSize(void)				{ return m_Size; }

	void *			getBuffer(void)				{ return m_pPointer; }

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
