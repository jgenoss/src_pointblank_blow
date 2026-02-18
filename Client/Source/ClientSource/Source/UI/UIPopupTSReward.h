#pragma once
#include "UIPopupBase.h"

class UIImgSetCtrl;

class UIPopupTSReward : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupTSReward, UIPopupBase);

	enum { MAX_ITEM = 2, };

public:
	UIPopupTSReward();
	virtual ~UIPopupTSReward();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnEntranceEnd() override;

	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnKey_Enter() override { Confirm(); return true; }
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

public:
	void SelectItem(int inx);
	void	 Confirm(void);

private:
	void SetLabel();
	void SetItem();
	void ChangeItemDisplay();

private:
	void EffectItem();

private:
	struct ItemContext
	{
		i3::rc_wstring name;
		i3UIFrameWnd *frame;
		UIImgSetCtrl* img_box;
		REAL32 factor; //alpha, scale 모두를 같이 처리한다.
	};

	ItemContext m_ItemContext[MAX_ITEM];
	REAL32 m_AccrueTime;
	UINT32 m_SelectInx;
	bool m_bEffect;
};

