#include "pch.h"
#include "DataBase.h"

#define DEF_CLAN_NOTE

static INT32 ReadDeleteDate(void)
{
	// 제거일시 재계산
	time_t serverTime;
	tm* deleteTime;
	time(&serverTime);
	serverTime += (REMAIN_DAY_FOR_READED_NOTE * 24 * 60 * 60);
	deleteTime = localtime(&serverTime);
	INT32 deleteDate = (deleteTime->tm_year + 1900) * 1000000 +	(deleteTime->tm_mon + 1) * 10000 + (deleteTime->tm_mday) * 100 + (deleteTime->tm_hour);
	return deleteDate; 
}


//쪽지 내부 작업 
INT32 CDataBase::Working_Note(INT32 i32ThreadIdx)
{
	IN_NOTE_LIST	 *	pInNoteList; 
	OUT_NOTE_LIST	 *	pOutNoteList; 
	IN_NOTE_COMMAND	 *	pInNoteCommand; 
	OUT_NOTE_COMMAND *	pOutNoteCommand; 
#ifdef DEF_CLAN_NOTE
	OUT_NOTE_COMMAND 	OutNoteCommandTemp;
#endif

	INT32				i32ECode = EVENT_ERROR_SUCCESS;
	INT32				i32Count = 0;
	INT32				i32ret_return = 0;
	INT32				i32ret_value = 0;
	INT32				i32WorkCount = 0;
	UINT32				ui32ListIdx = 0;
	wchar_t				wstrSQL[SQL_STRING_COUNT];

	INT32				i32DaySendCount = 0;	// 송신자가 하루에 보낼수 있는 쪽지의 최대수(MAX_SEND_NOTE_LIMIT로 제한)
	INT32				i32NoteCount = 0;		// 수신자의 쪽지함 최대 저장수(MAX_NORMAL_NOTE_COUNT로 제한)

	time_t				serverTime = 0;
	tm*					deleteTime = 0;	
	INT32				i32deleteDate = 0;
	TTCHAR				strNick[CONVERT_STRING_COUNT];
	TTCHAR				strMess[CONVERT_STRING_COUNT];
	INT32				i32TableIdx;

	// 초기화
	BOOL				bNoto_Delete_Flag = FALSE;
	i3mem::FillZero(&OutNoteCommandTemp, sizeof(OUT_NOTE_COMMAND));

	//1. 
	pInNoteList = (IN_NOTE_LIST*) m_ppI_NoteList[i32ThreadIdx]->Pop();
	if(pInNoteList != NULL)
	{
		pOutNoteList = (OUT_NOTE_LIST *)m_ppO_NoteList[i32ThreadIdx]->PushPointer();
		if(pOutNoteList == NULL)
		{
			//버퍼 오버 플러워 입니다. 로그를 남기고 이동합니다. 
			g_pLog->WriteLog( L"BUFFER OVERFLOW NoteList" );  
			goto note_list_exit;
		}

		pOutNoteList->i64UID	= pInNoteList->i64UID;
		pOutNoteList->iResult	= EVENT_ERROR_SUCCESS;
		pOutNoteList->iCount	= 0; 

		_GetNoteList(i32ThreadIdx, pInNoteList->i64UID, pOutNoteList);

		m_ppI_NoteList[i32ThreadIdx]->PopIdx();
		m_ppO_NoteList[i32ThreadIdx]->PushPointerIdx();
		i32WorkCount++;
	}
note_list_exit:

	pInNoteCommand = (IN_NOTE_COMMAND*) m_ppI_NoteCommand[i32ThreadIdx]->Pop();
	if(pInNoteCommand != NULL)
	{
		pOutNoteCommand = (OUT_NOTE_COMMAND * )m_ppO_NoteCommand[i32ThreadIdx]->PushPointer();
		if(pOutNoteCommand == NULL)
		{
			//버퍼 오버 플러워 입니다. 로그를 남기고 이동합니다. 
			g_pLog->WriteLog( L"BUFFER OVERFLOW NoteCommand" ); 
			goto note_command_exit; 
		}

		// OUT_NOTE_INFO 설정
		pOutNoteCommand->note_info.iResult	= EVENT_ERROR_SUCCESS;
		pOutNoteCommand->note_info.iType	= pInNoteCommand->iType;
		pOutNoteCommand->note_info.iCount	= 0;
		pOutNoteCommand->note_info.i64UID	= pInNoteCommand->i64UID;

		switch( pInNoteCommand->iType )
		{
		case NOTE_COMMAND_TYPE_ADD_NORMAL	:
		case NOTE_COMMAND_TYPE_ADD_NOTICE	:
		case NOTE_COMMAND_TYPE_ADD_CLAN		:
		case NOTE_COMMAND_TYPE_ADD_CLANASK	:		
			{
				//1.받을 사람 설정 
				pOutNoteCommand->note_info.i64RecvUID = pInNoteCommand->i64ReceiverUID;
				
				if( pInNoteCommand->iType == NOTE_COMMAND_TYPE_ADD_NORMAL )
				{
					if (0 < pOutNoteCommand->note_info.i64RecvUID)	// PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ 을 통해 왔을 경우.
					{
						//이런 유저가 있는지 UID 체크 
						if(! _GetNickByReceiverUID(i32ThreadIdx, pOutNoteCommand->note_info.i64RecvUID) )
						{
							pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
							goto note_error_code;
						}
					}
					else	// PROTOCOL_MESSENGER_NOTE_SEND_REQ 를 통해 왔을 경우.
					{
						//이런 유저가 있는지	닉네임 체크 
						if(! _GetUIDByNick(i32ThreadIdx, pInNoteCommand->strNickRecv, &pOutNoteCommand->note_info.i64RecvUID) )
						{
							pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
							goto note_error_code;
						}
					}

#if defined( USE_BLOCK_LIST )
					// 쪽지 보내려는 유저가 받는 유저로 부터 차단 당했는지 체크한다.
					BOOL	bCheckBlock = FALSE;
					i32ECode = _NoteCheckBlock(i32ThreadIdx, pOutNoteCommand->note_info.i64UID, pOutNoteCommand->note_info.i64RecvUID, &bCheckBlock );											
					if (EVENT_ERROR_SUCCESS != i32ECode)
					{
						pOutNoteCommand->note_info.iResult = EVENT_ERROR_EVENT_BLOCK_QUERY;	// DB 쿼리 실패.
						goto note_error_code;
					}
					if( TRUE == bCheckBlock ) // 차단 당했다면
					{
						pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_BLOCK_USER;	// 차단으로 인해 쪽지 전송 실패.
						goto note_error_code;
					}
#endif
					//  Sender Nick 끝자리 NULL 추가 저장				
					pInNoteCommand->iNickSendSize = (UINT8)(i3String::Length( pInNoteCommand->strNickSend ) + 1);
					if(pInNoteCommand->iNickSendSize == 1)pInNoteCommand->iNickSendSize = 0;
					if(pInNoteCommand->iNickSendSize > (NET_NICK_NAME_SIZE - 1)) 
					{
						pInNoteCommand->iNickSendSize = NET_NICK_NAME_SIZE - 1; 
						pInNoteCommand->strNickSend[pInNoteCommand->iNickSendSize] = '\0';
					}
				}
			}
			break;
		case NOTE_COMMAND_TYPE_DEL	:
		case NOTE_COMMAND_TYPE_READ :
			{
				; // skip
			}
			break;
		case NOTE_COMMAND_TYPE_ADD_GIFT:
		{
			//1.받을 사람 설정 
			pOutNoteCommand->note_info.i64RecvUID = pInNoteCommand->i64ReceiverUID;

			if (!_GetNickByReceiverUID(i32ThreadIdx, pOutNoteCommand->note_info.i64RecvUID))
			{
				pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
				goto note_error_code;
			}

			//  Sender Nick 끝자리 NULL 추가 저장			
			pInNoteCommand->iNickSendSize = (UINT8)(i3String::Length(pInNoteCommand->strNickSend) + 1);
			if (pInNoteCommand->iNickSendSize == 1)pInNoteCommand->iNickSendSize = 0;
			if (pInNoteCommand->iNickSendSize > (NET_NICK_NAME_SIZE - 1))
			{
				pInNoteCommand->iNickSendSize = NET_NICK_NAME_SIZE - 1;
				pInNoteCommand->strNickSend[pInNoteCommand->iNickSendSize] = '\0';
			}
		}
		break;

		default	:
			{//나올수 없음.. 이전 단계에서 막혀야 함.
				pOutNoteCommand->note_info.iResult = EVENT_ERROR_EVENT_UNEXPECTED_VALUE;
				g_pLog->WriteLog( L"Database Note iType Error" );
			}
			break; 
		}

		switch( pInNoteCommand->iType )
		{
		case NOTE_COMMAND_TYPE_ADD_NORMAL	:
		case NOTE_COMMAND_TYPE_ADD_NOTICE	:
		case NOTE_COMMAND_TYPE_ADD_CLAN		:
		case NOTE_COMMAND_TYPE_ADD_CLANASK	:
		case NOTE_COMMAND_TYPE_ADD_GIFT:
			{

				//2 자신인지				//EVENT_ERROR_NOTE_SEND_SELF
				if( pInNoteCommand->iType != NOTE_COMMAND_TYPE_ADD_NOTICE && pInNoteCommand->iType != NOTE_COMMAND_TYPE_ADD_GIFT)
				{
					if(pOutNoteCommand->note_info.i64RecvUID == pInNoteCommand->i64UID)
					{
						pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_SELF;
						//break;
						goto note_error_code;
					}
				}

				//3 하루발송제한 검사		//EVENT_ERROR_NOTE_SEND_DAILY_LIMIT
				i32DaySendCount = _GetNoteDaySendCount(i32ThreadIdx, pInNoteCommand->i64UID); 
				if( (pInNoteCommand->iType == NOTE_COMMAND_TYPE_ADD_NORMAL) &&
					((MAX_SEND_NOTE_LIMIT <= i32DaySendCount) || (i32DaySendCount < 0)) )
				{
					pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_DAILY_LIMIT;
					//break;
					goto note_error_code;
				}

				//4. 쪽지함이 풀인지 검사	//EVENT_ERROR_NOTE_SEND_BOX_FULL
				// TODO : 클랜 쪽지도 쪽지함의 갯수에 포함해 체크할 것이지 결정
				// 
				i32NoteCount = _GetNoteCount(i32ThreadIdx, pOutNoteCommand->note_info.i64RecvUID, &ui32ListIdx); 
				if( (pInNoteCommand->iType == NOTE_COMMAND_TYPE_ADD_NORMAL) &&
					((MAX_NORMAL_NOTE_COUNT <=  i32NoteCount) || (i32NoteCount < 0)) )
				{
					pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_BOX_FULL;
					goto note_error_code;
				}

				// 수신자의 Note Table Num
				i32TableIdx = (INT32)(pOutNoteCommand->note_info.i64RecvUID % DB_THREAD_COUNT);

				// 쪽지함이 풀일때 클랜 메시지이면 가장 오래된 쪽지를 삭제하고 새 쪽지 추가
				if( pInNoteCommand->iType != NOTE_COMMAND_TYPE_ADD_NORMAL)
				{
					// 클랜 쪽지들은 하루 제한을 받지 않는다.					
					// 쪽지함이 가득찬 유저에게 쪽지 전달시 오래된 쪽지를 삭제하고 전송한다.
					if(MAX_NORMAL_NOTE_COUNT <=  i32NoteCount)
					{
						i32ECode = _DeleteNoteTemp(i32ThreadIdx, i32TableIdx, ui32ListIdx, &pOutNoteCommand->note_info);											
						if (EVENT_ERROR_SUCCESS == i32ECode)
						{
						#ifdef DEF_CLAN_NOTE
							bNoto_Delete_Flag = TRUE;
							OutNoteCommandTemp.note_info.i64UID = pOutNoteCommand->note_info.i64RecvUID;
							OutNoteCommandTemp.note_info.iResult = EVENT_RECEIVE_CLAN_NOTE;
							OutNoteCommandTemp.note_info.iType = NOTE_COMMAND_TYPE_DEL;
							OutNoteCommandTemp.note_info.iCount = 1;
							OutNoteCommandTemp.note_info.pList[0] = ui32ListIdx;
						#endif
						}
					}
				}
				else
				{
					// 일반쪽지만 하루 전송량을 측정한다.
					i32DaySendCount = i32DaySendCount + 1;
				}

				//5. 쪽지를 보낸다. 
				{
					time(&serverTime);
					serverTime += (REMAIN_DAY_FOR_UNREAD_NOTE * 24 * 60 * 60);
					deleteTime = localtime(&serverTime);
					i32deleteDate = (deleteTime->tm_year + 1900) * 1000000 + (deleteTime->tm_mon + 1) * 10000 + (deleteTime->tm_mday) * 100 + (deleteTime->tm_hour);

					/*_ConvertString(strNick, pInNoteCommand->strNickSend); 
					_ConvertString(strMess, pInNoteCommand->strMess); */
					S2DB::ConvertString(strNick, CONVERT_STRING_COUNT, pInNoteCommand->strNickSend); 
					S2DB::ConvertString(strMess, CONVERT_STRING_COUNT, pInNoteCommand->strMess); 

					S2DB::MakeQueryW( wstrSQL, L"usp_Note_ins",
						DB_DATA_TYPE_INT,		i32TableIdx,
						DB_DATA_TYPE_BIGINT,	pOutNoteCommand->note_info.i64RecvUID, 
						DB_DATA_TYPE_BIGINT,	pOutNoteCommand->note_info.i64UID,
						DB_DATA_TYPE_INT,		pInNoteCommand->iType,
						DB_DATA_TYPE_INT,		NS_UNREAD,
						DB_DATA_TYPE_INT,		i32deleteDate,
						DB_DATA_TYPE_INT,		pInNoteCommand->iAppendingData,
						DB_DATA_TYPE_INT,		pInNoteCommand->iNickSendSize,
						DB_DATA_TYPE_INT,		pInNoteCommand->iMessSize,
						DB_DATA_TYPE_TTCHAR,	strNick,
						DB_DATA_TYPE_TTCHAR,	strMess,
						DB_DATA_TYPE_INT,		i32DaySendCount,
						DB_DATA_TYPE_INT,		i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD(),
						DB_DATA_TYPE_END);
				
					if( !m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL) )
					{
						pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;
						ErrorDBMess( wstrSQL, i32ThreadIdx);
						goto note_error_code;
					}

					// 결과 처리 과정
					i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
					if( 0 < i32Count )
					{
						i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
						if (1 != i32ret_return)
						{
							// Error
							i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
							g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
							pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY; 
							goto note_error_code;
						}
					}
					else
					{
						// 결과 처리 Error			
						g_pLog->WriteLog( L"[ERROR][Empty RecordSet][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
						pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY; 
						goto note_error_code;
					}
				
					// Save as New Note Id
					pOutNoteCommand->note_node._head._id = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);

				//-----------------------------------------------------
				// NOTE_NODE 설정
				//-----------------------------------------------------
					//기간 확인 					
					pOutNoteCommand->note_node._head._remainDay = GetRemainDay(i32deleteDate);

					MakeNoteNode(&pOutNoteCommand->note_node,
								 pOutNoteCommand->note_node._head._id,
								 pInNoteCommand->i64UID,
								 pInNoteCommand->iType,
								 NS_UNREAD,
								 pOutNoteCommand->note_node._head._remainDay,
								 pInNoteCommand->iAppendingData,
								 pInNoteCommand->iNickSendSize,
								 pInNoteCommand->iMessSize,
								 pInNoteCommand->strNickSend,
								 pInNoteCommand->strMess,
								 NULL);

				//-----------------------------------------------------
				}
			}

	note_error_code:
	#ifdef DEF_CLAN_NOTE
			if (TRUE == bNoto_Delete_Flag)
			{
				m_ppI_NoteCommand[i32ThreadIdx]->PopIdx();
				m_ppO_NoteCommand[i32ThreadIdx]->PushPointerIdx();
				m_ppO_NoteCommand[i32ThreadIdx]->Push(&OutNoteCommandTemp);
				i32WorkCount++;
				goto note_command_exit;
			}
	#endif
			break;
		case NOTE_COMMAND_TYPE_DEL	: 
			{
				for(UINT8 i = 0; i < pInNoteCommand->iCount; i++)
				{
					_DeleteNote(i32ThreadIdx, pInNoteCommand->pList[i], &pOutNoteCommand->note_info);
				}
			}
			break;
		case NOTE_COMMAND_TYPE_READ : 
			for(UINT8 i = 0; i < pInNoteCommand->iCount; i++)
			{
					S2DB::MakeQueryW(wstrSQL, L"usp_Note_upd",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_TINYINT,		NS_READED,
					DB_DATA_TYPE_INT,			ReadDeleteDate(),
					DB_DATA_TYPE_INT,			pInNoteCommand->pList[i],
					DB_DATA_TYPE_END);
				if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
				{
					pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;
					ErrorDBMess( wstrSQL, i32ThreadIdx);
					continue;
				}

				// 결과 처리 과정
				i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
				if( 0 < i32Count )
				{
					i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
					if (1 != i32ret_return)
					{
						// Error
						i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
						g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
						pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;
						continue;
					}
				}
				else
				{
					// 결과 처리 Error
					g_pLog->WriteLog( L"[ERROR][Empty RecordSet][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
					pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;
					continue;
				}

				// 결과 성공
				// 결과 처리
				pOutNoteCommand->note_info.pList[pOutNoteCommand->note_info.iCount] = pInNoteCommand->pList[i];
				pOutNoteCommand->note_info.iCount++; 

			}
			break;
		default						: 
			{//나올수 없음.. 이전 단계에서 막혀야 함.
				pOutNoteCommand->note_info.iResult = EVENT_ERROR_EVENT_UNEXPECTED_VALUE;
				g_pLog->WriteLog(  L"Database Note iType Error" );
			}
			break;
		}

		m_ppI_NoteCommand[i32ThreadIdx]->PopIdx();
		m_ppO_NoteCommand[i32ThreadIdx]->PushPointerIdx();
		i32WorkCount++;
	}
note_command_exit:
    
	return i32WorkCount;
}


