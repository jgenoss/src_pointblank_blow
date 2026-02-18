#include "i3SceneDef.h"
#include "i3ParticleRender.h"
#include "i3ParticleGroupForTriangles.h"
#include "i3Base/itl/vector_set_util.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3DecalManager.h"
#include "i3Decal.h"
#include "i3GeometrySubsetAttr.h"

#define DECAL_HIT_DEPTH	0.01f

I3_CLASS_INSTANCE( i3ParticleRender);

I3_EXPORT_SCENE i3ParticleRender *		g_pParticleRender = nullptr;

bool	operator<(const i3ParticleGroup& a1, const i3ParticleGroup& a2)
{
	return a2.getPriority() < a1.getPriority();			// 내림차순임..
}

// 아래와 같은 방식으로 렌더링을 걸기 위해서는 별도의 DrawIndexedPrim을 호출할만한 Attribute를 만들어두어야할
// 것이다...

struct i3ParticleSubsetForSort : i3::class_object_pool<i3ParticleSubsetForSort>
{
	i3ParticleGroup*			m_pGroup = nullptr;
	i3GeometrySubsetAttr*		m_pSubsetAttr = nullptr;
};

struct i3AlphaSortSubset : i3::class_object_pool<i3AlphaSortSubset>
{
	i3::vector<I3PARTICLEOBJECT*>			m_ParticleObjList;			// 분배하려면 필요하다..
	i3::vector<i3ParticleSubsetForSort*>	m_ParticleSubsetList;
	i3AlphaSortNodeSet*						m_pNodeSet = nullptr;
};


namespace
{
	
	struct CompParticleCameraDistance
	{
		bool operator()(const I3PARTICLEOBJECT* l, const I3PARTICLEOBJECT* r) const
		{
			return l->m_PosFromView.z < r->m_PosFromView.z;		// 먼것이 음수이며, 먼것부터 그린다.. (DX좌표계 반대)
		}
	} g_compParticle_Dist;

	struct CompParticlePriority
	{
		bool operator()(const I3PARTICLEOBJECT* l, const I3PARTICLEOBJECT* r) const
		{
			return l->m_Owner->getPriority() > r->m_Owner->getPriority();		
		}
	} g_compParticle_Priority;

	struct CompNodeCameraDistance
	{
		bool operator()(const i3AlphaSortNodeSet* l, const i3AlphaSortNodeSet* r) const
		{
			return l->m_fMaxDepth + l->m_fMinDepth < r->m_fMaxDepth + r->m_fMinDepth;			
		}
	} g_compNode_Dist;

	struct CompAlphaSortSubset
	{
		bool operator()( const i3AlphaSortSubset* l, const I3PARTICLEOBJECT* r) const
		{
			if ( l->m_pNodeSet == nullptr )		// 가장 카메라 뒤에 있는 놈으로 가정..(무한 양값) ... +2000  >   -10
				return false;

			if ( l->m_pNodeSet->m_fMinDepth < r->m_PosFromView.z )			// 
				return true;

			return false;
		}

	} g_compAlphaSortSubset;


	bool	RaySegOBB( const VEC3D& rayNDir, float rayLength, const i3AlphaOBB& obb)
	{
		const VEC3D& obb_extent = obb.m_extent;

		VEC3D seg_half = rayNDir; 		
		seg_half.x *= rayLength * .5f;		seg_half.y *= rayLength * .5f;		seg_half.z *= rayLength * .5f;

		//		VEC3D seg_center = seg_half; 		
		VEC3D diff = seg_half;	
		diff.x -= obb.m_center.x; 		diff.y -= obb.m_center.y; 		diff.z -= obb.m_center.z; 

		float f;
		VEC3D abs_axis_half_seg;

		abs_axis_half_seg.x = fabs(i3Vector::Dot(&seg_half, &obb.m_axis[0] ) );
		f = i3Vector::Dot( &diff, &obb.m_axis[0]);
		if (fabs(f) > obb_extent.x + abs_axis_half_seg.x ) return false;

		abs_axis_half_seg.y = fabs(i3Vector::Dot( &seg_half, &obb.m_axis[1] ) );
		f = i3Vector::Dot( &diff, &obb.m_axis[1]);
		if (fabs(f) > obb_extent.y + abs_axis_half_seg.y ) return false;

		abs_axis_half_seg.z = fabs(i3Vector::Dot( &seg_half, &obb.m_axis[2] ) );
		f = i3Vector::Dot( &diff, &obb.m_axis[2]);
		if (fabs(f) > obb_extent.z + abs_axis_half_seg.z ) return false;

		VEC3D half_cross_diff;
		i3Vector::Cross(&half_cross_diff, &seg_half, &diff);

		f = i3Vector::Dot(&half_cross_diff, &obb.m_axis[0]);
		if (fabs(f) > obb_extent.y * abs_axis_half_seg.z + obb_extent.z * abs_axis_half_seg.y ) return false;
		f = i3Vector::Dot(&half_cross_diff, &obb.m_axis[1]);
		if (fabs(f) > obb_extent.x * abs_axis_half_seg.z + obb_extent.z * abs_axis_half_seg.x ) return false;
		f = i3Vector::Dot(&half_cross_diff, &obb.m_axis[2]);
		if (fabs(f) > obb_extent.x * abs_axis_half_seg.y + obb_extent.y * abs_axis_half_seg.x ) return false;

		return true;
	}

