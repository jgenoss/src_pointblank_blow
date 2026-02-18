#ifndef __CBATTLEGUI_H__
#define __CBATTLEGUI_H__

#include "StageDef.h"

#include "BattleGUI_Define.h"
#include "BattleGUI_BOSS_Score.h"
#include "BattleGUI_Result.h"
#include "BattleGUI_BOSS_PreStart.h"
#include "BattleGUI_Stage_PreStart.h"

#define MAX_RESPAWN_SLOT_COUNT	5

#define MAX_RESULT_SLOT_COUNT	(SLOT_MAX_COUNT)

enum GUI_ID_BATTLE_TYPE
{
	GUI_ID_BATTLE_BUTTON_RESULT_CONFIRM = GUI_ID_OFFSET_BATTLE,
	
	GUI_ID_BATTLE_BUTTON_GAMESTART,
	GUI_ID_BATTLE_BUTTON_CANCEL,

	GUI_ID_BATTLE_BUTTON_PREV_1,
	GUI_ID_BATTLE_BUTTON_PREV_2,
	GUI_ID_BATTLE_BUTTON_PREV_3,
	GUI_ID_BATTLE_BUTTON_PREV_4,
	GUI_ID_BATTLE_BUTTON_PREV_5,
	GUI_ID_BATTLE_BUTTON_NEXT_1,
	GUI_ID_BATTLE_BUTTON_NEXT_2,
	GUI_ID_BATTLE_BUTTON_NEXT_3,
	GUI_ID_BATTLE_BUTTON_NEXT_4,
	GUI_ID_BATTLE_BUTTON_NEXT_5,

	GUI_ID_BATTLE_STATIC_PREV_1,
	GUI_ID_BATTLE_STATIC_PREV_2,
	GUI_ID_BATTLE_STATIC_PREV_3,
	GUI_ID_BATTLE_STATIC_PREV_4,
	GUI_ID_BATTLE_STATIC_PREV_5,
	GUI_ID_BATTLE_STATIC_NEXT_1,
	GUI_ID_BATTLE_STATIC_NEXT_2,
	GUI_ID_BATTLE_STATIC_NEXT_3,
	GUI_ID_BATTLE_STATIC_NEXT_4,
	GUI_ID_BATTLE_STATIC_NEXT_5,

	GUI_ID_BATTLE_BUTTON_TRAINING_CONFIRM,
	GUI_ID_BATTLE_BUTTON_TRAINING_UPDATE_SCORE,

	MAX_GUI_ID
};

enum GUI_TRAINING_TEXT
{
	GUI_TRAINING_TEXT_NICK = 0,
	GUI_TRAINING_TEXT_TOTAL_HIT_COUNT,
	GUI_TRAINING_TEXT_TARGET1,
	GUI_TRAINING_TEXT_TARGET1_HIT,
	GUI_TRAINING_TEXT_TARGET1_POINT,
	GUI_TRAINING_TEXT_TARGET2,
	GUI_TRAINING_TEXT_TARGET2_HIT,
	GUI_TRAINING_TEXT_TARGET2_POINT,

	GUI_TRAINING_TEXT_COUNT,
};

enum GUI_ID_EXITPOPUP_BUTTON
{
	GUI_ID_EXITPOPUP_BUTTON_OPTION = 0,
	GUI_ID_EXITPOPUP_BUTTON_REPORT,
	GUI_ID_EXITPOPUP_BUTTON_FORCEREMOVE,		// 강제퇴장
	GUI_ID_EXITPOPUP_BUTTON_EXIT,
	GUI_ID_EXITPOPUP_BUTTON_CANCEL,

	GUI_ID_EXITPOPUP_BUTTON_COUNT,
};


