#if !defined( _CGAMEOBJECT_WILDHOGCANNON_H__)
#define _CGAMEOBJECT_WILDHOGCANNON_H__

#include "GameControledObject.h"

#define MAX_HOGCANNON_IMPACT_COUNT	3

class CGameObjectWildHogCannon: public CGameControledObject
{
	I3_CLASS_DEFINE( CGameObjectWildHogCannon, CGameControledObject);

protected:
	enum WILDHOG_CANNON_ANIM
	{
		WHCA_NONE = -1,
		WHCA_IDLE,
		WHCA_ATTACK,
		WHCA_CRASH,

		WHCA_COUNT,
	};

	WILDHOG_CANNON_ANIM		m_AnimKey;

	i3Animation *			m_pAnim[WHCA_COUNT];

	REAL32					m_rAnimLocalTime;

	INT32					m_idxFxDummy;

	bool					m_bAttacking;

	bool					m_bFireBall;
	REAL32					m_rFireBallImpactTime;
	INT32					m_nImpactCount;
	VEC3D					m_vImpactPos[ MAX_HOGCANNON_IMPACT_COUNT];

public:
	CGameObjectWildHogCannon(void);
	virtual ~CGameObjectWildHogCannon(void);

public:
	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;

	virtual void	SetControledObjectState( INT32 state) override;
	virtual INT32	GetControledObjectState(void) override {	return m_AnimKey;}

public:
	void			_InitAnimation( void);
	void			_Destroy(void);

	void			_InitFx(void);

	WILDHOG_CANNON_ANIM	getAnimationKey(void)						{ return m_AnimKey;}
	void			setAnimationKey( WILDHOG_CANNON_ANIM animKey)	{ m_AnimKey = animKey;}

	REAL32			getAnimLocalTime( void)							{ return m_rAnimLocalTime; }
	void			setAnimLocalTime( REAL32 tm)					{ m_rAnimLocalTime = tm; }
	void			addAnimLocalTime( REAL32 rDelta)				{ m_rAnimLocalTime += rDelta; }

	void			SetAnimationKey( WILDHOG_CANNON_ANIM animKey);

	void			Process( REAL32 rDeltaSeconds);
	void			ProcessImpactBall( REAL32 rDeltatime);

public:
	void			_ProcessCamera( VEC3D * pPos, REAL32 rRange, REAL32 rDamage, REAL32 rSlop);
	void			_ProcessCharaHit( VEC3D * pPos, REAL32 rRange, REAL32 rDamage, REAL32 rSlop);
};

#endif //_CGAMEOBJECT_WILDHOGCANNON_H__