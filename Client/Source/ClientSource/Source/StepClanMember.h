#if !defined( _STEP_CLAN_MEMBER_H__)
#define _STEP_CLAN_MEMBER_H__

#include "StepBase.h"
#include "StageDef.h"
#include "ClanContext.h"
#include "GameChat_Body.h"

//#define ENABLE_CLAN_CHAT		//	클랜 채팅 가능


enum CLAN_PAGE_TYPE {
	CLAN_PAGE_UNKNOWN,
	CLAN_PAGE_INFO,
	CLAN_PAGE_BATTLE,
	CLAN_PAGE_SEARCH,
	CLAN_PAGE_MANAGEMENT,
};


#define MAX_CHAT_LINE_FOR_CLAN		15

enum BEST_MEMBER_SLOT
{
	BEST_MEMBER_SLOT_EXP		= 0,	
	BEST_MEMBER_SLOT_CONNECTING,
	BEST_MEMBER_SLOT_WIN_COUNT,	
	BEST_MEMBER_SLOT_KILL_COUNT,
	BEST_MEMBER_SLOT_HEAD_COUNT,	

	BEST_MEMBER_SLOT_MAX
};

struct BestMemberSlot
{
	i3GuiStatic * pSlot;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pSeasonMember;
	i3GuiEditBox * pTopMember;

	BestMemberSlot() : pSlot(NULL), pTitle(NULL), pSeasonMember(NULL), pTopMember(NULL)
	{
	}
};

#define CLANINFO_MEMBER_LIST_ROW_MAX 7

struct ClanMemberSlot
{
	i3GuiStatic * pSlot;
	i3GuiStatic * pRank;
	i3GuiEditBox * pClass;
	i3GuiEditBox * pNick;
	i3GuiEditBox * pState;	

	ClanMemberSlot() : pSlot(NULL), pRank(NULL), pClass(NULL), pNick(NULL), pState(NULL)
	{
	}
};

#define CLANINFO_HISTORY_LIST_ROW_MAX 4

enum CLAN_HISTORY_SLOT
{
	CLAN_HISTORY_SLOT_MATCH = 0,
	CLAN_HISTORY_SLOT_WIN,
	CLAN_HISTORY_SLOT_LOSE,
	CLAN_HISTORY_SLOT_MATCHES,
};

struct ClanHistorySlot
{
	i3GuiStatic * pSlot;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pSeasonScore;
	i3GuiEditBox * pTotalScore;

	ClanHistorySlot() : pSlot(NULL), pSeasonScore(NULL), pTotalScore(NULL)
	{
		pTitle = NULL;
	}
};

#define MANAGEMENT_LIST_ROW_MAX 12

struct ClanListSlot
{
	i3GuiStatic	*	_pSlot;
	i3GuiStatic	*	_pRankImage;
	i3GuiStatic	*	_pClanMarkImage;
	i3GuiEditBox*	_pName;
	i3GuiEditBox*	_pDate;
	i3GuiEditBox*	_pMember;
};

#define CLAN_SEARCH_LIST_SLOT_MAX 8

struct ClanEntryTeamSlot
{	
	i3GuiStatic * pSlot;
	i3GuiEditBox * pTeamName;		//	팀 이름
	i3GuiEditBox * pLocale;			//	방위치
	i3GuiStatic * pState;			//	상태
	i3GuiEditBox * pLeaderValue;	//	'리더'
	i3GuiStatic * pLeaderRank;		//	리더 계급
	i3GuiEditBox * pLeaderNick;		//	리더 닉네임
	i3GuiEditBox * pMatchType;		//	인원 타입
	i3GuiEditBox * pEntryMember;	//	현재 인원수
	i3GuiButton * pJoinButton;		//	'참여하기'
};

#define CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX	6

struct ClanManagementSlot
{
	i3GuiStatic * pSlot;
	i3GuiStatic * pRank;
	i3GuiCheckBox * pCheck;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pNick;
	i3GuiEditBox * pClass;
	i3GuiEditBox * pEntryDay;

	ClanManagementSlot() : pSlot(NULL), pCheck(NULL), pTitle(NULL), pNick(NULL), pClass(NULL), pEntryDay(NULL)
	{
		pRank = NULL;
	}
};

