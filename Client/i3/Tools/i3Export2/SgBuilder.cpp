#include "stdafx.h"
#include "SgBuilder.h"
#include "Util.h"
#include "i3Export.h"
#include "iLayer.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( BONE_INFO);

static INT32 _INodeCompProc( BONE_INFO * p1, BONE_INFO * p2)
{
	if( p1->m_pINode > p2->m_pINode)
		return 1;
	else if( p1->m_pINode < p2->m_pINode)
		return -1;

	return 0;
}

static INT32 _INodeFindProc( BONE_INFO * p1, INode * p2)
{
	if( p1->m_pINode > p2)
		return 1;
	else if( p1->m_pINode < p2)
		return -1;

	return 0;
}

static INT32 _ScanNodeCompProc( SCAN_NODE_INFO * p1, SCAN_NODE_INFO * p2)
{
	if( p1->m_pINode > p2->m_pINode)
		return 1;
	else if( p1->m_pINode < p2->m_pINode)
		return -1;

	return 0;
}

static INT32 _ScanNodeFindProc( SCAN_NODE_INFO * p1, INode * p2)
{
	if( p1->m_pINode > p2)
		return 1;
	else if( p1->m_pINode < p2)
		return -1;

	return 0;
}

SgBuilder::SgBuilder(void)
{
	m_pIRoot = NULL;
	m_pExport = NULL;

	m_pRoot = NULL;

//	m_BoneList.SetCompareProc( (COMPAREPROC) _INodeCompProc);
//	m_ScanNodeList.SetCompareProc( (COMPAREPROC) _ScanNodeCompProc);
}

SgBuilder::~SgBuilder(void)
{
	removeAllJobs();
	removeAllBones();
	removeAllSkeleton();
	RemoveAllScanNode();
	_RemoveAllMesh();

	I3_SAFE_RELEASE( m_pRoot);
}

bool SgBuilder::isTargetToExport( INode * pINode, i3Export* pExporter)
{
	INT32 forceExport = 0;

	if( pINode->GetUserPropInt( TSTR("ForceExport"), forceExport) == TRUE)
	{
	}

	if( pExporter->m_bNoCheckHide == false)
	{
		if( pINode->IsHidden())
			return false;
	}

	if( pINode->GetUserPropInt( TSTR("ForceExport3"), forceExport) == TRUE)
	{
		if( forceExport != FALSE)
			return true;
		else
			return false;
	}

	if( IsFootPrint( pINode))
		return false;

	// Exclude 조건에 해당하는지 확인
	if( pExporter->_IsValidNode( pINode) == false)
		return false;


	return true;
}

////////////////////////////////////////////////////////////////
SCAN_NODE_INFO *	SgBuilder::FindScanNode( INode * pNode)
{
	i3::vector_set<SCAN_NODE_INFO*>::iterator it = m_ScanNodeList.find_other(pNode);
	return ( it != m_ScanNodeList.end() ) ? *it : NULL;
}

void SgBuilder::RemoveAllScanNode(void)
{
	i3::cu::for_each_delete_clear(m_ScanNodeList);
}

////////////////////////////////////////////////////////////////
JOB_INFO	*	SgBuilder::addJob( INode * pINode, UINT32 npt)
{
	JOB_INFO * pInfo;

	pInfo = new JOB_INFO;

	pInfo->m_NPT = npt;
	pInfo->m_pINode = pINode;
	pInfo->m_bProcessed = false;
	pInfo->m_idxBone = -1;
	pInfo->m_pPhysixNode = NULL;
	pInfo->m_pShapeSet = NULL;

	m_JobList.push_back( pInfo);

	return pInfo;
}

void			SgBuilder::removeAllJobs(void)
{
	INT32 i;
	JOB_INFO * pInfo;

	for( i = 0; i < getJobCount(); i++)
	{
		pInfo = getJob( i);

		delete pInfo;
	}

	m_JobList.clear();
}

// Physix Proxy가 동일하게 설정된 Node의 경우,
// 전체를 묶어 하나의 Actor로 Export해야 한다.
// 이렇게 동일한 Proxy를 가진 Node들을 검색하기 위한 함수.
void SgBuilder::SearchJobByPhysixProxy( const char * pszProxy, i3::vector<JOB_INFO*>& List)
{
	INT32 i;
	JOB_INFO * pInfo;
	TSTR temp;

	for( i = 0; i < getJobCount(); i++)
	{
		pInfo = getJob( i);

		if( pInfo->m_bProcessed == true)
			continue;

		if( (pInfo->m_NPT & NPT_PHYSIX) == 0)		// Physix 대상 Node가 아니라면 고려 대상이 아니다.
			continue;

		if( pInfo->m_pINode->GetUserPropString( TSTR( "Proxy_Geometry"), temp))
		{
			if( i3::generic_is_iequal( temp.data(), pszProxy) )
			{
				List.push_back( pInfo);
			}
		}
	}
}

void SgBuilder::dumpJob(void)
{
	INT32 i;
	JOB_INFO * pInfo;

	I3TRACE( "-----------------------  Jobs ---------------------------\n");

	for( i = 0; i < getJobCount(); i++)
	{
		pInfo = (JOB_INFO *) getJob( i);

		I3TRACE( "JOB[%d] : %s -> ", i, pInfo->m_pINode->GetName());

		if( pInfo->m_NPT & NPT_MESH)
			I3TRACE( "Mesh ");
		if( pInfo->m_NPT & NPT_LIGHT)
			I3TRACE( "Light ");
		if( pInfo->m_NPT & NPT_CAMERA)
			I3TRACE( "Camera ");
		if( pInfo->m_NPT & NPT_PHYSIX)
			I3TRACE( "Physix ");

		I3TRACE( "\n");
	}
}

/////////////////////////////////////////////////////////////////////
BONE_INFO *	SgBuilder::addBone( INode * pINode, bool bSkinned, INode * pSkinMesh)
{
	BONE_INFO * pInfo;

	if( i3::generic_is_iequal( pINode->GetName(), "Scene Root") )
	{
		I3TRACE( "sdf\n");
	}

	pInfo = BONE_INFO::new_object();

	pInfo->m_pINode = pINode;
	pInfo->m_bSkinned = bSkinned;
	
	if( bSkinned)
		pInfo->m_pSkinMesh = pSkinMesh;

	m_BoneList.insert( pInfo);

	return pInfo;
}

INT32 SgBuilder::findBoneByINode( INode * pINode)
{
	i3::vector_set<BONE_INFO*>::iterator it = m_BoneList.find_other(pINode);
	return (it != m_BoneList.end()) ? i3::distance(m_BoneList.begin(), it) : -1;
}

