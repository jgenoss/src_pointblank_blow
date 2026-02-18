#include "i3SceneDef.h"
#include "i3NavigationMeshNode.h"

#include "i3Node.h"
#include "i3NavigationMesh.h"

I3_CLASS_INSTANCE( i3NavigationMeshNode);

// Child가 TriIndex를 가지고 있으면, 즉 Leaf이면 바운딩 박스 검사는 하지 않는다.

// Rect-A가 Rect-B를 포함하는 지 여부 판단.
inline bool CheckRect2DSubsets(
	REAL32 ax1, REAL32 ay1, REAL32 ax2, REAL32 ay2,
	REAL32 bx1, REAL32 by1, REAL32 bx2, REAL32 by2 )
{
	I3ASSERT( ax1 <= ax2 );
	I3ASSERT( ay1 <= ay2 );
	I3ASSERT( bx1 <= bx2 );
	I3ASSERT( by1 <= by2 );
	if( ax1 <= bx1 && bx2 <= ax2 &&	ay1 <= by1 && ay2 <= ay2 )
	{
		return true;
	}

	return false;
}

// 입력된 바운드 박스가 사분면안에 포함되는지,
// 아니면 현재 영역안에 있는지 없는지를 판단한다.
I3_NVNODE_DIVISION i3NavigationMeshNode::GetSubdivionIndex( i3BoundBox *pBox )
{
	I3ASSERT( m_pBound != nullptr );

	REAL32 min_x = i3Vector::GetX( m_pBound->GetMin());
	REAL32 min_y = i3Vector::GetY( m_pBound->GetMin());
	REAL32 max_x = i3Vector::GetX( m_pBound->GetMax());
	REAL32 max_y = i3Vector::GetY( m_pBound->GetMax());
	REAL32 center_x = i3Math::Mul( ( min_x + max_x ), 0.5f);
	REAL32 center_y = i3Math::Mul( ( min_y + max_y ), 0.5f);

	REAL32 bx1 = i3Vector::GetX( pBox->GetMin());
	REAL32 by1 = i3Vector::GetY( pBox->GetMin());
	REAL32 bx2 = i3Vector::GetX( pBox->GetMax());
	REAL32 by2 = i3Vector::GetY( pBox->GetMax());

	if( CheckRect2DSubsets( min_x, min_y, center_x, center_y, bx1, by1, bx2, by2 ) )
	{
		return I3_NVNODE_LT;
	}
	else if( CheckRect2DSubsets( center_x, min_y, max_x, center_y, bx1, by1, bx2, by2 ) )
	{
		return I3_NVNODE_RT;
	}
	else if( CheckRect2DSubsets( min_x, center_y, center_x, max_y, bx1, by1, bx2, by2 ) )
	{
		return I3_NVNODE_LB;
	}
	else if( CheckRect2DSubsets( center_x, center_y, max_x, max_y, bx1, by1, bx2, by2 ) )
	{
		return I3_NVNODE_RB;
	}
	else if( CheckRect2DSubsets( min_x, min_y, max_x, max_y, bx1, by1, bx2, by2 ) )
	{
		return I3_NVNODE_INSIDE;
	}

	return I3_NVNODE_ETC;
}

// 지정된 사분면에 해당하는 자식 노드를 하나 생성한다.
// 자식 영역의 가로, 세로 길이는 현재 노드의 1/2이다. 즉, 1/4의 면적을 가진다.
i3BoundBox* i3NavigationMeshNode::CreateBoundBoxForSubDivision( I3_NVNODE_DIVISION nDivision )
{
	i3BoundBox *pBox = i3BoundBox::new_object();
	VEC3D vecCenter;
	m_pBound->GetCenter( &vecCenter );

	REAL32 min_x = i3Vector::GetX( m_pBound->GetMin());
	REAL32 min_y = i3Vector::GetY( m_pBound->GetMin());
	REAL32 max_x = i3Vector::GetX( m_pBound->GetMax());
	REAL32 max_y = i3Vector::GetY( m_pBound->GetMax());

	switch( nDivision )
	{
	case I3_NVNODE_LT:
		{
			pBox->SetMinMax( m_pBound->GetMin(), &vecCenter );
		}
		break;
	case I3_NVNODE_RT:
		{
			VEC2D vecRT;
			i3Vector::Set( &vecRT, max_x, min_y );
			pBox->ExtendByVec( &vecRT );

			pBox->ExtendByVec( &vecCenter );
		}
		break;
	case I3_NVNODE_LB:
		{
			VEC2D vecLB;
			i3Vector::Set( &vecLB, min_x, max_y );
			pBox->ExtendByVec( &vecLB );

			pBox->ExtendByVec( &vecCenter );
		}
		break;
	case I3_NVNODE_RB:
		{
			pBox->SetMinMax( &vecCenter, m_pBound->GetMax() );
		}
		break;
	default:
		{
			I3_SAFE_RELEASE( pBox );
			I3ASSERT_0;
		}
		break;
	}
	
	return pBox;
}

