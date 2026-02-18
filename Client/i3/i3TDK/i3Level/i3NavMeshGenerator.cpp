#include "stdafx.h"
#include "i3NavMeshGenerator.h"

#include "i3Base/itl/set.h"

I3_CLASS_INSTANCE( i3NavMeshGenerator);//, i3ElementBase );

i3NavMeshGenerator::i3NavMeshGenerator() 
{
	m_ctx = new logContext;
}

i3NavMeshGenerator::~i3NavMeshGenerator()
{
	delete m_ctx;
	_CleanUp();
}

bool	i3NavMeshGenerator::DoGenerate( i3World* world, const navMeshConfig& cfg, fnAutoGenLoadingRatio pFn )
{
	m_world = world;

	if( nullptr == m_world ) 
	{
		return false;
	}

	if( pFn ) 
		(*pFn)( 0.05f, "calculate vertex data..." );

	_CleanUp();	// 데이타 초기화

	bool bGen = false;
	
	switch( cfg.genType )
	{
	case	NAVMESH_GENERATE_GEOMETRY:
			bGen = _CalculateVertexList_byGeometry();
			break;

	case	NAVMESH_GENERATE_TRANSFORM2:
			bGen = _CalculateVertexList_byTransform2();
			break;
	}	

	if( bGen )
		return _DoGenerate( cfg, pFn );

	return false;
}

bool	i3NavMeshGenerator::_CalculateVertexList_byGeometry()
{
	i3::vector< i3Node* > nodeList;
	i3Scene::FindNodeByExactType( m_world->getWorldRoot(), i3Geometry::static_meta(), nodeList );

	if( true == nodeList.empty() )
	{
		return true;
	}

	std::vector< std::pair< i3GeometryAttr*, MATRIX > >	geomList;

	// get geometry list
	{
		MATRIX wmat;	

		for each( i3Geometry* geom in nodeList )
		{
			_CalculateWTM( geom, wmat );

			const int nCnt = geom->GetGeometryAttrCount();

			for( int i = 0; i < nCnt; ++i )
			{
				i3RenderAttr* attr = geom->GetGeometryAttr( i );

				if( nullptr != attr )
					geomList.push_back( std::make_pair( static_cast< i3GeometryAttr* >( attr ), wmat ) );
			}
		}
	}

	// calculate vertex & triangle count
	{
		m_nvert = 0;
		m_ntri = 0;

		for each( std::pair< i3GeometryAttr*, MATRIX > p in geomList )
		{
			i3GeometryAttr* attr = p.first;

			if( nullptr == attr )
			{
				continue;
			}

			i3VertexArray* vt	= attr->GetVertexArray();
			i3IndexArray* it	= attr->GetIndexArray();

			if( nullptr != vt && nullptr != it )
			{
				i3::map< i3VertexArray*, _VA_info >::iterator itr;

				itr = m_mapVA.find( vt );
				if( itr == m_mapVA.end() )
				{
					const int nCnt = static_cast<int>(vt->GetCount());

					_VA_info info;

					info.nVertCnt	= nCnt;
					info.nOffset	= m_nvert;
					info.bLoaded	= false;

					m_nvert += nCnt;
					m_mapVA.insert( i3::make_pair( vt, info ) );
				}
				
				m_ntri	+= (it->GetCount() / 3);
			}
		}
	}

	if( 0 == m_nvert || 0 == m_ntri )
	{
		return false;
	}

	// calculate vertex list
	{
		m_verts = new float[ m_nvert *3 ];
		m_tris	= new int[ m_ntri *3 ];

		int nv = 0;
		int nt = 0;
		int ntri_offset		= 0;

		VEC3D v;

		for each( std::pair< i3GeometryAttr*, MATRIX > p in geomList )
		{
			i3GeometryAttr* attr = p.first;

			if( nullptr == attr )
			{
				continue;
			}

			i3VertexArray* vt	= attr->GetVertexArray();
			i3IndexArray* it	= attr->GetIndexArray();

			if( nullptr != vt && nullptr != it )
			{
				int nlen = 0;
				BOOL bLock = FALSE;

				// set vertex position
				{
					i3::map< i3VertexArray*, _VA_info >::iterator itr;

					itr = m_mapVA.find( vt );
					if( itr == m_mapVA.end() )
					{
						I3PRINTLOG(I3LOG_NOTICE,  "버텍스 어레이는 항상 찾을 수 있어야함.. 이상현상" );
						continue;
					}

					_VA_info& info = itr->second;

					if( false == info.bLoaded )
					{
						nlen	= vt->GetCount();
						bLock	= vt->Lock();

						for( int i = 0; i < nlen; ++i )
						{
							if( bLock )
								vt->GetPosition( i, &v );
							else
								i3Vector::Set( &v, 0,0,0 );

							i3Vector::TransformCoord( &v, &v, &p.second );

							// calculate bound min, max
							{
								if( 0 == i && 0 == nv )
								{
									m_min.x = v.x, m_min.y = v.y, m_min.z = v.z;
									m_max.x = v.x, m_max.y = v.y, m_max.z = v.z;
								}
								else
								{
									if( v.x < m_min.x ) m_min.x = v.x;
									if( v.y < m_min.y ) m_min.y = v.y;
									if( v.z < m_min.z ) m_min.z = v.z;

									if( m_max.x < v.x ) m_max.x = v.x;
									if( m_max.y < v.y ) m_max.y = v.y;
									if( m_max.z < v.z ) m_max.z = v.z;
								}
							}

							m_verts[ nv +0 ] = v.x;
							m_verts[ nv +1 ] = v.y;
							m_verts[ nv +2 ] = v.z;

							nv += 3;
						}

						if( bLock ) vt->Unlock();

						info.bLoaded = true;
					}

					ntri_offset = info.nOffset;
				}

				// set vertex index
				{
					nlen	= it->GetCount();
					bLock	= it->Lock();

					for( int i = 0; i < nlen; ++i )
					{
						if( bLock )
							m_tris[ nt++ ] = it->GetIndex( i ) + ntri_offset;
						else
							m_tris[ nt++ ] = 0;
					}

					if( bLock ) it->Unlock();
				}
				
				//ntri_offset += vt->GetCount();
			}
		}
	}

	return true;
}


