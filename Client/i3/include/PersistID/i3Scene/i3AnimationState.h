#if !defined( __I3_ANIMATION_STATE_H)
#define __I3_ANIMATION_STATE_H

#include "i3Animation.h"

enum I3_ANIM_STATE
{
	I3_ANIM_STATE_NONE				= 0,
	I3_ANIM_STATE_PLAY,
	I3_ANIM_STATE_AWAIT,
	I3_ANIM_STATE_END,
};

class I3_EXPORT_SCENE i3AnimationState : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3AnimationState);
protected:
	i3Animation *				m_pAnim;
	I3_ANIM_BLEND_MODE			m_BlendMode;
	I3_ANIM_PLAY_MODE			m_SrcPlayMode;
	I3_ANIM_PLAY_MODE			m_DestPlayMode;
	REAL32						m_timeStart;
	REAL32						m_timeInterval;
	I3_ANIM_STATE				m_State;

public:
	i3AnimationState(void);
	virtual ~i3AnimationState(void);

	i3Animation *		GetAnimation(void)							{ return m_pAnim; }
	void				SetAnimation( i3Animation * pAnim);

	I3_ANIM_BLEND_MODE	GetBlendMode(void)							{ return m_BlendMode; }
	void				SetBlendMode( I3_ANIM_BLEND_MODE mode)		{ m_BlendMode = mode; }

	I3_ANIM_PLAY_MODE	GetSourcePlayMode(void)						{ return m_SrcPlayMode; }
	void				SetSourcePlayMode( I3_ANIM_PLAY_MODE mode)	{ m_SrcPlayMode = mode; }

	I3_ANIM_PLAY_MODE	GetDestPlayMode(void)						{ return m_DestPlayMode; }
	void				SetDestPlayMode( I3_ANIM_PLAY_MODE mode)	{ m_DestPlayMode = mode; }

	REAL32				GetStartTime(void)							{ return m_timeStart; }
	void				SetStartTime( REAL32 tm)					{ m_timeStart = tm; }

	REAL32				GetBlendInterval(void)						{ return m_timeInterval; }
	void				SetBlendInterval( REAL32 tm)				{ m_timeInterval = tm; }

	I3_ANIM_STATE		GetState(void)								{ return m_State; }
	void				SetState( I3_ANIM_STATE state)				{ m_State = state; }

	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
