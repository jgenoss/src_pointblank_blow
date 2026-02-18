#include "i3SceneUtil.h"
#include "i3Math.h"

I3_EXPORT_SGUT
i3Node *	i3SceneUtil::BuildLine( i3VertexArray *&pVA, INT32 nLineCount, COLORREAL *pColor )
{
	i3Node * pRoot;
	i3AttrSet * pLineRoot;
	i3VertexFormat format;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;

	pRoot = i3Node::new_object();

	pLineRoot = i3AttrSet::new_object_ref();
	pRoot->AddChild( pLineRoot);

	if( pColor != nullptr)
	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
		pAttr->Set( pColor );
		pLineRoot->AddAttr( pAttr);
	}

	{
		i3TextureEnableAttr *pAttr = i3TextureEnableAttr::new_object_ref();
		pAttr->Set(false);
		pLineRoot->AddAttr( pAttr );
	}

	pGeo = i3Geometry::new_object_ref();
	pLineRoot->AddChild( pGeo);

	format.SetHasPosition( true );
	format.SetHasNormal( false );

	pVA = i3VertexArray::new_object_ref();
	pVA->Create( &format, 2 * nLineCount, 0);
	pVA->setRestoreType( I3G_RESTORE_MEM);
	pVA->Lock();

	INT32 i;
	VEC3D vec;

	i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);

	for( i = 0; i < 2 * nLineCount; i++ )
	{
		pVA->SetPosition( i, &vec);
	}

	pVA->Unlock();

	pGeoAttr = i3GeometryAttr::new_object_ref();
	pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, nLineCount, 0);

	pGeo->AppendGeometryAttr( pGeoAttr);

	return pRoot;
}

