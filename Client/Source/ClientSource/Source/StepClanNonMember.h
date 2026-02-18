#if !defined( _STEP_CLAN_NON_MEMBER_H__)
#define _STEP_CLAN_NON_MEMBER_H__

#include "StepBase.h"
#include "StageDef.h"
#include "ClanContext.h"

typedef struct _tagClanInfoSlot
{
	i3GuiStatic	*	_slot;
	i3GuiStatic	*	_pRank;
	i3GuiStatic	*	_ClanMarkImage;
	i3GuiEditBox*	_ClanName;
	i3GuiEditBox*	_ClanEntryDay;
	i3GuiEditBox*	_ClanMember;
}CLAN_NMB_INFO;


#define	CLANFIND_LIST_ROWMAX 16

class CStepClanNonMember : public CStepBase
{
	I3_CLASS_DEFINE(CStepClanNonMember);

protected:
	CClanContext	*	m_pClanContext;

	i3GuiStatic		*	m_pClanNonMemberRoot;

	// 클랜 정보
	i3GuiStatic		*	m_pClanSubcribeWin;				//	미가입자/클랜정보 창
	i3GuiEditBox	*	m_pClanNonMemberCaption;		//	'클랜 찾기'
	i3GuiEditBox	*	m_pClanNonMembeSerchText;		//	클랜찾기 진행 상태 텍스트

	i3GuiStatic		*	m_pClanSerchBar;				//	클랜찾기 프로그래스바 
	i3GuiStatic		*	m_pClanSerchGage;				//	클랜찾기 프로그래스바 게이지
	i3GuiEditBox	*	m_pClanSerchText;				//	클랜찾기 프로그래스바 텍스트

	i3GuiButton		*	m_pClanSubscription;			// 클랜 가입
	i3GuiButton		*	m_pClanCreate;					// 클랜 생성

	//
	i3GuiStatic		*	m_pClanMark;					// 클랜마크
	i3GuiStatic		*	m_pClanMarkFrame;				// 클랜마크 프레임
	
	i3GuiEditBox	*	m_pClanNameStatic;				// 클랜명 :
	i3GuiEditBox	*	m_pClanNameValue;				// 클랜명

	i3GuiEditBox	*	m_pClanStaffStatic;					// 인원수
	i3GuiEditBox	*	m_pClanStaffValue;

	i3GuiEditBox	*	m_pClanMasterStatic;				// 클랜 마스터 :
	i3GuiEditBox	*	m_pClanMasterValue;					// 클랜장
	i3GuiStatic		*	m_pClanMasterRankImage;				// 클랜 마스터의 등급 이미지 표시

	i3GuiEditBox	*	m_pClanRankStatic;					// 클랜 순위 :
	i3GuiEditBox	*	m_pClanRankValue;					// 100등
	i3GuiStatic		*	m_pClanRankImage;					// 클랜의 등급 이미지 표시

	i3GuiEditBox	*	m_pClanExpStatic;					// 클랜 경험치
	i3GuiEditBox	*	m_pClanExpValue;					//

	i3GuiEditBox	*	m_pClanOrderStatic;					// 클랜 순위
	i3GuiEditBox	*	m_pClanOrderValue;			

	i3GuiEditBox	*	m_pClanExpBarFrame;					// 클랜 경험치 바 프레임
	i3GuiStatic		*	m_pClanExpBar;

	i3GuiEditBox	*	m_pClanAddressStatic;				// 클랜 아지트 주소 URL
	i3GuiEditBox	*	m_pClanAddressValue;				// 클랜 아지트 주소 URL
	
	i3GuiStatic		*	m_pClanIntroduceFrame;				// 클랜 소개 바탕
	i3GuiEditBox	*	m_pClanIntroduceValue;				// 클랜 소개 글

	i3GuiStatic		*	m_pClanSearchBullet;		
	i3GuiEditBox	*	m_pClanSearchStatic;				//	클랜명 검색
	i3GuiEditBox	*	m_pClanSearchEdit;
	i3GuiButton		*	m_pClanSearchButton;

	// 클랜
	i3GuiStatic		*	m_pClanListWin;						//	클랜정보 창
	i3GuiEditBox	*	m_pClanListCaption;					//	'클랜'
	
