#if !defined( __I3_TIME_SEQUENCE_H)
#define __I3_TIME_SEQUENCE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Node.h"
#include "i3TimeSequenceInfo.h"

class I3_EXPORT_SCENE i3TimeSequence : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3TimeSequence, i3Node);

protected:
	i3::vector<i3TimeSequenceInfo*>	m_SeqList;

	// Volatile Members
	REAL32			m_LocalTime = 0.0f;
	REAL32			m_PrevLocalTime = 0.0f;
	REAL32			m_Duration = 0.0f;
	bool			m_bActive = true;
	bool			m_bInfinite = false;
	bool			m_bRepeat = false;
	UINT32			m_lastFrameID = 0xFFFFFFFF;

	REAL32			m_TimeScale = 1.0f;

protected:
	void			UpdateDuration(void);

public:
	i3TimeSequence(void);
	virtual ~i3TimeSequence(void);

	INT32					GetSequenceCount(void)		{ return (INT32)m_SeqList.size(); }
	i3TimeSequenceInfo *	GetSequence( INT32 idx)		{ return m_SeqList[idx]; }

	INT32					AddSequence( i3TimeSequenceInfo * pInfo);
	INT32					InsertSequence( INT32 idx, i3TimeSequenceInfo * pInfo);
	INT32					GetSequenceIndex( i3TimeSequenceInfo * pInfo);
	void					RemoveSequence( i3TimeSequenceInfo * pInfo);
	void					DeleteSequence( INT32 idx);
	void					RemoveAllSequence(void);

	bool					IsActive(void)				{ return m_bActive; }

	void					setTimeScale( REAL32 scale)	{	m_TimeScale = scale;}
	REAL32					getTimeScale(void)			{	return m_TimeScale;}

	REAL32					GetLocalTime(void)			{ return m_LocalTime; }
	void					SetLocalTime( REAL32 tm)	{ m_LocalTime = tm; }

	REAL32					getDuration(void)			{ return m_Duration; }

	void					Drive( i3SceneTracer * pTracer);

	void					Dump(void);

	virtual void	OnChangeTime( REAL32 tm) override;
	virtual void	CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual	UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif



