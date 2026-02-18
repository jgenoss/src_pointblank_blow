#include "i3SceneDef.h"
#include "i3SkinBuilder.h"
#include "i3GeometryAttr.h"
#include "i3Base/smart_ptr/make_shared.h"

I3_CLASS_INSTANCE( i3SkinBuilder);

////////////////////////////////////////////////////////////////////////////
void MESH_TRI::Sort(void)
{
	INT32 i, j;

	for( i = 0; i < (m_BoneCount - 1); i++)
	{
		for( j = i + 1; j < m_BoneCount; j++)
		{
			if( m_Bone[i] > m_Bone[j])
			{
				INT32 swap = m_Bone[i];
				m_Bone[i] = m_Bone[j];
				m_Bone[j] = swap;
			}
		}
	}
}

void MESH_TRI::AddBone( INT32 idx)
{
	INT32 i;

	for( i = 0; i < m_BoneCount; i++)
	{
		if( m_Bone[i] == idx)
			return;
	}

	m_Bone[ m_BoneCount] = idx;
	m_BoneCount++;
}

int	MESH_TRI::Compare( const void * p1, const void * p2)
{
	INT32 i;
	MESH_TRI * pTri1, * pTri2;

	pTri1 = (MESH_TRI *) p1;
	pTri2 = (MESH_TRI *) p2;

	INT32 cntBone = MIN( pTri1->m_BoneCount, pTri2->m_BoneCount);

	for( i = 0; i < cntBone; i++)
	{
		if( pTri1->m_Bone[i] > pTri2->m_Bone[i])
			return 1;
		else if( pTri1->m_Bone[i] < pTri2->m_Bone[i])
			return -1;
	}

	if( pTri1->m_BoneCount > pTri2->m_BoneCount)
		return 1;
	else if( pTri1->m_BoneCount < pTri2->m_BoneCount)
		return -1;

	return 0;
}

////////////////////////////////////////////////////////////////////////////

i3SkinBuilder::~i3SkinBuilder(void)
{
	I3MEM_SAFE_FREE( m_pTri);

	I3_SAFE_RELEASE( m_pVA);

	// 아래 5줄 주석은 원래부터 주석인 상태였다. (2012.05.17.수빈)
// 	if (m_pGroupList)
// 	{
// 		for (int i = 0; i < m_pGroupList->GetCount(); ++i)
// 			I3MEM_SAFE_FREE(m_pGroupList->Items[i]);
// 	}
	
//	I3_SAFE_RELEASE( m_pGroupList);			// 셰어드포인터에서는 소멸자처리는 필요없음..
}

void i3SkinBuilder::Prepare( i3GeometryAttr * pGeoAttr)
{
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	i3IndexArray * pIA = pGeoAttr->GetIndexArray();

	// 0.01 ms
	if( pVA->Lock( I3G_LOCK_READONLY) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "VA lock failed!!!");
	}

	if( pIA != nullptr)
	{
		if( pIA->Lock( 0, 0, I3G_LOCK_READONLY) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "IA lock failed!!!!");
		}
	}

	INT32 i, j, k;

	m_TriCount = pGeoAttr->GetPrimitiveCount();
	m_idxStart = pGeoAttr->GetStartIndex();
	m_cntBone	 = pVA->GetFormat()->GetBlendIndexCount();

	// Sorting을 하기 위한 Tri Struct 생성
	if( m_TriCount > 0)
	{
		INT32 idxVtx;

		// 2.9 ms
		m_pTri = (MESH_TRI *) i3MemAlloc( sizeof(MESH_TRI) * m_TriCount);
		i3mem::FillZero( m_pTri, sizeof(MESH_TRI) * m_TriCount);

		for( i = 0; i < m_TriCount; i++)
		{
			for( j = 0; j < m_cntBone; j++)
				m_pTri[i].m_Bone[j] = -1;
		}

		// 14.48 ms
		for( i = 0; i < m_TriCount; i++)
		{
			for( j = 0; j < 3; j++)
			{
				idxVtx = m_idxStart + (i * 3) + j;

				if( pIA != nullptr)
					idxVtx = pIA->GetIndex( idxVtx);

				m_pTri[i].m_Index[j] = idxVtx;

				REAL32 totalW = 0.0f;
				REAL32 w;

				for( k = 0; k < m_cntBone; k++)
				{
					INT32 idxBone = pVA->GetBlendIndex( k, idxVtx);

					if( k < (m_cntBone - 1))
						w = pVA->GetBlendWeight( k, idxVtx);
					else
						w = 1.0f - totalW;

					totalW += w;

					if( w > I3_EPS)
					{
						// 3.23 ms
						m_pTri[i].AddBone( idxBone);
					}

					if( totalW >= 1.f - I3_EPS)
						break;
				}
			}

			I3ASSERT( m_pTri[i].m_BoneCount > 0);

			// 0.66 ms
			m_pTri[i].Sort();
		}

		// 6.04 ms
		::qsort( m_pTri, m_TriCount, sizeof(MESH_TRI), MESH_TRI::Compare);
	}
}