INT32 SgBuilder::findBoneByName( const char * pszName)
{
	INT32 i;
	BONE_INFO * pInfo;

	for( i = 0; i < getBoneCount(); i++)
	{
		pInfo = getBone( i);

		if( i3::generic_is_iequal( pInfo->m_pINode->GetName(), pszName) )
			return i;
	}

	return -1;
}

void SgBuilder::removeAllBones(void)
{
	i3::cu::for_each_release_clear(m_BoneList, &BONE_INFO::Release);	
}

void SgBuilder::dumpBone(void)
{
	INT32 i;
	BONE_INFO * pInfo;

	I3TRACE( "-----------------------  Bones ---------------------------\n");

	for( i = 0; i < getBoneCount(); i++)
	{
		pInfo = getBone( i);

		I3TRACE( "NODE[%d] : %s [", i, pInfo->m_pINode->GetName());

		if( pInfo->m_bSkinned)
			I3TRACE( " S");

		I3TRACE( "]\n");
	}
}


/////////////////////////////////////////////////////////////////////
void SgBuilder::removeAllSkeleton(void)
{
	INT32 i;

	for( i = 0; i < getSkelCount(); i++)
	{
		SKELETON_INFO * pInfo = (SKELETON_INFO *) getSkel( i);

		delete pInfo;
	}

	m_SkelList.clear();
}

/////////////////////////////////////////////////////////////////////
void SgBuilder::ClassifyNode( INode * pINode)
{
	INT32 i;
	bool bCollisionLayer = false;
	TimeValue time = m_pExport->getStartTime();
	JOB_INFO * pJobInfo = NULL;

	Progress();

	if( isTargetToExport(pINode, m_pExport))
	{
		// Layer 확인
		{
			ILayer * pLayer = (ILayer *) pINode->GetReference( NODE_LAYER_REF);
			TSTR name;

			if( pLayer != NULL)
			{
				name = pLayer->GetName();
				char * pszSrc = name.data();

				if( strstr( pszSrc, "COL_") == pszSrc)
					bCollisionLayer = true;
			}
		}

		ObjectState os = pINode->EvalWorldState( time);

		if(os.obj)
		{
			// Targets are actually geomobjects but wee do not export them
			if (os.obj->SuperClassID() == SClass_ID(TARGET_CLASS_ID))
				return;

			if( os.obj->ClassID() == UNDERHULL_CLASS_ID)
				return;

			ulong partA = os.obj->ClassID().PartA();
			ulong partB = os.obj->ClassID().PartB();

			// 해당 Node가 Mesh를 포함하고 있느냐.
			if( os.obj->SuperClassID() == SClass_ID( LIGHT_CLASS_ID))
			{
				// Light
				if( m_pExport->m_Option.m_bExportLight)
				{
					pJobInfo = addJob( pINode, NPT_LIGHT);
				}
			}
			else if( os.obj->SuperClassID() == SClass_ID( GEOMOBJECT_CLASS_ID))
			{
				// Geometry
				bool bExportGeom;

				bExportGeom =	(m_pExport->m_Option.m_bExportMesh) &&	// Option에서 Mesh가 Export 가능 설정이어야 하며
								( bCollisionLayer == false) &&			// COL_... 이름의 Layer가 아니며
								( IsBone( os.obj) == false) && 
								( IsBiped( pINode) == false) &&
								(( m_pExport->_IsPhysixNode( pINode) == false) || ( m_pExport->m_Option.m_bExportPhysicsGeometry == true)) &&
								( pINode->Renderable());


				if( bExportGeom)
					pJobInfo = addJob( pINode, NPT_MESH);
			}
			else if( os.obj->SuperClassID() == SClass_ID( CAMERA_CLASS_ID))
			{
				// Camera
				if( m_pExport->m_Option.m_bExportCamera)
				{
					pJobInfo = addJob( pINode, NPT_CAMERA);
				}
			}
			else if( os.obj->SuperClassID() == SClass_ID( SHAPE_CLASS_ID))
			{
				// Shape
			}
			else if( os.obj->SuperClassID() == SClass_ID( HELPER_CLASS_ID))
			{
				// Helper Object
			}
			else
			{
			}

			bool bScanBone = (pJobInfo != NULL);

			// Physix 관련 처리를 해야 할 것이 있다면..
			if( m_pExport->_IsPhysixNode( pINode))
			{
				if( pJobInfo == NULL)
					pJobInfo = addJob( pINode, NPT_PHYSIX);
				else
					pJobInfo->m_NPT |= NPT_PHYSIX;

				// Proxy가 설정된 Convex의 경우 실제 Bone Hierarchy는 Export될 필요가 없다.
				if( GetProxyProperty( pINode))
					bScanBone = false;
			}

			{
				INT32 forceExport = FALSE;

				if( pINode->GetUserPropInt( TSTR("ForceExport"), forceExport) == TRUE)
				{
					if( forceExport != FALSE)
					{
						// 강제 Export 설정된 파일이이라면..
						bScanBone = true;
					}
				}
			}

			if( bScanBone)
			{
				// 처리대상임이 확인된 Node라면 이 Node와 연관된 Bone Node들을 검색한다.
				ScanUsedBone( pINode);
			}
		}
	}

	for( i = 0; i < pINode->NumberOfChildren(); i++)
	{
		INode * pChild = pINode->GetChildNode( i);

		ClassifyNode( pChild);
	}
}

static void		DumpController( Animatable * pCtrl, INT32 depth)
{
	ulong partA = pCtrl->ClassID().PartA();
	ulong partB = pCtrl->ClassID().PartB();
	INT32 i;

	for( i = 0; i < depth; i++)
	{
		I3TRACE( "\t");
	}
	
	I3TRACE( "controller Id : %x %x\n", partA, partB);

	for( i = 0; i < pCtrl->NumSubs(); i++)
	{
		Animatable * pChild = pCtrl->SubAnim( i);

		DumpController( pChild, depth + 1);
	}
}