I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildSphereMesh( REAL32 radius, bool bTri, bool bLine, UINT32 hdiv, UINT32 vdiv, COLORREAL * pLineColor, COLORREAL * pAmbient, COLORREAL * pDiffuse)
{
	UINT32 i, j, idx;
	REAL32 x, y, z, ystep, scale, sy;
	REAL32 deg, step;
	i3VertexFormat format;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;
	i3Node * pRoot;
	i3AttrSet * pLineAttrSet = nullptr;
	i3AttrSet * pTriAttrSet = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	UINT32 VertexCount, VertexPerLineCount, TotalTriCount, TotalLineCount, TriPerLine, BodyLineCount;
	VEC3D vec, norm;
	VEC2D uv;

	I3ASSERT( hdiv >= 3);
	I3ASSERT( vdiv >= 1);

	pRoot = i3Node::new_object();

	if( bTri)
	{
		pTriAttrSet = i3AttrSet::new_object_ref();

		if( pDiffuse != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			
			if( pDiffuse != nullptr)
				pAttr->SetDiffuse( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet);
	}

	if( bLine)
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			pAttr->Set( pLineColor);

			pLineAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pLineAttrSet);
	}


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
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, VertexCount, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		// 최상/하단 Vertex 2개는 항상 선두에 추가한다.
		i3Vector::Set( &vec, 0.0f, radius * 0.5f, 0.0f);
		pVA->SetPosition( 0, &vec);
		pVA->SetNormal( 0, &vec);
		i3Vector::Set( &uv, 0.0f, 0.0f);
		pVA->SetTextureCoord( 0, 0, &uv);

		i3Vector::Set( &vec, 0.0f, radius * -0.5f, 0.0f);
		pVA->SetPosition( 1, &vec);
		pVA->SetNormal( 1, &vec);
		i3Vector::Set( &uv, 0.0f, 1.0f);

		idx = 2;
		y = ystep;

		for( i = 0; i < BodyLineCount; i++)
		{
			deg = 0.0f;
			sy = i3Math::vcos( y * 2.0f) * 0.5f * radius;
			scale = i3Math::vsin( y * 2.0f) * 0.5f * radius;

			for( j = 0; j < hdiv; j++)
			{
				x = i3Math::sin( I3_DEG2RAD( deg));
				z = i3Math::cos( I3_DEG2RAD( deg));

				i3Vector::Set( &vec, x * scale, sy, z * scale);

				i3Vector::Normalize( &norm, &vec);

				pVA->SetPosition( idx, &vec);
				pVA->SetNormal( idx, &norm);

				i3Vector::Set( &uv, ((REAL32)j / (hdiv - 1)), (((REAL32)i + 1) / (BodyLineCount + 2)));

				pVA->SetTextureCoord( 0, idx, &uv);

				idx++;
				deg += step;
			}

			y += ystep;
		}

		pVA->Unlock();
	}

	// Index Array for Triangle
	if( bTri)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3;
		bool bBody;

		count = ((hdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((hdiv * 2) * 3);

		pIA = i3IndexArray::new_object_ref();
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
				bBody = false;

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
					bBody = true;
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

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	// Index Array for Line
	if( bLine)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3, i4;

		count = TotalTriCount * 2 * 2;

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		for( j = 0; j <= (UINT32) vdiv; j++)
		{
			for( i = 0; i < (UINT32) hdiv; i++)
			{
				if( j == 0)
					i1 = 0;
				else if( j == vdiv)
					i1 = 1;
				else
					i1 = (preidx + i);

				i2 = sidx + i;
				i3 = sidx + i;
				i4 = sidx + i + 1;

				if( i4 >= (sidx + hdiv))
					i4 = sidx;

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;
				pIA->SetIndex( idx,	i4);	idx++;
			}

			preidx = sidx;

			sidx += hdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - hdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, count >> 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}

	return pRoot;
}

I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildAxisMesh( INT32 hdiv, REAL32 Length, REAL32 rad, bool bTri, bool bLine, COLORREAL * pAxisColor, COLORREAL * pConeColor)
{
	i3Node * pRoot;
	i3AttrSet * pLineRoot;
	i3AttrSet * pConeRoot;
	i3VertexFormat format;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;
	VEC3D vec;
	INT32 i, VertexCount;
	UINT32 idx;

	pRoot = i3Node::new_object();

	pLineRoot = i3AttrSet::new_object_ref();
	pRoot->AddChild( pLineRoot);

	pConeRoot = i3AttrSet::new_object_ref();
	pRoot->AddChild( pConeRoot);

	if( pAxisColor != nullptr)
	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		pAttr->Set( pAxisColor);
		pLineRoot->AddAttr( pAttr);
	}

	if( pConeColor != nullptr)
	{
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pConeColor);
			pConeRoot->AddAttr( pAttr);
		}

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

			pAttr->SetDiffuse( pConeColor);
			pConeRoot->AddAttr( pAttr);
		}
	}

	pGeo = i3Geometry::new_object_ref();
	pLineRoot->AddChild( pGeo);

	format.SetHasPosition( true);
	format.SetHasNormal( false);

	// 몸통 선
	{
		i3VertexArray * pVA;

		pVA = i3VertexArray::new_object_ref();

		pVA->Create( &format, 2, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);
		pVA->SetPosition( 0, &vec);

		i3Vector::Set( &vec, Length, 0.0f, 0.0f);
		pVA->SetPosition( 1, &vec);

		pVA->Unlock();

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0);

		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	pGeo = i3Geometry::new_object_ref();
	pConeRoot->AddChild( pGeo);

	// Vertex Array for Cone
	{
		REAL32 x, y, z, deg, step;
		VEC3D norm, p0, p1;
		i3VertexFormat fmt;

		fmt.SetHasPosition( true);
		fmt.SetHasNormal( true);

		VertexCount = (hdiv * 3) + 1;

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &fmt, VertexCount, 0);		// 전체 둘레를 20등분한다. + (끝점 2개)
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		idx = 0;
		x = Length * 0.75f;
		deg = 0.0f;
		
		REAL32 norm_x = sinf( atan2f( rad, x));

		step = 360.0f / (REAL32) hdiv;

		// 뾰족한 끝 부분.
		for( i = 0; i < hdiv; i++)
		{
			y = i3Math::sin( I3_DEG2RAD( deg));
			z = i3Math::cos( I3_DEG2RAD( deg));

			// Positions
			i3Vector::Set( &p0, Length, 0.0f, 0.0f);
			pVA->SetPosition( i + (hdiv * 0), &p0);

			i3Vector::Set( &p1, x, y * rad, z * rad);
			pVA->SetPosition( i + (hdiv * 1), &p1);			// 좌표값은 같지만, Normal이 다르다.
			pVA->SetPosition( i + (hdiv * 2), &p1);

			// Normal
			{
				i3Vector::Set( &norm, norm_x, y, z);
				i3Vector::Normalize( &norm, &norm);
			}

			pVA->SetNormal( i + (hdiv * 0), &norm);
			pVA->SetNormal( i + (hdiv * 1), &norm);

			i3Vector::Set( &norm, -1.0f, 0.0f, 0.0f);
			pVA->SetNormal( i + (hdiv * 2), &norm);

			deg += step;
		}

		i3Vector::Set( &vec, x, 0.0f, 0.0f);
		pVA->SetPosition( (hdiv * 3), &vec);

		i3Vector::Set( &norm, -1.0f, 0.0f, 0.0f);
		pVA->SetNormal(  (hdiv * 3), &norm);

		pVA->Unlock();
	}

	// Triangles
	if( bTri)
	{
		UINT32 count, sidx;
		UINT32 i1, i2, i3;

		count = (hdiv * 2);			//  Caps

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count * 3, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 0;

		// Cone 부분
		for( i = 0; i < hdiv; i++)
		{
			i1 = i + hdiv;
			i2 = i;

			if( i < (hdiv - 1))
				i3 = i1 + 1;
			else
				i3 = 0 + hdiv;

			pIA->SetIndex( idx + 0,	i1);
			pIA->SetIndex( idx + 1,	i2);
			pIA->SetIndex( idx + 2,	i3);

			idx += 3;
		}

		// 바닥 부분
		sidx = (hdiv * 2);
		for( i = 0; i < hdiv; i++)
		{
			i1 = i;
			i2 = hdiv;

			if( i < (hdiv - 1))
				i3 = i1 + 1;
			else
				i3 = 0;

			pIA->SetIndex( idx + 0,	i3 + sidx);
			pIA->SetIndex( idx + 1,	i2 + sidx);
			pIA->SetIndex( idx + 2,	i1 + sidx);

			idx += 3;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	return pRoot;
}

I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildBoxMesh( REAL32 size, bool bTri, bool bLine, COLORREAL * pTriColor, COLORREAL * pLineColor)
{
	i3Node * pRoot;
	i3AttrSet * pTriRoot;
	i3AttrSet * pLineRoot;
	i3VertexArray * pVA;

	pRoot = i3Node::new_object();

	{
		VEC3D vmin, vmax;

		i3Vector::Set( &vmax, size * 0.5f, size * 0.5f, size * 0.5f);
		i3Vector::Scale( &vmin, &vmax, -1.0f);

		pVA = i3VertexArray::CreateBox( &vmin, &vmax, false, true, nullptr);
	}

	if( bTri)
	{
		i3Geometry * pGeo;
		i3GeometryAttr * pGeoAttr;

		pTriRoot = i3AttrSet::new_object_ref();
		pRoot->AddChild( pTriRoot);

		if( pTriColor != nullptr)
		{
			// Color Attribute
			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
				
				pAttr->Set( pTriColor);
				pTriRoot->AddAttr( pAttr);
			}

			// Material Attribute
			{
				i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

				pAttr->SetDiffuse( pTriColor);
				pTriRoot->AddAttr( pAttr);
			}
		}

		pGeo = i3Geometry::new_object_ref();
		pTriRoot->AddChild( pGeo);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pVA->GetCount() / 3);		
	}

	if( bLine)
	{
		i3IndexArray * pIA;
		i3Geometry * pGeo;
		i3GeometryAttr * pGeoAttr;

		pLineRoot = i3AttrSet::new_object_ref();
		pRoot->AddChild( pLineRoot);

		// Lighting Disable
		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineRoot->AddAttr( pAttr);
		}

		if( pLineColor != nullptr)
		{
			// Color Attribute
			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

				pAttr->Set( pLineColor);
				pLineRoot->AddAttr( pAttr);
			}
		}

		pGeo = i3Geometry::new_object_ref();
		pLineRoot->AddChild( pGeo);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		// Index Array
		pIA = i3IndexArray::new_object_ref();

		pIA->Create( 24, 0, pVA->GetCount());
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		// 위쪽 사각형
		pIA->SetIndex(	0,	17);		// x1, y1, z1 -> x2, y1, z1
		pIA->SetIndex(	1,	11);
		pIA->SetIndex(	2,	11);		// x2, y1, z1 -> x2, y1, z2
		pIA->SetIndex(	3,	10);
		pIA->SetIndex(	4,	10);		// x2, y1, z2 -> x1, y1, z2
		pIA->SetIndex(	5,	4);
		pIA->SetIndex(	6,	4);			// x1, y1, z2 -> x1, y1, z1
		pIA->SetIndex(	7,	17);

		// 아래족 사각형
		pIA->SetIndex(	8,	14);		// x1, y2, z1 -> x2, y2, z1
		pIA->SetIndex(	9,	8);
		pIA->SetIndex(	10,	8);			// x2, y2, z1 -> x2, y2, z2
		pIA->SetIndex(	11,	6);
		pIA->SetIndex(	12,	6);			// x2, y2, z2 -> x1, y2, z2
		pIA->SetIndex(	13,	0);
		pIA->SetIndex(	14,	0);			// x1, y2, z2 -> x1, y2, z1
		pIA->SetIndex(	15,	14);

		// 세로 선 (4개)
		pIA->SetIndex(	16,	17);		// x1, y1, z1 -> x1, y2, z1
		pIA->SetIndex(	17,	14);
		pIA->SetIndex(	18,	11);		// x2, y1, z1 -> x2, y2, z1
		pIA->SetIndex(	19,	8);
		pIA->SetIndex(	20,	7);			// x2, y1, z2 -> x2, y2, z2
		pIA->SetIndex(	21,	6);
		pIA->SetIndex(	22,	1);			// x1, y1, z2 -> x1, y2, z2
		pIA->SetIndex(	23,	0);

		pIA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 12, 0);
		pGeoAttr->SetIndexArray( pIA);
	}

	return pRoot;
}

