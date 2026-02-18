#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"

void i3Export::_ResolveISkin( INode * pNode, Modifier * pMod)
{
	INT32 i, blendBoneCount, j;
	ISkin * pISkin = NULL;
	ISkinContextData * pSkinCtx = NULL;
	i3::vector<INode*> BoneList;
		
	pISkin = (ISkin *) pMod->GetInterface( I_SKIN);
	if( pISkin == NULL)
		return;

	pSkinCtx = pISkin->GetContextInterface( pNode);
	if( pSkinCtx == NULL)
		goto ExitPart;

	{
		int rv = pISkin->GetSkinInitTM( pNode, m_SkinInitTM, true);

		if( rv != SKIN_OK)
		{
			Log( pNode, "Skin Initial Transform을 구할 수 없습니다.");
			m_SkinInitTM.IdentityMatrix();
		}
	}

	// m_pWeights Buffer에 대한 준비
	{
		m_WeightCount = pSkinCtx->GetNumPoints();

		m_pWeights = new i3tool::exporter::SKIN_VERTEX[ m_WeightCount];
		I3ASSERT( m_pWeights != NULL);

		memset( m_pWeights, 0, sizeof(i3tool::exporter::SKIN_VERTEX) * m_WeightCount);
	}

//	BoneList.SetOnceMode( TRUE);

	// 우선 Vertex에 의해 Reference되고 있는 모든 Bone Node들의 List를 작성한다.
	for( i = 0; i < m_WeightCount; i++)
	{
		blendBoneCount = pSkinCtx->GetNumAssignedBones( i);

		m_pWeights[i].m_WeightCount = blendBoneCount;

		for( j = 0; j < blendBoneCount; j++)
		{
			INT32 boneIdx = pSkinCtx->GetAssignedBone( i, j);
			INode * pBone = pISkin->GetBone( boneIdx);

			BoneList.push_back( pBone);

			m_pWeights[i].m_pBone[j] = pBone;
			m_pWeights[i].m_Weight[j] = pSkinCtx->GetBoneWeight( i, j);
		}
	}

	i3::vu::remove_duplicates(BoneList);

	m_pSkinBoneRoot = NULL;

	if( BoneList.size() <= 0)
	{
		I3PRINTLOG(I3LOG_FATAL, "Could not found any bones.\n");
		return;
	}

	// 발견된 Bone들 중에서 최상위 노드를 찾는다.
	m_pSkinBoneRoot = FindSkinRoot( BoneList);

	I3TRACE( "Skin Bone Root : %s\n", m_pSkinBoneRoot->GetName());

ExitPart:
	if( pISkin != NULL)
	{
		pMod->ReleaseInterface( I_SKIN, pISkin);
		pISkin = NULL;
	}
}

i3Skeleton * i3Export::_CreateSkinSkeleton( INode * pNode, Modifier * pMod)
{
	i3Skeleton * pSkel = NULL;
	INT32 i;
	MATRIX boneMtx;
	INode * pBone;
	TimeValue time = m_timeStart;
	ISkin * pISkin = NULL;
	MATRIX invMtx;
	i3MatrixArray * pInvBoneArray;

	pISkin = (ISkin *) pMod->GetInterface( I_SKIN);
	if( pISkin == NULL)
		return NULL;

	pSkel = i3Skeleton::new_object_ref();
	pSkel->SetBoneCount( _getJointInfoCount());

	pInvBoneArray = i3MatrixArray::new_object_ref();
	pInvBoneArray->Create( _getJointInfoCount());
	pSkel->getBoneMatrixListAttr()->setInverseBoneArray( pInvBoneArray);

	if( _getJointInfoCount() <= 0)
	{
		I3TRACE( "No Joint\n");
	}

	for( i = 0; i < _getJointInfoCount(); i++)
	{
		i3tool::exporter::SKIN_JOINT * pInfo = _getJointInfo( i);

		pBone = pInfo->m_pBoneNode;

		ExtractTransform( pBone, &boneMtx, time);

		pSkel->setMatrix( i, &boneMtx);		// Bone의 기본값

		// Inverse Bone Matrix를 가져온다.
		{
			Matrix3 mtx;

			ScaleBiped( pBone, TRUE);

			mtx = __NODE_TM( pBone, 0);

			ScaleBiped( pBone, FALSE);

			mtx.Invert();

			//mtx = mtx;
			::Convert( &invMtx, &mtx);
			pSkel->setInverseMatrix( i, &invMtx);
		}

		// Name
		pSkel->setName( i, pBone->GetName());

		// Parent Bone Index
		{
			INT32 parentIdx = _FindBoneByRef( pBone->GetParentNode());

			pSkel->setParentBoneIndex( i, parentIdx);
		}

		// Depth
		pSkel->setDepth( i, pInfo->m_Depth);
	}

	if( pISkin != NULL)
	{
		pMod->ReleaseInterface( I_SKIN, pISkin);
		pISkin = NULL;
	}

	return pSkel;
}

