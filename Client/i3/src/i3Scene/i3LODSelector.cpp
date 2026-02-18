#include "i3SceneDef.h"
#include "i3LODSelector.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3LODSelector, i3Node);

I3_TRACE_RESULT I3SG_NODE_PROC( i3LODSelector)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3LODSelector * pSelector = (i3LODSelector *) pNode;

	{
		INT32 level = pSelector->SelectCurrentLevel( pTracer);

		i3Node * pChild = (pNode)->GetChild( level);

		Result = pChild->Trace(  pTracer);
		if( Result == I3_TRACE_RESULT_STOP)
			return I3_TRACE_RESULT_STOP;
	}

	return Result;
}

i3LODSelector::i3LODSelector(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3LODSelector));

	m_L = 0;
	m_K = 0;
}

i3LODSelector::~i3LODSelector(void)
{
}

INT32 i3LODSelector::SelectCurrentLevel( i3SceneTracer * /*pTracer*/)
{
	return 0;
}

UINT32 i3LODSelector::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// L Value
	Rc = pStream->Write( &m_L, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LODSelector::OnSave()", "Could not write L value.");
		return STREAM_ERR;
	}
	Result += Rc;

	// K Value
	Rc = pStream->Write( &m_K, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LODSelector::OnSave()", "Could not write K value.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3LODSelector::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3LODSelector::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Node::OnLoad( pResFile);
	if( Result != STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// L Value;
	Rc = pStream->Read( &m_L, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LODSelector::OnLoad()", "Could not read L value. ");
		return STREAM_ERR;
	}
	Result += Rc;

	// K Value
	Rc = pStream->Read( &m_K, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LODSelector::OnLoad()", "Could not read K value. ");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

void i3LODSelector::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3LODSelector * pLOD = (i3LODSelector *) pDest;

	pLOD->SetLK( GetL(), GetK());
}