	void					_ReserveParticleSubset(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool, size_t num)
	{
		ParticleSubsetPool.resize(num);
		const size_t numSubset = ParticleSubsetPool.size();
		for (size_t i = 0; i < numSubset ; ++i)
		{
			i3ParticleSubsetForSort* new_obj = new i3ParticleSubsetForSort;
			new_obj->m_pGroup = nullptr;				// 일단 레퍼런스카운팅은 하지 않음..
			new_obj->m_pSubsetAttr = i3GeometrySubsetAttr::new_object();
			ParticleSubsetPool[i] = new_obj;
		}
	}
	
	void					_ReserveAlphaSortSubset(i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetPool, size_t num)
	{
		AlphaSortSubsetPool.resize(num);
		const size_t numSubset = AlphaSortSubsetPool.size();
		for (size_t i = 0; i < numSubset ; ++i)
		{
			i3AlphaSortSubset* new_obj = new i3AlphaSortSubset;
			new_obj->m_pNodeSet = nullptr;				// 일단 레퍼런스카운팅은 하지 않음..
			AlphaSortSubsetPool[i] = new_obj;
		}
		
	}


	i3ParticleSubsetForSort* _AllocParticleSubset(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool, 
		i3::vector<i3ParticleSubsetForSort*>& currParticleSubsetList)
	{
		if ( ParticleSubsetPool.empty() )
		{
		//	const size_t old_size = currParticleSubsetList.size();		
		// 이것으로 리사이즈처리하면 버그가 될수 있음..0이면 할당이 안된다.

			const size_t pool_capacity = ParticleSubsetPool.capacity();
			size_t size_to_reserve = 1;
			if (pool_capacity > 0)
			{
				size_to_reserve = (pool_capacity > 64) ? 64 : pool_capacity;		// 64로 일단 적당한 매직넘버 설정..
			}
			_ReserveParticleSubset(ParticleSubsetPool, size_to_reserve);
		}

		i3ParticleSubsetForSort* new_subset = ParticleSubsetPool.back();
		ParticleSubsetPool.pop_back();
		currParticleSubsetList.push_back(new_subset);		// 일단 레퍼런스카운팅은 하지 않도록 함..
		return new_subset;	
	}

	i3AlphaSortSubset*	_AllocAlphaSortSubset( i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetPool, 
		i3::vector<i3AlphaSortSubset*>& currAlphaSortSubsetList)
	{
		if (AlphaSortSubsetPool.empty())
		{
			const size_t old_size = currAlphaSortSubsetList.size();
			_ReserveAlphaSortSubset(AlphaSortSubsetPool, old_size);
		}
		
		i3AlphaSortSubset* new_subset = AlphaSortSubsetPool.back();
		AlphaSortSubsetPool.pop_back();
		currAlphaSortSubsetList.push_back(new_subset);
		return new_subset;
	}

	void		_ResetParticleSubsetList(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool, 
		i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetListForDelete)
	{
		const size_t num = ParticleSubsetListForDelete.size();

		for (size_t i = 0 ; i < num ; ++i)
		{
			i3ParticleSubsetForSort* subset = ParticleSubsetListForDelete[i];
			subset->m_pGroup = nullptr;
			subset->m_pSubsetAttr->SetGeometryAttr(nullptr);
		}

		ParticleSubsetPool.insert(ParticleSubsetPool.end(), 
			ParticleSubsetListForDelete.begin(), ParticleSubsetListForDelete.end());

		ParticleSubsetListForDelete.clear();
	}

	void  _ClearAlphaSortSubsetList( i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetPool, 
		i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetListForDelete,
		i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetList)
	{
		const size_t num = AlphaSortSubsetList.size();
		for (size_t i = 0 ; i < num ; ++i)
		{
			i3AlphaSortSubset* subset = AlphaSortSubsetList[i];
			subset->m_ParticleObjList.clear();
			
			i3::vector<i3ParticleSubsetForSort*>& psList = subset->m_ParticleSubsetList;
			ParticleSubsetListForDelete.insert(ParticleSubsetListForDelete.end(), 
				psList.begin(), psList.end());
			psList.clear();

			subset->m_pNodeSet = nullptr;
		}
		
		AlphaSortSubsetPool.insert(AlphaSortSubsetPool.end(), AlphaSortSubsetList.begin(), 
			AlphaSortSubsetList.end());

		AlphaSortSubsetList.clear();
	}

		
	i3ParticleSubsetForSort*	_CancelLastParticleSubset(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool, 
		i3::vector<i3ParticleSubsetForSort*>& currParticleSubsetList)
	{
		if ( currParticleSubsetList.empty() )
			return nullptr;

		i3ParticleSubsetForSort* last_obj =	currParticleSubsetList.back();
		currParticleSubsetList.pop_back();
		last_obj->m_pGroup = nullptr;
		last_obj->m_pSubsetAttr->SetGeometryAttr(nullptr);
		ParticleSubsetPool.push_back(last_obj);

		if ( currParticleSubsetList.empty() )
			return nullptr;

		return currParticleSubsetList.back();
	}

	void		_DestroyParticleSubsetList_InDestructor(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool)
	{
		const size_t num_pool_subset = ParticleSubsetPool.size();

		for (size_t i = 0 ; i < num_pool_subset ; ++i)
		{
			i3ParticleSubsetForSort* obj = ParticleSubsetPool[i];
			I3_SAFE_RELEASE(obj->m_pSubsetAttr);
			delete obj;
		}
	}

