#if !defined( __I3_TIME_EVENT_GEN_H)
#define __I3_TIME_EVENT_GEN_H

#include "i3NamedElement.h"
#include "i3MemoryBuffer.h"
#include "i3Action.h"
#include "i3EventReceiver.h"
#include "i3TimeEventInfo.h"
#include "i3BinList.h"

#define I3_TIMEEVENT_REPEAT			0x00000001

class I3_EXPORT_BASE i3TimeEventGen : public i3NamedElement
{
	I3_CLASS_DEFINE( i3TimeEventGen);
protected:
	UINT32				m_Style;
	i3BinList			m_EventList;
	REAL32				m_timeLength;

	// Volatile
	REAL32				m_timeLocal;
	INT32				m_CurEvent;

	BOOL				m_bInfinite;
	i3EventReceiver *	m_pReceiver;
	
public:
	i3TimeEventGen(void);
	virtual ~i3TimeEventGen(void);

	UINT32				getStyle(void)					{ return m_Style; }
	void				setStyle( UINT32 style)			{ m_Style = style; }
	bool				isStyle( UINT32 mask)			{ return (m_Style & mask) != 0; }

	UINT32				getEventCount(void)				{ return m_EventList.GetCount(); }
	i3TimeEventInfo *	getEvent( INT32 idx)			{ return (i3TimeEventInfo *) m_EventList.Items[ idx]; }
	INT32				AddEvent( i3TimeEventInfo * pInfo);
	INT32				GetEventIndex( i3TimeEventInfo * pInfo)		{ return m_EventList.IndexOf( pInfo); }
	void				RemoveEvent( i3TimeEventInfo * pInfo);
	void				RemoveEvent( INT32 idx);
	void				RemoveAllEvents(void);
	void				SortEvents(void);

	REAL32				getDuration(void)					{ return m_timeLength; }
	void				setDuration( REAL32 len)			{ m_timeLength = len; }

	REAL32				getLocalTime(void)					{ return m_timeLocal; }
	void				setLocalTime( REAL32 tm)			{ m_timeLocal = tm; }

	INT32				getCurrentEventIndex(void)			{ return m_CurEvent; }
	void				setCurrentEventIndex( INT32 idx)	{ m_CurEvent = idx; }

	i3TimeEventInfo *	getCurrentEvent(void)				
	{ 
		if( m_CurEvent == -1)
			return NULL;

		return getEvent( getCurrentEventIndex());
	}

	i3EventReceiver *	getEventReceiver(void)			{ return m_pReceiver; }
	void				setEventReceiver( i3EventReceiver * pObj)
	{
		m_pReceiver = pObj;
	}

	void			Reset(void);
	void			Stop(void);

	virtual void	OnUpdate( REAL32 tm);
	virtual void	OnChangeTime( REAL32 tm);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

protected:
	void			_UpdateDuration(void);
};

#endif
