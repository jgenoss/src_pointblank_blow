
#if !defined( __UI_PHASE_NATION_H__)
#define __UI_PHASE_NATION_H__

#include "UIPhaseBase.h"

class UIPhaseNation : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseNation, UIPhaseBase);
protected:

private:
	i3::vector<i3UIButtonImageSet *> m_pButton;
	bool							m_bExit_Popup;
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIPhaseNation();
	virtual ~UIPhaseNation();

	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	//------------------------------------------------------------------------------//
	//								Game Event Functions							//
	//------------------------------------------------------------------------------//

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual bool	OnKey_Enter( void) override;
	virtual bool	OnKey_Escape() override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	void			ClickExit(void);
	void			ClickBtn(INT32 btidx);
	void			ClickLogin(void);
	void			InitButtonState();



};

#endif
