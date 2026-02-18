#if !defined( __I3_MEMORY_BUFFER_H)
#define __I3_MEMORY_BUFFER_H

#include "i3PersistantElement.h"
#include "i3NamedElement.h"

class I3_EXPORT_BASE i3MemoryBuffer : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3MemoryBuffer, i3NamedElement);
protected:
	void	*	m_pPointer = nullptr;
	UINT32		m_Size = 0;
	bool		m_bOwnBuffer = true;

public:
	virtual ~i3MemoryBuffer(void);

	bool			Create( UINT32 sz);
	bool			Bind( void * pPointer, UINT32 sz);

	UINT32			GetSize(void)				{ return m_Size; }

	void *			getBuffer(void)				{ return m_pPointer; }

	#if defined( I3_DEBUG)
	virtual void	_getInfoString( char * pszStr, const INT32 nStrMax );
	#endif

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