	void		_DestroyAlphaSortSubsetList_InDestructor(i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetPool, 
		i3::vector<i3AlphaSortSubset*>& currAlphaSortSubsetList)
	{
		const size_t num_curr_subset = currAlphaSortSubsetList.size();

		for (size_t i = 0 ; i < num_curr_subset ; ++i)
		{
			i3AlphaSortSubset* obj = currAlphaSortSubsetList[i];

			const size_t num_particle_subset = obj->m_ParticleSubsetList.size();

			for (size_t j = 0 ; j < num_particle_subset ; ++j)
			{
				i3ParticleSubsetForSort* obj2 =	obj->m_ParticleSubsetList[j];
				I3_SAFE_RELEASE(obj2->m_pSubsetAttr);
				delete obj2;
			}

			delete obj;
		}

		const size_t num_pool_subset = AlphaSortSubsetPool.size();

		for (size_t i = 0 ; i < num_pool_subset ; ++i)
		{
			i3AlphaSortSubset* obj = AlphaSortSubsetPool[i];
			delete obj;
		}
	}



	i3ParticleSubsetForSort*	_QueryParticleSubset(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool, 
		i3::vector<i3ParticleSubsetForSort*>& currParticleSubsetList,
		i3ParticleSubsetForSort* pOldSubset, i3ParticleGroup* pCurrGroup)
	{
		if (pOldSubset == nullptr || pOldSubset->m_pGroup != pCurrGroup )
		{
			i3ParticleSubsetForSort* new_obj = _AllocParticleSubset(ParticleSubsetPool, currParticleSubsetList);
			new_obj->m_pGroup = pCurrGroup;
			new_obj->m_pSubsetAttr->SetGeometryAttr( pCurrGroup->GetGeomAttr() );
			new_obj->m_pSubsetAttr->SetPrimitiveCount(0);

			INT32 endIdx = pCurrGroup->GetEndIndex();
			new_obj->m_pSubsetAttr->SetStartIndex(endIdx);

			return new_obj;
		}

		return pOldSubset;		// 이 경우는 기존 것 그대로 씀...
	}
	
	void	_SortAlphaSortNode(i3SceneTracer* pTracer, 
			i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetPool,
			i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetList)
	{
		
		i3::vector<i3AlphaSortNodeSet*>& alpha_node_list = pTracer->GetAlphaSortNodeSetList();

		i3::stable_sort(alpha_node_list.begin(), alpha_node_list.end(), g_compNode_Dist);		// 일단 노드끼리 소팅부터 먼저한다...

		const size_t num_alpha_node = alpha_node_list.size();

		for (size_t i = 0; i < num_alpha_node ; ++i )
		{
			i3AlphaSortNodeSet* node_set = alpha_node_list[i];

			if (node_set->m_fMinDepth >= 0.f && node_set->m_fMaxDepth >= 0.f )		
				continue;

			i3AlphaSortSubset* new_set = _AllocAlphaSortSubset(AlphaSortSubsetPool, AlphaSortSubsetList);
			new_set->m_pNodeSet = node_set;
		}

		i3AlphaSortSubset* new_set = _AllocAlphaSortSubset(AlphaSortSubsetPool, AlphaSortSubsetList);
		new_set->m_pNodeSet = nullptr;
		
	}


	void	_DistributeObjectsIntoSubset(i3::vector<i3AlphaSortSubset*>& AlphaSortSubsetList, 
		i3::vector<I3PARTICLEOBJECT*>& currAccObjectList, 
		i3::vector<I3PARTICLEOBJECT*>& currAccObjectList_AddBlend)
	{
		// STL 소트 함수로 전체 소트 걸음..
		i3::stable_sort(currAccObjectList.begin(), currAccObjectList.end(), g_compParticle_Dist);
		i3::stable_sort(currAccObjectList_AddBlend.begin(), currAccObjectList_AddBlend.end(), g_compParticle_Priority);

		// 가장 값이 작은 놈 (먼 놈) 부터 순차적으로 분배...
		// 1. m_currAccObjectList 부터 먼저...
		const size_t numAccObj = currAccObjectList.size();

		size_t       count_alphasubset = 0;
		i3AlphaSortSubset* alpha_subset = AlphaSortSubsetList[0];
		const size_t num_alpha_subset = AlphaSortSubsetList.size();

		size_t i = 0;

		for ( ; i < numAccObj ; )
		{
			I3PARTICLEOBJECT* obj = currAccObjectList[i];

			i3AlphaSortNodeSet* node_set = alpha_subset->m_pNodeSet;

			REAL32 objZ = obj->m_PosFromView.z; 

			if (node_set == nullptr)			// 마지막엔 남아 있는 모든걸 털어낸다..
			{
				break;
			}
			else
				if ( node_set->m_fMinDepth > objZ )		// 파티클이 더 뒤에 있으면 파티클을 넣음..
				{
					alpha_subset->m_ParticleObjList.push_back(obj);		++i;
				}
				else									// 앞에 존재하면... 다음번 서브셋으로...
				{
					++count_alphasubset;

					if (count_alphasubset == num_alpha_subset)
						break;

					alpha_subset = AlphaSortSubsetList[count_alphasubset];
				}

		}

		alpha_subset->m_ParticleObjList.insert( alpha_subset->m_ParticleObjList.end(), 
			currAccObjectList.begin() + i, currAccObjectList.end() );

		const size_t numAccObj_AddBlend = currAccObjectList_AddBlend.size();		// 이건 거리소팅이 안되있어서 이진검색스타일로 처리한다.

		for ( size_t i = 0 ; i < numAccObj_AddBlend ; ++i )
		{
			I3PARTICLEOBJECT* obj = currAccObjectList_AddBlend[i];

			i3::vector<i3AlphaSortSubset*>::iterator it = i3::lower_bound( AlphaSortSubsetList.begin(), 
				AlphaSortSubsetList.end(), obj, g_compAlphaSortSubset);

			if ( it == AlphaSortSubsetList.end() )		
			{
				i3AlphaSortSubset* backSubset = AlphaSortSubsetList.back();
				backSubset->m_ParticleObjList.push_back(obj);
			}
			else
			{
				i3AlphaSortSubset* subset = *it;

				i3AlphaSortNodeSet* node_set = subset->m_pNodeSet;
				
				if (node_set != nullptr)
				{
					const i3AlphaOBB& obb = node_set->m_OBB;
					float rayLen = i3Vector::Length(&obj->m_PosFromView);
					VEC3D rayDir = obj->m_PosFromView;
					rayDir.x /= rayLen;		rayDir.y /= rayLen;		rayDir.z /= rayLen;
					
					if ( RaySegOBB(rayDir, rayLen, obb) == false )		// 파티클이 오브젝트뒤로 걸쳐져 있지 않다면 다시 한칸 나중에 그리도록 조절한다.
					{
						++it;

						if (it == AlphaSortSubsetList.end())
						{
							i3AlphaSortSubset* backSubset = AlphaSortSubsetList.back();
							backSubset->m_ParticleObjList.push_back(obj);
						}
						else
						{
							subset = *it;
							subset->m_ParticleObjList.push_back(obj);
						}
					}
					else
					{
						subset->m_ParticleObjList.push_back(obj);
					}
				}
				else
				{
					subset->m_ParticleObjList.push_back(obj);
				}
			}
		}
	}

