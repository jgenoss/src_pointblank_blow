#include "pch.h"
#include "AvatarGen.h"
#include "AvatarDefine.h"

#if defined( CLIENT_SIDE)
#include "../../Source/Avatar/AvatarManager.h"
#endif

#include "i3Base/string/ext/format_string.h"


bool AvatarGen::PrepareGeo( INT32 lod)
{
	// 전체 Mesh들을 검색해, 적절한 Vertex Format 및 Vertex Array를 생성한다.
	UINT32 i;
	INT32 part, vaCount = 0, iaCount = 0;
	INT32 maxRefBone = 1;
	i3::vector<i3Node*>	list;
	i3::vector<i3Node*>	attrSetlist;
	
	list.reserve( 128);
	attrSetlist.reserve( 128);

	I3_SAFE_RELEASE( m_pTempVA[ lod]);
	I3_SAFE_RELEASE( m_pTempIA[ lod]);

	for( part = 0; part < EQUIP::ePART_COUNT; part++)
	{
		if( AVATAR::getPartStyle( (EQUIP::ePART) part) & AVT_PART_STYLE_SKIN)
		{
			i3SceneGraphInfo * pSgInfo = getSgInfo( (EQUIP::ePART) part, lod);

			if( pSgInfo == nullptr)
				continue;

			if( pSgInfo->getInstanceSg() == nullptr)
				continue;
			
			i3Scene::FindNodeByType( pSgInfo->getInstanceSg(), i3AttrSet::static_meta(), attrSetlist);
		}
	}

	UINT32 cnt = attrSetlist.size();
	for( i = 0; i < cnt; i++)
	{
		i3AttrSet * pAttrSet = (i3AttrSet*) attrSetlist.at( i);

		if( _CanMergeGeometry( pAttrSet) == false)
			continue;

		if( i3Scene::FindAttrByExactType( pAttrSet, i3AlphaTestEnableAttr::static_meta()) )
		{
			i3AlphaFuncAttr * pAttr = static_cast<i3AlphaFuncAttr*>(i3Scene::FindAttrByExactType( pAttrSet, i3AlphaFuncAttr::static_meta()));
			if( pAttr != nullptr)
			{
				if( m_AlphaTestFuncValue > pAttr->GetRefValue())
					m_AlphaTestFuncValue = pAttr->GetRefValue();
			}

			m_bAlphaTestableMesh = true;
		}

		i3Scene::FindNodeByType( pAttrSet, i3Geometry::static_meta(), list);

	}

	I3ASSERT( list.size() > 0);

	cnt = list.size();
	for( i = 0; i < cnt; i++)
	{
		i3Geometry * pGeo = (i3Geometry*) list.at(i);

		INT32 geoCnt = pGeo->GetGeometryAttrCount();
		for( INT32 j = 0; j < geoCnt; j++)
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr(j);

			i3VertexArray * pVA = pGeoAttr->GetVertexArray();
			i3IndexArray * pIA = pGeoAttr->GetIndexArray();

			vaCount += pVA->GetCount();
			maxRefBone = MAX( maxRefBone, pVA->GetFormat()->GetBlendIndexCount());

			I3ASSERT( pIA != nullptr);
			iaCount += pIA->GetCount();
		}
	}

	if( vaCount <= 0)
		return false;

	// 실질적인 Geometry 생성
	{
		i3VertexFormat fmt;

		fmt.SetHasPosition();
		fmt.SetHasNormal();
		fmt.SetHasTangent();
		fmt.SetHasBinormal();
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);
		fmt.SetBlendIndexCount( (UINT8)maxRefBone);
		fmt.SetBlendWeightCount( (UINT8)maxRefBone - 1);

		if( m_bInThread)
		{
			m_pTempVA[ lod] = i3VertexArrayMem::new_object();
		}
		else
		{
			m_pTempVA[ lod] = i3VertexArray::new_object();
			m_pTempVA[ lod]->setRestoreType( I3G_RESTORE_MEM);
		}
		
		m_idxVA[ lod] = 0;
		m_pTempVA[ lod]->Create( &fmt, vaCount, 0);
	}

	{
		if( m_bInThread)
		{
			m_pTempIA[ lod] = i3IndexArrayMem::new_object();
		}
		else
		{
			m_pTempIA[ lod] = i3IndexArray::new_object();
			m_pTempIA[ lod]->setRestoreType( I3G_RESTORE_MEM);
		}
		I3ASSERT(iaCount > 0);
		m_pTempIA[ lod]->Create( iaCount, 0, m_pTempVA[lod]->GetCount());
		m_idxIA[ lod] = 0;
	}

	return true;
}

bool AvatarGen::isSameBoneStruct( i3Skeleton * pBaseSkel, i3Skeleton * pSkel)
{
	if( pBaseSkel->getBoneCount() != pSkel->getBoneCount())
	{
		return false;
	}

	INT32 boneCnt = pBaseSkel->getBoneCount();
	for( INT32 i = 0; i < boneCnt; i++)
	{
		i3Bone * pBaseBone = pBaseSkel->getBone( i);
		i3Bone * pPartBone = pSkel->getBone( i);

		if( i3::generic_is_iequal( pBaseBone->getName(), pPartBone->getName()) == false)
			return false;
	}

	return true;
}


namespace
{
	UINT32 CopyVertexArray( i3VertexArray * pDestVA, i3VertexArray * pSrcVA, const INT32 * boneMap, UINT32 start, UINT32 end, bool bMultiThread)
	{
		UINT32 i;

		VEC3D vec3;
		VEC2D vec2;

		I3ASSERT( pSrcVA != nullptr);
		I3ASSERT( pDestVA != nullptr);

		i3VertexFormat * pSrcFmt = pSrcVA->GetFormat();

		// pSrcVA는 DX, Mem 둘다 가능
		if( bMultiThread )
			pSrcVA->SafeLock( I3G_LOCK_READONLY);
		else
			pSrcVA->Lock( I3G_LOCK_READONLY);
		pDestVA->Lock();

		UINT32 cnt = 0;

		UINT32 srcVACnt = pSrcVA->GetCount();

		for( i = start; i < start + end && i < srcVACnt; i++)
		{
			// Pos
			pSrcVA->GetPosition( i, &vec3);
			pDestVA->SetPosition( i, &vec3);

			// Normal
			pSrcVA->GetNormal( i, &vec3);
			pDestVA->SetNormal( i, &vec3);

			// Texture Coords.
			pSrcVA->GetTextureCoord( 0, i, &vec2);
			pDestVA->SetTextureCoord( 0, i, &vec2);

			{
				UINT8 idxOldBone;
				REAL32 w;

				// Blend Index
				UINT8 blendCnt = (UINT8)pSrcFmt->GetBlendIndexCount();
				for( UINT8 j = 0; j < blendCnt; j++)
				{
					//로드될 본인덱스
					idxOldBone = (UINT8) pSrcVA->GetBlendIndex( j, i);

					// 현재 Part의 Bone 구조에 대한 Index를
					// 현재 LOD Upper Bone Hierarchy에서의 Index로 변환
					I3ASSERT( idxOldBone < 0xFF);
					I3ASSERT( boneMap[ idxOldBone] != -1);

					pDestVA->SetBlendIndex( j, i, boneMap[ idxOldBone]);
				}

				blendCnt = pSrcFmt->GetBlendWeightCount();
				for( UINT8 j = 0; j < blendCnt; j++)
				{
					w = pSrcVA->GetBlendWeight( j, i);

					pDestVA->SetBlendWeight( j, i, w);
				}
			}

			cnt++;
		}

		if( pSrcVA->HasTangent() && pDestVA->HasTangent() )
		{
			for( i = 0; i < srcVACnt; i++)
			{
				pSrcVA->GetTangent( i, &vec3);
				pDestVA->SetTangent( i, &vec3);

				pSrcVA->GetBinormal( i, &vec3);
				pDestVA->SetBinormal( i, &vec3);
			}
		}

		if( bMultiThread)
			pSrcVA->SafeUnlock();
		else
			pSrcVA->Unlock();
		
		pDestVA->Unlock();

		return cnt;
	}

