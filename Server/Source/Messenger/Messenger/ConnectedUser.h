#ifndef _ConnectedUser_h
#define _ConnectedUser_h

#include "UserHash.h"
#include "CircularIndex.h"

class CUser;
class CNoteBox;

class CConnectedUser: public i3ElementBase
{
	I3_CLASS_DEFINE(CConnectedUser);

private:
	/*
	CUser		**	m_user;
	CNoteBox	**	m_noteBox;
	INT32			m_count;

	CUserHash		m_userHashTable;
	CCircularIndex	m_emptyBuffer;

	NOTE_NODE		m_sendBuffer[MAX_NORMAL_NOTE_COUNT];
	INT32			m_sendBufferCount;

	INT32			m_ClanAskErrCode;
	*/

private:
//	BOOL _SendNote(UINT32 senderUserId,UINT32 receiverUserId,UINT8 messageSize,const char* message,NOTE_HEADER* noteHeader,NOTE_BODY* noteBody,UINT8 noteType,INT32 appendingData);

public:
	CConnectedUser();
	virtual ~CConnectedUser();

	////BOOL Create(void);

	////BOOL Login(UINT32 userId,const char* nickname,INT32 sessionIdx,INT32 serverIdx);
	////void Logout(UINT32 userId);
	////void EnterUser(UINT32 userId,INT32 sessionIdx,INT32 serverIdx);
	////void LeaveUser(UINT32 userId);
	////BOOL ChangeNickname(UINT32 userId,const char* nickname);
	////BOOL ChangeRand(UINT32 userId,UINT32 rank);


	////BOOL GetUserInfo(UINT32 userId,INT32* sessionIdx,INT32* serverIdx);	

	//BOOL SendNote(UINT32 senderUserId,UINT32 receiverUserId,UINT8 messageSize,const char* message,NOTE_HEADER* noteHeader,NOTE_BODY* noteBody);
	////BOOL CheckReadedNote(UINT32 userId,UINT8 readCount,INT32* noteIdList,UINT8* readedCount,INT32* readedNoteIdList);
	////BOOL DeleteNote(UINT32 userId,UINT8 deleteCount,INT32* noteIdList,UINT8* deletedCount,INT32* deletedNoteIdList);

	//void		ClearSendBuffer(void);
	//BOOL		AddSendBuffer(const NOTE_NODE* note);
	//NOTE_NODE*	GetSendBuffer(void);
	//INT32		GetSendBufferCount(void);

	////클랜 초대 
	////클랜 수락 
	////클랜 탈퇴
	//BOOL SendClanNote(UINT32 senderUserId, UINT32 receiverUserId, UINT8 messageSize, const char* message, NOTE_HEADER* noteHeader, NOTE_BODY* noteBody, UINT32 clanIdx);
	//BOOL SendClanAskNote(UINT32 senderUserId, UINT32 receiverUserId, UINT8 messageSize, const char* message, NOTE_HEADER* noteHeader, NOTE_BODY* noteBody, UINT32 clanIdx);
	//INT32 getClanAskErrCode(void)					{ return m_ClanAskErrCode;		}
};

extern CConnectedUser* g_pConnectedUser;

#endif
