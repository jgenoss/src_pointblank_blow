#if !defined( _I3_SOUND_OBJECT_H__)
#define _I3_SOUND_OBJECT_H__

#include "i3Base.h"
#include "i3SoundNode.h"
#include "i3SceneTracer.h"

class I3_EXPORT_SCENE i3SoundObject : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3SoundObject);

protected:
	// Persistent Members
	i3SoundNode		*		m_pSoundNode;
	REAL32					m_rResponseRange;			//소리를 들을수 있는 범위( 거리)

public:
	i3SoundObject(void);
	virtual ~i3SoundObject(void);

	void				SetSoundNode( i3SoundNode* pNode);
	i3SoundNode	*		getSoundNode( void)					{	return m_pSoundNode;}

	REAL32				getResponseRange( void)				{	return m_rResponseRange;}
	void				SetResponseRange( REAL32 rRange);

	void				Drive( i3SceneTracer * pTracer);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif // if !defined( _I3_SOUND_OBJECT_H__)