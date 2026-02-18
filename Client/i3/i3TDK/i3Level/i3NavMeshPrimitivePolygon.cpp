#include "stdafx.h"
#include "i3NavMeshPrimitivePolygon.h"

#include "i3LevelGlobalVariable.h"
#include "i3NavMesh.h"
#include "i3NavMeshDataMgr.h"

I3_CLASS_INSTANCE( i3NavMeshPrimitivePolygon); //, i3NavMeshPrimitive );

void	i3NavMeshPrimitivePolygon::SetVB( int idx, const VEC3D& v )
{
	i3VertexArray* pVA = GetVertexArray();

	if( nullptr == pVA )
	{
		return;
	}

	if( pVA->Lock() )
	{
		pVA->SetPosition( idx, (VEC3D*) &v );
		pVA->Unlock();
	}
}

void	i3NavMeshPrimitivePolygon::OnBuildMesh()
{
	SetVertexArray( nullptr );
	SetIndexArray( nullptr );

	i3VertexArray* pVA	= i3VertexArray::new_object_ref();
	i3IndexArray* pIA	= i3IndexArray::new_object_ref();

	// create vertex array
	{
		i3VertexFormat format;
		format.SetHasPosition( true );
		format.SetHasNormal( false );
		format.SetHasColor( true );

		pVA->Create( &format, MAX_VERTEXARRAY_SIZE, 0 );
		pVA->setRestoreType( I3G_RESTORE_MEM );

		SetVertexArray( pVA, I3G_PRIM_TRILIST, 0 );
	}

	// create index array
	{
		pIA->Create( MAX_INDEXARRAY_SIZE, 0, I3G_IDX_32BITS);
		SetIndexArray( pIA );
	}	
}

void	i3NavMeshPrimitivePolygon::OnRender()
{
	if( nullptr == m_ctx )
	{
		return;
	}

	// re build index buffer
	ReBuild_IB();

	// render poly
	{
		m_ctx->SetBlendEnable( TRUE );
		m_ctx->SetSrcBlend( I3G_BLEND_SRCALPHA );
		m_ctx->SetDestBlend( I3G_BLEND_INVSRCALPHA );
		m_ctx->SetBlendOperation( I3G_BLEND_OP_ADD );

		m_ctx->SetZTestEnable( TRUE );
		m_ctx->SetZWriteEnable( TRUE );

		// line
		{
			_Render_PolyList( m_crLine, m_matLine, 0, m_nNormPrimCnt, I3G_RENDER_LINE );
			_Render_PolyList( m_crLine, m_matLine, m_nNormPrimCnt * 3, m_nSelPrimCnt, I3G_RENDER_LINE );

			if( m_bHasOverPoly )
				_Render_PolyList( m_crLine, m_matLine, (m_nNormPrimCnt + m_nSelPrimCnt) * 3, 1, I3G_RENDER_LINE );
		}

		// poly
		{
			_Render_PolyList( m_crNormal, m_matPoly, 0, m_nNormPrimCnt, I3G_RENDER_POLYGON );
			_Render_PolyList( m_crSelect, m_matPoly, m_nNormPrimCnt * 3, m_nSelPrimCnt, I3G_RENDER_POLYGON );

			if( m_bHasOverPoly )
			{
				_Render_PolyList( m_bSelOverPoly ? m_crOverSelect : m_crOverNormal
								, m_matPoly
								, (m_nNormPrimCnt + m_nSelPrimCnt) * 3
								, 1
								, I3G_RENDER_POLYGON );
			}			
		}
	}
}

void	i3NavMeshPrimitivePolygon::ReBuild_VB()
{
	i3VertexArray* pVA	= GetVertexArray();

	i3NavMesh* pNav				= i3Level::GetNavMesh();
	i3NavMeshDataMgr* dataMgr	= pNav ? pNav->GetDataMgr() : nullptr;

	if( nullptr == pVA || nullptr == dataMgr )
	{
		return;
	}

	const i3::vector< navMeshVertex >& vertexList	= dataMgr->GetVertexList();
	const i3::vector< navMeshPoly >& polyList		= dataMgr->GetPolygonList();

	I3ASSERT( (polyList.size() * 3) < MAX_VERTEXARRAY_SIZE );

	if( TRUE == pVA->Lock() )
	{
		COLORREAL cr;
		i3Color::Set( &cr, 1.0f,1.0f,1.0f,1.0f );

		int idx = 0;
		for each( navMeshVertex vert in vertexList )
		{
			if( true == pNav->IsSelectedVertex( idx ) )
				i3Vector::Add( &vert.vec3D, &vert.vec3D, pNav->GetVertexMovePoint() );

			pVA->SetPosition( idx, &vert.vec3D );
			pVA->SetColor( idx, &cr );
			++idx;
		}

		pVA->Unlock();		
	}
}


void	i3NavMeshPrimitivePolygon::ReBuild_IB()
{
	m_nNormPrimCnt	= 0;
	m_nSelPrimCnt	= 0;
	m_bHasOverPoly	= false;
	m_bSelOverPoly	= false;

	i3IndexArray* pIA	= GetIndexArray();

	i3NavMesh* pNav				= i3Level::GetNavMesh();
	i3NavMeshDataMgr* dataMgr	= pNav ? pNav->GetDataMgr() : nullptr;

	if( nullptr == pIA || nullptr == dataMgr )
	{
		return;
	}

	if( TRUE == pIA->Lock() )
	{	
		navMeshPoly mouseOver;
		i3::vector< navMeshPoly > selList;

		int idx = 0;

		for each( int nPoly in pNav->m_InFrustumPolygonList )
		{
			if( dataMgr->IsRemovedPolygon( nPoly ) )
			{
				continue;
			}

			const navMeshPoly* poly = dataMgr->GetPolygon( nPoly );

			if( nullptr == poly )
			{
				continue;
			}

			if( true == pNav->IsMouserOverPolygon( nPoly ) )
			{
				m_bHasOverPoly = true;
				m_bSelOverPoly = pNav->IsSelectedPolygon( nPoly );

				mouseOver = *poly;
			}	
			else
			{
				if( false == pNav->IsSelectedPolygon( nPoly ) )
				{
					for( int i = 0; i < 3; ++i )
						pIA->SetIndex( idx++, poly->n[i] );

					++m_nNormPrimCnt;
				}
				else
				{
					selList.push_back( *poly );
				}
			}
		}

		for each( const navMeshPoly& poly in selList )
		{
			for( int i = 0; i < 3; ++i )
				pIA->SetIndex( idx++, poly.n[i] );
		}

		if( m_bHasOverPoly )
		{
			for( int i = 0; i < 3; ++i )
				pIA->SetIndex( idx++, mouseOver.n[i] );
		}

		pIA->Unlock();

		m_nSelPrimCnt = static_cast<int>(selList.size());
	}
}

void	i3NavMeshPrimitivePolygon::_Render_PolyList( COLORREAL& cr, MATRIX& mat, int nStart, int nPrimCnt, I3G_RENDER_MODE _mode )
{
	if( nullptr == m_ctx || nPrimCnt <= 0 )
	{
		return;
	}

	m_ctx->SetWorldMatrix( &mat );
	m_ctx->SetColor( &cr );
	m_ctx->SetMaterial( &cr, nullptr, nullptr, 0.0f, I3G_MATERIAL_DIFFUSE );
	m_ctx->SetRenderMode( _mode );

	SetStartIndex( nStart );
	SetPrimitiveCount( nPrimCnt );
	RenderElement();
}