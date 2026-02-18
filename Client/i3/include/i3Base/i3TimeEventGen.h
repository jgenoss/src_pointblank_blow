#if !defined( __I3_TIME_EVENT_GEN_H)
#define __I3_TIME_EVENT_GEN_H

#include "i3NamedElement.h"
#include "i3MemoryBuffer.h"
#include "i3Action.h"
#include "i3EventReceiver.h"
#include "i3TimeEventInfo.h"
#include "itl/vector_set.h"
#include "itl/vector_util.h"
#include "itl/algorithm/find.h"

#define I3_TIMEEVENT_REPEAT			0x00000001

bool operator<( const i3TimeEventInfo& a1, const i3TimeEventInfo& a2);
template<> struct i3::less<i3TimeEventInfo*> : i3::less_ptr<i3TimeEventInfo*> {};

class I3_EXPORT_BASE i3TimeEventGen : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3TimeEventGen, i3NamedElement);
protected:
	UINT32				m_Style = 0;
	i3::vector_multiset<i3TimeEventInfo*>	m_EventList;
	REAL32				m_timeLength = 0.0f;

	// Volatile
	REAL32				m_timeLocal = 0.0f;
	INT32				m_CurEvent = 0;

	bool				m_bInfinite = false;
	i3EventReceiver *	m_pReceiver = nullptr;
	
public:
	virtual ~i3TimeEventGen(void);

	UINT32				getStyle(void)					{ return m_Style; }
	void				setStyle( UINT32 style)			{ m_Style = style; }
	bool				isStyle( UINT32 mask)			{ return (m_Style & mask) != 0; }

	UINT32				getEventCount(void)				{ return (UINT32)m_EventList.size(); }
	i3TimeEventInfo *	getEvent( INT32 idx)			{ return m_EventList.get_vector()[ idx]; }
	INT32				AddEvent( i3TimeEventInfo * pInfo);
	INT32				GetEventIndex( i3TimeEventInfo * pInfo)	
	{
		i3::pair<i3::vector_multiset<i3TimeEventInfo*>::iterator, i3::vector_multiset<i3TimeEventInfo*>::iterator> pr =	m_EventList.equal_range(pInfo);
		i3::vector_multiset<i3TimeEventInfo*>::iterator it = i3::find(pr.first, pr.second, pInfo);
		if (it == pr.second) return -1;
		return (INT32)i3::distance(m_EventList.begin(), it);
	}
	void				SortEvents(void);
	void				RemoveEvent( i3TimeEventInfo * pInfo);
	void				RemoveEvent( INT32 idx);
	void				RemoveAllEvents(void);

	REAL32				getDuration(void)					{ return m_timeLength; }
	void				setDuration( REAL32 len)			{ m_timeLength = len; }

	REAL32				getLocalTime(void)					{ return m_timeLocal; }
	void				setLocalTime( REAL32 tm)			{ m_timeLocal = tm; }

	INT32				getCurrentEventIndex(void)			{ return m_CurEvent; }
	void				setCurrentEventIndex( INT32 idx)	{ m_CurEvent = idx; }

	i3TimeEventInfo *	getCurrentEvent(void)				
	{ 
		if( m_CurEvent == -1)
			return nullptr;

		return getEvent( getCurrentEventIndex());
	}

	i3EventReceiver *	getEventReceiver(void)			{ return m_pReceiver; }
	void				setEventReceiver( i3EventReceiver * pObj)
	{
		m_pReceiver = pObj;
	}

	void			Reset(void);
	void			Stop(void);

	void	OnUpdate( REAL32 tm);
	void	OnChangeTime( REAL32 tm);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

protected:
	void			_UpdateDuration(void);
};

#endif
