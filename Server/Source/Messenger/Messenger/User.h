#ifndef _User_h
#define _User_h

#include "ContentDef.h"

class CNoteBox;

class CUser: public i3ElementBase
{
	I3_CLASS_DEFINE(CUser);
private:
	USER_NODE1		m_user;
	CNoteBox	*	m_noteBox;

public:
	CUser();
	virtual ~CUser();
	
	void Init(UINT32 userId,INT32 sessionIdx,INT32 serverIdx,const char* nickname,CNoteBox* noteBox,UINT32 nodeIdx);
	void LeaveUser(UINT32 serverLeaveTime);
	void EnterUser(INT32 sessionIdx, INT32 serverIdx);
	void ChangeNickname(const char* nickname);
	void ChangeRank(UINT32 rank);
	
	UINT32 GetUserId(void) const;
	void SetUserId(UINT32 userId);
	UINT32 GetNodeIdx(void) const;
	INT32 GetSessionIdx(void) const;
	INT32 GetServerIdx(void) const;
	UINT8 GetNicknameSize(void) const;
	const char* GetNickname(void) const;

	BOOL AddNote(INT32 id);
	BOOL CheckReadedNote(INT32 id);
	BOOL DeleteNote(INT32 id);	    
};

#endif
