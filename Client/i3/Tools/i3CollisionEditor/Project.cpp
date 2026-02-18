#include "stdafx.h"
#include "Project.h"
#include "ColliderGroup.h"

I3_CLASS_INSTANCE( Project, i3ElementBase);

Project::Project(void)
{
	m_pRoot = i3Node::NewObject();

	{
		m_pWorldRoot = i3Node::NewObjectRef();
		m_pWorldRoot->AddFlag( I3_NODEFLAG_VOLATILE);

		m_pRoot->AddChild( m_pWorldRoot);
	}

	m_pCollRoot = i3AttrSet::NewObjectRef();
	m_pRoot->AddChild( m_pCollRoot);

	{
		i3ClearRenderTargetAttr * pAttr = i3ClearRenderTargetAttr::NewObjectRef();

		pAttr->SetRemoveClearMask( I3G_CLEAR_COLOR);
		m_pCollRoot->AddAttr( pAttr);
	}

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
		pAttr->Set( FALSE);

		m_pCollRoot->AddAttr( pAttr);
	}

	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();
		pAttr->Set( TRUE);

		m_pCollRoot->AddAttr( pAttr);
	}

	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();
		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_pCollRoot->AddAttr( pAttr);
	}

	m_pSgInfo = NULL;

	m_szFileName[0] = 0;
}

Project::~Project(void)
{
	Reset();

	I3_SAFE_RELEASE( m_pRoot);
}

void Project::Reset(void)
{
	RemoveAllGroup();
	RemoveAllCollidee();

	if( m_pSgInfo != NULL)
	{
		if( m_pSgInfo->getInstanceSg() != NULL)
		{
			m_pCollRoot->RemoveChild( m_pSgInfo->getInstanceSg());
		}

		I3_SAFE_RELEASE( m_pSgInfo);
	}

	m_pWorldRoot->RemoveAllChild();

	m_szFileName[0] = 0;
}

void Project::SetSceneGraphInfo( i3SceneGraphInfo * pInfo)
{
	Reset();

	I3_REF_CHANGE( m_pSgInfo, pInfo);

	if( m_pSgInfo != NULL)
	{
		if( m_pSgInfo->getInstanceSg() != NULL)
		{
			m_pCollRoot->AddChild( m_pSgInfo->getInstanceSg());
		}
	}

	UpdateAll();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SCENE, m_pSgInfo);
}

void Project::SetWorld( i3Node * pWorld)
{
	m_pWorldRoot->AddChild( pWorld);
}

void Project::SaveI3S(void)
{
	if( m_szFileName[0] == 0)
	{
		AfxMessageBox( "파일명이 지정되지 않았습니다.");
		return;
	}

	i3SceneFile file;

	file.setSceneGraphInfo( m_pSgInfo);

	file.Save( m_szFileName);
}

