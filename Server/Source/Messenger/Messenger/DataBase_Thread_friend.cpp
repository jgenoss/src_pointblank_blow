#include "pch.h"
#include "DataBase.h"
#include "NoteBox.h"
#include "ConnectedUser.h"


BOOL CDataBase::InFriendList(IN_FRIEND_LIST* pIn)
{

	INT32 i32ECode = LOCAL_RINGBUFFER_SUCCESS;
	INT32 i32ThreadIdx = (INT32)(pIn->i64uid % DB_THREAD_COUNT);
	if(i32ThreadIdx < 0)
	{
		g_pLog->WriteLog( L"[ERROR] CDataBase::InFriendList() : iThreadIdx < 0 (UID : %I64d,bReset:%d)", pIn->i64uid, pIn->bReset);
		return FALSE;
	}

	//Push
	m_ppI_FriendList[i32ThreadIdx]->Push( pIn );
	if(LOCAL_RINGBUFFER_SUCCESS != i32ECode)
	{
		g_pLog->WriteLog( L"[ERROR] CDataBase::InFriendList() : m_ppI_FriendList[%d] is Full(%d)", i32ThreadIdx, i32ECode  );
		return FALSE;
	}
	return TRUE;
}

BOOL CDataBase::InFriendControl(IN_FRIEND_CONTROL* pIn)
{
	INT32 i32ThreadIdx = (INT32)(pIn->i64uid % DB_THREAD_COUNT);

	//Push
	if( m_ppI_FriendControl[i32ThreadIdx]->Push( pIn ) != LOCAL_RINGBUFFER_SUCCESS )
	{
		return FALSE;
	}

	return TRUE;
}