//쪽지 리스트 받아 가지고 오기
BOOL CDataBase::InNoteList(INT64 i64UID)
{	
	INT32 i32ThreadIdx = (INT32)(i64UID % DB_THREAD_COUNT);
	if(i32ThreadIdx < 0)
	{
		g_pLog->WriteLog( L"[ERROR] CDataBase::InNoteList() : iThreadIdx < 0 (UID : %I64d)", i64UID);
		return FALSE;
	}

	IN_NOTE_LIST * pNoteList = (IN_NOTE_LIST *)m_ppI_NoteList[i32ThreadIdx]->PushPointer();
	if(pNoteList == NULL)return FALSE;

	pNoteList->i64UID = i64UID;
	m_ppI_NoteList[i32ThreadIdx]->PushPointerIdx();

	return TRUE;
}

//쪽지 리스트 받아가지고 가기 
OUT_NOTE_LIST * CDataBase::GetOutNoteList(void)
{ 
	OUT_NOTE_LIST * pNoteList = NULL; 

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32PopIdxNoteList++;
		if(m_i32PopIdxNoteList >= DB_THREAD_COUNT)m_i32PopIdxNoteList = 0;
		if( m_ppO_NoteList[m_i32PopIdxNoteList]->GetBufferCount() > 0)
		{
			pNoteList = (OUT_NOTE_LIST *)m_ppO_NoteList[m_i32PopIdxNoteList]->Pop();
			break; 
		}
	}

	return pNoteList; 
}

