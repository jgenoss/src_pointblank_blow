#include "i3SceneDef.h"
#include "i3BinaryPartitionNode.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3BinaryPartitionNode);

I3_TRACE_RESULT I3SG_NODE_PROC( i3BinaryPartitionNode)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3BinaryPartitionNode * pBSP = ( i3BinaryPartitionNode *) pNode;
	
	I3_FACE_SIDE side = i3Plane::getSide( pBSP->getPlane(), i3Matrix::GetPos( pTracer->GetInverseViewMatrix()));

	// Front-To-Back Rendering
	if( side == I3_FACE_BACK)
	{
		if( pBSP->getBack() != nullptr)
			pBSP->getBack()->Trace( pTracer);

		if( pNode->GetChildCount() > 0)
			Result = i3Node::CallTraceProc( pNode, pTracer);

		if( pBSP->getFront() != nullptr)
			pBSP->getFront()->Trace( pTracer);
	}
	else
	{
		if( pBSP->getFront() != nullptr)
			pBSP->getFront()->Trace( pTracer);

		if( pNode->GetChildCount() > 0)
			Result = i3Node::CallTraceProc( pNode, pTracer);

		if( pBSP->getBack() != nullptr)
			pBSP->getBack()->Trace( pTracer);
	}

	return Result;
}

i3BinaryPartitionNode::i3BinaryPartitionNode(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3BinaryPartitionNode));
}

i3BinaryPartitionNode::~i3BinaryPartitionNode(void)
{
	I3_SAFE_RELEASE( m_pFront);
	I3_SAFE_RELEASE( m_pBack);
}

void i3BinaryPartitionNode::GetPlaceMatrix( MATRIX * pMtx)
{
	i3Matrix::MakeWithAt( pMtx, (VEC3D *) &m_Plane, &I3_YAXIS);
}

void	i3BinaryPartitionNode::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	if( getFront() != nullptr)
		getFront()->_GetWrappingBound( pInfo);

	if( getBack() != nullptr)
		getBack()->_GetWrappingBound( pInfo);

	i3Node::_GetWrappingBound( pInfo);
}

void	i3BinaryPartitionNode::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled)
{
	if (getFront() != nullptr)
		getFront()->CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);

	if (getBack() != nullptr)
		getBack()->CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);

	i3Node::CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);
}

void	i3BinaryPartitionNode::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( i3::same_of<i3BinaryPartitionNode*>(pDest));

	i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pDest;

	pBSP->setPlane( getPlane());

	i3Node::CopyTo( pDest, method);
}

void	i3BinaryPartitionNode::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pFront != nullptr)
		m_pFront->OnBuildObjectList( List);

	if( m_pBack != nullptr)
		m_pBack->OnBuildObjectList( List);

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 BINARY_PARTITION_NODE
	{
		UINT8				m_ID[4] = { 'B', 'P', 'N', '1' };
		i3::pack::VEC4D		m_Plane;
		OBJREF				m_pFront = 0;
		OBJREF				m_pBack = 0;
		UINT32				pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3BinaryPartitionNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::BINARY_PARTITION_NODE data;

	result = i3Node::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_Plane.x	=	m_Plane.x;
	data.m_Plane.y	=	m_Plane.y;
	data.m_Plane.z	=	m_Plane.z;
	data.m_Plane.w	=	m_Plane.w;

	data.m_pFront	=	pResFile->GetObjectPersistID( m_pFront);
	data.m_pBack	=	pResFile->GetObjectPersistID( m_pBack);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3BinaryPartitionNode::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::BINARY_PARTITION_NODE data;

	result = i3Node::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	m_Plane.x	=	data.m_Plane.x;
	m_Plane.y	=	data.m_Plane.y;
	m_Plane.z	=	data.m_Plane.z;
	m_Plane.w	=	data.m_Plane.w;

	if( data.m_pFront != 0)
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pResFile->FindObjectByID( data.m_pFront);
		I3ASSERT( pBSP != nullptr);

		setFront( pBSP);
	}

	if( data.m_pBack != 0)
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pResFile->FindObjectByID( data.m_pBack);
		I3ASSERT( pBSP != nullptr);

		setBack( pBSP);
	}

	return result;
}
