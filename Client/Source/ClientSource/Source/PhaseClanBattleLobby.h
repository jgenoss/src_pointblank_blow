#if !defined( _STEP_CLAN_BATTLE_LOBBY_H)
#define _STEP_CLAN_BATTLE_LOBBY_H

#include "StepBase.h"
#include "ClanContext.h"


#define TEAM_MEMBER_SLOT_MAX	8
#define BATTLE_TEAM_SLOT_MAX	14

struct MyTeamMemberSlot
{
	i3GuiStatic * _pSlot;
	i3GuiStatic * _pRank;
	i3GuiEditBox * _pNick;
	i3GuiStatic * _pState;
};

struct BattleTeamSlot
{
	i3GuiStatic * _pSlot;
	i3GuiEditBox * _pNumber;
	i3GuiEditBox * _pClanName;
	i3GuiStatic * _pClanMark;
	i3GuiEditBox * _pTeamName;
	i3GuiStatic * _pRank;
	i3GuiStatic * _pState;
	i3GuiButton * _pMatch;	
};

class CStepClanBattleLobby : public CStepBase
{
	I3_CLASS_DEFINE( CStepClanBattleLobby);

public:
	CStepClanBattleLobby(void);
	virtual ~CStepClanBattleLobby(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnPreGuiNotify(void);
	virtual void OnUpdate(REAL32 rDeltaSeconds);	
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);

	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void OnQuickMatch(void);
	void OnLeave(void);
	void OnTeamListScroll(void);

private:
	CClanContext * m_pClanContext;

	i3GuiStatic * m_pGui;

	//	ÆÀ Á¤º¸ Ã¢
	i3GuiStatic * m_pMyTeamWin;
	i3GuiEditBox * m_pMyTeamWinCaption;
	i3GuiStatic * m_pMyTeamClanMarkImage;
	i3GuiEditBox * m_pMyTeamClanNameStatic;
	i3GuiEditBox * m_pMyTeamClanNameValue;
	i3GuiEditBox * m_pMyTeamClanRankStatic;
	i3GuiEditBox * m_pMyTeamClanRankValue;
	i3GuiStatic * m_pMyTeamClanRankImage;
	i3GuiEditBox * m_pMyTeamNameValue;		
	i3GuiEditBox * m_pMyTeamChatText;
	i3GuiList * m_pMyTeamChatScrollList;
	i3GuiEditBox * m_pMyTeamChatCombo;
	i3GuiButton * m_pMyTeamChatButton;
	i3GuiPopupList * m_pMyTeamChatPopup;
	i3GuiEditBox * m_pMyTeamChatEdit;
	i3GuiStatic * m_pMyTeamChatLang;

	MyTeamMemberSlot m_MyTeamMember[TEAM_MEMBER_SLOT_MAX];

	//	ÆÀ ¸®½ºÆ® Ã¢
	i3GuiStatic * m_pGuiTeamListWin;
	i3GuiEditBox * m_pTeamListCaption;
	
	i3GuiEditBox * m_pTeamSearchStatic;
	i3GuiEditBox * m_pTeamSearchEdit;
	i3GuiButton * m_pTeamSearchButton;
	i3GuiButton * m_pTeamViewAll;

	i3GuiButton * m_pTeamQuckMatch;
	i3GuiButton * m_pTeamLeave;

	i3GuiEditBox * m_pTeamBattleTypeValue;
	i3GuiList * m_pTeamScrollList;
	i3GuiEditBox * m_pListHeadSlotNum;
	i3GuiEditBox * m_pListHeadClanRank;
	i3GuiEditBox * m_pListHeadClanName;
	i3GuiEditBox * m_pListHeadTeam;
	i3GuiEditBox * m_pListHeadState;

	BattleTeamSlot m_BattleTeam[BATTLE_TEAM_SLOT_MAX];

	INT32 m_idxTeamListStartSlot;
	REAL32 m_fUpdateTeamListTime;
	INT32 m_nTeamListSection;
	BOOL m_bUpdateTeamList;

protected:
	void _InitMyTeam(void);
	void _InitTeamList(void);
	void _UpdateMyTeam(REAL32 rDeltaSeconds);
	void _UpdateTeamList(REAL32 rDeltaSeconds);
	void _UpdateClanInfo(REAL32 rDeltaSeconds);
	void _SetMemberSlot(INT32 idx, CLAN_BATTLE_MEMBER * pMember);
	void _SetTeamListSlot(INT32 idx, CLAN_BATTLE_TEAM_INFO * pTeamInfo, BOOL bMaster = FALSE );
};	

#endif // _PHASE_TEAM_LOBBY_H