//쪽지 추가, 삭제,읽기
BOOL CDataBase::InNoteCommand(INT8 i8Type, INT64 i64UID, UINT8 ui8Count, INT32 * pi32List)
{
	INT32 i32ThreadIdx = (INT32)(i64UID % DB_THREAD_COUNT);
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_NoteCommand[i32ThreadIdx]->PushPointer(); 
	if(pNoteCommand == NULL)return FALSE;
	pNoteCommand->i64UID	= i64UID;
	pNoteCommand->iType		= i8Type;
	pNoteCommand->iCount	= ui8Count;
	i3mem::Copy( pNoteCommand->pList, pi32List, sizeof(INT32) * pNoteCommand->iCount); 

	m_ppI_NoteCommand[i32ThreadIdx]->PushPointerIdx();
	return TRUE; 
}


BOOL CDataBase::InNoteCommandAddClan(INT8 i8Type, OGame2Mess_Clan_Note_Send_Req stGame2Mess_Clan_Note_Send_Req)
//BOOL CDataBase::InNoteCommandAddClan(INT8 iType, UINT32 iUID, UINT32 iReceiverUID, char * pClanName, char * pMessage, UINT8 iClanSize, UINT8 iMessSize, UINT32 iAppendingData)
{
	INT32 i32ThreadIdx = (INT32)(stGame2Mess_Clan_Note_Send_Req.iUID % DB_THREAD_COUNT);
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_NoteCommand[i32ThreadIdx]->PushPointer();
	if(pNoteCommand == NULL)return FALSE; 

	// 보내는 값이 없는 경우 에러로 처리
	if (0 == i3String::Length(stGame2Mess_Clan_Note_Send_Req.clanName)) { return FALSE; }
	if (0 == i3String::Length(stGame2Mess_Clan_Note_Send_Req.message)) { return FALSE; }

	pNoteCommand->iType				= i8Type;
	pNoteCommand->i64UID			= stGame2Mess_Clan_Note_Send_Req.iUID; 
	pNoteCommand->i64ReceiverUID	= stGame2Mess_Clan_Note_Send_Req.receiverUID; 

	//i3String::Copy( pNoteCommand->strNickSend, stGame2Mess_Clan_Note_Send_Req.clanName); 
	i3mem::FillZero(pNoteCommand->strNickSend, NET_NICK_NAME_SIZE);
	i3String::Copy(pNoteCommand->strNickSend, stGame2Mess_Clan_Note_Send_Req.clanName, NET_CLAN_NAME_SIZE);
	pNoteCommand->iNickSendSize	= stGame2Mess_Clan_Note_Send_Req.clanNameSize; 	

	//i3String::Copy( pNoteCommand->strMess, stGame2Mess_Clan_Note_Send_Req.message ); 
	i3mem::FillZero(pNoteCommand->strMess, sizeof(TTCHAR) * NET_NOTE_SIZE);
	i3String::NCopy(pNoteCommand->strMess, stGame2Mess_Clan_Note_Send_Req.message, NET_NOTE_SIZE-1);
	pNoteCommand->iMessSize		= stGame2Mess_Clan_Note_Send_Req.messageSize; 

	pNoteCommand->iAppendingData = stGame2Mess_Clan_Note_Send_Req.clanIdx; 

	//사용하지 않는 변수 세팅 	
	pNoteCommand->strNickRecv[0] = 0;

	m_ppI_NoteCommand[i32ThreadIdx]->PushPointerIdx();
	return TRUE; 
}

BOOL CDataBase::InNoteCommandAdd(INT8 i8Type, TTCHAR * pstrNickSend, OGame2Mess_Note_Send_Req stGame2Mess_Note_Send_Req)
//BOOL CDataBase::InNoteCommandAdd(INT8 iType, UINT32 iUID, char * pNickSend, char * pNickRecv, char * pMessage, UINT8 iMessSize)
{
	INT32 i32ThreadIdx = (INT32)(stGame2Mess_Note_Send_Req.iUID % DB_THREAD_COUNT);
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_NoteCommand[i32ThreadIdx]->PushPointer();
	if(pNoteCommand == NULL)return FALSE;

	// 보내는 값이 없는 경우 에러로 처리
	if (0 == i3String::Length(pstrNickSend)) { return FALSE; }

	// 수신자가 없거나 보낼 쪽지 내용이 없으면 에러로 처리
	if (0 == i3String::Length(stGame2Mess_Note_Send_Req.nickname)) { return FALSE; }
	if (0 == i3String::Length(stGame2Mess_Note_Send_Req.message)) { return FALSE; }

	pNoteCommand->iType			= i8Type; 
	pNoteCommand->i64UID		= stGame2Mess_Note_Send_Req.iUID;

	//i3String::Copy( pNoteCommand->strNickSend, pNickSend); 
	//i3String::Copy( pNoteCommand->strNickRecv, stGame2Mess_Note_Send_Req.nickname); 
	i3mem::FillZero(pNoteCommand->strNickSend, NET_NICK_NAME_SIZE);
	i3mem::FillZero(pNoteCommand->strNickRecv, NET_NICK_NAME_SIZE);
	i3mem::FillZero(pNoteCommand->strMess, NET_NOTE_SIZE);
	i3String::NCopy(pNoteCommand->strNickSend, pstrNickSend, NET_NICK_NAME_SIZE-1);
	i3String::NCopy(pNoteCommand->strNickRecv, stGame2Mess_Note_Send_Req.nickname, NET_NICK_NAME_SIZE-1);

	//i3String::Copy( pNoteCommand->strMess,		stGame2Mess_Note_Send_Req.message);
	i3String::NCopy( pNoteCommand->strMess,	stGame2Mess_Note_Send_Req.message, NET_NOTE_SIZE-1);
	
	pNoteCommand->iMessSize		= stGame2Mess_Note_Send_Req.messageSize;

	//사용하지 않는 변수 세팅 
	pNoteCommand->i64ReceiverUID	= 0;
	pNoteCommand->iAppendingData	= 0;

	m_ppI_NoteCommand[i32ThreadIdx]->PushPointerIdx();
	return TRUE; 
} // InNoteCommandAdd()

BOOL CDataBase::InNoteCommandAddFindUID(INT8 i8Type, TTCHAR * pstrNickSend, OGame2Mess_Note_Send_Find_UID_Req stGame2Mess_Note_Send_Find_UID_Req)
//BOOL CDataBase::InNoteCommandAddFindUID(INT8 iType, UINT32 iUID, char * pNickSend, char * pNickRecv, char * pMessage, UINT8 iMessSize)
{
	INT32 i32ThreadIdx = (INT32)(stGame2Mess_Note_Send_Find_UID_Req.iUID % DB_THREAD_COUNT);
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_NoteCommand[i32ThreadIdx]->PushPointer(); 
	if(pNoteCommand == NULL)return FALSE;

	// 보내는 값이 없는 경우 에러로 처리	
	if (0 == i3String::Length(pstrNickSend)) { return FALSE; }

	// 수신자가 없거나 보낼 쪽지 내용이 없으면 에러로 처리
	if (0 >= stGame2Mess_Note_Send_Find_UID_Req.iReceiverUID) { return FALSE; }
	//if (0 == strlen(stGame2Mess_Note_Send_Find_UID_Req.nickname)) { return FALSE; }
	if (0 == i3String::Length(stGame2Mess_Note_Send_Find_UID_Req.message)) { return FALSE; }

	pNoteCommand->iType			= i8Type;
	pNoteCommand->i64UID		= stGame2Mess_Note_Send_Find_UID_Req.iUID; 	

	i3mem::FillZero(pNoteCommand->strNickSend, NET_NICK_NAME_SIZE);
	i3mem::FillZero(pNoteCommand->strMess, NET_NOTE_SIZE);

	//i3String::Copy( pNoteCommand->strNickSend, pNickSend); 	
	i3String::NCopy( pNoteCommand->strNickSend, pstrNickSend, NET_NICK_NAME_SIZE-1);
	//i3String::Copy( pNoteCommand->strNickRecv, stGame2Mess_Note_Send_Find_UID_Req.nickname); 

	//i3String::Copy( pNoteCommand->strMess,		stGame2Mess_Note_Send_Find_UID_Req.message); 
	i3String::NCopy( pNoteCommand->strMess, stGame2Mess_Note_Send_Find_UID_Req.message, NET_NOTE_SIZE-1); 
	pNoteCommand->iMessSize		= stGame2Mess_Note_Send_Find_UID_Req.messageSize; 

	//사용하지 않는 변수 세팅 
	pNoteCommand->i64ReceiverUID	= stGame2Mess_Note_Send_Find_UID_Req.iReceiverUID; 
	pNoteCommand->iAppendingData	= 0; 

	m_ppI_NoteCommand[i32ThreadIdx]->PushPointerIdx();
	return TRUE; 
} // InNoteCommandAddFindUID()