// 해당 Node가 Bone으로 취급되어야 할지, 그렇다면 구체적으로 어떤 종류로 취급되어야 할지 확인한다.
BCR_TYPE SgBuilder::getBoneTreatType( INode * pINode)
{
	// bone으로 간주되어야 하는 조건은 아래와 같다.
	// 1) Biped나 Physique이다. -> 이 경우 ScanUsedBone 함수에서 Geometry를 통해 사용된 Bone을 검색하기 때문에 여기에서는 처리하지 않는다.
	// 2) Node의 Type이 Bone이나 Dummy이다.
	// 3) 노드의 이름이 "Bone"이나 "Pivot"으로 시작한다.
	// 4) "ForceExport" Property를 가지고 있다.
	// 5) 노드 자체에 Physix 설정이 있다.
	// 6) Physix의 Proxy로 설정된 노드이다.
	// 7) Animation을 가지고 있다.

	// 검색의 속도를 올리기 위해 미리 Bone Type이 확인된 Node의 경우에는
	// 재검색을 하지 않도록한다.
	{
		SCAN_NODE_INFO * pInfo = FindScanNode( pINode);

		if( pInfo != NULL)
		{
			return pInfo->m_BoneType;
		}
	}

	BCR_TYPE type = BCR_NO_BONE;

	if( IsFootPrint( pINode))
		goto ExitPart;

	// 1)
	if( IsBiped( pINode))
	{
		// 이름의 끝에서 3자가 "Nub"이라면 Export에서 제외한다.
		char * pszName = pINode->GetName();
		INT32 len = i3::generic_string_size( pszName);

		if( len >= 3)
		{
			if( i3::generic_is_niequal( pszName + len - 3, "Nub", 3) )
			{
				// Nub이다...더 이상 볼 것도 없다.
				goto ExitPart;
			}
		}

		type = BCR_SKIN_BONE;
		goto ExitPart;
	}

	// Billboard Controller를 가지고 있는 경우..
	{
		::Animatable * pCtrl;

		pCtrl = pINode->GetTMController();

		if( pCtrl != NULL)
		{
			//DumpController( pCtrl, 0);

			if( pCtrl->ClassID() == Class_ID( PRS_CONTROL_CLASS_ID, 0))
			{
				// PRS Controller

				// Rotation Controller를 구한다.
				pCtrl = pCtrl->SubAnim( 1);
				if( pCtrl != NULL)
				{
					if( pCtrl->ClassID() == Class_ID( LOOKAT_CONSTRAINT_CLASS_ID, 0))
					{
						type = BCR_BILLBOARD_BONE;
						goto ExitPart;
					}
				}
			}
		}
	}

	// 2) Node의 Type이 Bone이나 Dummy이다.
	if( IsBone( pINode->GetObjectRef()) || IsDummy( pINode->GetObjectRef()))
	{
		if( pINode->NumberOfChildren() > 0)
		{
			type = BCR_BONE;
			goto ExitPart;
		}
	}

	// 3) 노드의 이름이 "Bone"이나 "Pivot"으로 시작한다.
	if( i3::generic_is_niequal( pINode->GetName(), "Bone", 4) ||
		i3::generic_is_niequal( pINode->GetName(), "Pivot", 5) )
	{
		type = BCR_BONE;
		goto ExitPart;
	}

	// 4) "ForceExport" Property를 가지고 있다.
	INT32 forceExport;

	if( pINode->GetUserPropInt( TSTR("ForceExport"), forceExport) == TRUE)
	{
		if( forceExport)
		{
			type = BCR_BONE;
			goto ExitPart;
		}
	}

	// 5) 노드 자체에 Physix 설정이 있다.
	if( m_pExport->_IsPhysixNode( pINode))
	{
		if( GetProxyProperty( pINode))
		{
			// Proxy 설정이 있다면 그 자체는 단순히 Shape 정보만 포함하고 있는 것이라 볼 수 있다.
			// 이 경우 Bone으로 간주하지 않아야 한다.
			goto ExitPart;
		}
		else
		{
			// Proxy 설정이 없는 Node만이 Bone이 될 수 있다.
			type = BCR_PHSIX_BONE;
			goto ExitPart;
		}
	}

	// 6) Physix의 Proxy로 설정된 노드이다.
	// 아래의 함수는 매우 느리다.
	if( m_pExport->_IsReferencedForProxy( pINode))
	{
		type = BCR_PHSIX_BONE;
		goto ExitPart;
	}

	// 7) Animation을 가지고 있다.
	if( ::IsAnimation( pINode))
	{
		type =BCR_BONE;
		goto ExitPart;
	}

ExitPart:

	// 새로 확인된 Node의 Bone Type을 재활용을 위해 List에 넣어둔다.
	SCAN_NODE_INFO * pInfo = new SCAN_NODE_INFO;

	pInfo->m_BoneType = type;
	pInfo->m_pINode = pINode;

	m_ScanNodeList.insert( pInfo);

	return type;
}

void SgBuilder::ScanUsedBone( INode * pINode, bool bScanVertex, INT32 depth)
{
	// 해당 노드가 Mesh를 가지고 있으며 Skinning 된 것이라면,
	// Vertex들에 의해 Weighted Bone으로 참조되고 있는 모든 Bone을 추가해야 한다.
	if( bScanVertex)
	{
		BONETYPE boneType;
		Modifier * pMod;

		pMod = FindBonesModifier( pINode, &boneType);

		switch( boneType)
		{
			case BONES_3DS :		// Skin
				ScanUsedBone_Skin( pINode, pMod);
				break;

			case PHYSIQUE :			// Physique
				ScanUsedBone_Physique( pINode, pMod);
				break;
		}
	}

	// 해당 Node에서부터 최상위 Parent까지 따라가면서 Transform Node가 필요한지
	// 확인하고, 만약 필요하다면 Bone List에 추가한다.
	INT32 cnt;

	for( cnt = 0; pINode != NULL; cnt++)
	{
		BCR_TYPE type;
		bool bSkinned;

		INT32 idx = findBoneByINode( pINode);

		if( idx == -1)
		{
			type = getBoneTreatType( pINode);
		
			bSkinned = ( type == BCR_SKIN_BONE);

			if((pINode->GetParentNode() != NULL) && ((cnt > 0) || ( type != BCR_NO_BONE)))
			{
				// 기존에 Bone 정보가 존재하지 않았다면 추가한다.
				addBone( pINode, bSkinned);
			}
		}
		else
		{
			// 존재한다면 Skinned 상태만 갱신한다.
			BONE_INFO * pBoneInfo = getBone( idx);

			if( pBoneInfo->m_bSkinned == false)
			{
				type = getBoneTreatType( pINode);
		
				bSkinned = ( type == BCR_SKIN_BONE);

				// 한번이라도 Skinned Bone으로 사용된 Bone이라면 항상 Skinned 상태로 남아야 한다.
				pBoneInfo->m_bSkinned = pBoneInfo->m_bSkinned || bSkinned;
			}
		}

		pINode = pINode->GetParentNode();
	}
}

