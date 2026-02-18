#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "i3EXPTriObject.h"
#include "i3EXPBitmapStorage.h"
#include "iLayer.h"

#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/generic_string_cat.h"

bool i3Export::_Rec_Traverse( INT32 Level, INode * pNode)
{
	INT32 i;
	bool bResult = true;

	#if 0
	{
		// Tab
		for( i = 0; i < Level; i++)
		{
			I3TRACE( "\t");
		}

		ulong partA = pNode->ClassID().PartA();
		ulong partB = pNode->ClassID().PartB();

		// Name
		I3TRACE( "%s : %08X-%08X", pNode->GetName(), partA, partB);

		I3TRACE( "\n");
	}
	#endif

	{
		bool bShow = true, bSelected = true, bBone = false, bExport;

		//bFrozen = pNode->IsFrozen();
		//bShow = !pNode->IsNodeHidden();
		bSelected = (!m_Option.m_bExportSelectedOnly) || pNode->Selected();
		//bBone = (pNode->GetBoneNodeOnOff() == TRUE);

		bExport = (bShow && bSelected) || bBone;

		// if( m_ExcludeNodeList.IndexOf( pNode) != -1)
		if (m_ExcludeNodeList.find(pNode) != m_ExcludeNodeList.end() )
		{
			// 제외 리스트에 있다.
			return TRUE;
		}

		if( bExport)
		{
			if( m_pUserProc != NULL)
			{
				if( m_pUserProc( Level, pNode, m_pUserData) == false)
				{
					goto ExitPart;
				}
			}
		}
	}

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		INode * pChild = pNode->GetChildNode( i);

		if( _Rec_Traverse( Level + 1, pChild) == false)
			goto ExitPart;
	}

	bResult = true;

ExitPart:

	return bResult;
}

void i3Export::Traverse( INode * pRoot, INODE_TRAVERSE_PROC pUserProc, void * pUserData)
{
	m_pUserProc = pUserProc;
	m_pUserData = pUserData;

	_Rec_Traverse( 0, pRoot);
}

void i3Export::_ResetExportContext(void)
{
	I3_SAFE_RELEASE( m_pRoot);
//	m_GeomList.Clear();
	m_TexList.clear();

	m_ExcludeNodeList.clear();

	removeAllProcessInfo();
	_RemoveAllTexture();
	RemoveAllSkeletonInfo();

	g_pPhysixCtx->Dump();

	I3_SAFE_RELEASE( m_pAnim);

	//i3ElementBase::DumpObjects();
}

void i3Export::RefreshExportName(void)
{
	Interface * ip = GetCOREInterface();
	TSTR name = ip->GetCurFileName();

	if( m_bUpdateOption == false)
		return;

	strcpy( m_Option.m_szLastName, name.data());

	i3::remove_ext( m_Option.m_szLastName);
	i3::generic_string_cat( m_Option.m_szLastName, ".i3s");

	UpdateOption( m_hwndDialog);
}

void	i3Export::_AddTriToMeshGroup( MtlBase * pMtl, INT32 idxFace)
{
	i3tool::exporter::MESH_GROUP_INFO * pGrp;
	
	pGrp = _AddMeshGroup( pMtl);

	pGrp->m_FaceList.push_back( idxFace);
}

i3tool::exporter::MESH_GROUP_INFO *	i3Export::_AddMeshGroup( MtlBase * pMtl)
{
	i3tool::exporter::MESH_GROUP_INFO * pGrp;
	INT32 idx;

	idx = _FindMeshGroup( pMtl);
	if( idx == -1)
	{
		pGrp = (i3tool::exporter::MESH_GROUP_INFO *) new i3tool::exporter::MESH_GROUP_INFO;
		pGrp->m_pMtl = pMtl;

		m_MeshGroupList.push_back( pGrp);
	}
	else
	{
		pGrp = m_MeshGroupList[idx];
	}

	return pGrp;
}

INT32	i3Export::_FindMeshGroup( MtlBase * pMtl)
{

	i3tool::exporter::MESH_GROUP_INFO * pGrp;

	for(size_t i = 0; i < m_MeshGroupList.size(); i++)
	{
		pGrp = m_MeshGroupList[i];

		if( pGrp->m_pMtl == pMtl)
			return i;
	}

	return -1;
}