/*
BOOL CDataBase::InNoteCommandAdd(INT8 iType, UINT32 iUID, UINT32 iReceiverUID, char * pNickSend, char * pNick, char * pMessage, UINT8 iNickSize, UINT8 iMessSize)
{
	INT32 iThreadIdx = iUID % DB_THREAD_COUNT;
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_NoteCommand[iThreadIdx]->PushPointer();
	if(pNoteCommand == NULL)return FALSE;
	pNoteCommand->iUID			= iUID;
	pNoteCommand->iReceiverUID	= iReceiverUID;
	pNoteCommand->iType			= iType;

	i3String::Copy( pNoteCommand->strNickSend, pNickSend);
	i3String::Copy( pNoteCommand->strNickRecv, pNick );
	i3String::Copy( pNoteCommand->strMess, pMessage );

	pNoteCommand->iNickSendSize	= iNickSize;
	pNoteCommand->iMessSize		= iMessSize;

	m_ppI_NoteCommand[iThreadIdx]->PushPointerIdx();
	return TRUE; 
}
*/
OUT_NOTE_COMMAND * CDataBase::GetOutNoteCommand(void)
{
	OUT_NOTE_COMMAND * pNoteCommand = NULL; 

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32PopIdxNoteCommand++;
		if(m_i32PopIdxNoteCommand >= DB_THREAD_COUNT)m_i32PopIdxNoteCommand = 0;
		if( m_ppO_NoteCommand[m_i32PopIdxNoteCommand]->GetBufferCount() > 0)
		{
			pNoteCommand = (OUT_NOTE_COMMAND *)m_ppO_NoteCommand[m_i32PopIdxNoteCommand]->Pop();
			break; 
		}
	}

	return pNoteCommand; 
}

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// Change Nick

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// Change Rank

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// Clan Group Note
BOOL	CDataBase::InClanGroupNoteAddClan(INT8 i8Type, UINT32 ui32Count, OGame2Mess_Clan_Group_Note_Send_Req stGame2Mess_Clan_Group_Note_Send_Req)
{
	INT32 i32ThreadIdx = (INT32)(stGame2Mess_Clan_Group_Note_Send_Req.iUID % DB_THREAD_COUNT);
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_ClanGroupNote[i32ThreadIdx]->PushPointer(); 
	if(pNoteCommand == NULL)return FALSE;

	// 보내는 값이 없는 경우 에러로 처리
	if (0 == i3String::Length(stGame2Mess_Clan_Group_Note_Send_Req.clanName)) { return FALSE; }
	if (0 == i3String::Length(stGame2Mess_Clan_Group_Note_Send_Req.message)) { return FALSE; }

	pNoteCommand->iType				= i8Type;
	pNoteCommand->i64UID			= stGame2Mess_Clan_Group_Note_Send_Req.iUID; 
	pNoteCommand->i64ReceiverUID	= stGame2Mess_Clan_Group_Note_Send_Req.receiverUID[ui32Count];

	i3mem::FillZero(pNoteCommand->strNickSend, NET_NICK_NAME_SIZE);
	i3mem::FillZero(pNoteCommand->strMess, NET_NOTE_SIZE);

	//i3String::Copy( pNoteCommand->strNickSend, stGame2Mess_Clan_Group_Note_Send_Req.clanName); 
	i3String::Copy( pNoteCommand->strNickSend, stGame2Mess_Clan_Group_Note_Send_Req.clanName, NET_CLAN_NAME_SIZE); 
	pNoteCommand->iNickSendSize	= stGame2Mess_Clan_Group_Note_Send_Req.clanNameSize; 	

	//i3String::Copy( pNoteCommand->strMess, stGame2Mess_Clan_Group_Note_Send_Req.message ); 
	i3String::NCopy( pNoteCommand->strMess, stGame2Mess_Clan_Group_Note_Send_Req.message, NET_NOTE_SIZE-1); 
	pNoteCommand->iMessSize		= stGame2Mess_Clan_Group_Note_Send_Req.messageSize; 

	pNoteCommand->iAppendingData = stGame2Mess_Clan_Group_Note_Send_Req.clanIdx; 

	//사용하지 않는 변수 세팅 	
	pNoteCommand->strNickRecv[0] = 0;

	m_ppI_ClanGroupNote[i32ThreadIdx]->PushPointerIdx();
	return TRUE;
}

OUT_NOTE_COMMAND * CDataBase::GetOutClanGroupNoteCommand(void)
{
	OUT_NOTE_COMMAND * pNoteCommand = NULL;

	for(INT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_i32PopIdxClanGroupNoteCommand++;
		if(m_i32PopIdxClanGroupNoteCommand >= DB_THREAD_COUNT)	m_i32PopIdxClanGroupNoteCommand = 0;
		if( m_ppO_ClanGroupNote[m_i32PopIdxClanGroupNoteCommand]->GetBufferCount() > 0)
		{
			pNoteCommand = (OUT_NOTE_COMMAND *)m_ppO_ClanGroupNote[m_i32PopIdxClanGroupNoteCommand]->Pop();
			break; 
		}
	}

	return pNoteCommand;
}

