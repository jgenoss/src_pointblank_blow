#ifndef __DATABASE_H__
#define __DATABASE_H__

class CADODatabase; 
class CRingBuffer;

typedef struct __tag_RingBufferSet
{
	CRingBuffer*	_pRingGetIn;
	CRingBuffer*	_pRingGetOut;
	CRingBuffer*	_pRingBuyIn;
	CRingBuffer*	_pRingBuyOut;
	CRingBuffer*	_pRingLinkIn;
	CRingBuffer*	_pRingLinkOut;
}TYPE_RING_BUFFER_SET;

class CModuleDataBase : public i3Thread
{
	I3_CLASS_DEFINE( CModuleDataBase );

	BOOL					m_bRunning;

	char					m_strUdlFilePath[SERVER_STRING_COUNT];
	CADODatabase*			m_pDB;

	CRingBuffer*			m_pRingGetIn;
	CRingBuffer*			m_pRingGetOut;
	CRingBuffer*			m_pRingBuyIn;
	CRingBuffer*			m_pRingBuyOut;
	CRingBuffer*			m_pRingLinkIn;
	CRingBuffer*			m_pRingLinkOut;

	INT32					m_i32LogThreadIdx;

	void					_ReConnectDataBase( char* strQuery );

	BOOL					_GetUserCash( TYPE_GET_CASH_IN* pRingIn, TYPE_GET_CASH_OUT* pRingOut );
	BOOL					_ItemBuy( TYPE_ITEM_BUY_IN* pRingIn, TYPE_ITEM_BUY_OUT* pRingOut );
	BOOL					_InsertUserCash( TYPE_LINK_IN* pRingIn );
	BOOL					_UpdateUserCash( char* strID, INT64 i64Cash, UINT64 ui64Idx = 0 );

public:
	CModuleDataBase(void);
	virtual ~CModuleDataBase(void);
	
	BOOL					OnCreate( char* strFilePath, TYPE_RING_BUFFER_SET* pRingBufferSet );
	void					OnDestroy();
	virtual UINT32			OnRunning( void* pUserData );

	

	

	//-----------------------------------------------------------------------------------------------------------------
	void ConvertString(char * pOutStr, const char * pInStr)
	{
		INT32 InCount  = 0; 
		INT32 OutCount = 0; 
		while(pInStr[InCount] != 0)
		{
			if( pInStr[InCount] == 39 )//"'"
			{
				pOutStr[OutCount] = pInStr[InCount];
				OutCount++; 
			}
			pOutStr[OutCount] = pInStr[InCount];

			InCount++;
			OutCount++;

			if(CONVERT_STRING_COUNT <= OutCount)break; //이런일은 없지만 예외 처리입니다. 
		}
		pOutStr[OutCount] = 0; 
		return; 
	}
};

#endif 
