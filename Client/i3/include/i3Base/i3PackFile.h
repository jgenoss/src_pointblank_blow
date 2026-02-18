#if !defined( __I3_PACK_FILE_H)
#define __I3_PACK_FILE_H

#include "i3PackNode.h"
#include "i3PackVolume.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3MemStream;

//

class I3_EXPORT_BASE i3PackFile : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PackFile, i3ElementBase);
protected:
	I3_PACK_FILE_REF *		m_pFileInfo = nullptr;
	INT32					m_CurPos = 0;
	i3PackNode *			m_pPackNode = nullptr;

	i3FileStream*			m_pCurrThreadStream = nullptr;
	i3MemStream*			m_StreamForENC = nullptr;

public:
	virtual ~i3PackFile(void);

	bool		Open( i3PackVolume* pVolume, i3PackNode * pPackNode, I3_PACK_FILE_REF * pInfo);

	UINT32		Read( void * pBuff, UINT32 sz);
	UINT32		GetPosition(void);
	UINT32		SetPosition( INT32 pos, INT32 origin = STREAM_BEGIN);
	UINT32		GetSize(void) { return m_pFileInfo->m_Size; }
};

#endif
