#include "stdafx.h"
#include "i3NavMeshPrimitive.h"

#include "i3NavMeshDataMgr.h"
#include "i3LevelGlobalVariable.h"
#include "i3NavMesh.h"

I3_CLASS_INSTANCE( i3NavMeshPrimitive);

bool	i3NavMeshPrimitive::Init( i3Geometry* pParent )
{
	if( nullptr != pParent )
		pParent->AppendGeometryAttr( this );

	OnBuildMesh();
	return true;
}

void	i3NavMeshPrimitive::_BuildGeometry_Sphere( UINT32 hdiv, UINT32 vdiv )
{
	SetVertexArray( nullptr );
	SetIndexArray( nullptr );

	i3VertexArray* pVA	= i3VertexArray::new_object_ref();
	i3IndexArray* pIA	= i3IndexArray::new_object_ref();

	i3VertexFormat format;

	VEC3D vec, norm;

	UINT32 i, j, idx;
	REAL32 x, y, z, ystep, scale, sy;
	REAL32 deg, step;

	UINT32 VertexCount, VertexPerLineCount, TotalTriCount, TotalLineCount, TriPerLine, BodyLineCount;

	TotalLineCount = (vdiv + 2);
	BodyLineCount = vdiv;
	VertexPerLineCount = hdiv;

	VertexCount = (VertexPerLineCount * BodyLineCount) + 2;
	TriPerLine = (VertexPerLineCount * 2) * BodyLineCount;
	TotalTriCount = TriPerLine + (hdiv * 2);	// 아래와 위에 Fan 구조로 덮개가 붙는다.

	// 높이 스텝
	ystep = 1.0f / (vdiv + 1);

	// 회전값 스텝
	step = 360.0f / hdiv;

	// Vertex Array
	{
		format.SetHasPosition( true);
		format.SetHasNormal( true);
		format.SetHasColor( false);

		pVA->Create( &format, VertexCount, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		// 최상/하단 Vertex 2개는 항상 선두에 추가한다.
		i3Vector::Set( &vec, 0.0f, 0.5f, 0.0f);
		pVA->SetPosition( 0, &vec);
		pVA->SetNormal( 0, &vec);		

		i3Vector::Set( &vec, 0.0f, -0.5f, 0.0f);
		pVA->SetPosition( 1, &vec);
		pVA->SetNormal( 1, &vec);

		idx = 2;
		y = ystep;

		for( i = 0; i < BodyLineCount; i++)
		{
			deg = 0.0f;
			sy = i3Math::vcos( y * 2.0f) * 0.5f;
			scale = i3Math::vsin( y * 2.0f) * 0.5f;

			for( j = 0; j < hdiv; j++)
			{
				x = i3Math::sin( I3_DEG2RAD( deg));
				z = i3Math::cos( I3_DEG2RAD( deg));

				i3Vector::Set( &vec, x * scale, sy, z * scale);

				i3Vector::Normalize( &norm, &vec);

				pVA->SetPosition( idx, &vec);
				pVA->SetNormal( idx, &norm);			

				idx++;
				deg += step;
			}

			y += ystep;
		}

		pVA->Unlock();
	}

	UINT32 count;

	{
		UINT32 sidx, preidx = 0;
		UINT32 i1, i2, i3;
		BOOL bBody;

		count = ((hdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((hdiv * 2) * 3);

		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		// Caps
		for( j = 0; j <= (UINT32) vdiv; j++)
		{
			for( i = 0; i < (UINT32) hdiv; i++)
			{
				bBody = FALSE;

				if( j == 0)
				{
					i1 = 0;
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + hdiv))
						i3 = sidx;
				}
				else if( j == vdiv)
				{
					i1 = preidx + i + 1;
					i2 = preidx + i;
					i3 = 1;

					if( i1 >= (preidx + hdiv))
						i1 = preidx;
				}
				else
				{
					i1 = (preidx + i);
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + hdiv))
						i3 = sidx;
					bBody = TRUE;
				}

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;

				if( bBody)
				{
					//i1 = i2;
					i2 = i3;
					i3 = preidx + i + 1;

					if( i3 >= (preidx + hdiv))
						i3 = preidx;

					pIA->SetIndex( idx,	i1);	idx++;
					pIA->SetIndex( idx,	i2);	idx++;
					pIA->SetIndex( idx,	i3);	idx++;
				}
			}

			preidx = sidx;

			sidx += hdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - hdiv;
		}

		pIA->Unlock();
	}

	SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3 );
	SetIndexArray( pIA );
}

