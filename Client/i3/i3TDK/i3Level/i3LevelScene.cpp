#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelElement3D.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelResSceneGraph.h"
#include "i3LevelLight.h"


#include "i3Base/smart_ptr/make_shared.h"
I3_CLASS_INSTANCE( i3LevelScene);

i3LevelResSceneGraph * i3LevelScene::m_pAABoxRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pSphereRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pCameraRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pSpawnRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pRallyRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pHeroRes	= nullptr; 
i3LevelResSceneGraph * i3LevelScene::m_pStartPointRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pCharaRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pPathStart = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pPathMiddle = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pPathEnd = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pRespawnRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pPathRes = nullptr;
i3LevelResSceneGraph * i3LevelScene::m_pHitMapRes = nullptr;
i3LevelResTexture *		i3LevelScene::m_pArrowTexRes = nullptr;


i3LevelScene::i3LevelScene(void)
{
	CreateCommonResource();
}

i3LevelScene::~i3LevelScene(void)
{
	removeAllLayer();
	//RemoveAllElement();

	{
		//	for each( i3LevelElement3D* pElm in m_ElementList )
		//  주의 ) i3::vector는 MS키워드 for each는 제대로 동작 안하는것 같다.
		for (size_t i = 0 ; i < m_ElementList.size() ; ++i)
		{
			i3LevelElement3D* pElm = m_ElementList[i];

			if( pElm->IsState( I3_LEVEL_STATE_LOCKED) == false)
			{
				pElm->AddState( I3_LEVEL_STATE_LOCKED );
				pElm->GetRoot()->RemoveFromParent();
				pElm->RemoveState( I3_LEVEL_STATE_ATTACHED);
				pElm->RemoveState(I3_LEVEL_STATE_LOCKED);
				I3_MUST_RELEASE(pElm);
			}
		}

		m_ElementList.clear();
	}

	RemoveAllTrigger();
	removeAllGroup();

	I3_SAFE_RELEASE( m_pNavMeshRoot );

	I3_SAFE_RELEASE( m_pGrid);

	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_pSgContext);
}

