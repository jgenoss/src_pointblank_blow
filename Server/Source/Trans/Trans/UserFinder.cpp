#include "pch.h"
#include "UserFinder.h"
#include "HashTable.h"
#include "CircularIndex.h"

#define NO_WRITE_LOG_FOR_USERFINDER	// UserFinder에 로그를 남기지 않습니다.

I3_CLASS_INSTANCE(CUserFinder, i3ElementBase);

CUserFinder* g_pUserFinder = NULL;

//I3List에서 앞줄인지 뒷줄인지 검색
static INT32 _UID_CompareProc( void* p1, void* p2 )
{
	T_UID	i64UID1	= ((USER_NODE*)p1)->_i64UID;
	T_UID	i64UID2	= ((USER_NODE*)p2)->_i64UID;

	if( i64UID1 > i64UID2 )				return 1;
	else if( i64UID1 < i64UID2 )		return -1;

	return 0;
}

static INT32 _NickCompareProc(void* p1,void* p2)
{
	USER_NODE* pNode1 = (USER_NODE*)p1;
	USER_NODE* pNode2 = (USER_NODE*)p2;

	return i3String::Compare(pNode1->_sBasic.m_strNick, pNode2->_sBasic.m_strNick);
}


CUserFinder::CUserFinder()
{
	m_ppUserNode		= NULL;
	m_ui32MaxUserCount	= 0;
	m_ui32UserCount		= 0;
	i3mem::FillZero( m_ui32LocaleUserCount, sizeof(UINT32)*LOCALE_CODE_MAX );

	m_ui32Diviner		= MAX_CONNECT_USER_DIVISOR;	

	m_pUID_Table		= NULL;
	m_pNickTable		= NULL;

	m_pEmptyBuffer		= NULL;
}

CUserFinder::~CUserFinder()
{
	Destroy();
}

bool CUserFinder::Create(void)
{
	m_ui32MaxUserCount = g_pConfig->m_ui32MaxUserCount;

	//Make Hash Table 
	m_pUID_Table	= CHashTable::NewObject();
	if( NULL == m_pUID_Table )														return false;
	if( false == m_pUID_Table->Create(MAX_CONNECT_USER_DIVISOR,	_UID_CompareProc) )	return false;

	m_pNickTable	= CHashTable::NewObject();
	if( NULL == m_pNickTable )														return false;
	if( false == m_pNickTable->Create(MAX_CONNECT_USER_DIVISOR,	_NickCompareProc) )	return false;

	//Make User Node 
	m_ppUserNode = (USER_NODE**)i3MemAlloc( sizeof(USER_NODE*) * m_ui32MaxUserCount );
	if( NULL == m_ppUserNode )														return false;

	for( UINT32 i = 0 ; i < m_ui32MaxUserCount ; i++ )
	{
		m_ppUserNode[i] = (USER_NODE*)i3MemAlloc(sizeof(USER_NODE));
		if( NULL == m_ppUserNode[i] )												return false;
		if( FALSE == m_ppUserNode[i]->Create() )									return false;
	}

	//Create 
	m_pEmptyBuffer = CCircularIndex::NewObject();
	if( NULL == m_pEmptyBuffer )													return false;
	if( false == m_pEmptyBuffer->Create(m_ui32MaxUserCount) )						return false;

	return true; 
}

void CUserFinder::Destroy()
{
	I3_SAFE_RELEASE( m_pEmptyBuffer );
	I3_SAFE_RELEASE( m_pUID_Table );
	I3_SAFE_RELEASE(m_pNickTable);

	if( m_ppUserNode )
	{
		for( UINT32 i = 0 ; i < m_ui32MaxUserCount ; i++ )
		{
			I3_SAFE_DELETE(m_ppUserNode[i]->m_pDesignV1);
			I3_SAFE_DELETE(m_ppUserNode[i]->m_pDesignV2);

			I3MEM_SAFE_FREE(m_ppUserNode[i]);
		}
		I3MEM_SAFE_FREE(m_ppUserNode);
	}
}

