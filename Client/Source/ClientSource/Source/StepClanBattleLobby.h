#if !defined( _STEP_CLAN_BATTLE_LOBBY_H)
#define _STEP_CLAN_BATTLE_LOBBY_H

#include "StepBase.h"
#include "ClanContext.h"
#include "GameChat_Body.h"

#define TEAM_MEMBER_SLOT_MAX	8
#define BATTLE_TEAM_SLOT_MAX	7

struct MyTeamMemberSlot
{
	i3GuiStatic * _pSlot;
	i3GuiStatic * _pRank;
	i3GuiEditBox * _pNick;
	i3GuiStatic * _pState;
	i3GuiStatic * _pLeader;
};

struct BattleTeamSlot
{
	i3GuiStatic * _pSlot;
	i3GuiEditBox * _pNumber;
	i3GuiEditBox * _pClanName;
	i3GuiStatic * _pClanMark;
	i3GuiEditBox * _pTeamName;
	i3GuiEditBox * _pTeamType;
	i3GuiStatic * _pRank;
	i3GuiStatic * _pState;
	i3GuiButton * _pMatch;
	i3GuiStatic * _pFilter;
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
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);

	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void OnQuickMatch(void);
	void OnTeamMatch(INT32 slot);
	void OnLeave(void);
	void OnTeamListScroll(void);
	void OnSearch(void);
	void OnViewAll(void);
	void OnTeamTypeButton(void);
	void OnTeamTypePopup(INT32 idx);
	void OnTeamFocus(INT32 idx);
	void OnTeamSelect(INT32 idx);
	void OnChatCombo(void);
	void OnChatPopup(INT32 idx);

	void OnSortListByNumber(void);
	void OnSortListByRank(void);
	void OnSortListByName(void);
	void OnSortListByTeam(void);
	void OnSortListByType(void);
	void OnSortListByState(void);

	void CreateBattleRoom(void);

	static INT32 cbSortTeamListByNumber(void * item1, void * item2);
	static INT32 cbSortTeamListByRank(void * item1, void * item2);
	static INT32 cbSortTeamListByName(void * item1, void * item2);
	static INT32 cbSortTeamListByTeam(void * item1, void * item2);
	static INT32 cbSortTeamListByType(void * item1, void * item2);
	static INT32 cbSortTeamListByState(void * item1, void * item2);

	static void cbQueryLeave(void * pThis, INT32 nParam);
	
protected:
	void _InitMyTeam(void);
	void _InitTeamList(void);
	void _UpdateMyTeam(REAL32 rDeltaSeconds);
	void _UpdateTeamList(REAL32 rDeltaSeconds);
	void _UpdateClanInfo(REAL32 rDeltaSeconds);
	void _UpdateSelectClan(REAL32 rDeltaSeconds);
	void _SetMemberSlot(INT32 idx, CLAN_BATTLE_MEMBER * pMember);
	void _SetTeamListSlot(INT32 idx, INT32 nIdx, CLAN_BATTLE_TEAM_INFO * pTeamInfo, BOOL bMaster = FALSE );
	void _CreateTeamListBuffer(INT32 count);
	void _BuildBattleTeamList(void);
	void _BuildBattleTeamList(const char * pszKeyword);
	void _BuildBattleTeamList(CLAN_BATTLE_TEAM_INFO * pTeamInfo, INT32 count);
	void _BuildBattleTeamList(i3List * pList);		

	void _SetSelectClanInfo(CLAN_DETAIL_INFO * pSelect);
	void _SetSelectClanName(const char * pszName);
	void _SetSelectClanStaff(INT32 nNum, INT32 nTotal);
	void _SetSelectClanMaster(const char * pszName, UINT8 nRank);
	void _SetSelectClanTeamLeader(const char * pszName, UINT8 nRank);
	void _SetSelectClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);
	void _SetSelectClanOrder(INT32 nCurOrder, INT32 nPrevOrder);
	void _SetSelectClanAddress(const char * pszAddress);
	void _SetSelectClanExp(UINT32 nCurExp, UINT32 nMaxExp);
	void _SetSelectClanMark(UINT32 nFlag);

public:

	static CLAN_SORT_TYPE m_sTeamSortType;