#define SUBSCRIPTION_LIST_ROW_MAX 12

struct ClanSubscriptionSlot
{
	i3GuiStatic * pSlot;
	i3GuiStatic * pRank;
	i3GuiCheckBox * pCheck;
	i3GuiEditBox * pTitle;
	i3GuiEditBox * pNick;
	i3GuiEditBox * pEntryDay;

	ClanSubscriptionSlot() : pSlot(NULL), pCheck(NULL), pTitle(NULL), pNick(NULL), pEntryDay(NULL)
	{
		pRank = NULL;
	}
};

struct ManagementClanMemberInfo
{
	BOOL _bCheck;
	CLAN_MEMBER_BASIC_INFO _info;

	ManagementClanMemberInfo() : _bCheck(FALSE)
	{		
		i3mem::FillZero(&_info, sizeof(CLAN_MEMBER_BASIC_INFO));
	}
};

struct ManagementSubscriberInfo
{
	BOOL _bCheck;
	CLAN_REQUEST_BASIC_INFO _info;

	ManagementSubscriberInfo() : _bCheck(FALSE)
	{
		i3mem::FillZero(&_info, sizeof(CLAN_REQUEST_BASIC_INFO));
	}
};

enum ADMISSION_LIMIT_TYPE
{
	ADMISSION_LIMIT_RANK,
	ADMISSION_LIMIT_AGE1,
	ADMISSION_LIMIT_AGE2,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class CStepClanMember : public CStepBase
{
	I3_CLASS_DEFINE(CStepClanMember);

protected:
	CClanContext * m_pClanContext;

	INT32 m_nChatType;
	INT32 m_nOldChatType;
	CLAN_PAGE_TYPE m_CurrentPage;

	i3GuiStatic * m_pClanMemberRoot;		//	Gui Root

	i3GuiStatic * m_pMyClanWin;				//	내 클랜 창
	i3GuiEditBox * m_pMyClanCaption;		//	'내 클랜'

	i3GuiStatic * m_pClanManagementWin;		//	클랜정보 창
	i3GuiEditBox * m_pClanManagementCaption;	//	'클랜정보'

	i3GuiButton * m_pInfoTab;			//	클랜정보 탭
	i3GuiButton * m_pBattleTab;			//	클랜전 탭
	i3GuiButton * m_pSearchTab;			//	클랜찾기 탭
	i3GuiButton * m_pManagementTab;		//	클랜관리 탭
	i3GuiButton * m_pLeaveButton;		//	나가기 버튼

	i3GuiStatic * m_pInfoPhase;				//	클랜정보 탭 페이지
	i3GuiStatic * m_pBattlePhase;			//	클랜전 탭 페이지
	i3GuiStatic * m_pSearchPhase;			//	클랜찾기 탭 페이지
	i3GuiStatic * m_pManagementPhase;		//	클랜관리 탭 페이지

	//==================================================================================================
	//	내 클랜 컨트롤
	//==================================================================================================
	i3GuiStatic * m_pMyClanMarkImage;			//	내클랜 마크 이미지
	i3GuiStatic * m_pMyClanMasterRankImage;		//	내클랜 마스터 계급 이미지
	i3GuiStatic * m_pMyClanRankImage;			//	내클랜 계급 이미지
	i3GuiStatic * m_pMyClanExpBar;				//	내클랜 경험치 게이지

	i3GuiStatic * m_pMyClanAddressBullet;

	i3GuiEditBox * m_pMyClanNameStatic;			//	클랜명
	i3GuiEditBox * m_pMyClanNameValue;
	i3GuiEditBox * m_pMyClanStaffStatic;		//	클랜 인원수
	i3GuiEditBox * m_pMyClanStaffValue;
	i3GuiEditBox * m_pMyClanMasterStatic;		//	클랜 마스터
	i3GuiEditBox * m_pMyClanMasterValue;
	i3GuiEditBox * m_pMyClanRankStatic;			//	클랜 등급
	i3GuiEditBox * m_pMyClanRankValue;
	i3GuiEditBox * m_pMyClanExpStatic;			//	클랜 경험치
	i3GuiEditBox * m_pMyClanExpValue;
	i3GuiEditBox * m_pMyClanOrderStatic;		//	클랜 순위
	i3GuiEditBox * m_pMyClanOrderValue;
	i3GuiEditBox * m_pMyClanAddressStatic;		//	클랜 아지트
	i3GuiEditBox * m_pMyClanAddressValue;		
	i3GuiEditBox * m_pMyClanPointStatic;		//	클랜 포인트
	i3GuiEditBox * m_pMyClanPointValue;		

	//==================================================================================================
	//	클랜정보 컨트롤
	//==================================================================================================
	i3GuiButton * m_pInfoHistoryButton;		//	클랜전적 버튼
	i3GuiButton * m_pInfoMemberButton;		//	클랜멤버 버튼
	i3GuiButton * m_pInfoSecessionButton;	//	클랜탈퇴 버튼
	
	i3GuiStatic * m_pInfoHistroyPhase;		//	클랜전적 버튼 페이지
	i3GuiStatic * m_pInfoMemberPhase;		//	클랜멤버 버튼 페이지

	i3GuiEditBox * m_pInfoBestListStatic;				//	'우수 클랜 리스트'
	i3GuiEditBox * m_pInfoMemberListStatic;				//	'클랜원 리스트'
	i3GuiEditBox * m_slotBestMemberListHeadSeason;		//	'시즌' 컬럼
	i3GuiEditBox * m_slotBestMemberListHeadTotal;		//	'통산' 컬럼
	i3GuiEditBox * m_slotClanMemberListHeadClass;		//	'직급' 컬럼
	i3GuiEditBox * m_slotClanMemberListHeadNick;		//	'닉네임' 컬럼
	i3GuiEditBox * m_slotClanMemberListHeadState;		//	'상태' 컬럼
	i3GuiList *	m_pClanMemberScrollList;				//	클랜원 스크롤 리스트
	
	i3GuiEditBox * m_pInfoHistoryNoticeStatic;		//	'공지사항'
	i3GuiEditBox * m_pInfoHistoryNoticeValue;		//	공지사항 내용
	i3GuiEditBox * m_pInfoHistoryListHeadSeasonScore;	//	'시즌 전적' 컬럼
	i3GuiEditBox * m_pInfoHistoryListHeadTotalScore;	//	'통산 전적' 컬럼

	INT32 m_idxClanInfoMemberStartSlot;

	BestMemberSlot m_slotBestMemberList[BEST_MEMBER_SLOT_MAX];				//	클랜정보 우수 클랜원 리스트
	ClanMemberSlot m_slotClanMemberList[CLANINFO_MEMBER_LIST_ROW_MAX];		//	클랜정보 클랜원 리스트
	ClanHistorySlot m_slotClanHistoryList[CLANINFO_HISTORY_LIST_ROW_MAX];	//	클랜정보 전적 리스트

	//==================================================================================================
	//	클랜전 컨트롤
	//==================================================================================================
	i3GuiEditBox * m_pBattleMakeTeamStatic;			//	'팀 구성'
	i3GuiButton * m_pBattleMakeTeamEntryButton;		//	'팀 등록'

	i3GuiEditBox * m_pBattleMakeTeamCombo;			//	팀 등록 컴보
	i3GuiButton * m_pBattleMakeTeamComboButton;		//	팀 등록 컴보 버튼
	i3GuiPopupList * m_pBattleMakeTeamComboPopup;	//	'5 vs 5'...

	i3GuiList * m_pBattleTeamScrollList;		//	등록팀 스크롤 리스트
	INT32 m_idxBattleTeamListStartSlot;	
	INT32 m_idxBattleTeamMatchType;
	INT32 m_nBattleTeamListSelectedIdx;

	ClanEntryTeamSlot m_slotBattleTeamList[CLAN_BATTLE_ENTRY_TEAM_SLOT_MAX];	//	등록된 팀 슬롯

	BOOL m_bUpdateTeamList;
	REAL32 m_fUpdateTeamSection;
	INT32 m_nTeamSection;

	//==================================================================================================
	//	클랜찾기 컨트롤
	//==================================================================================================
	i3GuiEditBox * m_pSearchStatic;
	i3GuiEditBox * m_pSearchInput;
	i3GuiButton * m_pSearchButton;
	i3GuiButton * m_pSearchViewAll;
	i3GuiEditBox	*	m_pClanMembeSerchText;		//	클랜찾기 진행 상태 텍스트

	i3GuiStatic		*	m_pClanSerchBar;				//	클랜찾기 프로그래스바 
	i3GuiStatic		*	m_pClanSerchGage;				//	클랜찾기 프로그래스바 게이지
	i3GuiEditBox	*	m_pClanSerchText;				//	클랜찾기 프로그래스바 텍스트

	i3GuiStatic * m_pSearchInfoAddressBullet;

	i3GuiStatic * m_pSearchInfoClanMarkImage;
	i3GuiEditBox * m_pSearchInfoNameStatic;
	i3GuiEditBox * m_pSearchInfoNameValue;
	i3GuiEditBox * m_pSearchInfoMasterStatic;
	i3GuiEditBox * m_pSearchInfoMasterValue;
	i3GuiStatic * m_SearchInfoMasterRankImage;
	i3GuiEditBox * m_pSearchInfoClanRankStatic;
	i3GuiEditBox * m_pSearchInfoClanRankValue;
	i3GuiStatic * m_SearchInfoClanRankImage;
	i3GuiEditBox * m_pSearchInfoStaffStatic;
	i3GuiEditBox * m_pSearchInfoStaffValue;
	i3GuiEditBox * m_pSearchInfoAddressStatic;
	i3GuiEditBox * m_pSearchInfoAddressValue;
	i3GuiEditBox * m_pSearchInfoOrderStatic;
	i3GuiEditBox * m_pSearchInfoOrderValue;
	i3GuiEditBox * m_pSearchInfoExpStatic;
	i3GuiEditBox * m_pSearchInfoExpValue;
	i3GuiStatic * m_SearchInfoExpBar;

	i3GuiEditBox * m_pSearchClanListHeadRank;
	i3GuiEditBox * m_pSearchClanListHeadName;
	i3GuiEditBox * m_pSearchClanListHeadDate;
	i3GuiEditBox * m_pSearchClanListHeadMember;

	i3GuiList * m_pSearchClanListScroll;
	i3GuiStatic * m_pSearchClanListFocus;
	i3GuiStatic * m_pSearchClanListSelect;

	ClanListSlot m_ClanListSlot[CLAN_SEARCH_LIST_SLOT_MAX];

	BOOL m_bNeedUpdateSearchClanList;
	INT32 m_nSearchClanStartSlot;
	INT32 m_nSearchClanSelectedSlot;
	SORT_CLAN_INFO m_SelectedClanInfo;

	//==================================================================================================
	//	클랜관리 컨트롤
	//==================================================================================================
	i3GuiStatic * m_pManagementWritingBoardPhase;		//	게시판 관리 버튼 페이지
	i3GuiStatic * m_pManagementMemberPhase;				//	멤버 관리 버튼 페이지
	i3GuiStatic * m_pManagementSubscriptionPhase;		//	가입 관리 버튼 페이지
	i3GuiStatic * m_pManagementOthersPhase;				//	기타 관리 버튼 페이지

	i3GuiButton * m_pManagementWritingBoardButton;		//	게시판 관리 버튼
	i3GuiButton * m_pManagementMemberButton;			//	멤버 관리 버튼
	i3GuiButton * m_pManagementSubscriptionButton;		//	가입 관리 버튼
	i3GuiButton * m_pManagementOthersButton;			//	기타 관리 버튼

	i3GuiEditBox * m_pManagementWritingBoardNoticeStatic;	//	공지사항 - 0
	i3GuiEditBox * m_pManagementWritingBoardNoticeEdit;		//	공지사항 입력란 - 0
	i3GuiEditBox * m_pManagementWritingBoardGuideStatic;	//	클랜소개 - 0
	i3GuiEditBox * m_pManagementWritingBoardGuideEdit;		//	클랜소개 입력란 - 0
	i3GuiButton * m_pManagementWritingBoardNoticeModify;	//	공지사항 수정완료 버튼 - 0
	i3GuiButton * m_pManagementWritingBoardGuideModify;		//	클랜소개 수정완료 버튼 - 0

	i3GuiEditBox * m_pManagementMemberStatic;			//	'클랜원 리스트'
	i3GuiEditBox * m_pManagementListHeadNick;			//	'닉네임' 컬럼
	i3GuiEditBox * m_pManagementListHeadClass;			//	'직급' 컬럼
	i3GuiEditBox * m_pManagementListHeadEntryDay;		//	'가입일' 컬럼
	i3GuiCheckBox * m_pManagementListHeadCheckAll;		//	리스트 모든 항목 체크박스
	i3GuiList *	m_pManagementMemberScrollList;			//	클랜원 관리 스크롤 리스트

	i3GuiEditBox * m_pManagementSubscriberStatic;		//	'가입대기 유저 리스트'
	i3GuiEditBox * m_pSubscriberListHeadNick;			//	'닉네임' 컬럼
	i3GuiEditBox * m_pSubscriberListHeadEntryDay;		//	'신청일' 컬럼
	i3GuiCheckBox * m_pSubscriberListHeadCheckAll;		//	리스트 모든 항목 체크박스
	i3GuiList *	m_pManagementSubscriberScrollList;		//	가입 대기자 관리 스크롤 리스트
	i3GuiStatic * m_pManagementSubscriberFocus;
	i3GuiStatic * m_pManagementSubscriberSelection;

	i3GuiEditBox * m_pManagementInstructionStatic;		//	'선택된 클랜원'
	i3GuiEditBox * m_pManagementInstructionCombo;
	i3GuiButton * m_pManagementInstructionButton;
	i3GuiPopupList * m_pManagementInstructionPopup;

	i3GuiEditBox * m_pSubscriptionInstructionStatic;	//	'선택된 유저'
	i3GuiEditBox * m_pSubscriptionInstructionCombo;
	i3GuiButton * m_pSubscriptionInstructionButton;
	i3GuiPopupList * m_pSubscriptionInstructionPopup;

	i3GuiEditBox * m_pManagementOthersAuthorityStatic;				//	'스텝 권한 설정'
	i3GuiEditBox * m_pManagementOthersWritingAuthorityStatic;		//	'게시글 관리 권한'
	i3GuiEditBox * m_pManagementOthersInviteAuthorityStatic;		//	'클랜 초대 권한'
	i3GuiEditBox * m_pManagementOthersDimissalAuthorityStatic;		//	'클랜원 제명 권한'
	i3GuiEditBox * m_pManagementOthersSubscriptionAuthorityStatic;	//	'가입 관리 권한'
	i3GuiCheckBox * m_pManagementOthersWritingAuthorityCheck;		//	게시글 관리 권한 체크박스
	i3GuiCheckBox * m_pManagementOthersInviteAuthorityCheck;		//	클랜 초대 권한 체크박스
	i3GuiCheckBox * m_pManagementOthersDimissalAuthorityCheck;		//	클랜원 제명 권한 체크박스
	i3GuiCheckBox * m_pManagementOthersSubscriptionAuthorityCheck;	//	가입 관리 권한 체크박스	
	i3GuiEditBox * m_pManagementOthersSendNoteStatic;				//	'클랜 전체 쪽지 보내기'
	i3GuiButton * m_pManagementOthersSendNoteButton;				//	'쪽지 보내기'
	i3GuiEditBox * m_pManagementOthersAdmissionLimitStatic;			//	'클랜 가입 제한 설정
	i3GuiEditBox * m_pManagementOthersAdmissionLimitRankStatic;		//	클랜 가입 제한 설정 : '계급'
	i3GuiEditBox * m_pManagementOthersAdmissionLimitRankCombo;		//	클랜 가입 제한 설정 계급 컴보
	i3GuiButton * m_pManagementOthersAdmissionLimitRankButton;
	i3GuiPopupList * m_pManagementOthersAdmissionLimitRankPopup;	
	i3GuiEditBox * m_pManagementOthersAdmissionLimitAgeStatic;	//	클랜 가입 제한 설정 : '나이'
	i3GuiEditBox * m_pManagementOthersAdmissionLimitAgeCombo[ADMISSION_LIMIT_CONTROL_COUNT];	//	클랜 가입 제한 설정 나이 컴보
	i3GuiButton * m_pManagementOthersAdmissionLimitAgeButton[ADMISSION_LIMIT_CONTROL_COUNT];
	i3GuiPopupList * m_pManagementOthersAdmissionLimitAgePopup[ADMISSION_LIMIT_CONTROL_COUNT];	
	i3GuiEditBox * m_pManagementOthersChangeClanNameStatic;			//	'클랜명 변경하기'
	i3GuiButton * m_pManagementOthersChangeClanNameButton;			//	'클랜명 변경'
	i3GuiEditBox * m_pManagementOthersChangeClanMarkStatic;			//	'클랜 마크 변경하기'
	i3GuiButton * m_pManagementOthersChangeClanMarkButton;			//	'클랜 마크 변경'
	i3GuiEditBox * m_pManagementOthersDissolutionStatic;	//	'클랜 해체하기'
	i3GuiButton * m_pManagementOthersDissolutionButton;		//	'해체하기'
	i3GuiButton * m_pManagementOthersModifyButton;		//	 적용하기
		
	INT32 m_idxClanManagementMemberStartSlot;
	INT32 m_idxClanManagementSubscriberStartSlot;
	INT32 m_nLastedSelectSlot;
	INT32 m_authItemFlag;
	
	ManagementClanMemberInfo m_ManageMemberInfo[MAX_MEMBER_COUNT];
	ManagementSubscriberInfo m_ManageSubscriberInfo[MAX_REQUEST_COUNT];

	i3BinList m_listManageMember;
	i3BinList m_listManageSubscriber;

	ClanManagementSlot m_slotManagementList[MANAGEMENT_LIST_ROW_MAX];		//	클랜관리 클랜원 리스트
	ClanSubscriptionSlot m_slotSubscriptionList[SUBSCRIPTION_LIST_ROW_MAX];	//	클랜관리 가입대기 유저 리스트
	
	void _InitMyClanWin(void);
	void _InitClanManagementWin(void);

	void _InitClanInfoPhase(void);
	void _InitClanBattlePhase(void);
	void _InitClanSearchPhase(void);
	void _InitClanManamementPhase(void);

	void _InitMyClanInfo(void);
	void _InitClanInfoHistory(void);
	void _InitClanInfoMember(void);

	void _InitClanManagementWritingBoard(void);
	void _InitClanManagementMember(void);
	void _InitClanManagementSubscriber(void);
	void _InitClanManagementOthers(void);

	void OnClanInfoTab(void);
	void OnClanBattleTab(void);
	void OnClanSearchTab(void);
	void OnClanManagementTab(void);

	void OnClanChatPopup(INT32 idx);
	void OnClanChatType(void);

	void OnClanInfoHistoryButton(void);					//	클랜 전적 버튼
	void OnClanInfoMemberButton(void);					//	클랜 멤버 버튼
	void OnClanInfoSecessionButton(void);				//	탈퇴 버튼

	void OnClanBattleTeamEntry(void);					//	팀 등록
	void OnClanBattleTeamTypePopup(INT32 idx);			//	팀 구성 팝업
	void OnClanBattleTeamJoin(INT32 slot);				//	참여하기 버튼
	void OnClanBattleTeamTypeButton(void);
	void OnClanBattleTeamListScroll(void);

	void OnSortSearchClanListByRank(void);
	void OnSortSearchClanListByName(void);
	void OnSortSearchClanListByDate(void);
	void OnSortSearchClanListByMember(void);
	void OnSearchClanListScroll(void);
	void OnSearchClanKeyword(void);
	void OnSearchClanViewAll(void);
	void OnSearchClanFocus(INT32 idx);
	void OnSearchClanSelect(INT32 idx);

	void OnClanManagementWritingBoardButton(void);		//	클랜 관리 게시판 버튼
	void OnClanManagementMemberButton(void);			//	클랜원 관리 버튼
	void OnClanManagementSubscriptionButton(void);		//	클랜 가입 관리 버튼
	void OnClanManagementOthersButton(void);			//	클랜 기타 관리 버튼

	void OnManagementInstructionButton(void);			//	클랜원 관리 명령 버튼
	void OnSubscriptionInstructionButton(void);			//	클랜 가입자 관리 명령 버튼
	void OnManagementInstructionPopup(INT32 nItemIdx);		
	void OnSubscriptionInstructionPopup(INT32 nItemIdx);

	void OnClanManagementBoardNoticeModify(void);		//	공지사항 수정완료
	void OnClanManagementBoardGuideModify(void);		//	클랜소개 수정완료

	void OnManagementOthersSendNote(void);				//	클랜 전체 쪽기 보내기
	void OnManagementOthersChangeClanName(void);		//	클랜명 변경
	void OnManagementOthersChangeClanMark(void);		//	클랜 마크 변경
	void OnManagementOthersDissolution(void);			//	클랜 해체하기
	void OnManagementOthersModify(void);				//	클랜 기타 변경 적용하기
	void OnManagementOthersAuthorityCheck(INT32 nIdx);	//	클랜 관리 권한 체크
	void OnManagementOthersAdmissionLimitOpen( ADMISSION_LIMIT_TYPE type);							//	클랜 가입 제한
	void OnManagementOthersAdmissionLimitInstruction( ADMISSION_LIMIT_TYPE type, INT32 nItemIdx);	//	클랜 가입 제한
	

	void OnClanInfoMemberScroll(void);
	void OnClanInfoMemberContext(INT32 nItemIdx);
	void OnClanManagementMemberScroll(void);	
	void OnClanManagementSubscriberScroll(void);

	void OnClanManagementMemberCheck(INT32 nCheckIdx);
	void OnClanManagementMemberCheckAll(void);
	void OnClanManagementSubscriberCheck(INT32 nCheckIdx);
	void OnClanManagementSubscriberCheckAll(void);

	void OnClanManagementSubscriberFocus(INT32 nIdx);
	void OnClanManagementSubscriberSelect(INT32 nIdx);
	void OnClanManagementSubscriberConfirm(INT32 nIdx);	

	void OnSortClanMemberListByLevel(void);
	void OnSortClanMemberListByName(void);
	void OnSortClanMemberListByState(void);
	void OnSortClanMemberListByDate(void);

	void OnSortSubscriberListByName(void);
	void OnSortSubscriberListByDate(void);

	void OnLeave(void);

	void _UpdateMyClanPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanInfoPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanBattlePhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanSearchPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementPhase(RT_REAL32 rDeltaSeconds);

	void _UpdateClanSerchRate( REAL32 rDeltaSeconds);

	void _UpdateClanInfoHistoryPhase(RT_REAL32 rDeltaSeconds);	
	void _UpdateClanTopBast();					// 추후 삭제될 놈입니다만, ClanServer 수정 작업까지만 살아있을 놈입니다. // 20080423 김현우
	void _UpdateClanInfoMemberPhase(RT_REAL32 rDeltaSeconds);

	void _UpdateClanManagementWritingBoardPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementMemberPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementSubscriberPhase(RT_REAL32 rDeltaSeconds);
	void _UpdateClanManagementOthersPhase(RT_REAL32 rDeltaSeconds);
	
	void _UpdateSortClanMemberList(void);
	void _UpdateSortSubscriberList(void);

	void _SetBestMember(BEST_MEMBER_SLOT slot, const CLAN_MEMBER_BASIC_INFO * pSeasonMember, const CLAN_MEMBER_BASIC_INFO * pTopMember);
	
	void _ResetClanInfoMemberListSlot(INT32 nIdx);	
	void _SetClanInfoMemberListSlot(INT32 nIdx, ManagementClanMemberInfo * pMember);

	void _UpdateClanBattleTeamSlot(INT32 idx, CLAN_BATTLE_TEAM_LIST * pTeamInfo);

	void _ResetManagementListSlot(INT32 nIdx);
	void _SetManagementListCheckAll(BOOL bCheck);
	void _SetManagementListSlot(INT32 nIdx, ManagementClanMemberInfo * pMember);

	void _ResetSubscriptionListSlot(INT32 nIdx);
	void _SetSubscriptionListCheckAll(BOOL bCheck);
	void _SetSubscriptionListSlot(INT32 nIdx, ManagementSubscriberInfo * pSubscriber);

	void _SetSearchClanName(const char * pszString);
	void _SetSearchClanStaff(INT32 nNum, INT32 nTotal);
	void _SetSearchClanMasterName(const char * pszString, INT32 nRank);
	void _SetSearchClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);
	void _SetSearchClanOrder(INT32 nCurValue, INT32 nPrevValue);
	void _SetSearchClanExp(UINT32 nCurValue, UINT32 nMaxValue);
	void _SetSearchClanAddress(const char * pszString);
	void _SetSearchClanMark(UINT32 nFlag);
	void _SetSearchClanListSlot(INT32 idx, CLAN_BASIC_INFO * pInfo);
	void _UpdateSearchClanList(void);	
	void _UpdateSearchClanDetailedInfo(void);
	void _SearchClanList(const char * pszKeyword);
	void _SetSearchHilightClanList(INT32 idx);
	void _RebuildSearchClanListScrollBar(void);
	BOOL _FindSearchHilightClanList(void);
		 
