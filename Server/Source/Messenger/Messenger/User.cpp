#include "pch.h"
#include "User.h"
#include "NoteBox.h"

I3_CLASS_INSTANCE(CUser, i3ElementBase);

CUser::CUser()
{
	m_noteBox = NULL;
}

CUser::~CUser()
{
	// Do nothing
}

void CUser::Init(UINT32 userId, INT32 sessionIdx, INT32 serverIdx, const char* nickname, CNoteBox* noteBox, UINT32 nodeIdx)
{
	//
	m_user._userId		= userId;
	m_user._sessionIdx	= sessionIdx;
	m_user._serverIdx	= serverIdx;
	
	//
	m_user._nicknameSize = (UINT8)(i3String::Length(nickname) + 1);
	m_user._nicknameSize = MIN(m_user._nicknameSize, NET_NICK_NAME_SIZE);
	i3mem::Copy(m_user._nickname, (void*)nickname, m_user._nicknameSize);	
	m_user._nickname[m_user._nicknameSize - 1] = '\0';

	//
	m_user._serverLeaveTime = 0;
	m_user._nodeIdx = nodeIdx;

	// NoteBox
	m_noteBox = noteBox;

	return; 
}

void CUser::LeaveUser(UINT32 serverLeaveTime)
{
	m_user._serverLeaveTime = serverLeaveTime;
	return; 
}

void CUser::EnterUser(INT32 sessionIdx,INT32 serverIdx)
{
	m_user._sessionIdx		= sessionIdx;
	m_user._serverIdx		= serverIdx;
	m_user._serverLeaveTime	= 0;
	return; 
}

void CUser::ChangeNickname(const char* nickname)
{
	m_user._nicknameSize = (UINT8)(i3String::Length(nickname) + 1);
	m_user._nicknameSize = MIN(m_user._nicknameSize, NET_NICK_NAME_SIZE);
	i3mem::Copy(m_user._nickname, (void*)nickname, m_user._nicknameSize);	
	m_user._nickname[m_user._nicknameSize - 1] = '\0';
}

void CUser::ChangeRank(UINT32 rank)
{
	// Do nothing
}

UINT32 CUser::GetUserId(void) const
{
	return m_user._userId;
}

void CUser::SetUserId(UINT32 userId)
{
	m_user._userId = userId;
}

UINT32 CUser::GetNodeIdx(void) const
{
	return m_user._nodeIdx;
}

INT32 CUser::GetSessionIdx(void) const
{
	return m_user._sessionIdx;
}

INT32 CUser::GetServerIdx(void) const
{
	return m_user._serverIdx;
}

UINT8 CUser::GetNicknameSize(void) const
{
	return m_user._nicknameSize;
}

const char* CUser::GetNickname(void) const
{
	return m_user._nickname;
}

BOOL CUser::AddNote(INT32 id)
{
	if (m_noteBox)
	{
		return m_noteBox->Add(id);
	}

	return FALSE;
}

BOOL CUser::CheckReadedNote(INT32 id)
{
	if (m_noteBox)
	{
		return m_noteBox->CheckReaded(id);
	}

	return FALSE;
}

BOOL CUser::DeleteNote(INT32 id)
{
	if (m_noteBox)
	{
		return m_noteBox->Delete(id);
	}

	return FALSE;
}
