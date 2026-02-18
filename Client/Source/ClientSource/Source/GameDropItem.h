#pragma once


class CGameDropItem : public i3Chara
{
	I3_CLASS_DEFINE(CGameDropItem, i3Chara);

	i3AttrSet *			m_pAttrSet = nullptr;
	i3MaterialAttr *	m_pMaterialAttr = nullptr;
	i3BlendEnableAttr *	m_pBlendEnable = nullptr;
	REAL32				m_fAlphaVal = 0.0f;

	bool				m_bFree = false;

	//Info
	INT32				m_nItemType = -1;
	INT32				m_nNetCode = 0;			//서버에서 주는 값.
	
	//effect
	INT32				m_effect_idx = -1;

	//Drop
	REAL32				m_fLifeTime = 0.0f;
	bool				m_bDroped = false;
	bool				m_bPicked = false;


	//Flying
	REAL32				m_fFlyingTime = 0.0f;

	REAL32				m_fSpeed = 0.0f;
	VEC3D				m_vDir;

	VEC3D				m_vOldPos;
	VEC3D				m_vDestPos;
	
	VEC3D				m_vUp = { 0.0f, 0.0f, 1.0f };

	void				Instantiate();
	void				_SetBlendAlpha( REAL32 val);
	void				_InitDrop();

	bool				_IsValidPickedState();
	void				_ProcessPicked();

	REAL32				m_debug_time = 0.0f;

	static INT32		s_effect_idx[DINOKILLITEM_COUNT];
public:

	void				Create( i3GameResChara* pRes);
	virtual	void		OnUpdate( REAL32 rDeltaSeconds) override;

	void				UpdateAlpha( REAL32 fDeltaSeconds);
	void				UpdateFly( REAL32 fDeltaSeconds);
	virtual bool		CheckForItemPickUp( REAL32 fDeltaSeconds);
	virtual void		process_authentication( CGameCharaBase* p) {}

	void				SetFalling( VEC3D* pDir, REAL32 fSpeed, VEC3D* pStart, VEC3D* pDest = nullptr);				
	void				Disappear();
	void				pick_effect( CGameCharaBase* p);

	void				SetItemType( INT32 nType) { m_nItemType = nType; }
	INT32				GetItemType() const { return m_nItemType; }

	void				SetCode( INT32 nCode) { m_nNetCode = nCode; }
	INT32				GetCode() const { return m_nNetCode; }
	
	void				SetEffectIdx( INT32 idx) { m_effect_idx = idx;}
	INT32				GetEffectIdx() const { return m_effect_idx;}

	void				SetLifeTime( REAL32 fTime) { m_fLifeTime = fTime; }
	REAL32				GetLifeTime() const { return m_fLifeTime; }

	void				SetFree( bool bFree) { m_bFree = bFree; }
	bool				GetFree() const { return m_bFree; }

	bool				IsDroped() const { return m_bDroped; }


	static void			effect( CGameCharaBase* p, DINOKILLITEM e);
};