void SgBuilder::ScanUsedBone_Skin( INode * pINode, Modifier * pMod)
{
	INT32 i, blendBoneCount, j;
	ISkin * pISkin = NULL;
	ISkinContextData * pSkinCtx = NULL;
//	i3::vector<INode*> BoneList;		// 쓰이지 않아 주석..
	INT32 weightCount;
		
	pISkin = (ISkin *) pMod->GetInterface( I_SKIN);
	if( pISkin == NULL)
		return;

	pSkinCtx = pISkin->GetContextInterface( pINode);
	if( pSkinCtx == NULL)
		goto ExitPart;

	weightCount = pSkinCtx->GetNumPoints();

	// 우선 Vertex에 의해 Reference되고 있는 모든 Bone Node들의 List를 작성한다.
	for( i = 0; i < weightCount; i++)
	{
		blendBoneCount = pSkinCtx->GetNumAssignedBones( i);

		for( j = 0; j < blendBoneCount; j++)
		{
			INT32 boneIdx = pSkinCtx->GetAssignedBone( i, j);
			INode * pBone = pISkin->GetBone( boneIdx);

			if( pSkinCtx->GetBoneWeight( i, j) > 0.0f)
			{
				// Weight가 0 이상이라면 해당 Bone을 Bone Hierarchy에 추가한다.
				// 실제 Hierarchy 구성은 뒤에 한꺼번에 한다.
				INT32 idx = findBoneByINode( pBone);

				if( idx == -1)
				{
					addBone( pBone, true, pINode);
				}
				else
				{
					BONE_INFO * pInfo = getBone( idx);

					pInfo->m_bSkinned = true;
				}

				// Parent Bone들에 대해서도 모두 boneTree를 구성하도록..
				// 중복처리될 수 있다.
				{
					INode * pParent = pBone->GetParentNode();

					if( pParent != NULL)
					{
						ScanUsedBone( pParent, false);
					}
				}
			}
		}
	}

ExitPart:
	if( pISkin != NULL)
	{
		pMod->ReleaseInterface( I_SKIN, pISkin);
		pISkin = NULL;
	}
}

void SgBuilder::ScanUsedBone_Physique( INode * pINode, Modifier * pMod)
{
	INT32 i, blendBoneCount;
	IPhyVertexExport * pVertexExport = NULL;
	IPhysiqueExport * phyExport = NULL;
	IPhyContextExport * mcExport = NULL;
//	i3::vector<i3Node*> BoneList;		//쓰이지 않아 주석..
	INT32 weightCount;
		
	phyExport = (IPhysiqueExport *)pMod->GetInterface( I_PHYINTERFACE);
	if( phyExport == NULL)
		return;

	mcExport = (IPhyContextExport *)phyExport->GetContextInterface( pINode);
	if( mcExport == NULL)
		goto ExitPart;

	mcExport->ConvertToRigid( TRUE);
	mcExport->AllowBlending( TRUE);

	if( mcExport->GetNumberVertices() <= 0)
		goto ExitPart;

	weightCount = mcExport->GetNumberVertices();

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
					
					for( int x = 0; x < blendBoneCount; x++)
					{
						INode * pBone = vtx->GetNode( x);

						INT32 idx = findBoneByINode( pBone);
						
						if( idx == -1)
							addBone( pBone, true, pINode);
						else
						{
							BONE_INFO * pInfo = getBone( idx);

							pInfo->m_bSkinned = true;
						}

						// Parent Bone들에 대해서도 모두 boneTree를 구성하도록..
						// 중복처리될 수 있다.
						{
							INode * pParent = pBone->GetParentNode();

							if( pParent != NULL)
								ScanUsedBone( pParent, false);
						}
					}
				}
				break;

			case RIGID_NON_BLENDED_TYPE :
				{
					IPhyRigidVertex * vtx = (IPhyRigidVertex *) pVertexExport;

					INode * pBone = vtx->GetNode();

					INT32 idx = findBoneByINode( pBone);

					if( idx == -1)
					{
						addBone( pBone, true, pINode);
					}
					else
					{
						BONE_INFO * pInfo = getBone( idx);

						pInfo->m_bSkinned = true;
					}

					// Parent Bone들에 대해서도 모두 boneTree를 구성하도록..
					// 중복처리될 수 있다.
					{
						INode * pParent = pBone->GetParentNode();

						if( pParent != NULL)
							ScanUsedBone( pParent, false);
					}
				}
				break;

			default :
				break;
		}

		mcExport->ReleaseVertexInterface( pVertexExport);
		pVertexExport = NULL;
	}

ExitPart:
	if( phyExport != NULL)
	{
		pMod->ReleaseInterface( I_PHYINTERFACE, phyExport);
		phyExport = NULL;
	}
}

BONE_INFO * SgBuilder::FindParentBone( INode * pBone, bool bDump)
{
	INode * pParent = pBone->GetParentNode();

	while( pParent != NULL)
	{
		if( bDump)
			I3TRACE( "Parent : %s\n", pParent->GetName());

		INT32 idx = findBoneByINode( pParent);

		if( idx != -1)
			return getBone( idx);

		pParent = pParent->GetParentNode();
	}

	return NULL;
}

void SgBuilder::findBoneBySkinMesh( INode * pIMeshNode, i3::vector<BONE_INFO*>& List)
{
	INT32 i;
	BONE_INFO * pBoneInfo;

	for( i = 0; i < getBoneCount(); i++)
	{
		pBoneInfo = (BONE_INFO *) getBone( i);

		if( pBoneInfo->m_pSkinMesh == pIMeshNode)
			List.push_back( pBoneInfo);
	}
}

