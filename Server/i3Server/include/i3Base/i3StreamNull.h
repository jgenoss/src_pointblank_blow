#if !defined( __I3_NULL_STREAM_H)
#define __I3_NULL_STREAM_H

#include "i3Stream.h"

class I3_EXPORT_BASE i3NullStream : public i3Stream
{
	I3_CLASS_DEFINE( i3NullStream);

protected:
	UINT32 m_Pos;

public:
	i3NullStream(void);
	virtual ~i3NullStream(void);

	virtual BOOL Create( const char *Name, INT32 Acc, UINT32 Size = 0);
	virtual BOOL Open( const char *Name, INT32 Acc, UINT32 Size = 0);
	virtual BOOL Close(void);
	
	virtual UINT32 Read( void *Buf, INT32 Length);
	virtual UINT32 Write( const void *Buf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType);
	virtual UINT32 GetPosition(void);
};

#endif
