/**
 * \file	GameCharaHUDContext.h
 *
 * Declares the game chara hud context class.
 */


#if !defined( __GAME_CHARA_HUD_CONTEXT_H__)
#define __GAME_CHARA_HUD_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaHUDControl;
class CGameCharaHUDControlDino;
class CGameCharaHUDControlRaptor;
class CGameCharaHUDControlTRex;
class CGameCharaHUDControlSting;
class CGameCharaHUDControlElite;
class CHud;


class CGameCharaHUDContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaHUDContext, CGameCharaContextBase);

	friend class Avatar;
	friend class CGameCharaBase;
	friend class CGameCharaDino;
	friend class CGameCharaDinoRaptor;
	friend class CGameCharaDinoTRex;
	friend class CGameCharaDinoSting;
	friend class CGameCharaDinoAcid;
	friend class CGameCharaDinoTank;
	friend class CGameCharaDinoElite;

	friend class CGameCharaDinoCCRaptor;
	friend class CGameCharaDinoCCSting;
	friend class CGameCharaDinoCCAcid;
	friend class CGameCharaDinoRaptorMercury;
	friend class CGameCharaDinoStingMars;
	friend class CGameCharaDinoAcidVulcan;

private:
	CGameCharaHUDControl *		m_pCurrentControl = nullptr;
	CGameCharaHUDControl *		m_pHUDControl = nullptr;
	CGameCharaHUDControlRaptor *m_pRaptorHUDControl = nullptr;
	CGameCharaHUDControlTRex *	m_pTRexHUDControl = nullptr;
	CGameCharaHUDControlSting *	m_pStingHUDControl = nullptr;
	CGameCharaHUDControlElite *	m_pEliteHUDControl = nullptr;

	COLOR				m_EFFECT_ScreenColor = { 0, 0, 0, 255 };
	REAL32				m_fBlurStart = 0.0f;
	REAL32				m_fBlurEnd = 0.0f;
	SCR_EFFECT_TYPE		m_nScrEffectType = SCR_EFFECT_NONE;

	REAL32				m_rFlashElapseTime = 0.0f;				// 섬광 효과 계산용 타임
	REAL32				m_rFlashEffectiveTime = 0.0f;			// 섬광 지속 총시간(총시간이 계산되어서 저장된다)
	REAL32				m_rDamageElapseTime = 0.0f;				// 데미지 지속 효과
	REAL32				m_rBlowElapseTime = 0.0f;				// 섬광 효과 계산용 타임
	REAL32				m_rBlowEffectiveTime = 0.0f;			// 섬광 지속 총시간(총시간이 계산되어서 저장된다)
	
private:
	/** \brief World좌표에 대한 캐릭터를 기준으로 2D 좌표를 계산합니다. */
	bool			__CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn );


public:
	REAL32			getDamageElapseTime( void)		{ return m_rDamageElapseTime; }

public:
	CGameCharaHUDContext() {}
	virtual ~CGameCharaHUDContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnChange1PV( void) override;
	virtual void	OnChangeTrex() override;
	virtual void	OnChangeSting() override;
	virtual void	OnChangeRaptor() override;
	virtual void	OnChangeElite();

	virtual void	OnChangeCCRaptor();
	virtual void	OnChangeCCSting();
	virtual void	OnChangeCCAcid();
	virtual void	OnChangeRaptorMercury();
	virtual void	OnChangeStingMars();
	virtual void	OnChangeAcidVulcan();


	virtual void	OnReset( void) override;

	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief ScreenEffect 사용 가능 여부 */
	bool			IsEffectAvailable( void);

	/** \brief ScreenEffect를 초기화한다. */
	void			ClearScreenEffect( void);

	/** \brief ScreenEffect Flashbang을 설정한다.
		\param[in] pVecWeaponPos flashbang 위치
		\param[in] rEffectiveTime 지속 시간
		\param[in] rExplosionRange 폭발 거리
		\param[in] rExplosionSlope 강도
		\param[in] rDamagedLength 타격 거리 */
	bool			ScreenEffect_Flashbang( VEC3D * pVecWeaponPos, REAL32 rEffectiveTime, REAL32 rExplosionRange, REAL32 rExplosionSlope, REAL32 rDamagedLength);
	
	/** \brief Jumper 효과를 설정한다.
		\param[in] rProcessTime 지속 시간 */
	void			ScreenEffect_Jumper( REAL32 rProcessTime);

	void			ScreenEffect_Blow( REAL32 rProcessTime);

	/** \brief Howling 효과를 설정한다.
		\param[in] rEffectTime 지속 시간 */
	bool			ScreenEffect_Howl( REAL32 rEffectTime);

	/** \brief NickName 표시를 설정한다.
		\param[in] bFriendly 아군 여부		*/
	void			ShowCharacterNick( bool bFriendly);

	/** \brief Scope를 설정한다. 
		\param[in] bEnable 사용 유무
		\param[in] pWeapon 무기 pointer		*/
	void			SetScope( bool bEnable, WeaponBase * pWeapon = nullptr);

	/** \brief 미션용 message를 설정한다.
		\param[in] pszMessage
		\param[in] lifeTime 라이프 타임
		\param[in] offsetX X 좌표
		\param[in] offsetY Y 좌표 */
	void			ShowMissionNotice( const i3::rc_wstring& wstrMessage, REAL32 lifeTime = 3.f, INT32 offsetX = 0, INT32 offsetY = 0);

	/** \brief 무기 변경시 호출 */
	void			ChangeWeapon( void);

	SCR_EFFECT_TYPE GetCurScreenEffect() const { return m_nScrEffectType; }

protected:
	/** \brief Crosshair를 설정한다. */
	void			_ApplyCrosshair( void);

	/** \brief Damage 효과를 표시한다.
		\param[in] pDir 방향
		\param[in] bDirectionView Damage 방향 표시 유무		*/
	void			_Damage( VEC3D * pDir = nullptr, bool bDirectionView = false, INT32 filter_index = -1 );
};

#endif
