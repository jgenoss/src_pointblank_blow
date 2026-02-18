#if !defined( _OBJECT_SIEGEGOLEM_H__)
#define _OBJECT_SIEGEGOLEM_H__

#include "GameControledObject.h"

class CGameObjectSiegeGolem : public CGameControledObject
{
	I3_CLASS_DEFINE( CGameObjectSiegeGolem, CGameControledObject);

protected:
	enum SIEGEGOLEM_ANIM
	{
		SGA_NONE = -1,
		SGA_IDLE,
		SGA_ATTACK,
		SGA_CRASH,
		SGA_ATTACK2,

		SGA_COUNT,
	};

	SIEGEGOLEM_ANIM			m_AnimKey;

	i3Animation *			m_pAnim[SGA_COUNT];

	REAL32					m_rAnimLocalTime;

	bool					m_bAttacking;

	i3Transform	*			m_pFxDummy[2];

public:
	CGameObjectSiegeGolem(void);
	virtual ~CGameObjectSiegeGolem(void);

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

	SIEGEGOLEM_ANIM	getAnimationKey(void)						{ return m_AnimKey;}
	void			setAnimationKey( SIEGEGOLEM_ANIM animKey)	{ m_AnimKey = animKey;}

	REAL32			getAnimLocalTime( void)						{ return m_rAnimLocalTime; }
	void			setAnimLocalTime( REAL32 tm)				{ m_rAnimLocalTime = tm; }
	void			addAnimLocalTime( REAL32 rDelta)			{ m_rAnimLocalTime += rDelta; }

	void			SetAnimationKey( SIEGEGOLEM_ANIM animKey);

	void			Process( REAL32 rDeltaSeconds);

public:
	void			_ProcessCharaHit( VEC3D * pPos, REAL32 rRange, REAL32 rDamage, REAL32 rSlop);
};

#endif