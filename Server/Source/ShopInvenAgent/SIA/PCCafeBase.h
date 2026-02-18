#ifndef __PCCAFE_BASE_H__
#define __PCCAFE_BASE_H__

class CPCCafeBase
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bRunning;
	
protected:
	INT32					m_i32Idx;						// 로그용 Idx

	i3RingBuffer*			m_pPushBuffer;
	i3RingBuffer*			m_pPopBuffer;

	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeBase();
	virtual ~CPCCafeBase();

	virtual BOOL			OnCreate( INT32 i32Idx );
	virtual void			OnDestroy();

	void					ReleaseThread();				// OnDestroy 때 꼭 가장 먼저 호출 해 주셔야 합니다.

	void					OnBaseRunning();
	virtual BOOL			OnRunning();

	INT32					Push_T( PCCAFE_INPUT* pInputData )					{	return m_pPushBuffer->Push( pInputData );		}

	INT32					PopCount_T()										{	return m_pPopBuffer->GetBufferCount();			}
	PCCAFE_OUTPUT*			Pop_T()												{	return (PCCAFE_OUTPUT*)m_pPopBuffer->Pop();	}
	void					PopIdx_T()											{	m_pPopBuffer->PopIdx();							}
};

#endif