//
// 검색된 BONE_INFO에 대해 Tree Structure로 구성해주는 함수.
// 
// 모든 BONE_INFO가 검색되어 있어야 한다.
//
// Result 
//		1)	전체 Bone들의 Parent가 되는 m_pBoneRoot가 검색된다.
//		2)	m_pBoneRoot를 Root로 해, 모든 BONE_INFO들이 Parent-Child 관계로 연결된다.
//
void SgBuilder::BuildBoneHierarchy(void)
{
	BONE_INFO * pInfo, * pParentInfo;
	i3::vector<BONE_INFO*>	RootList;

	m_pBoneRoot = NULL;

	SetProgress( PRGSS_BUILD_BONE_HIER, getBoneCount());

	for(INT32 i = 0; i < getBoneCount(); i++)
	{
		pInfo = (BONE_INFO *) getBone( i);

		INode * pBone = pInfo->m_pINode;

		pParentInfo = FindParentBone( pBone);

		if( pParentInfo == NULL)
		{
			// Parent Node가 없다면 무조건 이 Node는 Root가 된다.
			RootList.push_back( pInfo);
		}
		else
		{
			pParentInfo->AddChild( pInfo);
		}

		Progress();
	}

	// 만약 Root 후보가 하나도 없거나 1개 이상이라면
	// 최상위 노드를 임시로 만들어 붙여준다.
	if( RootList.size() != 1)
	{
		m_pBoneRoot = addBone( m_pIRoot, false);

		for(size_t i = 0; i < RootList.size(); i++)
		{
			BONE_INFO * pTemp = RootList[i];

			m_pBoneRoot->AddChild( pTemp);
		}
	}
	else
	{
		m_pBoneRoot = RootList[0];
	}

	// m_pBoneRoot를 시작으로 하위의 Bone들을 재검색한다.
	//
	// <예외1>
	// 이전까지 구성된 Bone List들은 모두 Mesh나 Physix의 참조에 의해서만 사용이 확인된 Bone들이다.
	// 그러나 참조되지 않지만 실제로는 함께 Export되어야 하는 Node들이 있을 수 있기 때문에
	// 다시 한번 Top->Bottom Style로 추가적인 Bone을 검색한다.
	// 실제로 Biped 밑에 붙은 Dummy 및 Bone들이 여기에 포함되는 경우일 수 있다.
	//
	// <예외2>
	// Skin Bone와 하위의 Skin Bone 사이에 Non-Skin Bone이 끼어있는 경우가 발견된다.
	// 이런 경우, 중간에 끼어있는 Non-Skin bone은 Skin Bone으로 간주해 준다.

	_Rec_SearchBoneHierarchy( m_pBoneRoot->m_pINode, 0, 0);

	m_pBoneRoot->dump();

	//
	// 동일한 SkinMesh에 의해 Skin Bone으로 검색된 Bone들의 경우,
	// 반드시 하나의 i3Skeleton으로 구성될 필요가 있다.
	// 그러나, 흔히 Leaf Bone에만 Weight가 걸린 이유로 상위 Bone들은 Weight가 없고,
	// 따라서 Skin Bone으로 간주되지 못하는 현상이 있다.
	// 
	// 아래는 동일한 SkinMesh를 사용하는 Bone들을 검색해,
	// 상위로 따라 올라가면서 공통 부모를 찾고,
	// 그 최상위 공통 부모 Node에서부터 각 말단 Node까지의 모든 Parent들은 강제로
	// Skin Bone으로 설정해주어 하나의 i3Skeleton으로 Export되도록 한다.
	{
		i3::vector<INode*>	 MeshList;
		i3::vector<BONE_INFO*> boneList;
		BONE_INFO * pSharedParent;

//		MeshList.SetOnceMode( TRUE);

		// Bone들을 검색해 Skinning Mesh들을 검색한다.
		GatherSkinMesh( m_pBoneRoot, MeshList);
		i3::vu::remove_duplicates(MeshList);

		// 각 SkinMesh에 대해 역으로 관련 Bone들을 구한다.
		for(size_t i = 0; i < MeshList.size(); i++)
		{
			INode * pSkinMesh = MeshList[i];

			I3TRACE( "-------- %s Skin Mesh에 대한 공통부모 검색.\n", pSkinMesh->GetName());

			findBoneBySkinMesh( pSkinMesh, boneList);

			pSharedParent = FindSharedMostParent( boneList);

			if( pSharedParent != NULL)
			{
				I3TRACE( " 공통 부모 : %s\n", pSharedParent->m_pINode->GetName());

				// 공통 Parent Bone을 찾았다.
				// 이 Bone에서부터 각 Leaf Node들에 이어지는 모든 Bone들에 대해 Skin Bone으로 설정한다.

				for(size_t j = 0; j < boneList.size(); j++)
				{
					BONE_INFO * pTemp = boneList[j];

					while( pTemp != NULL)
					{
						if( pTemp->m_bSkinned == false)
						{
							I3TRACE( "%s -> skin bone으로 변경\n", pTemp->m_pINode->GetName());
							pTemp->m_bSkinned = true;
						}

						if( pTemp == pSharedParent)
							break;

						pTemp = (BONE_INFO *) pTemp->getParent();
					}
				}
			}

			I3TRACE( "--------\n");

			boneList.clear();
		}
	}
	m_pBoneRoot->dump();
}

// List의 Bone들의 공통 Parent Bone을 구한다.
BONE_INFO *		SgBuilder::FindSharedMostParent( const i3::vector<BONE_INFO*>& BoneList)
{
	BONE_INFO * pTemp, * pSharedParent = NULL;
	i3::vector<BONE_INFO *>	baseList;
	i3::vector<BONE_INFO *> parentList;

	// 관련 Bone이 1개 이하라면 검색할 필요없음.
	if( BoneList.size() < 2)
		return NULL;

//	parentList.SetOptmizeEnableWhenClear( FALSE);

	{
		// 첫번째 Bone
		pTemp = BoneList[0];
		GatherBoneHier( pTemp, baseList);
	}

	for(size_t i = 1; i < BoneList.size(); i++)
	{
		pTemp = BoneList[i];

		GatherBoneHier( pTemp, parentList);

		// baseList를 돌면서 parentList에서 발견되지 않는 Bone들을 제거한다.
		// 공통 Parent를 확인하기 위한 방법
		for(size_t j = 0; j < baseList.size(); j++)
		{
			BONE_INFO * pParent = baseList[j];

			if ( i3::vu::int_index_of(parentList, pParent) == -1 )
			{
				// 공통되지 않는다. 삭제
				i3::vu::erase_index(baseList, j);
				j--;
			}
		}

		parentList.clear();
	}

	for(size_t i = 0; i < baseList.size(); i++)
	{
		pTemp = baseList[i];

		I3TRACE( "Leaved Bone[%d] : %s\n", i, pTemp->m_pINode->GetName());
	}

	// baseList에 남아있는 bone들이 공통 bone이다.
	// 이 중 가장 첫번째 것이 Leaf Bone들과 가장 가까운 Bone이다.
	if( baseList.size() > 0)
	{
		pSharedParent = baseList[0];
	}

	return pSharedParent;
}

// Bone List를 돌면서 발견된 Skin Mesh를 검색한다.
void SgBuilder::GatherSkinMesh( BONE_INFO * pNode, i3::vector<INode*>& List)
{
	INT32 i;
	BONE_INFO * pBone;

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone( i);

		if( pBone->m_pSkinMesh != NULL)
			List.push_back( pBone->m_pSkinMesh);
	}
}

