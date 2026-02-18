#if !defined( __UI_USER_GUIDE_SLOT_H__)
#define __UI_USER_GUIDE_SLOT_H__

#include "UIBase.h"

struct GuideTargetAttr
{
	i3UIScene *		_pScene;
	i3UIControl *	_pCtrl;
	INT32			_i32EventIdx;
	INT32			_i32GuideType;
	INT32			_i32GuideNum;

	void Reset()
	{
		_pScene = nullptr;
		_pCtrl = nullptr;
		_i32EventIdx = -1;
		_i32GuideType = -1;
		_i32GuideNum = -1;
	}

	void CopyFrom(const GuideTargetAttr * pData)
	{
		_pScene = pData->_pScene;
		_pCtrl = pData->_pCtrl;
		_i32EventIdx = pData->_i32EventIdx;
		_i32GuideType = pData->_i32GuideType;
		_i32GuideNum = pData->_i32GuideNum;
	}

	bool IsSameAttr(const GuideTargetAttr * pData)
	{
		if(_pScene != pData->_pScene)				return false;
		if(_pCtrl != pData->_pCtrl)					return false;
		if(_i32EventIdx != pData->_i32EventIdx)		return false;
		if(_i32GuideType != pData->_i32GuideType)	return false;
		if(_i32GuideNum != pData->_i32GuideNum)		return false;

		return true;
	}
};

class UIUserGuideSlot : public UIBase
{
	I3_CLASS_DEFINE( UIUserGuideSlot, UIBase);
private:
	i3UIButtonImageSet *	m_pBtnCoin;
	i3UIButtonImageSet *	m_pBtnCoinEffect;

	GuideTargetAttr			m_TargetAttr;

	REAL32					m_rShapeChangeTime;
	bool					m_bRecievedCoin;
	bool					m_bDuplicate; 

protected:
	virtual void _InitializeAtLoad(i3UIScene * pScene) override;
	virtual void _ClearAtUnload(i3UIScene * pScene) override;
	void		 _AdjustSceneDepth(void);

public:
	UIUserGuideSlot();
	virtual ~UIUserGuideSlot();

	virtual void		OnCreate( i3GameNode * pParent) override;
	virtual void		ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;
	virtual bool		OnRevive( i3RenderContext * pCtx) override;

	void				EntranceSlot(GuideTargetAttr * pAttr, bool bDuplicate);
	void				ExitSlot(void);

	void				OnUpdateGuideSlot(REAL32 rDeltaSeconds);
	void				OnUpdateCoinAnim(i3UIButtonImageSet * pCtrl);
	bool				SendClickEvent(bool bDBClick);
	void				SetGuideAttr(GuideTargetAttr * pAttr, bool bDuplicate);
	void				SetCoinPosition(void);

	void				ProcessLvBoxScroll(i3UIListView_Box * pLvBox);
	void				ProcessSlotScroll(bool bEnableCtrl);

	bool				isEndAnimation(void);
	bool				isSameTargetEvent(UINT32 ui32Event);
	bool				isSameTargetCtrl(i3UIControl * pCtrl);
	bool				isReceivedCoin(void){ return m_bRecievedCoin; }
	bool				isDuplicate(void)	{ return m_bDuplicate; }

	bool				isFrameEnable(void)	{ return m_pFrameWnd != nullptr ? m_pFrameWnd->isEnable() : false; }



	const GuideTargetAttr *	GetTargetAttr(void) const	{ return &m_TargetAttr; }
	INT32				GetEventIndex(void) const	{ return m_TargetAttr._i32EventIdx; }
	INT32				GetGuideType(void) const	{ return m_TargetAttr._i32GuideType; }
	INT32				GetGuideNumber(void) const	{ return m_TargetAttr._i32GuideNum; }
};

#endif // __UI_USER_GUIDE_SLOT_H__
