#ifndef __MODULE_PRECBT_THAI_H__
#define __MODULE_PRECBT_THAI_H__

#define PC_BUFFER_COUNT	2

struct DB_QUERY_INFO
{
	CADODatabase	*	_pDB;
	wchar_t				_pQuery[SQL_STRING_COUNT];
};

class CModulePcList : public i3Thread
{
	I3_CLASS_DEFINE( CModulePcList );
private:		
	BOOL					m_bIsRunning;

	INT32					m_i32LogIdx;
	
protected:
	BOOL					m_bPCCafeActive;
	INT32					m_i32UsePCCafeIdx; 
	TYPE_IPLIST				m_pIpList[ PC_BUFFER_COUNT ][ PCCAFE_MAX_COUNT ];
	UINT32					m_iIpListCount[ PC_BUFFER_COUNT ];

	BOOL					m_bThaiIDPlusActive;
	INT32					m_i32UseIDPlusIdx; 
	TYPE_IDPLUS_LIST		m_pIDPlusList[ PC_BUFFER_COUNT ][THAI_IDPLUS_MAX_COUNT];
	UINT32					m_iIDPlusListCount[ PC_BUFFER_COUNT ];

	UINT32					m_iUpdateTime;

	// PCList용
	CDBConfig*				m_pIPListDBConfig;
	char					m_IDPlusDatabasePath[SERVER_STRING_COUNT];

	DB_QUERY_INFO			m_pDBPCCafeInfo;
	// Thai IDPlus용	
	DB_QUERY_INFO			m_pThaiIDPlus;

public: 
	CModulePcList(void); 
	virtual ~CModulePcList(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	BOOL					OnCreate(void);
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData);

	void					ErrorDBPcList( INT32 WorkThreadIdx , INT32 i32DBType = 0);

	//-----------------------------------------------------------------------------------------------------------------
	//
	BOOL					LoadPcCafeList_PcThread( INT32 i32ThreadIdx, TYPE_IPLIST * pList, UINT32 * Count);
	BOOL					LoadThaiIDPlusThread(INT32 i32ThreadIdx, TYPE_IDPLUS_LIST * pList, UINT32 * Count);	

	// 순차검색
	BOOL					IsPcCafe( UINT32 iIP, UINT8& ui8Rank );
	INT32					IsThaiIDPlus(T_UID i64UID);
	
};

extern CModulePcList * g_pModulePcList; 

#endif
