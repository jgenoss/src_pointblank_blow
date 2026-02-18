#include "i3SceneDef.h"
#include "i3OcTreeNode.h"

#include "i3AllAttrs.h"
#include "i3AllNodes.h"

I3_CLASS_INSTANCE( i3OcTreeNode);

I3_TRACE_RESULT I3SG_NODE_PROC( i3OcTreeNode)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3OcTreeNode * pTreeNode = (i3OcTreeNode *) pNode;

	if( pNode == nullptr)
	{
		return Result;
	}

	INT32 i, j;
	REAL32 dist[8];
	INT32  idx[8];
	VEC3D center, diff, vmin, vmax, * pCamPos;

	pCamPos = i3Matrix::GetPos( pTracer->GetInverseViewMatrix());

	// 8개 Child Node들에 대해 Center 점과 Camera의 거리를 구해 가까운 쪽부터 Traversal.
	for( i = 0; i < 8; i++)
	{
		pTreeNode->getSubBound( i, &vmin, &vmax);

		idx[i] = i;

		if( ContainAABBoxPoint( pCamPos, &vmin, &vmax))
		{
			dist[i] = 0.0f;
		}
		else
		{
			i3Vector::Add( &center, &vmin, &vmax);
			i3Vector::Scale( &center, &center, 0.5f);

			i3Vector::Sub( &diff, pCamPos, &center);
			
			dist[i] = i3Vector::LengthSq( &diff);
		}
	}

	// sorting
	for( i = 0; i < 8 - 1; i++)
	{
		for( j = i + 1; j < 8; j++)
		{
			if( dist[i] > dist[j])
			{
				INT32 idxTemp = idx[j];
				REAL32 distTemp = dist[j];

				idx[j] = idx[i];
				dist[j] = dist[i];

				idx[i] = idxTemp;
				dist[i] = distTemp;
			}
		}
	}

	for( i = 0; i < 8; i++)
	{
		INT32 div = idx[i];

		i3OcTreeNode * pChild = pTreeNode->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) div);

		if( pChild == nullptr)
			continue;

		if( (pChild->GetBound() == nullptr) || pTracer->IsInsideFrustum( pChild->GetBound()))
		{
			pChild->Trace( pTracer);
		}
	}

	if( pTreeNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3OcTreeNode::i3OcTreeNode(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3OcTreeNode));
}

i3OcTreeNode::~i3OcTreeNode(void)
{
	INT32 i;

	for( i = 0; i < 8; ++i)
	{
		I3_SAFE_RELEASE( m_pChildNode[i]);
	}
}

void i3OcTreeNode::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	INT32 i;
	i3OcTreeNode * pChild;

	for( i = 0; i < 8; i++)
	{
		pChild = getChildOctree( (DIVIDE_NODE) i);

		if( pChild != nullptr)
			pChild->_GetWrappingBound( pInfo);
	}

	i3Node::_GetWrappingBound( pInfo);
}

void i3OcTreeNode::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled)
{
	i3OcTreeNode * pChild;

	for (INT32 i = 0; i < 8; i++)
	{
		pChild = getChildOctree((DIVIDE_NODE)i);

		if (pChild != nullptr)
			pChild->CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);
	}

	i3Node::CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);
}


void i3OcTreeNode::getSubBound( INT32 idx, VEC3D * pMin, VEC3D * pMax)
{
	static PBSRC	s_idxSrcMin[8][3] =
	{
		{	PBS_MIN, PBS_MIN, PBS_MIN },	//	BOTTOM_LEFT_FRONT = 0,		// 0
		{	PBS_MIN, PBS_MIN, PBS_CTR },	//  BOTTOM_LEFT_BACK,			// 1
		{	PBS_CTR, PBS_MIN, PBS_MIN },	//  BOTTOM_RIGHT_FRONT,
		{	PBS_CTR, PBS_MIN, PBS_CTR },	//  BOTTOM_RIGHT_BACK,			// etc...
		{	PBS_MIN, PBS_CTR, PBS_MIN },	//	TOP_LEFT_FRONT = 0,		// 0
		{	PBS_MIN, PBS_CTR, PBS_CTR },	//  TOP_LEFT_BACK,			// 1
		{	PBS_CTR, PBS_CTR, PBS_MIN },	//  TOP_RIGHT_FRONT,
		{	PBS_CTR, PBS_CTR, PBS_CTR },	//  TOP_RIGHT_BACK,			// etc...
	};

	static PBSRC	s_idxSrcMax[8][3] =
	{
		{	PBS_CTR, PBS_CTR, PBS_CTR },	//	BOTTOM_LEFT_FRONT = 0,		// 0
		{	PBS_CTR, PBS_CTR, PBS_MAX },	//  BOTTOM_LEFT_BACK,			// 1
		{	PBS_MAX, PBS_CTR, PBS_CTR },	//  BOTTOM_RIGHT_FRONT,
		{	PBS_MAX, PBS_CTR, PBS_MAX },	//  BOTTOM_RIGHT_BACK,			// etc...
		{	PBS_CTR, PBS_MAX, PBS_CTR },	//	TOP_LEFT_FRONT = 0,		// 0
		{	PBS_CTR, PBS_MAX, PBS_MAX },	//  TOP_LEFT_BACK,			// 1
		{	PBS_MAX, PBS_MAX, PBS_CTR },	//  TOP_RIGHT_FRONT,
		{	PBS_MAX, PBS_MAX, PBS_MAX },	//  TOP_RIGHT_BACK,			// etc...
	};

	INT32 i;
	VEC3D * pSrc;

	I3_BOUNDCHK( idx, 8);

	for( i = 0; i < 3; i++)
	{
		// Min
        switch( s_idxSrcMin[idx][i])
		{
			case PBS_MIN :		pSrc = getMin();	break;
			case PBS_MAX :		pSrc = getMax();	break;
			default :			pSrc = getCenter();
		}

		pMin->vec[i] = pSrc->vec[i];

		// Max
		switch( s_idxSrcMax[idx][i])
		{
			case PBS_MIN :		pSrc = getMin();	break;
			case PBS_MAX :		pSrc = getMax();	break;
			default :			pSrc = getCenter();
		}

		pMax->vec[i] = pSrc->vec[i];
	}
}