I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildCapsuleMesh( REAL32 fRadius, REAL32 fLength, bool bTri, bool bLine, UINT32 hdiv, UINT32 vdiv, COLORREAL * pLineColor, COLORREAL * pAmbient, COLORREAL * pDiffuse)
{
	//	note. 0을 중심으로 Length가 Y축 기준인 캡슐을 생성한다.

	UINT32 i, j, idx;
	REAL32 x, y, z, ystep, scale, sy;
	REAL32 deg, step;
	i3VertexFormat format;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;
	i3Node * pRoot;
	i3AttrSet * pLineAttrSet = nullptr;
	i3AttrSet * pTriAttrSet = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	UINT32 VertexCount, VertexPerLineCount, TotalTriCount, TotalLineCount, TriPerLine, BodyLineCount;
	VEC3D vec, norm;

	REAL32 fHalfLength, fBasedRate;

	//	현재 Radius를 기준인 1.0인 지름을 비율로 한 Length를 구한다.	
	fBasedRate = 0.5f / fRadius;
	fLength = fLength * fBasedRate;
	fHalfLength = fLength * 0.5f;
	fRadius = 0.5f;		//	지름 1.0을 기준
	
	I3ASSERT( hdiv >= 3);
	I3ASSERT( vdiv >= 1);

	pRoot = i3Node::new_object();

	if( bTri)
	{
		pTriAttrSet = i3AttrSet::new_object_ref();

		if( pDiffuse != nullptr)
		{	//	현재 Shader 때문에 컬러를 바꿔도 소용없다. - komet
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			
			if( pDiffuse != nullptr)
				pAttr->SetDiffuse( pDiffuse);

			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet);
	}

	if( bLine)
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr)
		{	//	현재 Shader 때문에 컬러를 바꿔도 소용없다. - komet
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			pAttr->Set( pLineColor);

			pLineAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pLineAttrSet);
	}

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

	scale = 1.0f;

	// Vertex Array
	{
		format.SetHasPosition( true);
		format.SetHasNormal( true);
		format.SetHasColor( false);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, VertexCount, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		// 최상/하단 Vertex 2개는 항상 선두에 추가한다. (원통에 반지름도 포함시킨 높이에 주의)
		i3Vector::Set( &vec, 0.0f, fHalfLength + fRadius, 0.0f);
		pVA->SetPosition( 0, &vec);
		pVA->SetNormal( 0, &vec);

		i3Vector::Set( &vec, 0.0f, -(fHalfLength + fRadius), 0.0f);
		pVA->SetPosition( 1, &vec);
		pVA->SetNormal( 1, &vec);

		idx = 2;
		y = ystep;

		for( i = 0; i < BodyLineCount; i++)
		{
			deg = 0.0f;

			if( i >= BodyLineCount / 2)
				//	원통만 늘인다.
				sy = (i3Math::vcos( y * 2.0f) * fRadius) + fHalfLength - fLength;
			else
				sy = (i3Math::vcos( y * 2.0f) * fRadius) + fHalfLength;

			if( i != BodyLineCount / 2)		//	원통이 아닌 구일때만 계산
				scale = i3Math::vsin( y * 2.0f) * fRadius;

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

	// Index Array for Triangle
	if( bTri)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3;
		bool bBody;

		count = ((hdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((hdiv * 2) * 3);

		pIA = i3IndexArray::new_object_ref();
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
				bBody = false;

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
					bBody = true;
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

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	// Index Array for Line
	if( bLine)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3, i4;

		count = TotalTriCount * 2 * 2;

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		for( j = 0; j <= (UINT32) vdiv; j++)
		{
			for( i = 0; i < (UINT32) hdiv; i++)
			{
				if( j == 0)
					i1 = 0;
				else if( j == vdiv)
					i1 = 1;
				else
					i1 = (preidx + i);

				i2 = sidx + i;
				i3 = sidx + i;
				i4 = sidx + i + 1;

				if( i4 >= (sidx + hdiv))
					i4 = sidx;

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;
				pIA->SetIndex( idx,	i4);	idx++;
			}

			preidx = sidx;

			sidx += hdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - hdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, count >> 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}

	return pRoot;
}

I3_EXPORT_SGUT
i3Node *	i3SceneUtil::BuildHemiSphereMesh(REAL32 fRadius, bool bTri, bool bLine, bool isUpper, UINT32 horizdiv, UINT32 vertdiv, COLORREAL * pLineColor, COLORREAL * pAmbient, COLORREAL * pDiffuse)
{
	I3ASSERT( horizdiv >= 3);
	I3ASSERT( vertdiv >= 1);

	i3Geometry * pGeo;
	i3GeometryAttr * pGeoAttr;
	i3VertexFormat format;
	i3VertexArray  * pVA;
	i3IndexArray * pIA;
	i3AttrSet * pLineAttrSet = nullptr;
	i3AttrSet * pTriAttrSet = nullptr;
	i3Node * pRoot;
	UINT32 VertexCount, VertexPerLineCount, TotalTriCount, TotalLineCount, TriPerLine, BodyLineCount;
	REAL32 x, y, z, ystep, scale, sy;
	REAL32 deg, step;
	UINT32 idx;
	VEC3D vec, norm;
	

	pRoot = i3Node::new_object();
	fRadius = 0.5f;		//	지름 1.0을 기준

	if(bTri)
	{
		pTriAttrSet = i3AttrSet::new_object_ref();

		if( pDiffuse != nullptr)
		{	//	현재 Shader 때문에 컬러를 바꿔도 소용없다. - komet
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

			if( pDiffuse != nullptr)
				pAttr->SetDiffuse( pDiffuse);

			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild(pTriAttrSet);
	}

	if(bLine)
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr)
		{	//	현재 Shader 때문에 컬러를 바꿔도 소용없다. - komet
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			pAttr->Set( pLineColor);

			pLineAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pLineAttrSet);
	}

	TotalLineCount = (vertdiv + 2);
	BodyLineCount = vertdiv;
	VertexPerLineCount = horizdiv;

	VertexCount = (VertexPerLineCount * BodyLineCount) + 2;
	TriPerLine = (VertexPerLineCount * 2) * BodyLineCount;
	TotalTriCount = TriPerLine + (horizdiv * 2);

	// 높이 스텝
	ystep = 0.5f / vertdiv;

	// 회전값 스텝
	step = 360.0f / horizdiv;

	scale = 1.0f;


	{
		format.SetHasPosition( true);
		format.SetHasNormal( true);
		format.SetHasColor( false);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, VertexCount, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();
		
		if(isUpper)
		{
			i3Vector::Set( &vec, 0.0f, fRadius * 2.0f, 0.0f);
			pVA->SetPosition( 0, &vec);
			pVA->SetNormal( 0, &vec);

			i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);
			pVA->SetPosition( 1, &vec);
			pVA->SetNormal( 1, &vec);
		}
		else
		{
			i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);
			pVA->SetPosition( 0, &vec);
			pVA->SetNormal( 0, &vec);

			i3Vector::Set( &vec, 0.0f, fRadius * -2.0f, 0.0f);
			pVA->SetPosition( 1, &vec);
			pVA->SetNormal( 1, &vec);
		}
		
		idx = 2;
		y = ystep * 1.25f;

		for(UINT32 i = 0; i < BodyLineCount; i++)
		{
			deg = 0.0f;

			sy = i3Math::vcos( y * 2.0f);
			scale = i3Math::vsin( y * 2.0f);

			for(UINT32 j = 0; j < horizdiv; j++)
			{
				x = i3Math::sin(I3_DEG2RAD(deg));
				z = i3Math::cos(I3_DEG2RAD(deg));

				if(isUpper)
				{
					i3Vector::Set(&vec, x * scale, sy, z * scale);
					i3Vector::Normalize(&norm, &vec);
				}
				else
				{
					i3Vector::Set(&vec, x * scale, -sy, z * scale);
					i3Vector::Normalize(&norm, &vec);
				}
				
				pVA->SetPosition(idx, &vec);
				pVA->SetNormal(idx, &norm);

				idx++;
				deg += step;
			}

			y += ystep;
		}
	

		pVA->Unlock();
	}

	if(bTri)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3;
		bool bBody;

		count = ((horizdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((horizdiv * 2) * 3);

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		// Caps
		for( UINT32 j = 0; j <= (UINT32) vertdiv; j++)
		{
			for( UINT32 i = 0; i < (UINT32) horizdiv; i++)
			{
				bBody = false;

				if( j == 0)
				{
					i1 = 0;
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + horizdiv))
						i3 = sidx;
				}
				else if( j == vertdiv)
				{
					i1 = preidx + i + 1;
					i2 = preidx + i;
					i3 = 1;

					if( i1 >= (preidx + horizdiv))
						i1 = preidx;
				}
				else
				{
					i1 = (preidx + i);
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + horizdiv))
						i3 = sidx;
					bBody = true;
				}


				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;

				if( bBody)
				{
					i2 = i3;
					i3 = preidx + i + 1;

					if( i3 >= (preidx + horizdiv))
						i3 = preidx;

					pIA->SetIndex( idx,	i1);	idx++;
					pIA->SetIndex( idx,	i2);	idx++;
					pIA->SetIndex( idx,	i3);	idx++;
				}
			}

			preidx = sidx;

			sidx += horizdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - horizdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	// Index Array for Line
	if( bLine)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3, i4;

		count = TotalTriCount * 2 * 2;

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		for( UINT32 j = 0; j <= (UINT32) vertdiv; j++)
		{
			for( UINT32 i = 0; i < (UINT32) horizdiv; i++)
			{
				if(isUpper)
				{
					if( j == 0)
						i1 = 0;
					else if( j == vertdiv)
						i1 = 1;
					else
						i1 = (preidx + i);
				}
				else
				{
					if( j == vertdiv)
						i1 = 0;
					else if( j == 0)
						i1 = 1;
					else
						i1 = (preidx + i);
				}
				

				i2 = sidx + i;
				i3 = sidx + i;
				i4 = sidx + i + 1;

				if( i4 >= (sidx + horizdiv))
					i4 = sidx;

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;
				pIA->SetIndex( idx,	i4);	idx++;
			}

			preidx = sidx;

			sidx += horizdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - horizdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, count >> 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}


	return pRoot;
}