INT32 CDataBase::Working_Friend(INT32 i32ThreadIdx)
{	
	INT32	i32Count = 0;
	INT32	i32ret_return = 0;
	INT32	i32ret_value = 0;

	wchar_t	wstrSQL[SQL_STRING_COUNT];
	TTCHAR	strNick[CONVERT_STRING_COUNT];
	TTCHAR	strFriendNick[CONVERT_STRING_COUNT];

	INT32	i32WorkCount = 0;

	INT32	i32TableNumber = 0;
	INT32	i32FriendTableNumber = 0;
	INT32	i32State = 0;

	INT32	i32Loop = 0;

	//==========================================
	//GetFriendList
	if( m_ppI_FriendList[i32ThreadIdx]->GetBufferCount() > 0)
	{		
		DWORD dwOldTime = GetTickCount();

		i32WorkCount++;

		// Variable for Input.
		IN_FRIEND_LIST	* pBufferIn;
		OUT_FRIEND_LIST	* pBufferOut = (OUT_FRIEND_LIST*)m_ppO_FriendList[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{// Output Buffer is full.
			g_pLog->WriteLog( L"FriendList Output Buffer Overflow" );
			goto ExitErrorFriendList;
		}

		// 1. Get data from ring buffer that has Input data.
		pBufferIn = (IN_FRIEND_LIST *)m_ppI_FriendList[i32ThreadIdx]->Pop();
		
		pBufferOut->result			= EVENT_ERROR_SUCCESS;
		pBufferOut->i64uid			= pBufferIn->i64uid;

		// 2. Crate query string & Execute.
		if( pBufferIn->bReset)
		{
			// Reset Friend List			
			S2DB::MakeQueryW( wstrSQL, L"usp_FriendList_del_All",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pBufferIn->i64uid,
				DB_DATA_TYPE_END);
			if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
			{
				//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, wstrSQL  );
				ErrorDBMess( wstrSQL, i32ThreadIdx);
				pBufferOut->result = NET_ERROR_DB_QUERY;
				goto ExitFriendList;
			}

			// 결과 처리 과정
			if( 0 < m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount() )
			{
				i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
				if (1 != i32ret_return)
				{
					// Error
					i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
					g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendList;

				}
			} 
		}

		// Get Friend List
		S2DB::MakeQueryW( wstrSQL, L"usp_FriendList_sel_All", 
					DB_DATA_TYPE_INT,		i32ThreadIdx, 
					DB_DATA_TYPE_BIGINT,	pBufferIn->i64uid, 
					DB_DATA_TYPE_END);
		if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
		{
			ErrorDBMess( wstrSQL, i32ThreadIdx);
			pBufferOut->result = NET_ERROR_DB_QUERY;
			goto ExitFriendList;
		}	
				
		// 3. gather result from DB & Set data to output ring buffer.
		// TODO : Bug 있음
		i32Loop = 0;
		i32Loop = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
		if (0 > i32Loop)	i32Loop = 0;

	#ifdef DEF_TEST_LOG
		// 임시
		if(FRIEND_MAX_COUNT < i32Loop)
		{
			I3BCLOG(LOCATION, "CDataBase::Working_Friend()[UID(%I64d)]", pBufferIn->i64uid);
		}
	#endif
		// 초과값이 나오면 안됨
		pBufferOut->friendCount = MIN(i32Loop, FRIEND_MAX_COUNT);

		if (0 < pBufferOut->friendCount && FRIEND_MAX_COUNT >= pBufferOut->friendCount)
		{
			i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);

			if (1 != i32ret_return)
			{
				// Error
				i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
				g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
				pBufferOut->result = NET_ERROR_DB_QUERY;
				goto ExitFriendList;
			}

			for(UINT32 i = 0; i < pBufferOut->friendCount; i++)
			{
				pBufferOut->friendNode[i]._i64fuid	= m_ppADOdb_MESS[i32ThreadIdx]->GetINT64Data(1);
				pBufferOut->friendNode[i]._i32state	= MAKE_FRIEND_STATE(m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(2), 0, 0, 0);			
				m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(3, pBufferOut->friendNode[i]._strnick, NET_NICK_NAME_SIZE);
				pBufferOut->friendNode[i]._strnick[NET_NICK_NAME_SIZE -1] = 0;
				pBufferOut->friendNode[i]._ui8rank	= (UINT8)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(4);
				pBufferOut->friendNode[i]._ui8nickcolor	= m_ppADOdb_MESS[i32ThreadIdx]->GetUINT8Data(5);
				m_ppADOdb_MESS[i32ThreadIdx]->MoveNext();
			}
		}
		else
		{
			pBufferOut->result = NET_ERROR_DB_QUERY;
			pBufferOut->friendCount = 0;
			goto ExitFriendList;
		}

		// Compare And Update My Rank		
		S2DB::MakeQueryW( wstrSQL, L"usp_FriendList_sel_Rank",
			DB_DATA_TYPE_BIGINT,		pBufferIn->i64uid,
			DB_DATA_TYPE_END);
		if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
		{
			ErrorDBMess( wstrSQL, i32ThreadIdx); 
			pBufferOut->result = NET_ERROR_DB_QUERY;
			goto ExitFriendList;
		}

		// 3. gather result from DB & Set data to output ring buffer.
		// TODO : Bug 있음
		if(0 < m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount())
		{		
			i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
			if (1 != i32ret_return)
			{
				// Error
				i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
				/*g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );*/
				pBufferOut->result = NET_ERROR_DB_QUERY;
				goto ExitFriendList;
			}

			UINT32 ui32RANK = m_ppADOdb_MESS[i32ThreadIdx]->GetUINT32Data(1);
			// 메신저 유저들이 가지고 있는 랭크값(iRank)와 자신의 랭크값(pBufferIn->iRank)을 비교하여 랭크 업데이트를 결정
			pBufferOut->iRank = ui32RANK;
		}
		else
		{
			pBufferOut->result = NET_ERROR_DB_QUERY;
			goto ExitFriendList;
		}

ExitFriendList:

		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// DB TIMEOVER
			g_pLog->WriteLog( L"[Working_Friend() GetFriendList] DB Time Over : %d ", dwNewTime  );
		}

		m_ppO_FriendList[i32ThreadIdx]->PushPointerIdx();
		m_ppI_FriendList[i32ThreadIdx]->PopIdx();
	}

