#include "i3SceneDef.h"
#include "i3LightSet.h"
#include "i3SceneTracer.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3LightSet);

I3_TRACE_RESULT I3SG_NODE_PROC( i3LightSet)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3LightSet * pLightSet = (i3LightSet *) pNode;
	INT32 i;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

#if NODE_PROFILE
	START_PROFILE( 27);
#endif

	for( i = 0; i < pLightSet->GetLightAttrCount(); i++)
	{
		i3LightAttr * pAttr = pLightSet->GetLightAttr( i);

		pAttr->GetLightObject()->SetWorldSpaceTransform( pStack->GetTop());

		pTracer->AddAttributeToOutput( pAttr);
	}

#if NODE_PROFILE
	END_PROFILE( 27);
#endif

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3LightSet::i3LightSet(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3LightSet));
}

i3LightSet::~i3LightSet(void)
{
	RemoveAllLightAttrs();
}

void i3LightSet::AddLightAttr( i3LightAttr * pAttr)
{
	m_LightList.push_back( pAttr);
	I3_SAFE_ADDREF( pAttr);
}

void i3LightSet::RemoveLightAttr( i3LightAttr * pAttr)
{
	if( i3::vu::remove(m_LightList, pAttr) )
	{
		I3_MUST_RELEASE( pAttr);
	}
}

void i3LightSet::RemoveAllLightAttrs(void)
{
	i3::cu::for_each_container(m_LightList, i3::fu::mem_fun_safe(&i3LightAttr::Release) );
	//	m_LightList.SafeReleaseAll();
	m_LightList.clear();
}

void i3LightSet::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3LightAttr * pAttr;

	for( i = 0; i < GetLightAttrCount(); i++)
	{
		pAttr = GetLightAttr( i);
		
		pAttr->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

UINT32 i3LightSet::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	INT32 i;
	UINT16	AttrCount;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// write attributes count
	AttrCount = (UINT16) GetLightAttrCount();
	Rc = pStream->Write( &AttrCount, sizeof(UINT16));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LightSet::OnSave()", "Could not write attributes count.");
		return STREAM_ERR;
	}
	Result += Rc;

	// write instance pointer as ID.
	for( i = 0; i < GetLightAttrCount(); i++)
	{
		OBJREF ref = (UINT32) pResFile->GetObjectPersistID( GetLightAttr( i));			// Pointer value is treated as Instance ID.

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LightSet::OnSave()", "Could not write Instance ID.");
			return STREAM_ERR;
		}

		Result += Rc;
	}

	return Result;
}

UINT32 i3LightSet::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3LightSet::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	INT32 i;
	UINT16 AttrCount;
	OBJREF AttrID;
	i3Stream * pStream = pResFile->GetStream();
	i3LightAttr * pAttr;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// read attributes count
	Rc = pStream->Read( &AttrCount, sizeof(UINT16));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LightSet::OnLoad()", "Could not read attributes count.");
		return STREAM_ERR;
	}
	Result += Rc;

	// read attribute ID.
	for( i = 0; i < AttrCount; i++)
	{
		Rc = pStream->Read( &AttrID, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LightSet::OnLoad()", "Could not read %dth attribute ID.", i);
			return STREAM_ERR;
		}
		Result += Rc;

		pAttr = (i3LightAttr *) pResFile->FindObjectByID( AttrID);
		I3ASSERT( pAttr != nullptr);

		AddLightAttr( pAttr);
	}

	return Result;
}

void i3LightSet::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3LightSet * pLightSet = (i3LightSet *) pDest;
	INT32 i;

	if( method == I3_COPY_REF)
	{
		for( i = 0; i < GetLightAttrCount(); i++)
		{
			pLightSet->AddLightAttr( GetLightAttr(i));
		}
	}
	else
	{
//		i3ClassMeta * pMeta;
		i3LightAttr * pNewAttr, * pAttr;

		for( i = 0; i < GetLightAttrCount(); i++)
		{
			pAttr = GetLightAttr(i);
//			pMeta = pAttr->GetMeta();
			pNewAttr = pAttr->create_instance(); //(i3LightAttr *) CREATEINSTANCE( pMeta);

			pAttr->CopyTo( pNewAttr, method);
		}
	}
}
