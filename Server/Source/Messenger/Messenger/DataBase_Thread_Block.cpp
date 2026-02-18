#include "pch.h"
#include "DataBase.h"

#if defined( USE_BLOCK_LIST )
BOOL CDataBase::InBlockList(IN_BLOCK_LIST* pIn)
{
	INT32 i32ECode = LOCAL_RINGBUFFER_SUCCESS;
	INT32 i32ThreadIdx = (INT32)(pIn->_i64uid % DB_THREAD_COUNT);
	if(i32ThreadIdx < 0)
	{
		g_pLog->WriteLog( L"[ERROR] CDataBase::InBlockList() : iThreadIdx < 0 (UID : %I64d )", pIn->_i64uid  );
		return FALSE;
	}

	i32ECode = m_ppI_BlockList[i32ThreadIdx]->Push( pIn );
	if(LOCAL_RINGBUFFER_SUCCESS != i32ECode)
	{

		g_pLog->WriteLog( L"[ERROR] CDataBase::InBlockList() : m_ppI_BlockList[%d] is Full(%d)", i32ThreadIdx, i32ECode  );
		return FALSE; 
	}

	return TRUE;
}

BOOL CDataBase::InBlockControl(IN_BLOCK_CONTROL* pIn)
{
	INT32 i32ThreadIdx = (INT32)(pIn->_i64uid % DB_THREAD_COUNT);

	//Push
	if( m_ppI_BlockControl[i32ThreadIdx]->Push( pIn ) != LOCAL_RINGBUFFER_SUCCESS )
	{
		return FALSE;
	}

	return TRUE;
}

INT32 CDataBase::Working_Block(INT32 i32ThreadIdx)
{	
	INT32	i32Count = 0;
	INT32	i32ret_return = 0;
	INT32	i32ret_value = 0;
	wchar_t	wstrSQL[SQL_STRING_COUNT];

	INT32	i32WorkCount = 0;
	INT32	i32Loop = 0;

	//==========================================
	//GetBlockList
	if( m_ppI_BlockList[i32ThreadIdx]->GetBufferCount() > 0)
	{		
		DWORD dwOldTime = GetTickCount();

		i32WorkCount++;

		// Variable for Input.
		IN_BLOCK_LIST	* pBufferIn;
		OUT_BLOCK_LIST	* pBufferOut = (OUT_BLOCK_LIST*)m_ppO_BlockList[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{// Output Buffer is full.
			g_pLog->WriteLog( L"BlockList Output Buffer Overflow" );
			goto ExitErrorBlockList;
		}

		// 1. Get data from ring buffer that has Input data.
		pBufferIn = (IN_BLOCK_LIST *)m_ppI_BlockList[i32ThreadIdx]->Pop();
		
		pBufferOut->_i32result			= EVENT_ERROR_SUCCESS;
		pBufferOut->_i64uid				= pBufferIn->_i64uid;
 
		S2DB::MakeQueryW( wstrSQL, L"usp_BlockList_sel_All",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pBufferIn->_i64uid,
			DB_DATA_TYPE_END);
	 
		if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
		{
			ErrorDBMess( wstrSQL, i32ThreadIdx);
			pBufferOut->_i32result = NET_ERROR_DB_QUERY;
			goto ExitBlockList;
		}	

		i32Loop = 0;
		i32Loop = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
		if (0 > i32Loop)	i32Loop = 0;

		// 초과값이 나오면 안됨
		pBufferOut->_ui32blockCount = MIN(i32Loop, BLOCK_MAX_COUNT);

		if (0 < pBufferOut->_ui32blockCount && BLOCK_MAX_COUNT >= pBufferOut->_ui32blockCount)
		{
			i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);

			if (1 != i32ret_return)
			{
				// Error
				i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
				g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
				pBufferOut->_i32result = NET_ERROR_DB_QUERY;
				goto ExitBlockList;
			}

			for(UINT32 i = 0; i < pBufferOut->_ui32blockCount; i++)
			{
				// out : [idx], [buid],[nick],[rank],[comment]
				pBufferOut->_pBlockNode[i]._i32Idx	= m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
				pBufferOut->_pBlockNode[i]._i64buid	= m_ppADOdb_MESS[i32ThreadIdx]->GetINT64Data(2);
				m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(3, pBufferOut->_pBlockNode[i]._strNick, NET_NICK_NAME_SIZE);
				pBufferOut->_pBlockNode[i]._strNick[NET_NICK_NAME_SIZE -1] = 0;
				pBufferOut->_pBlockNode[i]._ui8rank		= m_ppADOdb_MESS[i32ThreadIdx]->GetUINT8Data(4);
				m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(5, pBufferOut->_pBlockComment[i]._strComment, BLOCK_COMMENT_MAX - 1);
				pBufferOut->_pBlockComment[i]._strComment[BLOCK_COMMENT_MAX -1] = 0;

				m_ppADOdb_MESS[i32ThreadIdx]->MoveNext();
			}
		}
		else
		{
			pBufferOut->_i32result = NET_ERROR_DB_QUERY;
			pBufferOut->_ui32blockCount = 0;
			goto ExitBlockList;
		}

ExitBlockList:

		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// DB TIMEOVER
			g_pLog->WriteLog( L"[Working_Block() GetBlockList] DB Time Over : %d ", dwNewTime );
		}

		m_ppO_BlockList[i32ThreadIdx]->PushPointerIdx();
		m_ppI_BlockList[i32ThreadIdx]->PopIdx();
	}

