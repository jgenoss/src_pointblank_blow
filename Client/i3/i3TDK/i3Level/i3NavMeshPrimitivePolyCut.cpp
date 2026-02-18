#include "stdafx.h"
#include "i3NavMeshPrimitivePolyCut.h"

#include "i3LevelGlobalVariable.h"
#include "i3NavMesh.h"
#include "i3NavMeshDataMgr.h"

I3_CLASS_INSTANCE( i3NavMeshPrimitivePolyCut); //, i3NavMeshPrimitive );

void	i3NavMeshPrimitivePolyCut::OnBuildMesh()
{
	SetVertexArray( nullptr );
	SetIndexArray( nullptr );

	i3VertexArray* pVA	= i3VertexArray::new_object_ref();

	// create vertex array
	{
		i3VertexFormat format;
		format.SetHasPosition( true );
		format.SetHasNormal( false );
		format.SetHasColor( false );

		pVA->Create( &format, 6, 0 );
		pVA->setRestoreType( I3G_RESTORE_MEM );

		SetVertexArray( pVA, I3G_PRIM_LINELIST, 0 );
	}
}

void	i3NavMeshPrimitivePolyCut::OnRender()
{
	i3NavMesh* pNav				= i3Level::GetNavMesh();
	i3NavMeshDataMgr* dataMgr	= pNav ? pNav->GetDataMgr() : nullptr;

	if( nullptr == dataMgr || nullptr == m_ctx )
	{
		return;
	}

	const navMeshCuttingInfo& info = pNav->GetCuttingInfo();

	if( navMeshCuttingInfo::CUTTING_FAIL == info._type )
	{
		return;
	}

	i3VertexArray* pVA = GetVertexArray();

	if( nullptr == pVA )
	{
		return;
	}

	switch( info._type )
	{
	case	navMeshCuttingInfo::LINE:
			{
				const bool bHasOtherLine = (-1 != info.otherCutVertex);

				pVA->Lock();				

				{
					pVA->SetPosition( 0, dataMgr->GetVec3D( info.cutVertex ) );
					pVA->SetPosition( 1, (VEC3D*) &info.cutPosition );

					if( bHasOtherLine )
					{
						pVA->SetPosition( 2, dataMgr->GetVec3D( info.otherCutVertex ) );
						pVA->SetPosition( 3, (VEC3D*) &info.cutPosition );
					}
				}

				pVA->Unlock();

				SetPrimitiveCount( bHasOtherLine ? 2 : 1 );
			}
			break;

	case	navMeshCuttingInfo::POLY:
			{
				const navMeshPoly* poly = dataMgr->GetPolygon( info.selPoly );

				if( nullptr != poly )
				{
					pVA->Lock();				

					for( int i = 0; i < 3; ++i )
					{						
						pVA->SetPosition( i * 2 + 0, (VEC3D*) &info.cutPosition );
						pVA->SetPosition( i * 2 + 1, dataMgr->GetVec3D( poly->n[i] ) );
					}

					pVA->Unlock();

					SetPrimitiveCount( 3 );
				}
				else
				{
					return;
				}
				
			}
			break;
	}


	////
	//{
	//	i3VertexArray* pVA = GetVertexArray();

	//	if( pVA )
	//	{
	//		bool bOtherLine = (-1 != pNav->GetPolyCutOther());

	//		pVA->Lock();

	//		pVA->SetPosition( 0, dataMgr->GetVec3D( pNav->GetSelectedPolyCut() ) );
	//		pVA->SetPosition( 1, pNav->GetPolyCutVertex() );

	//		if( bOtherLine )
	//		{
	//			pVA->SetPosition( 2, pNav->GetPolyCutVertex() );
	//			pVA->SetPosition( 3, dataMgr->GetVec3D( pNav->GetPolyCutOther() ) );
	//		}

	//		pVA->Unlock();

	//		SetPrimitiveCount( bOtherLine ? 2 : 1 );
	//	}

	//	/*VEC3D v1, v2;
	//	i3Vector::Copy( &v1, dataMgr->GetVec3D( pNav->GetSelectedPolyCut() ) );
	//	i3Vector::Copy( &v2, pNav->GetPolyCutVertex() );*/
	//}

	COLORREAL cr;
	i3Color::Set( &cr, 1.0f, 1.0f, 1.0f, 1.0f );

	m_ctx->SetZTestEnable( FALSE );
	m_ctx->SetZWriteEnable( FALSE );

	m_ctx->SetColor( &cr );
	m_ctx->SetMaterial( &cr, nullptr, nullptr, 0.0f, I3G_MATERIAL_DIFFUSE );

	RenderElement();
}