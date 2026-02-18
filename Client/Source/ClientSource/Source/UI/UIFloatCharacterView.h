#if !defined( __UI_FLOAT_CHARACTERVIEW_H__)
#define __UI_FLOAT_CHARACTERVIEW_H__

#include "UIFloatingBase.h"

#include "i3Base/itl/signal/signal_common.h"
//#include "UIPhaseInventory.h"

typedef UINT32 REVERT_TYPE;
#define REVERT_TYPE_WEAPON 0x01
#define REVERT_TYPE_EQUIP 0x02
#define REVERT_TYPE_ALL (REVERT_TYPE_WEAPON | REVERT_TYPE_EQUIP)

class UICharaView;

class UIFloatCharacterView : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatCharacterView, UIFloatingBase);


protected:
	INT32			m_iCurrentCharaIdx;
	
	TEAM_TYPE		m_TeamType;

	REAL32			m_ChangeCharaTimer;		// 3초에 한번 동작하도록 하자.

	REAL32			m_OriginalFrameWidth;
	
	i3UIFrameWnd *	m_pSettedUIFrame;	// UI가 배치된 FrameWnd

	i3UIFrameWnd *	m_pDurabilityWnd;
	i3UIProgressBar *	m_pDurability;
	i3UIButton *	m_pRepairBtn;

	REAL32			m_InterpolateTimeForBtnVisible;		// 알파 조절용 타임
	REAL32			m_DraggingTime;						// 드레깅 시간
	i3UIButton *	m_pPreCharaBtn;
	i3UIButton *	m_pNextCharaBtn;
	size_t m_repair_item_count;

	bool			m_bChangeChara;
	bool			m_bChangeTeam;
	bool			m_bVisbleBtn;
	bool			m_bDummyChara;
	bool			m_bMouseClicked;
	bool			m_bUpdatedCharaPos;
	bool			m_bCenterFrame;
	bool			m_bInputDisable;
	bool			m_bSkillSlot;

	i3PhysixShapeSet *	m_pPlaneShape;
	i3::scoped_connection_set	m_SigConnection;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	void			_SetCharaClass(void);
	void			_SetUIState( bool bChangeChara, bool bChangeTeam, bool bSkillSlot);
	void			_UpdateFrameWndPos( void);

	void			_UpdateDurability( void);
	void			_UpdateButtonVisible( REAL32 tm);

public:
	UIFloatCharacterView();
	virtual ~UIFloatCharacterView();

	virtual bool	OnEvent( UINT32 evt, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual void	UpdateCharaPosition( void);

	virtual void	SetInputDisable( bool bDisable) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;
	virtual bool	OnExitStart( void) override;

	// -->CReadyBg::CharaChangeClanMarkInReadyBg()
	void			ChangeCharaClanMark( void);

	void			PlayCeremonyCharacter( void);

	virtual void			SelectChara(T_ItemDBIdx chara_db_idx);
	// -->Chara Interface
	virtual void			UpdateCharaSlot(INT32 charaSlot = -1);

	/** \brief 아바타 정보를 설정
		\param[in] bDummyChara 실질 캐릭터인지 아닌지
		\param[in] mustUpdateWeapon 반드시 업데이트될 무기는 설정하면 됨 */
	virtual void			DoUpdateCharacter( bool bDummyChara = false, WEAPON_SLOT_TYPE mustUpdateWeapon = WEAPON_SLOT_UNKNOWN);
	void			ChangeOtherUserChara(INT32 i32SlotIdx);
	void			UpdateCharacter( INT32 slot);
	void			SetCameraPlayer();
	VIEW_CHARA_INFO * getViewCharaInfo( void);

	void			SetChangeChara(INT32 istate, bool rotation = true);
	void			ToggleTeam( void);
	virtual void	SetChangeTeam( TEAM_TYPE teamType);
	void			ClearAtUnloadChara();

	void			RepairAll( void);
	void			SendRepairEvent( bool change_equip_result );

	INT32			GetCurrentCharaIdx() const { return m_iCurrentCharaIdx; }

	virtual void	OnMovePhase( UIPHASE phase);
	virtual void	OnMoveSubPhase( UISUBPHASE phase);

	/** \brief Control에 Mouse Over되면 호출됩니다. */
	virtual bool	OnEnterMouse( i3UIControl * pControl);
	/** \brief Control에 Mouse leave되면 호출됩니다. */
	virtual void	OnLeaveMouse( i3UIControl * pControl);

	virtual void	OnDragStart( i3UIControl * pControl, INT32 x, INT32 y);
	virtual void	OnDragging( i3UIControl * pControl, INT32 x, INT32 y);
	virtual void	OnDragEnd( i3UIControl * pControl, INT32 x, INT32 y);

	virtual bool	OnRevive( i3RenderContext * pCtx);

	static void		CbRepairItem(void * pThis, INT32 nParam);
	
	//상점에서 사용
	void			Update_CharaView_Parts_Preview();

	void			Update_CharaView_Parts(const i3::vector<ITEM_INFO>&list);

	// 미리보기 전 상태로 설정
	virtual void			RevertChara(REVERT_TYPE type = REVERT_TYPE_ALL);

	bool			CheckViewRepairUI(void);

public:
	void cb_finish_repair_all_item(bool bArg);


	////////////////////////////////////////////////////////////////////////////
	//					Team Change Button And Team View UI
protected:
	i3UIFrameWnd *	m_pTeamRedWnd;
	i3UIFrameWnd *	m_pTeamBlueWnd;
	i3UIButtonImageSet * m_pBtnTeamSmallRed;
	i3UIButtonImageSet * m_pBtnTeamLargeRed;
	i3UIButtonImageSet * m_pBtnTeamSmallBlue;
	i3UIButtonImageSet * m_pBtnTeamLargeBlue;

	VEC3D			m_PosBtnSmall;
	VEC3D			m_PosBtnLarge;

	bool			m_bChangingTeam;
	REAL32			m_ChangingTeamTimer;
	T_ItemID		m_UpdatableItemID;

	void			__UpdateChangingTeamUI( REAL32 tm);

	////////////////////////////////////////////////////////////////////////////
	//								Skill Icon
public:
	void			ClickSkillIcon( INT32 icon);
	T_ItemID		getUpdatableItemID()	{ return m_UpdatableItemID; }

	void			SetInputStateTeamChange( bool bEnable);


	//----------------------------------------------------------------------//
	//						Received Game Event Function					//
	//----------------------------------------------------------------------//
public:
	typedef void	(UIFloatCharacterView::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_DoUpdateCharacter( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Team_Change( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Team_Balance( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Change_MainCharacterFromRoom( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_DeleteCharacter( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_SetCharacter(INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_RevertCharacter(INT32 arg, const i3::user_data&);
	virtual void	ReceiveGameEvent_RevertParts(INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Repair(INT32 arg, const i3::user_data&);

	//----------------------------------------------------------------------//
	//						Received Game Event Function					//
	//----------------------------------------------------------------------//
public:
	typedef bool	(UIFloatCharacterView::*ui_func)(i3ElementBase*, UINT32, I3_EVT_CODE);
	i3::unordered_map< UIEVENT, ui_func >	m_UIFuncMap;

	void			RegisterUIEventFunc( UIEVENT evt, ui_func f);

	bool			UIEvent_Change_Avatar( i3ElementBase *, UINT32, I3_EVT_CODE);
	bool			UIEvent_Change_Weapon( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE);
	virtual bool	UIEvent_Change_Parts( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE);
	bool			UIEvent_Equip_Parts( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE);
	bool			UIEvent_Update_Chara_Slot( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE);
	
};

#endif