// 삼각형 인덱스를 가지는 노드 즉 말단노드(Leaf Node)를 자식으로 추가한다.
void i3NavigationMeshNode::AddNvMeshNodeLeaf( i3NavigationMeshNode *pLeaf )
{
	I3_NVNODE_DIVISION nDivision;

	// 말단 노드가 들어갈 수 있는 영역을 알아낸다.
	nDivision = GetSubdivionIndex( pLeaf->GetBound() );
	I3ASSERT( nDivision != I3_NVNODE_ETC );

	if( nDivision == I3_NVNODE_INSIDE )
	{	// 사분면 안에 들어가지 못하는 노드는 직접 자식으로 등록한다.
		m_NvMeshLeafList.push_back( pLeaf );
		AddChild( pLeaf );
	}
	else
	{	// 사분면 안에 들어가는 경우
		if( m_pNvMeshNodeChild[ nDivision ] == nullptr )
		{	
			// 해당 사분면에 자식 노드가 존재하지 않는다면, 바로 자식 노드로 설정한다.
			m_pNvMeshNodeChild[ nDivision ] = pLeaf;
			AddChild( pLeaf );
		}
		else
		{
			// 해당 사분면에 자식 노드가 존재하는 경우...
			if( m_pNvMeshNodeChild[ nDivision ]->IsNvMeshLeaf() )
			{	// 이전 노드가 말단 노드인 경우

				// 이전 자식 Leaf 백업
				i3NavigationMeshNode *pOldLeaf = m_pNvMeshNodeChild[ nDivision ];
				RemoveChild( pOldLeaf );

				// 새 노드를 만든다.
				m_pNvMeshNodeChild[ nDivision ] = i3NavigationMeshNode::new_object();
				i3BoundBox *pBox = CreateBoundBoxForSubDivision( nDivision);				
				m_pNvMeshNodeChild[ nDivision ]->SetBound( pBox );
				AddChild( m_pNvMeshNodeChild[ nDivision ] );

				// 이전 Leaf와 새 Leaf를 새로 만든 노드에 더한다.
				m_pNvMeshNodeChild[ nDivision ]->AddNvMeshNodeLeaf( pOldLeaf );
				m_pNvMeshNodeChild[ nDivision ]->AddNvMeshNodeLeaf( pLeaf );
			}
			else
			{	// 이전 노드가 말단 노드가 아닌 경우
				// 그 노드의 자식으로 추가한다.
				m_pNvMeshNodeChild[ nDivision ]->AddNvMeshNodeLeaf( pLeaf );
			}
		}
	}
}

void i3NavigationMeshNode::SetTriagleIndex( I3NM_INDEX nTriIndex )
{
	m_TriIndex = nTriIndex;
}

// 주어진 좌표에 해당되는 메쉬 인덱스 찾기
I3NM_INDEX i3NavigationMeshNode::GetMeshIndex( i3NavigationMesh *pMesh, VEC2D *pPos )
{
	
	I3NM_INDEX nIndex;
	i3NavigationMeshNode *pNode;

	if( m_TriIndex == I3_NVMESH_INDEX_NONE )
	{
		for(INT32 i = 0; i < 4; i++ )
		{
			if( m_pNvMeshNodeChild[i] != nullptr )
			{
				nIndex = m_pNvMeshNodeChild[i]->GetMeshIndex( pMesh, pPos );
				if( nIndex != I3_NVMESH_INDEX_NONE )
				{
					return nIndex;
				}
			}
		}


		size_t nCount = m_NvMeshLeafList.size();
		for(size_t i = 0; i < nCount; i++ )
		{
			pNode = m_NvMeshLeafList[i];
			nIndex = pNode->GetMeshIndex( pMesh, pPos );
			if( nIndex != I3_NVMESH_INDEX_NONE )
			{
				return nIndex;
			}
		}

	}
	else
	{	// 현재 자신이 삼각형을 가지고 있는 Leaf 노드라면...
		if( pMesh->CheckTriContain( m_TriIndex, pPos ) )
		{
			return m_TriIndex;
		}
	}

	return I3_NVMESH_INDEX_NONE;
}

