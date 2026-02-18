#if !defined( __UI_POPUP_ABUSE_H__)
#define __UI_POPUP_ABUSE_H__

#if legacy_gui_scene

#include "UIPopupBase.h"

class UIPopupAbuse : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupAbuse, UIPopupBase);

private:
	i3GuiStatic		*m_pAbuseWindow;
	i3GuiEditBox	*m_pAbuseEditbox;
	i3GuiButton		*m_pAbuseConfirm;

	REAL32			m_rAbusingTotalTime;

public:
	UIPopupAbuse();
	virtual ~UIPopupAbuse();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnLinkControl( const i3::vector_set<STRCINFO*>* pList, i3GameNode * pControl) override;
	virtual void	OnInitControl( void) override;
	virtual void	OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify) override;

	void			ConfirmOK( void);
	virtual bool	OnKey_Enter( void) override {  ConfirmOK(); return true;    }
	virtual bool	OnKey_Escape() override { ConfirmOK(); return true; }
};

#endif

#endif
