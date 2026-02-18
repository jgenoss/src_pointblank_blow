
#pragma once

#include "UIPopupColorItemBase.h"

class UIPopupColorFireEffect : public UIPopupColorItemBase
{
	I3_CLASS_DEFINE(UIPopupColorFireEffect, UIPopupColorItemBase);
public:
	enum { FIRE_EFFECT_COLORIDX_MAX = 7}; 
	UIPopupColorFireEffect();
	virtual ~UIPopupColorFireEffect();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scene) override;

	virtual bool OnClick(i3UIControl* pControl) override;
	virtual bool OnOKButton( void) override;

protected:
	virtual LuaState *	_SetItemInfo( void) override;

private:
	bool _IsColorbox(i3UIControl* pControl) const;
	void _UpdateSelectedColor();

private:
	i3UIFrameWnd*						m_pFmwColor;
	int									m_SelectedIndex;
	i3UIButton*							m_colorboxes[FIRE_EFFECT_COLORIDX_MAX];

};