	void CopyIndexArray( i3IndexArray * pDestIA, i3IndexArray * pSrcIA, bool bMultiThread)
	{
		I3ASSERT( pDestIA != nullptr);
		I3ASSERT( pSrcIA != nullptr);

		// Indexed Geometry
		if( bMultiThread)
			pSrcIA->SafeLock( 0, 0, I3G_LOCK_READONLY);
		else
			pSrcIA->Lock( 0, 0, I3G_LOCK_READONLY);
		pDestIA->Lock();

		if( pSrcIA->GetDataType() == I3G_IDX_16BITS)
			pDestIA->SetIndices( 0, pSrcIA->GetCount(), (UINT16*) pSrcIA->GetData());
		else
			pDestIA->SetIndices( 0, pSrcIA->GetCount(), (UINT32*) pSrcIA->GetData());

		if( bMultiThread)
			pSrcIA->SafeUnlock();
		else
			pSrcIA->Unlock();
		pDestIA->Unlock();
	}
}

bool AvatarGen::_CanMergeGeometry( i3AttrSet * pAttrSet)
{
	I3ASSERT( pAttrSet != nullptr);

	char szName[ 260] = {0,};
	i3::safe_string_copy( szName, pAttrSet->GetName(), sizeof( szName));
	i3::to_upper( szName);

	return !((i3::contain_string( szName, "MODEL_CLAN") >= 0) ||		// Clan Mark는 바로 카피
		(i3::contain_string( szName, "TEAMBAND_") >= 0) ||				// team band도 카피
		//(i3Scene::FindAttrByExactType( pAttrSet, i3AlphaTestEnableAttr::static_meta())) ||
		(i3Scene::FindAttrByExactType( pAttrSet, i3BlendEnableAttr::static_meta())));		// AlphaTest나 Blend가 있다면 Merge하지 않는다.
}

bool AvatarGen::_CanMergeTexture( i3AttrSet * pAttrSet, bool bInLobby, bool bSkin /*= false*/)
{
	I3ASSERT( pAttrSet != nullptr);

	if( pAttrSet->IsFlag( I3_NODEFLAG_USER))
	{
		if( bInLobby || bSkin) return false;
	}

	char szName[ 260] = {0,};
	i3::safe_string_copy( szName, pAttrSet->GetName(), sizeof( szName));
	i3::to_upper( szName);

	return !((i3::contain_string( szName, "MODEL_CLAN") >= 0) || (i3::contain_string( szName, "TEAMBAND_") >= 0));
}

void AvatarGen::_BuildBoneMap( AVT_BUILD_CTX * pCtx)
{
	INT32 i;
	i3Skeleton * pSkelOld = nullptr;

	/////////
	// 기준이 되는 스켈레톤(FrameLOD) 새로 생성한 LOD
	i3Skeleton * pSkelNew = pCtx->m_pDestLOD->getSkeleton();
	
	{
		// Skin
		if( pCtx->m_pSrcBody == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Skin으로 제작되지 않았습니다.");
			return;
		}

		/////////
		//로드 될 파츠의 바디..
		i3LOD * pSrcLOD = pCtx->m_pSrcBody->getFrameLOD();
		if( pSrcLOD != nullptr)
		{
			I3ASSERT( pSrcLOD->getBoneMap() == nullptr);
			pSkelOld = pSrcLOD->getSkeleton();
		}

		I3ASSERT_RETURN( pSkelOld != nullptr);

		if( pSkelOld->getBoneCount() > pSkelNew->getBoneCount())
		{
			I3PRINTLOG(I3LOG_FATAL,  "Upper가 아닌 Part에서 더 많은 Bone이 발견되었습니다. Upper의 Bone 구조를 확인해 주세요.");
			return;
		}

		// Bone Map 작성
		// 로드될 본의 이름으로 
		// 기본 본의 인덱스 값을 찾는다.
		INT32 oldBoneCnt = pSkelOld->getBoneCount();
		for( i = 0; i < oldBoneCnt; i++)
		{
			i3Bone * pBone = pSkelOld->getBone( i);

			pCtx->_boneMap[i] = pSkelNew->FindBoneByName( pBone->getName());

			if( pCtx->_boneMap[i] == -1)
			{
				I3PRINTLOG(I3LOG_FATAL,  "%s Bone을 찾을 수 없습니다. Upper Part에 해당 LOD의 전체 Bone 구조가 포함되어 있는지 확인해 주세요.");
				return;
			}
		}
	}
}

