/**
 * \file	UI\UICharaView.h
 *
 * Declares the user interface chara view class.
 */


#if !defined( __UI_CHARA_VIEW_H__)
#define __UI_CHARA_VIEW_H__

class CGameCharaBase;

#define CHARAVIEW_RECREATE_CHARA	0x01
#define CHARAVIEW_RECREATE_WEAPON	0x02
#define CHARAVIEW_RECREATE_PARTS	0x04

#define CHARAVIEW_RECREATE_ALL			(CHARAVIEW_RECREATE_CHARA | CHARAVIEW_RECREATE_WEAPON | CHARAVIEW_RECREATE_PARTS)
#define CHARAVIEW_RECREATE_CHARA_WEAPON	(CHARAVIEW_RECREATE_CHARA | CHARAVIEW_RECREATE_WEAPON)

/************************************************************************/
/*							UICharaView Class							*/
/************************************************************************/

class UICharaView : public i3GameNode
{
	I3_CLASS_DEFINE( UICharaView, i3GameNode);

private:
	bool				m_bMyCharaView;
	VIEW_CHARA_INFO		m_viewCharaInfo;

	CGameCharaBase *	m_pCharacter;		// 완료하면 여기에 
	VEC3D				m_CharacterPosition;

	INT32				m_iIndex;
	bool				m_bFirstUpdate;

	UINT8				m_ui8RecreateFlag;
	bool				m_bChangeAction;
	WEAPON_SLOT_TYPE	m_ChangingWeaponSlot;

	UINT32				m_ui32CurViewItemID;
	bool				m_bIsSameWeapon;

protected:
	bool 			_RecreateSlotChara( const CHARA_PARTS_INFO* pPartsInfo, TEAM_TYPE team);
	bool 			_CheckEquip( const CHARA_PARTS_INFO * pPartsInfo);

	bool 			_CheckWeapon( const UI_CHARA_INFO * pUICharaInfo, WEAPON_SLOT_TYPE mustUpdateSlot, TEAM_TYPE team = TEAM_NONE);
	INT32			_CheckLimitWeapon( void);
	void			_RecreateWeapon( void);
	bool 			_RecreateOtherWeapon(VIEW_CHARA_INFO * pNewCharaInfo);

	void			_Reaction( void);

	void			_addRecreateFlag(UINT8 flag)		{ m_ui8RecreateFlag |= flag; }
	void			_removeRecreateFlag(UINT8 flag)		{ m_ui8RecreateFlag &= ~flag; }
	bool			_isRecreateFlag(UINT8 flag)			{ return ( (m_ui8RecreateFlag & flag) != 0 ); }


public:
	VIEW_CHARA_INFO * getViewCharaInfo( void)		{ return &m_viewCharaInfo; }

	void			setTeam( TEAM_TYPE team)		{ m_viewCharaInfo._charaInfo.SetTeam( (team==CHARACTER_TEAM_RED)?TEAM_RED:TEAM_BLUE );  }
	TEAM_TYPE		getTeam()						{ return (m_viewCharaInfo._charaInfo.GetTeam()==CHARACTER_TEAM_RED)?TEAM_RED:TEAM_BLUE;}
	
	void			setWeaponUsage( INT32 slot)		{ m_viewCharaInfo._weaponSlot = slot; }

	void			setCharaSlotIndex( INT32 idx)			{ m_viewCharaInfo._charaInfo.SetNetIdx( idx); }

	CGameCharaBase *	GetCharacter( void)			{ return m_pCharacter; }

public:
	UICharaView();
	virtual ~UICharaView();

	/** \brief Character를 삭제합니다. */
	void			RemoveViewChara( bool bRemoveInstance = false);

	/** \brief Character에 붙어 있는 ClanMark를 Update해줍니다 */
	void			ChangeCharaClanMark( void);

	void			PlayCharaAnim_Ceremony( void);

	/** \brief Character를 설정합니다.
		\desc 호출될시 이전 캐릭터와 다른 경우 다시 생성하게 됩니다.
		\param[in] idx 슬롯 인덱스
		\param[in] bFirstUpdate 최초 업데이트 여부 (다시 호출될시 이전 정보를 갖고 생성한다.)*/
	bool			UpdateCharacter( INT32 idx, bool bFirstUpdate, WEAPON_SLOT_TYPE mustUpdateSlot, TEAM_TYPE team );

	void			SetVisibleChara( bool bVisible);

	void			SetLoadedChara( INT32 idx, Avatar * pLoadedAvatar);
	static void		sAfterLoadChara( void * pAvatar, INT32 idx = -1, AI_BIND_DATA * pAIBind = nullptr);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			SetCharaPosition( VEC3D * pPos);
};

/************************************************************************/
/*							Chara View Mgr Shared Inst					*/
/************************************************************************/

class UILoadingIcon;

typedef void (*FinishLoadCallback) (i3GameNode * pThis);

class CharaViewMgr : public i3::shared_ginst<CharaViewMgr>
{
private:
	UICharaView *			m_pCharaView;
	UILoadingIcon *			m_pLoadWaitIcon;

	i3AttrSet	*			m_p3DRoot;
	i3UI3DViewBox *			m_pUI3DViewBox;
	i3Light *				m_pUI3DViewLight;

	REAL32					m_r32CharaRotTheta;	

	INT32					m_i32MouseOldX;
	INT32					m_i32MouseOldY;

	UINT32					m_i32Param;

	bool					m_bIsLoadedChara;

	FinishLoadCallback		m_pfFinishLoadCallback;
	
private:
	/************************************************************************/
	/*								UI3DView Used							*/
	/************************************************************************/
	void				_AddLight( void);
	void				_RemoveLight( void);
	
	i3RenderAttr *		_GetReflectAttr(void);

public:
	CharaViewMgr(void);
	virtual ~CharaViewMgr(void);

public:
	void				CreateCharaView(void);
	void				ReleaseCharaView(void);
	i3AttrSet	*       Get3DRoot() const { return m_p3DRoot; }

	void				SetCharaViewInfo(i3GameNode * pParent, i3AttrSet * p3DRoot );
	void				SetCharaViewInfo(i3GameNode * pParent, i3UIFrameWnd * pFrameWnd );

	void				ResetCharaViewInfo(i3GameNode * pParent);

	void				UpdateCharacter(INT32 idx, bool bFirstUpdate, WEAPON_SLOT_TYPE slot, TEAM_TYPE team, i3UIFrameWnd * pFrameWnd);

	void				LoadCompleteProcess(INT32 idx, Avatar * pAvatar);
	void				AddChildByChara(CGameCharaBase * pChara);		

	void				SetMouseOldPos(const INT32 x, const INT32 y)		{ m_i32MouseOldX = x; m_i32MouseOldY = y; }
	void				SetCharaRotateTheta(REAL32 r32Theta)				{ m_r32CharaRotTheta = r32Theta; }

	void				RotateCharacter(INT32 x, INT32 y);

	void				SetParam(const UINT32 i32Param)						{ m_i32Param = i32Param; }
	UINT32				GetParam(void)										{ return m_i32Param; }

	const bool			isLoadWaiting(void)									{ return (m_pLoadWaitIcon != nullptr); }
	const bool			isLoadedChara(void)									{ return m_bIsLoadedChara; }

	void				SetFinishLoadCallback(FinishLoadCallback cbFunc)	{ m_pfFinishLoadCallback = cbFunc; }

	static UICharaView * getView(void)
	{
		I3ASSERT(CharaViewMgr::i()->m_pCharaView != nullptr);
		return CharaViewMgr::i()->m_pCharaView; 
	}

};

#endif
