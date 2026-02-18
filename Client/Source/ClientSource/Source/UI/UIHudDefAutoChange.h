#pragma once

#include "iHudBase.h"
#include "UISlide.h"

class UIHudManager;
class i3UIButtonImageSet;

#define MAXTEAMSLOT 2

class UIHudDefAutoChange : public iHudBase
{
public:
	UIHudDefAutoChange(UIHudManager* p);
	virtual ~UIHudDefAutoChange();

	virtual void		InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		ClearAtUnload( i3UIScene * pScene) override;
	virtual void		SetParameter(INT32 arg1, INT32 arg2 = -1) override;
	virtual void		Update( REAL32 rDeltaSeconds ) override;
	virtual void		SetEnable(bool enable) override;
private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_first_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) override;

protected:
	void			_Create3DViewBox( i3UIControl * pParent);
	void			_Destroy3DViewBox( void);

	void			_CreateSwitchObject(void);
	void			_DestroySwitchObject(void);

	void			_UpdatePopup(REAL32 rDeltaSeconds);
	
	void			_AddLight( void);
	void			_ReleaseLight( void);

protected:
	i3UI3DViewBox *				m_p3DView;
	i3Object *					m_pObject;

	i3UIButtonImageSet*			m_apImgSetCtrl[MAXTEAMSLOT];	// ∆¿ ¿ÃπÃ¡ˆ
	i3UIStaticText*				m_pTeamText;	// ∆¿ TEXT

private:
	REAL32				m_rPopupElapsedTime;
	bool				m_bStarted;


};