struct WeaponSelectSlot
{
	i3GuiStatic* m_pSlotBg;
	i3GuiStatic* m_pSlotEnable;
	i3GuiStatic* m_pSlotDisable;
	i3GuiEditBox* m_pSlotKey;
	i3GuiButton* m_pSlotPrev;
	i3GuiButton* m_pSlotNext;
	i3GuiStatic* m_pPrevWeapon;
	i3GuiStatic* m_pPrevWeaponDummy;
	i3GuiStatic* m_pPrevWeaponExtension;
	i3GuiEditBox* m_pPrevWeaponValue;
	i3GuiEditBox* m_pPrevWeaponCategory;
	i3GuiStatic* m_pSelectWeapon;
	i3GuiStatic* m_pSelectWeaponDummy;
	i3GuiStatic* m_pSelectWeaponExtension;
	i3GuiEditBox* m_pSelectWeaponValue;
	i3GuiEditBox* m_pSelectWeaponCategory;
	i3GuiStatic* m_pNextWeapon;
	i3GuiStatic* m_pNextWeaponDummy;
	i3GuiStatic* m_pNextWeaponExtension;
	i3GuiEditBox* m_pNextWeaponValue;
	i3GuiEditBox* m_pNextWeaponCategory;

	void Reset(void)
	{
		m_pSlotBg = NULL;
		m_pSlotEnable = NULL;
		m_pSlotDisable = NULL;

		m_pSlotKey = NULL;
		m_pSlotPrev = NULL;
		m_pSlotNext = NULL;

		m_pPrevWeapon = NULL;
		m_pPrevWeaponDummy = NULL;
		m_pPrevWeaponExtension = NULL;
		m_pPrevWeaponValue = NULL;
		m_pPrevWeaponCategory = NULL;

		m_pSelectWeapon = NULL;
		m_pSelectWeaponDummy = NULL;
		m_pSelectWeaponExtension = NULL;
		m_pSelectWeaponValue = NULL;
		m_pSelectWeaponCategory = NULL;

		m_pNextWeapon = NULL;
		m_pNextWeaponDummy = NULL;
		m_pNextWeaponExtension = NULL;
		m_pNextWeaponValue = NULL;	
		m_pNextWeaponCategory = NULL;
	}
};

class CAwayChecker;
class CMessageBox;
//class CPopupOption;

template<> struct is_tinst_manual<CBattleGui> : true_type {};

class CBattleGui : public i3GameObjBase
{
	I3_CLASS_DEFINE( CBattleGui, i3GameObjBase);
public:
	static CBattleGui*	i() { return tinst<CBattleGui>(); }

protected:
	BOOL				m_bEnableRespawn;
	BOOL				m_bEnableSelectWep;
	BOOL				m_bEnableResult;
	BOOL				m_bEnableTrainingResult;
	BOOL				m_bAutoRespawn;

	i3GuiRoot *			m_pBattleGUIRoot;

	REAL32				m_fRespawnTime;
	REAL32				m_fTimeBarScale;

	// Respawn
	i3GuiStatic *		m_pStaticRespawnBG;
	i3GuiStatic *		m_pStaticRespawnWepSelectBG;
	i3GuiStatic *		m_pStaticSeconds;
	i3GuiStatic *		m_pStaticText;	
	i3GuiEditBox*		m_pEditNotice;
	i3GuiStatic *		m_pStaticTimeBar;
	i3GuiStatic *		m_pStaticTimeBarBG;
	i3GuiStatic *		m_pStaticTimeItem;	//	리스폰 단축 아이템 사용시 추가 텍스트 이미지
	i3GuiEditBox *		m_pEditTimeItemText;	//	리스폰 단축 아이템 사용시 추가 텍스트

	i3GuiButton*		m_pConfirm;
	i3GuiButton*		m_pCancel;
	WeaponSelectSlot	m_weaponSlot[MAX_CATEGORY_WEAPON];
	i3GuiEditBox*		m_pLimitKey;

	// Result
	CBattleGUI_Result *	m_pPanelResult;

	// Training Result
	i3GuiStatic*		m_pStaticTrainingBG;	
	i3GuiButton *		m_pButtonTrainingConfirm;
	i3GuiButton *		m_pButtonTrainingUpdateScore;
	i3GuiStatic*		m_pStaticTrainingNumber[5];
	i3GuiEditBox *		m_pEditBoxTraining[GUI_TRAINING_TEXT_COUNT];	

	// Exit Popup
	i3GuiStatic*		m_pStaticExitPopupBG;
	i3GuiEditBox *		m_pEditBoxExitPopupText;
	i3GuiButton *		m_pButtonExitPopup[4];


