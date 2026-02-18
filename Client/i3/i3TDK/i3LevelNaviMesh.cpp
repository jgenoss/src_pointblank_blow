#include "stdafx.h"
#include "i3LevelNaviMesh.h"

#include "i3Level/i3LevelViewport.h"
#include "i3Level/i3LevelFramework.h"

//I3_CLASS_INSTANCE( i3LevelNaviMesh_GeometryAttr, i3GeometryAttr );
I3_CLASS_INSTANCE( i3LevelNaviMesh_GeometryAttr);

i3LevelNaviMesh_GeometryAttr::i3LevelNaviMesh_GeometryAttr() 
{
	i3Vector::Set( &m_pos, 0,0,0 );	
	i3Color::Set( &m_cr, 1.0f, 1.0f, 1.0f, 1.0f );
	m_scale = 1.0f;
}

i3LevelNaviMesh_GeometryAttr::~i3LevelNaviMesh_GeometryAttr()
{
}

void	i3LevelNaviMesh_GeometryAttr::SetPos( RT_VEC3D* v )
{	
	i3Vector::Copy( &m_pos, v );
}

void	i3LevelNaviMesh_GeometryAttr::SetScale( REAL32 scale )
{
	m_scale = scale;
}

void	i3LevelNaviMesh_GeometryAttr::SetColor( COLORREAL* cr )
{
	m_cr = *cr;
}

bool	i3LevelNaviMesh_GeometryAttr::IsPick( VEC3D* p1, VEC3D* p2, REAL32* t )
{
	VEC3D vIntersect;
	i3Vector::Set( &vIntersect, 0,0,0 );

	return i3Math::IntersectLineToSphere( p1, p2, &m_pos, m_scale * m_scale, &vIntersect, t ) ? true : false;
}

void	i3LevelNaviMesh_GeometryAttr::_CalculateWorldMatrix( RT_MATRIX& mat )
{
	i3Matrix::Identity( &mat );

	mat.m00 *= m_scale;
	mat.m11 *= m_scale;
	mat.m22 *= m_scale;

	mat.m30 = m_pos.x;
	mat.m31 = m_pos.y;
	mat.m32 = m_pos.z;
}

void	i3LevelNaviMesh_GeometryAttr::Apply( i3RenderContext * pContext )
{
	if( NULL == pContext )
	{
		return;
	}

	RT_MATRIX mvp;
	i3Matrix::Copy( &mvp, pContext->GetViewMatrix() );
	i3Matrix::Mul( &mvp, &mvp, pContext->GetProjectMatrix() );
	

	i3Frustum frustum;
	frustum.Set( &mvp );	

	if( false == frustum.TestSphere( &m_pos, m_scale ) )
	{
		return;
	}

	RT_MATRIX matOld, matWorld;
	COLORREAL crOld, crDiffuseOld;

	_CalculateWorldMatrix( matWorld );
	i3Matrix::Copy( &matOld, pContext->GetWorldMatrix() ) ;

	crOld = *pContext->GetColor();
	crDiffuseOld = *pContext->getMaterialDiffuse();

	pContext->SetColor( &m_cr );
	*pContext->getMaterialDiffuse() = m_cr;

	pContext->SetWorldMatrix( &matWorld );

	i3GeometryAttr::Apply( pContext );

	pContext->SetWorldMatrix( &matOld );
	pContext->SetColor( &crOld );
	*pContext->getMaterialDiffuse() = crDiffuseOld;
}


//I3_CLASS_INSTANCE( i3LevelNaviMesh, i3Node );
I3_CLASS_INSTANCE( i3LevelNaviMesh);

i3LevelNaviMesh::i3LevelNaviMesh() : m_pGeomPoint( NULL )
{
	m_pVASphere = NULL;
	m_pIASphere = NULL;

	m_pVASelBox = NULL;
	m_pIASelBox	= NULL;
}