inline UINT32		getXLocale( UINT32 code)
{
	return (code >> 20) & 0x03FF;
}

inline UINT32		getYLocale( UINT32 code)
{
	return (code >> 10) & 0x03FF;
}

inline UINT32		getZLocale( UINT32 code)
{
	return code & 0x03FF;
}

UINT32 i3OcTreeNode::calcLocaleCode( i3BoundBox * pBox)
{
	//
	// Ref : "Simple and Efficient Traversal Methods for Quadtrees and Octrees", Sarah F. Frisken, Ronald N. Perry
	//
	VEC3D size;

	pBox->getSize( &size);

	UINT32 x, y, z;
	REAL32 rd;

	rd = (REAL32) powf( 2.0f, (REAL32) getMaxDepth() - 1);

	x = (UINT32)(((getMin()->x - pBox->GetMin()->x + I3_EPS) / size.x) * rd);
	y = (UINT32)(((getMin()->y - pBox->GetMin()->y + I3_EPS) / size.y) * rd);
	z = (UINT32)(((getMin()->z - pBox->GetMin()->z + I3_EPS) / size.z) * rd);

	x = x & 0x000003FF;
	y = y & 0x000003FF;
	z = z & 0x000003FF;

	return (x << 20) | (y << 10) | (z);
}

void i3OcTreeNode::UpdateLocaleCode( i3BoundBox * pBox, bool bRecursive)
{
	UINT32 code;

	code = calcLocaleCode( pBox);

	setLocaleCode( code);

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < 8; i++)
		{
			i3OcTreeNode * pChild = getChildOctree( (i3OcTreeNode::DIVIDE_NODE)i );

			if( pChild != nullptr)
				pChild->UpdateLocaleCode( pBox, true);
		}
	}
}

i3OcTreeNode *	i3OcTreeNode::GetNodeByLocaleCode( UINT32 code)
{
	UINT32 x, y, z, mask, i, idx, shift;
	i3OcTreeNode * pTemp = this;

	x = getXLocale( code);
	y = getYLocale( code);
	z = getZLocale( code);

	mask = (1 << getMaxDepth());

	for( i = 0; (i < getMaxDepth()) && (pTemp->isLeafNode()); i++)
	{
		shift = getMaxDepth() - i;

		idx =	(((y & mask) >> shift) << 2) |
				(((x & mask) >> shift) << 1) |
				(((z & mask) >> shift) << 0);

		pTemp = pTemp->getChildOctree( (DIVIDE_NODE) idx);

		if( pTemp == nullptr)
			break;
				
		mask = mask >> 1;
	}

	if( pTemp != nullptr)
	{
		I3ASSERT( pTemp->getLocaleCode() == code);
	}

	return pTemp;
}

