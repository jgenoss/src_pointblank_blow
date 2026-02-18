#if !defined( __OBJECT_HATCHLING_H__)
#define __OBJECT_HATCHLING_H__

#include "GameControledObject.h"

class CGameObjectHatchling : public CGameControledObject
{
	I3_CLASS_DEFINE( CGameObjectHatchling, CGameControledObject );
protected:
	enum HATCHLING_ANIM
	{
		HATA_NONE = -1,
		HATA_IDLE,
		HATA_DAMAGE,

		HATA_COUNT,
	};

	HATCHLING_ANIM			m_AnimKey;

	i3Animation *			m_pAnim[HATA_COUNT];

	REAL32					m_rAnimLocalTime;

protected:
	void			_InitAnimation( void);

public:
	CGameObjectHatchling();
	virtual ~CGameObjectHatchling();

	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;

	virtual void	SetControledObjectState( INT32 state) override;
	virtual UINT32	getControlObjectState(void) { return (INT32)m_AnimKey;}
	

	HATCHLING_ANIM	getAnimationKey(void)						{ return m_AnimKey;}
	void			setAnimationKey( HATCHLING_ANIM animKey)	{ m_AnimKey = animKey;}

	REAL32			getAnimLocalTime( void)						{ return m_rAnimLocalTime; }
	void			setAnimLocalTime( REAL32 tm)				{ m_rAnimLocalTime = tm; }
	void			addAnimLocalTime( REAL32 rDelta)			{ m_rAnimLocalTime += rDelta; }

	void			SetAnimationKey( HATCHLING_ANIM animKey);
};

#endif
