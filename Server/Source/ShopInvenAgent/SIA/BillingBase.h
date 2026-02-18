#ifndef __BILLING_BASE_H__
#define __BILLING_BASE_H__

class CBillingBase
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bRunning;
	
protected:
	INT32					m_i32Idx;						// 로그용 Idx
	UINT8					m_ui8LinkMethod;

	i3RingBuffer*		m_pPushBuffer;
	i3RingBuffer*		m_pPopBuffer;

	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );	// pInput->m_eLinkType 의 값에 따라 작업(구매,잔액조회)을 구분해야합니다.

public:
	CBillingBase();
	virtual ~CBillingBase();

	virtual BOOL			OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx );
	virtual void			OnDestroy();

	void					ReleaseThread();				// OnDestroy 때 꼭 가장 먼저 호출 해 주셔야 합니다.

	void					OnBaseRunning();
	virtual BOOL			OnRunning();

	INT32					Push_T( BILLING_INPUT* pInputData )					{	return m_pPushBuffer->Push( pInputData );		}

	INT32					PopCount_T()										{	return m_pPopBuffer->GetBufferCount();			}
	BILLING_OUTPUT*			Pop_T()												{	return (BILLING_OUTPUT*)m_pPopBuffer->Pop();	}
	void					PopIdx_T()											{	m_pPopBuffer->PopIdx();							}
};

#endif