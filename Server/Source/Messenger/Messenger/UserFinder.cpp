#include "pch.h"
#include "UserFinder.h"
#include "DataBase.h"
#include "HashTable.h"
#include "CircularIndex.h"

I3_CLASS_INSTANCE(CUserFinder, i3ElementBase);

//I3List에서 앞줄인지 뒷줄인지 검색
static INT32 _UID_CompareProc(void* p1,void* p2)
{
	USER_NODE* pNode1 = (USER_NODE*)p1;
	USER_NODE* pNode2 = (USER_NODE*)p2;

	INT64 i64Result = pNode1->_i64UID - pNode2->_i64UID;

	if(0 < i64Result)		{return 1;}	// 크다
	else if(0 > i64Result)	{return -1;}// 작다

	return 0;//같다
}

static INT32 _NickCompareProc(void* p1,void* p2)
{
	USER_NODE* pNode1 = (USER_NODE*)p1;
	USER_NODE* pNode2 = (USER_NODE*)p2;

	return i3String::Compare(pNode1->_strnick, pNode2->_strnick);
}

CUserFinder::CUserFinder()
{
	m_ui32MaxCount	= 0;
	m_ui32Count		= 0;

	m_ui32Diviner	= MAX_CONNECT_USER_DIVISOR;	// 20 이상 소수
#ifdef USE_TWO_POINTER
	m_ppUserNode	= NULL;
#else
	m_pUserNode		= NULL;
#endif
	m_pUIDTable		= NULL;
	m_pNickTable	= NULL;
	m_pEmptyBuffer	= NULL;
}

CUserFinder::~CUserFinder()
{	
	I3_SAFE_RELEASE(m_pUIDTable);
#ifdef USE_TWO_POINTER
	if( m_ppUserNode )
	{
		for( UINT32 i = 0 ; i < m_ui32MaxCount ; i++ )
		{
			I3MEM_SAFE_FREE( m_ppUserNode[i] );
		}
		I3MEM_SAFE_FREE(m_ppUserNode);
	}
#else
	I3MEM_SAFE_FREE(m_pUserNode);
#endif
	I3_SAFE_RELEASE(m_pNickTable);
	I3_SAFE_RELEASE(m_pEmptyBuffer);
}

BOOL CUserFinder::Create(UINT32 ui32MaxUser, UINT32 ui32Divisor)
{
#ifdef USE_TWO_POINTER
	I3ASSERT(NULL == m_ppUserNode);
#else
	I3ASSERT(NULL == m_pUserNode);
#endif
	I3ASSERT(ui32MaxUser);
	
	if( 0 < g_pConfig->m_i32UserMaxCount )
	{
		m_ui32MaxCount = g_pConfig->m_i32UserMaxCount;
	}
	else m_ui32MaxCount = ui32MaxUser;

	//Create User Buffer
#ifdef USE_TWO_POINTER
	m_ppUserNode = (USER_NODE**)i3MemAlloc(sizeof(USER_NODE*) * m_ui32MaxCount);
	if(m_ppUserNode == NULL)return FALSE;
	for( UINT32 i = 0 ; i < m_ui32MaxCount ; i++ )
	{
		m_ppUserNode[i] = (USER_NODE*)i3MemAlloc(sizeof(USER_NODE));
		if(m_ppUserNode[i] == NULL)return FALSE;
	}
#else
	m_pUserNode = (USER_NODE*)i3MemAlloc(sizeof(USER_NODE) * m_ui32MaxCount);
	if(NULL == m_pUserNode)return FALSE;
#endif

	m_pEmptyBuffer = CCircularIndex::NewObject();
	m_pEmptyBuffer->Create(m_ui32MaxCount);
	for(UINT32 i = 0; i < m_ui32MaxCount; i++)m_pEmptyBuffer->Push(i, FALSE);

	//Create HashTable 
	m_pUIDTable = CHashTable::NewObject();
	m_pUIDTable->Create(ui32Divisor, _UID_CompareProc);
	m_pNickTable = CHashTable::NewObject();
	m_pNickTable->Create(ui32Divisor, _NickCompareProc);


	return TRUE;
}