void AvatarGen::AddMesh( AVT_BUILD_CTX * pCtx, i3GeometryAttr * pGeoAttr, MATRIX * pMat)
{
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	I3ASSERT( pVA != nullptr);

	if( i3::same_of<i3VertexArrayMem*>(pVA) == false)
		return;

	UINT32 i;
	i3IndexArray * pIA = pGeoAttr->GetIndexArray();
	INT32 j, destBase, idx, iaStart, idxDest;
	VEC3D vec3;
	VEC2D vec2;
	i3VertexFormat * pSrcFmt = pVA->GetFormat();
	i3VertexFormat * pDestFmt = m_pTempVA[ pCtx->m_LOD]->GetFormat();
	i3Skeleton * pSkelNew = nullptr;
	const AVT_PARTINFO * pPart = pCtx->m_pPart;
	VEC2D offset, scale;

	pSkelNew = pCtx->m_pDestLOD->getSkeleton();

	if( (pPart->m_Style & AVT_PART_STYLE_SKIN) != AVT_PART_STYLE_SKIN)
	{
		I3ASSERT_0;
		return;
	}

	{
		offset.x = (REAL32) pCtx->m_pTexInfo->m_X / AVT_TEX_WIDTH;
		offset.y = (REAL32) pCtx->m_pTexInfo->m_Y / AVT_TEX_HEIGHT;

		scale.x = (REAL32) pCtx->m_pTexInfo->m_W / AVT_TEX_WIDTH;
		scale.y = (REAL32) pCtx->m_pTexInfo->m_H / AVT_TEX_HEIGHT;
	}

	iaStart = pGeoAttr->GetStartIndex();

	bool bMultiThread = false;
#if defined( CLIENT_SIDE)
	if( pCtx->m_pAvatar->m_bInThread)
	{
		bMultiThread = pCtx->m_pAvatar->m_bInThread;// && !pCtx->m_pAvatar->m_bInLobby;
		pVA->setThreadMode( true);
	}
#endif

	if( bMultiThread )
		pVA->SafeLock( I3G_LOCK_READONLY);
	else
		pVA->Lock( I3G_LOCK_READONLY);

	{
		UINT32 vaCnt = pVA->GetCount();

		for (i = 0; i < vaCnt; i++)
		{
			idxDest = m_idxVA[pCtx->m_LOD] + i;

			// Pos
			pVA->GetPosition(i, &vec3);
			m_pTempVA[pCtx->m_LOD]->SetPosition(idxDest, &vec3);

			// Normal
			pVA->GetNormal(i, &vec3);
			m_pTempVA[pCtx->m_LOD]->SetNormal(idxDest, &vec3);

			// Texture Coords.
			pVA->GetTextureCoord(0, i, &vec2);

			// Normalize
			if (vec2.x < 0.0f)
				vec2.x = 1.0f + vec2.x;

			if (vec2.y < 0.0f)
				vec2.y = 1.0f + vec2.y;

			i3Vector::Mul(&vec2, &vec2, &scale);
			i3Vector::Add(&vec2, &vec2, &offset);
			m_pTempVA[pCtx->m_LOD]->SetTextureCoord(0, idxDest, &vec2);

			INT32 idxOldBone, idxNewBone;
			REAL32 w;

			// Blend Index
			INT8 blendCnt = pDestFmt->GetBlendIndexCount();
			for (j = 0; j < blendCnt; j++)
			{
				if (j < pSrcFmt->GetBlendIndexCount())
				{
					//로드될 본인덱스
					idxOldBone = (INT32)pVA->GetBlendIndex(j, i);

					// 현재 Part의 Bone 구조에 대한 Index를
					// 현재 LOD Upper Bone Hierarchy에서의 Index로 변환
					idxNewBone = pCtx->_boneMap[idxOldBone];
				}
				else
				{
					idxNewBone = 0;
				}


				I3ASSERT(idxNewBone != -1);

				m_pTempVA[pCtx->m_LOD]->SetBlendIndex(j, idxDest, idxNewBone);
			}

			REAL32 totalW = 1.f;

			blendCnt = pDestFmt->GetBlendWeightCount();
			for (j = 0; j < blendCnt; j++)
			{
				if (j < pSrcFmt->GetBlendWeightCount())
				{
					w = pVA->GetBlendWeight(j, i);

					totalW -= w;
				}
				else
				{
					w = totalW;
					totalW = 0.f;
				}

				m_pTempVA[pCtx->m_LOD]->SetBlendWeight(j, idxDest, w);
			}
		}
	}

	if( pVA->HasTangent() && pVA->HasBinormal())
	{
		UINT32 vaCnt = pVA->GetCount();
		for( i = 0; i < vaCnt; i++)
		{
			idxDest = m_idxVA[ pCtx->m_LOD] + i;

			pVA->GetTangent( i, &vec3);
			if( m_pTempVA[ pCtx->m_LOD]->HasTangent())
				m_pTempVA[ pCtx->m_LOD]->SetTangent( idxDest, &vec3);

			pVA->GetBinormal( i, &vec3);
			if( m_pTempVA[ pCtx->m_LOD]->HasBinormal())
				m_pTempVA[ pCtx->m_LOD]->SetBinormal( idxDest, &vec3);
		}
	}

	if( bMultiThread)
		pVA->SafeUnlock();
	else
		pVA->Unlock();

#if defined( CLIENT_SIDE)
	pVA->setThreadMode( false);
#endif

	destBase = m_idxVA[ pCtx->m_LOD];
	m_idxVA[ pCtx->m_LOD] += pVA->GetCount();

	if( pIA != nullptr)
	{
#if defined( CLIENT_SIDE)
		pIA->setThreadMode( pCtx->m_pAvatar->m_bInThread);
#endif

		// Indexed Geometry
		if( bMultiThread)
			pIA->SafeLock( 0, 0, I3G_LOCK_READONLY);
		else
			pIA->Lock( 0, 0, I3G_LOCK_READONLY);

		UINT32 iaCnt = pIA->GetCount();
		for( i = 0; i < iaCnt; i++)
		{
			idx = pIA->GetIndex( i + iaStart);

			m_pTempIA[ pCtx->m_LOD]->SetIndex( m_idxIA[ pCtx->m_LOD] + i, destBase + idx);
		}

		if( bMultiThread)
			pIA->SafeUnlock();
		else
			pIA->Unlock();

#if defined( CLIENT_SIDE)
		pIA->setThreadMode( false);
#endif

		m_idxIA[ pCtx->m_LOD] += pIA->GetCount();

		pCtx->m_PrimCount += (pIA->GetCount() / 3);
	}
	else
	{
		UINT32 vaCnt = pVA->GetCount();
		for( i = 0; i < vaCnt; i++)
		{
			idx = i + iaStart;
			m_pTempIA[ pCtx->m_LOD]->SetIndex( m_idxIA[ pCtx->m_LOD] + i, destBase + idx);
		}

		m_idxIA[ pCtx->m_LOD] += pVA->GetCount();

		pCtx->m_PrimCount += (pVA->GetCount() / 3);
	}
}