private:
	CClanContext * m_pClanContext;

	i3GuiStatic * m_pGui;

	//	내 팀 정보 창
	i3GuiStatic * m_pMyTeamWin;
	i3GuiEditBox * m_pMyTeamWinCaption;
	i3GuiStatic * m_pMyTeamClanMarkImage;
	i3GuiEditBox * m_pMyTeamClanNameStatic;
	i3GuiEditBox * m_pMyTeamClanNameValue;
	i3GuiEditBox * m_pMyTeamClanRankStatic;
	i3GuiEditBox * m_pMyTeamClanRankValue;
	i3GuiStatic * m_pMyTeamClanRankImage;
	i3GuiEditBox * m_pMyTeamClanPointStatic;
	i3GuiEditBox * m_pMyTeamClanPointValue;
	i3GuiEditBox * m_pMyTeamNameValue;		
	i3GuiEditBox * m_pMyTeamTypeStatic;
	i3GuiEditBox * m_pMyTeamTypeCombo;
	i3GuiButton * m_pMyTeamTypeComboButton;
	i3GuiPopupList * m_pMyTeamTypeComboPopup;

	MyTeamMemberSlot m_MyTeamMember[TEAM_MEMBER_SLOT_MAX];

	//	팀 리스트 창
	i3GuiStatic * m_pGuiTeamListWin;
	i3GuiEditBox * m_pTeamListCaption;
	
	i3GuiEditBox * m_pTeamSearchStatic;
	i3GuiEditBox * m_pTeamSearchEdit;
	i3GuiButton * m_pTeamSearchButton;
	i3GuiButton * m_pTeamViewAll;

	i3GuiButton * m_pTeamQuickMatch;
	i3GuiButton * m_pTeamLeave;

	i3GuiEditBox * m_pTeamBattleTypeValue;
	i3GuiList * m_pTeamScrollList;
	i3GuiStatic * m_pBattleTeamFocus;
	i3GuiStatic * m_pBattleTeamSelect;
	i3GuiEditBox * m_pListHeadSlotNum;
	i3GuiEditBox * m_pListHeadClanRank;
	i3GuiEditBox * m_pListHeadClanName;
	i3GuiEditBox * m_pListHeadTeam;
	i3GuiEditBox * m_pListHeadType;
	i3GuiEditBox * m_pListHeadState;

	BattleTeamSlot m_BattleTeam[BATTLE_TEAM_SLOT_MAX];

	//	상대 팀 정보
	i3GuiEditBox * m_pSelectClanInfoStatic;

	i3GuiStatic * m_pSelectClanMarkImage;		//	클랜 마크 이미지
	i3GuiStatic * m_pSelectClanMasterRankImage;		//	클랜 마스터 계급 이미지
	i3GuiStatic * m_pSelectClanTeamLeaderRankImage;	//	팀 리더 계급 이미지
	i3GuiStatic * m_pSelectClanRankImage;			//	클랜 계급 이미지
	i3GuiStatic * m_pSelectClanExpBar;				//	클랜 경험치 게이지
	i3GuiStatic * m_pSelectClanAddressBullet;

	i3GuiEditBox * m_pSelectClanNameStatic;			//	클랜명
	i3GuiEditBox * m_pSelectClanNameValue;
	i3GuiEditBox * m_pSelectClanTeamLeaderStatic;	//	팀 리더
	i3GuiEditBox * m_pSelectClanTeamLeaderValue;
	i3GuiEditBox * m_pSelectClanStaffStatic;		//	클랜 인원수
	i3GuiEditBox * m_pSelectClanStaffValue;
	i3GuiEditBox * m_pSelectClanMasterStatic;		//	클랜 마스터
	i3GuiEditBox * m_pSelectClanMasterValue;
	i3GuiEditBox * m_pSelectClanRankStatic;			//	클랜 등급
	i3GuiEditBox * m_pSelectClanRankValue;
	i3GuiEditBox * m_pSelectClanExpStatic;			//	클랜 경험치
	i3GuiEditBox * m_pSelectClanExpValue;
	i3GuiEditBox * m_pSelectClanOrderStatic;		//	클랜 순위
	i3GuiEditBox * m_pSelectClanOrderValue;
	i3GuiEditBox * m_pSelectClanAddressStatic;		//	클랜 아지트
	i3GuiEditBox * m_pSelectClanAddressValue;		

	i3BinList m_BattleTeamList;
	CLAN_BATTLE_TEAM_INFO * m_pBattleTeamInfo;
	INT32 m_nBattleTeamInfoCount;

	INT32 m_idxBattleTeamMatchType;
	INT32 m_idxTeamListStartSlot;
	INT32 m_nMatchTeamSlot;
	INT32 m_nSelectTeamSlot;
	REAL32 m_fUpdateTeamListTime;
	INT32 m_nTeamListSection;
	BOOL m_bUpdateTeamList;

	char m_szSearchKeyword[MAX_STRING_COUNT];
	
};	

#endif // _PHASE_TEAM_LOBBY_H