void SgBuilder::_Rec_SearchBoneHierarchy( INode * pINode, INT32 Depth, INT32 skinDepth)
{
	INT32 idx;
	BONE_INFO * pBoneInfo = NULL;

	idx = this->findBoneByINode( pINode);
	if( idx == -1)
	{
		// 해당 INode에 대한 BONE_INFO가 존재하지 않는다.
		// Bone으로 Export해야 하는 INode인지 확인부터 한다.
		if( isTargetToExport(pINode, m_pExport))
		{
			BCR_TYPE type = getBoneTreatType( pINode);

			bool bSkinned = ( type == BCR_SKIN_BONE);

			if( type != BCR_NO_BONE)
			{
				// Hierarchy 연결을 해준다.
				INode * pParentNode = pINode->GetParentNode();
				I3ASSERT( pParentNode != NULL);

				idx = findBoneByINode( pParentNode);
				if( idx == -1)
				{
					I3TRACE( "Missing Parent : %s\n", pParentNode->GetName());

					// 어렵군...
					// 부모가 존재하지 않는다는 것은 이 노드만 특별하게 Export 대상으로 검색된 경우라고 판단.
					// 이 Bone은 Export되지 않도록 해주어야 한다.
				}
				else
				{
					// 기존에 Bone 정보가 존재하지 않았다면 추가한다.
					pBoneInfo = addBone( pINode, bSkinned);

					BONE_INFO * pParentBone = getBone( idx);

					pParentBone->AddChild( pBoneInfo);
				}
			}
		}
	}
	else
	{
		pBoneInfo = getBone( idx);
	}

	if( pBoneInfo != NULL)
	{
		// "Bone", "Pivot" 이름을 가진 Bone의 경우에는 Weight가 걸려있지 않은 경우라도
		// Parent Node가 Skinning Bone이라면 Skinning Bone으로 간주해야 한다.
		if( i3::generic_is_niequal( pINode->GetName(), "Bone", 4)  ||
			i3::generic_is_niequal( pINode->GetName(), "Pivot", 5) )
		{
			if( pBoneInfo->m_bSkinned == false)
			{
				BONE_INFO * pParentInfo = (BONE_INFO *) pBoneInfo->getParent();

				if( pParentInfo->m_bSkinned)
					pBoneInfo->m_bSkinned = true;
			}
		}

		// SBone-Bone-SBone 상태를 해결하기 위한 코드

		if( pBoneInfo->m_bSkinned)
			skinDepth ++;				// 현재 Bone이 Skin Bone인 경우에만 증가.

		if( skinDepth > 0)
			Depth ++;					// Skin Bone이 발견된 이 후 계속 증가.

		if( pBoneInfo->m_bSkinned)
		{
			// 현재 Bone이 Skin Bone이고 Depth > skinDepth 상태라면
			// Skin Bone과 Skin Bone 사이에 Non-Skin Bone이 있다는 뜻.
			// Parent를 따라가며 중간 Bone들을 Skin Bone으로 설정해 준다.
			if( Depth != skinDepth)
			{
				INT32 i;
				BONE_INFO * pTemp = (BONE_INFO *) pBoneInfo->getParent();

				for( i = 0; i < (Depth - skinDepth); i++)
				{
					pTemp->m_bSkinned = true;

					pTemp = (BONE_INFO *) pTemp->getParent();
				}

				skinDepth = Depth;
			}
		}
	}

	INT32 i;
	INode * pChild;

	for( i = 0; i < pINode->NumberOfChildren(); i++)
	{
		pChild = pINode->GetChildNode( i);

		_Rec_SearchBoneHierarchy( pChild, Depth, skinDepth);
	}
}

//
// pInfo를 포함하고 있는 i3Skeleton을 찾는다.
//
i3Body *	SgBuilder::FindSkeleton( BONE_INFO * pInfo)
{
	INT32 i;

	for( i = 0; i < getSkelCount(); i++)
	{
		SKELETON_INFO * pSkelInfo = (SKELETON_INFO *) getSkel( i);

		i3Skeleton * pSkel = pSkelInfo->m_pSkel;
		I3ASSERT( pSkel != NULL);

		if( pSkel->FindBoneByName( pInfo->m_pINode->GetName()) != -1)
			return pSkelInfo->m_pBody;
	}

	return NULL;
}

INT32		SgBuilder::_Rec_BuildSkeleton( BONE_INFO * pInfo, i3Skeleton * pSkel, INT32 cnt, INT32 depth)
{
	if( pInfo->m_bSkinned == false)
		return cnt;

	INT32 i;
	INode * pBone = pInfo->m_pINode;

	if( pSkel != NULL)
	{
		MATRIX mtxBone;
		MATRIX invMtx;

		// 기본 Transform
		{
			ExtractTransform( pInfo->m_pINode, &mtxBone, m_pExport->getStartTime());

			pSkel->setMatrix( cnt, &mtxBone);
		}

		if( i3::generic_is_iequal( pInfo->m_pINode->GetName(), "BN_Coat_TpRGT_Clavicle") )
		{
			I3TRACE( "mirrored --------------\n");
			i3Matrix::Dump( &mtxBone);
		}
		else if( i3::generic_is_iequal( pInfo->m_pINode->GetName(), "BN_Coat_TpLFT_Clavicle") )
		{
			I3TRACE( "original --------------\n");
			i3Matrix::Dump( &mtxBone);
		}
		
		// Inverse Bone Matrix를 가져온다.
		{
			Matrix3 mtx;

			ScaleBiped( pBone, TRUE);

			mtx = __NODE_TM( pBone, 0);

			ScaleBiped( pBone, FALSE);

			mtx.Invert();

			//mtx = mtx;
			::Convert( &invMtx, &mtx);
			pSkel->setInverseMatrix( cnt, &invMtx);
		}

		// Name
		pSkel->setName( cnt, pBone->GetName());

		// Parent Bone Index
		{
			INT32 parentIdx = -1;
			
			if( pBone->GetParentNode() != NULL)
				parentIdx = pSkel->FindBoneByName( pBone->GetParentNode()->GetName());

			pSkel->setParentBoneIndex( cnt, parentIdx);
		}

		// Depth
		pSkel->setDepth( cnt, depth);
	}

	cnt++;

	for( i = 0; i < pInfo->getChildCount(); i++)
	{
		BONE_INFO * pChild = (BONE_INFO *) pInfo->getChild( i);

		cnt = _Rec_BuildSkeleton( pChild, pSkel, cnt, depth + 1);
	}

	return cnt;
}