I3_EXPORT_SGUT
i3Node *	i3SceneUtil::BuildCylinderMesh(REAL32 fRadius, REAL32 fLength, bool bTri, bool bLine, UINT32 horizdiv /* = 20 */, UINT32 vertdiv /* = 10 */, COLORREAL * pLineColor /* = nullptr */, COLORREAL * pAmbient /* = nullptr */, COLORREAL * pDiffuse /* = nullptr */)
{
	I3ASSERT( horizdiv >= 3);
	I3ASSERT( vertdiv >= 1);

	i3Geometry * pGeo;
	i3GeometryAttr * pGeoAttr;
	i3VertexFormat format;
	i3VertexArray  * pVA;
	i3IndexArray * pIA;
	i3AttrSet * pLineAttrSet = nullptr;
	i3AttrSet * pTriAttrSet = nullptr;
	UINT32 VertexCount, VertexPerLineCount, TotalTriCount, TotalLineCount, TriPerLine, BodyLineCount;
	i3Node * pRoot;
	REAL32 x, y, z, ystep, sy;
	REAL32 deg, step;
	UINT32 idx;
	VEC3D vec, norm;
	REAL32 scale;

	pRoot = i3Node::new_object();
	fRadius = 0.5f;		//	지름 1.0을 기준
	scale = 1.0f;

	if(bTri)
	{
		pTriAttrSet = i3AttrSet::new_object_ref();

		if( pDiffuse != nullptr)
		{	//	현재 Shader 때문에 컬러를 바꿔도 소용없다. - komet
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

			if( pDiffuse != nullptr)
				pAttr->SetDiffuse( pDiffuse);

			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild(pTriAttrSet);
	}

	if(bLine)
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr)
		{	//	현재 Shader 때문에 컬러를 바꿔도 소용없다. - komet
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			pAttr->Set( pLineColor);

			pLineAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pLineAttrSet);
	}

	TotalLineCount = vertdiv + 2;
	BodyLineCount = vertdiv;
	VertexPerLineCount = horizdiv;

	VertexCount = (VertexPerLineCount * BodyLineCount) + 2;
	TriPerLine = (VertexPerLineCount * 2) * BodyLineCount;
	TotalTriCount = TriPerLine + (horizdiv * 2);	// 아래와 위에 Fan 구조로 덮개가 붙는다.

	// 높이 스텝
	ystep = 1.0f / (vertdiv);

	// 회전값 스텝
	step = 360.0f / horizdiv;

	{
		format.SetHasPosition(true);
		format.SetHasNormal(true);
		format.SetHasColor(false);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create(&format, VertexCount, 0);
		pVA->setRestoreType(I3G_RESTORE_MEM);

		pVA->Lock();

		// 최상/하단 Vertex 2개는 항상 선두에 추가한다
		i3Vector::Set( &vec, 0.0f, fLength, 0.0f);
		pVA->SetPosition( 0, &vec);
		pVA->SetNormal( 0, &vec);
		
		i3Vector::Set( &vec, 0.0f, -fLength, 0.0f);
		pVA->SetPosition( 1, &vec);
		pVA->SetNormal( 1, &vec);

		idx = 2;
		y = 0.0f;

		for(UINT32 i = 0; i < BodyLineCount; i++)
		{
			deg = 0.0f;

			if(i == BodyLineCount - 1)
			{
				sy = i3Math::vcos(2.0f);	
			}
			else
			{
				sy = i3Math::vcos(y * 2.0f);
			}

			for(UINT32 j = 0; j < horizdiv; j++)
			{
				x = i3Math::sin(I3_DEG2RAD(deg));
				z = i3Math::cos(I3_DEG2RAD(deg));

				i3Vector::Set(&vec, x, sy, z);
				i3Vector::Normalize(&norm, &vec);

				pVA->SetPosition(idx, &vec);
				pVA->SetNormal(idx, &norm);

				idx++;
				deg += step;
			}

			y += ystep;
		}

		pVA->Unlock();
	}

	if(bTri)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3;
		bool bBody;

		count = ((horizdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((horizdiv * 2) * 3);

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		// Caps
		for( UINT32 j = 0; j <= (UINT32) vertdiv; j++)
		{
			for( UINT32 i = 0; i < (UINT32) horizdiv; i++)
			{
				bBody = false;

				if( j == 0)
				{
					i1 = 0;
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + horizdiv))
						i3 = sidx;
				}
				else if( j == vertdiv)
				{
					i1 = preidx + i + 1;
					i2 = preidx + i;
					i3 = 1;

					if( i1 >= (preidx + horizdiv))
						i1 = preidx;
				}
				else
				{
					i1 = (preidx + i);
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + horizdiv))
						i3 = sidx;
					bBody = true;
				}

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;

				if( bBody)
				{
					i2 = i3;
					i3 = preidx + i + 1;

					if( i3 >= (preidx + horizdiv))
						i3 = preidx;

					pIA->SetIndex( idx,	i1);	idx++;
					pIA->SetIndex( idx,	i2);	idx++;
					pIA->SetIndex( idx,	i3);	idx++;
				}
			}

			preidx = sidx;

			sidx += horizdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - horizdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	// Index Array for Line
	if( bLine)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3, i4;

		count = TotalTriCount * 2 * 2;

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		for( UINT32 j = 0; j <= (UINT32) vertdiv; j++)
		{
			for( UINT32 i = 0; i < (UINT32) horizdiv; i++)
			{
				if( j == 0)
					i1 = 0;
				else if( j == vertdiv)
					i1 = 1;
				else
					i1 = (preidx + i);

				i2 = sidx + i;
				i3 = sidx + i;
				i4 = sidx + i + 1;

				if( i4 >= (sidx + horizdiv))
					i4 = sidx;

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;
				pIA->SetIndex( idx,	i4);	idx++;
			}

			preidx = sidx;

			sidx += horizdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - horizdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, count >> 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}

	return pRoot;
}