I3SG_TRAVERSAL_RESULT Rec_Traverse( i3Node * pNode, I3_SG_TRAVERSE_PROC pUserProc, void * pUserData, i3MatrixStack * pStack)
{
	bool bTrans;
	I3SG_TRAVERSAL_RESULT rv;

	if( i3::same_of<i3Body*>(pNode))
	{
		i3Body * pBody = static_cast<i3Body*>(pNode);

		UINT32 lodCnt = pBody->getLODCount();
		for( UINT32 i = 0; i < lodCnt; i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);

			if( (pLOD != nullptr) && (pLOD->getShapeNode() != nullptr))
				Rec_Traverse( pLOD->getShapeNode(), pUserProc, pUserData, pStack);
		}
	}

	INT32 childCnt = pNode->GetChildCount();
	for( INT32 i = 0; i < childCnt; i++)
	{
		i3Node * pChild = pNode->GetChild(i);

		bTrans = false;

		if( pStack != nullptr)
		{
			// Matrix Stack을 Update한다.
			if( pChild->IsFlag( I3_NODEFLAG_TRANSFORM))
			{
				I3ASSERT( i3::kind_of<i3Transform*>(pChild));
				i3Transform * pTrans = static_cast<i3Transform*>(pChild);

				pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
				bTrans = true;
			}
		}

		rv = pUserProc( pChild, pUserData, pStack);

		if( rv == I3SG_TRAVERSAL_CONTINUE)
		{
			Rec_Traverse( pChild, pUserProc, pUserData, pStack);
		}

		if( bTrans)
		{
			pStack->Pop();
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void RecMergeGeo( i3Node * pRoot, I3_SG_TRAVERSE_PROC pUserProc, void * pUserData, i3MatrixStack * pStack)
{
	bool bTrans = false;
	I3SG_TRAVERSAL_RESULT rv;

	if( pStack != nullptr)
	{
		// Matrix Stack이 있다면 Update한다.
		if( pRoot->IsFlag( I3_NODEFLAG_TRANSFORM))
		{
			i3Transform * pTrans = (i3Transform *) pRoot;

			pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
			bTrans = true;
		}
	}

	rv = pUserProc( pRoot, pUserData, pStack);

	if( rv == I3SG_TRAVERSAL_CONTINUE)
		Rec_Traverse( pRoot, pUserProc, pUserData, pStack);

	if( bTrans)
	{
		pStack->Pop();
	}
}

void AvatarGen::AddMeshFunc( AVT_BUILD_CTX * pCtx, i3::vector<i3Node*> &list, i3MatrixStack * pStack)
{
	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		i3Geometry * pGeo = static_cast<i3Geometry*>( list.at(i) );

		if( (pGeo->GetFlag() & I3_NODEFLAG_USER) != I3_NODEFLAG_USER)
		{	// 머징 가능한 것
			INT32 geoCnt = pGeo->GetGeometryAttrCount();
			for( INT32 j = 0; j < geoCnt; j++)
			{
				i3RenderAttr * pAttr = pGeo->GetGeometryAttr( j);

				if( !i3::same_of<i3GeometryAttr*>(pAttr))
					continue;

				pCtx->m_pAvatar->AddMesh( pCtx, (i3GeometryAttr*)pAttr, pStack->GetTop());
 			}
		}
	}
}

I3SG_TRAVERSAL_RESULT AvatarGen::_Rec_MergeGeoProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	AVT_BUILD_CTX * pCtx = (AVT_BUILD_CTX *) pUserData;

	if( i3::same_of<i3Body*>(pNode))
	{
		pCtx->m_pSrcBody = (i3Body *) pNode;

		if( (pCtx->m_pPart->m_Style & AVT_PART_STYLE_BODY) > 0)
			pCtx->m_pAvatar->_BuildBoneMap( pCtx);
	}
	else if( i3::same_of<i3AttrSet*>(pNode) )
	{
		// i3AttrSet
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		if( _CanMergeGeometry( pAttrSet) == false)
		{	// 합치지 못한다. 그냥 복사
			i3Node * pNewNode = i3Scene::CreateClone( pNode, I3_COPY_INSTANCE);
			pCtx->m_pDestLOD->getShapeNode()->AddChild( pNewNode);

			const AVATAR::TEXINFO * pInfo = pCtx->m_pTexInfo;
			if( _CanMergeTexture( pAttrSet, pCtx->m_pAvatar->m_bInLobby, pCtx->m_pAvatar->hasSkinParts()) == false)
				pCtx->m_pTexInfo = nullptr;
#if !defined( CLIENT_SIDE)
			else
			{// 툴에서는 합치지 못하는 모델(blend, alpha)에 대해서 macrotexture를 설정
				i3AttrSet * pNewAttrSet = static_cast<i3AttrSet*>(pNewNode);
				for( INT32 i = 0; i < pNewAttrSet->GetAttrCount(); i++)
				{
					i3RenderAttr * pAttr = pNewAttrSet->GetAttr( i);
					if( i3::same_of<i3TextureBindAttr*>( pAttr))
					{
						((i3TextureBindAttr*)pAttr)->SetTexture( pCtx->m_pAvatar->getDiffuseTexture());
					}
					else if( i3::same_of<i3NormalMapBindAttr*>( pAttr))
					{
						((i3NormalMapBindAttr*)pAttr)->SetTexture( pCtx->m_pAvatar->getNormalMap());
					}
					else if( i3::same_of<i3SpecularMapBindAttr*>( pAttr))
					{
						((i3SpecularMapBindAttr*)pAttr)->SetTexture( pCtx->m_pAvatar->getSpecularMap());
					}
					else if( i3::same_of<i3ReflectMaskMapBindAttr*>( pAttr))
					{
						((i3ReflectMaskMapBindAttr*)pAttr)->SetTexture( pCtx->m_pAvatar->getReflectionMaskMap());
					}
				}
			}
#endif

			_ReplaceNewGeometry( pNewNode, pCtx);

			pCtx->m_pTexInfo = pInfo;

			return I3SG_TRAVERSAL_STOP;
		}
		else
		{
			i3::vector<i3Node*> list;
			i3Scene::FindNodeByExactType( pNode, i3Geometry::static_meta(), list);

			AddMeshFunc( pCtx, list, pStack);
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}



INT32 AvatarGen::MergeGeo( INT32 lod, EQUIP::ePART part, i3LOD * pTargetLOD)
{
	i3SceneGraphInfo * pSgInfo	= getSgInfo( part, lod);
	const AVT_PARTINFO * pPart	= AVATAR::getPartInfo( part);
	i3MatrixStack	stack;
	AVT_BUILD_CTX	ctx;
	AVATAR::VIEW view;

	if( pSgInfo == nullptr)
		return 0;

	if( lod == 0)
		view = AVATAR::VIEW_1P;
	else
		view = AVATAR::VIEW_3P;

	ctx.m_pAvatar	= this;
	ctx.m_pPart		= pPart;
	ctx.m_pTexInfo	= AVATAR::getPartTexInfo( view, part);
	ctx.m_pDestLOD	= pTargetLOD;
	ctx.m_LOD		= lod;

	RecMergeGeo( pSgInfo->getInstanceSg(), _Rec_MergeGeoProc, &ctx, &stack);

	return ctx.m_PrimCount;
}

void AvatarGen::_BuildBoneMap( i3LOD * pLOD, i3Skeleton * pFrameSkel)
{
	INT32 boneMap[512] = { 0, };

	i3Skeleton * pSkel = pLOD->getSkeleton();

	// Frame Skeleton에 비춘 Bone Map을 생성한다.
	I3ASSERT( pSkel->getBoneCount() < 512);

	bool bSame = true;
	INT32 boneCnt = pSkel->getBoneCount();
	for( INT32 i = 0; i < boneCnt; i++)
	{
		i3Bone * pBone = pSkel->getBone( i);

		INT32 idx = pFrameSkel->FindBoneByName( pBone->getName());

		if( idx == -1)
		{
			I3PRINTLOG(I3LOG_FATAL,  "LOD의 %s Bone이 Frame Skeleton에서 찾을 수 없습니다. Bone 구조를 확인해 주세요.", (const char*)pBone->getName() );
		}

		if( boneMap[i] != idx)
			bSame = false;

		boneMap[i] = idx;
	}

	if( bSame == false)
	{
		pLOD->SetBoneMapTable( boneMap, pSkel->getBoneCount());

		//Dump
#if 0
		I3TRACE( "BoneMap Dump--------\n");
		for( INT32 i = 0; i < boneCnt; i++)
		{
			I3TRACE( "%d - %d (%s)\n", i, boneMap[i], pSkel->getBone( i)->getName() );
		}
#endif
	}
	else
		pLOD->setBoneMap( nullptr);
}

i3LOD * AvatarGen::BuildLOD( INT32 lod, i3Skeleton * pFrameSkel)
{
	i3LOD * pLOD;
	i3Skeleton * pSkel;
	INT32 i, cntPrim;
	bool bResult = false;

	// Skeleton
	// 기본 Skeleton 구조는 항상 Upper Part에서 가져온다.
	{
		i3SceneGraphInfo * pSgInfo = getSgInfo( EQUIP::ePART_VEST, lod);
		if( pSgInfo == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "%d LOD용의 Upper Part가 존재하지 않습니다.", lod );
			return nullptr;
		}

		i3Body * pBody = (i3Body *) i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta());
		if( pBody == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "%d LOD : 올바른 Upper Part가 아닙니다. Skinning이 되지 않은 파일일 수 있습니다.");
			return nullptr;
		}

		pLOD = pBody->getFrameLOD();

		pSkel = pLOD->getSkeleton();

		// New LOD
		{
			m_pNewLOD = i3LOD::new_object();
			m_pNewLOD->setSkeleton( i3Skeleton::new_object_ref());
			pSkel->CopyTo( m_pNewLOD->getSkeleton(), I3_COPY_INSTANCE);
		}

		// Bone Map
		if( lod != m_FrameLODNum)			// LOD 1이 Frame Skeleton이기 때문에 BoneMap을 생성할 필요가 없다.
		{
			I3ASSERT( pFrameSkel->getBoneCount() >= pSkel->getBoneCount());
			_BuildBoneMap( m_pNewLOD, pFrameSkel);
		}
	}

	{	// 미리 만들어야 원본에서 Merge하지 않는 것을 복사할 수 있다.
		m_pNewLOD->setShapeNode( i3AttrSet::new_object_ref());
	#if defined( I3_DEBUG)
		m_pNewLOD->getShapeNode()->SetName( "Material");
	#endif
	}

	// Part별로 Merge
	{
		cntPrim = 0;

		m_pTempVA[ lod]->Lock();
		m_pTempIA[ lod]->Lock();

		for( i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			AVT_PART_STYLE style = AVATAR::getPartStyle( (EQUIP::ePART) i);
			if( style & AVT_PART_STYLE_SKIN)
				cntPrim += MergeGeo( lod, (EQUIP::ePART) i, m_pNewLOD);
		#if !defined( USE_EXCHANGE_HEAD)
			else if( style & AVT_PART_STYLE_EXCHANGE)
			{
				// 합치지 못한다. 그냥 복사
				_CopyLODShapeNode( lod, (EQUIP::ePART) i, m_pNewLOD);
			}
		#endif
		}

		m_pTempVA[ lod]->Unlock();
		m_pTempIA[ lod]->Unlock();
	}

	// Scene-Graph 구성.
	{
		i3AttrSet * pAttrSet = (i3AttrSet*) m_pNewLOD->getShapeNode();

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();			
			pAttr->setValidDiffuse(true);
			pAttrSet->AddAttr( pAttr);
		}

		if( m_bAlphaTestableMesh)
		{
			i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
			pAttr->Set( true);
			pAttrSet->AddAttr( pAttr);

			i3AlphaFuncAttr * pFuncAttr = i3AlphaFuncAttr::new_object_ref();
			pFuncAttr->SetRefValue( m_AlphaTestFuncValue);
			pFuncAttr->SetFunction( I3G_COMP_GREATER);
			pAttrSet->AddAttr( pFuncAttr);
		}

		// Diffuse Texture
		I3ASSERT( m_pTex_Diff[ m_CurrentTexLevel] != nullptr);
		{
			i3TextureEnableAttr * pEnableAttr = i3TextureEnableAttr::new_object_ref();
			i3TextureBindAttr * pDiffuseTexAttr = i3TextureBindAttr::new_object_ref();
			
			pEnableAttr->Set( true);
			pDiffuseTexAttr->SetTexture( m_pTex_Diff[ m_CurrentTexLevel]);

			pAttrSet->AddAttr( pEnableAttr);
			pAttrSet->AddAttr( pDiffuseTexAttr);
		}

		// Normal Texture
		if( m_pTex_Norm[ m_CurrentTexLevel] != nullptr && lod < 2)
		{
			i3NormalMapEnableAttr * pEnableAttr = i3NormalMapEnableAttr::new_object_ref();
			i3NormalMapBindAttr * pBindAttr = i3NormalMapBindAttr::new_object_ref();

			pEnableAttr->Set( true);
			pBindAttr->SetTexture( m_pTex_Norm[ m_CurrentTexLevel]);

			pAttrSet->AddAttr( pEnableAttr);
			pAttrSet->AddAttr( pBindAttr);
		}

		// Specular Texture
		if( m_pTex_Spec[ m_CurrentTexLevel] != nullptr && lod < 2)
		{
			i3SpecularMapEnableAttr * pEnableAttr = i3SpecularMapEnableAttr::new_object_ref();
			i3SpecularMapBindAttr * pBindAttr = i3SpecularMapBindAttr::new_object_ref();

			pEnableAttr->Set( true);
			pBindAttr->SetTexture( m_pTex_Spec[ m_CurrentTexLevel]);

			pAttrSet->AddAttr( pEnableAttr);
			pAttrSet->AddAttr( pBindAttr);
		}

		// Reflect Mask Texture
		if( m_pTex_RefMask[ m_CurrentTexLevel] != nullptr)
		{
			i3ReflectMapEnableAttr* pReflectEnableAttr = i3ReflectMapEnableAttr::new_object_ref();
			i3ReflectMaskMapEnableAttr * pEnableAttr = i3ReflectMaskMapEnableAttr::new_object_ref();
			i3ReflectMaskMapBindAttr * pBindAttr = i3ReflectMaskMapBindAttr::new_object_ref();
			
			pReflectEnableAttr->Set(true);
			pEnableAttr->Set( true);
			pBindAttr->SetTexture( m_pTex_RefMask[ m_CurrentTexLevel]);
			
			pAttrSet->AddAttr(pReflectEnableAttr);
			pAttrSet->AddAttr( pEnableAttr);
			pAttrSet->AddAttr( pBindAttr);
		}

		// Geometry
		{
			i3Geometry * pGeo = i3Geometry::new_object_ref();

			// Render 순서를 먼저 하도록.. alpha sort를 해야함.
			pAttrSet->AddChild( pGeo);
			INT32 childCnt = pAttrSet->GetChildCount()-1;
			for( i = 0; i < childCnt; i++)
			{
				i3Node * pChild = pAttrSet->GetChild( 0);
				pChild->ChangeParent(pAttrSet);
			}
			
			i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();

#if defined( CLIENT_SIDE)
			if( m_bInThread)
			{
				// OptimizeSkin
				i3GeometryAttr TempGeoAttr;
				m_pTempVA[ lod]->setThreadMode( true);
				m_pTempIA[ lod]->setThreadMode( true);
				TempGeoAttr.SetVertexArray( m_pTempVA[ lod], I3G_PRIM_TRILIST, cntPrim, 0, false);
				TempGeoAttr.SetIndexArray( m_pTempIA[ lod]);

				// 스키닝 옵티마이즈
				i3SkinBuilder SkinBuilder;
				SkinBuilder.Build( &TempGeoAttr, false);
				pGeoAttr->setGroupList( &SkinBuilder.getGroupList());

				{
					SkinBuilder.getVertexArray()->setBoundSphereCenter( m_pTempVA[ lod]->getBoundSphereCenter());
					SkinBuilder.getVertexArray()->setBoundSphereRadius( m_pTempVA[ lod]->getBoundSphereRadius());
					
					SkinBuilder.getVertexArray()->setRestoreType( I3G_RESTORE_MEM);
					pGeoAttr->setPrimitiveVertexArray( SkinBuilder.getVertexArray());
					SkinBuilder.getVertexArray()->setThreadMode( false);
				}

				pGeoAttr->SetVertexArray(nullptr, I3G_PRIM_TRILIST, cntPrim, 0, false);

				pGeoAttr->CreateSkinMeshAABBProcessor(m_pTempVA[lod]);
			}
			else
			{
				pGeoAttr->SetVertexArray( m_pTempVA[ lod], I3G_PRIM_TRILIST, cntPrim, 0, false);
				pGeoAttr->SetIndexArray( m_pTempIA[ lod]);

				pGeoAttr->CreateSkinMeshAABBProcessor(m_pTempVA[lod]);
			}
#else
			pGeoAttr->SetVertexArray( m_pTempVA[ lod], I3G_PRIM_TRILIST, cntPrim, 0, false);
			pGeoAttr->SetIndexArray( m_pTempIA[ lod]);
#endif

			pGeoAttr->setVertexBlendMethod( I3G_VERTEXBLEND_NONINDEXED);

			pGeo->AppendGeometryAttr( pGeoAttr);
		}
	}

	I3_SAFE_RELEASE( m_pTempVA[ lod]);
	I3_SAFE_RELEASE( m_pTempIA[ lod]);

	bResult = true;

	if( !bResult)
	{
		I3_MUST_RELEASE(m_pNewLOD);

		return nullptr;
	}

	return m_pNewLOD;
}