// 그룹 쪽지 DB 작업
INT32 CDataBase::Working_Clan_Group_Note(INT32 i32ThreadIdx)
{
	IN_NOTE_LIST	 *	pInNoteList;
	OUT_NOTE_LIST	 *	pOutNoteList;
	IN_NOTE_COMMAND	 *	pInNoteCommand;
	OUT_NOTE_COMMAND *	pOutNoteCommand;
#ifdef DEF_CLAN_NOTE
	OUT_NOTE_COMMAND 	OutNoteCommandTemp;
#endif

	INT32				i32ECode = EVENT_ERROR_SUCCESS;
	INT32				i32Count = 0;
	INT32				i32ret_return = 0;
	INT32				i32ret_value = 0;
	INT32				i32WorkCount = 0;
	UINT32				ui32ListIdx = 0;
	wchar_t				wstrSQL[SQL_STRING_COUNT];

	INT32				i32DaySendCount = 0;	// 송신자가 하루에 보낼수 있는 쪽지의 최대수(MAX_SEND_NOTE_LIMIT로 제한)
	INT32				i32NoteCount = 0;		// 수신자의 쪽지함 최대 저장수(MAX_NORMAL_NOTE_COUNT로 제한)

	time_t				serverTime = 0;
	tm*					deleteTime = 0;
	INT32				i32deleteDate = 0;
	TTCHAR				strNick[CONVERT_STRING_COUNT];
	TTCHAR				strMess[CONVERT_STRING_COUNT];
	INT32				i32TableIdx;

	// 초기화
	BOOL				bNoto_Delete_Flag = FALSE;
	i3mem::FillZero(&OutNoteCommandTemp, sizeof(OUT_NOTE_COMMAND));

	//1. 
	pInNoteList = (IN_NOTE_LIST*) m_ppI_NoteList[i32ThreadIdx]->Pop(); 
	if(pInNoteList != NULL)
	{
		pOutNoteList = (OUT_NOTE_LIST *)m_ppO_NoteList[i32ThreadIdx]->PushPointer();
		if(pOutNoteList == NULL)
		{
			//버퍼 오버 플러워 입니다. 로그를 남기고 이동합니다. 
			g_pLog->WriteLog( L"BUFFER OVERFLOW NoteList" );  
			goto note_list_exit; 
		}

		pOutNoteList->i64UID	= pInNoteList->i64UID;
		pOutNoteList->iResult	= EVENT_ERROR_SUCCESS;
		pOutNoteList->iCount	= 0; 

		_GetNoteList(i32ThreadIdx, pInNoteList->i64UID, pOutNoteList);

		m_ppI_NoteList[i32ThreadIdx]->PopIdx();
		m_ppO_NoteList[i32ThreadIdx]->PushPointerIdx();
		i32WorkCount++; 
	}
note_list_exit:

	pInNoteCommand = (IN_NOTE_COMMAND*) m_ppI_ClanGroupNote[i32ThreadIdx]->Pop();
	if(pInNoteCommand != NULL)
	{
		pOutNoteCommand = (OUT_NOTE_COMMAND * )m_ppO_ClanGroupNote[i32ThreadIdx]->PushPointer();
		if(pOutNoteCommand == NULL)
		{
			//버퍼 오버 플러워 입니다. 로그를 남기고 이동합니다. 
			g_pLog->WriteLog( L"BUFFER OVERFLOW NoteCommand" ); 
			goto note_command_exit;
		}

		//g_pLog->WriteLog(m_i32TaskIdx, "Sender(%d), Receiver(%d)", pOutNoteCommand->note_info.iUID, pOutNoteCommand->note_info.iRecvUID); 

		// OUT_NOTE_INFO 설정
		pOutNoteCommand->note_info.iResult	= EVENT_ERROR_SUCCESS;
		pOutNoteCommand->note_info.iType	= pInNoteCommand->iType;
		pOutNoteCommand->note_info.iCount	= 0 ;
		pOutNoteCommand->note_info.i64UID	= pInNoteCommand->i64UID;

		switch( pInNoteCommand->iType )
		{
		case NOTE_COMMAND_TYPE_ADD_NORMAL	:
		case NOTE_COMMAND_TYPE_ADD_NOTICE	:
		case NOTE_COMMAND_TYPE_ADD_CLAN		:
		case NOTE_COMMAND_TYPE_ADD_CLANASK	:
			{
				//1.받을 사람 설정 
				pOutNoteCommand->note_info.i64RecvUID = pInNoteCommand->i64ReceiverUID; 

				if( pInNoteCommand->iType == NOTE_COMMAND_TYPE_ADD_NORMAL )
				{
					if (0 < pOutNoteCommand->note_info.i64RecvUID)
					{
						//이런 유저가 있는지 UID 체크 
						if(! _GetNickByReceiverUID(i32ThreadIdx, pOutNoteCommand->note_info.i64RecvUID) )
						{
							pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
							//break; 
							goto note_error_code;
						}
					}
					else
					{
						//이런 유저가 있는지	닉네임 체크 
						if(! _GetUIDByNick(i32ThreadIdx, pInNoteCommand->strNickRecv, &pOutNoteCommand->note_info.i64RecvUID) )
						{
							pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
							//break; 
							goto note_error_code;
						}
					}

					//  Sender Nick 끝자리 NULL 추가 저장
					pInNoteCommand->iNickSendSize = (UINT8)(i3String::Length( pInNoteCommand->strNickSend ) + 1);
					if(pInNoteCommand->iNickSendSize == 1)pInNoteCommand->iNickSendSize = 0;
					if(pInNoteCommand->iNickSendSize > (NET_NICK_NAME_SIZE - 1))
					{
						pInNoteCommand->iNickSendSize = NET_NICK_NAME_SIZE - 1;
						pInNoteCommand->strNickSend[pInNoteCommand->iNickSendSize] = '\0';
					}
				}
			}
			break;
		case NOTE_COMMAND_TYPE_DEL	:
		case NOTE_COMMAND_TYPE_READ :
			{
				; // skip
			}
			break;
		default	:
			{//나올수 없음.. 이전 단계에서 막혀야 함.
				pOutNoteCommand->note_info.iResult = EVENT_ERROR_EVENT_UNEXPECTED_VALUE;
				g_pLog->WriteLog( L"Database Note iType Error" );
			}
			break; 
		}

		switch( pInNoteCommand->iType )
		{
		case NOTE_COMMAND_TYPE_ADD_NORMAL	:
		case NOTE_COMMAND_TYPE_ADD_NOTICE	:
		case NOTE_COMMAND_TYPE_ADD_CLAN		:
		case NOTE_COMMAND_TYPE_ADD_CLANASK	:
			{

				//2 자신인지				//EVENT_ERROR_NOTE_SEND_SELF
				if( pInNoteCommand->iType != NOTE_COMMAND_TYPE_ADD_NOTICE )
				{
					if(pOutNoteCommand->note_info.i64RecvUID == pInNoteCommand->i64UID)
					{
						pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_SELF;
						//break; 
						goto note_error_code;
					}
				}

				//3 하루발송제한 검사		//EVENT_ERROR_NOTE_SEND_DAILY_LIMIT
				i32DaySendCount = _GetNoteDaySendCount(i32ThreadIdx, pInNoteCommand->i64UID); 
				if( (pInNoteCommand->iType == NOTE_COMMAND_TYPE_ADD_NORMAL) &&
					((MAX_SEND_NOTE_LIMIT <= i32DaySendCount) || (i32DaySendCount < 0)) )
				{
					pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_DAILY_LIMIT;
					//break;
					goto note_error_code;
				}

				//4. 쪽지함이 풀인지 검사	//EVENT_ERROR_NOTE_SEND_BOX_FULL
				// TODO : 클랜 쪽지도 쪽지함의 갯수에 포함해 체크할 것이지 결정
				// 
				i32NoteCount = _GetNoteCount(i32ThreadIdx, pOutNoteCommand->note_info.i64RecvUID, &ui32ListIdx); 
				if( (pInNoteCommand->iType == NOTE_COMMAND_TYPE_ADD_NORMAL) 
					&& (MAX_NORMAL_NOTE_COUNT <=  i32NoteCount) || (i32NoteCount < 0) )
				{
					pOutNoteCommand->note_info.iResult = EVENT_ERROR_NOTE_SEND_BOX_FULL;
					//break;
					goto note_error_code;
				}

				// 수신자의 Note Table Num
				i32TableIdx = (INT32)(pOutNoteCommand->note_info.i64RecvUID % DB_THREAD_COUNT);

				// 쪽지함이 풀일때 클랜 메시지이면 가장 오래된 쪽지를 삭제하고 새 쪽지 추가
				if( pInNoteCommand->iType != NOTE_COMMAND_TYPE_ADD_NORMAL)
				{
					// 클랜 쪽지들은 하루 전송량 제한을 받지 않는다.									
					// 쪽지함이 가득찬 유저에게 쪽지 전달시 오래된 쪽지를 삭제하고 전송한다.
					if(MAX_NORMAL_NOTE_COUNT <=  i32NoteCount)
					{
						//ECode = _DeleteNote(iTableIdx, iListIdx, &pOutNoteCommand->note_info);					
						i32ECode = _DeleteNoteTemp(i32ThreadIdx, i32TableIdx, ui32ListIdx, &pOutNoteCommand->note_info);											
						if (EVENT_ERROR_SUCCESS == i32ECode)
						{	
						#ifdef DEF_CLAN_NOTE
							bNoto_Delete_Flag = TRUE;
							OutNoteCommandTemp.note_info.i64UID = pOutNoteCommand->note_info.i64RecvUID;
							OutNoteCommandTemp.note_info.iResult = EVENT_RECEIVE_CLAN_NOTE;
							OutNoteCommandTemp.note_info.iType = NOTE_COMMAND_TYPE_DEL;
							OutNoteCommandTemp.note_info.iCount = 1;
							OutNoteCommandTemp.note_info.pList[0] = ui32ListIdx;
						#endif
						}
					}
				}
				else
				{	
					// 일반쪽지만 하루 전송량을 측정한다.						
					i32DaySendCount = i32DaySendCount + 1;
				}

				//5. 쪽지를 보낸다. 
				{
					time(&serverTime);
					serverTime += (REMAIN_DAY_FOR_UNREAD_NOTE * 24 * 60 * 60);
					deleteTime = localtime(&serverTime);
					i32deleteDate = (deleteTime->tm_year + 1900) * 1000000 + (deleteTime->tm_mon + 1) * 10000 + (deleteTime->tm_mday) * 100 + (deleteTime->tm_hour);

					/*_ConvertString(strNick, pInNoteCommand->strNickSend); 
					_ConvertString(strMess, pInNoteCommand->strMess); */
					S2DB::ConvertString(strNick, CONVERT_STRING_COUNT, pInNoteCommand->strNickSend); 
					S2DB::ConvertString(strMess, CONVERT_STRING_COUNT, pInNoteCommand->strMess);

					S2DB::MakeQueryW(wstrSQL, L"usp_Note_ins",
						DB_DATA_TYPE_INT,		i32TableIdx,
						DB_DATA_TYPE_BIGINT,	pOutNoteCommand->note_info.i64RecvUID,
						DB_DATA_TYPE_BIGINT,	pOutNoteCommand->note_info.i64UID,
						DB_DATA_TYPE_INT,		pInNoteCommand->iType,
						DB_DATA_TYPE_INT,		NS_UNREAD,
						DB_DATA_TYPE_INT,		i32deleteDate,
						DB_DATA_TYPE_INT,		pInNoteCommand->iAppendingData,
						DB_DATA_TYPE_INT,		pInNoteCommand->iNickSendSize,
						DB_DATA_TYPE_INT,		pInNoteCommand->iMessSize,
						DB_DATA_TYPE_TTCHAR,	strNick,
						DB_DATA_TYPE_TTCHAR,	strMess,
						DB_DATA_TYPE_INT,		i32DaySendCount,
						DB_DATA_TYPE_INT,		i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD(),
						DB_DATA_TYPE_END	);
				
					if( !m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL) )
					{
						pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;
						ErrorDBMess( wstrSQL, i32ThreadIdx);
						goto note_error_code;
					}

					// 결과 처리 과정
					i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
					if( 0 < i32Count )
					{
						i32ret_return = (UINT32)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
						if (1 != i32ret_return)
						{
							// Error
							i32ret_value = (UINT32)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
							g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
							pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY; 
							goto note_error_code;
						}
					}
					else
					{
						// 결과 처리 Error						
					g_pLog->WriteLog( L"[ERROR][Empty RecordSet][ret_value(%d)][%s]", i32ret_value, wstrSQL   );
						pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY; 
						goto note_error_code;
					}
					// Save as New Note Id
					pOutNoteCommand->note_node._head._id = (UINT32)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);

					//-----------------------------------------------------
					// NOTE_NODE 설정
					//-----------------------------------------------------
					//기간 확인 					
					pOutNoteCommand->note_node._head._remainDay = GetRemainDay(i32deleteDate);

					MakeNoteNode(&pOutNoteCommand->note_node,
								 pOutNoteCommand->note_node._head._id,
								 pInNoteCommand->i64UID,
								 pInNoteCommand->iType,
								 NS_UNREAD,
								 pOutNoteCommand->note_node._head._remainDay,
								 pInNoteCommand->iAppendingData,
								 pInNoteCommand->iNickSendSize,
								 pInNoteCommand->iMessSize,
								 pInNoteCommand->strNickSend,
								 pInNoteCommand->strMess,
								 NULL);

					//-----------------------------------------------------
				}
			}

note_error_code:
		#ifdef DEF_CLAN_NOTE
			if (TRUE == bNoto_Delete_Flag)
			{
				m_ppI_ClanGroupNote[i32ThreadIdx]->PopIdx(); 
				m_ppO_ClanGroupNote[i32ThreadIdx]->PushPointerIdx(); 
				m_ppO_ClanGroupNote[i32ThreadIdx]->Push(&OutNoteCommandTemp);
				i32WorkCount++; 
				goto note_command_exit;
			}
		#endif
			break; 
		case NOTE_COMMAND_TYPE_DEL	: 
			{ 
				for(UINT8 i = 0; i < pInNoteCommand->iCount; i++)
				{
					_DeleteNote(i32ThreadIdx, pInNoteCommand->pList[i], &pOutNoteCommand->note_info);		
				}
			}
			break; 
		case NOTE_COMMAND_TYPE_READ :
			{
				for(UINT8 i = 0; i < pInNoteCommand->iCount; i++)
				{
					S2DB::MakeQueryW(wstrSQL, L"usp_Note_upd",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_TINYINT,		NS_READED,
					DB_DATA_TYPE_INT,			ReadDeleteDate(),
					DB_DATA_TYPE_INT,			pInNoteCommand->pList[i],
					DB_DATA_TYPE_END);

					// 결과 처리 과정
					i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
					if( 0 < i32Count )
					{
						i32ret_return =  (UINT32)m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
						if (1 != i32ret_return)
						{
							// Error
							i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
							g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
							pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;						
							continue;
						}
					}
					else
					{
						// 결과 처리 Error
						g_pLog->WriteLog( L"[ERROR][Empty RecordSet][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
						pOutNoteCommand->note_info.iResult = NET_ERROR_DB_QUERY;
						continue;
					}

					// 결과 성공
					// 결과 처리
					pOutNoteCommand->note_info.pList[pOutNoteCommand->note_info.iCount] = pInNoteCommand->pList[i];
					pOutNoteCommand->note_info.iCount++; 

				}
			}
			break;
		default						: 
			{//나올수 없음.. 이전 단계에서 막혀야 함.
				pOutNoteCommand->note_info.iResult = EVENT_ERROR_EVENT_UNEXPECTED_VALUE;
				g_pLog->WriteLog( L"Database Note iType Error"  );
			}
			break; 
		}

		m_ppI_ClanGroupNote[i32ThreadIdx]->PopIdx(); 
		m_ppO_ClanGroupNote[i32ThreadIdx]->PushPointerIdx(); 
		i32WorkCount++;
	}