i3LevelNaviMesh::~i3LevelNaviMesh()
{
	_CleanUp();
}

void	i3LevelNaviMesh::AddPoint( RT_VEC3D* pos )
{
	i3LevelNaviMesh_GeometryAttr* pGeom = i3LevelNaviMesh_GeometryAttr::new_object();

	COLORREAL cr;
	i3Color::Set( &cr, 1.0f, 0.1f, 0.1f, 1.0f );

	pGeom->SetPos( pos );
	pGeom->SetScale( 0.2f );
	pGeom->SetColor( &cr );

	if( m_pVASphere && m_pIASphere )
	{
		pGeom->SetVertexArray( m_pVASphere, I3G_PRIM_TRILIST, m_pIASphere->GetCount() /3 );
		pGeom->SetIndexArray( m_pIASphere );
	}

	m_PointList.push_back( pGeom );
	
	if( NULL != m_pGeomPoint )
		m_pGeomPoint->AppendGeometryAttr( pGeom );
}

void	i3LevelNaviMesh::MoveSelectedPoint( RT_VEC3D* pMove )
{
	for each( map_selpoint::value_type p in m_SelPointList )
	{
		p.second.geom->Move( pMove );
		p.second.sel->Move( pMove );
	}
}

void	i3LevelNaviMesh::RemoveSelectedPoint()
{
	i3::vector< int > selList;
	GetSelectPointList( selList );

	for each( int idx in selList )
	{
		RemovePoint( idx );
	}
}

void	i3LevelNaviMesh::RemovePoint( i3LevelNaviMesh_GeometryAttr* pDel )
{
	int i = 0;
	for each( i3LevelNaviMesh_GeometryAttr* ptr in m_PointList )
	{
		if( pDel == ptr )
		{
			RemovePoint( i );
			break;
		}

		++i;
	}
}

void	i3LevelNaviMesh::RemovePoint( int idx )
{
	if( idx < 0 || static_cast<int>(m_PointList.size()) <= idx )
	{
		return;
	}

	i3LevelNaviMesh_GeometryAttr* ptr = m_PointList.at( idx );

	_Remove_SelectPoint( ptr );

	if( m_pGeomPoint )
		m_pGeomPoint->RemoveGeometryAttr( ptr );

	m_PointList.erase( m_PointList.begin() + idx );
	ptr->Release();
}

void	i3LevelNaviMesh::GetSelectPointList( i3::vector< int >& selList )
{
	selList.clear();

	int i = 0;
	for each( i3LevelNaviMesh_GeometryAttr* ptr in m_PointList )
	{
		if( m_SelPointList.end() != m_SelPointList.find( ptr ) )
		{
			selList.push_back( i );
		}

		++i;
	}
}

bool	i3LevelNaviMesh::Create()
{
	_CleanUp();

	{
		i3AttrSet* pAttrSet = i3AttrSet::new_object_ref();

		{
			i3ZTestEnableAttr* pAttr = i3ZTestEnableAttr::new_object_ref();
			pAttr->Set( TRUE );
			pAttrSet->AddAttr( pAttr );
		}


		{
			i3LightingEnableAttr* pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( FALSE );
			pAttrSet->AddAttr( pAttr);
		}

		m_pGeomPoint = i3Geometry::new_object();
		pAttrSet->AddChild( m_pGeomPoint );

		m_pGeomSelBox = i3Geometry::new_object();
		pAttrSet->AddChild( m_pGeomSelBox );

		AddChild( pAttrSet );
	}	
	
	_Build_SphereMesh( 20, 10 );
	_Build_SelectBoxMesh();
	return true;
}

