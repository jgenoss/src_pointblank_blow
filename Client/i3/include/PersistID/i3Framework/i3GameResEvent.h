#if !defined( _I3_GAMERES_EVENT_H__)
#define _I3_GAMERES_EVENT_H__

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResEvent : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResEvent);

protected:
	i3TimeEventGen	*	m_pTimeEvent;

public:
	i3GameResEvent(void);
	virtual ~i3GameResEvent(void);

public:
	void	setTimeEvent( i3TimeEventGen *pEvent)	{	I3_REF_CHANGE( m_pTimeEvent, pEvent);}
	i3TimeEventGen *	getTimeEvent( void)			{	return m_pTimeEvent;}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};
#endif // #define _I3_GAMERES_EVENT_H__