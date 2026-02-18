#if !defined( __BATTLEHUD_CHARA_STATE_H)
#define __BATTLEHUD_CHARA_STATE_H

#if 지워질_것들

#include "BattleGui.h"
#include "Weapon/WeaponOnObject.h"
#include "BattleHUD_WeaponHeatGauge.h"
#include "BattleHUD_Define.h"

enum HUD_CSSPR
{
	// 상태변화가 없는 Sprite
	HUD_CSSPR_STATUS_SLOT = 0,				// CharaState Panel Frame

	HUD_CSSPR_KNIFE,
	HUD_CSSPR_GRENADE,
	HUD_CSSPR_GRENADE_ETC,
	HUD_CSSPR_BOMB,						// 폭탄 이미지

	// 상태변화가 있는 Sprite
	HUD_CSSPR_CHAR_STATE,

	HUD_CSSPR_HELMET,
	HUD_CSSPR_POINT,
	HUD_CSSPR_C5,							// C5 사용 여부
	HUD_CSSPR_C5_BULLET,					// HUD C5의 탄환쪽 이미지
	HUD_CSSPR_MINIAXE,
	HUD_CSSPR_BALLISTIC,



	HUD_CSSPR_COUNT,
};

enum HUD_CSSPR2
{
	HUD_CSSPR2_CHAR_STATE2,
	HUD_CSSPR2_DINO_SCRACH,
	HUD_CSSPR2_DINO_BITE,
	HUD_CSSPR2_DINO_HOWL,
	HUD_CSSPR2_DINO_SPIT,
	HUD_CSSPR2_DINO_POISON,
	HUD_CSSPR2_DINO_BITE2,		//티라노의 위치조정을위해 생성
	HUD_CSSPR2_DINO_BUTT,
	HUD_CSSPR2_DINO_DASH,		
	HUD_CSSPR2_DINO_EXPLOSION,		
#ifdef CC_DINO
	HUD_CSSPR2_CC_DINO_SCRACH,
	HUD_CSSPR2_CC_DINO_BITE,
	HUD_CSSPR2_CC_DINO_SPIT,
	HUD_CSSPR2_CC_DINO_EXPLOSION,
#endif

	HUD_CSSPR2_COUNT
};

enum HUD_CSSPR3
{
	HUD_CSSPR3_ELITE_SCRACH,
	HUD_CSSPR3_ELITE_BUFF,
#ifdef CC_DINO
	HUD_CSSPR3_CC_RAPTOR_BUFF,
#endif

	HUD_CSSPR3_COUNT
};


	enum HUD_RECOVER_TYPE
	{
		HUD_RECOVER_HP		= 0,
		HUD_RECOVER_AMMO,

		HUD_RECOVER_MAX
	};



#define HUD_BULLET_ANIM_COUNT	10

class CHud;