BOOL CUserFinder::Insert(USER_NODE* pNode)
{
#ifdef USE_TWO_POINTER
	I3ASSERT( m_ppUserNode );
#else
	I3ASSERT( m_pUserNode );
#endif
	I3ASSERT( pNode );

	USER_NODE	node;
	UINT32		ui32UID_key = 0;
	UINT32		ui32Nick_key = 0;
	UINT32		ui32idx = 0;

	//Max를 넘는다. 
	if (m_ui32Count >= m_ui32MaxCount)			return FALSE;

	// 이미 존재하면 추가하지 않는다	
	if (FindUid(pNode->_i64UID, &node))			return FALSE;

	//버퍼가 없으면 넣지 않는다. 	
	if (FALSE == m_pEmptyBuffer->Pop(&ui32idx))	return FALSE;

	//기록 하기 	
	pNode->_ui32idxBuffer	= ui32idx;

#ifdef USE_TWO_POINTER	
	USER_NODE * pPosition	= m_ppUserNode[ui32idx];
#else
	USER_NODE * pPosition	= m_pUserNode + ui32idx;
	// 변경
	//USER_NODE * pPosition	= GetUserNode(pNode->_idxBuffer);
#endif
	i3mem::Copy(pPosition, pNode, sizeof(USER_NODE));

	if (NULL == pPosition)
	{
		I3BCLOG(LOCATION, "CUserFinder::Insert(NULL) Alloc failed(idx(%d), m_nCount(%d), m_MaxCount(%d))", ui32idx, m_ui32Count, m_ui32MaxCount);
		return FALSE;
	}
	
#ifdef DEF_TEST_LOG
	// 임시
	if(FRIEND_MAX_COUNT < pPosition->_friendCount || RANK_MAX < pPosition->_iRank || MAX_CONNECT_USER_COUNT < pPosition->_idxBuffer)
	{
		I3BCLOG(LOCATION, "CUserFinder::Insert()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
				pPosition->_i64UID, pPosition->_iRank, pPosition->_idxBuffer, pPosition->_friendCount, pPosition->_nick);
	}
#endif

	m_ui32Count++;

	// UID
	ui32UID_key = CHashTable::MakeKeyForINT(pPosition->_i64UID);
	if (FALSE == m_pUIDTable->Insert(ui32UID_key, pPosition)) return FALSE;

	//NickName 테이블에 넣기
	if(pPosition->_strnick[0] != 0)
	{
		ui32Nick_key = CHashTable::MakeKeyForString(pPosition->_strnick);
		if(FALSE == m_pNickTable->Insert(ui32Nick_key, pPosition)) return FALSE;
	}
	
	//g_CurrentUser = m_nCount;
	//I3BCLOG(LOCATION, "[%d] CUserFinder::Insert()[UID(%I64d),idx(%d),UID_key(%d),Nick_key(%d),pPosition->_nick(%s)][User(%d)]", m_nCount, pNode->_i64UID, pNode->_idxBuffer, UID_key, Nick_key, pPosition->_nick, g_CurrentUser);
	return TRUE;
}

BOOL CUserFinder::Delete(INT32 i32ServerIdx, INT64 i64UID)
{
	USER_NODE node;
	UINT32 ui32UID_key = 0;
	UINT32 ui32Nick_key = 0;

	if( 0 == m_ui32Count )						return FALSE;
	if( !FindUid(i64UID, &node) )				return FALSE; 
	if( node._i32serveridx != i32ServerIdx )	return FALSE;
	if( m_ui32MaxCount <= node._ui32idxBuffer)
	{
		I3BCLOG(LOCATION, "[%d] [ERROR]CUserFinder::Delete(iServerIdx(%d),iUID(%I64d))[_UID(%I64d),_iRank(%d),_idxBuffer(%d),_friendCount(%d),UID_key(%d),Nick_key(%d),node._nick(%s)]", m_ui32Count, i32ServerIdx, i64UID, node._i64UID, node._ui32Rank, node._ui32idxBuffer, node._ui32friendCount, ui32UID_key, ui32Nick_key, node._strnick);
	}

	ui32UID_key = CHashTable::MakeKeyForINT(node._i64UID);
	m_pUIDTable->Delete(ui32UID_key, &node);

	// Nick이 있을경우만 테이블 생성(UpdateNick한 유저는 불가)
	if (node._strnick[0])
	{		
		ui32Nick_key = CHashTable::MakeKeyForString(node._strnick);
		m_pNickTable->Delete(ui32Nick_key, &node);
	}

	//I3BCLOG(LOCATION, "[%d] CUserFinder::Delete(iServerIdx(%d),iUID(%d))[_iUID(%I64d),_iRank(%d),_idxBuffer(%d),_friendCount(%d),UID_key(%d),Nick_key(%d),node._nick(%s)][User(%d)]", m_nCount, iServerIdx, i64UID, node._i64UID, node._iRank, node._idxBuffer, node._friendCount, UID_key, Nick_key, node._nick, g_CurrentUser);	
	
	//돌려줄때 기존에 있는 변수를 초기화 합니다. 
#ifdef USE_TWO_POINTER
	m_ppUserNode[node._ui32idxBuffer]->_i64UID			= 0;
	m_ppUserNode[node._ui32idxBuffer]->_i32sessionidx	= 0;
	m_ppUserNode[node._ui32idxBuffer]->_i32serveridx	= 0;
	m_ppUserNode[node._ui32idxBuffer]->_strnick[0]		= 0;
#else
	m_pUserNode[node._ui32idxBuffer]._i64UID			= 0;
	m_pUserNode[node._ui32idxBuffer]._i32sessionidx		= 0;
	m_pUserNode[node._ui32idxBuffer]._i32serveridx		= 0;
	m_pUserNode[node._ui32idxBuffer]._strnick[0]		= 0;
#endif

	m_pEmptyBuffer->Push(node._ui32idxBuffer);
	m_ui32Count--;

	//g_CurrentUser = m_nCount;

	return TRUE;
}

BOOL CUserFinder::FindUid(INT64 i64uid, USER_NODE * pNode)
{
	I3ASSERT(pNode);

	if (0 == m_ui32Count)
	{
		return FALSE;
	}

	UINT32 ui32key		= CHashTable::MakeKeyForINT(i64uid);
	i3BinList* pList	= m_pUIDTable->Retrieve(ui32key);

	if (0 == pList->GetCount())
	{
		return FALSE;
	}

	// 검색 비교군 설정
	USER_NODE node;
	node._i64UID = i64uid;

	INT32 i32idx = pList->FindItem(&node);
	
	if (0 > i32idx)
	{
		return FALSE;
	}

	i3mem::Copy(pNode, pList->GetItem(i32idx), sizeof(USER_NODE));

	//I3BCLOG(LOCATION, "CUserFinder::FindUid(uid(%I64d))[pNode->_idxBuffer(%d), key(%d),idxBuffer(%d), idx(%d)]", i64uid, pNode->_idxBuffer, key, node._idxBuffer, idx);
	return TRUE;
}

BOOL CUserFinder::FindNick(TTCHAR * pstrNick, USER_NODE* pNode)
{
	if (0 == m_ui32Count)return FALSE;

	UINT32		ui32key	= CHashTable::MakeKeyForString( pstrNick );
	i3BinList * pList	= m_pNickTable->Retrieve( ui32key );

	if (0 == pList->GetCount())return FALSE;

	USER_NODE node;
	i3mem::FillZero(&node, sizeof(USER_NODE));
	//i3String::Copy(node._nick, pNick);
	i3String::NCopy(node._strnick, pstrNick, NET_NICK_NAME_SIZE-1);

	INT32 i32idx = pList->FindItem(&node);
	
	if (0 > i32idx)return FALSE;

	i3mem::Copy(pNode, pList->GetItem(i32idx), sizeof(USER_NODE));

	//I3BCLOG(LOCATION, "CUserFinder::FindNick(pNick(%s))[pNode->_idxBuffer(%d), key(%d),idxBuffer(%d), idx(%d)]", pNick, pNode->_idxBuffer, key, node._idxBuffer, idx);
	return TRUE;
}


BOOL CUserFinder::UpdateByUid(USER_NODE* pNode)
{
	I3ASSERT(pNode);

	if (0 == m_ui32Count)
	{
		return FALSE;
	}

	UINT32 ui32key		= CHashTable::MakeKeyForINT(pNode->_i64UID);
	i3BinList* pList	= m_pUIDTable->Retrieve(ui32key);

	if (0 == pList->GetCount())
	{
		return FALSE;
	}

	// 검색 비교군 설정
	INT32 i32idx = pList->FindItem(pNode);
	
	if (0 > i32idx)
	{
		return FALSE;
	}

	i3mem::Copy(pList->GetItem(i32idx), pNode, sizeof(USER_NODE));

	//I3BCLOG(LOCATION, "CUserFinder::UpdateByUid(pNode->_i64UID(%I64d)[key(%d),idxBuffer(%d),_friendCount(%d),idx(%d),Nick(%s)]", pNode->_i64UID, key, pNode->_idxBuffer,pNode->_friendCount, idx, pNode->_nick);
	return TRUE;
}

