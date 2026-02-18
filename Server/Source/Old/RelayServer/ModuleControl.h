#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

typedef struct _tagControlNetBuffer
{
	i3NetworkPacket	**	_ppNetMessage;
	INT32				_WritePos;
	INT32				_ReadPos;
}CONTROL_NET_BUFFER; 


class CModuleControlSocket; 

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );
private:	
	HANDLE						m_hKillEvent;	
	CModuleControlSocket	*	m_pControlSocket;
	UINT32						m_iConnectTime; 
	UINT32						m_iSystemInfoSendTime;

// 
	UINT32						m_wUpdateCheck;
	UINT32						m_wUpdateCheckStartIdx;

	CONTROL_NET_BUFFER			m_SendBuffer;

public: 
	CModuleControl(void); 
	virtual ~CModuleControl(void); 	
	
	BOOL	OnCreate(); 
	void	OnDestroy(void);
	INT32	_ServerConnect();
    	
	virtual UINT32 OnRunning( void * pUserData ); 

	BOOL	SendRelayRegisterGroupRequest( INT32 nResult, P_BATTLE_ROOM_CREATE_ACK * pAckInfo );	
};

extern CModuleControl * g_pModuleControl; 

#endif
