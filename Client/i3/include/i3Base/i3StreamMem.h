#if !defined( __I3_MEM_STREAM_H)
#define __I3_MEM_STREAM_H

#include "i3Stream.h"

class I3_EXPORT_BASE i3MemStream : public i3Stream
{
	I3_EXPORT_CLASS_DEFINE( i3MemStream, i3Stream);

protected:
	void	*	m_pHandle = nullptr;
	UINT32 m_Pos = 0;
	bool	m_bExternMem = false;

public:
	virtual ~i3MemStream(void);

	bool  Create( const void* pNewPointer, INT32 Acc, UINT32 Size);
	bool  Open( const void* pNewPointer, INT32 Acc, UINT32 Size);
	virtual bool  Close(void) override;
	virtual bool  IsOpened() const { return m_pHandle != nullptr; }

	virtual UINT32 Read( void *Buf, INT32 Length) override;
	virtual UINT32 Write( const void *Buf, INT32 Length) override;
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType) override;
	virtual UINT32 GetPosition(void) const { return m_Pos; }

	void *	GetHandle(void) const { return m_pHandle; }
	
	static i3MemStream * FromHandle( void *pNew);
	static i3MemStream * CreateFromFile( const char * pszPath);
};

#endif
