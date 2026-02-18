#if !defined( __I3_NULL_STREAM_H)
#define __I3_NULL_STREAM_H

#include "i3Stream.h"

class I3_EXPORT_BASE i3NullStream : public i3Stream
{
	I3_EXPORT_CLASS_DEFINE( i3NullStream, i3Stream);

protected:
	UINT32 m_Pos = 0;

public:
	i3NullStream(void) {}
	virtual ~i3NullStream(void) {}

	virtual bool Close(void) override { return true; }
	virtual bool IsOpened() const { return false; }

	virtual UINT32 Read( void *Buf, INT32 Length) override;
	virtual UINT32 Write( const void *Buf, INT32 Length) override;
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType) override;
	virtual UINT32 GetPosition(void) const { return m_Pos; }
};

#endif
