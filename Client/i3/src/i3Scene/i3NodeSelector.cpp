#include "i3SceneDef.h"
#include "i3NodeSelector.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3NodeSelector);

I3_TRACE_RESULT I3SG_NODE_PROC( i3NodeSelector)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3NodeSelector * pSelector = (i3NodeSelector *) pNode;

	{
		INT32 _i = 0;
		UINT64		mask = 1;

		while( _i < (pNode)->GetChildCount())
		{
			if( pSelector->IsMask( mask))
			{
				i3Node * pChild = (pNode)->GetChild( _i);

				Result = pChild->Trace( pTracer);
				if( Result == I3_TRACE_RESULT_STOP)
					return I3_TRACE_RESULT_STOP;
			}

			mask = mask << 1;
			_i++;
		}
	}

	return Result;
}

i3NodeSelector::i3NodeSelector(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3NodeSelector));
}

UINT32 i3NodeSelector::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Write( &m_Mask, sizeof(m_Mask));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NodeSelector::OnSave()", "Could not write mask data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3NodeSelector::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3NodeSelector::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &m_Mask, sizeof(m_Mask));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NodeSelector::OnLoad()", "Could not read mask data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

void i3NodeSelector::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	((i3NodeSelector *)pDest)->SetMask( GetMask());
}
