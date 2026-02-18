#pragma once

#include "../UI/UIFloatingBase.h"

#define MAX_ANITIME 0.5

class UIFloatReadyRoomUserInfo : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatReadyRoomUserInfo, UIFloatingBase);

	enum STATIC_TEXT_RECORD
	{
		STATIC_RECORD = 0,
		STATIC_KILLRATE,
		STATIC_KILLDEATH,
		STATIC_HEADSHOT,
		STATIC_LEAVE,

		STATIC_TEXT_MAX,
	};
public:
	UIFloatReadyRoomUserInfo();
	virtual ~UIFloatReadyRoomUserInfo();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart( void) override;

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	void			SetMyInfo() { _SetMyInfo(); }

	void			ClickButton();
	void			SetSelectedIdx(INT32 Idx) { m_SelectedSlot = Idx; }

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	OnLoadAllScenes() override;

	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

private:
	void			_SetMyInfo();
	void			_SetUserInfo();

	void			_SetClanMarkIcon(const USER_INFO_BASIC * info_basic, bool iconEnable);

	void			_SetOpenCloseisFrame();
	void			_SetUserClassImage(INT32 _class);

	void			_SetSeasonRecord(const USER_INFO_RECORD * record);
	void			_SetClanMatchRecord(const CLAN_PERSONAL_MATCH_RECORD * record);
	void			_SetTotalRecord(const USER_INFO_RECORD * record);

private:
	i3UIStaticText * static_TextTitle_;
	bool			 enable_OpenClose_;

	i3UIStaticText * text_Nickname_;
	i3UIStaticText * text_Clanname_;
	i3UIStaticText * text_Clanlevel_;

	i3UIButtonImageSet * image_Class_;

	i3UIImageBoxEx *	 image_ClanMark_;
	i3UIButtonImageSet * button_ClanRankIcon_;

	// Season Text Lua를 쓰지 않고 코드를 사용하기 위함.
	i3::vector<i3UIStaticText*> vec_Season_;

	// Total Text
	i3::vector<i3UIStaticText*> vec_Total_;

	i3UIFrameWnd *  frame_MainInfo_;
	i3UIFrameWnd *  frame_HideInfo_;
	VEC3D			m_ControlPos[2];

	INT32			m_SelectedSlot;
};