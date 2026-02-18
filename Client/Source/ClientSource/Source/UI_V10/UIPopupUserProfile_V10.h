#pragma once

#include "../UI/UIPopupBase.h"

class UIPopupUserProfile_V10 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserProfile_V10, UIPopupBase);

	enum 
	{
		STATIC_SCORE = 0,
		STATIC_KILLDEATH,
		STATIC_HEADSHOT,

		STATIC_TEXT_TOTAL,
	};

	enum
	{
		BUTTONSTATE_NONE = 0,
		BUTTONSTATE_USERLIST,
		BUTTONSTATE_FIREND,
		BUTTONSTATE_SEARCHUSER,
		BUTTONSTATE_CLAN,
		
		BUTTONSTATE_MAX,
	};

public:
	UIPopupUserProfile_V10();
	virtual ~UIPopupUserProfile_V10();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 INT32*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

public:
	void			Close();
	void			Accpet();
	void			VIsit();
	void			Invite();
	void			Delete();
	void			Refuse();
	void			AddFriend();
	void			TabClick(INT32 idx);

	INT32			GetViewFriend() { return ViewFriendIdx_; }

private:
	// 버튼 설정
	void			_SetButton(INT32 idx);
	
	void			__SetButtonUserList();
	void			__SetButtonFriendList();
	void			__SetButtonSearchUser();
	void			__SetButtonClanList();
	void			__SetButtonDefault();
	
	UINT32			__GetIsFriendState();
	INT32			__GetViewFriendIdx();

	UINT64			__GetIsClanState();
	INT32			__GetViewClanIdx();

	void			__SetRoomPosition();

	// Info 텍스트 설정
	void			_SetInfoText();
	void			__SetSeasonRecord(const USER_INFO_RECORD * record);
	void			__SetTotalRecord(const USER_INFO_RECORD * record);

	//------------------------------------------------------------------------------//
	//								Callback rutine									//
	//------------------------------------------------------------------------------//
	static void		CbDeleteFriend(void* pThis,INT32 nParam);

private:
	i3::vector<i3UIStaticText *> vecSeason;
	i3::vector<i3UIStaticText *> vecTotal;

	i3UIStaticText *			 text_NickName_;
	i3UIStaticText *			 text_Position_;
	i3UIStaticText *			 static_textPosition_;
	i3UIStaticText *			 static_Season_;
	i3UIStaticText *			 static_Total_;

	INT32						 Buttonidx_;
	INT32						 ViewFriendIdx_;
	INT32						 Clantabidx_;

	i3UIButtonComposed3 *		 btn_Accpet_;
	i3UIButtonComposed3 *		 btn_Visit_;
	i3UIButtonComposed3 *		 btn_Invite_;
	i3UIButtonComposed3 *		 btn_Delete_;
	i3UIButtonComposed3 *		 btn_Refuse_;
	i3UIButtonComposed3 *		 btn_AddFriend_;

	i3UITab *					 tab_Clan_;
	i3UITab *					 tab_Person_;

	VEC3D						 addFriendBasePos_;

	bool						 basicInfo_;
	bool						 stateInfo_;
	bool						 m_bClanMercenary;
};