ExitErrorBlockList :

	//==========================================
	// BlockControl ( DELETE, ADD )
	if( m_ppI_BlockControl[i32ThreadIdx]->GetBufferCount() > 0)
	{
		DWORD dwOldTime = GetTickCount();

		i32WorkCount++;

		// Variable for Input.
		IN_BLOCK_CONTROL* pBufferIn;
		OUT_BLOCK_CONTROL* pBufferOut = (OUT_BLOCK_CONTROL *)m_ppO_BlockControl[i32ThreadIdx]->PushPointer();
		if( pBufferOut == NULL)
		{
			// Output Buffer is full.
			//ExitErrorFriendControl 
			g_pLog->WriteLog( L"BlockControl Output Buffer Overflow" );
			goto ExitErrorBlockControl;
		}
		
		// 1. Get data from ring buffer that has Input data.
		pBufferIn = (IN_BLOCK_CONTROL *)m_ppI_BlockControl[i32ThreadIdx]->Pop();

		pBufferOut->_i32result	= EVENT_ERROR_SUCCESS;
		pBufferOut->_i32Idx		= pBufferIn->_i32Idx;
		pBufferOut->_i64uid		= pBufferIn->_i64uid;
		pBufferOut->_i32type	= pBufferIn->_i32type;
		pBufferOut->_i64buid	= pBufferIn->_i64buid;
		// pBufferOut->idx		= pBufferIn->idx;

		// 2. Crate query string & Execute.
		switch( pBufferIn->_i32type)
		{
			case BLOCK_CONTROL_ADD:	// 차단 유저 추가
			{
			//	INT32	i32TableNumber	= (INT32)(pBufferOut->_i64uid % DB_THREAD_COUNT);

			//	//_ConvertString(strNick, pBufferIn->_strNick); 	// 닉네임에 ' 있는지 체크( 있으면 제거 )			
			//	//_ConvertString(strBlockComment, pBufferIn->_strComment ); // 차단사유에 ' 있는지 체크( 있으면 제거 )
			//	S2DB::ConvertString(strNick, CONVERT_STRING_COUNT, pBufferIn->_strNick);				// 닉네임에 ' 있는지 체크( 있으면 제거 )			
			//	S2DB::ConvertString(strBlockComment, CONVERT_STRING_COUNT, pBufferIn->_strComment);		// 차단사유에 ' 있는지 체크( 있으면 제거 )
 
			//	pBufferOut->_pBlockNode._ui8rank = pBufferIn->_ui8Rank;

			//	// 나와 차단 리스트에 차단 유저 추가				
			//	S2DB::MakeQueryW( wstrSQL, "usp_BlockList_ins",
			//		DB_DATA_TYPE_INT,			i32TableNumber,
			//		DB_DATA_TYPE_BIGINT,		pBufferOut->_i64uid,
			//		DB_DATA_TYPE_BIGINT,		pBufferOut->_i64buid,
			//		DB_DATA_TYPE_CHAR,			strNick,
			//		DB_DATA_TYPE_TINYINT,		pBufferOut->_pBlockNode._ui8rank,
			//		DB_DATA_TYPE_CHAR,			strBlockComment,
			//		DB_DATA_TYPE_END);					// 차단 사유

			//	if( !m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL) )
			//	{
			//		ErrorDBMess( wstrSQL, i32ThreadIdx);
			//		pBufferOut->_i32result = NET_ERROR_DB_QUERY;
			//		goto ExitBlockControl;
			//	}

			//	// 결과 처리 과정
			//	i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
			//	if(0 < i32Count)
			//	{
			//		i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
			//		if (1 != i32ret_return)
			//		{
			//			i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
			//			if ( SP_RV_USP_TCMNBK_BLOCK_ADD_ERROR_INSERT_FAIL == i32ret_value)
			//			{
			//				pBufferOut->_i32result = EVENT_ERROR_EVENT_BLOCK_QUERY;	// DB INSERT FAIL
			//				goto ExitBlockControl;
			//			}
			//		}
			//	}
			//	else
			//	{
			//		// 결과 처리 Error
			//		g_pLog->WriteLog(LOG_DB_IDX,  "[ERROR][Empty RecordSet Block Add][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
			//		pBufferOut->_i32result = EVENT_ERROR_EVENT_BLOCK_QUERY;
			//		goto ExitBlockControl;
			//	}

			//	// out : idx, ret_return, buid, bnick, brank, comment
			//	// 결과 저장
			//	// 나의 차단 유저 리스트
			//			
			//	pBufferOut->_pBlockNode._i32Idx = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
			//	pBufferOut->_pBlockNode._i64buid = m_ppADOdb_MESS[i32ThreadIdx]->GetINT64Data(2);
			//
			//	m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(3, pBufferOut->_pBlockNode._strNick, NET_NICK_NAME_SIZE);
			//	pBufferOut->_pBlockNode._strNick[NET_NICK_NAME_SIZE -1] = 0;

			//	pBufferOut->_pBlockNode._ui8rank = (UINT8)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(4);

			//	m_ppADOdb_MESS[i32ThreadIdx]->GetTextData(5, pBufferOut->_strComment, BLOCK_COMMENT_MAX-1);
			//	pBufferOut->_strComment[BLOCK_COMMENT_MAX -1] = 0;
			}
			break;

		case BLOCK_CONTROL_DELETE:
			{
				INT32 i32TableNumber = (INT32)(pBufferIn->_i64uid % DB_THREAD_COUNT);

				// 차단 유저를 삭제한다				
				S2DB::MakeQueryW( wstrSQL, L"usp_BlockList_del",
					DB_DATA_TYPE_INT,			i32TableNumber,
					DB_DATA_TYPE_BIGINT,		pBufferIn->_i64buid,
					DB_DATA_TYPE_INT,			pBufferIn->_i32Idx,
					DB_DATA_TYPE_END);
			
				if( !m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL) )
				{
					ErrorDBMess( wstrSQL, i32ThreadIdx);
					pBufferOut->_i32result = NET_ERROR_DB_QUERY;
					goto ExitBlockControl;
				}

				// 결과 처리 과정
				i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
				if(0 < i32Count)
				{
					i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
						if ( SP_RV_USP_TCMNBK_BLOCK_DELETE_ERROR_DELETE_FAIL == i32ret_value)
						{
							pBufferOut->_i32result = EVENT_ERROR_EVENT_BLOCK_QUERY;	// DB INSERT FAIL
							goto ExitBlockControl;
						}
					}
				}
				else
				{
					// 결과 처리 Error
					g_pLog->WriteLog( L"[ERROR][Empty RecordSet Block Delete][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
					pBufferOut->_i32result = EVENT_ERROR_EVENT_BLOCK_QUERY;
					goto ExitBlockControl;
				}

				// 삭제 성공시 DB Index 전달.
				pBufferOut->_pBlockNode._i32Idx = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
			}
			break;



		}
