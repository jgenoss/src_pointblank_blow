#ifndef __PCCAFE_WORKER_ZPT_H__
#define __PCCAFE_WORKER_ZPT_H__

#include "PCCafeDB.h"

#define PC_BUFFER_COUNT				2
#define PCCAFE_MAX_COUNT			20000

struct TYPE_IPLIST
{
	UINT32					m_ui32IP;
	UINT8					m_ui8Rank;
	UINT32					m_ui32CIDR;
};

class CPCCafeWorker_ZPT : public CPCCafeDB
{
	INT32					m_i32UseIdx; 
	TYPE_IPLIST				m_pIpList[ PC_BUFFER_COUNT ][ PCCAFE_MAX_COUNT ];
	UINT32					m_iIpListCount[ PC_BUFFER_COUNT ];

	INT32					m_i32PCCafeVer;

	UINT32					m_ui32UpdateTime;

protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT* pLoginIn, PCCAFE_OUTPUT* pLoginOut );
	
	BOOL					_LoadPCCafeList( TYPE_IPLIST* pList, UINT32* pui32Count );
public:
	CPCCafeWorker_ZPT();
	virtual ~CPCCafeWorker_ZPT();

	virtual BOOL			OnRunning();

	BOOL					IsPcCafe( UINT32 ui32IP, UINT8& ui8Rank );
};

#endif //__PCCAFE_WORKER_ZPT_H__