	//	채팅
	void _ProcessChat( REAL32 rDeltaSeconds);
	void _UpdateMacroChat(void);
	void _UpdateParsingType(void);
	void _UpdateLang(void);
	void _UpdateChatBuffer(void);

public:
	CStepClanMember();
	virtual ~CStepClanMember();

	virtual void OnPreGuiNotify(void);	
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);
	virtual void AuthStartSuccess(INT32 arg);

	static CLAN_SORT_TYPE m_sInfoClanMemberSortType;
	static CLAN_SORT_TYPE m_sManageClanMemberSortType;
	static CLAN_SORT_TYPE m_sManageSubscriberSortType;
	
	static INT32 cbSortManageMemberListByName(void* item1, void* item2);
	static INT32 cbSortManageMemberListByLevel(void* item1, void* item2);
	static INT32 cbSortManageMemberListByDate(void* item1, void* item2);

	static INT32 cbSortInfoMemberListByName(void* item1, void* item2);
	static INT32 cbSortInfoMemberListByLevel(void* item1, void* item2);
	static INT32 cbSortInfoMemberListByState(void* item1, void* item2);

	static INT32 cbSortSubscriberListByName(void* item1, void* item2);
	static INT32 cbSortSubscriberListByDate(void* item1, void* item2);

	void EnterServer(void);
	void LeaveServer(void);

	//	내 클랜 페이지
	void SetMyClanName(char * pszString, UINT8 cColor );					//	내 클랜 이름 설정
	void SetMyClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);	//	클랜 등급 호칭
	void SetMyClanOrder(INT32 nCurOrder, INT32 nPrevOrder);	//	내 클랜 순위 설정
	void SetMyClanStaff(INT32 nNum, INT32 nTotal);			//	내 클랜원 수 설정
	void SetMyClanMaster(char * pszName, UINT8 nRank);		//	내 클랜 마스터 설정
	void SetMyClanExp(UINT32 nCurExp, UINT32 nMaxExp);		//	내 클랜 경험치 설정
	void SetMyClanAddress(char * pszAddress);				//	내 클랜 아지트 URL
	void SetMyClanMark(UINT32 nFlag);						//	내 클랜 마크
	void SetMyClanPoint(REAL32 fPoint);						//	내 클랜 포인트

	//	전적 페이지
	void SetClanInfoHistoryMatch( INT32 nSeason, INT32 nTotal);		//	매치 수
	void SetClanInfoHistoryWin(INT32 nSeason, INT32 nTotal);		//	승리 수
	void SetClanInfoHistoryLose(INT32 nSeason, INT32 nTotal);		//	패배 수
	void SetClanInfoHistoryMatches(REAL32 fSeason, REAL32 fTotal);	//	승률	

	// 우측버튼 기능
	void OnWhisper(INT32 memberIdx);
	void OnInviteRoom(INT32 memberIdx);
	void OnVisit(INT32 memberIdx);
	void OnAddFriend(INT32 memberIdx);	
};

#endif // _STEP_CLAN_MEMBER__H