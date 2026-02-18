#include	"stdafx.h"
#include	"NavMesh.h"

#include <set>

CNavMesh::CNavMesh()
{
	m_cfg.setDefaultConfig();

	m_pGenerator = NULL;
}

CNavMesh::~CNavMesh()
{
	I3_SAFE_RELEASE( m_pGenerator );
}

bool	CNavMesh::DoGenerate( i3World* world )
{
	if( NULL == m_pGenerator )
		m_pGenerator = i3NavMeshGenerator::NewObject();

	//bool bRes = m_generator.DoGenerate( world, m_cfg );

	bool bRes = m_pGenerator->DoGenerate( world, m_cfg );

	if( bRes )
	{
		m_positions.clear();
		m_indicies.clear();

		//rcPolyMesh* pmesh = m_generator.GetPolyMesh();
		rcPolyMesh* pmesh = m_pGenerator->GetPolyMesh();

		if( pmesh )
		{
			struct tempVec
			{				
				float f[3];
				int id;

				bool operator < ( const tempVec& rhs ) const
				{
					for( int i = 0; i < 3; ++i )
					{
						if( f[i] < rhs.f[i] ) return true;
						else if( f[i] > rhs.f[i] ) return false;
					}

					return false;
				}
			};

			std::set< tempVec > vecList;
			std::vector< int > indexList;

			rcPolyMesh& mesh = *pmesh;

			const int nvp = mesh.nvp;
			const float cs = mesh.cs;
			const float ch = mesh.ch;
			const float* orig = mesh.bmin;

			for (int i = 0; i < mesh.npolys; ++i)
			{
				const unsigned short* p = &mesh.polys[i*nvp*2];

				unsigned short vi[3];
				for (int j = 2; j < nvp; ++j)
				{
					if (p[j] == RC_MESH_NULL_IDX) break;
					vi[0] = p[0];
					vi[1] = p[j-1];
					vi[2] = p[j];
					for (int k = 0; k < 3; ++k)
					{
						const unsigned short* v = &mesh.verts[vi[k]*3];
						const float x = orig[0] + v[0]*cs;
						const float y = orig[1] + v[1]*ch;
						const float z = orig[2] + v[2]*cs;

						tempVec _v;

						_v.f[0] = x;
						_v.f[1] = y;
						_v.f[2] = z;

						std::set< tempVec >::iterator itr = vecList.find( _v );

						if( itr != vecList.end() )
						{
							indexList.push_back( itr->id );
						}

						_v.id = vecList.size();

						vecList.insert( _v );
					}
				}
			}

			m_positions.reserve( vecList.size() );
			m_indicies.reserve( indexList.size() /3 );

			for each( tempVec _v in vecList )
			{
				RT_VEC3D vec;
				i3Vector::Set( &vec, _v.f[0], _v.f[1], _v.f[2] );
				m_positions.push_back( vec );
			}

			for ( int i = 0; i < static_cast<int>(indexList.size() /3); ++i )
			{
				_tri t;
				t.n[0] = indexList[ i*3 +0 ];
				t.n[1] = indexList[ i*3 +1 ];
				t.n[2] = indexList[ i*3 +2 ];

				m_indicies.push_back( t );
			}
		}

		return true;
	}

	return false;
}