void i3SkinBuilder::DumpTri(void)
{
	INT32 i, j;

	for( i = 0; i < m_TriCount; i++)
	{
		I3TRACE( "TRI[%d] : ", i);

		for( j = 0; j < m_pTri[i].m_BoneCount; j++)
		{
			I3TRACE( "%d, ", m_pTri[i].m_Bone[j]);
		}

		I3TRACE( "\n");
	}
}

void i3SkinBuilder::BuildMesh( i3GeometryAttr * pGeoAttr, bool bOnMem)
{
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	INT32 i, j, idxVtx;
	MESH_TRI * pTri;

	// 새로운 Mesh 준비.

	{
		// Vertex Array는 재사용을 한다.
		I3ASSERT( pVA != nullptr);

		if( bOnMem)
			m_pVA = i3VertexArrayMem::new_object();
		else
		{
			m_pVA = i3VertexArray::new_object();
			m_pVA->setThreadMode( pVA->getThreadMode());
		}

		m_pVA->Create( pVA->GetFormat(), pGeoAttr->GetPrimitiveCount() * 3, 0);//I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY);
	}

	I3ASSERT( m_pVA->Lock() );// I3G_LOCK_OVERWRITE) );

	idxVtx = 0;

	for( i = 0; i < m_TriCount; i++)
	{
		pTri = & m_pTri[i];

		for( j = 0; j < 3; j ++)
		{
			m_pVA->CopyVertexFrom( idxVtx, pVA, pTri->m_Index[j]);
			idxVtx++;
		}
	}
}

void i3SkinBuilder::Build( i3GeometryAttr * pGeoAttr, bool bOnMem)
{
	// Total 65.1 ms

	// 16.9 ms
	Prepare( pGeoAttr);

	// 39.4 ms
	BuildMesh( pGeoAttr, bOnMem);

	// 각 Vertex를 돌면서 Group 생성
	{
		INT32 i;
		I3_DRAW_GROUP group;

		// 0.06 ms
		group.init();
		ClearBoneMap();

		m_pGroupList = i3::make_shared< i3::vector<I3_DRAW_GROUP*> >();
			
		// 5.3 ms
		for( i = 0; i < m_TriCount; i++)
		{
			if( TryAdd( &group, i))
			{
				// 추가되었다.
			}
			else
			{
				// 더 이상 추가가 불가능.
				// 지금까지의 내용을 새로운 Group으로 등록...

				if( group.m_PrimCount > 0)
				{
					// 8.1 ms
					AddGroup( &group);
				}

				group.init();
				group.m_idxStart = (i * 3);
				i--;
			}
			
		}

		if( group.m_PrimCount > 0)
		{
			// 마지막 남은 짜투리 정리.
			// 8.1 ms
			AddGroup( &group);
		}
	}

	// 0.01 ms
	m_pVA->Unlock();
	
	{
		i3VertexArray * pVA = pGeoAttr->GetVertexArray();
		i3IndexArray * pIA = pGeoAttr->GetIndexArray();

		pVA->Unlock();

		if( pIA != nullptr)
		{
			pIA->Unlock();
		}
	}
}