void	i3Export::_RemoveAllMeshGroup(void)
{
	i3tool::exporter::MESH_GROUP_INFO * pGrp;

	for(size_t i = 0; i < m_MeshGroupList.size(); i++)
	{
		pGrp = m_MeshGroupList[i];

		delete pGrp;
	}

	m_MeshGroupList.clear();
}

void	i3Export::_SaveMeshGroup( i3::vector<i3tool::exporter::MESH_GROUP_INFO *>& List)
{
	i3tool::exporter::MESH_GROUP_INFO * pGrp;

	for(size_t i = 0; i < m_MeshGroupList.size(); i++)
	{
		pGrp = m_MeshGroupList[i];

		List.push_back( pGrp);
	}

	m_MeshGroupList.clear();
}

void i3Export::_RestoreMeshGroup( const i3::vector<i3tool::exporter::MESH_GROUP_INFO *>& List)
{
	i3tool::exporter::MESH_GROUP_INFO * pGrp;

	_RemoveAllMeshGroup();

	for(size_t i = 0; i < List.size(); i++)
	{
		pGrp = List[i];

		m_MeshGroupList.push_back( pGrp);
	}
}

//////////////////////////////////////////////////////////////////////////

i3ExpProcessInfo * i3Export::addProcessInfo( INode * pNode)
{
	I3ASSERT( findProcessInfo( pNode) == NULL);

	i3ExpProcessInfo * pInfo = new i3ExpProcessInfo;

	pInfo->setINode( pNode);

	m_ProcessInfoList.push_back( pInfo);

	return pInfo;
}

void i3Export::removeAllProcessInfo(void)
{
	INT32 i;
	i3ExpProcessInfo * pInfo;

	for( i = 0; i < getProcessInfoCount(); i++)
	{
		pInfo = getProcessInfo( i);

		delete pInfo;
	}

	m_ProcessInfoList.clear();

	m_pCurProcessInfo = NULL;
}

i3ExpProcessInfo *	i3Export::findProcessInfo( INode * pNode)
{
	INT32 i;
	i3ExpProcessInfo * pInfo;

	for( i = 0; i < getProcessInfoCount(); i++)
	{
		pInfo = getProcessInfo( i);

		if( pInfo->getINode() == pNode)
			return pInfo;
	}

	return NULL;
}

void i3Export::excludeNode( INode * pNode)
{
	i3ExpProcessInfo * pInfo;

	pInfo = findProcessInfo( pNode);

	// 해당 INode에 대해 생성된 Node는 Child까지 포함하여 제거한다.
	if( pInfo != NULL)
	{
		pInfo->setExcluded( true);
	}
}

//////////////////////////////////////////////////////////////////////////
void i3Export::_OnExportCommonNode( INode * pNode, ObjectState * pState)
{
}

void i3Export::_OnExportShape( INode * pNode, ObjectState * pState)
{
}

void i3Export::_OnExportHelper( INode * pNode, ObjectState * pState)
{
	// Reactor Class일 수 있따.
	UINT32	partA	=	pState->obj->ClassID().PartA();
	UINT32	partB	=	pState->obj->ClassID().PartB();

	switch( partA)
	{
		case DUMMY_CLASS_ID :	I3TRACE( "(Dummy)\n");	break;
		case BONE_CLASS_ID :	I3TRACE( "(Bone)\n");		break;

		default :
			if( pState->obj->ClassID() == REACTOR_RBCOLLECTION)
			{
				I3TRACE( "REACTOR : RBCollection\n");
			}
			else if( pState->obj->ClassID() == REACTOR_HINGE_JOINT)
			{
				I3TRACE( "REACTOR : Hinge\n");
			}
			else
			{
				I3TRACE( "Helper Object : %s : %08X:%08X\n", pNode->GetName(), partA, partB);
			}
			break;
	}
}

void i3Export::_OnExportUnknownNode( INode * pNode, ObjectState * pState)
{
	I3TRACE( "Unknown SuperClass ID : %08X", pState->obj->SuperClassID());

	i3Node * pNewNode = i3Node::new_object_ref();

	pNewNode->SetName( pNode->GetName());

	m_pCurProcessInfo->pushi3Node( pNewNode);
}