void AvatarGen::_BuildShape( void)
{
	I3ASSERT( m_pPreFrameSkeleton != nullptr);

	for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
	{
		if( m_pTempVA[ lod] == nullptr)
			continue;

		i3LOD * pLOD = BuildLOD( lod, m_pPreFrameSkeleton);
 
 		if( pLOD != nullptr)
		{
			if( lod == 0 && m_bInLobby == false)
				pLOD->getShapeNode()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);	// LOD0은 1인칭으로 그림자는 안나오도록
			else
				pLOD->getShapeNode()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
 			m_pBody->AddLOD( pLOD);
			I3_SAFE_RELEASE( m_pNewLOD);
		}
	}

	if( m_bInThread == false)
		RecMergeGeo( m_pBody, _OptimizeForSkin , nullptr, nullptr);

	m_pBody->setFrameLOD( m_pBody->getLOD( m_FrameLODNum));
	m_pBody->Prepare(false);
	m_pBody->setConst( 1.f);
	m_pBody->setSlope( 3.5f);
}

void AvatarGen::BuildShape( void )
{
#if defined(CLIENT_SIDE)
	if( m_bInThread )//&& m_bInLobby)
	{
		i3Monitor2 mon( g_pCharaManager->GetAvatarManager()->getMutex());
		_BuildShape();
	}
	else
#endif
		_BuildShape();
}

