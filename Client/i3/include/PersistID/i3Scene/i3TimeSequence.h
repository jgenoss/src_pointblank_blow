#if !defined( __I3_TIME_SEQUENCE_H)
#define __I3_TIME_SEQUENCE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Node.h"
#include "i3TimeSequenceInfo.h"

class I3_EXPORT_SCENE i3TimeSequence : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3TimeSequence);

protected:
	i3List			m_SeqList;

	// Volatile Members
	REAL32			m_LocalTime;
	REAL32			m_Duration;
	BOOL			m_bActive;
	BOOL			m_bInfinite;
	BOOL			m_bRepeat;
	UINT32			m_lastFrameID;

protected:
	void			UpdateDuration(void);

public:
	i3TimeSequence(void);
	virtual ~i3TimeSequence(void);

	INT32					GetSequenceCount(void)		{ return m_SeqList.GetCount(); }
	i3TimeSequenceInfo *	GetSequence( INT32 idx)		{ return (i3TimeSequenceInfo *) m_SeqList.Items[idx]; }

	INT32					AddSequence( i3TimeSequenceInfo * pInfo);
	INT32					InsertSequence( INT32 idx, i3TimeSequenceInfo * pInfo);
	INT32					GetSequenceIndex( i3TimeSequenceInfo * pInfo);
	void					RemoveSequence( i3TimeSequenceInfo * pInfo);
	void					DeleteSequence( INT32 idx);
	void					RemoveAllSequence(void);

	BOOL					IsActive(void)				{ return m_bActive; }
	
	REAL32					GetLocalTime(void)			{ return m_LocalTime; }
	void					SetLocalTime( REAL32 tm)	{ m_LocalTime = tm; }

	REAL32					getDuration(void)			{ return m_Duration; }

	void					Drive( i3SceneTracer * pTracer);

	void					Dump(void);

	virtual void	OnChangeTime( REAL32 tm);
	virtual void	CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif



