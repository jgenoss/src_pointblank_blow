#if !defined( __I3_TIME_SEQUENCE_INFO_H)
#define __I3_TIME_SEQUENCE_INFO_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Node.h"

#define	I3SG_TIMESEQ_STATE_ACTIVE			0x00000001
#define	I3SG_TIMESEQ_STATE_SELECTED			0x00000002

#define I3SG_TIMESEQ_STYLE_DISABLE			0x00000001
#define I3SG_TIMESEQ_STYLE_REPEAT			0x00000002

class I3_EXPORT_SCENE i3TimeSequenceInfo : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3TimeSequenceInfo, i3PersistantElement);
protected:
	// Persistant Members
	REAL32			m_StartTime = 0.0f;
	REAL32			m_Duration = 0.0f;
	UINT32			m_Style = 0;
	i3Node *		m_pNode = nullptr;
	INT32			m_MaxLoopCount = 0;

	// Volatile Members
	UINT32			m_State = 0;
	INT32			m_loopCount = 0;

public:
	virtual ~i3TimeSequenceInfo(void);

	UINT32			GetState(void)					{ return m_State; }
	void			SetState( UINT32 state)			{ m_State = state; }
	void			AddState( UINT32 state)			{ m_State |= state; }
	void			RemoveState( UINT32 state)		{ m_State &= ~state; }

	REAL32			GetStartTime(void)				{ return m_StartTime; }
	void			SetStartTime( REAL32 tm)		{ m_StartTime = tm; }

	REAL32			GetDuration(void)				{ return m_Duration; }
	void			SetDuration( REAL32 tm)		{ m_Duration = tm; }

	UINT32			GetStyle(void)					{ return m_Style; }
	void			SetStyle( UINT32 style)			{ m_Style = style; }

	i3Node *		GetNode(void)					{ return m_pNode; }
	void			SetNode( i3Node * pNode);

	INT32			getMaxLoopCount(void)			{ return m_MaxLoopCount; }
	void			SetMaxLoopCount( INT32 cnt);		


	INT32			getLoopCount(void)				{ return m_loopCount; }
	void			setLoopCount( INT32 cnt)		{ m_loopCount = cnt; }
	void			addLoopCount(void)				{ m_loopCount++; }

	void			Start(void);
	void			Stop(void);

	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;

	bool			IsSame( i3TimeSequenceInfo * pDest);
};

#endif