ExitErrorFriendList :

	//==========================================
	// FriendControl ( AUTH, DELETE, ADD )
	if( m_ppI_FriendControl[i32ThreadIdx]->GetBufferCount() > 0)
	{
		DWORD dwOldTime = GetTickCount();

		i32WorkCount++;

		// Variable for Input.
		IN_FRIEND_CONTROL* pBufferIn;
		OUT_FRIEND_CONTROL* pBufferOut = (OUT_FRIEND_CONTROL *)m_ppO_FriendControl[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{
			// Output Buffer is full.
			//ExitErrorFriendControl 
			g_pLog->WriteLog( L"FriendControl Output Buffer Overflow" );
			goto ExitErrorFriendControl;
		}

		// 1. Get data from ring buffer that has Input data.
		pBufferIn = (IN_FRIEND_CONTROL *)m_ppI_FriendControl[i32ThreadIdx]->Pop();

		pBufferOut->result		= EVENT_ERROR_SUCCESS;
		pBufferOut->i64uid		= pBufferIn->i64uid;
		pBufferOut->eWorkType	= pBufferIn->eWorkType;
		pBufferOut->i64fuid		= pBufferIn->i64fuid;
		pBufferOut->idx			= pBufferIn->idx;
		
		// 2. Crate query string & Execute.
		switch( pBufferIn->eWorkType)
		{
		case FRIEND_CONTROL_AUTH:
			{
				INT32 i32tableNumber		= (INT32)(pBufferIn->i64uid % DB_THREAD_COUNT);
				INT32 i32friendTableNumber	= (INT32)(pBufferIn->i64fuid % DB_THREAD_COUNT);
		
				// 친구 승락				
				S2DB::MakeQueryW( wstrSQL, L"usp_FriendList_upd_Accept",
					DB_DATA_TYPE_INT,			i32tableNumber,
					DB_DATA_TYPE_INT,			i32friendTableNumber,
					DB_DATA_TYPE_INT,			FRIEND_STATE_OFFLINE,
					DB_DATA_TYPE_INT,			FRIEND_STATE_OFFLINE,
					DB_DATA_TYPE_BIGINT,		pBufferIn->i64uid,
					DB_DATA_TYPE_BIGINT,		pBufferIn->i64fuid,
					DB_DATA_TYPE_END);
				if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
				{
					ErrorDBMess( wstrSQL, i32ThreadIdx); 
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendControl;
				}

				// 결과 처리 과정
				if( 0 < m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount() )
				{
					i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						// Error
						i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);

						// 확실한 에러
						if (1203 != i32ret_value)
						{
							//i3String::Format(strTemp, "[ERROR][ret_value(%d)][%s]", ret_value, wstrSQL);
							//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, strTemp  );
							pBufferOut->result = NET_ERROR_DB_QUERY;
							goto ExitFriendControl;
						}

							//i3String::Format(strTemp, "[HALF SUCCESS][ret_value(%d)][%s]", ret_value, wstrSQL);
							//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, strTemp  );
					}
				}
				else
				{	// 결과 처리 Error					
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendControl;
				}
			}
			break;

		case FRIEND_CONTROL_DELETE:
			{
				i32TableNumber = (INT32)(pBufferIn->i64uid % DB_THREAD_COUNT);

				// 친구를 삭제한다 				
				S2DB::MakeQueryW( wstrSQL, L"usp_FriendList_del",
					DB_DATA_TYPE_INT,			i32TableNumber,
					DB_DATA_TYPE_BIGINT,		pBufferIn->i64uid,
					DB_DATA_TYPE_BIGINT,		pBufferIn->i64fuid,
					DB_DATA_TYPE_END);
				if( !m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL) )
				{
					ErrorDBMess( wstrSQL, i32ThreadIdx); 
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendControl;
				}

				// 결과 처리 과정
				if( 0 < m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount() )
				{
					i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						// Error
						i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
						g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
						pBufferOut->result = NET_ERROR_DB_QUERY;
						goto ExitFriendControl;

					}
				}
				else
				{
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendControl;
				}
			}
			break;
		case FRIEND_CONTROL_ADD:
			{
				i32TableNumber	= (INT32)(pBufferOut->i64uid % DB_THREAD_COUNT);
				i32FriendTableNumber = 0;
				i32State = 0;

				//_ConvertString(strNick, pBufferIn->strNick);
				S2DB::ConvertString(strNick, CONVERT_STRING_COUNT, pBufferIn->strNick);

				// 친구 추가
				// 나와 친구의 필요 정보 가져오기
				S2DB::MakeQueryW(wstrSQL, L"usp_Basic_sel_Friend", 
					DB_DATA_TYPE_TTCHAR,	strNick, 
					DB_DATA_TYPE_BIGINT,	pBufferIn->i64uid, 
					DB_DATA_TYPE_INT,		i32TableNumber,
					DB_DATA_TYPE_END);
				if( !m_ppADOdb_USER[i32ThreadIdx]->ExecuteSP(wstrSQL) )
				{
					ErrorDBUser( wstrSQL, i32ThreadIdx); 
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendControl;
				}

				// 결과 처리 과정
				i32Count = m_ppADOdb_USER[i32ThreadIdx]->GetRecordCount(); 
				if( 0 < i32Count )
				{
					i32ret_return = m_ppADOdb_USER[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						// Error
						i32ret_value = m_ppADOdb_USER[i32ThreadIdx]->GetINT32Data(1);
				
						//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "[ERROR][ret_value(%d)][%s]", ret_value, wstrSQL  );

						if (1502 == i32ret_value)
						{
							pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INSERT_FULL;
						}
						else
						{
							pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER;
						}
						
						goto ExitFriendControl;
					}
				}
				else
				{
					// 결과 처리 Error
					g_pLog->WriteLog( L"[ERROR][Empty RecordSet][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );						
					pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER;
					goto ExitFriendControl;
				}

				// 결과 저장 : 친구의 데이터가 있어야 반드시 결과 저장이 된다.
				// 친구의 친구 리스트
				pBufferOut->friendNode._i64fuid = pBufferOut->i64uid;
				//pBufferOut->friendNode._state = MAKE_FRIEND_STATE(FRIEND_STATE_HOLD, 0, 0, 0);				
				m_ppADOdb_USER[i32ThreadIdx]->GetTextData(1, pBufferOut->friendNode._strnick, NET_NICK_NAME_SIZE);
				pBufferOut->friendNode._strnick[NET_NICK_NAME_SIZE -1] = 0;
				//pBufferOut->friendNode._ui8rank = (UINT8)m_ppADOdb_USER[i32ThreadIdx]->GetINT32Data(2);
				//위장계급 적용을 위해 Game서버에서 받아온 계급을 사용합니다.
				pBufferOut->friendNode._ui8rank = pBufferIn->ui8Rank;
				pBufferOut->friendNode._ui8nickcolor = pBufferIn->ui8NickColor; //m_ppADOdb_USER[i32ThreadIdx]->GetUINT8Data(3);


				// 나의 친구 리스트
				pBufferOut->i64fuid = m_ppADOdb_USER[i32ThreadIdx]->GetINT64Data(4);
				i32FriendTableNumber = (INT32)(pBufferOut->i64fuid % DB_THREAD_COUNT);
				pBufferOut->targetFriendNode._i64fuid = pBufferOut->i64fuid;
				//pBufferOut->targetFriendNode._state = MAKE_FRIEND_STATE(FRIEND_STATE_HOLD, 0, 0, 0);				
				m_ppADOdb_USER[i32ThreadIdx]->GetTextData(5, pBufferOut->targetFriendNode._strnick, NET_NICK_NAME_SIZE);
				pBufferOut->targetFriendNode._strnick[NET_NICK_NAME_SIZE -1] = 0;
				//pBufferOut->targetFriendNode._ui8rank = (UINT8)m_ppADOdb_USER[i32ThreadIdx]->GetINT32Data(6);
				//위장계급 적용을 위해 Game서버에서 받아온 계급을 사용합니다.
				pBufferOut->targetFriendNode._ui8rank = pBufferIn->ui8fRank;
				pBufferOut->targetFriendNode._ui8nickcolor = pBufferIn->ui8fNickColor; // m_ppADOdb_USER[i32ThreadIdx]->GetUINT8Data(7);


				/*_ConvertString(strNick, pBufferOut->friendNode._nick); 
				_ConvertString(strFriendNick, pBufferOut->targetFriendNode._nick); */
				S2DB::ConvertString(strNick, CONVERT_STRING_COUNT, pBufferOut->friendNode._strnick); 
				S2DB::ConvertString(strFriendNick, CONVERT_STRING_COUNT, pBufferOut->targetFriendNode._strnick); 

#if defined( USE_BLOCK_LIST )
				// 추가하려는 유저가 자신을 차단했는지 체크한다.
				BOOL	bCheckBlock = FALSE;
				INT32 i32Result = _NoteCheckBlock(i32ThreadIdx, pBufferOut->i64uid, pBufferOut->i64fuid, &bCheckBlock );											
				if (EVENT_ERROR_SUCCESS != i32Result)
				{
					// 결과 처리 Error
					pBufferOut->result = EVENT_ERROR_EVENT_BLOCK_QUERY;
					goto ExitFriendControl;
				}
				if( TRUE == bCheckBlock ) // 차단 당했다면
				{
					pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INSERT_BLOCK_USER;
					goto ExitFriendControl;
				} 
#endif

				// 나와 친구의 친구 리스트에 친구 추가
				S2DB::MakeQueryW(wstrSQL, L"usp_FriendList_mer",
					DB_DATA_TYPE_INT,		i32TableNumber,
					DB_DATA_TYPE_INT,		i32FriendTableNumber,
					DB_DATA_TYPE_BIGINT,	pBufferOut->i64uid,
					DB_DATA_TYPE_BIGINT,	pBufferOut->i64fuid,
					DB_DATA_TYPE_INT,		FRIEND_STATE_REQUEST,
					DB_DATA_TYPE_INT,		FRIEND_STATE_HOLD,
					DB_DATA_TYPE_TTCHAR,	strNick,
					DB_DATA_TYPE_TTCHAR,	strFriendNick,
					DB_DATA_TYPE_INT,		pBufferOut->friendNode._ui8rank,
					DB_DATA_TYPE_INT,		pBufferOut->targetFriendNode._ui8rank,
					DB_DATA_TYPE_TINYINT,	pBufferOut->friendNode._ui8nickcolor,
					DB_DATA_TYPE_TINYINT,	pBufferOut->targetFriendNode._ui8nickcolor,
					DB_DATA_TYPE_END		);

				if( !m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL) )
				{
					ErrorDBMess( wstrSQL, i32ThreadIdx); 
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitFriendControl;
				}

				// 결과 처리 과정
				i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
				if(0 < i32Count)
				{
					i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
						if (1502 == i32ret_value)
						{
							pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INSERT_FULL;
						}
						else
						{
							pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND;
						}

						// Error
						//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "[ERROR][ret_value(%d)][%s]", ret_value, wstrSQL  );
						goto ExitFriendControl;

					}
				}
				else
				{
					// 결과 처리 Error
					g_pLog->WriteLog( L"[ERROR][Empty RecordSet][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
					pBufferOut->result = EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND;
					goto ExitFriendControl;
				}

				// 결과 저장
				// 나의 친구 리스트
				pBufferOut->targetFriendNode._i64fuid	= m_ppADOdb_MESS[i32ThreadIdx]->GetINT64Data(1);
				pBufferOut->targetFriendNode._i32state	= MAKE_FRIEND_STATE((UINT32)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(2), 0, 0, 0);				
				m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(3, pBufferOut->targetFriendNode._strnick, NET_NICK_NAME_SIZE);
				pBufferOut->targetFriendNode._strnick[NET_NICK_NAME_SIZE -1] = 0;
				pBufferOut->targetFriendNode._ui8rank	= (UINT8)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(4);
				pBufferOut->targetFriendNode._ui8nickcolor	= m_ppADOdb_MESS[i32ThreadIdx]->GetUINT8Data(5);

				// 친구의 친구 리스트				
				pBufferOut->friendNode._i64fuid		= m_ppADOdb_MESS[i32ThreadIdx]->GetINT64Data(6);
				pBufferOut->friendNode._i32state	= MAKE_FRIEND_STATE((UINT32)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(7), 0, 0, 0); 
				m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(8, pBufferOut->friendNode._strnick, NET_NICK_NAME_SIZE);
				pBufferOut->friendNode._strnick[NET_NICK_NAME_SIZE -1] = 0;
				pBufferOut->friendNode._ui8rank		= (UINT8)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(9);
				pBufferOut->friendNode._ui8nickcolor= m_ppADOdb_MESS[i32ThreadIdx]->GetUINT8Data(10);
		
			}
			break;
		}