ExitBlockControl:
		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_DB_LIMIT_TIME )
		{	// DB TIMEOVER
			g_pLog->WriteLog( L"[Working_Block() BlockControl] DB Time Over : %d ", dwNewTime  );
		}

		m_ppO_BlockControl[i32ThreadIdx]->PushPointerIdx();
		m_ppI_BlockControl[i32ThreadIdx]->PopIdx();
	}

ExitErrorBlockControl :

	return i32WorkCount;
}

void CDataBase::GetOutBlockListIdx(void)
{
	m_ppO_BlockList[m_i32OutBlockListThreadIdx]->PopIdx();
}	

void CDataBase::GetOutBlockControlIdx(void)
{
	m_ppO_BlockControl[m_i32OutBlockControlThreadIdx]->PopIdx();
}

OUT_BLOCK_LIST * CDataBase::GetOutBlockListData(void)
{
	OUT_BLOCK_LIST * pOut = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32OutBlockListThreadIdx++;
		if(m_i32OutBlockListThreadIdx >= DB_THREAD_COUNT) m_i32OutBlockListThreadIdx = 0;

		if(m_ppO_BlockList[m_i32OutBlockListThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (OUT_BLOCK_LIST *)m_ppO_BlockList[m_i32OutBlockListThreadIdx]->Pop();
			m_ppO_BlockList[m_i32OutBlockListThreadIdx]->PopIdx();
			break;
		}
	}

	return pOut;
}

OUT_BLOCK_CONTROL * CDataBase::GetOutBlockControlData(void)
{
	OUT_BLOCK_CONTROL * pOut = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32OutBlockControlThreadIdx++; 
		if(m_i32OutBlockControlThreadIdx >= DB_THREAD_COUNT) m_i32OutBlockControlThreadIdx = 0;

		if(m_ppO_BlockControl[m_i32OutBlockControlThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (OUT_BLOCK_CONTROL *)m_ppO_BlockControl[m_i32OutBlockControlThreadIdx]->Pop();
			m_ppO_BlockControl[m_i32OutBlockControlThreadIdx]->PopIdx();
			break;
		}
	}

	return pOut;
}
#endif