note_command_exit:

	return i32WorkCount;
} // Working_Clan_Group_Note()

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//내부 쿼리문 

BOOL CDataBase::_GetNickByReceiverUID(const INT32 i32ThreadIdx,  const INT64 i64ReceiverUID)
{	
	CADODatabase *	pDataBase = m_ppADOdb_USER[i32ThreadIdx]; 

	INT32			i32Count = 0;
	INT32			i32ret_return = 0;
	INT32			i32ret_value = 0;

	wchar_t			wstrSQL[SQL_STRING_COUNT];

	S2DB::MakeQueryW(wstrSQL, L"usp_Basic_sel_NickGet",
		DB_DATA_TYPE_BIGINT,		i64ReceiverUID,
		DB_DATA_TYPE_END);

	if( !pDataBase->ExecuteSP(wstrSQL) )
	{
		ErrorDBUser( wstrSQL, i32ThreadIdx );
		pDataBase = m_ppADOdb_USER[i32ThreadIdx];
		return FALSE; 
	}

	// 결과 처리 과정
	// 1개의 Row가 아니면 모두 에러처리
	i32Count = pDataBase->GetRecordCount();
	if( 1 == i32Count )
	{
		i32ret_return =  pDataBase->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = pDataBase->GetINT32Data(1);
			g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			return FALSE; 
		}
		
		//INT32 iDelDate = pDataBase->GetUINT32Data(1);
		INT32 iDelDate = S2DB::ConvertSmallDateToUINT32( pDataBase->GetDateData(1) );
		if(iDelDate != 0)
		{
			g_pLog->WriteLog( L"[ERROR][iDelDate(%d)][%s]", iDelDate, wstrSQL  );
			return FALSE;
		}
		
		return TRUE;
	}

	// 결과처리 Error
	g_pLog->WriteLog( L"[ERROR][GetCount(0 != %d)][%s]", i32Count, wstrSQL  );

	return FALSE;
} // _GetUIDByReceiverUID()

BOOL CDataBase::_GetUIDByNick(INT32 i32ThreadIdx, const TTCHAR* pstrNickName, INT64* pi64outUID)
{	
	CADODatabase *	pDataBase = m_ppADOdb_USER[i32ThreadIdx]; 

	INT32			i32Count = 0;
	INT32			i32ret_return = 0;
	INT32			i32ret_value = 0;

	wchar_t			wstrSQL[SQL_STRING_COUNT];

	TTCHAR			strTempStr[CONVERT_STRING_COUNT];
	//_ConvertString(TempStr, strNickName); 
	S2DB::ConvertString(strTempStr, CONVERT_STRING_COUNT, pstrNickName);
			
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_USNGet",
		DB_DATA_TYPE_TTCHAR,	strTempStr,
		DB_DATA_TYPE_END);

	if( !pDataBase->ExecuteSP(wstrSQL) )
	{
		ErrorDBUser( wstrSQL, i32ThreadIdx );
		pDataBase = m_ppADOdb_USER[i32ThreadIdx];
		return FALSE; 
	}
	
	// 결과 처리 과정
	// 1개의 Row가 아니면 모두 에러처리
	i32Count = pDataBase->GetRecordCount();
	if( 1 == i32Count )
	{
		i32ret_return = pDataBase->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = pDataBase->GetINT32Data(1);
			g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			return FALSE; 
		}
		
		//INT32 iDelDate = pDataBase->GetUINT32Data(1);
		INT32 iDelDate = S2DB::ConvertSmallDateToUINT32( pDataBase->GetDateData(1) );
		if(iDelDate == 0)
		{
			*pi64outUID = pDataBase->GetINT64Data(2);
		
			return TRUE;
		}
		else
		{
			// 결과 처리 Error		
			g_pLog->WriteLog( L"[ERROR][iDelDate(%d)][%s]", iDelDate, wstrSQL  );
			return FALSE; 
		}
	}

	// 결과 처리 Error		
	//g_pLog->WriteLog(LOG_DB_DATABASE_IDX,  "[ERROR][GetRecordCount(%d)][%s]", iCount, strQuer);
	return FALSE; 
} // _GetUIDByNick()

INT32 CDataBase::_GetNoteCount(INT32 i32ThreadIdx, INT64 i64UID, UINT32* pui32ListIdx)
{
	CADODatabase *	pDataBase = m_ppADOdb_MESS[i32ThreadIdx];

	INT32			i32Count = 0;
	INT32			i32ret_return = 0;
	INT32			i32ret_value = 0;

	wchar_t			wstrSQL[SQL_STRING_COUNT];

	INT32			i32TableIdx = (INT32)(i64UID % DB_THREAD_COUNT); 
	
	S2DB::MakeQueryW( wstrSQL, L"usp_Note_sel_Count",
		DB_DATA_TYPE_INT,			i32TableIdx,
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);

	if(! pDataBase->ExecuteSP(wstrSQL) )
	{
		ErrorDBMess( wstrSQL, i32ThreadIdx );
		pDataBase = m_ppADOdb_MESS[i32ThreadIdx];
		return NET_ERROR_DB_QUERY;
	}

// 결과 처리 과정
	i32Count = pDataBase->GetRecordCount();
	if( 0 < i32Count)
	{
		i32ret_return = pDataBase->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = pDataBase->GetINT32Data(1);
			g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			return NET_ERROR_DB_QUERY;
		}

		*pui32ListIdx = pDataBase->GetINT32Data(1);
	}
	else
	{
		// 에러는 아님
		// 하나도 없다
	}

	return i32Count;
}

