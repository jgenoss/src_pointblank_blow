#include "pch.h"
#include "ConnectedUser.h"
#include "User.h"
#include "NoteBox.h"
#include "DataBase.h"

I3_CLASS_INSTANCE(CConnectedUser, i3ElementBase);

CConnectedUser* g_pConnectedUser = NULL;

CConnectedUser::CConnectedUser()
{
	//m_user = NULL;
	//m_noteBox = NULL;
	//m_count = 0;
	//
	//m_sendBufferCount = 0;
	//m_ClanAskErrCode = EVENT_ERROR_FAIL;
}

CConnectedUser::~CConnectedUser()
{
	//// 유저 릴리즈
	//if (m_user)
	//{
	//	for(INT32 i = 0; i < MAX_CONNECT_USER_COUNT; i++)
	//	{
	//		I3_SAFE_RELEASE(m_user[i]);
	//	}

	//	I3MEM_SAFE_FREE(m_user);
	//}

	//// 쪽지함 릴리즈
	//if (m_noteBox)
	//{
	//	for(INT32 i = 0; i < MAX_CONNECT_USER_COUNT; i++)
	//	{
	//		I3_SAFE_RELEASE(m_noteBox[i]);
	//	}

	//	I3MEM_SAFE_FREE(m_noteBox);
	//}
}

/*
BOOL CConnectedUser::Create(void)
{
	// 유저정보
	m_user = (CUser**)i3MemAlloc(sizeof(CUser*) * MAX_CONNECT_USER_COUNT);
	
	if (NULL == m_user)
	{
		return FALSE;
	}

	for(INT32 i = 0; i < MAX_CONNECT_USER_COUNT; i++)
	{
		m_user[i] = CUser::NewObject();
		
		if(NULL == m_user[i])
		{
			return FALSE;
		}
	}

	// 쪽지함
	m_noteBox = (CNoteBox**)i3MemAlloc(sizeof(CNoteBox*) * MAX_CONNECT_USER_COUNT);

	if (NULL == m_noteBox)
	{
		return FALSE;
	}
	
	for(INT32 i = 0; i < MAX_CONNECT_USER_COUNT; i++)
	{
		m_noteBox[i] = CNoteBox::NewObject();

		if (NULL == m_noteBox[i])
		{
			return FALSE;
		}
	}

	// userId 해시 테이블
	if (!m_userHashTable.Create())
	{
		return FALSE;
	}
	
	// 유저정보 빈 노드 찾기용 핼퍼클래스
	if (!m_emptyBuffer.Create(MAX_CONNECT_USER_COUNT))
	{
		return FALSE;
	}

	for(UINT32 i = 0; i < MAX_CONNECT_USER_COUNT; i++)
	{
		m_emptyBuffer.Push(i, FALSE);
	}

	return TRUE;
}
*/

/*
BOOL CConnectedUser::Login(UINT32 userId,const char* nickname,INT32 sessionIdx,INT32 serverIdx)
{
	if (m_count < MAX_CONNECT_USER_COUNT)
	{
		UINT32 emptyIdx;

		// 동접 여유가 있다면,
		if(m_emptyBuffer.Pop(&emptyIdx))
		{
			// 초기화
			m_noteBox[emptyIdx]->Init();
			m_user[emptyIdx]->Init(userId, sessionIdx, serverIdx, nickname, m_noteBox[emptyIdx], emptyIdx);

			// 해시 테이블에 등록
			m_userHashTable.Insert(m_user[emptyIdx]);
			m_count++;

			// 메신저 Info 테이블 생성 - 있으면 무시
			g_pDataBase->CreateNewAccountForMessenger(userId);

            // DB 기본 알림 쓰기
			if (nickname[0] == '\0')
			{
				INT32 nextNoteId;
				
				if (g_pDataBase->GetNextNoteId(userId, &nextNoteId))
				{
					NOTE_HEADER head;
					NOTE_BODY body;

					head._id			= nextNoteId;
					head._type			= NT_NOTICE;
					head._state			= NS_NEW;
					head._remainDay		= REMAIN_DAY_FOR_UNREAD_NOTE;
					head._appendingData = 0;
					body._sender[0]		= '\0';
					//태국은 메신저 서버 연동 확인해야함 
					//러시아 메신저 서버 연동 확인해야함
					//i3String::Copy(body._message, "알림: 쪽지 사용안내\n쪽지함을 이용해 쪽지를 주고 받거나 선물을 받으실 수 있습니다.\n읽은 쪽지는 7일, 읽지 않은 쪽지는 15일 후 자동으로 삭제됩니다.\n쪽지의 최대 보관개수는 100개이며 하루에 보낼 수 있는 쪽지도 100개로 제한됩니다.");
					i3String::Copy(body._message, "Notice:Panduan Inbox\nBisa mengirim, menerima pesan serta menerima gift di dalam Inbox\nPesan yang sudah dibaca akan dihapus setelah 7 hari, pesan belum dibaca 15 hari.\nMaksimal menyimpan 100 pesan dan mengirim maksimal 100 pesan sehari.");
					body._senderSize = (UINT8) (i3String::Length(body._sender) + 1);
					body._messageSize = (UINT8) (i3String::Length(body._message) + 1);

					g_pDataBase->InsertNote(userId, &head, &body);
				}
			}

			// DB 읽기
			ClearSendBuffer();
			g_pDataBase->ReadNoteBox(userId, this, m_noteBox[emptyIdx]);

			return TRUE;
		}
		else 
		{
			return FALSE; 
		}
	}

	return FALSE;
}
*/

