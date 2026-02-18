#if !defined( __UI_MAP_SLOT_H__)
#define __UI_MAP_SLOT_H__

#include "UIPopupBase.h"
#include "UIDefine.h"

#include "CSI_TypeDef.h"

class UIMapSlot
{
public:
	UIMapSlot();
	~UIMapSlot();

public:
	bool	IsSelected() const						{ return m_isSelected; }

public:
	bool	Init(i3UIFrameWnd* wnd);

public:
	void	SetSelect(bool bSel);

	void	SetMapInfo(CSI_STAGE* info);

	void	SetVisible(bool bVisible);
	void	SetEnable(bool bEnable);
	void	SetActivate(bool bActivate);

	void	OnMouseOver(bool bOver);

private:
	i3UIFrameWnd*			m_wnd;
	i3UIButtonImageSet*		m_img;
	i3UIButtonImageSet*		m_frame;
	i3UIStaticText*			m_text;

	i3UIControl*			m_modename_bg;
	i3UIStaticText*			m_modetext;

	i3UIControl*			m_bg;
	i3UIControl*			m_lock;

	CSI_STAGE*				m_stage;

private:
	bool	m_isSelected;
	bool	m_isMouseOver;
	bool	m_isModeVisible;
};

#endif