void i3SkinBuilder::ClearBoneMap(void)
{
	INT32 i;

	for( i = 0; i < MAX_BONE; i++)
		m_BoneMap[i] = -1;

	m_UsedBoneCount = 0;
}

bool i3SkinBuilder::TryAdd( I3_DRAW_GROUP * pGroup, INT32 idxTri)
{
	INT32 i, idxBone;
	INT32 cntAddBone = 0;
	INT32	addedBone[ LIMIT_BONE_COUNT];
	MESH_TRI * pTri = & m_pTri[ idxTri];

	// Triangle에서 사용되는 Bone들 중, 새롭게 추가되어야 하는 개수를 구한다.
	for( i = 0; i < pTri->m_BoneCount; i++)
	{
		idxBone = pTri->m_Bone[ i];

		if( m_BoneMap[idxBone] == -1)
		{
			// BoneMap == -1 : 이번 Draw Group에서 사용등록되지 않은 Bone이다.
			addedBone[ cntAddBone] = idxBone;
			cntAddBone++;
		}
	}

	// 추가 Bone까지 감안했을 때, Limit Bone Count를 넘어가는지 확인.
	if((cntAddBone + m_UsedBoneCount) <= m_LimitBoneCount)
	{
		// 추가 가능!

		// Bone Map에 추가 Bone들을 설정해둔다.
		for( i = 0; i < cntAddBone; i++)
		{
			idxBone = addedBone[ i];

			// Group의 Bone Table에 추가.
			// BoneMap Update
			m_BoneMap[ idxBone] = pGroup->AddBone( idxBone);
			I3ASSERT( m_BoneMap[ idxBone] != -1);
		}

		m_UsedBoneCount += cntAddBone;
		pGroup->m_PrimCount++;

		return true;
	}

	// 제한 Bone을 넘어간다!.
	return false;
}

void i3SkinBuilder::AddGroup( I3_DRAW_GROUP * pGroup)
{
	I3_DRAW_GROUP * pNew;

	pNew = (I3_DRAW_GROUP *) i3MemAlloc( sizeof( I3_DRAW_GROUP));

	i3mem::Copy( pNew, pGroup, sizeof( I3_DRAW_GROUP));

	/*
	I3TRACE( "------ GROUP (%d) ------\n");
	I3TRACE( " start : %d\n", pGroup->m_idxStart);
	I3TRACE( " prim  : %d\n", pGroup->m_PrimCount);
	I3TRACE( " bone  : %d (", pGroup->m_BoneCount);

	for( INT32 i = 0; i < pGroup->m_BoneCount; i++)
	{
		I3TRACE( "%d ", pGroup->m_Bone[i]);
	}
	I3TRACE( "\n");
	*/

	m_pGroupList->push_back( pNew);

	AdjustBlendIndex( pGroup);

	ClearBoneMap();
}

void i3SkinBuilder::AdjustBlendIndex( I3_DRAW_GROUP * pGroup)
{
	INT32 i, j, k, idxVtx;
	REAL32 w, totalW;

	idxVtx = pGroup->m_idxStart;

	for( i = 0; i < pGroup->m_PrimCount; i++)
	{
		for( j = 0; j < 3; j++)
		{
			totalW = 0.0f;

			for( k = 0; k < m_cntBone; k++)
			{
				INT32 idxOldBone = m_pVA->GetBlendIndex( k, idxVtx);

				if( k < (m_cntBone - 1))
					w = m_pVA->GetBlendWeight( k, idxVtx);
				else
					w = 1.0f - totalW;

				totalW += w;

				if( w > I3_EPS)
				{
					INT32 idxNewBone = m_BoneMap[ idxOldBone];
					I3ASSERT( idxNewBone != -1);
					I3ASSERT( idxNewBone < LIMIT_BONE_COUNT);

					m_pVA->SetBlendIndex( k, idxVtx, idxNewBone);
				}
			}

			idxVtx++;
		}
	}
}