I3_EXPORT_SGUT
i3Node *	i3SceneUtil::BuildBoneMesh( REAL32 Length, REAL32 Size, bool bLine)
{
	i3VertexArray * pVA;
	//i3Geometry * pGeo;
	//i3GeometryAttr * pGeoAttr;

	// 원점에서 X 방향으로 Length만큼 떨어진 점을 잊는 Bone Mesh를 생성한다.

	// Vertices
	{
		i3VertexFormat fmt;

		fmt.SetHasPosition(true);

		if( bLine == false)
			fmt.SetHasNormal(true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &fmt, 24, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);
		pVA->Lock();
		pVA->Unlock();
	}

	return nullptr;
}

#define LEAD		(1.0f / 3.0f)

static REAL32 s_SelBoxVertex[32][3] =
{
	{	-0.5f,			-0.5f,			-0.5f},
	{	-0.5f + LEAD,	-0.5f,			-0.5f},
	{	-0.5f,			-0.5f + LEAD,	-0.5f},
	{	-0.5f,			-0.5f,			-0.5f + LEAD},

	{	-0.5f,			0.5f,			-0.5f},
	{	-0.5f + LEAD,	0.5f,			-0.5f},
	{	-0.5f,			0.5f - LEAD,	-0.5f},
	{	-0.5f,			0.5f,			-0.5f + LEAD},

	{	-0.5f,			-0.5f,			0.5f},
	{	-0.5f + LEAD,	-0.5f,			0.5f},
	{	-0.5f,			-0.5f + LEAD,	0.5f},
	{	-0.5f,			-0.5f,			0.5f - LEAD},

	{	-0.5f,			0.5f,			0.5f},
	{	-0.5f + LEAD,	0.5f,			0.5f},
	{	-0.5f,			0.5f - LEAD,	0.5f},
	{	-0.5f,			0.5f,			0.5f - LEAD},

	{	0.5f,			-0.5f,			-0.5f},
	{	0.5f - LEAD,	-0.5f,			-0.5f},
	{	0.5f,			-0.5f + LEAD,	-0.5f},
	{	0.5f,			-0.5f,			-0.5f + LEAD},

	{	0.5f,			0.5f,			-0.5f},
	{	0.5f - LEAD,	0.5f,			-0.5f},
	{	0.5f,			0.5f - LEAD,	-0.5f},
	{	0.5f,			0.5f,			-0.5f + LEAD},

	{	0.5f,			-0.5f,			0.5f},
	{	0.5f - LEAD,	-0.5f,			0.5f},
	{	0.5f,			-0.5f + LEAD,	0.5f},
	{	0.5f,			-0.5f,			0.5f - LEAD},

	{	0.5f,			0.5f,			0.5f},
	{	0.5f - LEAD,	0.5f,			0.5f},
	{	0.5f,			0.5f - LEAD,	0.5f},
	{	0.5f,			0.5f,			0.5f - LEAD}
};

