#ifndef __MODULE_AUTH_H__
#define __MODULE_AUTH_H__

class CUserSession; 
class CModuleAuthSocket; 

typedef struct _tagAuthNetBuffer
{
	i3NetworkPacket	**	_ppNetMessage;
	INT32				_WritePos;			
	INT32				_ReadPos;			
}AUTH_NET_BUFFER; 

/*-----------------------------------------------------------------------
Name : CModuleAuth
2007 07 23 : Auth Server의 변경으로 버퍼를 삭제하고 실시간을 메시지를 보내는것으로 변경.
-----------------------------------------------------------------------*/
class CModuleAuth : public i3Thread
{
	I3_CLASS_DEFINE( CModuleAuth );
private:	
	HANDLE					m_hKillEvent;	
	CModuleAuthSocket	*	m_pAuthSocket;
	UINT32					m_iLastSendTime; 	
	i3NetworkPacket			m_AuthPacket;
	UINT32					m_iConnectTime; 

	INT32					m_ThreadCount;						//여기에도 하나 가지고 있음
	AUTH_NET_BUFFER			m_MainMessage;						//Main Buffer 용
	AUTH_NET_BUFFER			m_AuthMessage;						//Auth Buffer 용
	AUTH_NET_BUFFER		*	m_UserMessage;						//User Buffer용 

	BOOL _CheckBufferIdx(AUTH_NET_BUFFER	* pBufferMessage)
	{
		if( pBufferMessage->_WritePos == pBufferMessage->_ReadPos )
		{
			pBufferMessage->_WritePos	= 0;
			pBufferMessage->_ReadPos	= 0;
		}
		else
		{			
			if( pBufferMessage->_ReadPos > 2000000000 )return FALSE; 
		}
		return TRUE; 
	}

	INT32		m_SendAuthCount; 
	UINT32		m_iShopCheckTime; 
public: 
	CModuleAuth(void); 
	virtual ~CModuleAuth(void); 	
	
	BOOL	OnCreate(INT32 ThreadCount ); 


	void	OnDestroy(void); 

	//--------------------------------------------------------------------------------------------------------------------------
	//Send Auth 
	BOOL	SendLogIn(			INT32 WorkIdx, INT32 * piSessionIdx, char * pId, char * pPass, UINT64 * pMacAddress, UINT32 * pNetIp);	//로그인(일반)
	BOOL	SendLogInWEB(		INT32 WorkIdx, INT32 * piSessionIdx, char * pSessionKey, UINT64 * pMacAddress, UINT32 * pNetIp);		//로그인(WEB)
	BOOL	SendLogOut(			INT32 WorkIdx, unsigned char * pNcGusId, INT32 DbIdx);								//로그아웃
	BOOL	SendServerEnter(	INT32 WorkIdx, CUserSession * pSession);											//서버입장(서버이동)
	BOOL	SendServerLeave(	INT32 WorkIdx, CUserSession * pSession);											//서버나가기(서버이동)
	BOOL	SendUserNowInfo(	INT32 WorkIdx, UINT32 SessionIdx, INT32 ChannelIdx, INT32 RoomIdx, INT32 SlotIdx);	//유저정보 요청하기 

	//Shop
	BOOL	SendBuyGoods(	INT32 WorkIdx, CUserSession * pSession, INT32 GoodsIdx);						//상점에서 구입하기
	BOOL	SendInsertItem(	INT32 WorkIdx, CUserSession * pSession, INT32 ItemIdx, INT32 authDay);			//상점에서 아이템 넣기 
	BOOL	SendAuthItem(	INT32 WorkIdx, CUserSession * pSession, INT32 InventoryIdx, UINT64 ItemId);		//사용 인증 요청하기 

	//Friend 
	BOOL	SendFriendInviteUser(	INT32 WorkIdx, CUserSession * pSession, UINT8 FriendIdx);				//친구 초대하기 	
	BOOL	SendFriendAccept(		INT32 WorkIdx, CUserSession * pSession, UINT8 Idx);						//친구 허용하기		
	BOOL	SendFriendInsert(		INT32 WorkIdx, CUserSession * pSession, UINT32 DbIdx);					//친구 추가하기		
	BOOL	SendFriendDelete(		INT32 WorkIdx, CUserSession * pSession, UINT8 Idx);						//친구 삭제하기		
	BOOL	SendFriendRoomEnter(	INT32 WorkIdx, CUserSession * pSession, UINT8 ChIdx, UINT16 RoomIdx);	//친구 방에 들어가기 
	BOOL	SendFriendRoomLeave(	INT32 WorkIdx, CUserSession * pSession );								//친구 방에서 나오기 
	
	virtual UINT32 OnRunning( void * pUserData ); 
};

//extern CModuleAuth * g_pModuleAuth; 

#endif