void Project::LoadI3S( const char * pszPath)
{
	i3SceneFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		I3WARN( "Could not load %s file", pszPath);
		return;
	}

	{
		i3List GeoList, CollList;

		i3Scene::FindNodeByType( file.GetSceneGraph(), i3Geometry::GetClassMeta(), &GeoList);
		i3Scene::FindNodeByType( file.GetSceneGraph(), i3ColliderSet::GetClassMeta(), &CollList);

		if( GeoList.GetCount() <= 0)
		{
			SetSceneGraphInfo( file.getSceneGraphInfo());

			i3String::Copy( m_szFileName, pszPath);
		}
		else if( CollList.GetCount() <= 0)
		{
			SetWorld( file.GetSceneGraph());
		}
		else
		{
			int id = AfxMessageBox( "이 파일을 Collision용 Data로 Load할래?", MB_YESNO);

			if( id == IDNO)
			{
				SetWorld( file.GetSceneGraph());
			}
			else
			{
				SetSceneGraphInfo( file.getSceneGraphInfo());

				i3String::Copy( m_szFileName, pszPath);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
void	Project::AddCollidee( Collidee * pColl)
{
	I3ASSERT( pColl != NULL);

	pColl->AddRef();
	m_CollList.Add( pColl);

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pColl);
}

INT32	Project::FindCollidee( Collidee * pColl)
{
	INT32 i;
	
	for( i = 0; i < getCollideeCount(); i++)
	{
		if( getCollidee( i) == pColl)
		{
			return i;
		}
	}

	return -1;
}

void	Project::RemoveCollidee( INT32 idx)
{
	Collidee * pColl;

	I3ASSERT( idx != -1);

	pColl = getCollidee( idx);
	I3ASSERT( pColl != NULL);

	pColl->Release();

	m_CollList.Delete( idx);
}

void	Project::RemoveAllCollidee(void)
{
	INT32 i;
	Collidee * pColl;

	for( i = 0; i < getCollideeCount(); i++)
	{
		pColl = getCollidee( i);

		pColl->Release();
	}

	m_CollList.Clear();
}

void Project::SelectTri( I3RECT * pRect, MATRIX * pViewProj, BOOL bSelect, BOOL bBothSide)
{
	INT32 i, selIdx = 0;
	Collidee * pColl;
	i3List SelList;
	bool bNearest = false;

	if( (i3Math::abs( pRect->right - pRect->left) <= 0.02f) &&
		(i3Math::abs( pRect->bottom - pRect->top) <= 0.02f))
		bNearest = true;


	for( i = 0; i < getCollideeCount(); i++)
	{
		pColl = getCollidee( i);

		pColl->GetTouchedTri( pRect, pViewProj, bBothSide, &SelList);
	}

	if( bNearest)
	{
		REAL32 fDist = 1000000.0f;
		
		selIdx = -1;

		for( i = 0; i < SelList.GetCount(); i++)
		{
			SELECT_TRI_INFO * pInfo = (SELECT_TRI_INFO *) SelList.Items[i];

			if( i == 0)
			{
				selIdx = i;
				fDist = pInfo->m_Distance;
			}
			else
			{
				if( pInfo->m_Distance < fDist)
				{
					selIdx = i;
					fDist = pInfo->m_Distance;
				}
			}
		}

		if( selIdx != -1)
		{
			SELECT_TRI_INFO * pSelInfo = (SELECT_TRI_INFO *) SelList.Items[ selIdx];
			pSelInfo->m_pCollidee->SelectTri( pSelInfo->m_TriIndex, bSelect);

			i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, pSelInfo->m_pCollidee);
		}
	}
	else
	{
		for( i = 0; i < SelList.GetCount(); i++)
		{
			SELECT_TRI_INFO * pInfo = (SELECT_TRI_INFO *) SelList.Items[i];

			pInfo->m_pCollidee->SelectTri( pInfo->m_TriIndex, bSelect);

			if( i == SelList.GetCount() - 1)
			{
				i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, pInfo->m_pCollidee);
			}
		}
	}

	for( i = 0; i < SelList.GetCount(); i++)
	{
		SELECT_TRI_INFO * pInfo = (SELECT_TRI_INFO *) SelList.Items[i];

		delete pInfo;
	}
}

void Project::SelectAllTri(void)
{
	INT32 i;
	Collidee * pColl;

	for( i = 0; i < getCollideeCount(); i++)
	{
		pColl = getCollidee( i);

		pColl->SelectAllTri();
	}
}

void Project::UnselectAllTri(void)
{
	INT32 i;
	Collidee * pColl;

	for( i = 0; i < getCollideeCount(); i++)
	{
		pColl = getCollidee( i);

		pColl->UnselectAllTri();
	}
}

///////////////////////////////////////////////////////////////////////////
//

INT32	Project::AddGroup( INT32 id)
{
	ColliderGroup * pGrp;

	pGrp = ColliderGroup::NewObject();
	pGrp->setGroup( id);

	return m_GroupList.Add( pGrp);
}

INT32	Project::FindGroupByID( INT32 id)
{
	INT32 i;
	ColliderGroup * pGrp;

	for( i = 0; i < getGroupCount(); i++)
	{
		pGrp = getGroup( i);

		if( pGrp->getGroup() == id)
			return i;
	}

	return -1;
}

void	Project::RemoveAllGroup(void)
{
	INT32 i;
	ColliderGroup * pGrp;

	for( i = 0; i < getGroupCount(); i++)
	{
		pGrp = getGroup( i);

		pGrp->Release();
	}

	m_GroupList.Clear();
}

inline void _DoOpt( i3ClassMeta * pMeta, i3SceneGraphInfo * pSgInfo)
{
	i3SceneOptimizer * pOpt = (i3SceneOptimizer *) CREATEINSTANCE( pMeta);

	pOpt->AddRef();

	pOpt->Trace( pSgInfo->getInstanceSg());

	if( pOpt->GetSceneRoot() != pSgInfo->getInstanceSg())
	{
		pSgInfo->setInstanceSg( pOpt->GetSceneRoot());
	}

	pOpt->Release();
}

