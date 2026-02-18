#include "stdafx.h"
#include "i3NavMeshFile.h"

#include "AIStage.h"
#include "i3NavMeshDataMgr.h"

I3_CLASS_INSTANCE( i3NavMeshFile); //, i3ElementBase );

i3NavMeshFile::i3NavMeshFile()
{
	m_pNMParser = new CAIStageArray;
}

i3NavMeshFile::~i3NavMeshFile()
{
	m_pNMParser->Empty();
	delete m_pNMParser;
}

bool	i3NavMeshFile::LoadNM( const i3::string& _path, i3NavMeshDataMgr* dataMgr )
{
	m_pNMParser->Empty();

	if( nullptr == dataMgr )
	{
		return false;
	}

#ifdef _UNICODE
	i3::stack_wstring strPath;	i3::mb_to_utf16(_path, strPath);
#else
	const i3::string& strPath = _path;
#endif

	CFile file;
	file.Open( strPath.c_str(), CFile::modeRead );
	
	if( FALSE == m_pNMParser->Load( &file ) )
	{
		return false;
	}

	navMeshVertex vertex;
	i3::vector< navMeshVertex > vertexList;

	vertexList.reserve( m_pNMParser->m_pSelected->m_vtMesh.size() *3 );
	for each( const CAIMesh& Mesh in m_pNMParser->m_pSelected->m_vtMesh )
	{
		for( int k = 0; k < 3; ++k )
		{
			// 어째서인지 nm 파일에서 로드 할때 x좌표를 음수로 바꾼다... 그래서 다시 바꿔줌.. 
			vertex.x = -Mesh.m_dxv3Pt[k].x;
			vertex.y = Mesh.m_dxv3Pt[k].y;
			vertex.z = Mesh.m_dxv3Pt[k].z;

			vertexList.push_back( vertex );
		}
	}
		
	return dataMgr->SetNavMeshData( vertexList );
}

bool	i3NavMeshFile::SaveNM( const i3::string& _path, const i3NavMeshDataMgr* dataMgr )
{
	m_pNMParser->Empty();

	if( nullptr == dataMgr )
	{
		return false;
	}

	CAIStage* pAIStage = new CAIStage;
	m_pNMParser->m_vtpChild.push_back( pAIStage );

	CAIMesh	Mesh;
	D3DXVECTOR3 dxv3Pt[3];

	int idx = 0;
	for each( const navMeshPoly& poly in dataMgr->GetPolygonList() )
	{
		if( dataMgr->IsRemovedPolygon( idx ) )
		{
			++idx;
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			const VEC3D* pv = dataMgr->GetVec3D( poly.n[i] );

			dxv3Pt[ i ].x = -pv->x;
			dxv3Pt[ i ].y = pv->y;
			dxv3Pt[ i ].z = pv->z;
		}

		Mesh.Create( dxv3Pt, 0 );
		pAIStage->m_vtMesh.push_back( Mesh );

		++idx;
	}

#ifdef _UNICODE
	i3::stack_wstring strPath;	i3::mb_to_utf16(_path, strPath);
#else
	const i3::string& strPath = _path;
#endif
	CFile file;
	file.Open( strPath.c_str(), CFile::modeCreate | CFile::modeWrite );
	
	m_pNMParser->Save( &file );
	return true;
}

bool	i3NavMeshFile::LoadASE( const i3::string& _path, i3NavMeshDataMgr* dataMgr )
{
	m_pNMParser->Empty();

	if( nullptr == dataMgr )
	{
		return false;
	}

#ifdef _UNICODE
	i3::stack_wstring strPath;	i3::mb_to_utf16(_path, strPath);
#else
	const i3::string& strPath = _path;
#endif


	CFile file;
	file.Open( strPath.c_str(), CFile::modeRead );

	if( FALSE == m_pNMParser->InsertASE( &file ) )
	{
		return false;
	}

	navMeshVertex vertex;
	i3::vector< navMeshVertex > vertexList;

	vertexList.reserve( m_pNMParser->m_pSelected->m_vtMesh.size() *3 );
	for each( const CAIMesh& Mesh in m_pNMParser->m_pSelected->m_vtMesh )
	{
		for( int k = 0; k < 3; ++k )
		{
			// 어째서인지 nm 파일에서 로드 할때 x좌표를 음수로 바꾼다... 그래서 다시 바꿔줌.. 
			vertex.x = -Mesh.m_dxv3Pt[k].x;
			vertex.y = Mesh.m_dxv3Pt[k].y;
			vertex.z = Mesh.m_dxv3Pt[k].z;

			vertexList.push_back( vertex );
		}
	}

	return dataMgr->SetNavMeshData( vertexList );
}