bool i3LevelScene::Create(void)
{
	m_pSgContext = i3SceneGraphContext::new_object();
	m_pSgContext->Create();

	m_pRoot = i3AttrSet::new_object();
	m_pRoot->SetName( "Root");

	m_pSgContext->get3DRoot()->AddChild( m_pRoot);

	// Camera Root
	{
		m_pCameraRoot = i3Node::new_object_ref();
		m_pCameraRoot->SetName( "Camera Root");
		m_pRoot->AddChild( m_pCameraRoot);	
	}

	// Light Root
	{
		m_pLightRoot = i3LightSet::new_object_ref();
		m_pLightRoot->SetName( "Light Root");
		m_pRoot->AddChild( m_pLightRoot);

		m_pLightAttr = i3LightAttr::new_object_ref();
		
		m_pLightRoot->AddLightAttr( m_pLightAttr);
	}

	m_pUnitRoot = i3AttrSet::new_object_ref();
	m_pUnitRoot->SetName( "Unit Root");
	m_pRoot->AddChild( m_pUnitRoot);

	{
		m_pRenderModeAttr = i3RenderModeAttr::new_object_ref();
		m_pUnitRoot->AddAttr( m_pRenderModeAttr);
	}

	{
		m_pFogEnableAttr = i3FogEnableAttr::new_object_ref();
		m_pUnitRoot->AddAttr( m_pFogEnableAttr);
	}

	{
		m_pFogColorAttr = i3FogColorAttr::new_object_ref();
		m_pUnitRoot->AddAttr( m_pFogColorAttr);
	}

	{
		m_pFogModeAttr = i3FogModeAttr::new_object_ref();
		m_pUnitRoot->AddAttr( m_pFogModeAttr);
	}

	{
		m_pFogDensityAttr = i3FogDensityAttr::new_object_ref();
		m_pUnitRoot->AddAttr( m_pFogDensityAttr);
	}
	
	{
		m_pFogNearFarAttr = i3FogNearFarAttr::new_object_ref();
		m_pUnitRoot->AddAttr( m_pFogNearFarAttr);
	}

	{
		m_pLightingEnableAttr = i3LightingEnableAttr::new_object_ref();

		m_pLightingEnableAttr->Set( TRUE);
		m_pUnitRoot->AddAttr( m_pLightingEnableAttr);
	}

	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);
		m_pUnitRoot->AddAttr( pAttr);
	}

	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource(I3G_BLEND_SRCALPHA);
		pAttr->SetDestination(I3G_BLEND_INVSRCALPHA);
		m_pUnitRoot->AddAttr(pAttr);
	}

	{
		m_pPrimitiveRoot = i3AttrSet::new_object_ref();
		m_pPrimitiveRoot->SetName("Primitive Root");
		m_pRoot->AddChild(m_pPrimitiveRoot);

		{
			i3RenderModeAttr* pRenderModeAttr = i3RenderModeAttr::new_object_ref();
			m_pPrimitiveRoot->AddAttr(pRenderModeAttr);
		}

		{
			i3FogEnableAttr* pFogEnableAttr = i3FogEnableAttr::new_object_ref();
			m_pPrimitiveRoot->AddAttr(pFogEnableAttr);
		}

		{
			i3FogColorAttr* pFogColorAttr = i3FogColorAttr::new_object_ref();
			m_pPrimitiveRoot->AddAttr(pFogColorAttr);
		}

		{
			i3FogModeAttr* pFogModeAttr = i3FogModeAttr::new_object_ref();
			m_pPrimitiveRoot->AddAttr(pFogModeAttr);
		}

		{
			i3FogDensityAttr* pFogDensityAttr = i3FogDensityAttr::new_object_ref();
			m_pPrimitiveRoot->AddAttr(pFogDensityAttr);
		}

		{
			i3FogNearFarAttr* pFogNearFarAttr = i3FogNearFarAttr::new_object_ref();
			m_pPrimitiveRoot->AddAttr(pFogNearFarAttr);
		}

		{
			i3LightingEnableAttr* pLightingEnableAttr = i3LightingEnableAttr::new_object_ref();

			pLightingEnableAttr->Set(TRUE);
			m_pPrimitiveRoot->AddAttr(pLightingEnableAttr);
		}

		{
			i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

			pAttr->SetFunction(I3G_COMP_GREATER);
			pAttr->SetRefValue(0);
			m_pPrimitiveRoot->AddAttr(pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource(I3G_BLEND_SRCALPHA);
			pAttr->SetDestination(I3G_BLEND_INVSRCALPHA);
			m_pPrimitiveRoot->AddAttr(pAttr);
		}
	}

	


	m_pControlRoot = i3AttrSet::new_object_ref();
	m_pControlRoot->SetName( "Control Root");
	m_pRoot->AddChild( m_pControlRoot);

	// Grid Lines
	{
		i3VertexArray * pVA;
		i3GeometryAttr * pGeoAttr;
		i3Geometry * pGeo;
		i3AttrSet * pAttrSet;
		VEC3D origin;

		i3Vector::Set( &origin, 0.0f, 0.0f, 0.0f);

		pVA = i3VertexArray::CreateGrid( &origin, 1.0f, 1.0f, 0.08f, 0.08f);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, pVA->GetCount() / 2);

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->AddChild( pGeo);
		pAttrSet->SetName( "GridLine");

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}


		m_pGrid = i3Transform::new_object();
		m_pGrid->AddChild( pAttrSet);
	}

	SetGridShow( TRUE);

	{
		VEC3D vmin, vmax;

		i3Vector::Set( &vmin, -5.0f, -5.0f, -5.0f);
		i3Vector::Set( &vmax, 5.0f, 5.0f, 5.0f);

		SetBound( &vmin, &vmax);
	}

	{
		// Default Layer
		i3LevelLayer * pLayer = i3LevelLayer::new_object_ref();

		pLayer->SetName( "Default");
	
		addLayer( pLayer);
	}

	m_pNavMeshRoot = i3NavMesh::new_object();
	m_pNavMeshRoot->Create();
	m_pRoot->AddChild( m_pNavMeshRoot );


	{
		// Default Layer
		i3LevelLayer * pLayer = i3LevelLayer::new_object_ref();

		pLayer->SetName("HitMap");

		addLayer(pLayer);
	}
	return TRUE;
}

void i3LevelScene::SetGridShow( bool bShow)
{
	// Unlink from scene graph
	i3Node * pParent = m_pGrid->GetParent();

	if( pParent != nullptr)
	{
		pParent->RemoveChild( m_pGrid);
		m_pGrid->SetParent( nullptr);
	}

	m_bShowGrid = bShow;

	if( m_bShowGrid)
	{
		m_pRoot->AddChild( m_pGrid);
	}
}

