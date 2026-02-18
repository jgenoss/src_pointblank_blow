#include "stdafx.h"
#include "i3NavMeshPrimitiveList.h"

I3_CLASS_INSTANCE( i3NavMeshPrimitive, i3ElementBase );

i3NavMeshPrimitive::i3NavMeshPrimitive()
{
}

i3NavMeshPrimitive::~i3NavMeshPrimitive()
{
}

I3_CLASS_INSTANCE( i3NavMeshPrimitiveList, i3GeometryAttr );

i3NavMeshPrimitiveList::i3NavMeshPrimitiveList()
{
}

i3NavMeshPrimitiveList::~i3NavMeshPrimitiveList()
{
	for each( i3NavMeshPrimitive* ptr in m_primList )
	{
		ptr->Release();
	}

	m_primList.clear();
}

bool	i3NavMeshPrimitiveList::IsInFrustum( i3RenderContext* ctx, i3NavMeshPrimitive* ptr )
{
	if( NULL == ctx || NULL == ptr )
	{
		return false;
	}

	RT_MATRIX mvp;
	i3Matrix::Copy( &mvp, ctx->GetViewMatrix() );
	i3Matrix::Mul( &mvp, &mvp, ctx->GetProjectMatrix() );


	i3Frustum frustum;
	frustum.Set( &mvp );	

	return frustum.TestSphere( (VEC3D*) &ptr->GetPosition(), ptr->GetScale() );
}

void	i3NavMeshPrimitiveList::Apply( i3RenderContext* ctx )
{
	RT_MATRIX mat, matold;
	COLORREAL cr, crold, crdiffuseold;

	for each( i3NavMeshPrimitive* pPrim in m_primList )
	{
		if( false == IsInFrustum( ctx, pPrim ) )
		{
			continue;
		}

		// 월드 행렬
		{
			i3Matrix::Identity( &mat );

			mat.m00 *= pPrim->GetScale();
			mat.m11 *= pPrim->GetScale();
			mat.m22 *= pPrim->GetScale();

			mat.m30 = pPrim->GetPosition().x;
			mat.m31 = pPrim->GetPosition().y;
			mat.m32 = pPrim->GetPosition().z;
		}

		cr = pPrim->GetColor();

		i3Matrix::Copy( &matold, ctx->GetWorldMatrix() );

		crold			= *(ctx->GetColor());
		crdiffuseold	= *(ctx->getMaterialDiffuse());

		ctx->SetWorldMatrix( &mat );
		ctx->SetColor( &cr );
		ctx->SetMaterial( &crdiffuseold, NULL, NULL, NULL, I3G_MATERIAL_DIFFUSE );

		i3GeometryAttr::Apply( ctx );

		// 이전으로 돌린다
		ctx->SetWorldMatrix( &matold );
		ctx->SetColor( &crold );
		ctx->SetMaterial( &crdiffuseold, NULL, NULL, NULL, I3G_MATERIAL_DIFFUSE );
	}
}

void	i3NavMeshPrimitiveList::BuildGeometry()
{
	_BuildGeometry_Sphere();
}


void	i3NavMeshPrimitiveList::_BuildGeometry_Sphere( UINT32 hdiv, UINT32 vdiv )
{
	SetVertexArray( NULL );
	SetIndexArray( NULL );

	i3VertexArray* pVA	= i3VertexArray::NewObjectRef();
	i3IndexArray* pIA	= i3IndexArray::NewObjectRef();

	i3VertexFormat format;

	RT_VEC3D vec, norm;

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
		i3Vector::Set( &vec, __RT(0.0f), __RT(0.5f), __RT_0);
		pVA->SetPosition( 0, &vec);
		pVA->SetNormal( 0, &vec);		

		i3Vector::Set( &vec, __RT(0.0f), __RT(-0.5f), __RT_0);
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

				i3Vector::Set( &vec, __RT(x * scale), __RT(sy), __RT(z * scale));

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

		pIA->Create( count, 0);				// Line List
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

void	i3NavMeshPrimitiveList::_BuildGeometry_SelBox()
{
	SetVertexArray( NULL );
	SetIndexArray( NULL );

	i3VertexArray* pVA	= i3VertexArray::NewObjectRef();
	i3IndexArray* pIA	= i3IndexArray::NewObjectRef();

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

		pIA->Create( 48, 0);
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