INT32 CDataBase::_GetNoteDaySendCount(INT32 i32ThreadIdx, INT64 i64UID)
{
	CADODatabase *	pDataBase = m_ppADOdb_MESS[i32ThreadIdx];
	
	INT32			i32Count = 0;
	INT32			i32ret_return = 0;
	INT32			i32ret_value = 0;
	wchar_t			wstrSQL[SQL_STRING_COUNT];

	INT32			i32sendCount = 0;

	S2DB::MakeQueryW( wstrSQL, L"usp_NoteState_sel",
		DB_DATA_TYPE_BIGINT,			i64UID,
		DB_DATA_TYPE_END);

	if(! pDataBase->ExecuteSP(wstrSQL) )
	{
		ErrorDBMess(wstrSQL, i32ThreadIdx); 
		pDataBase = m_ppADOdb_MESS[i32ThreadIdx]; 
		return NET_ERROR_DB_QUERY; 
	}

	// 결과 처리 과정
	i32Count = pDataBase->GetRecordCount();
	if( 0 < i32Count )
	{
		i32ret_return = pDataBase->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = pDataBase->GetINT32Data(1);
	
			g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			return NET_ERROR_DB_QUERY; 
		}
	}
	else
	{
		// 결과 처리 Error		
		g_pLog->WriteLog( L"[ERROR][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
		return NET_ERROR_DB_QUERY; 
	}

	// 결과 성공
	// 결과 처리
	//날짜 계산  // 오늘 날짜 확인
	i32sendCount = pDataBase->GetINT32Data(1); 
	UINT32 ui32sendDate	= pDataBase->GetINT32Data(2);
	if( i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD() != ui32sendDate) i32sendCount = 0;
	
	return i32sendCount;
}

BOOL CDataBase::_GetNoteList(INT32 i32ThreadIdx, INT64 i64UID, OUT_NOTE_LIST * pOutList)
{
	CADODatabase *	pDataBase	= m_ppADOdb_MESS[i32ThreadIdx];

	INT32			i32Count = 0;
	INT32			i32ret_return = 0;
	INT32			i32ret_value = 0;
	wchar_t			wstrSQL[SQL_STRING_COUNT];

	INT32			i32AddCount		= 0;
	INT32			i32DeleteCount	= 0;
	INT32			ai32DelteNodeIdx[MAX_NORMAL_NOTE_COUNT];
	
	S2DB::MakeQueryW(wstrSQL, L"usp_Note_sel_All",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);

	if( !pDataBase->ExecuteSP(wstrSQL) )
	{
		ErrorDBMess( wstrSQL, i32ThreadIdx );
		pDataBase	= m_ppADOdb_MESS[i32ThreadIdx];
		return FALSE;
	}

	// iCount가 0이면 자동적으로 Skip
	i32Count = pDataBase->GetRecordCount();
	// 결과 처리
	if (0 < i32Count)
	{
		i32ret_return	= pDataBase->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = pDataBase->GetINT32Data(1);
	
			g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			return FALSE;
		}

		if(i32Count  > MAX_NORMAL_NOTE_COUNT)
		{
			// 클랜 쪽지에 의해 쪽지 갯수가 늘어날 수 있음.
			// 사용자가 삭제하거나, 저장 시간이 지난 후, 특정 메시지의 경우 자동 삭제를 통해 쪽지함을 비울 수 있음
			//
			// TODO : 계속 이런 경우면 점검해야하는데??
			i32Count = MAX_NORMAL_NOTE_COUNT;
		}
	}
	else
	{
		i32Count = 0;
	}
	
	for(INT32 i = 0; i < i32Count; i++)
	{			
		NOTE_NODE * pNote = &pOutList->stNode[i32AddCount];

		pNote->_head._id	= pDataBase->GetINT32Data(1); 
	#ifdef USE_MESS_TEST
		pNote->_head._suid	= pDataBase->GetINT64Data(2);	
	#endif
		//(3) noteId
		pNote->_head._type	= (UINT8) pDataBase->GetINT32Data(4);
		pNote->_head._state	= (UINT8) pDataBase->GetINT32Data(5);

		//기간 확인 
		INT32 deleteDate = pDataBase->GetINT32Data(6);
		pNote->_head._remainDay = GetRemainDay(deleteDate);

		pNote->_head._appendingData		= pDataBase->GetINT32Data(7);
		pNote->_body._senderSize		= (UINT8) MIN(pDataBase->GetINT32Data(8), NET_NICK_NAME_SIZE);
		pNote->_body._messageSize		= (UINT8) MIN(pDataBase->GetINT32Data(9), NET_NOTE_SIZE);
		pDataBase->GetTextData(10, pNote->_body._sender,		pNote->_body._senderSize);

		if(pNote->_head._type == NT_CLAN || pNote->_head._type ==  NT_CLAN_ASK)
			pDataBase->GetTextData(11, pNote->_body._message,	NET_CLAN_REQUEST_SIZE);			//참 말도 되지 않는 코드  입니다.
		else 
			pDataBase->GetTextData(11, pNote->_body._message,	pNote->_body._messageSize);

		// 기간 남으면 목록에 추가 + 읽은 알림쪽지는 제거
		if( (pNote->_head._remainDay != 0) && // 0일이 아니고 
			(pNote->_head._type != NT_NOTICE || pNote->_head._state != NS_READED) )	//
		{
			i32AddCount++;
		}
		else
		{
			ai32DelteNodeIdx[i32DeleteCount] = pNote->_head._id;
			i32DeleteCount++;
		}

		pDataBase->MoveNext();
	}

	//기간 지난 Note 삭제 
	for(INT32 i = 0; i < i32DeleteCount; i++)
	{
		S2DB::MakeQueryW(wstrSQL, L"usp_Note_del",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_INT,			ai32DelteNodeIdx[i],
			DB_DATA_TYPE_END);
		if(!pDataBase->ExecuteSP(wstrSQL))
		{
			ErrorDBMess( wstrSQL, i32ThreadIdx);
			pDataBase	= m_ppADOdb_MESS[i32ThreadIdx];
			continue;
		}
		
		i32Count = pDataBase->GetRecordCount();
		if (0 < i32Count)
		{
			i32ret_return = pDataBase->GetINT32Data(0);
			if (1 != i32ret_return)
			{
				// Error
				i32ret_value = pDataBase->GetINT32Data(1);
				//g_pLog->WriteLog(LOG_DB_IDX, "[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
				continue;
			}
		}
		else
		{
			g_pLog->WriteLog( L"[ERROR][Empty RecordSet][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
			continue;
		}
	}

	//리턴값 정리 
	pOutList->iResult	= EVENT_ERROR_SUCCESS;
	pOutList->iCount	= i32AddCount;

	return TRUE;
}



INT32 CDataBase::_DeleteNote(const INT32 i32ThreadIdx, const INT32 i32ListIdx, OUT_NOTE_INFO * pOutNoteInfo)
{		

	INT32				i32Count = 0;
	INT32				i32ret_return = 0;
	INT32				i32ret_value = 0;
	wchar_t				wstrSQL[SQL_STRING_COUNT];

	S2DB::MakeQueryW(wstrSQL, L"usp_Note_del",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_INT,			i32ListIdx,
		DB_DATA_TYPE_END);
	if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
	{
		pOutNoteInfo->iResult = NET_ERROR_DB_QUERY;
		ErrorDBMess( wstrSQL, i32ThreadIdx);
		return pOutNoteInfo->iResult;
	}

	// 결과 처리 과정
	i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
	if( 0 < i32Count )
	{
		i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
			//g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			pOutNoteInfo->iResult = NET_ERROR_DB_QUERY;
			return pOutNoteInfo->iResult;
		}
	}
	else
	{
		// 결과 처리 Error
		g_pLog->WriteLog( L"[ERROR][Empty RecordSet][GetRecordCount(%d)][%s]", i32Count, wstrSQL  );
		pOutNoteInfo->iResult = NET_ERROR_DB_QUERY;	
		return pOutNoteInfo->iResult;
	}

	// 결과 성공
	// 결과 처리
	pOutNoteInfo->pList[pOutNoteInfo->iCount] = i32ListIdx;
	pOutNoteInfo->iCount++;

	return EVENT_ERROR_SUCCESS;
} // _DeleteNote()

