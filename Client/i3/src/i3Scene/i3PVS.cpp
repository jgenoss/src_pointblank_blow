#include "i3SceneDef.h"
#include "i3PVS.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3PVS, i3Node );

I3_TRACE_RESULT I3SG_NODE_PROC( i3PVS )( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3PVS * pPVS = (i3PVS*) pNode;
	I3_TRACE_RESULT	Result = I3_TRACE_RESULT_CONTINUE;

	return Result;
}

i3PVS::i3PVS()
{
}

i3PVS::~i3PVS()
{
}

void	i3PVS::_RemoveAllList(void)
{
	INT32	i;
	i3PVSAttr * pPVS;

	for( i = 0; i < m_LeafList.GetCount(); i++)
	{
		pPVS = (i3PVSAttr*) m_LeafList.Items[ i ];

		I3_SAFE_RELEASE( pPVS );
	}

	m_LeafList.Clear();
}

void	i3PVS::_AddLeaf( i3PVSAttr * pLeaf )
{
	pLeaf->AddRef();

	m_LeafList.Add( pLeaf );
}

void	i3PVS::_SetLeaf( UINT32 idx, i3PVSAttr * pLeaf )
{
	m_LeafList.SetItem( idx, pLeaf );
}

void	i3PVS::_RemoveLeaf( i3PVSAttr * pLeaf )
{
	m_LeafList.Remove( pLeaf );
}

void	i3PVS::Create()
{
}

void	i3PVS::Build()
{
	I3ASSERT( m_pGeometry );

}

void	i3PVS::BuildLeaf()
{
	I3ASSERT( m_pGeometry );
	// 월드에 대한 각 블럭 설정
	// x 최하 좌표에 대한 y값을 산출하여 이어진 라인을 찾는다.


	// 포탈을 생성

	// Area에 대한 리스트 작성
}

UINT32	i3PVS::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Rc = 0;

	return Result;
}

UINT32	i3PVS::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Rc = 0;

	return Result;
}