bool	i3NavMeshFile::SaveASE( const i3::string& _path, const i3NavMeshDataMgr* dataMgr )
{
	if( nullptr == dataMgr )
	{
		return false;
	}

	i3::vector< navMeshVertex > vertexList;
	i3::vector< navMeshPoly > polyList;

	dataMgr->GetASEData( polyList, vertexList );

#ifdef _UNICODE
	i3::stack_wstring strPath;	i3::mb_to_utf16(_path, strPath);
#else
	const i3::string& strPath = _path;
#endif


	CStdioFile file;
	file.Open( strPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText );
	

	//
	{
#if _UNICODE
		i3::wstring sz;
#else
		i3::string sz;
#endif

		_ASE_AddString( sz, _T("*3DSMAX_ASCIIEXPORT	200"), true );
		_ASE_AddString( sz, _T("*COMMENT \"AsciiExport Version  2.00 - Mon Jun 11 15:13:33 2012\""), true );

		// SCENE
		{
			_ASE_AddString( sz, _T("*SCENE "), false );
			_ASE_AddString( sz, _T("{"), true );
			_ASE_AddString( sz, _T("*SCENE_FILENAME \"\""), true, 1 );
			_ASE_AddString( sz, _T("*SCENE_FIRSTFRAME 0"), true, 1 );
			_ASE_AddString( sz, _T("*SCENE_LASTFRAME 100"), true, 1 );
			_ASE_AddString( sz, _T("*SCENE_FRAMESPEED 30"), true, 1 );
			_ASE_AddString( sz, _T("*SCENE_TICKSPERFRAME 160"), true, 1 );
			_ASE_AddString( sz, _T("*SCENE_BACKGROUND_STATIC 0.0000	0.0000	0.0000"), true, 1 );
			_ASE_AddString( sz, _T("*SCENE_AMBIENT_STATIC 0.1569	0.1569	0.1569"), true, 1 );
			_ASE_AddString( sz, _T("}"), true );
		}

		// MATERIAL_LIST
		{
			_ASE_AddString( sz, _T("*MATERIAL_LIST "), false );
			_ASE_AddString( sz, _T("{"), true );
			_ASE_AddString( sz, _T("*MATERIAL_COUNT 1"), true, 1 );

			{
				_ASE_AddString( sz, _T("*MATERIAL 0 "), false, 1 );
				_ASE_AddString( sz, _T("{"), true );

				_ASE_AddString( sz, _T("*MATERIAL_NAME \"Material #116\""), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_CLASS \"Standard\""), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_AMBIENT 0.0471	0.0000	1.0000"), true, 2 );				
				_ASE_AddString( sz, _T("*MATERIAL_DIFFUSE 0.0471	0.0000	1.0000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_SPECULAR 0.9000	0.9000	0.9000"), true, 2 );
				
				_ASE_AddString( sz, _T("*MATERIAL_SHINE 0.1000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_SHINESTRENGTH 0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_TRANSPARENCY 0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_WIRESIZE 1.0000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_SHADING Blinn"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_XP_FALLOFF 0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_SELFILLUM 0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_FALLOFF In"), true, 2 );
				_ASE_AddString( sz, _T("*MATERIAL_XP_TYPE Filter"), true, 2 );

				_ASE_AddString( sz, _T("}"), true, 1 );
			}

			_ASE_AddString( sz, _T("}"), true );
		}

		// GEOMOBJECT
		{
			_ASE_AddString( sz, _T("*GEOMOBJECT "), false );
			_ASE_AddString( sz, _T("{"), true );
			_ASE_AddString( sz, _T("*NODE_NAME \"NM_ASE_DATA\""), true, 1 );

			// NODE_TM
			{
				_ASE_AddString( sz, _T("*NODE_TM "), false, 1 );
				_ASE_AddString( sz, _T("{"), true, 0 );

				_ASE_AddString( sz, _T("*NODE_NAME \"NM_ASE_DATA\""), true, 2 );
				_ASE_AddString( sz, _T("*INHERIT_POS 0 0 0"), true, 2 );
				_ASE_AddString( sz, _T("*INHERIT_ROT 0 0 0"), true, 2 );
				_ASE_AddString( sz, _T("*INHERIT_SCL 0 0 0"), true, 2 );
				_ASE_AddString( sz, _T("*TM_ROW0 1.0000	0.0000	0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_ROW1 0.0000	1.0000	0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_ROW2 0.0000	0.0000	1.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_ROW3 -21.9899	4.8721	1.9046"), true, 2 );
				_ASE_AddString( sz, _T("*TM_POS -21.9899	4.8721	1.9046"), true, 2 );
				_ASE_AddString( sz, _T("*TM_ROTAXIS 0.0000	0.0000	0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_ROTANGLE 0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_SCALE 1.0000	1.0000	1.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_SCALEAXIS 0.0000	0.0000	0.0000"), true, 2 );
				_ASE_AddString( sz, _T("*TM_SCALEAXISANG 0.0000"), true, 2 );

				_ASE_AddString( sz, _T("}"), true, 1 );
			}

			// MESH
			{
				CString tmp;

				_ASE_AddString( sz, _T("*MESH "), false, 1 );
				_ASE_AddString( sz, _T("{"), true, 0 );

				_ASE_AddString( sz, _T("*TIMEVALUE 0"), true, 2 );

				tmp.Format( _T("*MESH_NUMVERTEX %d"), vertexList.size() );

				_ASE_AddString( sz, tmp.GetString(), true, 2 );

				tmp.Format( _T("*MESH_NUMFACES %d"), polyList.size() );

				_ASE_AddString( sz, tmp.GetString(), true, 2 );

				
				// MESH_VERTEX_LIST
				{
					_ASE_AddString( sz, _T("*MESH_VERTEX_LIST "), false, 2 );
					_ASE_AddString( sz, _T("{"), true, 0 );
					
					int idx = 0;					
					for each( const navMeshVertex& v in vertexList )
					{
						tmp.Format( _T("*MESH_VERTEX %4d\t%0.4f\t%0.4f\t%0.4f"), idx, -v.x, v.z, v.y );
						_ASE_AddString( sz, tmp.GetString(), true, 3 );
						++idx;
					}

					_ASE_AddString( sz, _T("}"), true, 2 );
				}

				// MESH_FACE_LIST
				{
					_ASE_AddString( sz, _T("*MESH_FACE_LIST "), false, 2 );
					_ASE_AddString( sz, _T("{"), true, 0 );

					int idx = 0;
					for each( const navMeshPoly& p in polyList )
					{
						tmp.Format( _T("*MESH_FACE %4d:"), idx );
						_ASE_AddString( sz, tmp.GetString(), false, 3 );

						tmp.Format( _T("A: %4d B: %4d C: %4d "), p.na, p.nb, p.nc );
						_ASE_AddString( sz, tmp.GetString(), false, 1 );

						tmp.Format( _T("AB: %4d BC: %4d CA: %4d  *MESH_SMOOTHING 1 \t*MESH_MTLID 0"), 1,1,1 );
						_ASE_AddString( sz, tmp.GetString(), true, 1 );


						++idx;
					}

					_ASE_AddString( sz, _T("}"), true, 2 );
				}

				_ASE_AddString( sz, _T("}"), true, 1 );
			}

			{
				_ASE_AddString( sz, _T("*PROP_MOTIONBLUR 0 "), true, 1 );
				_ASE_AddString( sz, _T("*PROP_CASTSHADOW 1 "), true, 1 );
				_ASE_AddString( sz, _T("*PROP_RECVSHADOW 1 "), true, 1 );
				_ASE_AddString( sz, _T("*MATERIAL_REF 0"), true, 1 );
			}			

			_ASE_AddString( sz, _T("}"), true );
		}

		file.WriteString( sz.c_str() );
	}

	return true;
}


void	i3NavMeshFile::_ASE_AddString( i3::string& szSrc, const i3::string& szAdd, bool bReturn, int tabCnt )
{
	for( int i = 0; i < tabCnt; ++i )
	{
		szSrc += "\t";
	}

	szSrc += szAdd;

	if( bReturn )
	{
		szSrc += "\n";
	}
}

void	i3NavMeshFile::_ASE_AddString( i3::wstring& szSrc, const i3::wstring& szAdd, bool bReturn, int tabCnt )
{
	for( int i = 0; i < tabCnt; ++i )
	{
		szSrc += L"\t";
	}

	szSrc += szAdd;

	if( bReturn )
	{
		szSrc += L"\n";
	}
}