void i3NavigationMeshNode::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	// AddChild를 했기 때문에 다음 구문은 필요없다.
	//INT32 i;
	//for( i = 0; i < 4; i++ )
	//{
	//	if( m_pNvMeshNodeChild[i] != nullptr )
	//	{
	//		m_pNvMeshNodeChild[i]->OnBuildObjectList( pList );
	//	}
	//}

	//if( m_pNvMeshLeafList != nullptr )
	//{
	//	i3NavigationMeshNode *pLeaf;
	//	for( i = 0; i < m_pNvMeshLeafList->GetCount(); i++)
	//	{
	//		pLeaf = (i3NavigationMeshNode*)m_pNvMeshLeafList->GetItem( i );
	//		pLeaf->OnBuildObjectList( pList);
	//	}
	//}

	i3Node::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 NVMESH_NODE
	{
		I3NM_INDEX			m_TriIndex = 0;
		OBJREF				m_ChildRef[4] = { 0 };
		UINT16				m_nLeafCount = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3NavigationMeshNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	OBJREF ref;
	pack::NVMESH_NODE Data;

	// Node
	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Data.m_TriIndex		= m_TriIndex;

	for( i = 0; i < 4; i++ )
	{
		if( m_pNvMeshNodeChild[i] == nullptr )
		{
			Data.m_ChildRef[i] = (OBJREF)nullptr;
		}
		else
		{
			Data.m_ChildRef[i] = pResFile->GetObjectPersistID( m_pNvMeshNodeChild[i] );
		}
	}


	I3ASSERT( m_NvMeshLeafList.size() <= 0xFFFF );
	Data.m_nLeafCount = (UINT16)m_NvMeshLeafList.size();


	Rc = pStream->Write( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationMeshNode::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	i3NavigationMeshNode *pLeaf;
	for( i = 0; i < Data.m_nLeafCount; i++)
	{
		pLeaf = m_NvMeshLeafList[i];
		I3ASSERT( pLeaf != nullptr );
		ref = (OBJREF) pResFile->GetObjectPersistID( pLeaf );

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationMeshNode::OnSave()", "Could not write Navigation Mesh Leaf object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3NavigationMeshNode::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3NavigationMeshNode::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	OBJREF ref;
	pack::NVMESH_NODE Data;

	// node
	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationMeshNode::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_TriIndex		= Data.m_TriIndex;

	for( i = 0; i < 4; i++ )
	{
		if( Data.m_ChildRef[i] == (OBJREF)nullptr )
		{
			m_pNvMeshNodeChild[i] = nullptr;
		}
		else
		{
			m_pNvMeshNodeChild[i] = (i3NavigationMeshNode*)pResFile->FindObjectByID( Data.m_ChildRef[i] );
			I3ASSERT( m_pNvMeshNodeChild[i] != nullptr );
		}
	}

	if( Data.m_nLeafCount > 0 )
	{
	
		i3NavigationMeshNode *pLeaf;
		for( i = 0; i < Data.m_nLeafCount; i++)
		{
			Rc = pStream->Read( &ref, sizeof(OBJREF));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3NavigationMeshNode::OnLoad()", "Could not read Leaf object reference.");
				return STREAM_ERR;
			}
			Result += Rc;

			I3ASSERT( ref != 0 );
			pLeaf = (i3NavigationMeshNode *) pResFile->FindObjectByID( ref);
			I3ASSERT( pLeaf != nullptr);

			m_NvMeshLeafList.push_back( pLeaf );
		}

		I3ASSERT( Data.m_nLeafCount == (UINT32)m_NvMeshLeafList.size() );
	}

	return Result;
}
