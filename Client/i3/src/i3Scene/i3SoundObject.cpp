#include "i3SceneDef.h"
#include "i3SoundObject.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3SoundObject);

I3_TRACE_RESULT I3SG_NODE_PROC( i3SoundObject)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3SoundObject * pSoundObject = (i3SoundObject *) pNode;

	pSoundObject->Drive( pTracer);

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}
	
	return Result;
}

i3SoundObject::i3SoundObject( void)
{
	_setTraceProc( I3SG_NODE_PROC( i3SoundObject));
}

i3SoundObject::~i3SoundObject(void)
{
	I3_SAFE_RELEASE( m_pSoundNode);
}

void i3SoundObject::SetSoundNode( i3SoundNode * pNode)
{
	if( m_pSoundNode != nullptr)
	{
		RemoveChild( m_pSoundNode);
	}

	I3_REF_CHANGE( m_pSoundNode, pNode);

	if( m_pSoundNode != nullptr)
	{
		AddChild( m_pSoundNode);
	}
}

void i3SoundObject::SetResponseRange( REAL32 rRange)
{
	m_rResponseRange = rRange;

	if( m_pSoundNode != nullptr)
	{
		i3SoundPlayInfo * pInfo = m_pSoundNode->getSoundPlayInfo();
		if( pInfo != nullptr)
		{
			pInfo->SetDistanceInfo( rRange, pInfo->getMinDistance(), pInfo->getVolumeType());
		}
	}
}

void i3SoundObject::Drive( i3SceneTracer * pTracer)
{
	if( m_pSoundNode == nullptr)
		return;

	i3SoundContext	* pSndCtx	= pTracer->getSoundContext();

	if( pSndCtx == nullptr)
		return;

	i3MatrixStack	* pStack	= pTracer->GetModelViewMatrixStack();

	VEC3D			vDiff;
	i3Vector::Sub( &vDiff, i3Matrix::GetPos( pSndCtx->GetListnerMatrix()), i3Matrix::GetPos( pStack->GetTop()));

	if( i3Vector::LengthSq( &vDiff) < getResponseRange() * getResponseRange())
	{
		if( m_pSoundNode->IsFlag( I3_NODEFLAG_DISABLE))
		{
			m_pSoundNode->ActivateNode();
		}
	}
	else
	{
		if( !m_pSoundNode->IsFlag( I3_NODEFLAG_DISABLE))
		{
			m_pSoundNode->DeactivateNode();
		}
	}
}

void	i3SoundObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3SoundObject * pSndObj = (i3SoundObject *)pDest;

	if( method == I3_COPY_REF)
	{
		pSndObj->SetSoundNode( m_pSoundNode);
	}
	else
	{
		if( m_pSoundNode != nullptr)
		{
			i3SoundNode * pNode = i3SoundNode::new_object_ref();

			m_pSoundNode->CopyTo( pNode, method);

			pSndObj->SetSoundNode( pNode);
		}
	}

	pSndObj->SetResponseRange( m_rResponseRange);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 1)
#endif

namespace pack
{
	struct SOUND_OBJECT
	{
		UINT8	nID[4] = { 'I', 'S', 'O', '1' };
		UINT32	nVersion = 1;

		OBJREF64	pSoundNode = 0;
		REAL32		rResponseRange = 0.0f;
		UINT32		pad[6] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

void i3SoundObject::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pSoundNode != nullptr)
	{
		m_pSoundNode->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

UINT32 i3SoundObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::SOUND_OBJECT data;

	Result = i3Node::OnSave( pResFile);
	I3_CHKIO( Result);

	data.pSoundNode		= (OBJREF64) pResFile->GetObjectPersistID( m_pSoundNode);
	data.rResponseRange	= getResponseRange();

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3SoundObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::SOUND_OBJECT data;

	Result = i3Node::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

//	if( i3String::NCompare( (char*)data.nID, "ISO1", 4) != nullptr)
	if ( i3::generic_is_niequal((char*)data.nID, "ISO1", 4) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Invalid SoundObject Sign or Error Occurred");
		return STREAM_ERR;
	}

	if( data.nVersion == 1)
	{
		if( data.pSoundNode > 0)
		{
			i3SoundNode * pNode = (i3SoundNode*)pResFile->FindObjectByID( (OBJREF)data.pSoundNode);

			I3ASSERT( pNode != nullptr);

			SetSoundNode( pNode);
		}

		SetResponseRange( data.rResponseRange);
	}

	return Result;
}
