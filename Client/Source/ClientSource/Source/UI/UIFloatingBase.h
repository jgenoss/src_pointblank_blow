/**
 * \file	UI\UIFloatingBase.h
 *
 * Declares the user interface floating base class.
 */


#if !defined( __UI_FLOATING_BASE_H__)
#define __UI_FLOATING_BASE_H__

#include "UIBase.h"
#include "UIDefine.h"

class UIMainFrame;
class UIFramework;

enum UIFLOATING_POS
{
	UIFLOATING_POS_NONE = -1,
	UIFLOATING_POS_LEFTTOP,
	UIFLOATING_POS_TOP,
	UIFLOATING_POS_RIGHTTOP,
	UIFLOATING_POS_RIGHT,
	UIFLOATING_POS_RIGHTBOTTOM,
	UIFLOATING_POS_BOTTOM,
	UIFLOATING_POS_LEFTBOTTOM,
	UIFLOATING_POS_LEFT,

	UIFLOATING_POS_MAX
};

class UIFloatingBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIFloatingBase, UIBase);

protected:
	UIFLOATING_POS	m_AlignedPos;		// 전체적인 Align
	UIFLOATING_POS	m_AlignedPos2;		// sub align

	bool			m_bFixedWindow;		// 고정 윈도우인지 판별

public:
	UIFLOATING_POS	getAlignedPos( void)			{ return m_AlignedPos; }
	UIFLOATING_POS	getAlignedPos2( void)			{ return m_AlignedPos2; }

	bool			IsFixedWindow(void)				{ return m_bFixedWindow;}
public:
	UIFloatingBase();
	virtual ~UIFloatingBase();

	UIMainFrame *	GetMainFrame( void);

	/** \brief Phase 이동시 호출됩니다. */
	virtual void	OnMovePhase( UIPHASE phase) {}
	virtual void	OnMoveSubPhase( UISUBPHASE phase) {}
};

#endif