/*
void CConnectedUser::Logout(UINT32 userId)
{
	if (0 < m_count)
	{
		CUser* user;

		// 동접 목록에 있다면, 
		if (m_userHashTable.Retrieve(userId, &user))
		{
			// 해시 제거
			m_userHashTable.Delete(userId);

			// 사용한 영역 반납
			m_emptyBuffer.Push(user->GetNodeIdx());
			m_count--;
		}
	}
}
*/

/*
void CConnectedUser::EnterUser(UINT32 userId,INT32 sessionIdx,INT32 serverIdx)
{
	if (0 < m_count)
	{
		CUser* user;
		if (m_userHashTable.Retrieve(userId, &user))
		{
			user->EnterUser(sessionIdx, serverIdx);			
		}
	}
}

void CConnectedUser::LeaveUser(UINT32 userId)
{
	if (0 < m_count)
	{
		CUser* user;
		if (m_userHashTable.Retrieve(userId, &user))
		{
			user->LeaveUser(g_pConfig->GetServerTime());
		}
	}
}
*/

/*
BOOL CConnectedUser::ChangeNickname(UINT32 userId,const char* nickname)
{
	CUser* user;

	if (m_userHashTable.Retrieve(userId, &user))
	{
		user->ChangeNickname(nickname);

		return TRUE;
	}

	return FALSE;
}
*/

/*
BOOL CConnectedUser::ChangeRand(UINT32 userId,UINT32 rank)
{
	CUser* user;

	if (m_userHashTable.Retrieve(userId, &user))
	{
		user->ChangeRank(rank);

		return TRUE;
	}

	return FALSE;
}
*/

/*
BOOL CConnectedUser::GetUserInfo(UINT32 userId,INT32* sessionIdx,INT32* serverIdx)
{
	CUser* user;

	if (m_userHashTable.Retrieve(userId, &user))
	{
		*sessionIdx = user->GetSessionIdx();
		*serverIdx = user->GetServerIdx();

		return TRUE;
	}

	return FALSE;
}
*/