void AvatarGen::_BuildHeadScene( const AVT_PARTINFO * pPart)
{
	INT32 i, j;
	for( i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
	{
		for( j = 1; j < AVT_LOD_COUNT; j++)
		{
			if( m_pHeadSgInfo[i][j] == nullptr)
				continue;

			i3Body * pBody = (i3Body *) i3Scene::FindNodeByExactType( m_pHeadSgInfo[i][j]->getInstanceSg(), i3Body::static_meta());

			if( pBody == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL,  "%d %d Skinning이 되지 않은 파일일 수 있습니다.", i, j );
				return;
			}

			// Upper의 Skeleton을 써야한다.
			i3SceneGraphInfo * pUpperScene = getSgInfo( EQUIP::ePART_VEST, j);
			if( pUpperScene == nullptr)
			{
				pUpperScene = getSgInfo( EQUIP::ePART_SKIN, j);
				if( pUpperScene == nullptr)
					continue;	// 특정 LOD는 없을 수 있다. (only lobby)
			}

			i3Body * pUpperBody = (i3Body *) i3Scene::FindNodeByExactType( pUpperScene->getInstanceSg(), i3Body::static_meta());
			I3ASSERT( pUpperBody != nullptr);

			i3LOD * pUpperLOD = pUpperBody->getFrameLOD();

			I3ASSERT( pUpperLOD->getSkeleton() != nullptr);

			// New LOD
			I3_SAFE_RELEASE( m_pHeadLOD[i][j]);
			m_pHeadLOD[i][j] = i3LOD::new_object();
			i3Node * pNewNode = i3Node::new_object_ref();
			m_pHeadLOD[i][j]->setShapeNode( pNewNode);
			
			{
				m_pHeadLOD[i][j]->setSkeleton( i3Skeleton::new_object_ref());
				pUpperLOD->getSkeleton()->CopyTo( m_pHeadLOD[i][j]->getSkeleton(), I3_COPY_INSTANCE);
			}

			char szName[ 64];
			i3::snprintf( szName, sizeof( szName), "Head_%d", i);
			pNewNode->SetName( szName);

			// 기존 FrameSkeleton을 기준으로 생성한 LOD들의 BoneMap을 작성
			if( j != m_FrameLODNum)
				_BuildBoneMap( m_pHeadLOD[i][j], m_pPreFrameSkeleton);

			i3MatrixStack	stack;
			AVT_BUILD_CTX	ctx;

			ctx.m_pAvatar	= this;
			ctx.m_pPart		= pPart;
			ctx.m_pTexInfo = AVATAR::getPartTexInfo( AVATAR::VIEW_3P, EQUIP::ePART_CHARA);
			ctx.m_pDestLOD	= m_pHeadLOD[i][j];
			ctx.m_LOD		= j;
			ctx.m_pFrameSkeleton = m_pPreFrameSkeleton;

			RecMergeGeo( m_pHeadSgInfo[i][j]->getInstanceSg(), _Rec_CopyLODProc, &ctx, &stack);
		}
	}
}

void AvatarGen::BuildHeadScene( void)
{
	__HeadIOCheck();

	I3ASSERT( m_pPreFrameSkeleton != nullptr);

	const AVT_PARTINFO * pPart	= AVATAR::getPartInfo( EQUIP::ePART_CHARA);

	if( (pPart->m_Style & AVT_PART_STYLE_BODY) == 0 )
		return;

	_BuildHeadScene( pPart);
}

void AVT_BUILD_CTX::RegistVA( i3VertexArray * pVA)
{
	VASharedCon::const_iterator it = m_mapList.find( pVA);
	if( it == m_mapList.end())
	{
		m_mapList.insert( VASharedCon::value_type( pVA, nullptr));
	}
}

bool AVT_BUILD_CTX::SharedVA( i3VertexArray * pSrcVA, i3VertexArray ** pDestVA)
{
	VASharedCon::iterator it = (VASharedCon::iterator)(m_mapList.find( pSrcVA));
	if( it != m_mapList.end())
	{
		if( it->second != nullptr)
		{
			*pDestVA = it->second;
			(*pDestVA)->AddRef();
			return true;
		}
		else
		{
			it->second = *pDestVA = i3VertexArray::new_object();
			(*pDestVA)->AddRef();
		}
	}
	else
		*pDestVA = i3VertexArray::new_object();
	return false;
}

