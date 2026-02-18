#pragma once

#include "BattleHUD_Define.h"
#include "StageObject/ObjectWeaponBox.h"

// 탭미니맵의 WAVE 동기화를 위해 추가. 2011.11.03. ugeun.ji
// * 탭미니맵은 활성화 때에만 Update 됩니다.
class DestroyObjectAttackState
{
public:
	DestroyObjectAttackState() {}
	virtual ~DestroyObjectAttackState() {}

	static bool IsUnderAttack(INT32 index);
	void SetUnderAttack(INT32 index, bool isAttacked);

private:
	static bool	_isRedUnderAttack;
	static bool _isBlueUnderAttack;
};

#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
class CBattleHUD_MiniMap;
class DomiSkillObjOldMinimapUnitMgr
{
	enum IMAGES_SKILLOBJ
	{
		IMAGE_SENTRY_DUMMY_DEACTIVATED,
		IMAGE_SENTRY_DUMMY_ACTIVATED,
		IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING,
		IMAGE_SENTRY_DUMMY_MAX
	};

	enum IMAGE_SKILLSHOP
	{
		IMAGE_SKILLSHOP_DEACTIVATED,
		IMAGE_SKILLSHOP_ACTIVATED,
		IMAGE_SKILLSHOP_MAX
	};

public:
	DomiSkillObjOldMinimapUnitMgr(CBattleHUD_MiniMap* minimap);
	~DomiSkillObjOldMinimapUnitMgr();

	void AddUnit(INT32 type, INT32 idx);
	void DeleteUnit(INT32 type, INT32 idx);
	void Update(REAL32 tm);

private:
	CBattleHUD_MiniMap*	m_minimap;

	struct Element
	{
		INT32		type;
		INT32		idx;
		i3Sprite2D* image;

		Element() : type(0), idx(0), image(0) {}
		~Element() { I3_SAFE_RELEASE(image); }
	};
	i3::vector<Element*> m_units;

	i3Sprite2D* MakeSentrygunImage();
	i3Sprite2D* MakeDummyImage(); 
	i3Sprite2D* MakeSkillShopImage();

	void UpdateSentrygunDummy(i3Sprite2D* image, INT32 type, INT32 idx);
	void UpdateSkillShop(i3Sprite2D* image, INT32 type, INT32 idx);
	void SetPosition(REAL32 theta, i3Sprite2D* image, INT32 type, VEC3D* pos);
};
#endif

class CStageBattle;
class CBattleHUD_MiniMap : public i3GameObjBase
{
	I3_CLASS_DEFINE( CBattleHUD_MiniMap, i3GameObjBase);

	friend class DomiSkillObjOldMinimapUnitMgr;

	enum	MINIMAP_MODE
	{
		MINIMAP_MODE_UNKNOWN = -1,
		MINIMAP_MODE_DEFAULT = 0,
		MINIMAP_MODE_FULLSCREEN,
	};

	enum	MINIMAP_SCALE_BUTTON
	{
		MINIMAP_SCALE_DISABLE = 0,
		MINIMAP_SCALE_INCREASE,
		MINIMAP_SCALE_DECREASE,

		MINIMAP_SCALE_COUNT,
	};

	enum	MINIMAP_CLEAVERKNIFE
	{
		MINIMAP_CLEAVERKNIFE_A = 0,
		MINIMAP_CLEAVERKNIFE_B,

		MINIMAP_CLEAVERKNIFE_MAX,
	};

public:
	CBattleHUD_MiniMap( void);
	~CBattleHUD_MiniMap( void);

public:
	static CBattleHUD_MiniMap* instance();

public:
	bool	Create( CStageBattle * pCurStage, i3Node* pParent);
	bool	CreateMiniMap();

	bool	IncreaseMiniMapScale( bool bIncrease, REAL32 rAmount = 0.1f);

	void	SetDestroyObjectUnderAttack( UINT32 nidx, bool bEnable);

	i3AttrSet* GetAttrRoot() const { return m_pAttrRoot; }

#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
	DomiSkillObjOldMinimapUnitMgr*			GetSkillObjUnitMgr()		{ return m_pDomiSkillObjUnitMgr; }
	const DomiSkillObjOldMinimapUnitMgr*	GetSkillObjUnitMgr() const	{ return m_pDomiSkillObjUnitMgr; }
#endif

protected:
	void	_InitViewport( void);
	void	_InitBGScene( void);
	void	_CalcMapScale( void);
	bool	_InitStageImage(void);
	bool	_CreateUnitTexture(void);
	bool	_CreateMissionObject(void);
	bool	_CreateDestroyMissionObject(void);
	bool	_CreateDefenceMissionObject( void);
	bool	_CreateUsurpationObject(void);

	bool	_InitMiniMapScaleButton(void);
	void	_InitMiniMapLocationText(void);
	bool	_InitC5PositionObject(void);
	
	void	_UpdateMiniMapMode( REAL32 rDeltaSeconds);
	void	_UpdateFullScreenMode( REAL32 rDeltaSeconds);
	bool	_UpdateMiniMapButton( REAL32 tm);
	bool	_ResetCurrentMiniMapButton(bool bOrigin);