bool	i3NavMeshGenerator::_CalculateVertexList_byTransform2()
{
	i3::vector< i3Node* > nodeList;
	//i3Scene::FindNodeByExactType( m_world->getWorldCollision()->getInstanceSg(), i3Transform2::GetClassMeta(), nodeList );
	i3Scene::FindNodeByExactType( m_world->getHitCollision()->getInstanceSg(), i3Transform2::static_meta(), nodeList );

	if( true == nodeList.empty() )
	{
		return false;
	}

	std::vector< std::pair< i3Transform2*, MATRIX > > tfm2List;

	// calculate wtm
	{
		MATRIX wmat;

		for each( i3Node* node in nodeList )
		{
			i3Transform2* tfm = static_cast< i3Transform2* >( node );

			_CalculateWTM( node, wmat );

			if( tfm->getShapeSet() )
				tfm2List.push_back( std::make_pair( tfm, wmat ) );
		}
	}


	// calculate vertex & triangle count
	{
		m_nvert = 0;
		m_ntri = 0;

		for each( std::pair< i3Transform2*, MATRIX > p in tfm2List )
		{
			i3PhysixShapeSet* shape = p.first->getShapeSet();

			switch( shape->getShapeType() )
			{
			case	I3_PHYSIX_SHAPE_SPHERE:
			case	I3_PHYSIX_SHAPE_CAPSULE:
			case	I3_PHYSIX_SHAPE_CONVEXMESH:
			case	I3_PHYSIX_SHAPE_BOX:
					{
						// ??
					}
					break;

			case	I3_PHYSIX_SHAPE_TRIMESH:
					{
						i3Vector3Array* varr = shape->getPositionArray();

						if( varr )
						{
							m_ntri += shape->getFaceCount();
							m_nvert += varr->GetVectorCount();
						}
					}
					break;

			default:
				break;
			}
		}
	}


	if( 0 == m_nvert || 0 == m_ntri )
	{
		return false;
	}

	m_verts		= new float[ m_nvert *3 ];
	m_tris		= new int[ m_ntri *3 ];

	int nv = 0;
	int nt = 0;
	int ntri_offset = 0;

	VEC3D v;

	for each( std::pair< i3Transform2*, MATRIX > p in tfm2List )
	{
		i3PhysixShapeSet* shape = p.first->getShapeSet();

		switch( shape->getShapeType() )
		{
		case	I3_PHYSIX_SHAPE_SPHERE:
		case	I3_PHYSIX_SHAPE_CAPSULE:
		case	I3_PHYSIX_SHAPE_CONVEXMESH:
		case	I3_PHYSIX_SHAPE_BOX:
				break;

		case	I3_PHYSIX_SHAPE_TRIMESH:
				{
					i3Vector3Array* varr = shape->getPositionArray();

					if( nullptr == varr )
					{
						continue;
					}

					for( unsigned int i = 0; i < varr->GetVectorCount(); ++i )
					{
						i3Vector::Copy( &v, varr->GetVector(i) );
						i3Vector::TransformCoord( &v, &v, &p.second );

						if( 0 == nv && 0 == i )
						{
							m_max.x = m_min.x = v.x;
							m_max.y = m_min.y = v.y;
							m_max.z = m_min.z = v.z;
						}
						else
						{
							if( v.x < m_min.x ) m_min.x = v.x;
							if( v.y < m_min.y ) m_min.y = v.y;
							if( v.z < m_min.z ) m_min.z = v.z;

							if( m_max.x < v.x ) m_max.x = v.x;
							if( m_max.y < v.y ) m_max.y = v.y;
							if( m_max.z < v.z ) m_max.z = v.z;
						}

						m_verts[ nv +0 ] = v.x;
						m_verts[ nv +1 ] = v.y;
						m_verts[ nv +2 ] = v.z;

						nv += 3;
					}

					for( unsigned int i = 0; i < shape->getFaceCount(); ++i )
					{
						I3_PHYSIX_TRI* ptri = shape->getFace(i);

						m_tris[ nt + 0 ] = ptri->m_Index[ 0 ] + ntri_offset;
						m_tris[ nt + 1 ] = ptri->m_Index[ 1 ] + ntri_offset;
						m_tris[ nt + 2 ] = ptri->m_Index[ 2 ] + ntri_offset;

						nt += 3;

					}

					ntri_offset += varr->GetVectorCount();
				}
				break;
		}
	}


	return true;
}