	INT32				m_idxPrimaryWeapon;
	INT32				m_idxSecondaryWeapon;
	INT32				m_idxMeleeWeapon;
	INT32				m_idxThrowWeapon;
	INT32				m_idxWeaponItem;

	//
	//CPopupOption *		m_pInGamePopup;

	REAL32				m_rWheelDelta;

#if defined	USE_AWAY_INPUT
	CAwayChecker*		m_pAwayChecker;
	UIMessageBox*		m_pAwayMessageBox;
public:
	void Check_AwayCheck();
#endif

	REAL32				m_fDeathAtWeaponSelectCoolTime;

	CBattleGUI_BOSS_Score *	m_pPanel_BOSS_Score;
	CBattleGUI_BOSS_PreStart *	m_pPanel_BOSS_PreStart;
	CBattleGUI_Stage_PreStart *	m_pPanel_Stage_PreStart;
	
public:
	CBattleGui(void);
	virtual ~CBattleGui(void);

	BOOL				Create();
	void				OnUpdate( REAL32 rDeltaSeconds );

	virtual void		OnControlNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void		OnPreControlNotify(void);
	virtual void		OnPostControlNotify(void);

	virtual BOOL		OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	BOOL				OnInputForWeaponSelect(i3InputKeyboard* pKeyboard);
	BOOL				OnInputForTraining(i3InputKeyboard*pKeyboard);

	void				OnTrainingConfrim(void);
	void				OnTrainingUpdateScore(void);
	void				OnResultConfrim(void);
	void				OnGameStart(void);
	void				OnWeaponSelect(UINT32 nWeaponUsage,BOOL bNext);
	void				OnWeaponFocus(UINT32 idxSlot,BOOL bNext);
	void				OnSelectWeaponCancel(void);

	void				SetEnablePopup(void);
	BOOL				GetEnablePopup(void);
	void				SetEnableGui( BOOL bFlag, GAME_GUI_STATUS nState = GAME_GUI_RESPAWN, BOOL bDeath = FALSE);
	BOOL				GetEnableGui( INT32 nState);
	BOOL				LoadGUIFile(const char * szGUIFileName);
	void				_Rec_BuildScene( i3GuiObjBase * pObj);
	void				_LinkControl(void);
	void				_LinkControlByHash(const i3::vector_set<STRCINFO*>* plist, i3GuiControl * pControl);

	void				InitGui();
	void				SetRespawnImage( i3GuiStatic* pStatic, INT32 WepClass, INT32 nType);

	void	 ProcessSelectHud(REAL32 rDeltaSeconds);
	void				ProcessRespawn( REAL32 rDeltaSeconds );
	void				ProcessTrainingResult( REAL32 rDeltaSeconds );

	INT32				GetWinnerTeam(void);

	i3GuiRoot *			getGUIRoot(void)								{ return m_pBattleGUIRoot; }

	void				SetResult(ResultState state);

#if defined	USE_AWAY_INPUT
	BOOL				isEnableRespawnLimitMessage(void)	{ return NULL != m_pAwayMessageBox;		}
#endif

	void				UpdateScore( bool bVisible, REAL32 tm);

private:
	INT32				_ControlNotifyPump(void);
	void				_InitWeaponSelect(void);
	void				_UpdateSelectWeapon(INT32 idxSlot);

	INT32				_GetNextUsingWeapon(INT32 idx,INT32 usage, UINT8 MyRoomWeaponFlag);
	INT32				_GetPrevUsingWeapon(INT32 idx,INT32 usage, UINT8 MyRoomWeaponFlag);

	void				_ProcessWheel(void);

	void				_SetWeaponCategory( i3GuiEditBox * pControl, CWeaponInfo * pInfo);

	void				RPG7_SelectWeapon();

	void				_UpdateRootEnableState(void);

	void				_SetSelectWeaponName(INT32 idxWeapon, i3GuiEditBox * pControl, const i3::string& weaponName, INT32 flag);

	BOOL				_IsUseableWeapon(INT32 IdxWeapon, INT32 WeaponUsage, UINT8 ItemAttr);

public:
	BOOL				IsResultState() { return m_bEnableResult; }
};

tinst<CBattleGui>;
////////////////////////////////////////////////////////////////////////////////////


#endif // __CBATTLEGUI_H__