I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildSelectBoxMesh( COLOR * pColor)
{
	INT32 i;
	i3GeometryAttr * pGeoAttr;
	i3AttrSet * pRoot;

	{
		pRoot = i3AttrSet::new_object();
		pRoot->SetPriority( 0);

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

			pAttr->Set(true);
			pRoot->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			pRoot->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			pAttr->Set( pColor);
			pRoot->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set(false);
			pRoot->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

			pAttr->Set(false);
			pRoot->AddAttr( pAttr);
		}
	}

	// Create line box
	{
		// Vertex Array
		{
			VEC3D vec;
			i3VertexArray * pVA;
			i3VertexFormat format;
			
			format.SetHasPosition( true);

			pVA = i3VertexArray::new_object_ref();

			pVA->Create( &format, 32, 0);
			pVA->setRestoreType( I3G_RESTORE_MEM);

			pVA->Lock();

			for( i = 0; i < 32; i++)
			{
				setXYZ( &vec, s_SelBoxVertex[i][0], s_SelBoxVertex[i][1], s_SelBoxVertex[i][2]);
				pVA->SetPosition( i, &vec);
			}

			pVA->Unlock();

			// Primitive
			pGeoAttr = i3GeometryAttr::new_object_ref();
			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 24);
		}

		// Index Array
		{
			i3IndexArray * pIA;
			INT32 base, i, count;

			pIA = i3IndexArray::new_object_ref();

			pIA->Create( 48, 0, pGeoAttr->GetVertexArray()->GetCount());
			pIA->setRestoreType( I3G_RESTORE_MEM);

			pIA->Lock();

			count = 1;
			base = 0;

			for( i = 0; i < 48; i++)
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

				pIA->SetIndex( i, idx);
			}

			pIA->Unlock();

			pGeoAttr->SetIndexArray( pIA);
		}
	}

	{
		i3Geometry * pGeo;

		pGeo = i3Geometry::new_object_ref();

		pGeo->AppendGeometryAttr( pGeoAttr);

		pRoot->AddChild( pGeo);
	}

	return pRoot;
}