/*
BOOL CConnectedUser::SendNote(UINT32 senderUserId,UINT32 receiverUserId,UINT8 messageSize,const char* message,NOTE_HEADER* noteHeader,NOTE_BODY* noteBody)
{
	return _SendNote(senderUserId, receiverUserId, messageSize, message, noteHeader, noteBody, NT_NORMAL, 0);
}

BOOL CConnectedUser::SendClanNote(UINT32 senderUserId,UINT32 receiverUserId,UINT8 messageSize,const char* message,NOTE_HEADER* noteHeader,NOTE_BODY* noteBody,UINT32 clanIdx)
{
	return _SendNote(senderUserId, receiverUserId, messageSize, message, noteHeader, noteBody, NT_CLAN, (INT32)clanIdx);
}

BOOL CConnectedUser::SendClanAskNote(UINT32 senderUserId,UINT32 receiverUserId,UINT8 messageSize,const char* message,NOTE_HEADER* noteHeader,NOTE_BODY* noteBody,UINT32 clanIdx )
{
	return _SendNote(senderUserId, receiverUserId, messageSize, message, noteHeader, noteBody, NT_CLAN_ASK, (INT32)clanIdx);
}

BOOL CConnectedUser::_SendNote(UINT32 senderUserId,UINT32 receiverUserId,UINT8 messageSize,const char* message,NOTE_HEADER* noteHeader,NOTE_BODY* noteBody,UINT8 noteType,INT32 appendingData )
{
	CUser* sender;

	if (m_userHashTable.Retrieve(senderUserId, &sender))
	{
		// 쪽지 준비
		NOTE_NODE note;

		if (!g_pDataBase->GetNextNoteId(receiverUserId, &note._head._id))
		{
			m_ClanAskErrCode = EVENT_ERROR_FAIL;
			return FALSE;
		}

		note._head._type			= noteType;
		note._head._state			= NS_NEW;
		note._head._remainDay		= REMAIN_DAY_FOR_UNREAD_NOTE;
		note._head._appendingData	= appendingData;

		// 보내는 사람란에, 클랜명이 들어가지 않는 경우 보낸사람 닉네임 설정
		// 클랜명은 프로토콜 처리단에서 미리 설정해놓았다.
		if( (noteType != NT_CLAN && noteType != NT_CLAN_ASK) || appendingData == 0 )
		{
			note._body._senderSize = sender->GetNicknameSize();
			i3mem::Copy(note._body._sender, (void*)sender->GetNickname(), note._body._senderSize);
		}
		else
		{
			note._body._senderSize = noteBody->_senderSize;
			i3mem::Copy(note._body._sender, noteBody->_sender, note._body._senderSize);
		}
		
		note._body._messageSize = messageSize;

		if( 0 < note._body._messageSize )
		{
            i3mem::Copy(note._body._message, (void*)message, note._body._messageSize);
		}
		else
		{
			i3mem::Copy(note._body._message, (void*)message, sizeof(char) + message[0] );
		}

		// DB 기록
		if (!g_pDataBase->InsertNote(receiverUserId, &note._head, &note._body))
		{
			m_ClanAskErrCode = EVENT_ERROR_FAIL;
			return FALSE;
		}


		// 클랜쪽지는 전송제한 없다.
		if (noteType != NT_CLAN && noteType != NT_CLAN_ASK)
		{
			// 전송제한 수치 증가
			// ! 성공유무 검사하지 않는다. 트레이드오프: 전송실패비용 대신 전송제한오차(+1)
			g_pDataBase->IncreaseSendLimit(senderUserId);
		}


		// 수신자가 접속 중이면 직접 발송
		CUser* receiver;

		if (m_userHashTable.Retrieve(receiverUserId, &receiver))
		{
			if (!receiver->AddNote(note._head._id))
			{
				m_ClanAskErrCode = EVENT_ERROR_NOTE_SEND_BOX_FULL;
				return FALSE;
			}
		}

		// 전송용
		*noteHeader = note._head;
		*noteBody = note._body;		
		
		return TRUE;
	}

	return FALSE;
}

/*
BOOL CConnectedUser::CheckReadedNote(UINT32 userId,UINT8 readCount,INT32* noteIdList,UINT8* readedCount,INT32* readedNoteIdList)
{
	CUser* user;

	if (m_userHashTable.Retrieve(userId, &user))
	{
		*readedCount = 0;

		for(INT32 i = 0; i < readCount; i++)
		{
			if (user->CheckReadedNote(noteIdList[i]))
			{
				g_pDataBase->CheckReadedNote(userId, noteIdList[i]);

				readedNoteIdList[*readedCount] = noteIdList[i];
				(*readedCount)++;
			}
		}

		return TRUE;
	}

	return FALSE;
}
*/
/*
BOOL CConnectedUser::DeleteNote(UINT32 userId,UINT8 deleteCount,INT32* noteIdList,UINT8* deletedCount,INT32* deletedNoteIdList)
{
	CUser* user;

	if (m_userHashTable.Retrieve(userId, &user))
	{
		*deletedCount = 0;

		for(INT32 i = 0; i < deleteCount; i++)
		{
			if (user->DeleteNote(noteIdList[i]))
			{
				g_pDataBase->DeleteNote(userId, noteIdList[i]);

				deletedNoteIdList[*deletedCount] = noteIdList[i];
				(*deletedCount)++;
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CConnectedUser::ClearSendBuffer(void)
{
	m_sendBufferCount = 0;
}

BOOL CConnectedUser::AddSendBuffer(const NOTE_NODE* note)
{
	if (m_sendBufferCount < MAX_NORMAL_NOTE_COUNT)
	{
        m_sendBuffer[m_sendBufferCount] = *note;
		m_sendBufferCount++;

		return TRUE;
	}

	return FALSE;
}

NOTE_NODE* CConnectedUser::GetSendBuffer(void)
{
	return m_sendBuffer;
}

INT32 CConnectedUser::GetSendBufferCount(void)
{
	return m_sendBufferCount;
}

*/
