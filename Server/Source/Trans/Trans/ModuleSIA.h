#ifndef __MODULE_SIA_H__
#define __MODULE_SIA_H__

class CModuleSIASocket; 

#define NET_PACKET_BUFFER_SIZE	8912

#pragma pack( push, 1 )
struct SIA_SEND_MESSAGE
{	
	UINT16	_ui16TotalSize;
	UINT16	_ui16DataSize;
	UINT16	_ui16Protocol;
	char	_strMessage[NET_PACKET_BUFFER_SIZE];		//ÃæºÐ
	
	SIA_SEND_MESSAGE()
	{
		Clear();
	}
	void	Clear()
	{
		_ui16TotalSize	= 6;
		_ui16DataSize	= 0; 
	}
	void	SetProtocol( UINT16 ui16Protocol )
	{
		_ui16Protocol = ui16Protocol;
	}
	void	WriteData(void* pData, UINT16 ui16Size )
	{
		if( (_ui16TotalSize + ui16Size) > NET_PACKET_BUFFER_SIZE )
		{
#ifdef _DEBUG
			I3ASSERT( "Trans->SIA Write Packet OverFlow" );
#endif
			return;
		}

		i3mem::Copy( &_strMessage[ _ui16DataSize ], pData, ui16Size );
		_ui16DataSize	= _ui16DataSize + ui16Size;
		_ui16TotalSize	= _ui16TotalSize + ui16Size;
	}
	void	ReadData(void* pData, UINT16 ui16Size )
	{
		i3mem::Copy( pData, &_strMessage[ _ui16DataSize ], ui16Size );
		_ui16DataSize	= _ui16DataSize + ui16Size;
		_ui16TotalSize	= _ui16TotalSize + ui16Size;
	}
};

struct SIA_NET_BUFFER
{
	SIA_SEND_MESSAGE**	_ppNetMessage;
	INT32				_i32WritePos;
	INT32				_i32ReadPos;
}; 

#pragma pack( pop )

class CModuleSIA
{
private:
	INT32						m_i32LogIdx;
	BOOL						m_bIsRunning;
	HANDLE						m_hThread;

	CModuleSIASocket	*		m_pSIASocket;
	UINT32						m_ui32LastSendTime;
	UINT32						m_ui32ConnectTime; 
	//UINT32					m_iSystemInfoSendTime;

	SIA_NET_BUFFER				m_SendBufferT;

public: 
	CModuleSIA(void); 
	virtual ~CModuleSIA(void); 	
	
	BOOL						OnCreate(); 
	void						OnDestroy(void);
    	
	UINT32						OnRunning(); 

	BOOL						SendLogin(	ST_PACKET_LOGIN_REQ* pLoginData );
	BOOL						SendLogout( ST_PACKET_LOGOUT_REQ* pLogoutData );
	BOOL						BuyGoods_T(	PACKET_BUY_GOODS_REQ *pBuyReq, void* pBuffer );
	BOOL						GetCash_T(	PACKET_GET_CASH_REQ *pGetCashReq );
	BOOL						UseGiftCoupon_T( PACKET_USE_GIFTCOUPON_REQ* pUseCouponReq );

	BOOL						SendPCCafeLogin( PACKET_PCCAFE_LOGIN_REQ* pPCCafeLoginReq );
	BOOL						SendPCCafeCheckAmount( PACKET_PCCAFE_CHECK_AMOUNT_REQ* pPCCafeCheckAmountReq );
	
	BOOL						IsActive();
};

extern CModuleSIA * g_pModuleSIA; 

#endif