void	i3NavMeshGenerator::_CalculateWTM( i3Node* node, MATRIX& wmat )
{
	i3Matrix::Identity( &wmat );

	if( nullptr == node )
	{
		return;
	}

	while( nullptr != node )
	{
		if( i3::kind_of<i3Transform*>(node))
		{
			i3Matrix::Mul( &wmat, &wmat, ((i3Transform*) node)->GetMatrix() );
		}

		node = node->GetParent();
	}
}

void	i3NavMeshGenerator::_CleanUp()
{
	if( nullptr != m_verts )
	{
		delete [] m_verts;
		m_verts = nullptr;
	}

	if( nullptr != m_tris )
	{
		delete [] m_tris;
		m_tris = nullptr;
	}

	m_nvert = 0;
	m_ntri	= 0;

	if( nullptr != m_triareas )
	{
		delete [] m_triareas;
		m_triareas = nullptr;
	}

	if( nullptr != m_solid )
	{
		rcFreeHeightField(m_solid);
		m_solid = nullptr;
	}	

	if( nullptr != m_chf )
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = nullptr;
	}

	if( nullptr != m_cset )
	{
		rcFreeContourSet(m_cset);
		m_cset = nullptr;
	}	
}

bool	i3NavMeshGenerator::_DoGenerate( const navMeshConfig& cfg, fnAutoGenLoadingRatio pFn )
{
	if( nullptr == m_verts || nullptr == m_tris )
	{
		return false;
	}

	if( 0 == m_nvert || 0 == m_ntri )
	{
		return false;
	}

 	const float* bmin	= (float*) (&m_min.x);
	const float* bmax	= (float*) (&m_max.x);
	const float* verts	= m_verts;
	const int nverts	= m_nvert;
	const int* tris		= m_tris;
	const int ntris		= m_ntri;

	// Step 1. Initialize build config.
	{
		::memset(&m_cfg, 0, sizeof(m_cfg));

		// 임시로 하드코딩
		m_cfg.cs						= cfg.cellSize;		//0.1f;
		m_cfg.ch						= cfg.cellHeight;	//0.1f;
		m_cfg.walkableSlopeAngle		= 45.0f;
		m_cfg.walkableHeight			= (int)ceilf( 2.0f / m_cfg.ch );
		m_cfg.walkableClimb				= (int)floorf( 0.9f / m_cfg.ch );
		m_cfg.walkableRadius			= (int)ceilf( 0.6f / m_cfg.cs );
		m_cfg.maxEdgeLen				= (int)( cfg.edgeMaxLen / cfg.cellSize );
		m_cfg.maxSimplificationError	= cfg.edgeMaxError;
		m_cfg.minRegionArea				= (int)rcSqr( cfg.regionMinSize );		// Note: area = size*size
		m_cfg.mergeRegionArea			= (int)rcSqr( cfg.regionMergeSize);		// Note: area = size*size
		m_cfg.maxVertsPerPoly			= (int) 3;
		m_cfg.detailSampleDist			= cfg.detailSampleDist < 0.9f ? 0 : cfg.cellSize * cfg.detailSampleDist;
		m_cfg.detailSampleMaxError		= cfg.cellHeight * cfg.detailSampleMaxError;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.

		rcVcopy( m_cfg.bmin, bmin );
		rcVcopy( m_cfg.bmax, bmax );
		rcCalcGridSize( m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height );
		
	}		
	
	if( pFn )
		(*pFn)( 0.1f, "Rasterize input polygon soup..." );

	// log
	{
		// Reset build times gathering.
		m_ctx->resetTimers();

		// Start the build process.	
		m_ctx->startTimer(RC_TIMER_TOTAL);

		m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
		m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
		m_ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts/1000.0f, ntris/1000.0f);
	}

	// Step 2. Rasterize input polygon soup.
	{
		// Allocate voxel heightfield where we rasterize our input data to.
		m_solid = rcAllocHeightfield();
		if (!m_solid)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
			return false;
		}
		if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
			return false;
		}

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		m_triareas = new unsigned char[ntris];
		if (!m_triareas)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
			return false;
		}

		// Find triangles which are walkable based on their slope and rasterize them.
		// If your input data is multiple meshes, you can transform them here, calculate
		// the are type for each of the meshes and rasterize them.
		memset(m_triareas, 0, ntris*sizeof(unsigned char));
		rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
		rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb);

		if (!m_keepInterResults)
		{
			delete [] m_triareas;
			m_triareas = nullptr;
		}

		if( pFn )
			(*pFn)( 0.25f, "Filter walkables surfaces..." );
	}
	

	// Step 3. Filter walkables surfaces.
	{
		// Once all geoemtry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.

		rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
		rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
		rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

		if( pFn )
			(*pFn)( 0.3f, "Partition walkable surface to simple regions..." );
	}
	

	// Step 4. Partition walkable surface to simple regions.
	{
		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		
		m_chf = rcAllocCompactHeightfield();
		if (!m_chf)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
			return false;
		}
		if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
			return false;
		}

		if( pFn )
			(*pFn)( 0.4f, "Erode the walkable area by agent radius..." );

		if (!m_keepInterResults)
		{
			rcFreeHeightField(m_solid);
			m_solid = nullptr;
		}

		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
			return false;
		}

		if( pFn )
			(*pFn)( 0.5f, "Partition the walkable surface.." );

		// (Optional) Mark areas.
		/*const ConvexVolume* vols = m_geom->getConvexVolumes();
		for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
			rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);*/

		//if( m_monotonePartitioning )
		if( cfg.monotonePartitioning )
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
				return false;
			}

			if( pFn )
				(*pFn)( 0.8f, "Trace and simplify region contours..." );
		}
		else
		{
			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if (!rcBuildDistanceField(m_ctx, *m_chf))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
				return false;
			}

			if( pFn )
				(*pFn)( 0.6f, "Partition the walkable surface.." );

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
				return false;
			}

			if( pFn )
				(*pFn)( 0.8f, "Trace and simplify region contours..." );
		}

		
	}

	// Step 5. Trace and simplify region contours.
	{
		// Create contours.
		m_cset = rcAllocContourSet();
		if (!m_cset)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
			return false;
		}
		if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
			return false;
		}
	}

	// Step 6. Build polygons mesh from contours.
	{

		// Build polygon navmesh from the contours.
		m_pmesh = rcAllocPolyMesh();
		if (!m_pmesh)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
			return false;
		}
		if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
			return false;
		}

		if( pFn )
			(*pFn)( 0.85f, "Create detail mesh..." );
	}

	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	{
		m_dmesh = rcAllocPolyMeshDetail();
		if (!m_dmesh)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
			return false;
		}

		if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
			return false;
		}

		if (!m_keepInterResults)
		{
			rcFreeCompactHeightfield(m_chf);
			m_chf = nullptr;
			rcFreeContourSet(m_cset);
			m_cset = nullptr;
		}

		if( pFn )
			(*pFn)( 0.9f, "..." );

		// At this point the navigation mesh data is ready, you can access it from m_pmesh.
		// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.
	}

	return true;
}