i3Body *	SgBuilder::BuildSkeleton( BONE_INFO * pInfo)
{
	INT32 cnt;
	i3Body * pBody;
	i3Skeleton * pSkel;
	
	// i3Skeleton을 생성하기 전, 필요한 Bone 개수를 구한다.
	cnt = _Rec_BuildSkeleton( pInfo, NULL, 0, 0);

	if( cnt <= 0)
		return NULL;

	// i3Skeleton 생성.
	{
		pSkel = i3Skeleton::new_object_ref();
		pSkel->SetBoneCount( cnt);

		i3MatrixArray * pInvBoneArray = i3MatrixArray::new_object_ref();
		pInvBoneArray->Create( cnt);

		pSkel->getBoneMatrixListAttr()->setInverseBoneArray( pInvBoneArray);
	}

	// 실제 i3Skeleton에 Transform 및 Animation 키값 생성.
	{
		_Rec_BuildSkeleton( pInfo, pSkel, 0, 0);
		// 위의함수에의해 사용하는본들은 i3BoneMatrixListAttr에 기록됩니다.
		// i3Skeleton::FindBoneByName() 에 최신의 i3BoneMatrixListAttr를 반영할필요가 있습니다.
		pSkel->UpdateBoneMatrixListAttrInfo();
	}

	pBody = i3Body::new_object_ref();
	
	{
		i3LOD * pLOD = i3LOD::new_object_ref();
		pLOD->setSkeleton( pSkel);

		pBody->AddLOD( pLOD);
	}

	{
		// SKELETON_INFO
		SKELETON_INFO * pSkelInfo = new SKELETON_INFO;

		pSkelInfo->m_pBody = pBody;
		pSkelInfo->m_pBoneRoot = pInfo->m_pINode;
		pSkelInfo->m_pSkel = pSkel;

		m_SkelList.push_back( pSkelInfo);
	}

	return pBody;
}


void SgBuilder::_Rec_BuildFrameSg( BONE_INFO * pInfo, BONE_INFO * pParentInfo)
{
	Progress();

	if( pInfo->m_bSkinned)
	{
		if( pParentInfo->m_bSkinned == false)
		{
			// 처음으로 Skin Bone이 발견되었다.
			// 현재 Bone을 Root로 해, i3Skeleton을 구성한다.
			i3Body * pBody = BuildSkeleton( pInfo);
			I3ASSERT( pBody != NULL);

			pBody->SetName( pInfo->m_pINode->GetName());

			pInfo->m_pNode = pBody;

			// i3Body는 단순히 Parent Node에 붙인다.
			pParentInfo->m_pNode->AddChild( pBody);
		}
		else
		{
			// 현재 Bone을 포함한 i3Skeleton을 찾는다.
			i3Body * pBody = FindSkeleton( pInfo);

			if( pBody == NULL)
			{
				I3TRACE( "Inalid Skin bone : %s\n", pInfo->m_pINode->GetName());
				return;
			}

			pInfo->m_pNode = pBody;

			// 현재 Bone은 이미 i3Skeleton의 한 부분으로 포함되어 있기 때문에
			// 추가적인 Node 구성은 불필요하다.
		}
	}
	else
	{
		BCR_TYPE type = getBoneTreatType( pInfo->m_pINode);
		i3Transform * pTrans;
		i3Node *	 pCreatedNode = NULL;

		switch( type)
		{
			//case BCR_BONE :
				
				//break;

			case BCR_PHSIX_BONE :
				pCreatedNode = pTrans = i3Transform2::new_object_ref();
				break;

			default :
				/*
				// Bone이 아닌 경우에 대해서도 문제시하지 않는다.
				pCreatedNode = i3Node::new_object_ref();

				// 이 후의 처리를 하지 않도록 하기 위해 NULL 처리.
				pTrans = NULL;
				*/
				pCreatedNode = pTrans = i3Transform::new_object_ref();
				break;
		}

		pCreatedNode->SetName( pInfo->m_pINode->GetName());

		if( pTrans != NULL)
		{
			// 기본 Transform Matrix
			{
				MATRIX Matrix;

				ExtractTransform( pInfo->m_pINode, &Matrix, m_pExport->getStartTime());
				
				pTrans->SetMatrix( &Matrix);
			}
		}

		// i3Transform Node를 Parent Node에 붙인다.

		if( pParentInfo->m_bSkinned)
		{
			// Parent가 Skin인 경우, i3BoneRef로 추가되어야 함을 나타낸다.
			//I3ASSERT( pParentInfo->m_pNode != NULL);
			I3ASSERT( i3::kind_of<i3Body*>(pParentInfo->m_pNode)); //->IsTypeOf( i3Body::static_meta()));
			//i3Body * pBody = FindSkeleton( pParentInfo);
			i3Body * pBody = (i3Body *) pParentInfo->m_pNode;

			i3LOD * pLOD = pBody->getLOD( 0);
			I3ASSERT( pLOD != NULL);

			i3Skeleton * pTempSkel = pLOD->getSkeleton();

			INT32 idxBone = pTempSkel->FindBoneByName( pParentInfo->m_pINode->GetName());
			if( idxBone == -1)
			{
				I3TRACE( "Bone : %s\n", pInfo->m_pINode->GetName());
				I3TRACE( "Skinning bone(%s)에 붙어 있다고 했지만 i3Skeleton에서는 그 Bone을 찾을 수없음.\n", pParentInfo->m_pINode->GetName());
				I3ASSERT( idxBone != -1);
			}
			else
			{
				i3BoneRef * pBoneRef = i3BoneRef::new_object_ref();

				pBoneRef->setBoneIndex( idxBone);
				pBoneRef->SetName( pInfo->m_pINode->GetName());

				// i3Body의 Child로 붙인다.
				pBody->AddChild( pBoneRef);

				// i3Transform 노드는 i3BoneRef의 Child로 붙인다.
				pBoneRef->AddChild( pCreatedNode);
			}
		}
		else
		{
			// 이전 Node의 Child로 단순히 Attach
			pParentInfo->m_pNode->AddChild( pCreatedNode);
		}

		if( type == BCR_BILLBOARD_BONE)
		{
			i3TransformBillboard * pBillboard = i3TransformBillboard::new_object_ref();

			pBillboard->setAnchorEnable( false);

			pCreatedNode->AddChild( pBillboard);

			// Billboard
			pCreatedNode = pBillboard;

		}

		pInfo->m_pNode = pCreatedNode;
	}

	INT32 i, skinBoneCount = 0;

	for( i = 0; i < pInfo->getChildCount(); i++)
	{
		BONE_INFO * pChildInfo = (BONE_INFO *) pInfo->getChild( i);

		_Rec_BuildFrameSg( pChildInfo, pInfo);

		if( pChildInfo->m_bSkinned)
			skinBoneCount++;
	}

	// 만약 Child가 모두 Skin Bone이라면 자신도 Skin Bone으로 설정한다.
	// 매우 드문 드문 Skin Bone이 있는 경우를 위한 처방.
	if( skinBoneCount == pInfo->getChildCount())
	{
		pInfo->m_bSkinned = true;
	}
}

void SgBuilder::BuildFrameSg(void)
{
	BONE_INFO rootInfo;

	SetProgress( PRGSS_BUILD_FRAME_SG, getBoneCount());

	rootInfo.m_pNode = m_pRoot;

	_Rec_BuildFrameSg( m_pBoneRoot, &rootInfo);
}