INT32	CDataBase::_DeleteNoteTemp(const INT32 i32ThreadIdx, const INT32 i32TableIDX, const INT32 i32ListIdx, OUT_NOTE_INFO * pOutNoteInfo)
{	

	INT32				i32Count = 0;
	INT32				i32ret_return = 0;
	INT32				i32ret_value = 0;
	wchar_t				wstrSQL[SQL_STRING_COUNT];

	S2DB::MakeQueryW( wstrSQL, L"usp_Note_del",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_INT,			i32ListIdx,
		DB_DATA_TYPE_END);
	if(!m_ppADOdb_MESS[i32ThreadIdx]->ExecuteSP(wstrSQL))
	{
		pOutNoteInfo->iResult = NET_ERROR_DB_QUERY;
		ErrorDBMess( wstrSQL, i32ThreadIdx);
		return pOutNoteInfo->iResult;
	}

	// 결과 처리 과정
	i32Count = m_ppADOdb_MESS[i32ThreadIdx]->GetRecordCount();
	if( 0 < i32Count )
	{
		i32ret_return = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = m_ppADOdb_MESS[i32ThreadIdx]->GetINT32Data(1);
			//g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			pOutNoteInfo->iResult = NET_ERROR_DB_QUERY;

			return pOutNoteInfo->iResult;
		}
	}
	else
	{
		// 결과 처리 Error
		g_pLog->WriteLog( L"[ERROR][Empty RecordSet][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
		pOutNoteInfo->iResult = NET_ERROR_DB_QUERY;
		return pOutNoteInfo->iResult;
	}

	// 결과 성공
	// 결과 처리
	pOutNoteInfo->pList[pOutNoteInfo->iCount] = i32ListIdx;
	pOutNoteInfo->iCount++;

	return EVENT_ERROR_SUCCESS;
} // DeleteNoteTemp()

//----------------------------
//  Note Remain Day
//------------------------------
UINT8
CDataBase::GetRemainDay(INT32 i32deleteDate)
{
	tm deleteTm;
	deleteTm.tm_year	= i32deleteDate / 1000000 - 1900;
	deleteTm.tm_mon		= (i32deleteDate % 1000000) / 10000 - 1;
	deleteTm.tm_mday	= (i32deleteDate % 10000) / 100;
	deleteTm.tm_hour	= i32deleteDate % 100;
	deleteTm.tm_min		= 0;
	deleteTm.tm_sec		= 0;
	deleteTm.tm_isdst	= SUMMER_TIME_FOLLOW_SYSTEM;
	time_t deleteTime = mktime(&deleteTm);
	time_t serverTime;
	time(&serverTime);

	if (deleteTime < serverTime)
	{
		return 0;
	}

	return (UINT8)((deleteTime - serverTime) / (60 * 60 * 24) + 1);
} // GetRemainDay()



//-------------------------------------------------------------------
// Make Packet
//-------------------------------------------------------------------

// NOTE_NODE
BOOL
CDataBase::MakeNoteNode(NOTE_NODE* pNoteNode,	
						INT32	   i32id,
						INT64	   i64suid,
						UINT8	   ui8type,
						UINT8	   ui8state,
						UINT8	   ui8remainDay,
						INT32	   i32appendingData,
						UINT8	   ui8senderSize,
						UINT8	   ui8messageSize,
						TTCHAR *   pstrSender,
						TTCHAR *   pstrMessage,
						void *	   pUser)
{
	if (NULL == pNoteNode)
	{
		return FALSE;
	}

	i3mem::FillZero(pNoteNode, sizeof (NOTE_NODE));
	
	pNoteNode->_head._id = i32id;
#ifdef USE_MESS_TEST
	pNoteNode->_head._suid = i64suid;
#endif
	pNoteNode->_head._type = ui8type;
	pNoteNode->_head._state = ui8state;
	pNoteNode->_head._remainDay = ui8remainDay;
	pNoteNode->_head._appendingData = i32appendingData;

	pNoteNode->_body._senderSize = ui8senderSize;
	pNoteNode->_body._messageSize = ui8messageSize;

	i3mem::Copy(&pNoteNode->_body._sender, pstrSender, sizeof (TTCHAR) * ui8senderSize);
	pNoteNode->_body._sender[ui8senderSize - 1] = '\0';

	if( 0 < ui8messageSize )
	{	// 일반 메시지의 데이터를 보관
		i3mem::Copy(&pNoteNode->_body._message, pstrMessage, sizeof (TTCHAR) * ui8messageSize);
		pNoteNode->_body._message[ui8messageSize - 1] = '\0';
	}
	else
	{	// 클랜 메시지의 코드값과 Sender를 보관
		i3mem::Copy(&pNoteNode->_body._message[0], &pstrMessage[0], sizeof(TTCHAR));
		i3mem::Copy(&pNoteNode->_body._message[1], &pstrMessage[1], sizeof(TTCHAR));
		i3mem::Copy(&pNoteNode->_body._message[2], &pstrMessage[2], sizeof(TTCHAR) * pNoteNode->_body._message[0]);
		pNoteNode->_body._message[pNoteNode->_body._message[0]] = '\0';
	}

	if (NULL != pUser)
	{
		pNoteNode->_body._user = pUser;
	}

	return TRUE;
} // MakeNoteBody()


INT32	CDataBase::_NoteCheckBlock( INT32 i32ThreadIdx, INT64 i64SenderUID, INT64 i64ReceiverUID, BOOL* pbIsBlock )	// 쪽지 보내려는 사람이 받는 사람으로부터 차단 당했는지 체크.
{
	// BOOL bIsBlock = FALSE;	// TRUE : 차단, FALSE : 차단 X.

	CADODatabase *	pDataBase = m_ppADOdb_MESS[i32ThreadIdx];

	INT32	i32Count = 0;
	INT32	i32ret_return = 0;
	INT32	i32ret_value = 0;
 
	wchar_t	wstrSQL[SQL_STRING_COUNT];

	INT32	i32TableIdx = (INT32)(i64ReceiverUID % DB_THREAD_COUNT);	// 중요. 메시지 받는 사람의 테이블 인덱스여야 한다.
			
	S2DB::MakeQueryW( wstrSQL, L"usp_BlockList_sel_BlockCheck",
		DB_DATA_TYPE_INT,			i32TableIdx,
		DB_DATA_TYPE_BIGINT,		i64SenderUID,
		DB_DATA_TYPE_BIGINT,		i64ReceiverUID,
		DB_DATA_TYPE_END);

 	if(! pDataBase->ExecuteSP(wstrSQL) )
	{
		ErrorDBMess(wstrSQL, i32ThreadIdx);
		pDataBase = m_ppADOdb_MESS[i32ThreadIdx];
		return NET_ERROR_DB_QUERY; 
	}

	// 결과 처리 과정
	i32Count = pDataBase->GetRecordCount();
	if( 0 < i32Count )
	{
		// out : ret_return, Isblock

		i32ret_return = pDataBase->GetINT32Data(0);
		if (1 != i32ret_return)
		{
			// Error
			i32ret_value = pDataBase->GetINT32Data(1);
 
			g_pLog->WriteLog( L"[ERROR][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
			return NET_ERROR_DB_QUERY; 
		}
		else 
		{
			INT32 i32IsBlock = pDataBase->GetINT32Data(1);
			if( 1 == i32IsBlock ) *pbIsBlock = TRUE;	// 차단된 경우.
		}
	}
	else
	{
		// 결과 처리 Error
		g_pLog->WriteLog( L"[ERROR][Empty RecordSet][ret_value(%d)][%s]", i32ret_value, wstrSQL  );
		return NET_ERROR_DB_QUERY;
	}

	return EVENT_ERROR_SUCCESS;
}
BOOL	CDataBase::InNoteCommandAddGiftOff(INT8 i8Type, OGame2Mess_Note_Send_Gift_Notice_Req* stGame2Mess_Note_Send_Gift_Notice_Req)
{
	INT32 i32ThreadIdx = (INT32)(stGame2Mess_Note_Send_Gift_Notice_Req->iReceiverUID % DB_THREAD_COUNT);
	IN_NOTE_COMMAND * pNoteCommand = (IN_NOTE_COMMAND *)m_ppI_NoteCommand[i32ThreadIdx]->PushPointer();
	if (pNoteCommand == NULL)return FALSE;

	// 보내는 값이 없는 경우 에러로 처리	
	if (0 == i3String::Length(stGame2Mess_Note_Send_Gift_Notice_Req->SenderNick)) { return FALSE; }

	// 수신자가 없거나 보낼 쪽지 내용이 없으면 에러로 처리
	if (0 >= stGame2Mess_Note_Send_Gift_Notice_Req->iReceiverUID) { return FALSE; }
	if (0 == i3String::Length(stGame2Mess_Note_Send_Gift_Notice_Req->message)) { return FALSE; }

	pNoteCommand->iType = i8Type;
	pNoteCommand->i64UID = stGame2Mess_Note_Send_Gift_Notice_Req->iReceiverUID;

	i3mem::FillZero(pNoteCommand->strNickSend, NET_NICK_NAME_SIZE);
	i3mem::FillZero(pNoteCommand->strMess, NET_NOTE_SIZE);

	
	i3String::NCopy(pNoteCommand->strNickSend, stGame2Mess_Note_Send_Gift_Notice_Req->SenderNick, NET_NICK_NAME_SIZE - 1);
	i3String::NCopy(pNoteCommand->strMess, stGame2Mess_Note_Send_Gift_Notice_Req->message, NET_NOTE_SIZE - 1);
	pNoteCommand->iMessSize = stGame2Mess_Note_Send_Gift_Notice_Req->messageSize;

	//사용하지 않는 변수 세팅 
	pNoteCommand->i64ReceiverUID = stGame2Mess_Note_Send_Gift_Notice_Req->iReceiverUID;
	pNoteCommand->iAppendingData = 0;

	m_ppI_NoteCommand[i32ThreadIdx]->PushPointerIdx();
	return TRUE;

}