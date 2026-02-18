#include "stdafx.h"
#include "TimeEvent.h"

I3_CLASS_INSTANCE( CTimeEvent, i3PersistantElement);

CTimeEvent::CTimeEvent(void)
{
	memset( &m_Event, 0, sizeof(m_Event));
}

CTimeEvent::~CTimeEvent(void)
{
	I3_SAFE_RELEASE( m_Event.m_pAction);
}

void CTimeEvent::OnBuildObjectList( i3List * pList)
{
	if( m_Event.m_pAction != NULL)
	{
		m_Event.m_pAction->OnBuildObjectList( pList);
	}

	i3PersistantElement::OnBuildObjectList( pList);
}

UINT32 CTimeEvent::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc;

	rc = pResFile->Write( &m_Event, sizeof(m_Event));
	I3_CHKIO( rc);

	return rc;
}

UINT32 CTimeEvent::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc;

	rc = pResFile->Read( &m_Event, sizeof(m_Event));
	I3_CHKIO( rc);

	if( m_Event.m_pAction != NULL)
	{
		i3Action * pAction = (i3Action *) pResFile->FindObjectByID( (OBJREF) m_Event.m_pAction);
		I3ASSERT( pAction != NULL);

		m_Event.m_pAction = pAction;
		pAction->AddRef();
	}

	return rc;
}