////////////////////////////////////////////////////////////////////////////
// Node Counting Traverse
bool	i3Export::_NodeCountProc( INT32 Level, INode * pNode, void * pData)
{
	i3Export * pExport = (i3Export *) pData;
	TimeValue time = pExport->getStartTime();

	pExport->m_TotalNodeCount ++;

	{
		bool bValidNode = false;
		ObjectState os = pNode->EvalWorldState( time);

		if(os.obj)
		{
			// Targets are actually geomobjects but wee do not export them
			if (os.obj->SuperClassID() == SClass_ID(TARGET_CLASS_ID))
			{
				return true;
			}
			else if( os.obj->SuperClassID() == SClass_ID( LIGHT_CLASS_ID))
			{
				// Light
				if( pExport->getOption()->m_bExportLight)
					bValidNode = true;
			}
			else if( os.obj->SuperClassID() == SClass_ID( GEOMOBJECT_CLASS_ID))
			{
				// Geometry
				if( pExport->getOption()->m_bExportMesh)
					bValidNode = true;
			}
			else if( os.obj->SuperClassID() == SClass_ID( CAMERA_CLASS_ID))
			{
				// Camera
				if( pExport->getOption()->m_bExportCamera)
					bValidNode = true;
			}
		}

		if( bValidNode)
		{
			// 무언가 Export되어야 하는 Object가 있는데, 그것의 Parent가
			// Biped라면 Biped를 Exclude하지 않고 처리해야 한다.
			INode * pParent = pNode->GetParentNode();

			if((pParent != NULL) && IsBiped( pNode))
			{
				pExport->m_bExcludeBiped = false;
			}
		
		}
	}

	return true;
}

//#define __NODE_DUMP

#if defined( __NODE_DUMP)
	#define		NODELOG				I3TRACE
#else
	#define		NODELOG				__noop
#endif

