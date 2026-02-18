#include "stdafx.h"
#include "i3LevelResSceneGraph.h"
#include "i3LevelStaticObject.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

#include "i3Base/string/ext/extract_directoryname.h"

I3_CLASS_INSTANCE( i3LevelResSceneGraph);

i3LevelResSceneGraph::i3LevelResSceneGraph(void)
{
	m_ResType		= I3LV_TYPE_SCENEGRAPH;
}

i3LevelResSceneGraph::~i3LevelResSceneGraph(void)
{
	I3_SAFE_RELEASE( m_pInfo);
}

void i3LevelResSceneGraph::SetSceneRoot( i3Node * pRoot)
{
	if( m_pInfo == nullptr)
	{
		m_pInfo = i3SceneGraphInfo::new_object();

		m_pInfo->SetName( getPath());
	}

	m_pInfo->setInstanceSg( pRoot);

}

i3LevelElement3D *	i3LevelResSceneGraph::CreateInstance( i3LevelScene * pScene)
{
	i3LevelStaticObject * pObj;

	// 이 경우, 필요하다면 Loading을 하고
	// Scene의 각 Node에 Res를 연결한다.
	if( OnValidate( pScene, false) == false)
		return nullptr;

	pObj = i3LevelStaticObject::new_object_ref();

	pObj->BindRes( this);

	pScene->AddElement( pObj, GetName());

	return pObj;
}

i3Node *	i3LevelResSceneGraph::GetElementAttachNode( i3LevelScene * pScene)
{
	if( m_pInfo == nullptr)
		return nullptr;

	if( m_pInfo->getCommonSg() != nullptr)
	{
		i3Node * pNode = m_pInfo->getCommonSg();

		return pNode;
	}
	
	return nullptr;
}

bool i3LevelResSceneGraph::OnValidateBrief( bool bForce)
{
	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;
	}

	i3SceneFile file;
	char szFull[ MAX_PATH];

	getFullPath( szFull);

	if( file.Load( szFull) == STREAM_ERR)
	{
		return false;
	}

	i3Node * pRoot;

	pRoot = file.GetSceneGraph();

	if( i3Scene::FindNodeByType( pRoot, i3TimeSequence::static_meta()) != nullptr)
	{
		m_SgType = I3LV_SG_TYPE_EFFECT;
	}
	else if( i3Scene::FindNodeByType( pRoot, i3Body::static_meta()) != nullptr)
	{
		m_SgType = I3LV_SG_TYPE_BODY;
	}
	else 
	{
		m_SgType = I3LV_SG_TYPE_NORMAL;
	}

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResSceneGraph::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	BeginValidate();
	{
		i3SceneFile file;
		char szFull[ MAX_PATH];

		getFullPath( szFull);

		if( file.Load( szFull) == STREAM_ERR)
		{
			return false;
		}

		I3_REF_CHANGE( m_pInfo, file.getSceneGraphInfo());
	}
	EndValidate();

	m_pInfo->SetResourcePath( getPath());
	m_pInfo->SetExternRes( TRUE);

	if( m_pInfo->getCommonSg() != nullptr)
	{
		// Common Node가 있을 때에는 Common을 직접 Scene::UnitRoot에 연결하고
		// Instance를 Common Node의 아래에 붙인다.
		if( pScene != nullptr)
			pScene->GetUnitRoot()->AddChild( m_pInfo->getCommonSg());
	}
	else
	{
		// Common Node가 없다면 Instance Sg.를 직접 Scene::UnitRoot에 연결한다.
		// 이 경우, 아무런 처리가 필요없다.
	}

	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

bool i3LevelResSceneGraph::OnSerialize(void)
{
	i3SceneFile file;

	if( m_pInfo == nullptr)
		return true;

	file.setSceneGraphInfo( m_pInfo);

	char szFull[MAX_PATH];

	sprintf( szFull, "%s/%s", g_szResPath, getPath());

	{
		char szPath[MAX_PATH];

	//	i3String::SplitPath( szFull, szPath);
		i3::extract_directoryname(szFull, szPath);

		i3System::PrepareDirectory( szPath);
	}

	if( file.Save( szFull) == STREAM_ERR)
		return false;

	return true;
}

i3GameRes *	i3LevelResSceneGraph::CreateGameRes(void)
{
	i3GameResSceneGraph * pRes;

	pRes = i3GameResSceneGraph::new_object();
	pRes->setSceneGraphInfo( m_pInfo);

	return pRes;
}

void i3LevelResSceneGraph::OnChangeScene( i3LevelScene * pScene)
{
	if( isLoaded() == false)
		return;

	if( m_pInfo != nullptr)
	{
		if( m_pInfo->getCommonSg() != nullptr)
		{
			i3Node * pNode = m_pInfo->getCommonSg();

			if( pNode->GetParent() != nullptr)
			{
				pNode->RemoveFromParent();
			}

			if( pScene != nullptr)
			{
				pScene->GetUnitRoot()->AddChild( pNode);
			}
		}
	}
}

#if defined(I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct RES_SCENEGRAPH
	{
		UINT8		m_ID[4] = { 'L', 'S', 'G', '1' };
		UINT32		m_SgType = 0;
		UINT32		pad[32] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelResSceneGraph::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result;
	pack::RES_SCENEGRAPH data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_SgType = m_SgType;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3LevelResSceneGraph::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_SCENEGRAPH data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_SgType = data.m_SgType;

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return Result;
}
