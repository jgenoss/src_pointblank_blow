#if !defined( __UI_TAB_PROFILE_MEDAL_H__)
#define __UI_TAB_PROFILE_MEDAL_H__

#include	"UITabProfileBase.h"
#include	"UILBSlot.h"
#include	"MedalManager.h"

#define MEDAL_DETAIL_INFO_MAX_SLOT 4

class UIBlinkLVButtons;
class UILoadingIcon;

class UITabProfileMedal : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileMedal, UITabProfileBase );

public:
	UITabProfileMedal();
	~UITabProfileMedal();

	virtual void	OnCreate( i3GameNode * pParent ) override;
	virtual bool	OnEntranceStart( void * pArg1, void * pArg2 ) override;
	virtual void	OnEntranceEnd() override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool	OnExitStart() override;
	virtual void	OnExitEnd() override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData) override;

public:		// 초기화 함수
	void	InitScroll();
	void	InitViewList()		{ if( m_pviewList != nullptr) m_pviewList->clear();		}
	void	InitRewardList()	{ if( m_prewardList != nullptr) m_prewardList->clear();	}
	void	InitSortType();
	//void	InitLVButtonState();
	void	InitTab();
	void	InitNotify();
	void	MakeViewList();
	void	SetMedalScrollFocus();

private:
	void	_SetDetailInfoSlot();
	void	SetRewardMedalPage();
	void	SetRewardMedalSlot(INT32 idx, INT32 rewardmedal);
	void	SetRewardPageControl();
	void	SetAchievementRate();									// 메달 퀘스트 달성률 셋팅
	void	UpdateToolTip(INT32 idx);

public:
	void	_PushNotifyMedal(INT32 idx);
	void	SelectedMedalSlot(INT32 idx);
	void	UpdateRewardMedalList();
	void	MoveRewardPage(INT32 idx);
	void	ChangeRewardPage(INT32 idx);
	void	SetRewardToolTip(INT32 idx);
	void	CloseRewardToolTip();

public:		// 슬롯 콜백 등록 함수
	void	OnLoadSlot(INT32, i3UIScene*);
	void	OnUnloadSlot(INT32, i3UIScene*);
	void	OnEnabledSlot(INT32, i3UIScene*);
	void	OnDisabledSlot(INT32, i3UIScene*);
	void	OnEnterSlot(INT32, i3UIScene*);
	void	OnLeaveSlot(INT32, i3UIScene*);
	bool	EnableMouseOver() { return true; }

public:		// 메달 프레임 함수
	void	UpdateMedalInfo(UINT8 lv = 0xff, bool inout = true);
	void	UpdateMedalTab();
	void	ChangeTab( MEDALTAB eTab );		// 메달 텝 변경
	void	ChangeSortType(INT32);			// 메달 정렬 변경
	void	ChangeLevel(INT32, bool bEvent = true);				// 메달 레벨 클릭
	//bool	SetSelIdx(INT32 idx);				// 메달 슬롯 선택
	void	GetReward(INT32);				// 보상 획득 버튼 클릭
	void	AllGetReward();
	void	NotifySelected(INT32);			// 알리미 선택
	void	NotifySelectedClear(INT32);			// 알리미 선택 해제
	void	NotifyAllClear();				// 알리미 전체 해제
	bool	isChecked(INT32, UINT16);
	void	CheckSlotEnable();				// 슬롯 체크박스 활성/비활성 체크

	void	SetComboEditBoxText(INT32 sortType);	// 외부에서 콤보박스 EditBox 이름을 강제로 설정
	void	setMedalTab(INT32 idx);

public:		// 입력 함수(루아 InputEvent)
	void	Click_MedalSlot(INT32);
	void	DoubleClick_MedalSlot(INT32);
	void	Check_MedalSlot(INT32, bool);
	void	UpdateWheel(INT32);				// 마우스 휠
	void	UpdateScroll(INT32 idx);					// 스크롤 이벤트

private:
	INT32	getSlotRow();
	void	SetMedalPushed(UINT8);			// 프레임 하단 레벨별 버튼 정보 갱신
	void	UpdateBlinkingLVButtons();

private:
	typedef TUILBSlot_Callback<UITabProfileMedal, &UITabProfileMedal::OnLoadSlot, &UITabProfileMedal::OnUnloadSlot, &UITabProfileMedal::OnEnabledSlot, &UITabProfileMedal::OnDisabledSlot,
		&UITabProfileMedal::OnEnterSlot, &UITabProfileMedal::OnLeaveSlot, &UITabProfileMedal::EnableMouseOver> SlotCallback;
	friend class SlotCallback;
	SlotCallback	m_slotCallback;

	MEDALTAB			m_eTab;								// 현재 선택된 탭
	//UILBSlotSet			m_SlotSet;							// 슬롯 셋트
	class UIImgSetCtrl*	m_apImgSetCtrl[Medal_SLOT_TOTAL];	// 슬롯 이미지
	i3UIScrollBar*		m_pMedalScroll;						// 메달프레임 스크롤
	i3UIComboBox*		m_pComboBox;						// 메달 정렬 콤보 박스
	i3UIToolTip*		m_pRewardToolTip;					// 보상 목록 툴팁
	i3UIButtonComposed3*	m_pMedalDetailButton[4];		// 
	INT32				m_selectedIdx;						// 데이터의 인덱스
	INT32				m_viewIdx;							// 화면상의 인덱스
	UINT8				m_gameEventLevel;					// 패킷 갱신 후 저장용 레벨

	INT32				m_CurPos;
	INT32				m_OldPos;
	bool				m_bScrollFirst;
	INT32				m_SelectedSlotIdx;

	INT32				m_RewardMedalPage;
	INT32				m_RewardMedalCount;
	INT32				m_RewardMedalCurPage;

	REAL32				m_RewardToolTipHeight;

	i3::vector<MedalViewInfo *>*	m_pviewList;
	i3::vector<MedalViewInfo *>*	m_prewardList;
	NOTIFY_MEDAL		m_NotifyInfo[MAX_NOTIFY_MADAL_COUNT];

	//UIBlinkLVButtons*	m_pBlinkLVButtons;
};

//class UIBlink;

// 퀘스트 창 네개의 LV버튼 깜빡거림 효과 구현.
/*class UIBlinkLVButtons
{
public:
	explicit UIBlinkLVButtons(i3UIScene* pScene);
	~UIBlinkLVButtons();

	void OnUpdate(REAL32 tm);

	void Enable(const MEDAL_LEVEL lv);
	void Disable(const MEDAL_LEVEL lv);

private:
	UIBlink* m_pUIBlink[MEDAL_LEVEL_MAX];
};*/

#endif