USER_NODE* CUserFinder::Insert(USER_NODE * pNode)
{
	//빈슬롯을 찾는다. 
	UINT32	ui32Idx;
	if( FALSE == m_pEmptyBuffer->Pop( &ui32Idx ) )	return NULL;
	
	//기록 하기 
	pNode->_ui32State			= USER_STATE_LOGIN;
	pNode->_ui32ConnectState	= MAKE_FRIEND_STATE( FRIEND_STATE_ONLINE, pNode->_i32Serveridx, 0, 0 );
	pNode->m_dt32LoginDate		= i3ThreadTimer::LocalTime();
	pNode->_ui32NowConnectTime	= i3ThreadTimer::GetServerTime();
	pNode->_idx					= ui32Idx;

	USER_NODE * pInsertNode		= m_ppUserNode[ ui32Idx ];
	i3mem::Copy(pInsertNode, pNode, sizeof(USER_NODE));
	m_ui32UserCount++;
	if( LOCALE_CODE_MAX > pInsertNode->m_ui8LocaleCode )
	{
		m_ui32LocaleUserCount[ pInsertNode->m_ui8LocaleCode ]++;
	}

	// 해시테이블에 넣기
	if (FALSE == m_pUID_Table->Insert( pInsertNode->_i64UID, pInsertNode)) return FALSE;

	return pInsertNode; 
}

BOOL CUserFinder::InsertNameHash( USER_NODE* pNode )
{
	UINT32	ui32Nick_key;

	//NickName 테이블에 넣기
	if(pNode->_sBasic.m_strNick[0] == 0)	return FALSE;
		
	ui32Nick_key = CHashTable::MakeKeyForString(pNode->_sBasic.m_strNick);
	if(FALSE == m_pNickTable->Insert(ui32Nick_key, pNode)) return FALSE;

	return TRUE;
}

BOOL CUserFinder::Delete( T_UID TUID )
{
	USER_NODE* pNode = GetUserNodeByUID( TUID );
	if( NULL == pNode )	return FALSE;

	m_pUID_Table->Delete( pNode->_i64UID, pNode );

	DeleteNameHash( pNode->_sBasic.m_strNick );

	//돌려줄때 기존에 있는 변수를 초기화 합니다.
	m_ppUserNode[pNode->_idx]->_i64UID			= 0;
	m_ppUserNode[pNode->_idx]->_i32Sessionidx	= -1;
	m_ppUserNode[pNode->_idx]->_i32Serveridx	= 0;

	m_pEmptyBuffer->Push(pNode->_idx);
	m_ui32UserCount--;
	if( LOCALE_CODE_MAX > pNode->m_ui8LocaleCode )
	{
		m_ui32LocaleUserCount[ pNode->m_ui8LocaleCode ]--;
	}

	return TRUE; 
}

BOOL CUserFinder::DeleteNameHash( TTCHAR* strNickname )
{
	UINT32 ui32Nick_key = 0;

	USER_NODE * pNode = GetUserNodeByNick( strNickname );

	if( FALSE == pNode ) return FALSE;
	
	ui32Nick_key = CHashTable::MakeKeyForString(strNickname);
	m_pNickTable->Delete(ui32Nick_key, pNode);
	
	return TRUE;
}

BOOL CUserFinder::UpdateAlivePacket( T_UID TUID, INT32 i32Session, INT32 i32ServerIdx)
{
	USER_NODE * pNode = GetUserNodeByUID( TUID ); 
	if(pNode == NULL)return FALSE; 

	pNode->_LastRecvTimeST = i3ThreadTimer::GetServerTime();

	return TRUE; 
}

USER_NODE* CUserFinder::GetUserNodeByUID( T_UID TUID )
{
	if( 0 == m_ui32UserCount )		return NULL;
	if( 0 > TUID )					return NULL;

	i3BinList* pList = m_pUID_Table->Retrieve( TUID );
	if ( NULL == pList )			return NULL;
	if (0 == pList->GetCount())		return NULL;

	INT32 i32Idx = pList->FindItem( &TUID );
	if( 0 > i32Idx )				return NULL;

	if( ((USER_NODE*)pList->GetItem( i32Idx ))->_i64UID != TUID )	return NULL;

	return (USER_NODE*)pList->GetItem( i32Idx );
}

USER_NODE* CUserFinder::GetUserNodeByNick( TTCHAR * pstrNick )
{
	if (0 == m_ui32UserCount)		return FALSE;

	UINT32		ui32key	= CHashTable::MakeKeyForString( pstrNick );
	i3BinList * pList	= m_pNickTable->Retrieve( ui32key );

	if (0 == pList->GetCount())		return FALSE;

	USER_NODE node;
	ZeroMemory(&node,sizeof(USER_NODE));
	//i3String::Copy(node._nick, pNick);
	i3String::NCopy( node._sBasic.m_strNick, pstrNick, NET_NICK_NAME_SIZE-1);

	INT32 i32idx = pList->FindItem(&node);
	
	if (0 > i32idx)					return FALSE;

	return (USER_NODE*)pList->GetItem(i32idx);	
}

#if defined I3_DEBUG
void CUserFinder::DumpHashTables(void)
{
	I3PRINTF("HashTable DumpCount\n");

	I3PRINTF("UID Table\n");
	m_pUID_Table->DumpCount();
}
#endif