///////////////////////////////////////////////////////////////////////////
//
BOOL Project::_UpdateAllProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 j;
	Project * pProject = (Project *) pUserData;

	if( pNode->IsTypeOf( i3ColliderSet::GetClassMeta()))
	{
		i3ColliderSet * pCollSet = (i3ColliderSet *) pNode;

		for( j = 0; j < pCollSet->GetColliderCount(); j++)
		{
			i3Collider * pColl = pCollSet->GetCollider( j);

			Collidee * pNewCollidee = Collidee::NewObjectRef();

			pNewCollidee->SetCollidee( pCollSet, pColl->GetCollidee(), pStack->GetTop());

			pProject->AddCollidee( pNewCollidee);
		}
	}

	return TRUE;
}

void Project::UpdateAll(void)
{

	if( m_pSgInfo == NULL)
		return;

	if( m_pSgInfo->getInstanceSg() == NULL)
		return;

	// 읽어들인 Sg는 Collision용이 아닐 수도 있기 때문에
	// 미리 Optimize를 한다.

	// i3Transform Node의 제거
	//_DoOpt( i3OptCollapseTransform::GetClassMeta(), m_pSgInfo);

	// Collision Node로 변환
	//_DoOpt( i3OptBuildCollisionMesh::GetClassMeta(), m_pSgInfo);

	// 불필요한 Node의 제거
	//_DoOpt( i3OptCollapseNode::GetClassMeta(), m_pSgInfo);

	{
		i3MatrixStack Stack;

		Stack.Reset();

		i3Scene::Traverse( m_pSgInfo->getInstanceSg(), _UpdateAllProc, this, &Stack);
	}
}

typedef struct _tagNodeNameInfo
{
	char				m_szName[32];
	I3_TERRAIN_TYPE		m_Type;
} NODE_NAME_INFO;

static NODE_NAME_INFO s_NodeNameTable[] =
{
	{	"CCR_",		I3_TERRAIN_CONCRETE	},
	{	"MET_",		I3_TERRAIN_STEEL },
	{	"GND_",		I3_TERRAIN_GROUND },
	{	"WOOD_",	I3_TERRAIN_WOOD },
	{	"SNOW_",	I3_TERRAIN_SNOW },
	{	"WTD_",		I3_TERRAIN_WATER_DEEP },
	{	"WTS_",		I3_TERRAIN_WATER_SHALLOW },
	{	"CLAY_",	I3_TERRAIN_WET_CLAY },
	{	"GRASS_",	I3_TERRAIN_GRASS },
	{	"MARB_",	I3_TERRAIN_MARBLE },
	{	"",			I3_TERRAIN_MARBLE },
};

static bool	_FindTerrainTypeByNodeName( char * pszName, I3_TERRAIN_TYPE * pType)
{
	INT32 i;
	char * pszNode;

	for( i = 0; s_NodeNameTable[i].m_szName[0] != 0; i++)
	{
		pszNode = s_NodeNameTable[i].m_szName;

		if( i3String::NCompare( pszNode, pszName, strlen( pszNode)) == 0)
		{
			*pType = s_NodeNameTable[i].m_Type;

			return true;
		}
	}

	return false;
}

void Project::AutoTerrainAssign(void)
{
	INT32 i, j;
	Collidee * pCollidee;
	i3ColliderSet * pCollSet;
	I3_TERRAIN_TYPE type;

	for( i = 0; i < m_CollList.GetCount(); i++)
	{
		pCollidee = (Collidee *) m_CollList.Items[i];
		pCollSet = (i3ColliderSet *) pCollidee->getColliderSet();

		if( pCollSet->GetName() == NULL)
			continue;

		if( _FindTerrainTypeByNodeName( (char *) pCollSet->GetName(), &type) == false)
			continue;
		
		for( j = 0; j < pCollidee->getTriangleCount(); j++)
		{
			I3_COLLIDEE_TRI32 * pTri = pCollidee->getTriangle( j);

			pTri->m_Terrain = (UINT8) type;
		}

		pCollidee->Update();
	}
}

void Project::VolumeLightAssign(void)
{
	INT32 i, cnt = 0, j;
	Collidee * pColl;
	i3ColliderSet * pCollSet;
	UINT8 mtl;

	for( i = 0; i < m_CollList.GetCount(); i++)
	{
		pColl = (Collidee *) m_CollList.Items[i];
		pCollSet = (i3ColliderSet *) pColl->getColliderSet();

		if( pCollSet->GetName() == NULL)
			continue;

		mtl = (UINT8) atoi( pCollSet->GetName());

		for( j = 0; j < pColl->getTriangleCount(); j++)
		{
			I3_COLLIDEE_TRI32 * pTri = pColl->getTriangle( j);

			pTri->m_Reverb = (UINT8) mtl;
			cnt++;
		}
	}

	NotifyBox( NULL, "Total %d triangles are setted.", cnt);
}
