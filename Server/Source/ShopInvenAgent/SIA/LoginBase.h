#ifndef __LOGIN_BASE_H__
#define __LOGIN_BASE_H__

class CLoginBase
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bRunning;

protected:
	INT32					m_i32Idx;
	UINT8					m_ui8LinkMethod;
	INT32					m_i32WorkMaxCount;

	i3RingBuffer*		m_pPushBuffer;
	i3RingBuffer*		m_pPopBuffer;

	virtual BOOL			_WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput ); 
	virtual void			_LoginExtension( LOGIN_INPUT *pInput, LOGIN_OUTPUT * pOutput );

	virtual	BOOL			_Create();
	virtual	void			_Destroy();

public:
							CLoginBase();	
	virtual					~CLoginBase();

	virtual BOOL			OnCreate( INT32 i32idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx );
	void					OnDestroy(void);

	virtual UINT32			OnRunning();
	virtual BOOL			OnRunningEx();
	
	INT32					Push_T( LOGIN_INPUT* pInputData );

	INT32					PopCount_T()										{	return m_pPopBuffer->GetBufferCount();			}
	LOGIN_OUTPUT*			Pop_T()												{	return (LOGIN_OUTPUT*)m_pPopBuffer->Pop();		}
	void					PopIdx_T()											{	m_pPopBuffer->PopIdx();							}


};

#endif //__LOGIN_BASE_H__