void AvatarGen::_ReplaceNewGeometry( i3Node * pNode, AVT_BUILD_CTX * pCtx)
{
	VEC2D offset, scale;

	i3::vector<i3Node*> List;
	List.reserve( 16);

	i3Scene::FindNodeByType( pNode, i3Geometry::static_meta(), List);

	if( pCtx->m_pTexInfo != nullptr)
	{
		offset.x = (REAL32) pCtx->m_pTexInfo->m_X / AVT_TEX_WIDTH;
		offset.y = (REAL32) pCtx->m_pTexInfo->m_Y / AVT_TEX_HEIGHT;

		scale.x = (REAL32) pCtx->m_pTexInfo->m_W / AVT_TEX_WIDTH;
		scale.y = (REAL32) pCtx->m_pTexInfo->m_H / AVT_TEX_HEIGHT;
	}

	// shared VA 를 찾자
	UINT32 cnt = List.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		i3Geometry * pGeo = (i3Geometry*) List.at( i);

		INT32 geoCnt = pGeo->GetGeometryAttrCount();
		for( INT32 j = 0; j < geoCnt; j++)
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr*) pGeo->GetGeometryAttr( j);
			pCtx->RegistVA( pGeoAttr->GetVertexArray());
		}
	}

	for( UINT32 i = 0; i < cnt; i++)
	{
		i3Geometry * pGeo = (i3Geometry*) List.at( i);

		INT32 geoCnt = pGeo->GetGeometryAttrCount();
		for( INT32 j = 0; j < geoCnt; j++)
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr*) pGeo->GetGeometryAttr( j);
			i3VertexArray * pSrcVA = pGeoAttr->GetVertexArray();
			i3IndexArray * pSrcIA = pGeoAttr->GetIndexArray();

			if( pGeoAttr->IsModified())
				continue;

			i3GeometryAttr * pNewGeoAttr = i3GeometryAttr::new_object_ref();
			i3VertexArray * pDestVA = nullptr;

			// 공유하는 VA
			if( pCtx->SharedVA( pSrcVA, &pDestVA) == false)
			{
				i3VertexFormat fmt;

				fmt.SetHasPosition();
				fmt.SetHasNormal();
				fmt.SetHasTangent();
				fmt.SetHasBinormal();

				fmt.SetTextureCoordSetCount( 1);
				fmt.SetTextureCoordCount( 2);

				INT32 blendCount = pSrcVA->GetFormat()->GetBlendIndexCount();
				fmt.SetBlendIndexCount( (UINT8) blendCount); 
				fmt.SetBlendWeightCount( (UINT8) blendCount - 1);

				bool bMultiThread = false;
			#if defined( CLIENT_SIDE)
				if( pCtx->m_pAvatar->m_bInThread)
				{
					pDestVA->setThreadMode( true);
					pSrcVA->setThreadMode( true);

					bMultiThread = true;//!pCtx->m_pAvatar->m_bInLobby;
				}
			#endif
				pDestVA->setRestoreType( I3G_RESTORE_MEM);

				pDestVA->Create( &fmt, pSrcVA->GetCount(), pSrcVA->GetUsageFlag() );
				CopyVertexArray( pDestVA, pSrcVA, pCtx->_boneMap, 0, pSrcVA->GetCount(), bMultiThread);

				// Macrotexture의 UV로 설정
				if(  pCtx->m_pTexInfo != nullptr)
				{
					VEC2D vec2;

					if( bMultiThread)
						pSrcVA->SafeLock( I3G_LOCK_READONLY);
					else
						pSrcVA->Lock( I3G_LOCK_READONLY);
					pDestVA->Lock();

					UINT32 vaCnt = pDestVA->GetCount();
					for( UINT32 k = 0; k < vaCnt; k++)
					{
						pSrcVA->GetTextureCoord( 0, k, &vec2);

						if( vec2.x < 0.0f)
							vec2.x = 1.0f + vec2.x;

						if( vec2.y < 0.0f)
							vec2.y = 1.0f + vec2.y;

						i3Vector::Mul( &vec2, &vec2, &scale);
						i3Vector::Add( &vec2, &vec2, &offset);

						pDestVA->SetTextureCoord( 0, k, &vec2);
					}

					pDestVA->Unlock();

					if( bMultiThread)
						pSrcVA->SafeUnlock();
					else
						pSrcVA->Unlock();
				}

			#if defined( CLIENT_SIDE)
				{
					pDestVA->setThreadMode( false);
					pSrcVA->setThreadMode( false);
				}
			#endif
			}

			i3IndexArray * pDestIA = nullptr;

			if( pSrcIA != nullptr)
			{
				pDestIA = i3IndexArray::new_object();

				bool bMultiThread = false;

			#if defined( CLIENT_SIDE)
				if( pCtx->m_pAvatar->m_bInThread)
				{
					pDestIA->setThreadMode( true);
					pSrcIA->setThreadMode( true);

					bMultiThread = true;//!pCtx->m_pAvatar->m_bInLobby;
				}
			#endif

				pDestIA->setRestoreType( I3G_RESTORE_MEM);

				pDestIA->Create( pSrcIA->GetCount(), pSrcIA->GetUsageFlag(), pSrcIA->GetDataType());
				CopyIndexArray( pDestIA, pSrcIA, bMultiThread );

			#if defined( CLIENT_SIDE)
				{
					pDestIA->setThreadMode( false);
					pSrcIA->setThreadMode( false);
				}
			#else
				pNewGeoAttr->SetIndexArray( pDestIA);
			#endif
			}

			#if defined( CLIENT_SIDE)
			{
				// 스키닝 옵티마이즈
				i3GeometryAttr TempGeoAttr;

				if( pCtx->m_pAvatar->m_bInThread)
				{
					if (pDestIA)
						pDestIA->setThreadMode( true);
					pDestVA->setThreadMode( true);
				}

				TempGeoAttr.SetVertexArray( pDestVA, I3G_PRIM_TRILIST, pGeoAttr->GetPrimitiveCount(), 0, false);
				TempGeoAttr.SetIndexArray( pDestIA);

				i3SkinBuilder SkinBuilder;
				SkinBuilder.Build( &TempGeoAttr, false);
				pNewGeoAttr->setGroupList( &SkinBuilder.getGroupList());

				SkinBuilder.getVertexArray()->setBoundSphereCenter( pDestVA->getBoundSphereCenter());
				SkinBuilder.getVertexArray()->setBoundSphereRadius( pDestVA->getBoundSphereRadius());
				
				SkinBuilder.getVertexArray()->setRestoreType( I3G_RESTORE_MEM);
				pNewGeoAttr->setPrimitiveVertexArray( SkinBuilder.getVertexArray());
				SkinBuilder.getVertexArray()->setThreadMode( false);

				pNewGeoAttr->SetVertexArray(nullptr, I3G_PRIM_TRILIST, pGeoAttr->GetPrimitiveCount(), 0, false);

				pNewGeoAttr->CreateSkinMeshAABBProcessor(pDestVA);

				if (pDestIA)
					pDestIA->setThreadMode( false);
				pDestVA->setThreadMode( false);
								
			}
			#else
				pNewGeoAttr->SetVertexArray( pDestVA, pGeoAttr->GetPrimitiveType(), pGeoAttr->GetPrimitiveCount(), pGeoAttr->GetStartIndex(), true);
			#endif

			pGeo->RemoveGeometryAttr( pGeoAttr);
			pNewGeoAttr->SetModified(true);
			pGeo->AppendGeometryAttr( pNewGeoAttr);

			I3_SAFE_RELEASE( pDestVA);
			I3_SAFE_RELEASE( pDestIA);
			
			j--;
		}
	}
}

/*static*/ void AvatarGen::_ReplaceTeamTexture( i3AttrSet * pAttrSet, const AVT_BUILD_CTX * pCtx)
{
	i3::vector<i3RenderAttr*> list;
	i3Scene::FindAttrByType( pAttrSet, i3TextureBindAttr::static_meta(), list);

	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		i3TextureBindAttr * pAttr = static_cast<i3TextureBindAttr*>(list.at(i));
		if( pAttr->GetTexture() != nullptr)
		{
			i3::string resName = pAttr->GetTexture()->GetName();
			i3Texture * pTex = AVATAR::LoadTex( resName.c_str(), pCtx->m_pAvatar->GetTeam());
			if( pTex != nullptr)
				pAttr->SetTexture( pTex);
		}
	}
}

