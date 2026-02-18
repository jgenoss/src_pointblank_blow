#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"

// Physique에서 추출 가능한 아래의 값들을 생성해 둔다.
// - Vertex Bone Reference.
// - Vertex Weights
// - Bone List
// - Root Bone
void i3Export::_ResolvePhysique( INode * pNode, Modifier * pMod)
{
	INT32 i, blendBoneCount;
	IPhyVertexExport * pVertexExport = NULL;
	IPhysiqueExport * phyExport = NULL;
	IPhyContextExport * mcExport = NULL;
	i3::vector<INode*> BoneList;
		
	phyExport = (IPhysiqueExport *)pMod->GetInterface( I_PHYINTERFACE);
	if( phyExport == NULL)
		return;

	mcExport = (IPhyContextExport *)phyExport->GetContextInterface( pNode);
	if( mcExport == NULL)
		goto ExitPart;

	mcExport->ConvertToRigid( TRUE);
	mcExport->AllowBlending( TRUE);

//	BoneList.SetOnceMode( TRUE);			// remove_duplicates사용..

	if( mcExport->GetNumberVertices() <= 0)
		goto ExitPart;

	{
		m_WeightCount = mcExport->GetNumberVertices();

		m_pWeights = new i3tool::exporter::SKIN_VERTEX[ m_WeightCount];
		I3ASSERT( m_pWeights != NULL);

		memset( m_pWeights, 0, sizeof(i3tool::exporter::SKIN_VERTEX) * mcExport->GetNumberVertices());
	}

	// 우선 Vertex에 의해 Reference되고 있는 모든 Bone Node들의 List를 작성한다.
	for( i = 0; i < mcExport->GetNumberVertices(); i++)
	{
		pVertexExport = (IPhyVertexExport *) mcExport->GetVertexInterface( i);

		switch( pVertexExport->GetVertexType())
		{
			case RIGID_BLENDED_TYPE :
				//type-cast the node to the proper class		
				{
					IPhyBlendedRigidVertex *vtx = (IPhyBlendedRigidVertex *) pVertexExport;

					blendBoneCount = MIN( vtx->GetNumberNodes(), MAX_BLEND_MATRIX);

					m_pWeights[i].m_WeightCount = blendBoneCount;
					
					for( int x = 0; x < blendBoneCount; x++)
					{
						m_pWeights[i].m_pBone[x] = vtx->GetNode( x);
						m_pWeights[i].m_Weight[x] = vtx->GetWeight( x);
						m_pWeights[i].m_Offset[x] = vtx->GetOffsetVector( x);

						BoneList.push_back( m_pWeights[i].m_pBone[x]);
					}
				}
				break;

			case RIGID_NON_BLENDED_TYPE :
				{
					IPhyRigidVertex * vtx = (IPhyRigidVertex *) pVertexExport;

					m_pWeights[i].m_WeightCount = 1;
					m_pWeights[i].m_pBone[0] = vtx->GetNode();
					m_pWeights[i].m_Weight[0] = 1.0f;
					m_pWeights[i].m_Offset[0] = vtx->GetOffsetVector();

					BoneList.push_back( vtx->GetNode());
				}
				break;

			default :
				break;
		}

		mcExport->ReleaseVertexInterface( pVertexExport);
		pVertexExport = NULL;
	}
	
	i3::vu::remove_duplicates(BoneList);

	m_pSkinBoneRoot = NULL;

	if( BoneList.size() <= 0)
	{
		I3PRINTLOG(I3LOG_FATAL, "Could not found any bones.\n");
		goto ExitPart;
	}

	// 발견된 Bone들 중에서 최상위 노드를 찾는다.
	m_pSkinBoneRoot = FindRootOfBone( BoneList[0]);

ExitPart:
	if( phyExport != NULL)
	{
		pMod->ReleaseInterface( I_PHYINTERFACE, phyExport);
		phyExport = NULL;
	}
}

INT32 i3Export::_FindBoneByRef( INode * pNode)
{
	INT32 i;
	i3tool::exporter::SKIN_JOINT * pInfo;

	for( i = 0; i < _getJointInfoCount(); i++)
	{
		pInfo = _getJointInfo( i);

		if( pInfo->m_pBoneNode == pNode)
			return i;
	}

	return -1;
}

i3Skeleton * i3Export::_CreateSkeleton( INode * pNode, Modifier * pMod)
{
	IPhysiqueExport * phyExport;
	i3Skeleton * pSkel;
	INT32 i;
	Matrix3 mtx;
	INode * pBone;
	TimeValue time = m_timeStart;
	i3MatrixArray * pInvBoneArray;
	MATRIX boneMtx;
	MATRIX invMtx;

	phyExport = (IPhysiqueExport *)pMod->GetInterface( I_PHYINTERFACE);
	if( phyExport == NULL)
		return NULL;

	pSkel = i3Skeleton::new_object_ref();
	pSkel->SetBoneCount( _getJointInfoCount());

	pInvBoneArray = i3MatrixArray::new_object_ref();
	pInvBoneArray->Create( _getJointInfoCount());
	pSkel->getBoneMatrixListAttr()->setInverseBoneArray( pInvBoneArray);

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

	return pSkel;
}

void	i3Export::_OnExportPhysiqueSkin( INode * pNode, Modifier * pMod, Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, bool bVertexColor, INT32 TexCoordCount)
{
	INT32 maxBlendMatrix = 0;
	INT32 i;
	i3Skin * pSkin;
	bool bBoneRefProcess = false;

	m_pSkinBoneRoot = NULL;

	// 기존에 생성된 Skeleton이 없기 때문에 새롭게 생성
	pSkin = i3Skin::new_object_ref();
	m_pCurProcessInfo->pushi3Node( pSkin);

	// Physique 관련 정보를 추출한다.
	_ResolvePhysique( pNode, pMod);

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

	// Bone Array Data를 생성한다.
	_Rec_BuildJointInfo( 0, m_pSkinBoneRoot);

	{
		i3tool::exporter::SKELETON_INFO * pSkelInfo = FindSkeletonByBoneRoot( m_pSkinBoneRoot);
		i3Skeleton * pSkel;

		if( pSkelInfo == NULL)
		{
			// Skeleton
			{
				pSkel = _CreateSkeleton( pNode, pMod);
				I3ASSERT( pSkel != NULL);

				pSkin->AddSkeleton( pSkel);
			}

			AddSkeletonInfo( pSkin, pSkel, m_pSkinBoneRoot);

			// Animation
			{
				i3Animation * pAnim = _ExtractAnimationFromBone();

				if( pAnim != NULL)
				{
					pSkin->PlayAnim( pAnim);
				}
			}

			bBoneRefProcess = true;
		}
		else
		{
			// Skeleton 구조를 공유한다.
			pSkin->AddSkeleton( pSkelInfo->m_pSkeleton);
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
		Matrix3 posTx;
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
	/*
	if( bBoneRefProcess)
	{
		List TempMeshGroupList;

		// Skeleton에 추가로 붙어 있는 Dummy들은 Traversal에서 제외되며,
		// 특성 상, Bone을 참조하는 특수한 Node인 i3BoneRef Node를
		// 이용해, Scene Graph에 추가된다.
		_SaveMeshGroup( &TempMeshGroupList);

		_ProcessBoneRef( pSkin, pSkel);

		_RestoreMeshGroup( &TempMeshGroupList);
	}
	*/

	m_pCurProcessInfo->popi3Node();
}