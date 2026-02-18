#ifndef __MODULE_DATABASE_NICK_H__
#define __MODULE_DATABASE_NICK_H__

#include "S2RingBuffer.h"

class CSystemTimer;
class i3ODBC; 
class CADODatabase; 

class CModuleDBNick
{
	HANDLE					m_hThread;
	bool					m_bIsRunning;

	INT32					m_i32LogIdx;

	CDBConfig*				m_pSIADBConfig;
	CADODatabase*			m_pADONick;
	S2RingBuffer*			m_pRingNickNameIn;
	S2RingBuffer*			m_pRingNickNameOut;

	CSystemTimer*			m_pSystemTimer;


protected:
	void					ErrorDBNick(INT32 i32ThreadIdx, wchar_t* wstrSQL);
	void					_DataBaseResultFailLog(INT32 i32ThreadIdx, wchar_t* wstrSQL);


public:
	CModuleDBNick();
	virtual ~CModuleDBNick();

	bool					Create();
	void					Destroy();
	void					Working(void); 
	INT32					PushNickName(INT8 i8Type, USER_NODE* pNode, TTCHAR * strNickname = _TT(""), TTCHAR * strOldNickName = _TT(""), T_ItemDBIdx TItemWareIdx = 0, SEND_GIFT* pstSendGift = NULL );

	S2RingBuffer*			GetRingOut()						{	return m_pRingNickNameOut;	}
};

#endif