#ifndef _DATABASE_H__
#define _DATABASE_H__

#include "RingBuffer.h"
#include "DataBase_Script.h"

class i3ODBC; 
class CUserSession; 
class CADODatabase; 
/*-----------------------------------------------------------------------
Name : CDataBase
2007 03 06 : 버그확인 : 버그없음 : Query 버퍼를 모두 512로 변경
2007 11 24 : 버그확인 : 버그없음 : Query 버퍼를 클래스 생성할때 만듬 
2007 12 26 : 버그확인 : 크리스마스 이벤트에서 크래쉬 .확인 중.
2008 08 26 : cash 업데이트 하는것을 삭제함. DB에 넣을 필요없음.
-----------------------------------------------------------------------*/

//유저가 가지고 있는 도전과제 
typedef struct _DataQueryInfo
{
	CADODatabase	*	_pDB;
	wchar_t				_pQuery[SQL_STRING_COUNT];
}DB_QUERY_INFO;

class CDataBase :	public i3Thread
{
	I3_CLASS_DEFINE( CDataBase );
protected :
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Connect Value 

	CDBConfig*				m_pInfoDBConfig;
	INT32					m_i32WorkThreadCount;			//Config에 있지만..하나 더 가지고 있는다. 

	DB_QUERY_INFO			m_DBUser;					//Work Thread (Make Thread Count)
	NSM_CRingBuffer**		m_ppRingIn;
	NSM_CRingBuffer*		m_pRingOut;

	char					m_strBinaryTemp[ SQL_STRING_COUNT ];	// Qurey에서 사용

	BOOL					m_bRunning;

	//void ConvertString(char * pOutStr, const char * pInStr)
	//{
	//	INT32 InCount  = 0; 
	//	INT32 OutCount = 0; 
	//	while(pInStr[InCount] != 0)
	//	{
	//		if( pInStr[InCount] == 39 )//"'"
	//		{
	//			pOutStr[OutCount] = pInStr[InCount];
	//			OutCount++; 
	//		}
	//		pOutStr[OutCount] = pInStr[InCount];

	//		InCount++;
	//		OutCount++;

	//		if(CONVERT_STRING_COUNT <= OutCount)break; //이런일은 없지만 예외 처리입니다. 
	//	}
	//	pOutStr[OutCount] = 0; 
	//	return; 
	//}
	
public:
	CDataBase(void);
	virtual ~CDataBase(void);

	BOOL					OnCreate(INT32 i32WorkThreadCount, char * pstrUdlFilePath); 
	void					OnDestroy( void );

	void					ErrorDB( void ); 
	void					ReConnectDataLog( void );
	NSM_CRingBuffer*		GetRingOutBuffer( void )			{ return m_pRingOut; }
	virtual UINT32			OnRunning( void * pUserData);
	INT32					RingInputDB( INT32 i32WorkThread, TYPE_RING_IN* pIn );	
	void					SetOption( TYPE_RING_IN* pIn );
    void					GetOption( TYPE_RING_IN* pIn, TYPE_RING_OUT* pOut );
};

extern CDataBase * g_pDataBase;

#endif 
