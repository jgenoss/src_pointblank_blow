#if !defined( __UI_POPUP_MEDAL_RESTRICTION_H__)
#define __UI_POPUP_MEDAL_RESTRICTION_H__

#include "UIPopupClanBase.h"

class UIPopupMedalRestriction : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMedalRestriction, UIPopupBase);

private:

protected:


public:
	
	UIPopupMedalRestriction();
	virtual ~UIPopupMedalRestriction();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 MedalViewInfo*
	param[in] pArg2 UINT8*
	*/
	virtual bool	OnEntranceStart(void * pArg1 = nullptr , void * pArg2 = nullptr ) override;

	virtual bool	OnKey_Enter(void) override { Close(); return true; }
	virtual bool	OnKey_Escape(void) override { Close(); return true; }

	void			Close( void);

private:
	void			_InitControl( void);

};
#endif