void i3LevelScene::SetBound( VEC3D * pMin, VEC3D * pMax)
{
	VEC3D vec;

	// Grid Size
	{
		REAL32 GridSz;

		i3Vector::Sub( &vec, pMax, pMin);
		GridSz = MAX( getX( &vec), getZ( &vec)) * 2.5f;

		if( GridSz > 0.0f)
		{
			i3Matrix::SetScale( m_pGrid->GetMatrix(), GridSz, 1.0f, GridSz);
		}
		else
		{
			i3Matrix::Identity( m_pGrid->GetMatrix());
		}
	}
}

void i3LevelScene::ToggleRenderMode(void)
{
	if( m_pRenderModeAttr->Get() == I3G_RENDER_LINE)
	{
		m_pRenderModeAttr->Set( I3G_RENDER_POLYGON);
	}
	else
	{
		m_pRenderModeAttr->Set( I3G_RENDER_LINE);
	}
}

void i3LevelScene::Update(void)
{

}

void i3LevelScene::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	//i3LevelRes * pRes;
	i3LevelElement3D * pElm;
	i3::vector<i3LevelElement3D*> ElmList;

	GetPersistElement( ElmList);

	for(size_t i = 0; i < ElmList.size(); i++)
	{
		pElm = ElmList[i];

		pElm->OnBuildObjectList( List);
	}