////////////////////////////////////////////////////////////////////////////
// Processing Traverse
bool i3Export::_NodeTraverseProc( INT32 Level, INode * pNode, void * pData)
{
	bool	bDummyNode, bCollisionLayer = false;
	Object * pObj = pNode->GetObjectRef();
	i3Export * pExport = (i3Export *) pData;
	TimeValue time = pExport->getStartTime();

	// Progress Update
	pExport->m_ProcessedNodeCount++;
	Interface * ip = GetCOREInterface();
	ip->ProgressUpdate( (INT32)((pExport->m_ProcessedNodeCount * 100) / (pExport->m_TotalNodeCount + 1)));

	if( ip->GetCancel())
	{
		// Stop Traversal 
		return false;
	}

	if( IsFootPrint( pNode))
		return false;

	if( pExport->m_bExcludeBiped && IsBiped( pNode))
	{
		return false;
	}

	// Exclude 조건에 해당하는지 확인
	if( pExport->_IsValidNode( pNode) == false)
	{
		return false;
	}

	if( pExport->m_bNoCheckHide == false)
	{
		if( pNode->IsHidden())
			return false;
	}

	bDummyNode = false;

	#if defined( __NODE_DUMP)
	{
		for( INT32 i = 0; i < Level; i++)
			I3TRACE( "\t");
		I3TRACE( "<%s>", pNode->GetName());
	}
	#endif

	// Layer 확인
	{
		ILayer * pLayer = (ILayer *) pNode->GetReference( NODE_LAYER_REF);
		TSTR name;

		if( pLayer != NULL)
		{
			name = pLayer->GetName();
			char * pszSrc = name.data();

			if( strstr( pszSrc, "COL_") == pszSrc)
			{
				bCollisionLayer = true;
			}
		}
	}

	{
		ObjectState os = pNode->EvalWorldState( time);

		if(os.obj)
		{
			// Targets are actually geomobjects but wee do not export them
			if (os.obj->SuperClassID() == SClass_ID(TARGET_CLASS_ID))
			{
				return true;
			}

			if( os.obj->ClassID() == UNDERHULL_CLASS_ID)
				return true;
		}

		if( pExport->m_bNoAddProcessInfo == false)
		{
			// i3BoneRef Node의 처리에서와 같은 경우에는
			// 외부에서 직접 ProcessInfo를 생성하기 때문에 생성할 필요가 없다.
			pExport->m_pCurProcessInfo = pExport->addProcessInfo( pNode);
		}

		{
			BONETYPE boneType;

			FindBonesModifier( pNode, &boneType);

			switch( boneType)
			{
				case BONES_3DS :
				case PHYSIQUE :
					break;

				default :
					// Physique 및 Skin의 경우에는 i3Transform Node를 추가하면 안된다.
					if( pNode->IsRootNode() == FALSE)
						pExport->_OnExportTransform( pNode, &os);
					break;
			}
		}


		if( os.obj)
		{
			ulong partA = os.obj->ClassID().PartA();
			ulong partB = os.obj->ClassID().PartB();

			// Name
			NODELOG( "%s : %08X-%08X", pNode->GetName(), partA, partB);

			if( os.obj->SuperClassID() == SClass_ID( LIGHT_CLASS_ID))
			{
				// Light
				if( pExport->m_Option.m_bExportLight)
				{
					pExport->_OnExportLight( pNode, &os);
				}
			}
			else if( os.obj->SuperClassID() == SClass_ID( GEOMOBJECT_CLASS_ID))
			{
				// Geometry
				bool bExportGeom;

				bExportGeom =	(pNode->IsHidden() == FALSE) &&			// 숨겨진 Node가 아니고
								(pExport->m_Option.m_bExportMesh) &&	// Option에서 Mesh가 Export 가능 설정이어야 하며
								(bCollisionLayer == false) &&			// COL_... 이름의 Layer가 아니며
								(IsBone( os.obj) == false) &&		
								(( pExport->_IsPhysixNode( pNode) == false) || (pExport->m_Option.m_bExportPhysicsGeometry == true)) &&
								(pNode->Renderable());


				if( bExportGeom)
				{
					pExport->_OnExportGeometry( pNode, &os);
				}

				NODELOG( " ");

				switch( partA)
				{
					case EDITTRIOBJ_CLASS_ID :	NODELOG( "[EDITABLE-MESH]");	break;
					case BOXOBJ_CLASS_ID :		NODELOG( "[BOX]");	break;
					case SPHERE_CLASS_ID :		NODELOG( "[SPHERE]");	break;
					case CYLINDER_CLASS_ID :	NODELOG( "[CYLINDER]");	break;
					case CONE_CLASS_ID :		NODELOG( "[CONE]");	break;
					case TORUS_CLASS_ID :		NODELOG( "[TORUS]");	break;
					case TUBE_CLASS_ID :		NODELOG( "[TUBE]");	break;
					case HEDRA_CLASS_ID :		NODELOG( "[HEDRA]");	break;
					case BOOLOBJ_CLASS_ID :		NODELOG( "[BOOL]");	break;
					default :
						if( os.obj->ClassID() == CAPSULE_CLASS_ID)
							NODELOG( "[CAPSULE]");
						else if( os.obj->ClassID() == PX_HULL_CLASS_ID)
							NODELOG( "[PX-HULL]");
						else if( os.obj->ClassID() == PX_AGGREGATE_HULL_CLASS_ID)
							NODELOG( "[PX-AGGREGATE-HULL]");
						else if( os.obj->ClassID() == PX_D6_JOINT_CLASS_ID)
							NODELOG( "[PX-D6JOINT]");
						else if( os.obj->ClassID() == PLANE_CLASS_ID)
							NODELOG( "[PLANE]");
						else if( os.obj->ClassID() == PYRAMID_CLASS_ID)
							NODELOG( "[PYRAMID]");
						else if( os.obj->ClassID() == GSPHERE_CLASS_ID)
							NODELOG( "[GEO-SPHERE]");
						else
							NODELOG( "[Unknown]\n");	
						break;
				}
			}
			else if( os.obj->SuperClassID() == SClass_ID( CAMERA_CLASS_ID))
			{
				// Camera
				if( pExport->m_Option.m_bExportCamera)
				{
					pExport->_OnExportCamera( pNode, &os);
				}
			}
			else if( os.obj->SuperClassID() == SClass_ID( SHAPE_CLASS_ID))
			{
				// Shape
				pExport->_OnExportShape( pNode, &os);
			}
			else if( os.obj->SuperClassID() == SClass_ID( HELPER_CLASS_ID))
			{
				// Helper Object
				pExport->_OnExportHelper( pNode, &os);
			}
			else
			{
				pExport->_OnExportUnknownNode( pNode, &os);
			}
		}
		else
		{
			bDummyNode = true;
		}
	}

	if( bDummyNode)
	{
		// 아무런 처리를 하지 않은 경우..
		// Dummy i3Node를 추가해 둔다.
		i3Node * pTemp = i3Node::new_object_ref();

		pTemp->SetName( pNode->GetName());
		pExport->m_pCurProcessInfo->pushi3Node( pTemp);
	}

	NODELOG( "\n");

	return true;
}

