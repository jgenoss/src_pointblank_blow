#ifndef __USERFINDER_H
#define __USERFINDER_H

class CHashTable;
class CCircularIndex;

//-----------------------------------------------------------------------------------------------------------------------------
// Task,UserDB Thread에서만 접근합니다. 
// 싱글 쓰레드만 지원합니다. 다른 쓰레드에서 접근하면 크래쉬 합니다. 
// 이전까지는 다른 것도 접근하고 있었는데 이제 부터는 접근하면 안됩니다.		2009-02-25 김성백

class CUserFinder: public i3ElementBase
{
	I3_CLASS_DEFINE(CUserFinder);

private:
	USER_NODE		**	m_ppUserNode;
	UINT32				m_ui32MaxUserCount;
	UINT32				m_ui32UserCount;
	UINT32				m_ui32LocaleUserCount[ LOCALE_CODE_MAX ];
	UINT32				m_ui32Diviner;

	CHashTable		*	m_pUID_Table;
	CHashTable		*	m_pNickTable;

	CCircularIndex	*	m_pEmptyBuffer;	

public:
	CUserFinder();
	virtual ~CUserFinder();

	bool					Create( void );
	void					Destroy();

	USER_NODE*				Insert( USER_NODE* pNode );
	BOOL					InsertNameHash( USER_NODE* pNode );
	BOOL					Delete( T_UID TUID );
	BOOL					DeleteNameHash( TTCHAR* pstrNickname );
	UINT32					GetLocalUserCount( LOCALE_CODE eLocaleCode )		{ return m_ui32LocaleUserCount[ eLocaleCode ]; }
	UINT32					GetUserCount(void)									{ return m_ui32UserCount; }
	UINT32					GetMaxUserCount(void)								{ return m_ui32MaxUserCount; }

	//Update	
	BOOL					UpdateAlivePacket( T_UID i64DbIdx, INT32 i32Session, INT32 i32ServerIdx);

	//Get UserNode
	USER_NODE	*			GetUserNodeByNick( TTCHAR * pstrNick );
	USER_NODE	*			GetUserNodeByUID( T_UID TUID );
	USER_NODE	*			GetUserNodeByIdx(INT32 i32Idx)		{ return m_ppUserNode[i32Idx]; }	

#if defined I3_DEBUG
	void DumpHashTables(void);
#endif
};

extern CUserFinder* g_pUserFinder;

#endif //__USERFINDER_H
