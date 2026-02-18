#if !defined( __I3_SOUND_NODE_H)
#define __I3_SOUND_NODE_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3SoundH.h"
#include "i3SceneTracer.h"

#define	I3SG_SNDNODE_STYLE_LOOP				0x00000001
#define	I3SG_SNDNODE_STYLE_POSITIONAL		0x00000002

class I3_EXPORT_SCENE i3SoundNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3SoundNode);

protected:
	// Persistent Members
	i3SoundPlayInfo *	m_pSndPlayInfo;
	REAL32				m_timeStart;
	UINT32				m_Style;
	UINT32				m_LoopCount;

	// Volatile Members
	i3SoundPlayState *	m_pSndPlayState;
	REAL32				m_timeAccm;
	INT32				m_PlayedCount;
	UINT32				m_lastFrameID;

public:
	i3SoundNode(void);
	virtual ~i3SoundNode(void);

	i3SoundPlayInfo *	getSoundPlayInfo(void)			{ return m_pSndPlayInfo; }
	void				setSoundPlayInfo( i3SoundPlayInfo * pSnd);

	REAL32				getStartTime(void)				{ return m_timeStart; }
	void				setStartTime( REAL32 tm)		{ m_timeStart = tm; }

	UINT32				getStyle(void)					{ return m_Style; }
	void				setStyle( UINT32 style)			{ m_Style = style; }
	void				addStyle( UINT32 style)			{ m_Style |= style; }
	void				removeStyle( UINT32 style)		{ m_Style &= ~style; }

	UINT32				getLoopCount(void)				{ return m_LoopCount; }
	void				setLoopCount( UINT32 loop)		{ m_LoopCount = loop; }

	i3SoundPlayState *	getSoundPlayState(void)			{ return m_pSndPlayState; }

	void				Drive( i3SceneTracer * pTracer);

	void				Reset(void);
	virtual bool		OnQueryDeactivateNode();

public:
	void				_StopSound(bool bForce = false);

	virtual void		OnChangeTime( REAL32 tm);
	virtual void		OnDeactivate(void);
	virtual void		OnActivate(void);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