	void	_UpdateUnit( REAL32 rTheta);
#ifdef DOMI_DINO_MINIMAP_UNIT
	void	_UpdateUnitDomiDino(REAL32 rTheta);
#endif
	void	_UpdateMissionObject( REAL32 rTheta);

	bool	_UpdateScale();
	void	_UpdateDeathUnit( REAL32 rDeltaSeconds);

	void	_GetOuterLocation( VEC3D vIn, VEC2D * vOut, VEC2D	vImgSz);

	void	_SetWavEnable(UINT32 idxObject,bool bEnable);
	void	_UpdateWav(REAL32 rDeltaSeconds);

	// KEYWORD : WEAPON_C5
	bool	_CreateWeaponC5Wave( void);
	void	_UpdateWeaponC5Wave( REAL32 rDeltaTime);

public:
	void	SetEnableMiniMap( bool bEnable = true);
	
	bool	SetMiniMapMode( MINIMAP_MODE mode, bool force=false);
	MINIMAP_MODE GetMiniMapMode() { return m_nCurrentMode; }

	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	void	SetCurrentFloorIdx(INT32 nFloor);

	void	SetCurrentLocationText( const i3::wliteral_range& wLocationRng);
	const i3::wstring&	getCurrentLocationText(void) const { return m_pCurLocationText->getTextString();	}

	MINIMAP_MODE	getCurrentMapMode(void)	{	return m_nCurrentMode;}

	void SetPosition(INT32 x, INT32 y);
	void SetSize(INT32 w, INT32 h);

protected:
	CStageBattle	*	m_pCurStage;
	i3AttrSet		*	m_pAttrRoot;
	i3ViewportAttr	*	m_pViewport;

	i3Texture		*	m_pHUDTexture;
	i3Texture		*	m_pHUD3Texture;

	INT32				m_idxFloor;

	i3Texture		*	m_pMinimapTexture;
	i3::vector<i3Texture*>	m_plistMiniMapTextures;

	//Text 추가되어서 추가됨 
	i3::vector<i3Texture*>	m_plistMiniMapTextTextures;
	i3Texture		*	m_pMinimapTextTexture; 

	i3Sprite2D *		m_pSprBackGround;
	i3Sprite2D *		m_pSprMapImage;
	i3Sprite2D *		m_pUnitImage;
	i3Sprite2D *		m_pOuterUnitImage;
	i3Sprite2D *		m_pDeathUnitImage;
	i3Sprite2D *		m_pDeathOuterUnitImage;
#ifdef DOMI_DINO_MINIMAP_UNIT
	i3Sprite2D *		m_pUnitDomiDino;
	i3Sprite2D *		m_pOutlineUnitDomiDino;
#endif

#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
	DomiSkillObjOldMinimapUnitMgr* m_pDomiSkillObjUnitMgr;
#endif

	i3Sprite2D *		m_pMiniMapScaleButton;

	bool				m_bChangedViewport;
	MINIMAP_MODE		m_nCurrentMode;

	//Mission Object
	i3Sprite2D *		m_pMissionObject;
	i3AttrSet	*		m_pMissionObjectWavRoot;
	i3Sprite2D	*		m_pMissionObjectWave;

	// 탈취 모드 미니맵UI 추가
	i3Sprite2D *		m_pUsurpationObject;
	CGameObjectWeaponBox* m_pCleaverKnife[MINIMAP_CLEAVERKNIFE_MAX];	

	// KEYWORD : WEAPON_C5
	i3AttrSet *			m_pWeaponC5WaveRoot;
	i3Sprite2D *		m_pWeaponC5WaveSprite;

	// Stage Info
	REAL32				m_rMapScale;

	VEC2D				m_vMiniMapScale;
	VEC2D				m_vHoldMapScale;

	REAL32				m_rHalfScreenX;
	REAL32				m_rHalfScreenY;

	REAL32				m_rMapCenterX;
	REAL32				m_rMapCenterY;

	REAL32				m_rScaleFactoX;
	REAL32				m_rScaleFactoY;

	REAL32				m_rMapWidth;
	REAL32				m_rMapHeight;

	REAL32				m_rMiniMapScale;
	REAL32				m_rMiniMapScaleElapsed;

	void		*		m_pMapInfo;

	REAL32				m_rMissionObjectWavScale;

	REAL32				m_rDeathUnitImageDeltaTime[MAX_ALL_USER];
	bool				m_bDeathUnitOnceUpdate[MAX_ALL_USER];

	// KEYWORD : WEAPON_C5
	REAL32				m_rC5WaveScaleTimer;
	REAL32				m_rC5WaveScaleMultiple;

	// Text
	i3TextNodeDX2 *		m_pCurLocationText;	

	DestroyObjectAttackState m_DestroyObjectAttackState;

#ifdef UITEST_MINIMAP_MOVE
	REAL32				m_minimapMoveFactorX;
	REAL32				m_minimapMoveFactorY;
#endif
};