ExitFriendControl:
		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// DB TIMEOVER
			g_pLog->WriteLog( L"[Working_Friend() FriendControl] DB Time Over : %d ", dwNewTime );
		}

		m_ppO_FriendControl[i32ThreadIdx]->PushPointerIdx();
		m_ppI_FriendControl[i32ThreadIdx]->PopIdx();
	}

ExitErrorFriendControl :

	return i32WorkCount;
}

void CDataBase::GetOutFriendListIdx(void)
{
	m_ppO_FriendList[m_i32OutFriendListThreadIdx]->PopIdx();
}	

void CDataBase::GetOutFriendControlIdx(void)
{
	m_ppO_FriendControl[m_i32OutFriendControlThreadIdx]->PopIdx();
}

OUT_FRIEND_LIST * CDataBase::GetOutFriendListData(void)
{
	OUT_FRIEND_LIST * pOut = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32OutFriendListThreadIdx++; 
		if(m_i32OutFriendListThreadIdx >= DB_THREAD_COUNT) m_i32OutFriendListThreadIdx = 0;

		if(m_ppO_FriendList[m_i32OutFriendListThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (OUT_FRIEND_LIST *)m_ppO_FriendList[m_i32OutFriendListThreadIdx]->Pop(); 
			m_ppO_FriendList[m_i32OutFriendListThreadIdx]->PopIdx();
			break;
		}
	}

	return pOut;
}