void	i3NavMeshPrimitive::_BuildGeometry_SelBox()
{
	SetVertexArray( nullptr );
	SetIndexArray( nullptr );

	i3VertexArray* pVA	= i3VertexArray::new_object_ref();
	i3IndexArray* pIA	= i3IndexArray::new_object_ref();

	static REAL32 selBoxVertex[32][3] =
	{
		{	-0.5f,			-0.5f,			-0.5f},
		{	-0.5f + 0.3f,	-0.5f,			-0.5f},
		{	-0.5f,			-0.5f + 0.3f,	-0.5f},
		{	-0.5f,			-0.5f,			-0.5f + 0.3f},

		{	-0.5f,			0.5f,			-0.5f},
		{	-0.5f + 0.3f,	0.5f,			-0.5f},
		{	-0.5f,			0.5f - 0.3f,	-0.5f},
		{	-0.5f,			0.5f,			-0.5f + 0.3f},

		{	-0.5f,			-0.5f,			0.5f},
		{	-0.5f + 0.3f,	-0.5f,			0.5f},
		{	-0.5f,			-0.5f + 0.3f,	0.5f},
		{	-0.5f,			-0.5f,			0.5f - 0.3f},

		{	-0.5f,			0.5f,			0.5f},
		{	-0.5f + 0.3f,	0.5f,			0.5f},
		{	-0.5f,			0.5f - 0.3f,	0.5f},
		{	-0.5f,			0.5f,			0.5f - 0.3f},

		{	0.5f,			-0.5f,			-0.5f},
		{	0.5f - 0.3f,	-0.5f,			-0.5f},
		{	0.5f,			-0.5f + 0.3f,	-0.5f},
		{	0.5f,			-0.5f,			-0.5f + 0.3f},

		{	0.5f,			0.5f,			-0.5f},
		{	0.5f - 0.3f,	0.5f,			-0.5f},
		{	0.5f,			0.5f - 0.3f,	-0.5f},
		{	0.5f,			0.5f,			-0.5f + 0.3f},

		{	0.5f,			-0.5f,			0.5f},
		{	0.5f - 0.3f,	-0.5f,			0.5f},
		{	0.5f,			-0.5f + 0.3f,	0.5f},
		{	0.5f,			-0.5f,			0.5f - 0.3f},

		{	0.5f,			0.5f,			0.5f},
		{	0.5f - 0.3f,	0.5f,			0.5f},
		{	0.5f,			0.5f - 0.3f,	0.5f},
		{	0.5f,			0.5f,			0.5f - 0.3f}
	};

	// Vertex Array
	{
		VEC3D vec;
		i3VertexFormat format;

		format.SetHasPosition( true);


		pVA->Create( &format, 32, 0);
		pVA->Lock();

		MATRIX tmp;

		i3Matrix::SetScale( &tmp, 1.01f, 1.01f, 1.01f);

		for( int i = 0; i < 32; ++i )
		{
			setXYZ( &vec, selBoxVertex[i][0], selBoxVertex[i][1], selBoxVertex[i][2]);

			i3Vector::TransformCoord( &vec, &vec, &tmp);
			pVA->SetPosition( i, &vec);
		}

		pVA->Unlock();
	}

	// Index Array
	{
		INT32 base, count;

		pIA->Create( 48, 0, pVA->GetCount());
		pIA->Lock();

		count = 1;
		base = 0;

		for( int i = 0; i < 48; ++i )
		{
			INT32 idx;

			if( i & 1)
			{
				idx = base + count;

				count++;

				if( count > 3)
				{
					count = 1;
					base += 4;
				}
			}
			else
			{
				idx = base;
			}

			pIA->SetIndex( i, (UINT16)idx);
		}

		pIA->Unlock();
	}

	SetVertexArray( pVA, I3G_PRIM_LINELIST, 24 );
	SetIndexArray( pIA );
}

void	i3NavMeshPrimitive::Apply( i3RenderContext* ctx )
{
	m_ctx = ctx;

	if( i3Level::GetNavMesh() )
	{
		if( i3Level::GetNavMesh()->IsVisible() )
		{
			_PrevRenderState();

			{
				OnRender();
			}			

			_ReSetRenderState();
		}
	}

	m_ctx = nullptr;
}

void	i3NavMeshPrimitive::RenderElement()
{
	if( m_ctx )
		i3GeometryAttr::Apply( m_ctx );
}

void	i3NavMeshPrimitive::_PrevRenderState()
{
	if( nullptr == m_ctx ) 
	{
		return;
	}

	m_blendEnable	= m_ctx->GetBlendEnable();
	m_srcBlend		= m_ctx->GetSrcBlend();
	m_destBlend		= m_ctx->GetDestBlend();
	m_blendOp		= m_ctx->GetBlendOperation();
	m_renderMode	= m_ctx->GetRenderMode();
	m_cr			= *(m_ctx->GetColor());
	m_crDiffuse		= *(m_ctx->getMaterialDiffuse());
	m_zwrite		= m_ctx->GetZWriteEnable();
	m_ztest			= m_ctx->GetZTestEnable();

	i3Matrix::Copy( &m_mat, m_ctx->GetWorldMatrix() );
}

void	i3NavMeshPrimitive::_ReSetRenderState()
{
	if( nullptr == m_ctx )
	{
		return;
	}

	m_ctx->SetWorldMatrix( &m_mat );
	m_ctx->SetColor( &m_cr );
	m_ctx->SetMaterial( &m_crDiffuse, nullptr, nullptr, 0.0f, I3G_MATERIAL_DIFFUSE );
	m_ctx->SetBlendEnable( m_blendEnable );
	m_ctx->SetSrcBlend( m_srcBlend );
	m_ctx->SetDestBlend( m_destBlend );
	m_ctx->SetBlendOperation( m_blendOp );
	m_ctx->SetRenderMode( m_renderMode );
	m_ctx->SetZTestEnable( m_ztest );
	m_ctx->SetZWriteEnable( m_zwrite );
}