/*static*/ I3SG_TRAVERSAL_RESULT AvatarGen::_Rec_CopyLODProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	AVT_BUILD_CTX * pCtx = (AVT_BUILD_CTX *) pUserData;
	
	if( i3::same_of<i3Body*>(pNode))
	{
		pCtx->m_pSrcBody = (i3Body *) pNode;
		if( (pCtx->m_pPart->m_Style & AVT_PART_STYLE_BODY) > 0)
			pCtx->m_pAvatar->_BuildBoneMap( pCtx);
	}
	else if( i3::same_of<i3AttrSet*>(pNode))
	{
		i3Node * pNewNode = i3Scene::CreateClone( pNode, I3_COPY_INSTANCE);
		pCtx->m_pDestLOD->getShapeNode()->AddChild( pNewNode);
		i3::vector<i3Node*> list;
		i3Scene::FindNodeByExactType( pNewNode, i3AttrSet::static_meta(), list);

		const AVATAR::TEXINFO * pTexInfo = pCtx->m_pTexInfo;

		UINT32 cnt = list.size();
		for( UINT32 i = 0; i < cnt; i++)
		{
			i3AttrSet * pAttrSet = static_cast<i3AttrSet*>(list.at(i));
			if( _CanMergeTexture( pAttrSet, pCtx->m_pAvatar->m_bInLobby, pCtx->m_pAvatar->hasSkinParts()) == false)
				pCtx->m_pTexInfo = nullptr;

			_ReplaceNewGeometry( pAttrSet, pCtx);

			// replace team texture
			_ReplaceTeamTexture( pAttrSet, pCtx);

			pCtx->m_pTexInfo = pTexInfo;
		}

		return I3SG_TRAVERSAL_STOP;	// 이 노드는 LOD의 ShapeNode이다. Child에 대한 처리가 끝난상태이므로 돌지 않는다.
	}	

	return I3SG_TRAVERSAL_CONTINUE;
}

void AvatarGen::_CopyLODShapeNode( INT32 lod, EQUIP::ePART part, i3LOD * pTarget)
{
	i3SceneGraphInfo * pSgInfo	= getSgInfo( part, lod);
	if( pSgInfo == nullptr)
		return;

	const AVT_PARTINFO * pPart	= AVATAR::getPartInfo( part);

	if( (pPart->m_Style & AVT_PART_STYLE_BODY) == 0 )
		return;

	i3MatrixStack	stack;
	AVT_BUILD_CTX	ctx;
	AVATAR::VIEW view;

	if( lod == 0)
		view = AVATAR::VIEW_1P;
	else
		view = AVATAR::VIEW_3P;


	ctx.m_pAvatar	= this;
	ctx.m_pPart		= pPart;
	ctx.m_pTexInfo	= pSgInfo->isCommonRes()?AVATAR::getPartTexInfo( view, part):nullptr;
	ctx.m_pDestLOD	= pTarget;

	RecMergeGeo( pSgInfo->getInstanceSg(), _Rec_CopyLODProc, &ctx, &stack);
}

i3LOD * AvatarGen::_BuildLODNoMerge( INT32 lod, i3Skeleton * pFrameSkel )
{
	// Skeleton
	// 기본 Skeleton 구조는 항상 Upper Part에서 가져온다.
	i3SceneGraphInfo * pSgInfo = getSgInfo( EQUIP::ePART_VEST, lod);
	if( pSgInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%d LOD용의 Upper Part가 존재하지 않습니다.", lod );
		return nullptr;
	}

	i3Body * pBody = (i3Body *) i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta());
	if( pBody == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%d LOD : 올바른 Upper Part가 아닙니다. Skinning이 되지 않은 파일일 수 있습니다.", lod );
		return nullptr;
	}

	i3LOD * pLOD = pBody->getFrameLOD();
	i3Skeleton * pSkel = pLOD->getSkeleton();

	// New LOD
	{
		m_pNewLOD = i3LOD::new_object();
		m_pNewLOD->setSkeleton( i3Skeleton::new_object_ref());
		pSkel->CopyTo( m_pNewLOD->getSkeleton(), I3_COPY_INSTANCE);
	}

	// Bone Map
	if( lod != m_FrameLODNum)			// LOD 1이 Frame Skeleton이기 때문에 BoneMap을 생성할 필요가 없다.
	{
		_BuildBoneMap( m_pNewLOD, pFrameSkel);
	}

	{	// 미리 만들어야 원본에서 Merge하지 않는 것을 복사할 수 있다.
		m_pNewLOD->setShapeNode( i3Node::new_object_ref());
	#if defined( I3_DEBUG)
		m_pNewLOD->getShapeNode()->SetName( "Material");
	#endif
	}
	
	// Part별로 Merge
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		_CopyLODShapeNode( lod, (EQUIP::ePART) i, m_pNewLOD);
	}

	return m_pNewLOD;
}

void AvatarGen::_BuildShapeNoMerge(void)
{
	I3ASSERT( m_pBody != nullptr);
	I3ASSERT( m_pPreFrameSkeleton != nullptr);

	// 로비의 캐릭터는 LOD1만 필요하다.
	if( m_LoadStyle & AVATAR_LOAD_STYLE_1PV)
	{
		i3LOD * pNewLOD = _BuildLODNoMerge( 0, m_pPreFrameSkeleton);

		if( pNewLOD != nullptr)
		{
			m_pBody->AddLOD( pNewLOD);
			I3_MUST_RELEASE(pNewLOD);
		}

		pNewLOD = _BuildLODNoMerge( 1, m_pPreFrameSkeleton);

		if( pNewLOD != nullptr)
		{
			m_pBody->AddLOD( pNewLOD);
			m_pBody->setFrameLOD( pNewLOD);
			I3_MUST_RELEASE(pNewLOD);
		}

		m_pNewLOD = nullptr;
	}
	else
	{
		_BuildLODNoMerge( 1, m_pPreFrameSkeleton);
 
 		if( m_pNewLOD != nullptr)
		{
 			m_pBody->AddLOD( m_pNewLOD);
			m_pBody->setFrameLOD( m_pNewLOD);

			I3_MUST_RELEASE(m_pNewLOD);
			m_pNewLOD = nullptr;
		}
	}

	m_pBody->Prepare( false);
}

I3SG_TRAVERSAL_RESULT AvatarGen::_OptimizeForSkin( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	
	if( i3::same_of<i3Geometry*>(pNode))
	{
		i3GeometryAttr * pGeoAttr;
		i3Geometry * pGeo = (i3Geometry *) pNode;

		INT32 geoCnt = pGeo->GetGeometryAttrCount();
		for( INT32 i = 0; i < geoCnt; i++)
		{
			pGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr( i);

			if( pGeoAttr->getPrimitiveVertexArray() == nullptr)
			{
				pGeoAttr->OptimizeForSkin();

				// 기존의 VA, IA는 필요 없다.
				pGeoAttr->SetIndexArray( nullptr);
				pGeoAttr->SetVertexArray( nullptr, I3G_PRIM_TRILIST, pGeoAttr->GetPrimitiveCount(), 0, false);
			}
		}
	}

	INT32 childCnt = pNode->GetChildCount();
	for( INT32 i = 0; i < childCnt; i++)
	{
		i3Node * pChild = pNode->GetChild(i);
		if( pChild->GetRefCount() == 0xdddddddd )
		{
			I3PRINTLOG(I3LOG_FATAL,  "%s",  pNode->GetName() );
		}

		RecMergeGeo( pChild, _OptimizeForSkin, pUserData, pStack);

	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void AvatarGen::CheckValid(void)
{
	VEC3D vec3;

	for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
	{
		UINT32 vaCnt = m_pTempVA[lod]->GetCount();
		for( UINT32 i = 0; i < vaCnt; i++)
		{
			m_pTempVA[lod]->GetPosition( i, &vec3);

			I3ASSERT( i3Math::isValid( vec3.x) && i3Math::isValid( vec3.y) && i3Math::isValid( vec3.z));
		}

		UINT32 iaCnt = m_pTempIA[lod]->GetCount();
		for( UINT32 i = 0; i < iaCnt; i++)
		{
			INT32 idx = m_pTempIA[lod]->GetIndex( i);

			I3ASSERT( idx >= 0);
			I3ASSERT( idx < (INT32) m_pTempVA[lod]->GetCount());
		}
	}
}