	i3GuiButton		*	m_pClanListTab;						// 클랜 리스트 버튼
	i3GuiButton		*	m_pUserListTab;						// 유저 리스트 버튼
	i3GuiButton		*	m_pViewAllButton;					// 전체보기

	i3GuiStatic		*	m_pClanListPhase;					// 클랜리스트 페이지
	i3GuiStatic		*	m_pUserListPhase;					// 유저 리스트 페이지

	i3GuiList		*	m_pClanListScrollList;				// 클랜 리스트 스크롤 
	i3GuiList		*	m_pUserListScrollList;				// 유저 리스트 스크롤
	CLAN_NMB_INFO		m_ClanInfo[CLANFIND_LIST_ROWMAX];	//클랜 리스트내 각 슬롯의 정보

	// 클랜 리스트 헤더
	i3GuiEditBox	*	m_pClanListHeadRank;				// 클랜 리스트 헤더 : 클랜등급
	i3GuiEditBox	*	m_pClanListHeadClanName;			// 클랜 리스트 헤더 : 클랜 이름
	i3GuiEditBox	*	m_pClanListHeadEntryDay;			// 클랜 리스트 헤더 : 클랜 개설일
	i3GuiEditBox	*	m_pClanListHeadMember;				// 클랜 리스트 헤더 : 클랜 멤버수

	// 유저 리스트 헤더
	i3GuiEditBox	*	m_pUserListHeadTitle;				// 유저 리스트 헤더 : 타이틀

	// 나가기
	i3GuiButton		*	m_pLeaveButton;

	i3GuiStatic*		m_pFocusedSlot;
	i3GuiStatic*		m_pSelect;

	//	미가입자 채팅
	i3GuiPopupList *	m_pChatComboPopup;
	
protected:
	INT32				m_nClanListStartIndex;
	INT32				m_nClanListPageIndex;

	INT32				m_nUserListStartIndex;
	INT32				m_nUserListPageIndex;

	INT32				m_nCurFocusSlotIdx;
	INT32				m_nCurSelectedSlotIdx;

	BOOL				m_bChangedClanSlot;

	INT32				m_nCurDisplayingClanDetailInfo;		//현재 보여지고있는 Detailed Info의 dbindex
	BOOL				m_bNeedUpdateClanList;

	SORT_CLAN_INFO 		m_SelectedClanInfo;
	
public:
	CStepClanNonMember();
	virtual ~CStepClanNonMember();

	virtual void OnPreGuiNotify(void);	
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void EnterServer(void);
	void LeaveServer(void);

	void SetClanName(const char * pszString, UINT8 cColor );
	void SetClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);
	void SetClanOrder(INT32 nCurValue, INT32 nPrevValue);
	void SetClanStaff(INT32 nNum, INT32 nTotal);
	void SetClanMasterName(const char * pszString, INT32 nRank);
	void SetClanExp(UINT32 nCurValue, UINT32 nMaxValue); 
	void SetClanAddress(const char * pszString);
	void SetClanMark(UINT32 nFlag);
	
	void OnSortClanListByRank(void);
	void OnSortClanListByName(void);
	void OnSortClanListByDay(void);
	void OnSortClanListByMember(void);
	void OnSearchClanList(void);
	void OnViewAll(void);

protected:
	void	_UpdateClanList( REAL32 rDeltaSeconds);
	void	_UpdateClanSerchRate( REAL32 rDeltaSeconds);

	void	_InitClanListTab( void);
	void	_InitUserListTab( void);

	void	_InitFindClanPage( void);
	void	_InitClanPage( void);

	void	_OnClanCreate();
	void	_OnClanSubscription();
	void	_OnLeave();

	void	_OnClanListOnMouse( INT32 idx);
	void	_OnClanListOnClicked( INT32 idx);
	void	_OnClanListDoubleClick( INT32 idx);
	void	_UpdateClanDetailedInfo(void);
	void	_OnClanListScroll(void);
	void	_OnUserListScroll(void);

	void	_SetClanListSelectedHilight( INT32 idx);
	BOOL	_SearchHilightClanList(void);

	void	_OnUserListTab(void);

	void	_SearchClanListView(const char * pszKeyword);
	void	_RebuildClanListScrollBar(void);
};

#endif // _STEP_CLAN_NON_MEMBER__H