DWORD WINAPI _nullProc( LPVOID)
{
	return 0;
}

i3tool::exporter::POSTPROCESS_INFO * i3Export::_RegisterPostProcess( i3tool::exporter::POSTPROCESS_TYPE type, INode * pINode, i3Node * pNode, MATRIX * pMtx, bool bToHead)
{
	i3tool::exporter::POSTPROCESS_INFO * pInfo;

	pInfo = new i3tool::exporter::POSTPROCESS_INFO;
	pInfo->m_bProcessed = false;
	pInfo->m_Type = type;
	pInfo->m_pINode = pINode;
	pInfo->m_pNode = pNode;
	pInfo->m_pShapeSet = NULL;
	pInfo->m_Index = -1;
	i3Matrix::Copy( &pInfo->m_LocalTransform, pMtx);

	if( bToHead)
	{
		m_PostProcessList.insert( m_PostProcessList.begin(), pInfo);
	}
	else
	{
		m_PostProcessList.push_back( pInfo);
	}

	return pInfo;
}

void i3Export::_FindPostProcessByProxy( const char * pszProxy, i3::vector<i3tool::exporter::POSTPROCESS_INFO*>& List)
{
	INT32 i;
	i3tool::exporter::POSTPROCESS_INFO * pInfo;
	TSTR temp;

	for( i = 0; i < _getPostProcessCount(); i++)
	{
		pInfo = _getPostProcess( i);

		if( pInfo->m_bProcessed == true)
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

void i3Export::_ResetPostProcess(void)
{
	INT32 i;
	i3tool::exporter::POSTPROCESS_INFO * pInfo;

	for( i = 0; i < _getPostProcessCount(); i++)
	{
		pInfo = _getPostProcess( i);

		delete pInfo;
	}

	m_PostProcessList.clear();
}

void i3Export::_DoPostProcess(void)
{
	INT32 i;
	i3tool::exporter::POSTPROCESS_INFO * pInfo;

	for( i = 0; i < _getPostProcessCount(); i++)
	{
		pInfo = _getPostProcess( i);

		if( pInfo->m_bProcessed == false)
		{
			_OnExportPhysics( pInfo);

			pInfo->m_bProcessed = true;
		}
	}

	for( i = 0; i < _getPostProcessCount(); i++)
	{
		pInfo = _getPostProcess( i);

		_OnExportJoint( pInfo);
	}


	_ResetPostProcess();
}

void i3Export::LinkNode(void)
{

	i3ExpProcessInfo * pInfo, * pParentInfo;
	INode * pINode;
	INode * pParentINode;

	for( size_t i = 0; i < m_ProcessInfoList.size(); i++)
	{
		pInfo = m_ProcessInfoList[i];

		pINode = pInfo->getINode();

		if( pInfo->isExcluded())
		{
			I3TRACE( "EXCLUDED : %s\n", pINode->GetName());
			continue;
		}

		if( pInfo->geti3Node() == NULL)
		{
			I3TRACE( "NULL NODE : %s\n", pINode->GetName());
			continue;
		}

		I3TRACE( "NODE : %s\n", pINode->GetName());

		if( pInfo->getAttachNode() == NULL)
		{
			// Attach Node에 대한 특별한 지정이 없다면, Parent Node를 기반으로 검색한다.
			if( pInfo->getParentINode() == NULL)
				pParentINode = pINode->GetParentNode();
			else
				pParentINode = pInfo->getParentINode();

			if( pParentINode == NULL)
			{
				I3_REF_CHANGE( m_pRoot, pInfo->geti3Node());
			}
			else
			{
				pParentInfo = findProcessInfo( pParentINode);

				if( pParentInfo != NULL)
				{
					i3Node * pTemp = pParentInfo->getTop();

					if( pTemp != NULL)
						pTemp->AddChild( pInfo->geti3Node());
					else
						Log( pINode, "이상하다....왜??");
				}
				else
				{
					Log( pParentINode, "해당 Node에 대한 처리가 올바르게 수행되지 않았습니다.");
				}
			}
		}
		else
		{
			// 특정 i3Node에 대한 Attach 설정이 있다면....볼 거 있나...
			i3Node * pTemp = pInfo->getAttachNode();

			pTemp->AddChild( pInfo->geti3Node());
		}
	}

	//I3_SG_DUMP( m_pRoot);
}

bool i3Export::DoExport( char * pszPath, char * pszName, i3Node** outExportRootNode, bool doClearLog)
{
	bool exportOk = true;
	Interface * ip = GetCOREInterface();
	INode * pRoot;
	INT32 i;
	char szPath[MAX_PATH], szExportName[MAX_PATH];

	if(doClearLog)
		ClearLog();

	pRoot = ip->GetRootNode();

	char* name = pRoot->GetName();
// 	INT32 nCnt = ip->GetSelNodeCount();
// 
// 	for (INT32 nSel = 0; nSel < nCnt; ++nSel)
// 	{
// 		m_ExcludeNodeList.insert(ip->GetSelNode(nSel));
// 	}

	m_TotalNodeCount = 0;
	m_CameraCount = 0;
	m_LightCount = 0;
	m_bExcludeBiped = true;
	I3_SAFE_RELEASE( m_pAnim);

	m_pSkinBoneRoot = NULL;

	{
		Interval intv;

		intv = ip->GetAnimRange();

		m_timeStart = intv.Start();
		m_timeEnd = intv.End();
	}

	Traverse( pRoot, _NodeCountProc, this);

	ip->SetCancel( FALSE);
	ip->ProgressStart( "i3Engine Exporter", TRUE, _nullProc, NULL);

	{
		SgBuilder builder;

		builder.Export( pRoot, this);

		I3_REF_CHANGE( m_pRoot, builder.getRoot());
	}

	// 실제적인 Exporting 처리
	m_ProcessedNodeCount = 0;
	//Traverse( pRoot, _NodeTraverseProc, this);

	//LinkNode();

	// Light가 발견되었다면 Lighting을 On 시킨다.
	if((m_pRoot != NULL) && ( m_LightCount > 0))
	{
		i3AttrSet * pAttrSet = i3AttrSet::new_object();

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		pAttrSet->SetName( m_pRoot->GetName());

		for( i = 0; i < m_pRoot->GetChildCount(); i++)
		{
			i3Node * pChild = m_pRoot->GetChild( i);

			pAttrSet->AddChild( pChild);
		}

		m_pRoot->Release();
		m_pRoot = pAttrSet;
	}

	// 만약 최상위가 쓸모없는 Node인지 확인...
	{
		if( i3::same_of<i3Node*>(m_pRoot)		//->IsExactTypeOf( i3Node::static_meta())) 
			&& (m_pRoot->GetChildCount() == 1))
		{
			i3Node * pChild = m_pRoot->GetChild( 0);

			pChild->AddRef();

			m_pRoot->RemoveChild( pChild);

			I3_REF_CHANGE( m_pRoot, pChild);

			pChild->Release();
		}
	}

	// Animation이 있다면 재생을 설정해준다.
	if( m_pAnim != NULL)
	{
		_PlayAnim();
	}

	// Post-Process
	_DoPostProcess();

	// 3DSMAX Axis -> i3Engine Axis
	if( m_Option.m_bRotateAxis == TRUE)
	{
		MATRIX * pMtx;
		i3Transform * pTrans;

		if( !i3::kind_of<i3Transform*>(m_pRoot)) //->IsTypeOf( i3Transform::static_meta()) == FALSE)
		{
			pTrans = i3Transform::new_object();

			pTrans->SetName( "AxisRotate");
			pTrans->AddFlag( I3_NODEFLAG_CONTROL);
			pTrans->AddChild( m_pRoot);

			I3_REF_CHANGE( m_pRoot, pTrans);

			pMtx = pTrans->GetMatrix();
		}
		else
		{
			pTrans = (i3Transform *) m_pRoot;

			pMtx = pTrans->GetMatrix();
		}

		
		pMtx = pTrans->GetMatrix();
		
		i3Matrix::SetRight(	pMtx,	-1.0f,	0.0f,	0.0f,	0.0f);
		i3Matrix::SetUp(	pMtx,	0.0f,	0.0f,	1.0f,	0.0f);
		i3Matrix::SetAt(	pMtx,	0.0f,	1.0f,	0.0f,	0.0f);
		i3Matrix::SetPos(	pMtx,	0.0f,	0.0f,	0.0f,	1.0f);
	}

	// Bounding Sphere 생성
	{
		i3OptBuildBoundSphere opt;

		opt.Trace( m_pRoot);
	}

	// Vertex Format에 대한 Optimize
	{
		i3OptVertexFormat opt;

		opt.Trace( m_pRoot);
	}

	{
// 		i3OptSkinGeometry opt;
// 
// 		opt.Trace( m_pRoot);
	}

	{
		i3OptGeo_NonIndexed_to_Indexed opt;

		opt.Trace( m_pRoot);
	}

	// Global-Environment-Map의 설정
	{
		if( ip->GetUseEnvironmentMap() && (ip->GetEnvironmentMap() != NULL))
		{
			Texmap * pTex = ip->GetEnvironmentMap();
			bool bError = true;

			if( pTex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
			{
				BitmapTex * pBmpTex = (BitmapTex *) pTex;

				// Texture 
				if( pBmpTex->GetBitmap( getStartTime()) != NULL)
				{
					i3Texture * pTex;

					pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap( getStartTime()));

					if( pTex != NULL)
					{
						i3ReflectMapBindAttr * pAttr = i3ReflectMapBindAttr::new_object_ref();

						pAttr->SetTexture( pTex);

						pTex->Release(); 

						// 
						i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();

						pAttrSet->AddAttr( pAttr);

						i3Scene::MoveChildren( pAttrSet, m_pRoot);

						m_pRoot->AddChild( pAttrSet);

						bError = false;
					}
				}
			}

			if( bError)
			{
				Log( NULL, "Invalid global environment map \n");
			}
		}
	}

	// 최종적으로 생성된 Scene Graph를 파일에 저장
	if( (ip->GetCancel() == FALSE) && (m_pRoot != NULL))
	{
		sprintf( szPath, "%s\\%s", pszPath, pszName);

		strcpy( szExportName, szPath);

		if( m_Option.m_bExportI3S)
		{
			i3SceneFile file;

			file.SetSceneGraph( m_pRoot);
			if( file.Save( szPath) == STREAM_ERR)
			{
				char conv[256];

				sprintf( conv, "%s 파일을 저장할 수 없습니다. 경로를 확인해 주세요.", szPath);
				Log( NULL, conv);

				exportOk = false;
			}

			i3ResourceFile::SetProtectionCode( szPath, "Zepetto Inc.", 12);
		}

		// Animation
		if((m_Option.m_bExportAnim) && (m_pAnim != NULL))
		{
			i3::remove_ext( szPath);
			i3::generic_string_cat( szPath, ".i3a");

			{
				i3ResourceFile file;

				file.setKeyObject( m_pAnim);

				if(file.Save(szPath) == STREAM_ERR)
				{
					char conv[256];

					sprintf(conv, "%s 파일을 저장할 수 없습니다. 경로를 확인해 주세요.", szPath);
					Log( NULL, conv);

					exportOk = false;
				}
			}
		}

		if( m_Option.m_bViewAfterExport)
		{
			ShellExecute( NULL, "open", szExportName, NULL, pszPath, SW_SHOW);
		}
	}

	ip->ProgressUpdate( 100);
	ip->ProgressEnd();

	if(exportOk && outExportRootNode)
	{
		*outExportRootNode = m_pRoot;
		(*outExportRootNode)->AddRef(); // _ResetExportContext에의해 삭제되기에
	}

	_ResetExportContext();

	return exportOk;
}

namespace
{
	struct APPLYANIMINFO
	{
		i3Animation * m_pAnim = nullptr;
		INT32		m_Index = 0;
	};
}

I3SG_TRAVERSAL_RESULT _ApplyAnim( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	APPLYANIMINFO * pInfo = (APPLYANIMINFO *) pUserData;
	i3TransformSequence * pSeq;

	pSeq = pInfo->m_pAnim->GetTrack( pInfo->m_Index);

	if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
	{
		if( i3::generic_is_iequal( pNode->GetName(), pSeq->GetName()) )
		{
			i3Transform * pTrans = (i3Transform *) pNode;
			
			pTrans->SetAnimation( pInfo->m_pAnim, pInfo->m_Index);

			return I3SG_TRAVERSAL_STOP;
		}
	}
	else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		
		pBody->PlayAnim( pInfo->m_pAnim);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Export::_PlayAnim(void)
{
	INT32 i;
	APPLYANIMINFO info;

	info.m_pAnim = m_pAnim;

	for( i = 0; i < m_pAnim->GetTrackCount(); i++)
	{
		info.m_Index = i;

		i3Scene::Traverse( m_pRoot, _ApplyAnim, &info, NULL, 0);
	}
}
