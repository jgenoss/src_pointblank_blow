#ifndef __MODULE_TASK_
#define __MODULE_TASK_

#include "LoginManager.h"
#include "BillingManager.h"
#include "PCCafeManager.h"
#include "AccountTrans.h"
#include "MainStreamBuffer_Buy.h"
#include "MainStreamBuffer_Login.h"

struct SALE_COUPON;

class CModuleTask
{
private		: 
	//For Thread 
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bIsRunning;

	CLoginManager**			m_ppModuleLogin;
	CBillingManager**		m_ppModuleBilling;
	CPCCafeManager*			m_pModulePCCafe;
	CAccountTrans*			m_pModuleAccountTrans;

	CMainStreamBuffer_Buy*	m_pMainBufferBuy;
	CMainStreamBuffer_Login* m_pMainBufferLogin;

	//For Log
	UINT32					m_ui32WorkCount_Net;
	UINT32					m_ui32WorkCount_Billing;
	UINT32					m_ui32SleepCount;
	UINT32					m_ui32WorkCount;
	UINT32					m_ui32LogFileHour;

	UINT32					m_ui32LoginTimeArg;

	void	ResetCounter(void)
	{
		m_ui32WorkCount_Net		= 0;
		m_ui32WorkCount_Billing	= 0;
		m_ui32SleepCount		= 0;
		m_ui32WorkCount			= 0;
	}

private		: 
	void	_ParshingNet( UINT32 ui32ServerIdx, NET_PACKET_BUFFER * pPacket );
	void	_ParshingLogin( LOGIN_OUTPUT* pOutBuffer );
	void	_ParshingBilling( BILLING_OUTPUT* pOutBuffer, UINT8 ui8BillingMethod );
	void	_ParshingPCCafe( PCCAFE_OUTPUT* pOutBuffer );

	// New
	void	SendDataLink( BILLING_OUTPUT* pLinkOut, void* pData = NULL );

public		: 
	CModuleTask(); 
	virtual ~CModuleTask(); 

	INT32	OnCreate_M(void);
	bool	OnDestroy(void);
	void	OnWorking(void);

	CPCCafeManager*	GetModulePCCafe()	{ return m_pModulePCCafe;	}
};

extern CModuleTask * g_pModuleTask; 

#endif
