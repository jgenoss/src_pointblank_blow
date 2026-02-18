#if !defined( __UI_POPUP_ATTENDANCECHECK_H__)
#define __UI_POPUP_ATTENDANCECHECK_H__

#include "UIPopupBase.h"

class UIImgSetCtrl;

class UIAttendanceStampSlot
{
private:
	enum { STAMP_WIDTH = 66 , STAMP_HEIGHT = 62 };

	i3UIButtonImageSet *	m_pStampCtrl;

public:
	UIAttendanceStampSlot(void);
	~UIAttendanceStampSlot(void);

	void		SetStampData(i3UIFrameWnd * pParent, const INT32 idx, const INT32 count );

	void		SetShapeIdx(INT32 idx)							{ m_pStampCtrl->SetShapeIdx(idx); }
	void		GetCtrlPos(VEC3D * pOutPos)						{ i3Vector::Copy(pOutPos,  m_pStampCtrl->getAccumulatedPos() ); }
	void		GetCtrlSize(VEC2D * pOutSize)					{ i3Vector::Copy(pOutSize,  m_pStampCtrl->getSize() ); }

	const bool	IsSameCtrl(const i3UIButtonImageSet * pCtrl )	{ return m_pStampCtrl == pCtrl; }
};

typedef i3::vector<UIAttendanceStampSlot*> StampSlotList;

class UIPopupAttendanceCheck : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupAttendanceCheck, UIPopupBase);

public:
	UIPopupAttendanceCheck();
	virtual ~UIPopupAttendanceCheck();

	virtual void OnCreate ( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;
	virtual bool OnEntranceStart(void * pArg1 = nullptr  , void * pArg2 = nullptr ) override;
	virtual void OnEntranceEnd(void) override;
	virtual void OnExitEnd(void) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;
	virtual bool OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool OnKey_Enter() override { return false; }
	virtual bool OnKey_Escape() override { ClosePopup();	return true;}

public:
	void CheckStamp(void);
	void ClosePopup();
	void UpdateItemView(INT32 nItemIdx);
	/*버튼 입력이 중복 입력되어 특정 시점까지 버튼을 막습니다.
	특정 시점 : 메세지 박스 or 출석 지급 팝업 Close*/
	void SetBtnEnable(bool bFlag);
	
public:
	// 메세지박스 콜백
	static void cbSetBtnEnable(void * pThis, INT32 nParam);

protected:
	void _InitStampSlotInfo(i3UIFrameWnd * pFrame);
	void _ClearStampSlotInfo(void);

	void _SetEventData(void);
	void _SetItemInfo(i3::rc_wstring& out, UIImgSetCtrl *pCtrl, INT32 nGoodsID);
	void _SetStampShape(void);

	void _SetStampAnim(void);
	void _SetStampControl(void);
	void _PlayStampAnim(REAL32 rDeltaSeconds);
	void _StopStampAnim(void);

	void _SetButtonText(void);

	void _OpenComplatePopup(void);

	void _SetPeriod(i3::wstring& out, UINT32 ui32StartDate, UINT32 ui32EndDate);
	void _SetMission(i3::wstring& out, UINT8 ui8CheckDay);

	void _SetErrorMsgBox(const i3::rc_wstring& wstrError);

private:
	UIImgSetCtrl *			m_pImgCtrl[2];
	i3UIButtonImageSet *	m_pStampCtrl;

	StampSlotList			m_vecStampSlotList;
	
	INT32					m_nItemCount;
	bool					m_bPlayStampAnim;
	bool					m_bIsCheckComplete;

	REAL32					m_rAnimEndTime;
	REAL32					m_rAccumTime;

	VEC3D					m_vTargetPos;
	VEC2D					m_vTargetSize;

	bool					m_bRes;
	
	void			ReceiveGameEvent_Attendance_Check( INT32 arg, const i3::user_data&);
	
};

#endif