BOOL CUserFinder::IsAliveUser(INT64 i64UID)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE;

	return TRUE; 
}

BOOL CUserFinder::UpdateAlivePacket(INT64 i64UID, INT32 i32SessionIdx, INT32 i32ServerIdx)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE;

	pNode->_i32serveridx		= i32ServerIdx;
	pNode->_i32sessionidx		= i32SessionIdx;
	pNode->_ui32LastRecvTimeST	= i3ThreadTimer::GetServerTime();

	return TRUE;
}

BOOL CUserFinder::UpdateNickName(INT64 i64UID, TTCHAR * pstrNick)
{
	UINT32 ui32Key;
	USER_NODE * pNode = _GetUserNode(i64UID);

	if(pNode == NULL)return FALSE;

	 //지난 닉네임을 Hash 테이블에서 제거해준다.
	if( pNode->_strnick[0] != 0)
	{
		ui32Key = CHashTable::MakeKeyForString( pNode->_strnick );
		m_pNickTable->Delete( ui32Key, pNode );
	}

	if(0 == pstrNick)
	{
		return FALSE;
	}

	// 새로운 닉네임을 Hash 테이블에서 추가해준다.
	ui32Key = CHashTable::MakeKeyForString( pstrNick );
	m_pNickTable->Insert( ui32Key, pNode );

	// Nick 추가
	i3mem::FillZero(pNode->_strnick,sizeof(USER_NODE));
	i3String::NCopy(pNode->_strnick, pstrNick, NET_NICK_NAME_SIZE-1);

	// TODO :
	// 닉네임이 변경된 유저의 정보를 친구 등록한 모든 유저에게 적용해야 함.
	// 실시간으로 하기에는 DB에 부하가 심하므로
	// 정기 점검때 일괄적으로 진행하기로 함.
	// TChangeInfo Table의 정보를 통해 TCmnFrXX를 Update

#ifdef DEF_TEST_LOG
	// 임시
	if(pNode->_i64UID != i64UID)
	{
		I3BCLOG(LOCATION, "CUserFinder::UpdateNickName(iUID(%I64d),pNick(%s))[pNode->_i64UID(%I64d)]", i64UID, pstrNick, pNode->_i64UID);
	}
	if(0 > pNode->_friendCount || FRIEND_MAX_COUNT < pNode->_friendCount)
	{
		I3BCLOG(LOCATION, "CUserFinder::UpdateNickName(iUID(%I64d),pNick(%s))[pNode->_friendCount(%d)]", i64UID, pstrNick, pNode->_friendCount);		
	}
#endif

	// 접속 중인 모든 친구에게 변경을 통보한다.
	for(UINT32 i = 0; i < pNode->_ui32friendCount; i++)
	{
		USER_NODE * pfriendNode = _GetUserNode(pNode->_afriend[i]._i64fuid);
		if(pfriendNode == NULL)continue;

		// 친구의 친구목록에서 자신의 정보를 찾는다.
		for(UINT32 j = 0; j < pfriendNode->_ui32friendCount; j++)
		{
			if (pfriendNode->_afriend[j]._i64fuid == i64UID)
			{
				i3mem::FillZero(pfriendNode->_afriend[j]._strnick, NET_NICK_NAME_SIZE);
				i3String::NCopy(pfriendNode->_afriend[j]._strnick, pstrNick, NET_NICK_NAME_SIZE-1);
				pfriendNode->_afriend[j]._strnick[NET_NICK_NAME_SIZE - 1] = '\0';
				break;
			}
		}
	}

	return TRUE;
}