#if defined( I3_DEBUG)
void i3OcTreeNode::CheckLocaleCode( UINT32 code, INT32 depth)
{
	INT32 i;

	if( code != getLocaleCode())
	{
		I3TRACE( "Invalid Locale Code : %08(C)X, %08X(F)\n", code, getLocaleCode());
		I3TRACE( "X : %08X - %08X\n", getXLocale( code), getXLocale( getLocaleCode()));
		I3TRACE( "Y : %08X - %08X\n", getYLocale( code), getYLocale( getLocaleCode()));
		I3TRACE( "Z : %08X - %08X\n", getZLocale( code), getZLocale( getLocaleCode()));
	}

	I3ASSERT( code == getLocaleCode());

	depth ++;

	for( i = 0; i < 8; i ++)
	{
		i3OcTreeNode * pChild = getChildOctree( (DIVIDE_NODE) i);

		if( pChild == nullptr)
			continue;

		static UINT32	s_CodeMask[8][3] =
		{
			{	0,		0,		0 },
			{	0,		0,		1 },
			{	1,		0,		0 },
			{	1,		0,		1 },
			{	0,		1,		0 },
			{	0,		1,		1 },
			{	1,		1,		0 },
			{	1,		1,		1 },
		};

		UINT32 x, y, z;
		INT32 shift = ((getMaxDepth() - 1) - depth);

		x = (s_CodeMask[i][0] << shift) << 20;
		y = (s_CodeMask[i][1] << shift) << 10;
		z = (s_CodeMask[i][2] << shift) << 00;

		pChild->CheckLocaleCode( code | x | y | z, depth);

		pChild = nullptr;
	}
}
#endif

i3OcTreeNode *	i3OcTreeNode::FindNodeByLocaleCode( UINT32 code)
{
	return nullptr;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct OCTREE_NODE
	{
		UINT8			m_ID[4] = { 'O', 'C', 'T', '1' };
		UINT32			m_OctreeStyle = 0;
		UINT32			m_nDepth = 0;
		i3::pack::VEC3D	m_Min;
		i3::pack::VEC3D	m_Max;
		i3::pack::VEC3D	m_Center;
		REAL32			m_K = 0.0f;
		UINT32			m_LocaleCode = 0;

		OBJREF			m_pNeighbor[6] = { 0 };
		OBJREF			m_pChildNode[8] = { 0 };	//분할된 자식 노드들

		UINT32			pad[10] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

void	i3OcTreeNode::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	for( i = 0; i < 8; i++)
	{
		i3OcTreeNode * pChild = getChildOctree( (DIVIDE_NODE) i);

		if( pChild != nullptr)
			pChild->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

void	i3OcTreeNode::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
}

UINT32 i3OcTreeNode::OnSave( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Result, Rc = 0;
	pack::OCTREE_NODE	data;

	Result = i3Node::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_OctreeStyle	= getOctreeStyle();
	data.m_nDepth		= m_nDepth;

	data.m_Min.x		= m_Min.x;
	data.m_Min.y		= m_Min.y;
	data.m_Min.z		= m_Min.z;

	data.m_Max.x		= m_Max.x;
	data.m_Max.y		= m_Max.y;
	data.m_Max.z		= m_Max.z;

	data.m_Center.x	= getX( &m_Center);
	data.m_Center.y	= getY( &m_Center);
	data.m_Center.z	= getZ( &m_Center);

	data.m_K			= getK();
	data.m_LocaleCode	= getLocaleCode();

	for( i = 0; i < 6; i++)
	{
		if( getNeighbor( (FACE) i) != nullptr)
		{
			data.m_pNeighbor[i] = pResFile->GetObjectPersistID( getNeighbor( (FACE) i));
		}
	}

	for( i = 0; i < 8; ++i)
	{
		if( m_pChildNode[i] != nullptr)
			data.m_pChildNode[i] = pResFile->GetObjectPersistID( m_pChildNode[i]);
		else
			data.m_pChildNode[i] = 0;
	}

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3OcTreeNode::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Result, Rc = 0;
	pack::OCTREE_NODE	data;

	Result = i3Node::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	setOctreeStyle( data.m_OctreeStyle);
	m_nDepth		= data.m_nDepth;
	i3Vector::Set( &m_Min, data.m_Min.x, data.m_Min.y, data.m_Min.z);
	i3Vector::Set( &m_Max, data.m_Max.x, data.m_Max.y, data.m_Max.z);
	i3Vector::Set( &m_Center, data.m_Center.x, data.m_Center.y, data.m_Center.z);

	setK( data.m_K);

	setLocaleCode( data.m_LocaleCode);

	for( i = 0; i < 6; i++)
	{
		if( data.m_pNeighbor[i] != 0)
		{
			i3OcTreeNode * pNode = (i3OcTreeNode *) pResFile->FindObjectByID( data.m_pNeighbor[i]);

			setNeighbor( (FACE) i, pNode);
		}
	}

	for( i = 0; i < 8; ++i)
	{
		if( data.m_pChildNode[i] != 0)
		{
			i3OcTreeNode * pChildNode = (i3OcTreeNode *)pResFile->FindObjectByID( (OBJREF)data.m_pChildNode[i]);

			I3ASSERT( pChildNode != nullptr);

			I3_REF_CHANGE( m_pChildNode[i], pChildNode);

			pChildNode->SetParent( this);
		}
	}

	return Result;
}
