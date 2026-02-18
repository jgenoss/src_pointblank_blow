#if !defined( __UI_PHASE_TEMP_V10_H__)
#define __UI_PHASE_TEMP_V10_H__

#include "UI/UIPhaseSubBase.h"


class UIPhaseTemp_V10 : public UIPhaseSubBase
{
	I3_CLASS_DEFINE( UIPhaseTemp_V10, UIPhaseSubBase);

private:
	i3UIScene *						m_pCurrentScene;
	
public:
	UIPhaseTemp_V10();
	virtual ~UIPhaseTemp_V10();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;
	virtual void	SetInputDisable(bool bDisable) override;

	virtual void	OnLoadAllScenes() override;
	virtual bool	OnExitStart() override;
	virtual void	OnExitEnd( void) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;
	virtual bool	OnKey_Escape() override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void	Leave( void) override;

	virtual bool	OnClick( i3UIControl * pControl) override;


};

#endif