/*
	for( i = 0; i < GetActionCount(); i++)
	{
		i3LevelAction * pAction = GetAction( i);

		pAction->OnBuildObjectList( pList);
	}
*/
	for(INT32 i = 0; i < GetTriggerCount(); i++)
	{
		i3LevelTrigger * pTrig = GetTrigger( i);

		pTrig->OnBuildObjectList( List);
	}

	for(INT32 i = 0; i < getLayerCount(); i++)
	{
		i3LevelLayer * pLayer = getLayer( i);

		pLayer->OnBuildObjectList( List);
	}

	for(INT32 i = 0; i < getGroupCount(); i++)
	{
		i3LevelGroup * pGroup = getGroup( i);

		pGroup->OnBuildObjectList( List);
	}

	i3LevelElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 LEVEL_SCENE_1
	{
		UINT8				m_ID[4] = { 'T', 'S', 'C', '1' };
		i3::pack::VEC3D		m_StartPos;
		REAL32				m_StartRot = 0.0f;
		REAL32				m_CameraNear = 0.0f;
		REAL32				m_CameraFar = 0.0f;
		REAL32				m_CameraFOV = 0.0f;

		BOOL				m_bFogEnable = FALSE;
		UINT32				m_FogColor = 0;
		INT8				m_FogMode = 0;
		REAL32				m_FogDensity = 0.0f;
		REAL32				m_FogNear = 0.0f;
		REAL32				m_FogFar = 0.0f;

		BOOL				m_bLightingEnable = FALSE;
		i3::pack::VEC3D		m_LightDir;
		UINT32				m_LightAmbient = 0;
		UINT32				m_LightDiffuse = 0;
		UINT32				m_LightSpecular = 0;

		UINT32				m_ResCount = 0;
		UINT32				m_ElementCount = 0;
		UINT32				m_ViewportCount = 0;
		UINT32				m_TriggerCount = 0;

		char				m_szStringTablePath[256] = { 0 };
	};

	struct ALIGN4 LEVEL_SCENE_2
	{
		UINT8				m_ID[4] = { 'T', 'S', 'C', '2' };
		i3::pack::VEC3D		m_StartPos;
		REAL32				m_StartRot = 0.0f;
		REAL32				m_CameraNear = 0.0f;
		REAL32				m_CameraFar = 0.0f;
		REAL32				m_CameraFOV = 0.0f;

		BOOL				m_bFogEnable = FALSE;
		UINT32				m_FogColor = 0;
		INT8				m_FogMode = 0;
		REAL32				m_FogDensity = 0.0f;
		REAL32				m_FogNear = 0.0f;
		REAL32				m_FogFar = 0.0f;

		BOOL				m_bLightingEnable = FALSE;
		i3::pack::VEC3D		m_LightDir;
		i3::pack::COLORREAL	m_LightDiffuse;
		i3::pack::COLORREAL	m_LightSpecular;

		UINT32				m_ResCount = 0;
		UINT32				m_ElementCount = 0;
		UINT32				m_ViewportCount = 0;
		UINT32				m_TriggerCount = 0;

		char				m_szStringTablePath[256] = { 0 };

		INT32				m_LayerCount = 0;
		INT32				m_GroupCount = 0;

		UINT32				pad[128] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3LevelScene::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::LEVEL_SCENE_2 Info;
	i3::vector<i3LevelElement3D*>	ElmList;

	Result = i3LevelElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	GetPersistElement( ElmList);

	{
		Info.m_StartPos.x		= i3Vector::GetX( &m_StartPos);
		Info.m_StartPos.y		= i3Vector::GetY( &m_StartPos);
		Info.m_StartPos.z		= i3Vector::GetZ( &m_StartPos);

		Info.m_StartRot			= m_StartRot;

		Info.m_CameraNear		= m_CameraNear;
		Info.m_CameraFar		= m_CameraFar;
		Info.m_CameraFOV		= m_CameraFOV;

		Info.m_bFogEnable		= m_pFogEnableAttr->Get();
		Info.m_FogColor			= i3Color::ConvertRGBA32( m_pFogColorAttr->Get());
		Info.m_FogMode			= (INT8) i3Gfx::Convert( m_pFogModeAttr->Get());
		Info.m_FogDensity		= m_pFogDensityAttr->Get();
		Info.m_FogNear			= m_pFogNearFarAttr->GetNear();
		Info.m_FogFar			= m_pFogNearFarAttr->GetFar();

		Info.m_bLightingEnable	= m_pLightingEnableAttr->Get();

		Info.m_LightDiffuse.r	= m_pLightAttr->getDiffuse()->r;
		Info.m_LightDiffuse.g	= m_pLightAttr->getDiffuse()->g;
		Info.m_LightDiffuse.b	= m_pLightAttr->getDiffuse()->b;
		Info.m_LightDiffuse.a	= m_pLightAttr->getDiffuse()->a;

		Info.m_LightSpecular.r	= m_pLightAttr->getSpecular()->r;
		Info.m_LightSpecular.g	= m_pLightAttr->getSpecular()->g;
		Info.m_LightSpecular.b	= m_pLightAttr->getSpecular()->b;
		Info.m_LightSpecular.a	= m_pLightAttr->getSpecular()->a;

		Info.m_ElementCount		= (INT32)ElmList.size();
		Info.m_TriggerCount		= GetTriggerCount();
		Info.m_ViewportCount	= 1;

		strncpy( Info.m_szStringTablePath, m_szStringTablePath, sizeof( Info.m_szStringTablePath) -1);

		Info.m_LayerCount		= getLayerCount();
		Info.m_GroupCount		= getGroupCount();

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelScene::OnSave()", "Could not write scene information.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( getLayerCount() > 0)
		{
			Rc = SaveFromListToResourceFile(m_LayerList, pResFile);		// m_LayerList.SaveTo( pResFile);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Layer 정보를 저장할 수 없습니다.");
				return STREAM_ERR;
			}
		}

		if( getGroupCount() > 0)
		{
			Rc = SaveFromListToResourceFile(m_GroupList, pResFile);		//	m_GroupList.SaveTo( pResFile);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Group 정보를 저장할 수 없습니다.");
				return STREAM_ERR;
			}
		}
	}

	// Element Reference;
	{
		OBJREF ref;

		for(size_t i = 0; i < ElmList.size(); i++)
		{
			ref = (OBJREF) pResFile->GetObjectPersistID( ElmList[i]);

			Rc = pStream->Write( &ref, sizeof(ref));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3LevelScene::OnSave()", "Could not write element reference.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Trigger
	{
		OBJREF ref;

		for(INT32 i = 0; i < GetTriggerCount(); i++)
		{
			ref = (OBJREF) pResFile->GetObjectPersistID( GetTrigger( i));

			Rc = pStream->Write( &ref, sizeof(ref));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3LevelScene::OnSave()", "Could not write trigger reference.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Viewport
	{
		i3::pack::Level_VIEWPORT ViewInfo;
		i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();

		{
			pView->OnSave( &ViewInfo);

			Rc = pStream->Write( &ViewInfo, sizeof(ViewInfo));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3LevelScene::OnSave()", "Could not write viewport information.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3LevelScene::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::LEVEL_SCENE_2 Info;
//	i3::vector<i3LevelElement3D*>	ElmList;			// 쓰이지 않는듯 하여 주석..
	UINT32 i;
	OBJREF ref;

	Result = i3LevelElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Create();

	// Header
	{
		COLOR color;
		COLORREAL rcolor;

		Rc = pStream->Read( Info.m_ID, 4);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "%s 파일은 올바른 Level Design 파일이 아닙니다.", pResFile->getFileName());
			return STREAM_ERR;
		}

		pStream->SetPosition( -4, STREAM_CURRENT);

		if( memcmp( Info.m_ID, "TSC1", 4) == 0)
		{
			// 이전 Version의 헤더
			pack::LEVEL_SCENE_1 Old;

			Rc = pStream->Read( &Old, sizeof(Old));
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "%s 파일에서 기초 정보를 읽어들일 수 없습니다.", pResFile->getFileName());
				return STREAM_ERR;
			}

			Info.m_StartPos.x			= Old.m_StartPos.x;
			Info.m_StartPos.y			= Old.m_StartPos.y;
			Info.m_StartPos.z			= Old.m_StartPos.z;
			
			Info.m_CameraNear			= Old.m_CameraNear;
			Info.m_CameraFar			= Old.m_CameraFar;
			Info.m_CameraFOV			= Old.m_CameraFOV;

			Info.m_bFogEnable			= Old.m_bFogEnable;
			Info.m_FogColor				= Old.m_FogColor;
			Info.m_FogMode				= Old.m_FogMode;
			Info.m_FogDensity			= Old.m_FogDensity;
			Info.m_FogNear				= Old.m_FogNear;
			Info.m_FogFar				= Old.m_FogFar;

			Info.m_bLightingEnable		= Old.m_bLightingEnable;
			Info.m_LightDir.x			= Old.m_LightDir.x;
			Info.m_LightDir.y			= Old.m_LightDir.y;
			Info.m_LightDir.z			= Old.m_LightDir.z;

			i3Color::ConvertRGBA32( Old.m_LightDiffuse, &color);
			i3Color::Set( &rcolor, &color);
			Info.m_LightDiffuse.r		= rcolor.r;
			Info.m_LightDiffuse.g		= rcolor.g;
			Info.m_LightDiffuse.b		= rcolor.b;
			Info.m_LightDiffuse.a		= rcolor.a;
			
			i3Color::ConvertRGBA32( Old.m_LightSpecular, &color);
			i3Color::Set( &rcolor, &color);
			Info.m_LightSpecular.r		= rcolor.r;
			Info.m_LightSpecular.g		= rcolor.g;
			Info.m_LightSpecular.b		= rcolor.b;
			Info.m_LightSpecular.a		= rcolor.a;

			Info.m_ElementCount			= Old.m_ElementCount;
			Info.m_ViewportCount		= Old.m_ViewportCount;
			Info.m_TriggerCount			= Old.m_TriggerCount;

			strncpy( Info.m_szStringTablePath,	Old.m_szStringTablePath, sizeof(Info.m_szStringTablePath) - 1);
		}
		else
		{
			Rc = pStream->Read( &Info, sizeof(Info));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3LevelScene::OnLoad()", "Could not read scene information.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		i3Vector::Set( &m_StartPos, Info.m_StartPos.x, Info.m_StartPos.y, Info.m_StartPos.z);
		m_StartRot			= Info.m_StartRot;
		m_CameraNear		= Info.m_CameraNear;
		m_CameraFar			= Info.m_CameraFar;
		m_CameraFOV			= Info.m_CameraFOV;

		m_pFogEnableAttr->Set( Info.m_bFogEnable != FALSE);

		i3Color::ConvertRGBA32( Info.m_FogColor, &color);
		m_pFogColorAttr->Set( &color);

		m_pFogModeAttr->Set( i3Gfx::Convert( (I3G_PERSIST_FOG_MODE) Info.m_FogMode));

		m_pFogDensityAttr->Set( Info.m_FogDensity);

		m_pFogNearFarAttr->SetNear( Info.m_FogNear);
		m_pFogNearFarAttr->SetFar( Info.m_FogFar);

		m_pLightingEnableAttr->Set( Info.m_bLightingEnable != FALSE);

		rcolor.r = Info.m_LightDiffuse.r;
		rcolor.g = Info.m_LightDiffuse.g;
		rcolor.b = Info.m_LightDiffuse.b;
		rcolor.a = Info.m_LightDiffuse.a;
		m_pLightAttr->setDiffuse( &rcolor);

		rcolor.r = Info.m_LightSpecular.r;
		rcolor.g = Info.m_LightSpecular.g;
		rcolor.b = Info.m_LightSpecular.b;
		rcolor.a = Info.m_LightSpecular.a;
		m_pLightAttr->setSpecular( &rcolor);

		strncpy( m_szStringTablePath, Info.m_szStringTablePath, sizeof(m_szStringTablePath) -1);
		if( m_szStringTablePath[0] != 0 )
		{
			LoadStringTable( m_szStringTablePath );
		}

		// Layer
		if( Info.m_LayerCount > 0)
		{
			removeAllLayer();

			Rc = LoadFromResourceFileToList(m_LayerList, pResFile, true);		// m_LayerList.LoadFrom( pResFile, true);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Layer 정보를 읽어들일 수 없습니다.");
				return STREAM_ERR;
			}

			if( m_LayerList.size() > 0)
			{
				setCurrentLayer( m_LayerList[0]);
			}
		}
		else
		{
			// 기본 Layer를 추가해둔다.
			i3LevelLayer * pLayer = i3LevelLayer::new_object_ref();

			pLayer->SetName( "Default");
			addLayer( pLayer);
		}

		// Group
		if( Info.m_GroupCount > 0)
		{
			Rc = LoadFromResourceFileToList(m_GroupList, pResFile, true);	// m_GroupList.LoadFrom( pResFile, true);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Group 정보를 읽어들일 수 없습니다.");
				return STREAM_ERR;
			}
		}
	}

	// Element
	for( i = 0; i < Info.m_ElementCount; i++)
	{
		i3LevelElement3D * pElm;

		Rc = pStream->Read( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelScene::OnLoad()", "Could not read element reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pElm = (i3LevelElement3D *) pResFile->FindObjectByID( ref);

		if( pElm != nullptr)
		{
			AddElement( pElm, "Element");
		}
		else
		{
			i3Log( "i3LevelScene::OnLoad()", "Could not found element object : %08x", ref);
		}
	}

	// Trigger
	for( i = 0; i < Info.m_TriggerCount; i++)
	{
		i3LevelTrigger * pTrig;

		Rc = pStream->Read( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelScene::OnLoad()", "Could not read trigger reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pTrig = (i3LevelTrigger *) pResFile->FindObjectByID( ref);

		if( pTrig != nullptr)
		{
			AddTrigger( pTrig);
		}
		else
		{
			i3Log( "i3LevelScene::OnLoad()", "Could not found trigger object : %08x", ref);
		}
	}

	// Viewport
	{
		i3::pack::Level_VIEWPORT ViewInfo;
		i3LevelViewport * pView;

		for( i = 0; i < Info.m_ViewportCount; i++)
		{
			Rc = pStream->Read( &ViewInfo, sizeof(ViewInfo));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3LevelScene::OnLoad()", "Could not read viewport information.");
				return STREAM_ERR;
			}
			Result += Rc;

			pView = i3LevelViewport::GetCurrentFocusViewport();
			if( pView != nullptr)
			{
				pView->OnLoad( &ViewInfo);
			}
			else
			{
				i3Log( "i3LevelScene::OnLoad()", "Could not found viewport(0x08X).", ViewInfo.m_ID);
			}
		}
	}

	return Result;
}


bool i3LevelScene::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;
	i3::vector<i3LevelElement3D*> ElmList;

	rv = i3LevelElement::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	// Start Position
	i3Vector::SetToXML( pXML, "StartPos", &m_StartPos);

	// Start Rot
	pXML->addAttr( "StartRot", m_StartRot);

	pXML->addAttr( "CameraNear",		m_CameraNear);
	pXML->addAttr( "CameraFar",		m_CameraFar);
	pXML->addAttr( "CameraFOV",		m_CameraFOV);

	pXML->addAttr( "Lighting",		m_pLightingEnableAttr->Get());

	i3Color::SetToXML( pXML, "LightDiffuse", m_pLightAttr->getDiffuse());
	i3Color::SetToXML( pXML, "LightSpecular", m_pLightAttr->getSpecular());

	pXML->addAttr( "StringTablePath",		m_szStringTablePath);

	SaveFromListToXML( m_LayerList, "Layers", pFile, pXML, true);			//m_LayerList.SaveToXML( "Layers", pFile, pXML, true);
	SaveFromListToXML( m_GroupList, "Groups", pFile, pXML, true);			//m_GroupList.SaveToXML( "Groups", pFile, pXML, true);

	GetPersistElement( ElmList);
	SaveFromListToXML(ElmList, "Elements", pFile, pXML, true);	//	ElmList.SaveToXML( "Elements", pFile, pXML, true);

	SaveFromListToXML( m_TriggerList, "Triggers", pFile, pXML, true);	//m_TriggerList.SaveToXML( "Triggers", pFile, pXML, true);

	return true;
}