BOOL CUserFinder::UpdateForNickName(INT64 i64UID, TTCHAR * pstrNick)
{
	UINT32 uiKey;
	UINT32 uiKey2;
	USER_NODE * pNode = _GetUserNode(i64UID);

	//g_pLog->WriteLog(m_i32LogIdx, "UpdateForNickName : Start : UID(%d),pNick(%s)", iUID, pNick);

	if(pNode == NULL)return FALSE; 	

	//지난 닉네임을 Hash 테이블에서 제거해준다.
	if( pNode->_strnick[0] != 0)
	{
		uiKey = CHashTable::MakeKeyForString( pNode->_strnick );
		m_pNickTable->Delete( uiKey, pNode );
	}

	//if(0 == pNick)
	//{
	//	g_pLog->WriteLog(m_i32LogIdx, "UpdateForNickName : Invalid pNick");
	//	return FALSE;
	//}

	// 새로운 닉네임을 Hash 테이블에서 추가해준다.
	uiKey2 = CHashTable::MakeKeyForString( pstrNick );
	m_pNickTable->Insert( uiKey2, pNode );

	// Nick 추가
	//i3String::Copy(pNode->_nick, pNick);
	i3mem::FillZero(pNode->_strnick, NET_NICK_NAME_SIZE);
	i3String::NCopy(pNode->_strnick, pstrNick, NET_NICK_NAME_SIZE-1);
	
	//g_pLog->WriteLog(m_i32LogIdx,"UpdateForNickName : OLD(%d:%s), NEW(%d:%s)",uiKey, pNode->_nick, uiKey2, pNick);

	return TRUE;
}

BOOL CUserFinder::UpdateFriendListNickName(INT64 i64UID, UINT32 ui32FListIdx, TTCHAR * pstrNick, UINT8 ui8NickColor)
{
	//UINT32 uiKey;
	USER_NODE * pNode = _GetUserNode(i64UID);  

	if(pNode == NULL)return FALSE; 	

	////지난 닉네임을 Hash 테이블에서 제거해준다.
	//if( pNode->_nick[0] != 0)
	//{
	//	uiKey = CHashTable::MakeKeyForString( pNode->_nick );
	//	m_pNickTable->Delete( uiKey, pNode );
	//}

	//if(0 != pNick)
	//{
	//	return FALSE;
	//}

	//// 새로운 닉네임을 Hash 테이블에서 추가해준다.
	//uiKey = CHashTable::MakeKeyForString( pNick );
	//m_pNickTable->Insert( uiKey, pNode );

	// Nick 추가
	//i3String::Copy(pNode->_friend[iFListIdx]._nick, pNick);
	i3mem::FillZero(pNode->_afriend[ui32FListIdx]._strnick, NET_NICK_NAME_SIZE);
	i3String::NCopy(pNode->_afriend[ui32FListIdx]._strnick, pstrNick, NET_NICK_NAME_SIZE-1);

	pNode->_afriend[ui32FListIdx]._ui8nickcolor = ui8NickColor;
	return TRUE;
}

BOOL CUserFinder::UpdateRank(INT64 i64UID, UINT32 ui32Rank)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE; 

	pNode->_ui32Rank = ui32Rank;

	// TODO :
	// 닉네임이 변경된 유저의 정보를 친구 등록한 모든 유저에게 적용해야 함.
	// 실시간으로 하기에는 DB에 부하가 심하므로
	// 정기 점검때 일괄적으로 진행하기로 함.
	// TChangeInfo Table의 정보를 통해 TCmnFrXX를 Update

	return TRUE; 
}

BOOL CUserFinder::UpdateFriendListRank(INT64 i64UID, UINT32 ui32FListIdx, UINT8 ui8Rank)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE;

	pNode->_afriend[ui32FListIdx]._ui8rank = ui8Rank;

	// TODO :
	// 닉네임이 변경된 유저의 정보를 친구 등록한 모든 유저에게 적용해야 함.
	// 실시간으로 하기에는 DB에 부하가 심하므로
	// 정기 점검때 일괄적으로 진행하기로 함.
	// TChangeInfo Table의 정보를 통해 TCmnFrXX를 Update

	return TRUE;
}

BOOL CUserFinder::ChangeState(INT64 i64UID, UINT32 ui32state)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE;

	pNode->_ui32connectState = ui32state;

