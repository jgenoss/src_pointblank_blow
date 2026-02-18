#if !defined( _OBJECT_HELICOPTER_H__)
#define _OBJECT_HELICOPTER_H__

#include "GameControledObject.h"

class CGameObjectHelicopter : public CGameControledObject
{
	I3_CLASS_DEFINE( CGameObjectHelicopter, CGameControledObject);

protected:
	enum HELICOPTER_ANIM
	{
		HA_NONE = -1,
		HA_IDLE,
		HA_HIT,
		HA_CRASH,

		HA_COUNT,
	};

	VEC3D					m_vFieldPos;

	HELICOPTER_ANIM			m_AnimKey;

	i3Animation *			m_pAnim[HA_COUNT];

	REAL32					m_rAnimLocalTime;

	i3TimeStamp *			m_pUH60_DustEffect;

public:
	CGameObjectHelicopter( void);
	virtual ~CGameObjectHelicopter( void);

public:
	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;

	virtual void	SetControledObjectState( INT32 state) override;

public:
	void					InitAnimation( void);
	void					Destroy(void);

	HELICOPTER_ANIM	getAnimationKey(void)								{ return m_AnimKey;}
	void					setAnimationKey( HELICOPTER_ANIM animKey)	{ m_AnimKey = animKey;}

	REAL32					getAnimLocalTime( void)						{ return m_rAnimLocalTime; }
	void					setAnimLocalTime( REAL32 tm)				{ m_rAnimLocalTime = tm; }
	void					addAnimLocalTime( REAL32 rDelta)			{ m_rAnimLocalTime += rDelta; }

	void					SetAnimationKey( HELICOPTER_ANIM animKey);

	void					EnableDustEffect( bool bEnable);

	void					Process( REAL32 rDeltaSeconds);
};

#endif