#include "i3StateType.h"
#include "i3StateGraph.h"
#include "i3StateInit.h"

I3_CLASS_INSTANCE( i3StateGraph, i3NamedElement);

i3StateGraph::i3StateGraph(void)
{
	m_pStateInit = NULL;
	m_pCurState = NULL;
	m_pOldState = NULL;
}

i3StateGraph::~i3StateGraph(void)
{
	if( m_pStateInit != NULL)
	{
		m_pStateInit->Release();
		m_pStateInit = NULL;
	}
}

void i3StateGraph::SetStartState( i3State * pState)
{
	I3ASSERT( pState->IsExactTypeOf( i3StateInit::GetClassMeta()));

	if( m_pStateInit != NULL)
	{
		m_pStateInit->Release();
	}

	m_pStateInit = pState;

	m_pCurState = pState;

	if( pState != NULL)
	{
		pState->AddRef();
	}
}

void i3StateGraph::SetCurrentState( i3State * pState)
{
	m_pOldState = m_pCurState;

	m_pCurState = pState;
}

void i3StateGraph::OnBuildObjectList( i3List * pList)
{
	if( m_pStateInit != NULL)
	{
		m_pStateInit->OnBuildObjectList( pList);
	}

	i3NamedElement::OnBuildObjectList( pList);
}

UINT32 i3StateGraph::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;

	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// State Init
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( m_pStateInit);

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3StateGraph::OnSave()", "Could not write initial state object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3StateGraph::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;
	i3State * pState;

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// State Init
	{
		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3StateGraph::OnLoad()", "Could not read initial state object reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( ref != 0)
		{
			pState = (i3State *) pResFile->FindObjectByID( ref);
		}
		else
		{
			pState = NULL;
		}

		SetStartState( pState);
	}

	return Result;
}