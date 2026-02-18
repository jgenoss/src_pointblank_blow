#include "i3SceneDef.h"
#include "i3BSPNode.h"

class i3Geometry;
class i3GeometryAttr;

I3_CLASS_INSTANCE( i3BSPNode);

I3_TRACE_RESULT I3SG_NODE_PROC( i3BSPNode )( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3BSPNode * pBSPNode = (i3BSPNode *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	if( pBSPNode->IsLeaf() )
	{
		// 마지막 노드이다
		Result = I3_TRACE_RESULT_STOP;
	}
	
	return Result;
}

i3BSPNode::~i3BSPNode(void)
{
	ReleaseParts();
}

void i3BSPNode::Create( void )
{
	// 플래너는 Leaf라면 생성할 필요가 없다.
}

void i3BSPNode::ReleaseParts( void )
{
	m_Parts.clear();
}

i3::vector<void*>* i3BSPNode::CollisionTest( VEC3D * pVec )
{
	VEC3D	Vec;
	i3BSPNode * pNode;

	if( m_bLeaf )
	{
		// 
		return &m_Parts;	// 
	}
	else
	{
		// 하위 노드를 검사
		i3Vector::Set( &Vec, i3Vector::GetX( getPlane() ), i3Vector::GetY( getPlane() ), i3Vector::GetZ( getPlane() ) );

		if( i3Vector::Dot( pVec, &Vec ) < i3Vector::GetW( getPlane() ) )
		{
			pNode = (i3BSPNode*) GetChild( 0 );
			if( pNode )
				return pNode->CollisionTest( pVec );
		}
		else if( GetChildCount() > 1 )
		{
			pNode = (i3BSPNode*) GetChild( 1 );
			if( pNode )
				return pNode->CollisionTest( pVec );
		}
	}
	return nullptr;
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가함. 2017.02.15. soon9
#endif

namespace pack
{
	struct ALIGN4 BSPNODE
	{
		i3::pack::VEC4D	m_Plane;					// 나눠줄 플랜
		bool			m_bLeaf = false;			// 차일드가 없다
		INT32			m_PartCount = 0;			// 오브젝트 갯수
		OBJREF			m_Parts = 0;				// 오브젝트

	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3BSPNode::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	pack::BSPNODE BSPNode;

	BSPNode.m_bLeaf		= IsLeaf();
	BSPNode.m_Plane.x	= i3Vector::GetX( getPlane() );
	BSPNode.m_Plane.y	= i3Vector::GetY( getPlane() );
	BSPNode.m_Plane.z	= i3Vector::GetZ( getPlane() );
	BSPNode.m_Plane.w	= i3Vector::GetW( getPlane() );

	BSPNode.m_PartCount	= getPartCount();
	//BSPNode.m_Parts = (OBJREF) &m_Parts;

	Rc = pStream->Write( &BSPNode, sizeof( pack::BSPNODE ) );
	if( Rc == STREAM_ERR )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not save BSPNode." );
		return Rc;
	}
	Result += Rc;

	return Result;
}

UINT32 i3BSPNode::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::BSPNODE BSPNode;
	VEC4D	Vec;

	Rc = pStream->Read( &BSPNode, sizeof( pack::BSPNODE ) );
	if( Rc == STREAM_ERR )
	{
		I3PRINTLOG(I3LOG_FATAL,  "i3BSPNode::OnLoad( read data failed. )" );
		return STREAM_ERR;
	}
	Result += Rc;

	m_bLeaf		= BSPNode.m_bLeaf;
	i3Vector::Set( &Vec, BSPNode.m_Plane.x, BSPNode.m_Plane.y, BSPNode.m_Plane.z, BSPNode.m_Plane.w );
	setPlane( &Vec );
	
	m_Parts.resize( BSPNode.m_PartCount );
	
	/*  이 부분은 구현이 될되있는것으로 판단되어 주석을 겁니다...( 근거는 OnSave함수의 BSPNode.m_Parts = (OBJREF) &m_Parts;가 주석으로 된 부분... 주석이 없더라도
	//																포인터 주소값을 저장대상으로 하는 것은 이치에 맞지 않는다..) 2012.05.18.수빈
	List * pList;
	pList = (List *) pResFile->FindObjectByID( BSPNode.m_Parts );
	for(size_t i = 0; i < m_Parts.size(); i++ )
		m_Parts[i] = pList->Items[ i ];
	*/
	return Result;
}

