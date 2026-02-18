#include "i3SceneDef.h"
#include "i3AttrSet.h"
#include "i3AttrStack.h"
#include "i3SceneTracer.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3AttrSet);

I3_TRACE_RESULT I3SG_NODE_PROC( i3AttrSet)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();

	// Check bound box
	bool bSet = ((pAttrSet->IsFlag( I3_NODEFLAG_NO_WORK) == false) && (pAttrSet->GetAttrCount() > 0));

	if( bSet)
	{
		pStackManager->PushAttrs( pAttrSet->GetAttrs(), pAttrSet->GetPriority());
		Result = i3Node::CallTraceProc( pNode, pTracer);
		pStackManager->PopAttrs( pAttrSet->GetAttrs(), pAttrSet->GetPriority());
	}
	else
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}
	
	return Result;
}

void	i3AttrSet::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled)
{
	const INT32 numAttr = this->GetAttrCount();

	for (INT32 i = 0; i < numAttr; ++i)
	{
		i3FaceCullModeAttr* pAttr = i3::same_cast<i3FaceCullModeAttr*>(this->GetAttr(i));
		if (pAttr)
		{
			twoSideEnabled = (pAttr->Get() == I3G_FACE_CULL_NONE);
			break;
		}
	}

	i3Node::CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);
}



i3AttrSet::i3AttrSet(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3AttrSet));
	m_AttrList.reserve( 16);
}

i3AttrSet::~i3AttrSet(void)
{
	RemoveAllAttrs();
}

void i3AttrSet::AddAttr( i3RenderAttr * pAttr)
{
	m_AttrList.push_back( pAttr);
	I3_MUST_ADDREF( pAttr);

#if defined( I3_DEBUG)
	pAttr->setOwner( this);
#endif
}

void i3AttrSet::RemoveAttr( i3RenderAttr * pAttr)
{
//	if( m_AttrList.Remove( pAttr) != -1)
	if ( i3::vu::remove(m_AttrList, pAttr) )
	{
		I3_MUST_RELEASE( pAttr);
	}
}

void i3AttrSet::RemoveAllAttrs(void)
{
	//m_AttrList.SafeReleaseAll();
	//m_AttrList.Clear();
	for_each(m_AttrList.begin(), m_AttrList.end(), i3::fu::mem_fun_safe(&i3RenderAttr::Release));		// 중간에 널값이 허용되지 않아야한다..(루틴상으로는 널이 없어보임)
	m_AttrList.clear();

}

i3RenderAttr *	i3AttrSet::FindAttr( i3ClassMeta * pMeta)
{
	register i3RenderAttr * pAttr;

	for(size_t i = 0; i < m_AttrList.size(); i++)
	{
		pAttr = m_AttrList[i];

		if( pAttr->same_of( pMeta))
			return pAttr;
	}

	return nullptr;
}

void i3AttrSet::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3RenderAttr * pAttr;

	for( i = 0; i < GetAttrCount(); i++)
	{
		pAttr = GetAttr( i);
		
		pAttr->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ATTRSET_HEADER
	{
		UINT8	m_ID[4] = { 'A', 'T', 'S', '1' };
		UINT32	m_AttrCount = 0;
		UINT32	m_Priority = 0;
		UINT32	pad = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3AttrSet::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	INT32 i;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	/// old version
#if 0
	{
		// write attributes count
		UINT16	AttrCount;
		AttrCount = (UINT16) GetAttrCount();
		Rc = pStream->Write( &AttrCount, sizeof(UINT16));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3AttrSet::OnSave()", "Could not write attributes count.");
			return STREAM_ERR;
		}
		Result += Rc;
	}
#else
	{	// new version include header info
		pack::ATTRSET_HEADER	head;
		
		head.m_AttrCount = (UINT32) GetAttrCount();
		head.m_Priority = GetPriority();

		Rc = pStream->Write( &head, sizeof( head) );
		I3_CHKIO( Rc);
		Result += Rc;
	}
#endif

	// write instance pointer as ID.
	for( i = 0; i < GetAttrCount(); i++)
	{
		Rc = (UINT32) pResFile->GetObjectPersistID( GetAttr( i));			// Pointer value is treated as Instance ID.
		Rc = pStream->Write( &Rc, sizeof(UINT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3AttrSet::OnSave()", "Could not write Instance ID.");
			return STREAM_ERR;
		}

		Result += Rc;
	}

	return Result;
}

UINT32 i3AttrSet::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3AttrSet::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	INT32 i;
	UINT16 AttrCount;
	OBJREF AttrID;
	i3Stream * pStream = pResFile->GetStream();
	i3RenderAttr * pAttr;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::ATTRSET_HEADER	head;

	Rc = pStream->Read( &head, sizeof( head) );
	I3_CHKIO( Rc);
	
//	if( i3String::NCompare( (const char*) head.m_ID, "ATS1", 4) == 0)
	if (i3::generic_is_niequal((const char*) head.m_ID, "ATS1", 4) )
	{
		// new version include header info

		SetPriority( head.m_Priority);
		
		AttrCount = (UINT16) head.m_AttrCount;

	}
	else
	{
		INT32 pos = sizeof( head);
		pStream->SetPosition( -pos , STREAM_CURRENT);

		// old version
		// read attributes count
		Rc = pStream->Read( &AttrCount, sizeof(UINT16));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3AttrSet::OnLoad()", "Could not read attributes count.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// read attribute ID.
	if( AttrCount > 0)
	{
		m_AttrList.resize( AttrCount);

		for( i = 0; i < AttrCount; i++)
		{
			Rc = pStream->Read( &AttrID, sizeof(OBJREF));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3AttrSet::OnLoad()", "Could not read %dth attribute ID.", i);
				return STREAM_ERR;
			}
			Result += Rc;

			pAttr = (i3RenderAttr *) pResFile->FindObjectByID( AttrID);
			I3_MUST_ADDREF( pAttr);

			m_AttrList[i] = pAttr;
		}
	}

	return Result;
}

void i3AttrSet::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pDest;
		INT32 i;
		i3RenderAttr * pAttr, * pNewAttr;
//		i3ClassMeta * pMeta;

		pAttrSet->m_AttrList.resize( GetAttrCount());

		pAttrSet->SetPriority( GetPriority() );

		if( method == I3_COPY_REF)
		{
			for( i = 0; i < GetAttrCount(); i++)
			{
				pAttr = GetAttr( i);
				pAttrSet->m_AttrList[ i] = pAttr;
				I3_SAFE_ADDREF( pAttr);
			}
		}
		else
		{
			for( i = 0; i < GetAttrCount(); i++)
			{
				pAttr = GetAttr(i);
	//			pMeta = pAttr->GetMeta();

				pNewAttr = pAttr->create_instance(); //(i3RenderAttr *) CREATEINSTANCE( pMeta);
				pAttr->CopyTo( pNewAttr, method);

				pAttrSet->m_AttrList[i] = pNewAttr;
				I3_SAFE_ADDREF( pNewAttr);
			}
		}
	}
}
