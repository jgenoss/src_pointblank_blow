#if !defined( __UI_BANNER_BASE_H__)
#define __UI_BANNER_BASE_H__

#include "UIBase.h"

class UIMainFrame;

class UIBannerBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIBannerBase);

public:
	UIBannerBase();
	virtual ~UIBannerBase();

	UIMainFrame *	GetMainFrame( void)		{ return (UIMainFrame*) m_pParent; }

	virtual bool	OnExitStart( void)		{ OnSlideOut(); return true}
};

#endif
