#if !defined( __I3_Level_TRIGGER_H)
#define __I3_Level_TRIGGER_H

#include "i3LevelPropertyContainer.h"
#include "i3LevelIcon.h"
#include "i3LevelTrigger.h"
#include "i3LevelAction.h"

////////////////////////////////////////////////////////////////////
// i3LevelTrigger State
#define				TRIG_STATE_DISABLE		0x00000001


class I3_EXPORT_TDK i3LevelTrigger : public i3LevelPropertyContainer
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelTrigger, i3LevelPropertyContainer);
protected:
	UINT32				m_InitState = 0;
	i3::vector<i3LevelAction*> m_ActionList;
	INT32				m_RepeatCount = 1;

	i3LevelIcon *			m_pIcon = nullptr;

public:
	virtual ~i3LevelTrigger(void);

	UINT32			GetInitState(void)				{ return m_InitState; }
	void			SetInitState( UINT32 state)		{ m_InitState = state; }
	void			AddInitState( UINT32 state)		{ m_InitState |= state; }
	void			RemoveInitState( UINT32 state)	{ m_InitState &= ~state; }

	INT32			GetRepeatCount(void)			{ return m_RepeatCount; }
	void			SetRepeatCount( INT32 c)		{ m_RepeatCount = c; }

	void			AddAction( i3LevelAction * pAction);
	void			RemoveAction( i3LevelAction * pAction);
	void			RemoveAllAction(void);
	INT32			FindActionByName( const char * pszName);
	INT32			FindAction( i3LevelAction * pAction);
	INT32			GetActionCount(void)				{ return (INT32)m_ActionList.size(); }
	i3LevelAction *	GetAction( INT32 idx)				{ return m_ActionList[idx]; }
	
	void			Freeze(void);
	void			Unfreeze(void);

	i3LevelIcon *		GetIcon(void)						{ return m_pIcon; }

	virtual char *	GetTypeName(void) = 0;
	virtual void	GetInformationString( char * pszStr);

	virtual void	OnAttachScene( i3LevelScene * pScene) override;
	virtual void	OnDetachScene( i3LevelScene * pScene) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