#ifdef DEF_TEST_LOG
	// 임시
	if(pNode->_iDbIdx != i64UID)
	{
		I3BCLOG(LOCATION, "CUserFinder::ChangeState(i64UID(%I64d),state(%d))[pNode->_i64UID(%I64d)]", i64UID, ui32state,pNode->_i64UID);
	}
	if(0 > pNode->_friendCount || FRIEND_MAX_COUNT < pNode->_friendCount)
	{
		I3BCLOG(LOCATION, "CUserFinder::ChangeState(i64UID(%I64d),state(%d))[pNode->_i64UID(%I64d)][pNode->_friendCount(%d)]", i64UID, ui32state,pNode->_i64UID, pNode->_friendCount);
	}
	if( m_ui32MaxCount <= pNode->_idxBuffer)
	{
		I3BCLOG(LOCATION, "[%d] [ERROR]CUserFinder::ChangeState(i64UID(%I64d),state(%d))[_i64UID(%I64d),_idxBuffer(%d),friendCount(%d),nick(%s)",m_ui32Count,i64UID,ui32state,pNode->_i64UID,pNode->_idxBuffer,pNode->_friendCount, pNode->_nick);
	}
#endif

	// 접속 중인 모든 친구에게 변경을 통보한다.
	for(UINT32 i = 0; i < pNode->_ui32friendCount; i++)
	{
		if (FRIEND_STATE_ONLINE <= GET_FRIEND_STATE(pNode->_afriend[i]._i32state))
		{
			USER_NODE * pfriendNode = _GetUserNode(pNode->_afriend[i]._i64fuid); 
		
			if(pfriendNode != NULL)
			{
				// 친구의 친구목록에서 자신의 정보를 찾는다.
				for(UINT32 j = 0; j < pfriendNode->_ui32friendCount; j++)
				{
					if (pfriendNode->_afriend[j]._i64fuid == i64UID)
					{
						pfriendNode->_afriend[j]._i32state = pNode->_ui32connectState;
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL CUserFinder::GetUserInfoByUID(INT64 i64UID, INT32* pi32SessionIdx,INT32* pi32ServerIdx)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE;

	*pi32SessionIdx = pNode->_i32sessionidx;
	*pi32ServerIdx	 = pNode->_i32serveridx;

	return TRUE; 
}

BOOL CUserFinder::GetUserInfoByNick(TTCHAR * pstrNick, INT64 * pi64OutUID, INT32 * pi32OutSessionIdx,INT32 * pi32OutServerIdx)
{
	if (0 == m_ui32Count)return FALSE;

	UINT32		ui32key	= CHashTable::MakeKeyForString( pstrNick );
	i3BinList * pList	= m_pNickTable->Retrieve( ui32key );

	if (0 == pList->GetCount())return FALSE;

	USER_NODE node;
	i3mem::FillZero(node._strnick, NET_NICK_NAME_SIZE);
	i3String::NCopy(node._strnick, pstrNick, NET_NICK_NAME_SIZE-1);

	INT32 i32idx = pList->FindItem(&node);
	if (0 > i32idx)return FALSE;
	
	USER_NODE * pRecv = (USER_NODE * )pList->GetItem(i32idx);

	if(pRecv == NULL)return FALSE;  //이부분은 잘 몰라서 일단 이렇게 처리 합니다. 필요없는 코드일것 같음

	*pi64OutUID			= pRecv->_i64UID;
	*pi32OutSessionIdx	= pRecv->_i32sessionidx;
	*pi32OutServerIdx	= pRecv->_i32serveridx;

	return TRUE; 
}

BOOL CUserFinder::GetCurrentUserInfoByUID(INT64 i64UID, INT32* pi32SessionIdx,INT32* pi32ServerIdx, TTCHAR * pstrNick)
{
	USER_NODE * pNode = _GetUserNode(i64UID);
	if(pNode == NULL)return FALSE;

	*pi32SessionIdx = pNode->_i32sessionidx;
	*pi32ServerIdx	 = pNode->_i32serveridx;

	INT16 i16length = (INT16) i3String::Length( pNode->_strnick );
	i16length = MIN(i16length + 1, NET_NICK_NAME_SIZE);

	i3mem::Copy(pstrNick, pNode->_strnick, sizeof(TTCHAR) * i16length);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
USER_NODE * CUserFinder::_GetUserNode(INT64 i64UID)
{
	if (0 == m_ui32Count)return NULL;

	UINT32 ui32key = CHashTable::MakeKeyForINT( i64UID );
	i3BinList* pList = m_pUIDTable->Retrieve( ui32key );

	if (0 == pList->GetCount())return NULL;

	USER_NODE Node;

	Node._i64UID = i64UID;
	INT32 i32idx = pList->FindItem(&Node);
	if (0 > i32idx)return NULL;

	return (USER_NODE *)pList->GetItem(i32idx);
}