OUT_FRIEND_CONTROL * CDataBase::GetOutFriendControlData(void)
{
	OUT_FRIEND_CONTROL * pOut = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32OutFriendControlThreadIdx++; 
		if(m_i32OutFriendControlThreadIdx >= DB_THREAD_COUNT) m_i32OutFriendControlThreadIdx = 0;

		if(m_ppO_FriendControl[m_i32OutFriendControlThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (OUT_FRIEND_CONTROL *)m_ppO_FriendControl[m_i32OutFriendControlThreadIdx]->Pop();
			m_ppO_FriendControl[m_i32OutFriendControlThreadIdx]->PopIdx();
			break;
		}
	}

	return pOut;
}



//////////////////////////////////////////////////////////////////////////
// Change Nick
#ifdef USE_CHANGE_NICK	
BOOL CDataBase::InChageNick(IN_CHANGE_NICK* pIn)
{
	INT32 i32ThreadIdx = (INT32)(pIn->i64fuid % DB_THREAD_COUNT);

	//Push
	if( m_ppI_ChangeNick[i32ThreadIdx]->Push( pIn ) != LOCAL_RINGBUFFER_SUCCESS )
	{
		return FALSE;
	}
	return TRUE;
}

INT32 CDataBase::WorkingChangeNick(INT32 i32ThreadIdx)
{

	INT32	i32ret_return = 0;
	INT32	i32ret_value = 0;

	wchar_t	wstrSQL[SQL_STRING_COUNT];
	TTCHAR	strNick[CONVERT_STRING_COUNT];

	INT32	i32WorkCount = 0;	

	//==========================================
	// Change Nick to Friend List
	if( m_ppI_ChangeNick[i32ThreadIdx]->GetBufferCount() > 0)
	{
		DWORD dwOldTime = GetTickCount();

		i32WorkCount++;

		// Variable for Input.
		IN_CHANGE_NICK* pBufferIn;
		OUT_CHANGE_NICK* pBufferOut = (OUT_CHANGE_NICK *)m_ppO_ChangeNick[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{
			// Output Buffer is full.
			//ExitErrorFriendControl 
			g_pLog->WriteLog( L"WorkingChangeNick() Output Buffer Overflow" );
			goto ExitErrorChangeNick;
		}

		// 1. Get data from ring buffer that has Input data.
		pBufferIn = (IN_CHANGE_NICK *)m_ppI_ChangeNick[i32ThreadIdx]->Pop();
		if (NULL == pBufferIn)
		{
			g_pLog->WriteLog( L"WorkingChangeNick() Input Buffer Overflow" );
			goto ExitErrorChangeNick;
		}

		//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "Sender(%d), Receiver(%d), Idx(%d), Nick(%s) ", pBufferIn->uid, pBufferIn->fuid, pBufferIn->idx, pBufferIn->strNick); 

		pBufferOut->result		= EVENT_ERROR_SUCCESS;
		pBufferOut->i64uid		= pBufferIn->i64uid;
		pBufferOut->eWorktype	= pBufferIn->eWorktype;
		pBufferOut->i64fuid		= pBufferIn->i64fuid;
		pBufferOut->idx			= pBufferIn->idx;
		
		UINT8 ui8NickSendSize = (UINT8)(i3String::Length( pBufferIn->strNick ) + 1);
		i3mem::Copy(&pBufferOut->strNick, pBufferIn->strNick, sizeof (TTCHAR) * ui8NickSendSize);
		pBufferOut->strNick[ui8NickSendSize - 1] = '\0';

		pBufferOut->ui8NickColor = pBufferIn->ui8NickColor;

		INT32 friendTableNumber = (INT32)(pBufferIn->i64fuid % DB_THREAD_COUNT);

		// 2. Crate query string & Execute.
		switch( pBufferIn->eWorktype)
		{
		case FRIEND_CHANGE_NICK:
			{
				//_ConvertString(strNick, pBufferIn->strNick);
				S2DB::ConvertString(strNick, CONVERT_STRING_COUNT, pBufferIn->strNick);

				// 자신의 변경된 닉네임을 친구의 목록에 업데이트한다. 
				S2DB::MakeQueryW(wstrSQL, L"usp_FriendList_upd_Nick",
					DB_DATA_TYPE_INT,		friendTableNumber,
					DB_DATA_TYPE_BIGINT,	pBufferIn->i64uid,
					DB_DATA_TYPE_BIGINT,	pBufferIn->i64fuid,
					DB_DATA_TYPE_TTCHAR,	strNick,
					DB_DATA_TYPE_END	);				
			}
			break;
		case FRIEND_CHANGE_NICKCOLOR:
			{
				// 자신의 변경된 닉네임 컬러를 친구의 목록에 업데이트한다. 
				S2DB::MakeQueryW(wstrSQL, L"usp_FriendList_upd_NickColor",
					DB_DATA_TYPE_INT,		friendTableNumber,
					DB_DATA_TYPE_BIGINT,	pBufferIn->i64uid,
					DB_DATA_TYPE_BIGINT,	pBufferIn->i64fuid,
					DB_DATA_TYPE_TINYINT,	pBufferIn->ui8NickColor,
					DB_DATA_TYPE_END	);				
			}
			break;
		default:
			{
				g_pLog->WriteLog( L"WorkingChangeNick() Invalid Type"  );

				pBufferOut->result = EVENT_ERROR_FAIL;
				goto ExitChangeNick;
			}
			break;
		}

		if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
		{
			g_pLog->WriteLog( L"[ERROR][Execute Fail][%s]", wstrSQL  );
			ErrorDBMess( wstrSQL, i32ThreadIdx); 
			pBufferOut->result = NET_ERROR_DB_QUERY;
			goto ExitChangeNick;
		}

		// 결과 처리 과정
		if( 0 < m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount() )
		{
			i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
			if (1 != i32ret_return) 
			{
				// Error
				i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
				g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
				pBufferOut->result = NET_ERROR_DB_QUERY;
				goto ExitChangeNick;
			}
		}
		else
		{	 
			pBufferOut->result = NET_ERROR_DB_QUERY;
			goto ExitChangeNick;
		}

ExitChangeNick:
		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// DB TIMEOVER
			g_pLog->WriteLog( L"[WorkingChangeNick()] DB Time Over : %d ", dwNewTime );
		}

		m_ppO_ChangeNick[i32ThreadIdx]->PushPointerIdx();
		m_ppI_ChangeNick[i32ThreadIdx]->PopIdx();
	}
ExitErrorChangeNick:

	return i32WorkCount;
}

