#pragma once

class GMAuthorityContext : public i3::shared_ginst<GMAuthorityContext>
{
public:
	void SetUserUID(const T_UID uid) { m_UserUID = uid; }
	T_UID GetUserUID() { return m_UserUID; }
	
	void SetUserNickName(const i3::rc_wstring& nickName) { m_wstrUserNickName = nickName; }
	const i3::rc_wstring& GetUserNickName() const { return m_wstrUserNickName; }

	void SetChatMessage(const i3::rc_wstring& msg ) { m_wstrChatMessage = msg; }
	const i3::rc_wstring& GetChatMessage() const { return m_wstrChatMessage; }

//	void SetMessageLenth(const UINT16 lenth) { m_MessageLenth = lenth; }
//	UINT16 GetMessageLenth() { return m_MessageLenth; }

	void SetBlockDate(const UINT32 date) { m_BlockDate = date; }
	UINT32 GetBlockDate() { return m_BlockDate; }

	void SetPenaltyAmount(const INT32 penalty) { m_PenaltyAmount = penalty; }
	INT32 GetPenaltyAmount() { return m_PenaltyAmount; }

	void SetBlockComment(const i3::rc_wstring& blockcomment) { m_wstrBlockComment = blockcomment; }
	const i3::rc_wstring& GetBlockComment() const { return m_wstrBlockComment; }

private:

	T_UID	m_UserUID = 0;

	i3::rc_wstring m_wstrBlockComment;
	i3::rc_wstring m_wstrUserNickName;
	i3::rc_wstring m_wstrChatMessage;

	UINT16	m_MessageLenth = 0;
	UINT32	m_BlockDate = 0;
	INT32	m_PenaltyAmount = 0;
};
 