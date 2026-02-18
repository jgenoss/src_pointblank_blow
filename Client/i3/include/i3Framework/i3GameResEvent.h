#if !defined( _I3_GAMERES_EVENT_H__)
#define _I3_GAMERES_EVENT_H__

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResEvent : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResEvent, i3GameRes);

protected:
	i3TimeEventGen	*	m_pTimeEvent = nullptr;

public:
	i3GameResEvent(void);
	virtual ~i3GameResEvent(void);

public:
	void	setTimeEvent( i3TimeEventGen *pEvent)	{	I3_REF_CHANGE( m_pTimeEvent, pEvent);}
	i3TimeEventGen *	getTimeEvent( void)			{	return m_pTimeEvent;}

	virtual void	Destroy( void) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

};
#endif // #define _I3_GAMERES_EVENT_H__