I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildArc( REAL32 fRadius, REAL32 fAngle, INT32 divcount, bool bTri, bool bLine, COLORREAL * pLineColor, COLORREAL * pAmbient, COLORREAL * pDiffuse, MATRIX * pMtx )
{
	INT32 i, idx;
	REAL32 x, z;
	REAL32 deg, step;
	i3VertexFormat format;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;
	i3Node * pRoot;
	i3AttrSet * pLineAttrSet = nullptr;
	i3AttrSet * pTriAttrSet = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	UINT32 VertexCount;
	VEC3D	vec;

	I3ASSERT( divcount >= 1 );

	pRoot = i3Node::new_object();

	if( bTri )
	{
		pTriAttrSet = i3AttrSet::new_object_ref();

		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
			pAttr->Set( I3G_FACE_CULL_NONE );
			pTriAttrSet->AddAttr( pAttr );
		}

		if( pDiffuse != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr && pDiffuse != nullptr )
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			
			pAttr->SetDiffuse( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet);
	}

	if( bLine )
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr )
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pLineColor );
			pLineAttrSet->AddAttr( pAttr );
		}

		pRoot->AddChild( pLineAttrSet );
	}

	VertexCount = divcount + 2;

	// 회전값 스텝
	step = fAngle / divcount;

	// Vertex Array
	{
		format.SetHasPosition( true);
		format.SetHasNormal( false);
		format.SetHasColor( false);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, VertexCount, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		deg = 0.f;

		// Vertex 2개는 항상 선두에 추가한다.
		i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);

		if( pMtx != nullptr)
			i3Vector::TransformCoord( &vec, &vec, pMtx);

		pVA->SetPosition( 0, &vec);

		x = i3Math::sin( I3_DEG2RAD( deg ) );
		z = i3Math::cos( I3_DEG2RAD( deg ) );

		i3Vector::Set( &vec, x * fRadius, 0.0f, z * fRadius);

		if( pMtx != nullptr)
			i3Vector::TransformCoord( &vec, &vec, pMtx);

		pVA->SetPosition( 1, &vec );

		idx = 2;
		
		for( i = 0; i < divcount; i++)
		{	
			deg += step;

			x = i3Math::sin( I3_DEG2RAD( deg));
			z = i3Math::cos( I3_DEG2RAD( deg));

			i3Vector::Set( &vec, x * fRadius, 0.0f, z * fRadius);

			if( pMtx != nullptr)
				i3Vector::TransformCoord( &vec, &vec, pMtx);

			pVA->SetPosition( idx, &vec);

			idx++;
		}

		pVA->Unlock();
	}

	// Index Array for Triangle
	if( bTri)
	{
		INT32 count;
		UINT32 i1, i2, i3;

		count = (divcount * 3);			//  Caps

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;

		//
		for( i = 0; i < divcount; i++)
		{
			i1 = 0;
			i2 = i + 1;
			i3 = i + 2;

			pIA->SetIndex( idx,	i1);	idx++;
			pIA->SetIndex( idx,	i2);	idx++;
			pIA->SetIndex( idx,	i3);	idx++;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	// Index Array for Line
	if( bLine)
	{
		INT32 count;
		UINT32 i1, i2;

		count = (divcount) * 2;

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		//
		for( i = 0; i < divcount; i++)
		{
			i1 = i+1;
			i2 = i+2;
			pIA->SetIndex( idx,	i1);	idx++;
			pIA->SetIndex( idx,	i2);	idx++;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, count >> 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}

	return pRoot;
}


I3_EXPORT_SGUT
i3Node * i3SceneUtil::BuildTriangle( VEC3D * pV1, VEC3D * pV2, VEC3D * pV3, bool bTri, bool bLine, COLORREAL * pLineColor, COLORREAL * pAmbient, COLORREAL * pDiffuse )
{
	i3Node *		pRoot;
	i3AttrSet *		pTriAttrSet	= nullptr;
	i3AttrSet *		pLineAttrSet = nullptr;
	i3VertexFormat	format;
	i3VertexArray * pVA;
	i3IndexArray *	pIA;
	i3GeometryAttr * pGeoAttr;
	i3Geometry *	pGeo;

	pRoot = i3Node::new_object();

	// Triangle
	if( bTri )
	{
		pTriAttrSet = i3AttrSet::new_object_ref();
		// Cull mode off
		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
			pAttr->Set( I3G_FACE_CULL_NONE );
			pTriAttrSet->AddAttr( pAttr );
		}

		if( pDiffuse != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr && pDiffuse != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			//pAttr->SetAmbient( pAmbient);
			pAttr->SetDiffuse( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet );
	}

	// Line
	if( bLine )
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr )
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pLineColor );
			pLineAttrSet->AddAttr( pAttr );
		}

		pRoot->AddChild( pLineAttrSet );
	}

	// Vertex Array
	{
		format.SetHasPosition( true);
		format.SetHasNormal( false);
		format.SetHasColor( false);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, 3, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();
		
		pVA->SetPosition( 0, pV1 );
		pVA->SetPosition( 1, pV2 );
		pVA->SetPosition( 2, pV3 );
		
		pVA->Unlock();
	}

	// Index Array for Triangle
	if( bTri)
	{
		pIA = i3IndexArray::new_object_ref();
		pIA->Create( 3, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();
		pIA->SetIndex( 0,	0);
		pIA->SetIndex( 1,	1);
		pIA->SetIndex( 2,	2);
		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	if( bLine )
	{
		pIA = i3IndexArray::new_object_ref();
		pIA->Create( 6, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		pIA->SetIndex( 0,	0);
		pIA->SetIndex( 1,	1);
		pIA->SetIndex( 2,	1);
		pIA->SetIndex( 3,	2);
		pIA->SetIndex( 4, 2);
		pIA->SetIndex( 5, 0);

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 3 );
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}
	
	return pRoot;
}

I3_EXPORT_SGUT 
i3Node *	i3SceneUtil::RemoveNodeByFlag( i3Node * pNode, UINT32 mask)
{
	INT32 i;
	i3Node * pChild;

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = (i3Node *) pNode->GetChild( i);

		if( pChild->IsFlag( mask))
		{
			pNode->RemoveChild( pChild);
			i--;
		}
		else
		{
			RemoveNodeByFlag( pChild, mask);
		}
	}

	return pNode;
}
