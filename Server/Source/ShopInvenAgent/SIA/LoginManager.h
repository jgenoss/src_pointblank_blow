#ifndef __LOGIN_MANAGER_H__
#define __LOGIN_MANAGER_H__

#include "LoginBase.h"

class CLoginManager
{
private:
	UINT32					m_ui32PushThreadIdx;				// Input Thread Idx
	UINT32					m_ui32PopThreadIdx;					// Output Thread Idx

	UINT32					m_ui32WorkThreadCount;

protected:
	CLoginBase		**		m_ppLogInModule; 

public:
	CLoginManager();
	virtual ~CLoginManager();

	BOOL					OnCreate( UINT8 ui8MethodIdx );
	void					OnDestroy(void);

	//Task Tread 에서 호출 됩니다.
	INT32					Push_T( LOGIN_INPUT* pLogInData );	

	/// LOGIN 처리 결과의 개수 (모든 Worker쓰레드의 결과 개수)
	INT32					PopCount_T(void);
	/// LOGIN_OUTPUT 버퍼주소를 반환 (Pop_T()를 실행할 때마다 데이터가 있는 Worker쓰레드를 순차적으로 검색해 반환)
	LOGIN_OUTPUT *			Pop_T(void);
	void					PopIdx_T(void)	{ m_ppLogInModule[m_ui32PopThreadIdx]->PopIdx_T();  }
};

#endif //_LOGINMANGER_H