void CDataBase::GetOutChangeNickIdx(void)
{
	m_ppO_ChangeNick[m_i32OutChangeNickThreadIdx]->PopIdx();
}

OUT_CHANGE_NICK*	CDataBase::GetOutChangeNickData(void)
{
	OUT_CHANGE_NICK * pOut = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32OutChangeNickThreadIdx++; 
		if(m_i32OutChangeNickThreadIdx >= DB_THREAD_COUNT) m_i32OutChangeNickThreadIdx = 0;

		if(m_ppO_ChangeNick[m_i32OutChangeNickThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (OUT_CHANGE_NICK *)m_ppO_ChangeNick[m_i32OutChangeNickThreadIdx]->Pop();
			m_ppO_ChangeNick[m_i32OutChangeNickThreadIdx]->PopIdx();
			break;
		}
	}

	return pOut;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Change Rank
#ifdef USE_CHANGE_RANK	
BOOL CDataBase::InChangeRank(IN_CHANGE_RANK* pIn)
{
	INT32 i32ThreadIdx = (INT32)(pIn->i64fuid % DB_THREAD_COUNT);

	//Push
	if( m_ppI_ChangeRank[i32ThreadIdx]->Push( pIn ) != LOCAL_RINGBUFFER_SUCCESS )
	{
		return FALSE;
	}
	return TRUE;
}

INT32 CDataBase::WorkingChangeRank(INT32 i32ThreadIdx)
{

	INT32	i32ret_return = 0;
	INT32	i32ret_value = 0;

	wchar_t	wstrSQL[SQL_STRING_COUNT];

	INT32	i32WorkCount = 0;

	//==========================================
	// Change Rank to Friend List
	if( m_ppI_ChangeRank[i32ThreadIdx]->GetBufferCount() > 0)
	{
		DWORD dwOldTime = GetTickCount();

		i32WorkCount++;

		// Variable for Input.
		IN_CHANGE_RANK* pBufferIn;
		OUT_CHANGE_RANK* pBufferOut = (OUT_CHANGE_RANK *)m_ppO_ChangeRank[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{
			// Output Buffer is full.
			//ExitErrorFriendControl 
			g_pLog->WriteLog( L"WorkingChangeRank() Output Buffer Overflow" );
			goto ExitErrorChangeRank;
		}

		// 1. Get data from ring buffer that has Input data.
		pBufferIn = (IN_CHANGE_RANK *)m_ppI_ChangeRank[i32ThreadIdx]->Pop();
		if (NULL == pBufferIn)
		{
			g_pLog->WriteLog( L"WorkingChangeRank() Input Buffer Overflow" );
			goto ExitErrorChangeRank;
		}

		//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "Sender(%d), Receiver(%d), Idx(%d), Rank(%d) ", pBufferIn->uid, pBufferIn->fuid, pBufferIn->idx, pBufferIn->rank); 

		pBufferOut->result = EVENT_ERROR_SUCCESS;
		pBufferOut->i64uid = pBufferIn->i64uid;
		pBufferOut->type = pBufferIn->type;
		pBufferOut->i64fuid = pBufferIn->i64fuid;
		pBufferOut->idx = pBufferIn->idx;
		pBufferOut->rank = pBufferIn->rank;
		
		// 2. Crate query string & Execute.
		switch( pBufferIn->type)
		{
		case FRIEND_CHANGE_RANK:
			{
				INT32 friendTableNumber = (INT32)(pBufferIn->i64fuid % DB_THREAD_COUNT);
				
				// 자신의 변경된 닉네임을 친구의 목록에 업데이트한다.				
				S2DB::MakeQueryW( wstrSQL, L"usp_FriendList_upd_Rank",
					DB_DATA_TYPE_INT,			friendTableNumber,
					DB_DATA_TYPE_BIGINT,		pBufferIn->i64uid,
					DB_DATA_TYPE_BIGINT,		pBufferIn->i64fuid,
					DB_DATA_TYPE_TINYINT,		pBufferIn->rank,
					DB_DATA_TYPE_END);
				if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
				{
					ErrorDBMess( wstrSQL, i32ThreadIdx); 
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitChangeRank;
				}
			
				// 결과 처리 과정
				if( 0 < m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount() )
				{
					i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						// Error
						i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
						g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
						pBufferOut->result = NET_ERROR_DB_QUERY;
						goto ExitChangeRank;
					}
				}
				else
				{	// 결과 처리 Error		
					//g_pLog->WriteLog(LOG_DB_RANK_IDX, "[ERROR][Empty Recordset][%s]", wstrSQL);
					//g_pLog->WriteLog(LOG_DB_DATABASE_IDX, "[ERROR][Empty Recordset][%s]", wstrSQL);
					pBufferOut->result = NET_ERROR_DB_QUERY;
					goto ExitChangeRank;
				} 
			}
			break;
		default:
			{
				g_pLog->WriteLog( L"WorkingChangeRank() Invalid Type" );
			}
			break;
		}
	ExitChangeRank:
		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// DB TIMEOVER
			g_pLog->WriteLog( L"[WorkingChangeRank()] DB Time Over : %d ", dwNewTime );
		}

		m_ppO_ChangeRank[i32ThreadIdx]->PushPointerIdx();
		m_ppI_ChangeRank[i32ThreadIdx]->PopIdx();
	}
ExitErrorChangeRank:

	return i32WorkCount;
}

void CDataBase::GetOutChangeRankIdx(void)
{
	m_ppO_ChangeRank[m_i32OutChangeRankThreadIdx]->PopIdx();
}

OUT_CHANGE_RANK*	CDataBase::GetOutChangeRankData(void)
{
	OUT_CHANGE_RANK * pOut = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32OutChangeRankThreadIdx++; 
		if(m_i32OutChangeRankThreadIdx >= DB_THREAD_COUNT) m_i32OutChangeRankThreadIdx = 0; 

		if(m_ppO_ChangeRank[m_i32OutChangeRankThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (OUT_CHANGE_RANK *)m_ppO_ChangeRank[m_i32OutChangeRankThreadIdx]->Pop(); 
			m_ppO_ChangeRank[m_i32OutChangeRankThreadIdx]->PopIdx();
			break;
		}
	}

	return pOut;
}
#endif
//////////////////////////////////////////////////////////////////////////