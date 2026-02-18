#include "stdafx.h"
#include "i3Scene.h"
#include "i3SceneUtil.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelGlobalRes.h"

//////////////////////////////////////////////////////////
// Common Resource

void i3LevelScene::ReleaseCommonResource(void)
{
	I3_SAFE_RELEASE( m_pAABoxRes);
	I3_SAFE_RELEASE( m_pSphereRes);
	I3_SAFE_RELEASE( m_pCameraRes);
	I3_SAFE_RELEASE( m_pSpawnRes);
	I3_SAFE_RELEASE( m_pRallyRes);
	I3_SAFE_RELEASE( m_pHeroRes);
	I3_SAFE_RELEASE( m_pStartPointRes);
	I3_SAFE_RELEASE( m_pCharaRes);
	I3_SAFE_RELEASE( m_pPathStart);
	I3_SAFE_RELEASE( m_pPathMiddle);
	I3_SAFE_RELEASE( m_pPathEnd);
	I3_SAFE_RELEASE( m_pRespawnRes);
	I3_SAFE_RELEASE( m_pPathRes);
	I3_SAFE_RELEASE (m_pHitMapRes);
	I3_SAFE_RELEASE( m_pArrowTexRes);
}

void i3LevelScene::CreateCommonResource(void)
{
	i3AttrSet * pAttrSet;

	// AABox Resource
	if( m_pAABoxRes == nullptr)
	{
		i3Node * pNode;
		COLORREAL col;

		i3Color::Set( &col, 0.0f, 0.0f, 1.0f, 0.5f);

		pNode = i3SceneUtil::BuildBoxMesh( 1.0f, TRUE, TRUE, &col, &col);

		pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->AddChild( pNode);

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_ONE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		m_pAABoxRes = i3LevelResSceneGraph::new_object();
		m_pAABoxRes->SetSceneRoot( pAttrSet);

		I3_MUST_RELEASE(pNode);
	}

	// Sphere Resource
	if( m_pSphereRes == nullptr)
	{
		i3Node * pNode;
		COLORREAL col;

		i3Color::Set( &col, 0.0f, 0.0f, 1.0f, 0.5f);

		pNode = i3SceneUtil::BuildSphereMesh( 1.0f, TRUE, TRUE, 10, 10, &col, &col, &col);

		pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->AddChild( pNode);

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_ONE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		m_pSphereRes = i3LevelResSceneGraph::new_object();
		m_pSphereRes->SetSceneRoot( pAttrSet);

		I3_MUST_RELEASE(pNode);
	}

	// Spawn Resource
	if( m_pSpawnRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/spawn.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pSpawnRes = i3LevelResSceneGraph::new_object();
			m_pSpawnRes->SetSceneRoot( pNode);
		}
		else
		{
			DebugBox( nullptr, "Spawn Icon Resource를 생성할 수 없습니다.");
		}
	}

	// Rally Resource
	if( m_pRallyRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/rally.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pRallyRes = i3LevelResSceneGraph::new_object();
			m_pRallyRes->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("Rally Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Camera Resource
	if( m_pCameraRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/camera.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pCameraRes = i3LevelResSceneGraph::new_object();
			m_pCameraRes->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("Camera Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// StartPoint Resource
	if( m_pStartPointRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/start_point.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pStartPointRes = i3LevelResSceneGraph::new_object();
			m_pStartPointRes->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("StartPoint Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Character Resource
	if( m_pCharaRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/character.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pCharaRes = i3LevelResSceneGraph::new_object();
			m_pCharaRes->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("Character Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Hero Resource
	if( m_pHeroRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/hero.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pHeroRes = i3LevelResSceneGraph::new_object();
			m_pHeroRes->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("Hero Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Path Start Resource
	if( m_pPathStart == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/start_point.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pPathStart = i3LevelResSceneGraph::new_object();
			m_pPathStart->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("Start Point Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Path Middle Resource
	if( m_pPathMiddle == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/key_point.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pPathMiddle = i3LevelResSceneGraph::new_object();
			m_pPathMiddle->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("Middle Point Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Path End Resource
	if( m_pPathEnd == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/end_point.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pPathEnd = i3LevelResSceneGraph::new_object();
			m_pPathEnd->SetSceneRoot( pNode);
		}
		else
		{
			AfxMessageBox( _T("End Point Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Respawn Resource
	if( m_pRespawnRes == nullptr )
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/respawn.i3s") != STREAM_ERR )
		{
			pNode = file.GetSceneGraph();

			m_pRespawnRes = i3LevelResSceneGraph::new_object();
			m_pRespawnRes->SetSceneRoot( pNode );
		}
		else
		{
			AfxMessageBox( _T("Respawn Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// Path Resource
	if( m_pPathRes == nullptr )
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( "res/Path.i3s") != STREAM_ERR )
		{
			pNode = file.GetSceneGraph();

			m_pPathRes = i3LevelResSceneGraph::new_object();
			m_pPathRes->SetSceneRoot( pNode );
		}
		else
		{
			AfxMessageBox( _T("Path Icon Resource를 생성할 수 없습니다.") );
		}
	}

	// HitMap Resource
	if (m_pHitMapRes == nullptr)
	{
		i3SceneFile file;
		i3Node * pNode;

		if (file.Load("res/character.i3s") != STREAM_ERR)
		{
			pNode = file.GetSceneGraph();

			m_pHitMapRes = i3LevelResSceneGraph::new_object();
			m_pHitMapRes->SetSceneRoot(pNode);
		}
		else
		{
			AfxMessageBox(_T("HitMap Icon Resource를 생성할 수 없습니다."));
		}
	}

	if( m_pArrowTexRes == nullptr)
	{
		i3ImageFile file;
		i3Texture * pTex;

		pTex = file.Load( "res/Arrow.i3i");

		if( pTex == nullptr)
		{
			AfxMessageBox( _T("Arrow Texture Resource를 읽어들일 수 없습니다.") );
		}
		else
		{
			m_pArrowTexRes = i3LevelResTexture::new_object();
			m_pArrowTexRes->setTexture( pTex);
			I3_MUST_RELEASE(pTex);		// 이것이 없어서 메모리 누수있었음(2012.09.11.수빈)
		}
	}
}

void i3LevelScene::CleanUpUnusedResource(void)
{
	INT32 i;
	i3::vector< i3LevelRes * > resList;
	i3::vector< i3LevelElement3D * > elmList;

	for( i = 0; i < g_pResDB->getResCount(); i++)
	{
		resList.push_back( g_pResDB->getRes( i));
	}

	for( i = 0; i < (INT32)resList.size(); i++)
	{
		i3LevelRes * pRes = resList[i];

		FindElementByResource( elmList, pRes);

		if( elmList.size() == 0)
		{
			I3TRACE( "REMOVE Res : %s\n", pRes->getPath());
			g_pResDB->RemoveRes( pRes);
		}

		elmList.clear();
	}
}
