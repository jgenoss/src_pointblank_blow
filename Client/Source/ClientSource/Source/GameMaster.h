#pragma once

#define		COMMAND_USER_KICK		L"/KICK"
#define		COMMAND_USER_EXIT		L"/EXIT"
#define		COMMAND_USER_BLOCK		L"/BLOCK"
#define		COMMAND_ROOM_DESTROY	L"/ROOMDEST"
#define		COMMAND_CHEAT_RANDOMBOX L"/RANDOMBOX"

#define		COMMAND_ARG_TEAM_BLUE	L"blue"
#define		COMMAND_ARG_TEAM_RED	L"red"

enum GM_COMMAND_TYPE
{
	GM_COMMAND_UNKNOWN	= 0,

	GM_COMMAND_USER_KICK,
	GM_COMMAND_USER_EXIT,
	GM_COMMAND_USER_BLOCK,
	GM_COMMAND_ROOM_DESTROY,

	GM_COMMAND_CHEAT_RANDOMBOX,

	MAX_GM_COMMAND,
};

class CGameMaster : public i3::shared_ginst<CGameMaster>
{
public:
	CGameMaster();
	~CGameMaster();

	void				SetCameraTopPlayer(const TEAM_TYPE eTeamType);

	bool				SetGMCommand(const i3::wliteral_range& wCommandRng);
	bool				SetGMLobbyCommand(const i3::wliteral_range& wCommandRng);
	GM_COMMAND_TYPE		GetCommandType(const i3::wliteral_range& wCommandRng);
	INT32				GetCommandTypeArg(GM_COMMAND_TYPE Type, const i3::wliteral_range& wCommandRng );
	bool				GetCommandTypeArgList(GM_COMMAND_TYPE Type, const i3::wliteral_range& wCommandRng, INT32 * pOutArgArr);
	INT32				GetCommandTypeSessionIdx(GM_COMMAND_TYPE Type, const i3::wliteral_range& wCommandRng, i3::rc_wstring& out_wstrNickName );
	INT32				GetCommandArgLength(INT8 ArgIndex, const i3::wliteral_range& wCommandRng, INT32 * ArgStartPtr = nullptr);

	bool				IsPaused(void) const	{ return m_bIsPaused; }
	void				SetPause(bool bPause);	


private:

	bool				m_bIsPaused;


};