class CHUD_CharaState : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_CharaState, i3GameObjBase);
protected:
	CHud *			m_pHUD;

	i3Texture *		m_pTex;
	i3Texture *		m_pTexHUD2;
	i3Texture *		m_pTexHUD3;
	i3Sprite2D *	m_pSprite;

	i3TextNodeDX2 *	m_pTextHP;
	i3TextNodeDX2 *	m_pTextBullet;
	i3TextNodeDX2 *	m_pTextWeapon;
	i3TextNodeDX2 *	m_pTextDinoWeapon;
	i3TextNodeDX2 *	m_pTextDinoWeapon2;
	i3TextNodeDX2 *	m_pTextPointTotal;
	i3TextNodeDX2 *	m_pTextPoint;

	UINT32			m_nCurWeaponFlag;
	BOOL			m_bIsEnduranceZeroWeapon;
	REAL32			m_fEnduranceZeroWeaponBlinkTime;

	// 공룡
	i3Sprite2D *	m_pSprHUD2;
	i3Sprite2D *	m_pSprHUD3;

	i3TextNodeDX2 *	m_pTextHPPercent;

	i3Sprite2D *	m_pSpriteBulletGage1;
	i3Sprite2D *	m_pSpriteBulletGage2;
	i3Sprite2D *	m_pSpriteBullet;

	BOOL			m_bUpdateHelmet;
	BOOL			m_bUpdateC5;

	INT32			m_nOldHp;

	bool			m_bUpdateCharaHpState;
	REAL32			m_UpdateCharaHpStateElapsed;
	INT32			m_nPlayerTotalPoint;
	REAL32			m_fPlayerPointTimeDelta;

	// 캐릭터의 상태 관련
	CHARA_ACTION_BODYLOWER	m_OldCharaLowerState;
	WEAPON_SLOT_TYPE		m_OldWeaponUsageType;

	// 듀얼탄창 관련
	BOOL			m_bEnableDualGage;
	INT32			m_bActivateDualIdx;

	INT32			m_nOldBulletCount;
	INT32			m_nOldBulletDualCount;
	WEAPON_CLASS_TYPE m_OldBulletClass;
	INT32			m_nOldBulletNumber;

	WeaponOnObject *	m_pM197;
	CHUD_WeaponHeatGauge *	m_pM197GaugeHUD;


	// 제압모드
	const static INT32	RECOVER_EFFT_MAX	= 5;


	i3TextNodeDX2*		m_pTextLife;

	i3Sprite2D*			m_psprtRecoverImg[RECOVER_EFFT_MAX];	// 캐릭터 정보창으로 이동하는 이펙트
	VEC2D				m_v2RecoverEfftCur[HUD_RECOVER_MAX][RECOVER_EFFT_MAX];
	INT32				m_nRecoverIndex[HUD_RECOVER_MAX];

	VEC2D				m_v2RecoverImgStart;
	VEC2D				m_v2RecoverImgEnd;
	VEC2D				m_v2RecoverImgSpd;
	VEC2D				m_v2RecoverImgSize;

	i3Sprite2D*			m_psprtRecoverEfft;						// 캐릭터 정보창에서 깜빡이는 이펙트
	INT32				m_nRecoverEfftCount[HUD_RECOVER_MAX];
	REAL32				m_rRecoverEfftTime[HUD_RECOVER_MAX];

	void	(CHUD_CharaState::*m_pfUpdateRecoverMove[HUD_RECOVER_MAX][RECOVER_EFFT_MAX])( REAL32, INT32, INT32 );
	void	(CHUD_CharaState::*m_pfUpdateRecoverEfft[HUD_RECOVER_MAX])( REAL32, INT32 );

protected:
	void			UpdateStandState(void);
	bool			UpdateCharaHpState(REAL32 tm);
	void			UpdateHp(REAL32 tm);
	void			UpdatePlayerPoint( REAL32 rDeltaSeconds);
	void			UpdateWeaponInfo( REAL32 rDeltaSeconds);
	void			UpdateBulletAnim( REAL32 rDeltaSeconds );

	void			UpdateRecoverEffect( REAL32 rDeltaSeconds );

	void			UpdateRecoverEfftMoveNull( REAL32 tm, INT32 type, INT32 nIdx )	{}
	void			UpdateRecoverEfftMove( REAL32 tm, INT32 type, INT32 nIdx );
	void			UpdateRecoverEfftVisibleNull( REAL32 tm, INT32 type )		{}
	void			UpdateRecoverEfftVisible( REAL32 tm, INT32 type );
	void			UpdateRecoverEfftNonVisible( REAL32 tm, INT32 type );


	// Equip
	void			UpdateEquipHelmet( REAL32 rDeltaSeconds );

	// KEYWORD : WEAPON_C5
	void			UpdateEquipC5( REAL32 rDeltaTime);

	void			SetBulletGage(void);

	////////////////////////////////////////////////////////////////
	//	듀얼탄창
	////////////////////////////////////////////////////////////////
	void			SetDualGage( BOOL bFalg);
	void			ChageDualGage( INT32 nID);

	void			CheckWeaponChange(CWeaponInfo* pInfo);

	INT32			_GetCurrentSlotIndex( void);
	CGameCharaBase * _GetCurrentChara( void);

public:
	CHUD_CharaState(void);
	virtual ~CHUD_CharaState(void);

	bool			Create( CHud * pHud);
	void			Init(void);
	void			Reset(void);

	void			SetEnableCharaState( BOOL bFlag);
	void			SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath = FALSE);

	void			setOldLoadedBullet( INT32 nCount )						{ m_nOldBulletCount = nCount; }

	void			SetHudMode( HUD_MODE mod);

	void			SetRecover( HUD_RECOVER_TYPE type );

	void			UpdateBullet(void);

	virtual void	OnUpdate( REAL32 tm);
};

#endif

#endif