void	i3LevelNaviMesh::SelectPoint( POINT pt, SEL_POINT_MODE selMode )
{
	if( SEL_POINT_EXCLUSIVE == selMode )
	{
		UnSelectPointAll();
	}

	i3CollideeLine line;	

	// calcualte collidee line
	{
		i3LevelFramework * pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

		VEC3D pos1, pos2;

		MATRIX tmx;
		REAL32 dx, dy;	
		INT32 w, h;

		w = pFramework->getViewport()->GetViewWidth();
		h = pFramework->getViewport()->GetViewHeight();

		pFramework->GetViewProjectionMatrix( &tmx );
		i3Matrix::Inverse( &tmx, NULL, &tmx);

		dx = ((REAL32) pt.x / w * 2.0f) - 1.0f;
		dy = ((REAL32) pt.y / h * 2.0f) - 1.0f;

		i3Vector::Set( &pos1, dx, -dy, 0.0f);
		i3Vector::Set( &pos2, dx, -dy, 1.0f);

		i3Vector::TransformCoord( &pos1, &pos1, &tmx);
		i3Vector::TransformCoord( &pos2, &pos2, &tmx);
		

		line.SetStart( &pos1);
		line.SetEnd( &pos2);	
	}

	bool bPick = false;
	REAL32 tmax = 0, t;

	i3LevelNaviMesh_GeometryAttr* pickGeom = NULL;

	for each( i3LevelNaviMesh_GeometryAttr* ptr in m_PointList )
	{
		if( false == ptr->IsPick( line.GetStart(), line.GetEnd(), &t ) )
		{
			continue;
		}

		if( false == bPick )
		{
			tmax = t;
			pickGeom = ptr;
			bPick = true;
		}
		else
		{
			if( t < tmax )
			{
				tmax = t;
				pickGeom = ptr;
			}
		}
	}

	if( bPick )
	{
		_SelectedPoint( pickGeom, selMode );
	}
}

void	i3LevelNaviMesh::SelectBandPoint( POINT pt1, POINT pt2, SEL_POINT_MODE selMode )
{
	if( SEL_POINT_EXCLUSIVE == selMode )
	{
		UnSelectPointAll();
	}

	VEC3D tmin, tmax;

	INT32 hw, hh;
	MATRIX view_project;
	I3RECT src, dest;

	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView->getFramework();

	pFramework->GetViewProjectionMatrix( &view_project);

	{
		REAL32 rw, rh;
		REAL32 x1, y1, x2, y2;

		// 화면 한가운데를 ( 0, 0)으로 간주한 Clipping-Space로의 좌표 변환용.
		hw = pView->GetViewWidth() >> 1;
		hh = pView->GetViewHeight() >> 1;

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		x1	=	(pt1.x - hw) * rw;
		y1		=	(pt1.y - hh) * rh;
		x2	=	(pt2.x - hw) * rw;
		y2	=	(pt2.y - hh) * rh;

		src.left = MIN( x1, x2);
		src.top = MIN( y1, y2);
		src.right = MAX( x1, x2);
		src.bottom = MAX( y1, y2);
	}

	for each( i3LevelNaviMesh_GeometryAttr* ptr in m_PointList )
	{
		i3Vector::Copy( &tmin, ptr->GetPos() );
		i3Vector::Copy( &tmax, ptr->GetPos() );

		tmin.x -= ptr->GetScale();
		tmin.y -= ptr->GetScale();
		tmin.z -= ptr->GetScale();

		tmax.x += ptr->GetScale();
		tmax.y += ptr->GetScale();
		tmax.z += ptr->GetScale();

		bool bHit;

		{
			VEC3D bb[8];
			i3BoundBox bbox;
			INT32 j;

			bbox.SetMinMax( &tmin, &tmax);

			bbox.GetCornerVertices( bb);

			i3Matrix::TransformPoints( bb, bb, 8, &view_project);

			i3Vector::Copy( &tmin,  &bb[0]);
			i3Vector::Copy( &tmax,  &bb[0]);
			for( j = 1; j < 8; j++)
			{
				i3Vector::Minimize( &tmin, &tmin, &bb[j]);
				i3Vector::Maximize( &tmax, &tmax, &bb[j]);
			}
		}

		dest.left	= MIN( i3Vector::GetX( &tmin), i3Vector::GetX( &tmax));
		dest.top	= MIN( i3Vector::GetY( &tmin), i3Vector::GetY( &tmax));
		dest.right	= MAX( i3Vector::GetX( &tmin), i3Vector::GetX( &tmax));
		dest.bottom	= MAX( i3Vector::GetY( &tmin), i3Vector::GetY( &tmax));

		if( i3Math::RectInRect( &dest, &src))
			bHit = true;
		else if( i3Math::RectInRect( &src, &dest))
			bHit = true;
		else
			bHit = false;

		if( bHit )
		{
			_SelectedPoint( ptr, selMode );
		}
	}
}

