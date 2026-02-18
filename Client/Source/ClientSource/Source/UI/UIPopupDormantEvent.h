#ifndef __UI_POPUP_DORMANT_EVENT_H__

#include "UIPopupBase.h"
#include "UIPopupAttendanceBase.h"

class UIImgSetCtrl;

/*
	휴면계정 출석 이벤트와 관련한 클래스입니다. (UPW_DORMANT_EVENT)
*/
class UIPopupDormantEvent : public UIPopupAttendanceBase
{
	I3_CLASS_DEFINE(UIPopupDormantEvent, UIPopupAttendanceBase);

public:
	UIPopupDormantEvent();
	virtual ~UIPopupDormantEvent();

public:
	virtual void				OnCreate(i3GameNode * pParent) override;
	virtual bool				OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	
	virtual void				OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool				OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code) override;

protected:
	virtual void				_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void				_ClearAtUnload(i3UIScene * pScene) override;

private:
	/*		UI Animation	*/
	i3UIFrameWnd*				m_pFrmEffect;
	i3UIImageBox*				m_pSlideBack;				// 총알이 지나가면서 TEXT가 보일 수 있게 미리 가려두는 이미지
	i3UIImageBox*				m_pSlideBullet;				// 총알 이미지

	bool						m_bAnimState;
	REAL32						m_rDelayTime_bullet;
	REAL32						m_rDelayTime_back;

	const REAL32				m_r32Bullet_PosX;
	const REAL32				m_r32BulletBack_PosX;
	const REAL32				m_r32BulletBack_MoveStart_PosX;
	const REAL32				m_r32BulletSpeed;
	const REAL32				m_r32AnimDuration;

	/*	보상 아이템 Control */
	UIImgSetCtrl *				m_pImgCtrl[2];
	i3UIStaticText *			m_pRewardText[2];
	i3UIButtonImageSet *		m_pRewardIcon[2];
	i3UIButtonComposed3*		m_pBtnOK;

	// 31일 아이템 리스트
	i3::vector<i3UIButtonImageSet *> m_vMonthItemBG;

	/* 현재 유저의 이벤트 진행 상태 관련 변수들 */
	ATTENDANCE_STATE			m_EventState;
	INT32						m_TodayAttendance;	// 오늘이 몇일차 출석인지
	INT32						m_ViewItemIdx;		// 하단에 몇일차 아이템이 보여야 하는지
	INT32						m_RewardItemIdx;

	INT32						m_InputState;

	enum {
		INPUT_NONE = 0,			// 초기상태
		INPUT_FIRST_PRESS,		// 애니메이션이 종료되기 전, 마우스 입력이 있는 상태
		INPUT_NON_RELATED,		// 입력과 더이상 관계가 없는 상태
	};

public:
	/*	UI LUA Input	*/
	void						Confirm();
	void						SelectReward(INT32 idx);
	void						StopAnim();

private:
	/*	UI Animation	 */
	void						ResetAnimation();
	void						PlayAnimation(REAL32 rDeltaSeconds);
	bool						DisableBtnConfirm();

	/* Set UI */
	void						SetEventData();
	void						SetCompletedDay(INT32 idx, INT32 shape_type);
	void						SetViewTodayItem();
	void						SetTodayItemIcon(ATTENDANCE_CHOICE_ITEM& item_, bool isViewOnly);

	/* About Game Event*/
	// ProcessGameEvent 함수는 Base 클래스에 존재하고, EVENT_ATTENDANCE_CHECK 가 발생할 경우 아래 함수가 호출됨
	virtual void				ReceiveGameEvent_Attendance_Check(INT32 arg, const i3::user_data &UserData);
	void						CompleteEvent();
};


#endif