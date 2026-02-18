#pragma once

//구버전에서 사용하던 이넘값들.
//예외처리 컨버팅등을 위하여 남겨둠.
enum HUD_SCOPE_TYPE
{
	HUD_SCOPE_TYPE_NONE = -1,
	HUD_SCOPE_TYPE_DEFAULT = 0,				// Sniper Scope
	HUD_SCOPE_TYPE_SCOPE2,					// Assault Scope
	HUD_SCOPE_TYPE_SCOPE3,					// F2000
	HUD_SCOPE_TYPE_SCOPE4,					// Assault Scope + Red Dot Fixed
	HUD_SCOPE_TYPE_SCOPE4_DOTFREE,			// Assault Scope + Red Dot
	HUD_SCOPE_TYPE_DOTSIGHT,				// Dotsight
	HUD_SCOPE_TYPE_DOTSIGHT2,				// Dotsight + Red Dot
	HUD_SCOPE_TYPE_DOTSIGHT3,				// Red Dot
	HUD_SCOPE_TYPE_ROCKET_LAUNCHER,			// RPG7
	HDD_SCOPE_TYPE_M197,					// M197
	HUD_SCOPE_TYPE_PBTC2015,				// PBTC2015
	HUD_SCOPE_TYPE_M14EBR,					// M14 EBR
	HUD_SCOPE_TYPE_KOBRA,					// AK_12
	HUD_SCOPE_TYPE_GRS,						// GRS용 Scope + Red Dot
	HUD_SCOPE_TYPE_GRS2,					// GRS용 Scope

											// Demonic용 HUD
	HUD_SCOPE_TYPE_DEMONIC_DOTSIGHT,		// Demonic RedDot
	HUD_SCOPE_TYPE_DEMONIC_DEFAULT,			// Demonic Default Scope
	HUD_SCOPE_TYPE_DEMONIC_P90,				// Demonic P90 RedDot
	HUD_SCOPE_TYPE_DEMONIC_DOTSIGHT2,		// Demonic Dotsight + Scope

											// Dolphin 용 HUD Type
	HUD_SCOPE_TYPE_DOLPHIN_DEFAULT,
	HUD_SCOPE_TYPE_DOLPHIN_DOTSIGHT,
	HUD_SCOPE_TYPE_DOLPHIN_DOTSIGHT2,
	HUD_SCOPE_TYPE_DOLPHIN_G36C,
	HUD_SCOPE_TYPE_DOLPHIN_P90,

	HUD_SCOPE_TYPE_GSL2016_DOTSIGHT,		// GSL2016 AUG
	HUD_SCOPE_TYPE_GSL2016_DOTSIGHT2,		// GSL2016 Kriss

	HUD_SCOPE_TYPE_MIDNIGHT2_DOTSIGHT,		// Midnight2 Aug
	HUD_SCOPE_TYPE_MIDNIGHT2_DOTSIGHT2,		// Midnight2 Kriss

	HUD_SCOPE_TYPE_PBTC2016,
	HUD_SCOPE_TYPE_SIG,						//SIG-Sauer_MCX

	HUD_SCOPE_TYPE_CRISTMAS2016_CAINGUN,

	HUD_SCOPE_TYPE_ICE_DEFAULT = 31,
	HUD_SCOPE_TYPE_ICE_DOTSIGHT,			//AUG A3 Ice
	HUD_SCOPE_TYPE_ICE_DOTSIGHT2,			//Kriss Ice
	HUD_SCOPE_TYPE_ICE_G36,
	HUD_SCOPE_TYPE_ICE_P90,

	HUD_SCOPE_TYPE_HYBRIDMAN_DOTSIGHT = 36,		//AUG A3 HYBRIDMAN
	HUD_SCOPE_TYPE_HYBRIDMAN_DOTSIGHT2,		//Kriss HYBRIDMAN, OA-93 HYBRIDMAN	

	HUD_SCOPE_TYPE_LIGHTNING_DEFAULT =38,
	HUD_SCOPE_TYPE_LIGHTNING_G36,
	HUD_SCOPE_TYPE_LIGHTNING_P90,

	HUD_SCOPE_TYPE_COUNT,
};

class CHUD_Scope : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_Scope, i3GameObjBase);

public:
	CHUD_Scope();
	~CHUD_Scope();

public:
	static CHUD_Scope* instance();

public:
	void	CreateScopeHUD( i3Node * pRoot);

	void	SetSniperScope( bool bVisible, CWeaponInfo * pWeaponInfo = nullptr, WeaponBase * pWeapon = nullptr); // revision 64440 PP-19 레드닷 예외처리 추가
	bool	IsEnabledSniperScope(void)	{	return m_bEnabledSniperScopeHUD;}
	void	UpdateSniperScope( REAL32 rDeltaTime);

	void	SetEnableSinperScope( bool bFlag);

	void	InsertScopeHudTexture(INT32 index);

private:
	i3::vector<i3Texture*>  m_vecScopeHudTexture;
	i3Sprite2D *	m_pSpriteScope;
	i3Texture *		m_pDefaultTex;

	bool			m_bEnabledCenterImage;
	bool			m_bEnabledSniperScopeHUD;
	bool			m_bEnablebScopeBreath;
	REAL32			m_rElapsedScopeEnable;

	INT32			m_nAspectScreenWidth;
	INT32			m_nAspectScreenHeight;
	INT32			m_nAspectOffsetWidth;
};