void	i3LevelNaviMesh::UnSelectPointAll()
{
	for each( map_selpoint::value_type p in m_SelPointList )
	{
		if( m_pGeomSelBox )
			m_pGeomSelBox->RemoveGeometryAttr( p.second.sel );

		p.second.geom->Release();
		p.second.sel->Release();
	}

	m_SelPointList.clear();
}

void	i3LevelNaviMesh::_CleanUp()
{
	for each( map_selpoint::value_type p in m_SelPointList )
	{
		p.second.geom->Release();
		p.second.sel->Release();
	}

	for each( i3LevelNaviMesh_GeometryAttr* ptr in m_PointList )
	{
		ptr->Release();
	}

	m_SelPointList.clear();
	m_PointList.clear();

	I3_SAFE_RELEASE( m_pVASphere );
	I3_SAFE_RELEASE( m_pIASphere );
	I3_SAFE_RELEASE( m_pVASelBox );
	I3_SAFE_RELEASE( m_pIASelBox );

	I3_SAFE_RELEASE( m_pGeomPoint );
}

void	i3LevelNaviMesh::_Build_SphereMesh( UINT32 hdiv, UINT32 vdiv )
{
	if( NULL != m_pVASphere && NULL != m_pIASphere )
	{
		return;
	}

	m_pVASphere		= i3VertexArray::new_object();
	m_pIASphere		= i3IndexArray::new_object();

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

		m_pVASphere->Create( &format, VertexCount, 0);
		m_pVASphere->setRestoreType( I3G_RESTORE_MEM);

		m_pVASphere->Lock();

		// 최상/하단 Vertex 2개는 항상 선두에 추가한다.
		i3Vector::Set( &vec, __RT(0.0f), __RT(0.5f), __RT_0);
		m_pVASphere->SetPosition( 0, &vec);
		m_pVASphere->SetNormal( 0, &vec);		

		i3Vector::Set( &vec, __RT(0.0f), __RT(-0.5f), __RT_0);
		m_pVASphere->SetPosition( 1, &vec);
		m_pVASphere->SetNormal( 1, &vec);

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

				m_pVASphere->SetPosition( idx, &vec);
				m_pVASphere->SetNormal( idx, &norm);			

				idx++;
				deg += step;
			}

			y += ystep;
		}

		m_pVASphere->Unlock();
	}


	{

		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3;
		BOOL bBody;

		count = ((hdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((hdiv * 2) * 3);

		m_pIASphere->Create( count, 0);				// Line List
		m_pIASphere->setRestoreType( I3G_RESTORE_MEM);

		m_pIASphere->Lock();

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

				m_pIASphere->SetIndex( idx,	i1);	idx++;
				m_pIASphere->SetIndex( idx,	i2);	idx++;
				m_pIASphere->SetIndex( idx,	i3);	idx++;

				if( bBody)
				{
					//i1 = i2;
					i2 = i3;
					i3 = preidx + i + 1;

					if( i3 >= (preidx + hdiv))
						i3 = preidx;

					m_pIASphere->SetIndex( idx,	i1);	idx++;
					m_pIASphere->SetIndex( idx,	i2);	idx++;
					m_pIASphere->SetIndex( idx,	i3);	idx++;
				}
			}

			preidx = sidx;

			sidx += hdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - hdiv;
		}

		m_pIASphere->Unlock();
	}
}

