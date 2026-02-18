#ifndef __DS_OBJECT_MANAGER_H__
#define __DS_OBJECT_MANAGER_H__

#include "DSObject.h"

struct CDSObjectList
{
	CDSObject*				m_pObject;
	CDSObjectList*			m_pPrev;
	CDSObjectList*			m_pNext;

	UINT32					m_ui32UpdateIdx;
};

class CDSObjectManager
{
	INT32					m_i32ObjectCount;
	CDSObjectList*			m_pList;

	CDSObjectList*			m_pFront;

	UINT32					m_ui32UpdateIdx;

public:
	CDSObjectManager();
	~CDSObjectManager();

	BOOL					Create( INT32 i32Count );
	void					Destroy();

	void					Reset();
	BOOL					AddObject( INT32 i32Idx, CDSObject* pObject );
	void					RenewalObject( INT32 i32Idx ) ;
	
	UINT32					MakePacket( char* pBuffer, UINT32 ui32MaxSize, SEND_PACKET_TYPE ePacketType, UINT32& ui32UpdateTime );
};

#endif