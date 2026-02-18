/**
 * \file	GameCharaUpdateContext.h
 *
 * Declares the game chara update context class.
 */


#if !defined( __GAME_CHARA_UPDATE_CONTEXT_H__)
#define __GAME_CHARA_UPDATE_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaUpdater;
class CGameCharaUpdater1PV;
class CGameCharaUpdater3PV;
class CGameCharaUpdater3PVProfile;

class CGameCharaUpdaterFullbody;

class CGameCharaUpdateContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaUpdateContext, CGameCharaContextBase);
protected:
	// Updater
	CGameCharaUpdater *				m_pCurrentUpdater = nullptr;
	CGameCharaUpdater1PV *			m_p1PVUpdater = nullptr;
	CGameCharaUpdater3PV *			m_p3PVUpdater = nullptr;
	CGameCharaUpdater3PVProfile *	m_p3PVProfileUpdater = nullptr;
	CGameCharaUpdaterFullbody *		m_pFullbodyUpdater = nullptr;
	REAL32			m_rHideTimer = 0.0f;
	REAL32			m_rToHideTime = 0.0f;

	// 캐릭터 사망시 높이 블랜드 타임 (PhysX off에서만 동작)
	REAL32			m_rDeathBlendTime = 0.0f;
	// 캐릭터 사망시 이동할 높이값 (PhysX off에서만 동작)
	REAL32			m_rDeathBlendHeight = 0.0f;
	// 캐릭터 사망시의 높이 (PhysX off에서만 동작)
	REAL32			m_rDeathOriginalHeight = 0.0f;

	REAL32			m_rDeathTime = 0.0f;		// 제압에서 AI캐릭터 죽은 뒤 일정시간이 지나면 캐릭터 삭제할때 쓰임

public:
	/** \brief Hide time을 반환합니다.
		\note hide time : 캐릭터 사망 후 사라지는 시간 */
	REAL32			getHideTimer( void)							{ return m_rHideTimer; }

	/** \brief Hide time을 설정합니다. */
	void			setHideTimer( REAL32 tm)					{ m_rHideTimer = tm; }
	
	/** \brief Hide 목표 시간을 반환합니다. */
	REAL32			getToHideTime( void)						{ return m_rToHideTime; }

	/** \brief Hide 목표 시간을 설정합니다. */
	void			setToHideTime( REAL32 tm)					{ m_rToHideTime = tm; }

	/** \brief Death Blend time을 반환합니다.
		\note 데스 애니메이션을 사용하는 경우 높은 곳에서 사망시 사용합니다.
			지면까지 Blend를 계산할때 사용합니다. */
	REAL32			getDeathBlendTime( void)					{ return m_rDeathBlendTime; }

	/** \brief Death Blend time을 설정합니다. */
	void			setDeathBlendTime( REAL32 tm)				{ m_rDeathBlendTime = tm; }

	/** \brief Death Blend 높이를 반환합니다. */
	REAL32			getDeathBlendHeight( void)					{ return m_rDeathBlendHeight; }

	/** \brief Death Blend 높이를 설정합니다. */
	void			setDeathBlendHeight( REAL32 height)			{ m_rDeathBlendTime = 0.f; m_rDeathBlendHeight = height; }

	/** \brief 죽을 시 지면으로부터의 높이를 반환합니다. */
	REAL32			getDeathOriginalHeight( void)				{ return m_rDeathOriginalHeight; }

	/** \brief 죽을 시 지면으로부터의 높이를 설정합니다. */
	void			setDeathOriginalHeight( REAL32 height)		{ m_rDeathOriginalHeight = height; }

	void			UpdateDeathTime(REAL32 time)				{ m_rDeathTime	+= time;		}
	REAL32			getDeathTime(void) const					{ return m_rDeathTime;			}

	CGameCharaUpdater * getUpdater( void)			{ return m_pCurrentUpdater; }

public:
	virtual ~CGameCharaUpdateContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;
	virtual void	OnChangeProfile() override;
	virtual void	OnChangeFullbody() override;
	
	virtual void	OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;
	virtual void	OnLastUpdate( REAL32 rDeltatime) override;
};

#endif
