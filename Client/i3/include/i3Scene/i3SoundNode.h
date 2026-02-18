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
	I3SG_NODE_CLASS_DEFINE( i3SoundNode, i3Node);

protected:
	// Persistent Members
	i3SoundPlayInfo *	m_pSndPlayInfo = nullptr;
	REAL32				m_timeStart = 0.0f;
	UINT32				m_Style = 0;
	UINT32				m_LoopCount = 0;

	// Volatile Members
	i3SoundPlayState *	m_pSndPlayState = nullptr;
	REAL32				m_timeAccm = 0.0f;
	INT32				m_PlayedCount = 0;
	UINT32				m_lastFrameID = 0xFFFFFFFF;

public:
	i3SoundNode(void);
	virtual ~i3SoundNode(void);

	i3SoundPlayInfo *	getSoundPlayInfo(void)			{ return m_pSndPlayInfo; }
	void				setSoundPlayInfo( i3SoundPlayInfo * pSnd, i3ResourceFile * pFile = nullptr);
	void				setSoundPlayInfoFromXML(i3SoundPlayInfo * pSnd, i3XMLFile * pFile = nullptr);

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
	virtual void		PreTrace(i3SceneTracer * pTracer) override;

	void				Reset(void);
	virtual bool		OnQueryDeactivateNode() override;

public:
	void				_StopSound(bool bForce = false);

	virtual void		OnChangeTime( REAL32 tm) override;
	virtual void		OnDeactivate(void) override;
	virtual void		OnActivate(void) override;
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