void	i3LevelNaviMesh::_Build_SelectBoxMesh()
{
	if( NULL != m_pVASelBox && NULL != m_pIASelBox )
	{
		return;	
	}

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


	m_pVASelBox = i3VertexArray::NewObject();
	m_pIASelBox = i3IndexArray::NewObject();

	// Vertex Array
	{
		VEC3D vec;
		i3VertexFormat format;

		format.SetHasPosition( true);


		m_pVASelBox->Create( &format, 32, 0);
		m_pVASelBox->Lock();

		MATRIX tmp;

		i3Matrix::SetScale( &tmp, 1.01f, 1.01f, 1.01f);

		for( int i = 0; i < 32; ++i )
		{
			setXYZ( &vec, selBoxVertex[i][0], selBoxVertex[i][1], selBoxVertex[i][2]);

			i3Vector::TransformCoord( &vec, &vec, &tmp);
			m_pVASelBox->SetPosition( i, &vec);
		}

		m_pVASelBox->Unlock();
	}

	// Index Array
	{
		INT32 base, count;

		m_pIASelBox->Create( 48, 0);
		m_pIASelBox->Lock();

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

			m_pIASelBox->SetIndex( i, (UINT16)idx);
		}

		m_pIASelBox->Unlock();
	}
}

void	i3LevelNaviMesh::_SelectedPoint( i3LevelNaviMesh_GeometryAttr* pickGeom, SEL_POINT_MODE selMode )
{
	switch( selMode )
	{
	case	SEL_POINT_INCLUSIVE:
	case	SEL_POINT_EXCLUSIVE:
			{
				_Add_SelectPoint( pickGeom );
			}
			break;

	case	SEL_POINT_ALTERNATIVE:
			{
				const bool isSelectd = (m_SelPointList.find( pickGeom ) != m_SelPointList.end());

				if( false == isSelectd )
					_Add_SelectPoint( pickGeom );
				else
					_Remove_SelectPoint( pickGeom );
			}
			break;

	case	SEL_POINT_UNSELECT:
			{
				_Remove_SelectPoint( pickGeom );
			}
			break;
	}
}

void	i3LevelNaviMesh::_Add_SelectPoint( i3LevelNaviMesh_GeometryAttr* pickGeom )
{
	if( m_SelPointList.find( pickGeom ) != m_SelPointList.end() )
	{
		return;
	}

	COLORREAL cr;
	i3Color::Set( &cr, 1.0f, 1.0f, 0.1f, 1.0f );

	_SelPoint selData;

	selData.geom	= pickGeom;
	selData.sel		= i3LevelNaviMesh_GeometryAttr::NewObject();

	selData.geom->AddRef();
	selData.sel->SetVertexArray( m_pVASelBox, I3G_PRIM_LINELIST, 24 );
	selData.sel->SetIndexArray( m_pIASelBox );
	selData.sel->SetPos( selData.geom->GetPos() );
	selData.sel->SetScale( selData.geom->GetScale() );
	selData.sel->SetColor( &cr );

	m_SelPointList.insert( i3::make_pair( selData.geom, selData ) );

	if( m_pGeomSelBox )
		m_pGeomSelBox->AppendGeometryAttr( selData.sel );
}

void	i3LevelNaviMesh::_Remove_SelectPoint( i3LevelNaviMesh_GeometryAttr* pickGeom )
{
	map_selpoint::iterator itr = m_SelPointList.find( pickGeom );

	if( itr == m_SelPointList.end() )
	{
		return;
	}

	if( m_pGeomSelBox )
		m_pGeomSelBox->RemoveGeometryAttr( itr->second.sel );

	itr->second.geom->Release();
	itr->second.sel->Release();

	m_SelPointList.erase( itr );
}