bool i3LevelScene::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;
	bool bFlag;
	COLORREAL col;
	
	rv = i3LevelElement::OnLoadXML( pFile, pXML);
	if( rv == false)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		return false;
	}

	Create();

	i3Vector::GetFromXML( pXML, "StartPos", &m_StartPos);

	pXML->getAttr( "StartRot",		& m_StartRot);
	pXML->getAttr( "CameraNear",	& m_CameraNear);
	pXML->getAttr( "CameraFar",		& m_CameraFar);
	pXML->getAttr( "CameraFOV",		& m_CameraFOV);

	pXML->getAttr( "Lighting",		& bFlag);
	m_pLightingEnableAttr->Set( bFlag);

	i3Color::GetFromXML( pXML, "LightDiffuse", &col);
	m_pLightAttr->setDiffuse( &col);

	i3Color::GetFromXML( pXML, "LightSpecular", &col);
	m_pLightAttr->setSpecular( &col);

	strncpy( m_szStringTablePath, pXML->getAttr( "StringTablePath"), sizeof(m_szStringTablePath) - 1);
	if( m_szStringTablePath[0] != 0)
	{
		LoadStringTable( m_szStringTablePath);
	}

	// Layer
	{
		removeAllLayer();

		LoadFromXMLToList( m_LayerList, "Layers", pFile, pXML, true);		// m_LayerList.LoadFromXML( "Layers", pFile, pXML, true);

		if( m_LayerList.size() > 0)
		{
			setCurrentLayer( m_LayerList[0]);
		}
	}

	// Group
	{
		LoadFromXMLToList( m_GroupList, "Groups", pFile, pXML, true);		// m_GroupList.LoadFromXML( "Groups", pFile, pXML, true);
	}

	// Elements
	{
		i3::vector<i3LevelElement3D*>	list;

		LoadFromXMLToList(list, "Elements", pFile, pXML, true); // list.LoadFromXML( "Elements", pFile, pXML, true);

		for(size_t i = 0; i < list.size(); i++)
		{
			i3LevelElement3D * pElm = list[i];

			if( pElm != nullptr)
			{
				AddElement( pElm, "Element");
				I3_MUST_RELEASE(pElm);			// LoadFromXMLToList()함수에 유의..내부에서 AddRef()가 존재..(2012.09.11.수빈)
			}
			else
			{
				I3ERRLOG( i3Error::_READFAIL);
			}
			//
		}
	}

	// Triggers
	{
		i3::vector<i3LevelTrigger*>	list;

		LoadFromXMLToList(list,"Triggers", pFile, pXML, true); // list.LoadFromXML( "Triggers", pFile, pXML, true);

		for(size_t i = 0; i < list.size(); i++)
		{
			i3LevelTrigger * pTrig = list[i];

			AddTrigger( pTrig);

			I3_MUST_RELEASE(pTrig);
		}
	}

	// Layer별 설정
	for(INT32 i = 0; i < getLayerCount(); i++)
	{
		i3LevelLayer * pLayer = getLayer( i);

		if( pLayer->getState() & I3LV_LAYER_STATE_HIDED)
		{
			pLayer->Show( FALSE);
		}

		if( pLayer->getState() & I3LV_LAYER_STATE_FREEZED)
		{
			pLayer->Freeze( TRUE);
		}

		if( pLayer->getState() & I3LV_LAYER_STATE_SELECTED)
		{
			setCurrentLayer( pLayer);
		}
	}

	return true;
}
