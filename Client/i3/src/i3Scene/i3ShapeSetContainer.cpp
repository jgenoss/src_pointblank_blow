#include "i3SceneDef.h"
#include "i3ShapeSetContainer.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3ShapeSetContainer);

I3_TRACE_RESULT I3SG_NODE_PROC( i3ShapeSetContainer)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3ShapeSetContainer * pShapeContainer = (i3ShapeSetContainer *) pNode;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3PhysixShapeSet * pShapeSet;

	pShapeSet = pShapeContainer->getShapeSet();

	if((pShapeSet != nullptr) && (pShapeSet->getNxActor() != nullptr))
	{
		if( pShapeSet->isDynamicActor())
		{
			pShapeSet->setShapeGlobalPose( pStack->GetTop());
		}
	}

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3ShapeSetContainer::i3ShapeSetContainer(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3ShapeSetContainer));
}

i3ShapeSetContainer::~i3ShapeSetContainer(void)
{
	I3_SAFE_RELEASE( m_pShapeSet);
}

void i3ShapeSetContainer::ModifyDynamicState( UINT32 addMask, UINT32 removeMask)
{
	if( m_Usage != USAGE_SIMULATION)
	{
		addMask |= I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_KINEMATIC;
		removeMask &= ~(I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_KINEMATIC);
	}

	if( m_pShapeSet != nullptr)
	{
		m_pShapeSet->modifyDynamicState( addMask, removeMask);
	}
}

void i3ShapeSetContainer::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pShapeSet != nullptr)
	{
		m_pShapeSet->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

void i3ShapeSetContainer::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3ShapeSetContainer * pShapeSet = (i3ShapeSetContainer *) pDest;

	if( getShapeSet() != nullptr)
	{
		i3PhysixShapeSet * pNewShape = i3PhysixShapeSet::new_object_ref();

		getShapeSet()->CopyTo( pNewShape, method);

		pShapeSet->setShapeSet( pNewShape);

		// revision 3390
		pShapeSet->setUsage( getUsage());
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 SHAPESET_CONTAINER
	{
		UINT8		m_ID[4] = { 'S', 'S', 'C', '1' };
		OBJREF64	m_pShapeSet = 0;
		UINT8		m_Usage = 0;
		UINT8		pad0[3] = { 0 };
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ShapeSetContainer::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::SHAPESET_CONTAINER data;

	result = i3Node::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pShapeSet		= (OBJREF64)	pResFile->GetObjectPersistID( getShapeSet());
	data.m_Usage			= (UINT8)		getUsage();

	rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ShapeSetContainer::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::SHAPESET_CONTAINER data;

	result = i3Node::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pShapeSet != 0)
	{
		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pResFile->FindObjectByID( (OBJREF) data.m_pShapeSet);
		I3ASSERT( pShapeSet != nullptr);

		setShapeSet( pShapeSet);
	}

	setUsage( (USAGE) data.m_Usage);

	return result;
}

void i3ShapeSetContainer::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	if( getShapeSet() != nullptr)
	{
		getShapeSet()->OnInstancing( pInfo);
	}

	i3Node::OnInstancing( pInfo);
}