void	i3Export::_OnExportSkin( INode * pNode, Modifier * pMod, Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, bool bVertexColor, INT32 TexCoordCount)
{
	INT32 maxBlendMatrix = 0;
	INT32 i;
	i3Skin * pSkin;
	i3Skeleton * pSkel;
	bool bBoneRefProcess = false;
	BONETYPE boneType;
//	List InterRootList;				// 쓰이지 않아서 주석..

	m_pSkinBoneRoot = NULL;

	FindBonesModifier( pNode, &boneType);

	switch( boneType)
	{
		case BONES_3DS :
			// Physique 관련 정보를 추출한다.
			_ResolveISkin( pNode, pMod);
			break;

		case PHYSIQUE :
			_ResolvePhysique( pNode, pMod);
			break;
	}

	pSkin = i3Skin::new_object_ref();
	m_pCurProcessInfo->pushi3Node( pSkin);

	if( m_pSkinBoneRoot != NULL)
	{
		// Skin Node가 붙을 Node를 선택해준다.
		INode * pInterRoot = m_pSkinBoneRoot->GetParentNode();

		m_pCurProcessInfo->setParentINode( pInterRoot);
	}

	// Skinning용으로 발견된 Bone Node들은 별도의 Scene Graph로 Export될 
	// 필요가 없기 때문에 Exclude List에 포함시킨다.
	m_ExcludeNodeList.insert( pNode);
	excludeNode( m_pSkinBoneRoot);

	m_BoneRefList.clear();
	// Bone Array Data를 생성한다.
	{
		// Bone의 총 갯수를 구한다.
		// 여기에는 Nub 및 Dummy들도 포함된 개수가 구해진다.
		_Rec_BuildJointInfo( 0, m_pSkinBoneRoot);
	}

	{
		i3tool::exporter::SKELETON_INFO * pInfo = FindSkeletonByBoneRoot( m_pSkinBoneRoot);

		if( pInfo == NULL)
		{
			// Skeleton
			{
				switch( boneType)
				{
					case BONES_3DS :
						pSkel = _CreateSkinSkeleton( pNode, pMod);
						break;

					case PHYSIQUE :
						// Skeleton
						pSkel = _CreateSkeleton( pNode, pMod);
						break;
				}
		
				I3ASSERT( pSkel != NULL);

				pSkin->AddSkeleton( pSkel);
			}

			// Animation
			{
				i3Animation * pAnim = _ExtractAnimationFromBone();

				if( pAnim != NULL)
				{
					pSkin->PlayAnim( pAnim);
				}
			}

			AddSkeletonInfo( pSkin, pSkel, m_pSkinBoneRoot);

			bBoneRefProcess = true;
		}
		else
		{
			pSkel = pInfo->m_pSkeleton;
			pSkin->AddSkeleton( pInfo->m_pSkeleton);
		}
	}

	// 사용된 Blending Matrix의 최대 개수를 구한다.
	for( i = 0; i < m_WeightCount; i++)
	{
		if( m_pWeights[i].m_WeightCount > maxBlendMatrix)
		{
			maxBlendMatrix = m_pWeights[i].m_WeightCount;
		}
	}

	// Shape
	{
		i3GeometryAttr * pGeoAttr;
		Matrix3 posTx, pivotTx;
		i3Shape * pShape;

		// Physique의 경우에는 NodeTM이 Skeleton에는 적용되지 않고, Shape에만 적용되어야
		// 하기 때문에, i3Transform Node를 추가하지 않고 Vertex들을 직접 Transform 시킨다.
		ExtractTransform( pNode, &posTx, m_timeStart);

		pGeoAttr = _BuildGeometry( pNode, pMesh, pGrpInfo, &posTx, bVertexColor, TexCoordCount, maxBlendMatrix);

		pShape = i3Shape::new_object_ref();
		pSkin->AddShape( pShape);
		pShape->AddGeometryAttr( pGeoAttr);
	}

	// i3BoneRef에 대한 처리
	if( bBoneRefProcess)
	{
		i3::vector<i3tool::exporter::MESH_GROUP_INFO *> TempMeshGroupList;

		// Skeleton에 추가로 붙어 있는 Dummy들은 Traversal에서 제외되며,
		// 특성 상, Bone을 참조하는 특수한 Node인 i3BoneRef Node를
		// 이용해, Scene Graph에 추가된다.
		_SaveMeshGroup( TempMeshGroupList);

		_ProcessBoneRef( pNode, pSkin, pSkel);

		_RestoreMeshGroup( TempMeshGroupList);
	}

	m_pCurProcessInfo->popi3Node();		// i3Skin
}

void i3Export::_ProcessBoneRef( INode * pSkinNode, i3Skin * pSkin, i3Skeleton * pSkel)
{
	INT32 idx;
	INode * pNode, * pParentBone;
	i3BoneRef * pBoneRef;
	i3ExpProcessInfo * pInfo, * pOldProcessInfo = m_pCurProcessInfo;

	//m_bNoAddProcessInfo = true;
	m_bNoCheckHide = true;

	for(size_t i = 0; i < m_BoneRefList.size(); i++)
	{
		pNode = m_BoneRefList[i];

		if( _IsValidNode( pNode) == false)
		{
			continue;
		}

		// pNode에서부터 Recursive하게 Child Node로의 일반적인 Traverse
		Traverse( pNode, _NodeTraverseProc, this);


		// 처리 결과로 만들어진 ProcessInfo를 검색한다.
		pInfo = findProcessInfo( pNode);
		I3ASSERT( pInfo != NULL);

		// Parent Node는 Skeleton으로 포함되었기 때문에
		// 개별적인 i3Node는 존재하지 않는다.
		pParentBone = pNode->GetParentNode();

		idx = pSkel->FindBoneByName( pParentBone->GetName());
		I3ASSERT( idx != -1);

		////
		pBoneRef = i3BoneRef::new_object_ref();
		pBoneRef->SetName( pNode->GetName());
		pBoneRef->setBoneIndex( idx);

		// 생성된 pBoneRef는 제일 상위로 설정한다.
		pInfo->replaceTop( pBoneRef);

		// i3BoneRef는 반드시 i3Skin의 하위에 붙어야 한다.
		pInfo->setAttachNode( pSkin );


		// 이후의 처리에서 제외시킨다.
		m_ExcludeNodeList.insert( pNode);
	}

	m_bNoAddProcessInfo = false;
	m_bNoCheckHide = false;
	m_pCurProcessInfo = pOldProcessInfo;
}
