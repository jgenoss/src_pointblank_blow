#if !defined( __I3_MEM_STREAM_H)
#define __I3_MEM_STREAM_H

#include "i3Stream.h"

class I3_EXPORT_BASE i3MemStream : public i3Stream
{
	I3_CLASS_DEFINE( i3MemStream);

protected:
	UINT32 m_Pos;
	bool	m_bExternMem;
public:
	i3MemStream(void);
	virtual ~i3MemStream(void);

	virtual BOOL  Create( const char * pNewPointer, INT32 Acc, UINT32 Size);
	virtual BOOL  Open( const char * pNewPointer, INT32 Acc, UINT32 Size);
	virtual BOOL  Close(void);

	virtual UINT32 Read( void *Buf, INT32 Length);
	virtual UINT32 Write( const void *Buf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType);
	virtual UINT32 GetPosition(void);

	static i3MemStream * FromHandle( void *pNew);
	static i3MemStream * CreateFromFile( const char * pszPath);
};

#endif
