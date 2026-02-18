#ifndef __CBATTLEGUI_H__
#define __CBATTLEGUI_H__

#include "StageDef.h"
#include "BattleGUI_BOSS_PreStart.h"

#if legacy_gui_scene
	#include "GuiNotifyReceiver.h"
	#include "BattleGUI_Stage_PreStart.h"
#endif

class CAwayChecker;
//class CMessageBox;
class CBattleGUI_BOSS_PreStart;
class CBattleGUI_Stage_PreStart;

template<> struct is_tinst_manual<CBattleGui> : std::tr1::true_type {};

class CBattleGui : public i3GameObjBase
{
	I3_CLASS_DEFINE( CBattleGui, i3GameObjBase);

public:
	static CBattleGui*	i() { return tinst<CBattleGui>(); }

public:
	CBattleGui(void);
	virtual ~CBattleGui(void);

public:
	bool				Create();
	void				OnUpdate( REAL32 rDeltaSeconds );
	virtual bool		OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;


	void				OnGameStart(void);
	void				OnSelectWeaponClose(void);
	void				SetEnablePopup(void);
	bool				GetEnablePopup(void);
	void				SetEnableGui( bool bFlag, GAME_GUI_STATUS nState = GAME_GUI_RESPAWN, bool bDeath = false);
	bool				GetEnableGui( INT32 nState);
	void				InitGui();
	void				_LinkControl(void);
	bool				LoadGUIFile(const char * szGUIFileName);
	void				_Rec_BuildScene( i3GuiObjBase * pObj);
	void	 ProcessSelectHud(REAL32 rDeltaSeconds);
	void				ProcessRespawn( REAL32 rDeltaSeconds );
	i3GuiRoot *			getGUIRoot(void)								{ return m_pBattleGUIRoot; }
	void				UpdateScore( bool bVisible, REAL32 tm);
	bool				IsResultState() { return m_bEnableResult; }

#if defined	USE_AWAY_INPUT
	bool				isEnableRespawnLimitMessage(void)	{ return nullptr != m_pAwayMessageBox;		}
	void Check_AwayCheck();
#endif

private:
	void				_UpdateRootEnableState(void);

protected:
	i3GuiRoot *			m_pBattleGUIRoot;
	//CBattleGUI_BOSS_PreStart *	m_pPanel_BOSS_PreStart;

	REAL32				m_fRespawnTime;
	REAL32				m_fTimeBarScale;
	REAL32				m_fDeathAtWeaponSelectCoolTime;

	bool				m_bEnableRespawn;
	bool				m_bEnableSelectWep;
	bool				m_bEnableResult;
	bool				m_bAutoRespawn;

#if defined	USE_AWAY_INPUT
	CAwayChecker*		m_pAwayChecker;
	UIMessageBox*		m_pAwayMessageBox;
#endif


#if legacy_gui_scene
private:
	CBattleGUI_Stage_PreStart *	m_pPanel_Stage_PreStart;
#endif

#if legacy_not_use_operator
private:
	REAL32				m_rWheelDelta;
	void				_ProcessWheel(void);

public:
	void				SetRespawnImage( i3GuiStatic* pStatic, INT32 WepClass, INT32 nType);
	void				OnTrainingConfrim(void);
	void				OnTrainingUpdateScore(void);
	void				OnResultConfrim(void);
#endif
};

tinst<CBattleGui>;
////////////////////////////////////////////////////////////////////////////////////


#endif // __CBATTLEGUI_H__
