#include "i3StateType.h"
#include "i3State.h"
#include "i3Event.h"

I3_CLASS_INSTANCE( i3Event, i3PersistantElement);

i3Event::i3Event(void)
{
	m_pFrom = NULL;
	m_pTo = NULL;
	m_bSignaled = FALSE;
	m_Priority = 0;
}

i3Event::~i3Event(void)
{
	SetFromState( NULL);
	SetToState( NULL);
}

void i3Event::SetFromState( i3State * pState)
{
	if( m_pFrom != NULL)
	{
		m_pFrom->Release();
		m_pFrom = NULL;
	}

	m_pFrom = pState;

	if( pState != NULL)
	{
		pState->AddRef();
	}
}

void i3Event::SetToState( i3State * pState)
{
	if( m_pTo != NULL)
	{
		m_pTo->Release();
		m_pTo = NULL;
	}

	m_pTo = pState;

	if( pState != NULL)
	{
		pState->AddRef();
	}
}

void i3Event::SetSignaled( BOOL bState)
{
	m_bSignaled = bState;
}

void i3Event::OnBuildObjectList( i3List * pList)
{
	if((m_pFrom != NULL) && (pList->IndexOf( m_pFrom) == -1))
	{
		m_pFrom->OnBuildObjectList( pList);
	}

	if((m_pTo != NULL) && (pList->IndexOf( m_pTo) == -1))
	{
		m_pTo->OnBuildObjectList( pList);
	}

	i3PersistantElement::OnBuildObjectList( pList);
}

UINT32 i3Event::OnSave( i3ResourceFile * pResFile)
{
	OBJREF ref;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	// Priority
	{
		Rc = pStream->Write( &m_Priority, sizeof(m_Priority));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Event::OnSave()", "Could not write priority information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// From State
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( m_pFrom);

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Event::OnSave()", "Could not write from-side state object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// To State
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( m_pTo);

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Event::OnSave()", "Could not write to-side state object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Event::OnLoad( i3ResourceFile * pResFile)
{
	OBJREF ref;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3State * pState;

	// Priority
	{
		Rc = pStream->Read( &m_Priority, sizeof(m_Priority));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Event::OnLoad()", "Could not read priority information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// From State
	{
		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Event::OnLoad()", "Could not read from-side state object reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pState = (i3State *) pResFile->FindObjectByID( ref);
		I3ASSERT( pState != NULL);

		SetFromState( pState);
	}

	// To State
	{
		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Event::OnLoad()", "Could not read to-side state object reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pState = (i3State *) pResFile->FindObjectByID( ref);
		I3ASSERT( pState != NULL);

		SetToState( pState);
	}

	return Result;
}