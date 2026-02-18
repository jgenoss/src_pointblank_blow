#include "i3StateType.h"
#include "i3Base.h"
#include "i3State.h"

I3_CLASS_INSTANCE( i3State, i3NamedElement);

i3State::i3State(void)
{
	m_Priority = 0;
}

i3State::~i3State(void)
{
	RemoveAllEvent();
}

void i3State::AddEvent( i3Event * pEvent)
{
	pEvent->AddRef();

	pEvent->SetFromState( this);

	m_EventList.Add( pEvent);
}

void i3State::RemoveEvent( i3Event * pEvent)
{
	pEvent->Release();

	pEvent->SetFromState( NULL);

	m_EventList.Remove( pEvent);
}

void i3State::RemoveAllEvent(void)
{
	UINT32 i;
	i3Event * pEvent;

	for( i = 0; i < GetEventCount(); i++)
	{
		pEvent = GetEvent( i );

		pEvent->SetFromState( NULL);
	}
}

void i3State::OnInit( i3StateMachine *)
{
}

void i3State::OnRun( i3StateMachine *)
{
}

void i3State::OnFinish( i3StateMachine *)
{
}

void i3State::OnBuildObjectList( i3List * pList)
{
	UINT32 i;
	i3Event * pEvent;

	for( i = 0; i < GetEventCount(); i++)
	{
		pEvent = GetEvent( i);

		pEvent->OnBuildObjectList( pList);
	}

	i3NamedElement::OnBuildObjectList( pList);
}

UINT32 i3State::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;
	i3Event * pEvent;
	UINT32 i;

	Result = i3NamedElement::OnSave( pResFile);
	if( Result == NULL)
	{
		return STREAM_ERR;
	}

	// Priority
	{
		Rc = pStream->Write( &m_Priority, sizeof( INT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3State::OnSave()", "Could not write priority information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}
	
	// Event Count
	{
		UINT32 count = GetEventCount();

		Rc = pStream->Write( &count, sizeof(count));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3State::OnSave()", "Could not write event count information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0 ; i < GetEventCount(); i++)
	{
		pEvent = GetEvent( i );

		ref = (OBJREF) pResFile->GetObjectPersistID( pEvent);

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3State::OnSave()", "Could not write event object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3State::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;
	i3Event * pEvent;
	UINT32 count;
	UINT32 i;

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == NULL)
	{
		return STREAM_ERR;
	}

	// Priority
	{
		Rc = pStream->Read( &m_Priority, sizeof(INT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3State::OnLoad()", "Could not read priority information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}
	
	// Event Count
	{
		Rc = pStream->Read( &count, sizeof(count));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3State::OnLoad()", "Could not read event count information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0 ; i < count; i++)
	{
		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3State::OnLoad()", "Could not read event object reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pEvent = (i3Event *) pResFile->FindObjectByID( ref);
		I3ASSERT( pEvent != NULL);

		AddEvent( pEvent);
	}

	return Result;
}