void SgBuilder::ExtractAnimation(void)
{
	INT32 i;
	BONE_INFO * pBoneInfo;
	INode * pINode;

	SetProgress( PRGSS_EXTRACT_ANIM, getBoneCount());

	// Animation
	for( i = 0; i < getBoneCount(); i++)
	{
		pBoneInfo = getBone( i);

		pINode = pBoneInfo->m_pINode;

		BOOL bForceExport;
		
		pINode->GetUserPropInt( TSTR("ForceExport"), bForceExport);

		if( m_pExport->m_Option.m_bForceExportAnim || ::IsAnimation( pINode) || bForceExport)
		{
			//if( IsLeafOfBiped( pINode) == false)
			{
				// Biped의 Child Node라면 Animation을 Export하지 않는다.
				m_pExport->_OnExportTransformSeq( pINode);
			}
			// 실제 Animation은 처리가 완전히 끝난 후 설정된다.
		}

		Progress();
	}
}

void SgBuilder::ATTACH_NODE( i3Node * pNode, INode * pINode, bool bGeometry)
{
	BONE_INFO * pParentBone;
	bool bDump = false;

	// 해당 INode에 대응하는 BONE_INFO를 검색한다.
	INT32 idx = findBoneByINode( pINode);

	if( idx == -1)
	{
		// 만약 해당 INode에 대한 것이 없다면 Parent Node들을 따라가며 연결될 수 있는 Bone을 찾는다.
		pParentBone = FindParentBone( pINode, bDump);
	}
	else
	{
		// 찾았다면 해당 Bone으로....
		pParentBone = getBone( idx);
	}

	if( pParentBone == NULL)
	{
		// 완전히 Parent가 없는 경우.
		// 실제 있을 수 있다.
		// m_pRoot에 붙인다.
		m_pRoot->AddChild( pNode);
		return;
	}

	I3ASSERT( pParentBone->m_pNode != NULL);

	if( bGeometry && i3::same_of<i3Body*>(pParentBone->m_pNode) ) //->IsExactTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pParentBone->m_pNode;
		i3LOD * pLOD = pBody->getLOD( 0);

		// Geometry를 i3Body에 붙이는 경우에는 LOD에 대한 Shape Node로 붙여야 한다.

		if( pLOD->getShapeNode() == NULL)
		{
			pLOD->setShapeNode( pNode);
		}
		else
		{
			pLOD->getShapeNode()->AddChild( pNode);
		}
	}
	else
	{
		pParentBone->m_pNode->AddChild( pNode);
	}
}

void SgBuilder::ProcessJobs(void)
{
	INT32 i;
	JOB_INFO * pJob;

	SetProgress( PRGSS_PROCESS_JOB, getJobCount() * 2);

	for( i = 0; i < getJobCount(); i++)
	{
		pJob = (JOB_INFO *) getJob( i);

		if( pJob->m_bProcessed == false)
		{
			if( pJob->m_NPT & NPT_MESH)
			{
				_OnJobMesh( pJob);
			}
			
			if( pJob->m_NPT & NPT_LIGHT)
			{
				_OnJobLight( pJob);
			}

			if( pJob->m_NPT & NPT_CAMERA)
			{
				_OnJobCamera( pJob);
			}

			if( pJob->m_NPT & NPT_PHYSIX)
			{
				_OnJobPhysix( pJob);
			}
		}

		Progress();
	}

	// Physix 설정이 있었던 Node인 경우 Joint 설정을 해주어야 한다.
	// Joint는 모든 Actor가 생성 완료된 이후에 해야하기 때문에
	// 여기서 처리한다.
	for( i = 0; i < getJobCount(); i++)
	{
		pJob = (JOB_INFO *) getJob( i);

		_OnJobJoint( pJob);

		Progress();
	}
}

void SgBuilder::Export( INode * pNode, i3Export * pExp)
{
	m_pIRoot = pNode;
	m_pExport = pExp;

	ResetProgress();

	CheckSameName();

	m_pRoot = i3Transform::new_object();
	m_pRoot->AddFlag( I3_NODEFLAG_CONTROL);

	// 우선 Node들을 돌면서, INode를 분류한다.
	//	결과물 1: Export될 Mesh를 포함하고 있는 INode -> MeshInfo
	//  결과물 2: Bone으로 간주되는 INode -> BoneInfo
	//  결과물 3: Physix 관련 정보를 포함하고 있는 INode -> PhysixInfo
	{
		SetProgress( PRGSS_CLASSIFY_NODE, m_pExport->getTotalNodeCount());

		ClassifyNode( m_pIRoot);
	}

	dumpJob();
	dumpBone();

	BuildBoneHierarchy();				// Skinning으로 사용된 Bone들을 그룹별로 묶어 Skeleton을 구성한다.

	// m_pBoneRoot에서부터 필요한 Hierarchy 정보가 모두 얻어졌으므로
	// 실제 Sg. 뼈대를 구성한다.
	BuildFrameSg();

	//I3_SG_DUMP( m_pRoot);

	// 검색된 Job들을 개별적으로 처리한다.
	ProcessJobs();

	SortNode();
	//I3_SG_DUMP( m_pRoot);

	// Animation을 뽑아낸다.
	ExtractAnimation();

	removeAllJobs();
	removeAllBones();
	removeAllSkeleton();
	RemoveAllScanNode();
}

void SgBuilder::ResetProgress(void)
{
	m_ProgressPhase = 0;
	m_ProgressCur = 0;
	m_ProgressMax = 0;
}

void SgBuilder::SetProgress( PRGSS phase, INT32 max)
{
	m_ProgressPhase = phase;
	m_ProgressCur = 0;
	m_ProgressMax = max;
}

void SgBuilder::Progress(void)
{
	static char *	s_pszProgressTitle[ PRGSS_MAX] =
	{
		"Classifing Nodes",			// PRGSS_CLASSIFY_NODE = 0,
		"Bone hierarchies",			// PRGSS_BUILD_BONE_HIER,
		"Frame of scene graph",		// PRGSS_BUILD_FRAME_SG,
		"Extracting meshes",		// PRGSS_PROCESS_JOB,
		"Extracting Animation",		// PRGSS_EXTRACT_ANIM
	};

	m_ProgressCur++;

	Interface * ip = GetCOREInterface();

	INT32 idx = (m_ProgressPhase * 100) + (m_ProgressCur * 100 / m_ProgressMax);
	INT32 percent = (idx * 100) / (PRGSS_MAX * 100);

	sprintf( m_szProgressText, "%s : (%d%%)", s_pszProgressTitle[ m_ProgressPhase], percent);

	ip->ProgressUpdate( percent, FALSE, m_szProgressText);
}