	void _PrepareSubsetForSort(i3::vector<i3ParticleSubsetForSort*>& ParticleSubsetPool, i3AlphaSortSubset* alpha_subset)
	{
		// 1. 여기서 소팅..
		// 2. 소팅 후 적절이 파티클 드로잉 서브셋 구축..
		// 3. 구축된 서브셋으로 적당이 렌더링..

		i3::vector<I3PARTICLEOBJECT*>& objList = alpha_subset->m_ParticleObjList;
		i3::vector<i3ParticleSubsetForSort*>& currParticleSubsetList = alpha_subset->m_ParticleSubsetList;

		const size_t num_obj = objList.size();

		i3ParticleSubsetForSort* currSubset = nullptr;
		i3ParticleSubsetForSort* oldSubset  = nullptr;

		for (size_t i = 0; i < num_obj; ++i)
		{
			I3PARTICLEOBJECT* obj = objList[i];

			i3Particle* Owner     = obj->m_Owner;
			i3ParticleInfo* pInfo = Owner->GetParticleInfo();
			i3ParticleGroup*  pGroup  = Owner->getParticleGroup();


			switch( pInfo->GetType() )
			{
			case i3ParticleInfo::PARTICLE_POINTSPRITE :
				{
					currSubset = _QueryParticleSubset(ParticleSubsetPool, currParticleSubsetList, oldSubset, pGroup);

					INT32 idx = pGroup->AddForQuad(&obj->m_Pos[0], &obj->m_Scale, obj->m_pTransMtx);

					if (idx != -1)
					{
						pGroup->SetColor( idx, obj->m_Color);
						if( Owner->isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							pGroup->SetTextureCoord( idx, &obj->m_UV1, &obj->m_UV2);

						INT32 oldPrimCount = currSubset->m_pSubsetAttr->GetPrimitiveCount();	
						currSubset->m_pSubsetAttr->SetPrimitiveCount(oldPrimCount + 2);		// 쿼드당 폴리건 2개..
					}
					else if ( oldSubset != currSubset )
					{
						currSubset = _CancelLastParticleSubset(ParticleSubsetPool, currParticleSubsetList);
					}
				}
				break;

			case i3ParticleInfo::PARTICLE_SPRITE:
				{
					currSubset = _QueryParticleSubset(ParticleSubsetPool, currParticleSubsetList, oldSubset, pGroup);

					INT32 idx = pGroup->AddForSprite( &obj->m_Pos[0], &obj->m_Scale, obj->m_RotateAccm, 
						Owner->isParticleStyle( I3_PARTICLE_STYLE_FACETOCAM_ENABLE), obj->m_pTransMtx);

					if( idx != -1)
					{
						pGroup->SetColor( idx, obj->m_Color);

						if( Owner->isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							pGroup->SetTextureCoord( idx, &obj->m_UV1, &obj->m_UV2);

						INT32 oldPrimCount = currSubset->m_pSubsetAttr->GetPrimitiveCount();	
						currSubset->m_pSubsetAttr->SetPrimitiveCount(oldPrimCount + 2);		// 쿼드당 폴리건 2개..
					}
					else if (oldSubset != currSubset )
					{
						currSubset = _CancelLastParticleSubset(ParticleSubsetPool, currParticleSubsetList);
					}
				}
				break;

			case i3ParticleInfo::PARTICLE_BILLBOARD:
				{
					currSubset = _QueryParticleSubset(ParticleSubsetPool, currParticleSubsetList, oldSubset, pGroup);

					VEC3D diff;

					i3Vector::Sub( &diff, &obj->m_Pos[0], &obj->m_Pos[1]);
					i3Vector::Scale( &diff, &diff, pInfo->GetScaleBillboardFactor());
					i3Vector::Add( &diff, &diff, &obj->m_Pos[0]);

					INT32 idx = pGroup->AddForBillboard( &obj->m_Pos[0], &diff, &obj->m_Scale, obj->m_pTransMtx);

					if( idx != -1)
					{
						pGroup->SetColor( idx, obj->m_Color);

						if( Owner->isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							pGroup->SetTextureCoordForBillboard( idx, &obj->m_UV1, &obj->m_UV2);

						INT32 oldPrimCount = currSubset->m_pSubsetAttr->GetPrimitiveCount();	
						currSubset->m_pSubsetAttr->SetPrimitiveCount(oldPrimCount + 2);		// 쿼드당 폴리건 2개..
					}
					else if ( oldSubset != currSubset)
					{
						currSubset = _CancelLastParticleSubset(ParticleSubsetPool, currParticleSubsetList);
					}
				}
				break;

			case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :	
				{
					currSubset = _QueryParticleSubset(ParticleSubsetPool, currParticleSubsetList, oldSubset, pGroup);

					INT32 idx = pGroup->AddForBillboard( & obj->m_Pos[1], &obj->m_Pos[0], &obj->m_Scale, obj->m_pTransMtx);

					if( idx != -1)
					{
						pGroup->SetColor( idx, obj->m_Color);

						if( Owner->isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							pGroup->SetTextureCoordForBillboard( idx, &obj->m_UV1, &obj->m_UV2);

						INT32 oldPrimCount = currSubset->m_pSubsetAttr->GetPrimitiveCount();	
						currSubset->m_pSubsetAttr->SetPrimitiveCount(oldPrimCount + 2);		// 쿼드당 폴리건 2개..
					}
					else if (oldSubset != currSubset)
					{
						currSubset = _CancelLastParticleSubset(ParticleSubsetPool, currParticleSubsetList);
					}
				}
				break;

			case i3ParticleInfo::PARTICLE_DECAL:		// 데칼의 경우 Particle쪽 업데이트와 분할 구현되어야함...
				{
					i3ParticleGroupForTriangles* pDecalGroup = i3::same_cast<i3ParticleGroupForTriangles*>(pGroup);
					I3ASSERT(pDecalGroup != nullptr);
					i3DecalDrawInfo*  decalInfo = Owner->GetDecalDrawInfo(); 

					if ( decalInfo != nullptr)
					{
						if ( decalInfo->isEnable() )
						{
							currSubset = _QueryParticleSubset(ParticleSubsetPool, currParticleSubsetList, oldSubset, pGroup);

							INT32 oldTriCount = pDecalGroup->GetTriangleCount();
							pDecalGroup->AddForDecal( decalInfo, obj->m_Color, obj->m_pTransMtx);
							INT32 newTriCount = pDecalGroup->GetTriangleCount();

							if (newTriCount > oldTriCount )
							{
								INT32 oldPrimCount = currSubset->m_pSubsetAttr->GetPrimitiveCount();	
								currSubset->m_pSubsetAttr->SetPrimitiveCount( oldPrimCount + newTriCount - oldTriCount );
							}
							else if (oldSubset != currSubset)
							{
								currSubset = _CancelLastParticleSubset(ParticleSubsetPool, currParticleSubsetList);
							}
						}
					}
				}
				break;
			}

			oldSubset = currSubset;			// 루프마다 항상 이것을 지나야함..
		}

		// i3ParticleInfo::PARTICLE_SCENE 의 경우 파티클 그룹이 들어가는 여지가 없어보인다..(아마도 씬렌더링에 그대로 종속되는 듯)
	}


	void	_RenderByAlphaSortSubset(i3SceneTracer* pTracer, i3ModelViewMatrixAttr* pWorldMtxAttr, i3AlphaSortSubset* pAlphaSortSubset)
	{
		i3ParticleGroup* pGrp;

		i3AttrStackManager * pManager = pTracer->GetAttrStackManager();
		i3GeometrySubsetAttr* geomSubsetAttr = nullptr;

		i3::vector<i3ParticleSubsetForSort*>& particle_subset_list = pAlphaSortSubset->m_ParticleSubsetList;

		if ( !particle_subset_list.empty() )
		{
			pManager->PushAttr( pWorldMtxAttr);

			const size_t num_curr_subset = particle_subset_list.size();

			for (size_t j = 0 ; j < num_curr_subset ; ++j )
			{
				i3ParticleSubsetForSort* subset = particle_subset_list[j];

				pGrp = subset->m_pGroup;
				geomSubsetAttr = subset->m_pSubsetAttr;

				pGrp->RenderBySort( pTracer, geomSubsetAttr);
			}

			pManager->PopAttr( pWorldMtxAttr->GetID());
		}

		// 그다음 노드 렌더링..

		i3AlphaSortNodeSet* node_set = pAlphaSortSubset->m_pNodeSet;

		if ( node_set != nullptr)
		{
			i3RenderAttrListAttr* attr = node_set->m_NodeAttrList;

	//		const size_t num_default = node_set->m_startNodeAttrIndex;

			i3::vector<i3RenderAttr*>& attr_list = attr->GetAttrList();
			
			const size_t num_attr = attr_list.size();

			for (size_t i = 0 ; i < num_attr; ++i)
			{
				i3RenderAttr* attr = attr_list[i];

				if (attr->GetID() == I3_ATTRID_GEOMETRY )
				{
					pManager->FlushPushedAttrs(pTracer->GetOutput());
					pTracer->AddAttributeToOutput(attr);
				}
				else
				{
					pManager->PushAttr(attr);
				}
			}
			
			for (size_t i = 0; i <num_attr ;++i)
			{
				i3RenderAttr* attr = attr_list[i];

				if (attr->GetID() != I3_ATTRID_GEOMETRY )
				{
					pManager->PopAttr(attr->GetID());
				}
			}


//			pManager->FlushPushedAttrs(pTracer->GetOutput());
/*
			for (size_t j = 0; j < num_default ; ++j)
			{
			//	i3::vector<i3RenderAttr*>& attr_list = attr->GetAttrList();
				i3RenderAttr* pPrevNodeAttr =	attr_list[j];
				i3AttrStack* pStack = pManager->GetAttrStackByID(pPrevNodeAttr->GetID());
				i3RenderAttr* pLastAttr = pStack->GetTop();
				if (pLastAttr)
					attr->AppendAttr(pLastAttr);
			}

			pTracer->AddAttributeToOutput(attr);
			pManager->FlushPushedAttrs(pTracer->GetOutput());		// 알파메시쪽은 스택매니저를 사용하지 않고,
*/																	// 상태변경되기 때문에, 스택의 변경리스트를 리셋해두어야함..
		}
	}
	
	void		_PushWorldRenderAttrsToStackManager(i3SceneTracer* pTracer, const i3::vector<i3RenderAttr*>& vecAttr)
	{
		if (vecAttr.empty())
			return;

		i3AttrStackManager* pStackManager =	pTracer->GetAttrStackManager();

		const size_t num = vecAttr.size();

		for (size_t i = 0; i < num; ++i)
		{
			pStackManager->PushAttr(vecAttr[i]);
		}
	}

	void		_PopWorldRenderAttrsFromStackManager(i3SceneTracer* pTracer, const i3::vector<i3RenderAttr*>& vecAttr)
	{
		if (vecAttr.empty())
			return;

		i3AttrStackManager* pStackManager = pTracer->GetAttrStackManager();
		const size_t num = vecAttr.size();
		
		for (size_t i = 0; i < num; ++i)
		{
			UINT32 attrID = vecAttr[i]->GetID();
			pStackManager->PopAttr(attrID);
		}
	}

}


i3ParticleRender::i3ParticleRender(void)
{
	g_pParticleRender = this;

	m_pWorldMtxAttr = i3ModelViewMatrixAttr::new_object();
	m_pFogEnableAttr = i3FogEnableAttr::new_object();
	m_pFogEnableAttr->Set(false);
	
	m_pDecalManager = i3DecalManager::new_object();
	m_pDecalManager->Create( this, I3SG_DECAL_MAX_COUNT);
	
	_ReserveParticleSubset(m_ParticleSubsetPool, 1024);			// 넉넉이 확보하는게 좋을것 같다...(2인에서 대략 300개 수준이었음)
	_ReserveAlphaSortSubset(m_AlphaSortSubsetPool, 256);
}

i3ParticleRender::~i3ParticleRender(void)
{
	i3ParticleRender::RemoveWorldRenderAttrsFromEffect();

	RemoveAllGroups();


	I3_SAFE_RELEASE(m_pFogEnableAttr);
	I3_SAFE_RELEASE( m_pWorldMtxAttr);

	I3_SAFE_RELEASE( m_pDecalManager);
	
	_DestroyParticleSubsetList_InDestructor(m_ParticleSubsetListForDelete);
	_DestroyParticleSubsetList_InDestructor(m_ParticleSubsetPool);

	_DestroyAlphaSortSubsetList_InDestructor(m_AlphaSortSubsetPool, m_AlphaSortSubsetList);

	i3::cu::for_each_delete_clear(m_ParticleMatrixListForDelete);
}

bool i3ParticleRender::Create(void)
{
	// 여러 번 호출될 수 있다.
	return true;
}

void i3ParticleRender::CreatePhysXShapes( void)
{
	if( m_pDecalManager != nullptr)
	{
		m_pDecalManager->CreatePhysXBox( this);
	}
}

void i3ParticleRender::ReleasePhysXShapes( void)
{
	if( m_pDecalManager != nullptr)
	{
		m_pDecalManager->ReleasePhysXBox();
	}
}

void i3ParticleRender::RemoveAllGroups(void)
{
	i3ParticleGroup * pGroup;

	for(size_t i = 0; i < m_GroupList.size(); i++)
	{
		pGroup = m_GroupList.get_vector()[i];
		I3_MUST_RELEASE( pGroup);
	}

	m_GroupList.clear();
}

void i3ParticleRender::RemoveGroup( i3ParticleGroup * pGroup)
{
	I3ASSERT( pGroup != nullptr);

	i3::vsu::linear_remove_1st(m_GroupList, pGroup);

	I3_MUST_RELEASE( pGroup);
}

bool i3ParticleRender::AddParticleScene( i3TimeSequence * pNode, INT32 maxCount, INT32 priority, bool bDecal)
{
	i3TimeSequenceInfo * pInfo;
	INT32 i;

	for( i = 0; i < pNode->GetSequenceCount(); i++)
	{
		pInfo = pNode->GetSequence( i);

		if( pInfo->GetNode() == nullptr)
			continue;

		if( i3::same_of<i3Particle*>(pInfo->GetNode()))
		{
			i3Particle * pParticle = (i3Particle *) pInfo->GetNode();

			pParticle->setPriority( priority + i);

			if( bDecal)
			{
				// Triangle Count로 계산하므로 2배 이상 설정해야 합니다. swoongoo
				// decal 면이 4개 이상 나오는 경우가 있습니다. 최소 8 ~12 사이의 갯수로 생성할 필요가 있음.
				AddParticle( pParticle, maxCount * 8 * pParticle->GetParticleInfo()->GetMaxObjectCapacity() );
			}
			else
				AddParticle( pParticle, maxCount * pParticle->GetParticleInfo()->GetMaxObjectCapacity() );
		}
	}

	return true;
}

//
i3ParticleGroup * i3ParticleRender::AddParticle( i3Particle * pEff, INT32 maxCount)
{
	i3ParticleGroup * pGroup;

	if( pEff->getParticleGroup() != nullptr)
	{
		pGroup = pEff->getParticleGroup();

		pGroup->setPriority( pEff->getPriority());

		if( pEff->isParticleStyle( I3_PARTICLE_STYLE_IS_DECAL) )
		{
			if( i3::same_of<i3ParticleGroup*>(pEff->getParticleGroup()))
			{
				i3ParticleGroup * pOldGroup = pEff->getParticleGroup();

				// Triangle Mesh를 다루는 Particle Group으로 변경하여야 하기 때문에...
				pGroup = i3ParticleGroupForTriangles::new_object();

				pGroup->Create( pEff, maxCount);
				pGroup->setPriority( pEff->getPriority());

				// m_GroupList.Remove( pOldGroup);
				i3::vsu::linear_remove_1st( m_GroupList, pOldGroup );
				m_GroupList.insert( pGroup);

				I3_MUST_RELEASE( pOldGroup);
			}
		}
	}
	else
	{
		if( pEff->isParticleStyle( I3_PARTICLE_STYLE_IS_DECAL))
			pGroup = i3ParticleGroupForTriangles::new_object();
		else
			pGroup = i3ParticleGroup::new_object();

		pGroup->Create( pEff, maxCount);

		pGroup->setPriority( pEff->getPriority());

		m_GroupList.insert( pGroup);
	}

	if( pEff->isParticleStyle( I3_PARTICLE_STYLE_IS_DECAL))
	{
		pEff->GetParticleInfo()->SetType( i3ParticleInfo::PARTICLE_DECAL);
	}

	if( pGroup->getCapacity() < maxCount)
		pGroup->SetCapacity( maxCount);

	return pGroup;
}

i3ParticleGroup *	i3ParticleRender::FindParticleGroup( i3Particle * pParticle)
{
#if 0
	INT32 i;
	i3ParticleGroup * pGroup;

	for( i = 0; i < m_GroupList.GetCount(); i++)
	{
		pGroup = (i3ParticleGroup *) m_GroupList.Items[i];

		if( pGroup->IsSame( pParticle))
			return pGroup;
	}
#endif
	return nullptr;
}

i3ParticleTransformMatrix*	i3ParticleRender::AllocParticleTransformMatrix(MATRIX * pWorldMtx, MATRIX * pViewMtx)
{
	i3ParticleTransformMatrix* new_mat = new i3ParticleTransformMatrix;

	i3Matrix::Copy( &new_mat->m_WorldTx, pWorldMtx);
	i3Matrix::Mul( &new_mat->m_invCamTx, pWorldMtx, pViewMtx);
	i3Matrix::Transpose3x3( &new_mat->m_invCamTx, &new_mat->m_invCamTx);

	m_ParticleMatrixListForDelete.push_back(new_mat);

	return new_mat;				
}

void		i3ParticleRender::AddParticleObject(I3PARTICLEOBJECT* obj)
{
	if (obj->m_PosFromView.z >= 0.f)					// 음수인 경우 넣도록 함.. (DX좌표계의 반대임)
		return;
	
	if (obj->m_Owner->getDestBlend() != I3G_BLEND_ONE)		// 가산 블랜딩은 따로 처리한다. (가산블랜딩은 거리소팅하지 않고 기존 우선순위소팅처리한다..)
		m_currAccObjectList.push_back(obj);
	else
		m_currAccObjectList_AddBlend.push_back(obj);			
}



// 먼저 알파소트 노드부터 소팅을 걸어두고 순서를 정함.. (비교로 minDepth를 쓴다..)
// 이미 소팅된 m_currAccObjectList를 갖고 먼것부터 가까운 것까지 파티클을 하나씩 넣어본다...
//

void i3ParticleRender::Render( i3SceneTracer * pTracer)
{
	if ( g_pRenderContext->IsRenderFlag(I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) )		// 이 경우 진행하지 않아본다..
		return;

	_SortAlphaSortNode(pTracer, m_AlphaSortSubsetPool, m_AlphaSortSubsetList);

	_DistributeObjectsIntoSubset(m_AlphaSortSubsetList, m_currAccObjectList, m_currAccObjectList_AddBlend);

	const size_t num_alpha_set = m_AlphaSortSubsetList.size();

	for (size_t i = 0 ; i < num_alpha_set ; ++i)
	{
		_PrepareSubsetForSort(m_ParticleSubsetPool, m_AlphaSortSubsetList[i]);
	}
	
	_PushWorldRenderAttrsToStackManager(pTracer, m_vecWorldRenderAttrForEffect);
	
	for (size_t i = 0 ; i < num_alpha_set ; ++i )
	{
		_RenderByAlphaSortSubset(pTracer, m_pWorldMtxAttr, m_AlphaSortSubsetList[i]);
	}
	
	i3AttrStackManager * pManager = pTracer->GetAttrStackManager();
	pManager->FlushPushedAttrs(pTracer->GetOutput());

	m_pDecalManager->ProcessDecal( pTracer);

	_PopWorldRenderAttrsFromStackManager(pTracer, m_vecWorldRenderAttrForEffect);
	
	i3ParticleRender::ClearParticles();
}

void		i3ParticleRender::ClearParticles()
{
	// 렌더링 반영 종료시... 클리어 처리함...
	_ClearAlphaSortSubsetList(m_AlphaSortSubsetPool, m_ParticleSubsetListForDelete, m_AlphaSortSubsetList);
	m_currAccObjectList.clear();			// 기존 있는 포인터로만 운용..벡터 클리어만 수행..
	m_currAccObjectList_AddBlend.clear();
	i3::cu::for_each_delete_clear(m_ParticleMatrixListForDelete);
}

void	i3ParticleRender::PostTrace()
{
	i3ParticleGroup * pGrp;
	const std::size_t numGroup = m_GroupList.size();

	for(size_t i = 0; i < numGroup; ++i)
	{
		pGrp = m_GroupList.get_vector()[i];
		pGrp->PostTrace();
	}
}

void		i3ParticleRender::PostRender()
{
	_ResetParticleSubsetList(m_ParticleSubsetPool, m_ParticleSubsetListForDelete);
	
}


I3SG_DECAL_HITBOX * i3ParticleRender::EmitDecal( i3SceneTracer * pTracer, i3Particle * pParticle, I3PARTICLEOBJECT * pObj)
{
	return m_pDecalManager->EmitDecal( pTracer, pParticle, pObj);
}

void i3ParticleRender::FreeDecal( i3Decal * pDecal)
{
	m_pDecalManager->FreeDecal( pDecal);
}

void i3ParticleRender::FreeDrawDecal( i3DecalDrawInfo * pInfo)
{
	m_pDecalManager->FreeDrawDecalInfo( pInfo);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 데칼용 이벤트입니다.
// Physix의 ContactReport가 발생하면 호출됩니다.
// 해당 Decal의 Receiver를 EffectManager로 설정해야됩니다.
bool i3ParticleRender::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
#if defined( I3_PHYSX)

	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_CONTACT )
		{
			if( !i3::same_of<i3PhysixShapeSet*>(pObj))
				return true;

			m_pDecalManager->ProcessEventContactHitBox( (i3PhysixShapeSet*) pObj);
		}
	}
#endif

	return i3EventReceiver::OnEvent( event, pObj, param2, code);
}


void i3ParticleRender::ArrangeRenderOrder(void)
{
//	m_GroupList.Sort( (COMPAREPROC)_ParticleGroupCompareProc);
	i3::sort( m_GroupList.begin(), m_GroupList.end(), i3::less<i3ParticleGroup*>());
//
}


void i3ParticleRender::OnLostDevice( bool bLostDevice)
{
	i3ParticleRender::ClearParticles();
	i3ParticleRender::PostTrace();
	i3ParticleRender::PostRender();
	//

	i3ParticleGroup * pGroup;

	for(size_t i = 0; i < m_GroupList.size(); i++)
	{
		pGroup = m_GroupList.get_vector()[i];

		pGroup->OnLostDevice( bLostDevice);
	}
}

void i3ParticleRender::OnRevive( i3RenderContext * pCtx)
{

	i3ParticleGroup * pGroup;

	for(size_t i = 0; i < m_GroupList.size(); i++)
	{
		pGroup = m_GroupList.get_vector()[i];

		pGroup->OnRevive( pCtx);
	}
}

#if defined( I3_DEBUG)
void i3ParticleRender::Dump(void)
{
	
	i3ParticleGroup * pGroup;

	for(size_t i = 0; i < m_GroupList.size(); i++)
	{
		pGroup = m_GroupList.get_vector()[i];

		I3TRACE( "[%d] : %s\n", pGroup->getPriority(), pGroup->getParticle()->GetName());
	}
}
#endif

//  i3RenderAttr의 복수 연결이 가능하다고 가정하고 다음과 같이 AddRef() 대입으로 처리..
void i3ParticleRender::AddWorldRenderAttrToEffect(i3RenderAttr* worldRenderAttr)
{
	if (worldRenderAttr == nullptr)
		return;
	worldRenderAttr->AddRef();
	m_vecWorldRenderAttrForEffect.push_back(worldRenderAttr);
}

void	i3ParticleRender::RemoveWorldRenderAttrsFromEffect()
{
	i3::cu::for_each_SafeRelease_clear(m_vecWorldRenderAttrForEffect);
}

void	i3ParticleRender::PushAttr_FogDisable(i3AttrStackManager* pStackManager)
{
	pStackManager->PushAttr(m_pFogEnableAttr);
}

void	i3ParticleRender::PopAttr_FogDisable(i3AttrStackManager* pStackManager)
{
	pStackManager->PopAttr(m_pFogEnableAttr->GetID());
}
