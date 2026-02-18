#if !defined( __I3_PACK_FILE_H)
#define __I3_PACK_FILE_H

#include "i3PackNode.h"
#include "i3PackVolume.h"

class i3PackFile : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PackFile);
protected:
	I3_PACK_FILE_REF *		m_pFileInfo;
	INT32					m_CurPos;
	i3PackNode *			m_pPackNode;
	i3PackVolume *			m_pVol;

protected:

public:
	i3PackFile(void);
	virtual ~i3PackFile(void);

	bool		Open( i3PackVolume * pVol, i3PackNode * pPackNode, I3_PACK_FILE_REF * pInfo);

	UINT32		Read( void * pBuff, UINT32 sz);
	UINT32		GetPosition(void);
	UINT32		SetPosition( INT32 pos, INT32 origin = STREAM_BEGIN);
	UINT32		GetSize(void)											{ return m_pFileInfo->m_Size; }
};

#endif
