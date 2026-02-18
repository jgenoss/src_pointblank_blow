#include "pch.h"
#include "GlobalVariables.h"
#include "BattleHUD_MiniMap.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "Camera/GameCamera_OnObject.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "Weapon/WeaponOnObject.h"
#include "BattleHud.h"
#include "ConfigRenderBattle.h"
#include "Mode/Domination/DominationSkillSystem.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameContextUtil.h"
#include "GameMissionManager.h"
#include "BattleObjContext.h"
#include "MainWeapon_C5.h"
#include "Weapon/WeaponItemID.h"

#include "i3Base/string/ext/utf16_to_mb.h"


#define ENABLE_C5WAVE

bool DestroyObjectAttackState::_isRedUnderAttack = false;
bool DestroyObjectAttackState::_isBlueUnderAttack = false;


bool DestroyObjectAttackState::IsUnderAttack(INT32 index)
{
	bool attackState = false;

	if (index == 0)
		attackState = _isRedUnderAttack;
	else
		attackState = _isBlueUnderAttack;

	return attackState;
}

void DestroyObjectAttackState::SetUnderAttack(INT32 index, bool isAttacked)
{
	if (index == 0)
		_isRedUnderAttack = isAttacked;
	else
		_isBlueUnderAttack = isAttacked;
}

I3_CLASS_INSTANCE( CBattleHUD_MiniMap); //, i3GameObjBase);


struct HUD_MINIMAP_INFO
{
	UINT32	nMissionType;
	REAL32	rRealWorldScale;			//월드의 실제 크기( X축길이와 Z축 길이중 큰쪽으로 선택)
	INT32	nRealImageSize_OnBaking;	//미니맵 이미지를 만들때 실제 최대 사이즈( 미니맵 이미지는 용량을줄이고 최적화하기위해 필요한 부분만 잘라서 만들어져있습니다.)
	INT32	nCenterPosX, nCenterPosY;	//미니맵에서 중앙으로 설정된 픽셀의 위치( 실제 월드위치위 미니맵의 위치를 맞추기위해 사용되는 정보입니다.
	INT32	nAdditionalImageCnt;		//이미지 리소스가 여러개인 맵의 경우 추가 이미지의 갯수( default : 0)
	char	sMapImage[MAX_PATH];		//현재 스테이지에서 쓰여지는 미니맵의 텍스쳐 이름입니다.
	char	sMapImage_text[MAX_PATH];		//현재 스테이지에서 쓰여지는 미니맵의 텍스쳐 이름입니다.
};

static	REAL32	s_rMiniMapScale = 0.8f;

#define	MINIMAP_SCALE_MAX	1.1f
#define MINIMAP_SCALE_MIN	0.5f

namespace inner { CBattleHUD_MiniMap* this_ptr = 0; }
CBattleHUD_MiniMap* CBattleHUD_MiniMap::instance() { return inner::this_ptr; }

CBattleHUD_MiniMap::CBattleHUD_MiniMap(void)
{
	inner::this_ptr = this;

	m_pSceneNode			= i3Node::new_object();

	m_pCurStage				= nullptr;
	m_pAttrRoot				= nullptr;
	m_pViewport				= nullptr;

	m_pHUDTexture			= nullptr;
	m_pHUD3Texture			= nullptr;
	m_pMinimapTexture		= nullptr;	

	m_pMinimapTextTexture		= nullptr; 

	m_idxFloor				= 0;

	m_pSprBackGround		= nullptr;
	m_pSprMapImage			= nullptr;
	m_pUnitImage			= nullptr;
	m_pOuterUnitImage		= nullptr;
	m_pDeathUnitImage		= nullptr;
	m_pDeathOuterUnitImage	= nullptr;
#ifdef DOMI_DINO_MINIMAP_UNIT
	m_pUnitDomiDino			= nullptr;
	m_pOutlineUnitDomiDino	= nullptr;
#endif

#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
	m_pDomiSkillObjUnitMgr = new DomiSkillObjOldMinimapUnitMgr(this);
	MEMDUMP_NEW( m_pDomiSkillObjUnitMgr, sizeof(DomiSkillObjOldMinimapUnitMgr));
#endif

	m_pMiniMapScaleButton	= nullptr;

	m_nCurrentMode			= MINIMAP_MODE_UNKNOWN;

	m_pMissionObject		= nullptr;
	m_pMissionObjectWavRoot	= nullptr;
	m_pMissionObjectWave	= nullptr;

	m_pUsurpationObject		= nullptr;

	m_pWeaponC5WaveRoot		= nullptr;
	m_pWeaponC5WaveSprite	= nullptr;
	
	m_rMapScale				= 1.0f;

	i3Vector::Zero( &m_vMiniMapScale );
	i3Vector::Zero( &m_vHoldMapScale );

	m_rHalfScreenX			= 0.f;
	m_rHalfScreenY			= 0.f;

	m_rMapCenterX			= 0.f;
	m_rMapCenterY			= 0.f;

	m_rScaleFactoX			= 0.f;

	m_rScaleFactoY			= 0.f;

	m_rMapHeight			= 0.0f;
	m_rMapWidth				= 0.0f;

	m_rMiniMapScale			= s_rMiniMapScale;
	m_rMiniMapScaleElapsed	= 0.0f;

	m_pMapInfo				= nullptr;

	m_rMissionObjectWavScale= 1.0f;

	for(INT32 i=0; i<MAX_ALL_USER; i++)
	{
		m_rDeathUnitImageDeltaTime[i] = 0.0f;
		m_bDeathUnitOnceUpdate[i] = false;
	}
	
	m_rC5WaveScaleTimer		= 0.f;
	m_rC5WaveScaleMultiple	= 5.f;
	m_bChangedViewport		= false;

	for(int i=0; i<MINIMAP_CLEAVERKNIFE_MAX; i++)
		m_pCleaverKnife[i] = nullptr;

	m_pCurLocationText		= nullptr;

#ifdef UITEST_MINIMAP_MOVE
	m_minimapMoveFactorX = 0.0f;
	m_minimapMoveFactorY = 0.0f;
#endif
}

CBattleHUD_MiniMap::~CBattleHUD_MiniMap(void)
{
	inner::this_ptr = 0;

	I3_SAFE_RELEASE( m_pAttrRoot);

	//Mission Object
	I3_SAFE_RELEASE( m_pHUDTexture);	
	I3_SAFE_RELEASE( m_pHUD3Texture);	

	I3_SAFE_RELEASE( m_pMinimapTexture);	


	i3::cu::for_each_safe_release(m_plistMiniMapTextures, &i3Texture::Release);
	
	I3_SAFE_RELEASE( m_pMinimapTextTexture );

	i3::cu::for_each_safe_release(m_plistMiniMapTextTextures, &i3Texture::Release);
	
	I3_SAFE_RELEASE( m_pSprBackGround);
	I3_SAFE_RELEASE( m_pSprMapImage);
	I3_SAFE_RELEASE( m_pUnitImage);
	I3_SAFE_RELEASE( m_pOuterUnitImage);
#ifdef DOMI_DINO_MINIMAP_UNIT
	I3_SAFE_RELEASE( m_pUnitDomiDino);
	I3_SAFE_RELEASE( m_pOutlineUnitDomiDino);
#endif
#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
	I3_SAFE_DELETE( m_pDomiSkillObjUnitMgr);
#endif

	I3_SAFE_RELEASE( m_pDeathUnitImage);
	I3_SAFE_RELEASE( m_pDeathOuterUnitImage);

	I3_SAFE_RELEASE( m_pMiniMapScaleButton);

	I3_SAFE_RELEASE( m_pViewport);

	I3_SAFE_RELEASE( m_pMissionObject);

	I3_SAFE_RELEASE( m_pMissionObjectWavRoot);
	I3_SAFE_RELEASE( m_pMissionObjectWave);

	I3_SAFE_RELEASE( m_pUsurpationObject );
	I3_SAFE_RELEASE( m_pWeaponC5WaveRoot);
	I3_SAFE_RELEASE( m_pWeaponC5WaveSprite);

	I3MEM_SAFE_FREE( m_pMapInfo);

	I3_SAFE_NODE_RELEASE( m_pCurLocationText);
}

void CBattleHUD_MiniMap::_InitViewport()
{
	m_pAttrRoot = i3AttrSet::new_object();
	m_pSceneNode->AddChild( m_pAttrRoot);

	// Viewport
	{
		m_pViewport = i3ViewportAttr::new_object();
		m_pViewport->SetViewportRect( 9, 9, 153, 153);
		m_pAttrRoot->AddAttr( m_pViewport);
	}	

	// Texture Enable = true
	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

		pAttr->Set( true);

		m_pAttrRoot->AddAttr( pAttr);
	}

	// Z Writer Enable = false;
	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

		pAttr->Set( false);

		m_pAttrRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		pAttr->Set( false);

		m_pAttrRoot->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

		pAttr->Set( I3G_FACE_CULL_NONE);

		m_pAttrRoot->AddAttr( pAttr);
	}

	// Lighting Enable = false
	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( false);

		m_pAttrRoot->AddAttr( pAttr);
	}

	// Alpha Test Enable = true
	{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();

		pAttr->Set( true);

		m_pAttrRoot->AddAttr( pAttr);
	}

	// Alpha Function
	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);

		m_pAttrRoot->AddAttr( pAttr);
	}

	// Projection Matrix Attr
	{
		i3ProjectMatrixAttr * pAttr = i3ProjectMatrixAttr::new_object_ref();
		MATRIX mtx;

		pAttr->SetMatrix( &mtx);

		m_pAttrRoot->AddAttr( pAttr);
	}


	// View Matrix Attr
	{
		i3ViewMatrixAttr * pAttr = i3ViewMatrixAttr::new_object_ref();
		MATRIX mtx;

		pAttr->SetMatrix( &mtx);
		m_pAttrRoot->AddAttr( pAttr);
	}

	{
		i3TextureFunctionAttr * pAttr = i3TextureFunctionAttr::new_object_ref();

		pAttr->SetFunction( I3G_TEXTURE_FUNC_MODULATE);

		m_pAttrRoot->AddAttr( pAttr);
	}

	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);

		m_pAttrRoot->AddAttr( pAttr);
	}

	{
		i3TextureWrapAttr * pAttr = i3TextureWrapAttr::new_object_ref();

		pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_CLAMP);
		pAttr->SetVertWrap( I3G_TEXTURE_WRAP_CLAMP);

		m_pAttrRoot->AddAttr( pAttr);
	}
}

bool CBattleHUD_MiniMap::Create( CStageBattle * pCurStage, i3Node* pParent)
{
	if( ! i3GameObjBase::Create( pParent ) )
	{
		return false;
	}

	I3ASSERT( i3::kind_of<CStageBattle*>(pCurStage));

	m_pCurStage = pCurStage;

	i3IniParser Parser;

	char conv[MAX_PATH];
	
	const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();
	if( pszStageName == nullptr)
		return false;

	sprintf_s( conv, "World/%s/Images/minfo_%s.txt", pszStageName, pszStageName);

	if( !Parser.OpenFromFile( conv))
	{
		I3TRACE1(" -- load fail stage name : %s (minfo)\n", pszStageName);
		i3::string str;		i3::utf16_to_mb(GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_FATAL_NO_MINIMAP_INFO"), str);
		I3PRINTLOG(I3LOG_FATAL,  str  );/*유효한 미니맵 정보가 없심미다*/
		I3ASSERT_0;
	}

	I3ASSERT( m_pMapInfo == nullptr);
	m_pMapInfo = (void*)i3MemAlloc( sizeof( HUD_MINIMAP_INFO));
	
	if( !Parser.ReadSection( "Default"))
	{
		i3::string str;		i3::utf16_to_mb(GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_FATAL_WRONG_SECTION"), str);
		I3PRINTLOG(I3LOG_FATAL, str);/*잘못된 section*/
		return false;
	}

	//nMissionType은 쓰는 곳이 없다. 일단 StageID값을 넣어버린다.
	((HUD_MINIMAP_INFO*)m_pMapInfo)->nMissionType = MyRoomInfoContext::i()->getStage()->m_ui32StageID;
	if( !Parser.GetValue( "RealMapScale", 0.0f, &((HUD_MINIMAP_INFO*)m_pMapInfo)->rRealWorldScale))
	{
		_CalcMapScale();
	}
	else
	{
		m_rMapScale = ((HUD_MINIMAP_INFO*)m_pMapInfo)->rRealWorldScale;
	}

	if( !Parser.GetValue( "RealBakedSize", 0, &((HUD_MINIMAP_INFO*)m_pMapInfo)->nRealImageSize_OnBaking))
	{
		i3::string str;		i3::utf16_to_mb(GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_FATAL_WRONG_SIZE"), str);

		I3PRINTLOG(I3LOG_FATAL, str);/*잘못된 RealBakedSize*/
		return false;
	}

	if( !Parser.GetValue( "CenterX", 0, &((HUD_MINIMAP_INFO*)m_pMapInfo)->nCenterPosX))
	{
		i3::string str;		i3::utf16_to_mb(GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_FATAL_WRONG_CENTER_X"), str);
		I3PRINTLOG(I3LOG_FATAL, str);/*잘못된 CenterX*/
		return false;
	}

	if( !Parser.GetValue( "CenterY", 0, &((HUD_MINIMAP_INFO*)m_pMapInfo)->nCenterPosY))
	{
		i3::string str;		i3::utf16_to_mb(GAME_RCSTRING("STR_TBL_INGAME_HUD_ERROR_FATAL_WRONG_CENTER_Y"), str);
		I3PRINTLOG(I3LOG_FATAL, str);/*잘못된 CenterY*/
		return false;
	}

	sprintf_s( ((HUD_MINIMAP_INFO*)m_pMapInfo)->sMapImage, "World/%s/Images/mini_%s.i3i", pszStageName, pszStageName);

	sprintf_s( ((HUD_MINIMAP_INFO*)m_pMapInfo)->sMapImage_text, "World/%s/Images/mini_%s_text.i3i", pszStageName, pszStageName);

	Parser.GetValue( "AdditionalImgCnt", 0, &((HUD_MINIMAP_INFO*)m_pMapInfo)->nAdditionalImageCnt);
	Parser.Close();

	return CreateMiniMap();
}

void CBattleHUD_MiniMap::SetCurrentFloorIdx(INT32 nFloor)
{
	m_idxFloor = nFloor;
	i3::vector<i3Texture*>* pMiniMapList = nullptr; 

	i3Texture	* pTex;

	switch (m_nCurrentMode)
	{
	case	MINIMAP_MODE_DEFAULT:
		pMiniMapList = &m_plistMiniMapTextures;
		pTex = m_pMinimapTexture;
		break;
	case	MINIMAP_MODE_FULLSCREEN:
	default:
		pMiniMapList = &m_plistMiniMapTextTextures;
		pTex = m_pMinimapTextTexture;
		break;
	}
	
	if(m_idxFloor <= -1)return; 

	if( m_idxFloor == 0 )
	{//1층
		m_pSprMapImage->SetTexture( pTex );		
	}
	else 
	{
		//층이 있는 미니맵
		
		if( m_idxFloor >= (INT32)pMiniMapList->size() )
		{
			I3PRINTLOG(I3LOG_FATAL,  "invalid floor index ");			// 지금은 FATAL보다 TRACE가 더 좋을지도 모른다..
			return;
		}

		i3Texture * pTex2 = (*pMiniMapList)[m_idxFloor];
		I3ASSERT( pTex2 != nullptr);
		m_pSprMapImage->SetTexture( pTex2);
	}

	return; 
}

void	CBattleHUD_MiniMap::SetCurrentLocationText( const i3::wliteral_range& wLocationRng)
{
	m_pCurLocationText->SetTextEllipsis(wLocationRng);
}

void	CBattleHUD_MiniMap::_CalcMapScale()
{
	i3World * pWorld = m_pCurStage->getWorld();
	I3ASSERT( pWorld != nullptr);

	i3AttrSet * pAttrSet = pWorld->getWorldRoot();
	I3ASSERT( pAttrSet != nullptr);

	i3BoundBox Box;
	pAttrSet->GetWrappingBound( &Box);

	REAL32	rWidth	= Box.GetMax()->x - Box.GetMin()->x;
	REAL32	rHeight = Box.GetMax()->z - Box.GetMin()->z;

	m_rMapScale	= ( rWidth > rHeight) ? rWidth : rHeight;
}

void	CBattleHUD_MiniMap::_InitBGScene(void)
{
	// 전체 미니맵 화면의 배경에 사용될 스프라이트
	m_pSprBackGround = i3Sprite2D::new_object();
	m_pSprBackGround->SetTexture( m_pHUDTexture ); 

	m_pSprBackGround->Create(1, true, true);

	m_pSprBackGround->SetEnable(0, true);
	m_pSprBackGround->SetColor( 0, 255, 255, 255, 255);
	m_pSprBackGround->SetTextureCoord(0, 340.7f, 6.0f, 504.0f, 187.0f);
	m_pSprBackGround->SetRect( 0, 5, 5, 164, 181);
}

bool	CBattleHUD_MiniMap::_InitStageImage(void)
{
	i3Viewer * pViewer = g_pFramework->GetViewer();
	if( m_pMapInfo == nullptr)return false;

	HUD_MINIMAP_INFO * pInfo = (HUD_MINIMAP_INFO*)m_pMapInfo;

	//MiniMap Texture 
	m_pMinimapTexture = g_pFramework->GetResourceManager()->LoadTexture( pInfo->sMapImage );
	
	if( pInfo->nAdditionalImageCnt > 0)
	{
		m_plistMiniMapTextures.push_back( m_pMinimapTexture );
		I3_MUST_ADDREF(m_pMinimapTexture);

		char conv[MAX_PATH];
		const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();
		if( pszStageName == nullptr)
			return false;

		for( INT32 i = 0;i < pInfo->nAdditionalImageCnt; ++i)
		{
			sprintf_s( conv, "World/%s/Images/mini_%s_%02d.i3i", pszStageName, pszStageName, i+1);

			i3Texture * pNewTex =g_pFramework->GetResourceManager()->LoadTexture( conv);
			I3ASSERT( pNewTex != nullptr);
			m_plistMiniMapTextures.push_back( pNewTex);
		}
	}

	//Text MiniMap Texture
	m_pMinimapTextTexture = g_pFramework->GetResourceManager()->LoadTexture( pInfo->sMapImage_text );
	
	if( pInfo->nAdditionalImageCnt > 0)
	{
		m_plistMiniMapTextTextures.push_back( m_pMinimapTextTexture );
		I3_MUST_ADDREF(m_pMinimapTextTexture);

		char conv[MAX_PATH];
		const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();
		if( pszStageName == nullptr)
			return false;

		for( INT32 i = 0;i < pInfo->nAdditionalImageCnt; ++i)
		{
			sprintf_s( conv, "World/%s/Images/mini_%s_text_%02d.i3i", pszStageName, pszStageName, i+1);

			i3Texture * pNewTex =g_pFramework->GetResourceManager()->LoadTexture( conv);
			I3ASSERT( pNewTex != nullptr);
			m_plistMiniMapTextTextures.push_back( pNewTex);
		}
	}

	m_pSprMapImage = i3Sprite2D::new_object();
	m_pSprMapImage->Create( 1, true, true);
	m_pSprMapImage->SetEnable(0, true);
	m_pSprMapImage->SetColor( 0, 255, 255, 255 ,255);
	m_pSprMapImage->SetTexture( m_pMinimapTexture );
	m_pSprMapImage->SetTextureCoord(0, 0.0f, 0.0f, pInfo->nCenterPosX * 2.0f, pInfo->nCenterPosY * 2.0f);	
	i3Vector::Set( &m_vMiniMapScale, (REAL32)pViewer->GetViewWidth() / m_pViewport->getWidth(), (REAL32)pViewer->GetViewHeight() / m_pViewport->getHeight());
	m_pSprMapImage->SetRect( 0, 0.0f, 0.0f, getX( &m_vMiniMapScale) * m_rMapWidth, getY( &m_vMiniMapScale) * m_rMapHeight);
	m_pAttrRoot->AddChild( m_pSprMapImage );

	return true;
}

#define UNIT_MAX_COUNT 16

bool	CBattleHUD_MiniMap::_CreateUnitTexture(void)
{
	m_pUnitImage = i3Sprite2D::new_object();

	m_pUnitImage->Create( UNIT_MAX_COUNT, true, true);
	m_pUnitImage->SetEnable(0, true);

	m_pUnitImage->SetTexture( m_pHUDTexture);
	m_pUnitImage->SetTextureCoord( 0, 227.0f, 44.0f, 245.0f, 78.50f);
	m_pUnitImage->SetColor( 0, 255, 255, 255, 255);

	m_pUnitImage->SetRect( 0,  g_pViewer->GetViewWidth() * 0.5f - 8.5f * getX( &m_vMiniMapScale),
								g_pViewer->GetViewHeight() * 0.5f - 17.25f * getY( &m_vMiniMapScale),
								17.0f * getX( &m_vMiniMapScale), 34.5f * getY( &m_vMiniMapScale));

	{
		for( INT32 i = 1; i < UNIT_MAX_COUNT; ++i)
		{
			m_pUnitImage->SetTextureCoord( i, 172.0f, 89.0f, 182.0f, 111.0f);
			m_pUnitImage->SetColor( i, 255, 255, 255, 255);
			m_pUnitImage->SetEnable( i, true);

			m_pUnitImage->SetRect( i,  g_pViewer->GetViewWidth() * 0.5f - 5.0f * getX( &m_vMiniMapScale),
									g_pViewer->GetViewHeight() * 0.5f - 11.0f * getY( &m_vMiniMapScale),
									10.0f * getX( &m_vMiniMapScale), 22.0f * getY( &m_vMiniMapScale));
		}

		m_pAttrRoot->AddChild( m_pUnitImage);
	}

	{
		m_pOuterUnitImage	= i3Sprite2D::new_object();
		m_pOuterUnitImage->Create( UNIT_MAX_COUNT, true, true);
		m_pOuterUnitImage->SetTexture( m_pHUDTexture);

		for( INT32 i = 0; i < UNIT_MAX_COUNT; ++i)
		{
			m_pOuterUnitImage->SetTextureCoord( i, 186.0f, 86.0f, 196.0f, 96.0f);
			m_pOuterUnitImage->SetColor( i, 255, 255, 255, 255);
			m_pOuterUnitImage->SetEnable(i, false);

			m_pOuterUnitImage->SetRect( i,  g_pViewer->GetViewWidth() * 0.5f - 5.0f * getX( &m_vMiniMapScale),
									g_pViewer->GetViewHeight() * 0.5f - 5.0f * getY( &m_vMiniMapScale),
									10.0f * getX( &m_vMiniMapScale), 10.0f * getY( &m_vMiniMapScale));
		}

		m_pAttrRoot->AddChild( m_pOuterUnitImage);
	}

	{
		m_pDeathUnitImage = i3Sprite2D::new_object();
		m_pDeathUnitImage->Create( UNIT_MAX_COUNT, true, true);
		m_pDeathUnitImage->SetTexture( m_pHUDTexture);

		for( INT32 i = 0; i < UNIT_MAX_COUNT; ++i)
		{
			m_pDeathUnitImage->SetTextureCoord( i, 255.0f, 69.0f, 267.0f, 81.0f);
			m_pDeathUnitImage->SetColor( i, 255, 255, 255, 255);
			m_pDeathUnitImage->SetEnable(i, false);

			m_pDeathUnitImage->SetRect( i,  g_pViewer->GetViewWidth() * 0.5f - 6.0f * getX( &m_vMiniMapScale),
									g_pViewer->GetViewHeight() * 0.5f - 6.0f * getY( &m_vMiniMapScale),
									12.0f * getX( &m_vMiniMapScale), 12.0f * getY( &m_vMiniMapScale));
		}

		m_pAttrRoot->AddChild( m_pDeathUnitImage);
	}

	{
		m_pDeathOuterUnitImage = i3Sprite2D::new_object();
		m_pDeathOuterUnitImage->Create( UNIT_MAX_COUNT, true, true);
		m_pDeathOuterUnitImage->SetTexture( m_pHUDTexture);

		for( INT32 i = 0; i < UNIT_MAX_COUNT; ++i)
		{
			m_pDeathOuterUnitImage->SetTextureCoord( i, 251.5f, 89.0f, 270.5f, 108.0f);
			m_pDeathOuterUnitImage->SetColor( i, 255, 255, 255, 255);
			m_pDeathOuterUnitImage->SetEnable(i, false);

			m_pDeathOuterUnitImage->SetRect( i,  g_pViewer->GetViewWidth() * 0.5f - 9.5f * getX( &m_vMiniMapScale),
									g_pViewer->GetViewHeight() * 0.5f - 9.5f * getY( &m_vMiniMapScale),
									19.0f * getX( &m_vMiniMapScale), 19.0f * getY( &m_vMiniMapScale));
		}

		m_pAttrRoot->AddChild( m_pDeathOuterUnitImage);
	}

	return true;
}

bool CBattleHUD_MiniMap::IncreaseMiniMapScale( bool bIncrease, REAL32 rAmount)
{
	if( m_nCurrentMode != MINIMAP_MODE_DEFAULT)	return false;
	if( bIncrease && m_rMiniMapScale >= MINIMAP_SCALE_MAX)	return false;
	if( !bIncrease && m_rMiniMapScale <= MINIMAP_SCALE_MIN)	return false;

	if( bIncrease)
	{
		m_rMiniMapScale += rAmount;

		if( m_pMiniMapScaleButton->GetEnable(0))
		{
			m_pMiniMapScaleButton->SetEnable( 0, false);
		}

		if( m_rMiniMapScale >= MINIMAP_SCALE_MAX)
		{
			if( !m_pMiniMapScaleButton->GetEnable(4))
			{
				m_pMiniMapScaleButton->SetEnable( 4, true);
			}
		}
		else
		{
			if( !m_pMiniMapScaleButton->GetEnable(2))
			{
				m_pMiniMapScaleButton->SetEnable( 2, true);
			}
		}
	}
	else
	{
		m_rMiniMapScale -= rAmount;

		if( m_pMiniMapScaleButton->GetEnable(1))
		{
			m_pMiniMapScaleButton->SetEnable( 1, false);
		}

		if( m_rMiniMapScale + rAmount > MINIMAP_SCALE_MAX)
		{
			if( !m_pMiniMapScaleButton->GetEnable(5))
			{
				m_pMiniMapScaleButton->SetEnable( 5, true);
			}
		}
		else
		{
			if( !m_pMiniMapScaleButton->GetEnable(3))
			{
				m_pMiniMapScaleButton->SetEnable( 3, true);
			}
		}
	}

	m_rMiniMapScale = MINMAX( MINIMAP_SCALE_MIN, m_rMiniMapScale, MINIMAP_SCALE_MAX);
	s_rMiniMapScale = m_rMiniMapScale;

	m_rMiniMapScaleElapsed = 0.0f;

	_UpdateScale();

	return true;
}

bool CBattleHUD_MiniMap::_UpdateMiniMapButton( REAL32 tm)
{
	if( m_nCurrentMode == MINIMAP_MODE_DEFAULT)
	{
		if( m_rMiniMapScaleElapsed < 0.1f && m_rMiniMapScaleElapsed + tm > 0.1f)
		{
			_ResetCurrentMiniMapButton(true);
		}

        m_rMiniMapScaleElapsed += tm;

		return true;
	}

	return false;
}

//viewport의 size가 변경되는경우에 새로 계산해주어야 할것들입니다.
bool	CBattleHUD_MiniMap::_UpdateScale(void)
{
	if( m_pMapInfo == nullptr)
		return false;

	HUD_MINIMAP_INFO * pInfo = (HUD_MINIMAP_INFO*)m_pMapInfo;

	REAL32 rScale = m_rMiniMapScale;
	if( m_nCurrentMode == MINIMAP_MODE_FULLSCREEN)
	{
		rScale = 1.0f;
	}

	i3Vector::Set( &m_vMiniMapScale, rScale * (REAL32)g_pViewer->GetViewWidth() / m_pViewport->getWidth(), rScale * (REAL32)g_pViewer->GetViewHeight() / m_pViewport->getHeight());

	m_rHalfScreenX	= g_pViewer->GetViewWidth() * 0.5f;
	m_rHalfScreenY	= g_pViewer->GetViewHeight() * 0.5f;

	m_rMapCenterX	= - getX( &m_vMiniMapScale) * pInfo->nCenterPosX + m_rHalfScreenX;
	m_rMapCenterY	= - getY( &m_vMiniMapScale) * pInfo->nCenterPosY + m_rHalfScreenY;

	m_rScaleFactoX	= ( pInfo->nRealImageSize_OnBaking/m_rMapScale) * getX( &m_vMiniMapScale);
	m_rScaleFactoY	= ( pInfo->nRealImageSize_OnBaking/m_rMapScale) * getY( &m_vMiniMapScale);

	m_rMapWidth		= pInfo->nCenterPosX * 2.0f;
	m_rMapHeight	= pInfo->nCenterPosY * 2.0f;

	return true;
}

#define DEATH_UNIT_FADE_START	5.0f
#define DEATH_UNIT_FADE_TIME	2.0f
#define DEATH_UNIT_FADE_ALPHA	150

void	CBattleHUD_MiniMap::_UpdateDeathUnit( REAL32 rDeltaSeconds)
{
	INT32 i = 0;
	COLOR * pCol = nullptr;
	COLOR * pColOuter = nullptr;
	REAL32 fAlpha = 0.0f;

	for( i=0; i<UNIT_MAX_COUNT; i++)
	{
		pCol = m_pDeathUnitImage->GetColor( i);
		pColOuter = m_pDeathOuterUnitImage->GetColor( i);

		if( i3Color::GetA( pCol) == 255) m_bDeathUnitOnceUpdate[i] = false;

		if( i3Color::GetA( pCol) > i3Color::GetA( pColOuter))
		{
			i3Color::SetA( pCol, i3Color::GetA( pColOuter));
		}
		else
		{
			i3Color::SetA( pColOuter, i3Color::GetA( pCol));
		}

		if( m_pDeathUnitImage->GetEnable( i) || m_pDeathOuterUnitImage->GetEnable( i))
		{
			if( !m_bDeathUnitOnceUpdate[i])	m_rDeathUnitImageDeltaTime[i] += rDeltaSeconds;

			if( m_rDeathUnitImageDeltaTime[i] > DEATH_UNIT_FADE_START)
			{				
				fAlpha = (REAL32)i3Color::GetA( pCol);

				if( fAlpha != (REAL32)DEATH_UNIT_FADE_ALPHA)
				{
					fAlpha -= 255.0f * (rDeltaSeconds / DEATH_UNIT_FADE_TIME);

					if( fAlpha < (REAL32)DEATH_UNIT_FADE_ALPHA)
					{
						fAlpha = (REAL32)DEATH_UNIT_FADE_ALPHA;
						m_rDeathUnitImageDeltaTime[i] = 0.0f;
						m_bDeathUnitOnceUpdate[i] = true;
					}

					i3Color::SetA( pCol, (UINT8)fAlpha);

					if( m_pDeathOuterUnitImage->GetEnable( i))
					{
						i3Color::SetA( pColOuter, (UINT8)fAlpha);
					}
				}
			}
		}
		else
		{
			i3Color::SetA( pCol, 255);
			i3Color::SetA( pColOuter, 255);			
		}
	}
}

// 탈취모드 미니맵 오브젝트
bool CBattleHUD_MiniMap::_CreateUsurpationObject()
{
	m_pUsurpationObject = i3Sprite2D::new_object();

	CGameMissionManager* pMissionMgr = CGameMissionManager::i();
	VEC3D* _objPos = (VEC3D*)pMissionMgr->getMissionObjPos();

	const OBJECT_COUNT_INFO* _objectList = BattleObjContext::i()->getObjectCountInfo();
	i3::vector<i3Object*> _cleaverlist = _objectList->m_pControledObjectList;

	m_pUsurpationObject->SetTexture(m_pHUD3Texture);
	m_pUsurpationObject->Create(3, true, true);	// 0: 탈취물. 1,2: 중식칼

	// 0: 탈취물. 1~2: 중식칼
	for(INT32 i=0; i<MINIMAP_CLEAVERKNIFE_MAX+1; i++)
		m_pUsurpationObject->SetEnable(i, false);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) && 
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		// 수박 오브젝트 출력
		m_pUsurpationObject->SetEnable(0, true);
		m_pUsurpationObject->SetTextureCoord( 0, 704.0f, 344.0f, 722.0f, 368.0f);
		m_pUsurpationObject->SetColor( 0, 255, 255, 255, 255);

		// 수박
		m_pUsurpationObject->SetRect( 0,	m_rHalfScreenX - 10.0f  * getX( &m_vMiniMapScale) + getX( _objPos) * m_rScaleFactoX,
			m_rHalfScreenY - 10.0f  * getY( &m_vMiniMapScale)+ getY( _objPos) * m_rScaleFactoY,
			20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
	}
	else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
			 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		// 정규탈취 미션 오브젝트 출력
		m_pUsurpationObject->SetEnable( 0, true);
		m_pUsurpationObject->SetTextureCoord( 0, 775.0f, 344.0f, 807.0f, 368.0f);
		m_pUsurpationObject->SetColor( 0, 255, 255, 255, 255);

		// 수박
		m_pUsurpationObject->SetRect( 0,	m_rHalfScreenX - 16.0f  * getX( &m_vMiniMapScale) + getX( _objPos) * m_rScaleFactoX,
			m_rHalfScreenY - 16.0f  * getY( &m_vMiniMapScale)+ getY( _objPos) * m_rScaleFactoY,
			32.0f * getX( &m_vMiniMapScale), 32.0f * getY( &m_vMiniMapScale));
	}

	// 중식칼 INDEX 1~2
	m_pUsurpationObject->SetEnable(1, true);
	m_pUsurpationObject->SetTextureCoord(1, 728.0f, 344.0f, 759.0f, 368.0f);
	m_pUsurpationObject->SetColor( 1, 255, 255, 255, 255);
	m_pUsurpationObject->SetEnable(2, true);
	m_pUsurpationObject->SetTextureCoord( 2, 728.0f, 344.0f, 759.0f, 368.0f);
	m_pUsurpationObject->SetColor(2, 255, 255, 255, 255);

	m_pAttrRoot->AddChild( m_pUsurpationObject);

	for(int i=0 ;i < MINIMAP_CLEAVERKNIFE_MAX; i++)
	{
		if(m_pCleaverKnife[i] == nullptr)
			return true;

		m_pCleaverKnife[i] = (CGameObjectWeaponBox*)_cleaverlist[i];
	}
	// 중식칼
	for(int i=0; i<MINIMAP_CLEAVERKNIFE_MAX; i++)
	{	// m_pUsurpationObject의 IDX 0번은 탈취물(수박)임.
		m_pUsurpationObject->SetRect( i+1,   m_rHalfScreenX - 7.0f * getX( &m_vMiniMapScale) + getX( m_pCleaverKnife[i]->GetPos()) * m_rScaleFactoX,
			m_rHalfScreenY - 7.0f * getY( &m_vMiniMapScale) + getY( m_pCleaverKnife[i]->GetPos()) * m_rScaleFactoY,
			14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
	}



	return true;
}

bool	CBattleHUD_MiniMap::_CreateMissionObject(void)
{
	m_pMissionObject = i3Sprite2D::new_object();
	m_pMissionObject->SetTexture( m_pHUDTexture);

	m_pMissionObject->Create( 4, true, true);

	//VEC3D * vAPos = C4Context::i()->getBombAreaPos( BOMB_AREA_A);
	//VEC3D * vBPos = C4Context::i()->getBombAreaPos( BOMB_AREA_B);
	
	{
		/*m_pMissionObject->SetEnable(0, true);
		m_pMissionObject->SetTextureCoord( 0, 153.0f, 43.0f, 173.0f, 63.0f);
		m_pMissionObject->SetColor( 0, 255, 255, 255, 255);

		m_pMissionObject->SetEnable(1, true);
		m_pMissionObject->SetTextureCoord( 1, 175.0f, 42.0f, 195.0f, 62.0f);
		m_pMissionObject->SetColor( 1, 255, 255, 255, 255);

		m_pMissionObject->SetRect( 0,	m_rHalfScreenX - 10.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
										m_rHalfScreenY - 10.0f  * getY( &m_vMiniMapScale)+ getY( vAPos) * m_rScaleFactoY,
										20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));

		m_pMissionObject->SetRect( 1,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
										m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
										20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));*/

		for (INT32 i=0; i<4; i++)
			m_pMissionObject->SetEnable(i, false);

		BombInstallAreaProp* pPropA = C4Context::i()->FindAreaProp(BOMB_AREA_A);
		if (pPropA)
		{
			VEC3D * vAPos = &pPropA->pos;

			m_pMissionObject->SetEnable(0, true);
			m_pMissionObject->SetTextureCoord( 0, 153.0f, 43.0f, 173.0f, 63.0f);
			m_pMissionObject->SetColor( 0, 255, 255, 255, 255);

			m_pMissionObject->SetRect( 0,	m_rHalfScreenX - 10.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
				m_rHalfScreenY - 10.0f  * getY( &m_vMiniMapScale)+ getY( vAPos) * m_rScaleFactoY,
				20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));

			m_pMissionObject->SetEnable(2, true);
			m_pMissionObject->SetTextureCoord( 2, 197.0f, 44.0f, 211.0f, 57.0f);
			m_pMissionObject->SetColor( 2, 255, 255, 255, 255);

			m_pMissionObject->SetRect( 2,	m_rHalfScreenX - 7.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
				m_rHalfScreenY - 7.0f  * getY( &m_vMiniMapScale)+ getY( vAPos) * m_rScaleFactoY,
				14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
		}

		BombInstallAreaProp* pPropB = C4Context::i()->FindAreaProp(BOMB_AREA_B);
		if (pPropB)
		{
			VEC3D * vBPos = &pPropB->pos;

			m_pMissionObject->SetEnable(1, true);
			m_pMissionObject->SetTextureCoord( 1, 175.0f, 42.0f, 195.0f, 62.0f);
			m_pMissionObject->SetColor( 1, 255, 255, 255, 255);

			m_pMissionObject->SetRect( 1,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
				m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
				20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));

			m_pMissionObject->SetEnable(3, true);
			m_pMissionObject->SetTextureCoord( 3, 211, 43.0f, 225.0f, 57.0f);
			m_pMissionObject->SetColor( 3, 255, 255, 255, 255);

			m_pMissionObject->SetRect( 3,   m_rHalfScreenX - 7.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
				m_rHalfScreenY - 7.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
				14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
		}
	}

	/*{
		m_pMissionObject->SetEnable(2, true);
		m_pMissionObject->SetTextureCoord( 2, 197.0f, 44.0f, 211.0f, 57.0f);
		m_pMissionObject->SetColor( 2, 255, 255, 255, 255);

		m_pMissionObject->SetEnable(3, true);
		m_pMissionObject->SetTextureCoord( 3, 211, 43.0f, 225.0f, 57.0f);
		m_pMissionObject->SetColor( 3, 255, 255, 255, 255);

		m_pMissionObject->SetRect( 2,	m_rHalfScreenX - 7.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
										m_rHalfScreenY - 7.0f  * getY( &m_vMiniMapScale)+ getY( vAPos) * m_rScaleFactoY,
										14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));

		m_pMissionObject->SetRect( 3,   m_rHalfScreenX - 7.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
										m_rHalfScreenY - 7.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
										14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
	}*/

	// 폭파오브젝트 설치파동 - 임시 이미지를 사용합니다.
	{
		m_pMissionObjectWavRoot	= i3AttrSet::new_object();
		
		// Blend Enable = true;
		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
			pAttr->Set( true);
			m_pMissionObjectWavRoot->AddAttr( pAttr);
		}

		// Blend Mode( I3G_BLEND_SRCCOLOR, I3G_BLEND_INVSRCCOLOR)
		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_ONE);
			pAttr->SetBlendOperation( I3G_BLEND_OP_ADD);

			m_pMissionObjectWavRoot->AddAttr( pAttr);
		}


		m_pMissionObjectWave	= i3Sprite2D::new_object();
		m_pMissionObjectWave->SetTexture( m_pHUDTexture);
		m_pMissionObjectWave->Create( 2, true, true);

		m_pMissionObjectWave->SetEnable(0, true);
		m_pMissionObjectWave->SetTextureCoord(0, 115.0f, 52.0f, 150.0f, 87.0f);		
		m_pMissionObjectWave->SetColor( 0, 255, 255, 255, 255);

		m_pMissionObjectWave->SetEnable(1, true);
		m_pMissionObjectWave->SetTextureCoord(1, 115.0f, 52.0f, 150.0f, 87.0f);
		m_pMissionObjectWave->SetColor( 1, 255, 255, 255, 255);

		m_pMissionObjectWavRoot->AddChild( m_pMissionObjectWave);
	}

	m_pAttrRoot->AddChild( m_pMissionObjectWavRoot);
	m_pAttrRoot->AddChild( m_pMissionObject);

	return true;
}

bool	CBattleHUD_MiniMap::_CreateDestroyMissionObject(void)
{
	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY)) return false;

	m_pMissionObject = i3Sprite2D::new_object();
	m_pMissionObject->SetTexture( m_pHUDTexture);

	m_pMissionObject->Create( 4, true, true);

	CGameMissionManager * pMng = CGameMissionManager::i();  

	i3Object * pRedObject = pMng->getRedObject();
	I3ASSERT( pRedObject != nullptr);

	i3Object * pBlueObject = pMng->getBlueObject();
	I3ASSERT( pBlueObject != nullptr);

	VEC3D * vAPos = pRedObject->GetPos();
	VEC3D * vBPos = pBlueObject->GetPos();

	if( i3::same_of<CGameObjectRespawnHelicopter*>(pRedObject))
		vAPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pRedObject)->getRootDummy()->GetCacheMatrix());
	if( i3::same_of<CGameObjectRespawnHelicopter*>(pBlueObject))
		vBPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pBlueObject)->getRootDummy()->GetCacheMatrix());
	
	{
		m_pMissionObject->SetEnable(0, true);
		m_pMissionObject->SetTextureCoord( 0, 125.0f, 90.0f, 145.0f, 110.0f);
		m_pMissionObject->SetColor( 0, 255, 255, 255, 255);

		m_pMissionObject->SetEnable(1, true);
		m_pMissionObject->SetTextureCoord( 1, 102.0f, 90.0f, 122.0f, 110.0f);
		m_pMissionObject->SetColor( 1, 255, 255, 255, 255);

		m_pMissionObject->SetRect( 0,	m_rHalfScreenX - 10.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
										m_rHalfScreenY - 10.0f  * getY( &m_vMiniMapScale) + getY( vAPos) * m_rScaleFactoY,
										20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));


		m_pMissionObject->SetRect( 1,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
										m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
										20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
	}

	{
		m_pMissionObject->SetEnable(2, true);
		m_pMissionObject->SetTextureCoord( 2, 71.0f, 93.0f, 85.0f, 107.0f);
		m_pMissionObject->SetColor( 2, 255, 255, 255, 255);

		m_pMissionObject->SetEnable(3, true);
		m_pMissionObject->SetTextureCoord( 3, 86.0f, 93.0f, 100.0f, 107.0f);
		m_pMissionObject->SetColor( 3, 255, 255, 255, 255);

		m_pMissionObject->SetRect( 2,	m_rHalfScreenX - 7.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
										m_rHalfScreenY - 7.0f  * getY( &m_vMiniMapScale)+ getY( vAPos) * m_rScaleFactoY,
										14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));

		m_pMissionObject->SetRect( 3,   m_rHalfScreenX - 7.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
										m_rHalfScreenY - 7.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
										14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
	}

	// 폭파오브젝트 설치파동 - 임시 이미지를 사용합니다.
	{
		m_pMissionObjectWavRoot	= i3AttrSet::new_object();
		
		// Blend Enable = true;
		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
			pAttr->Set( true);
			m_pMissionObjectWavRoot->AddAttr( pAttr);
		}

		// Blend Mode( I3G_BLEND_SRCCOLOR, I3G_BLEND_INVSRCCOLOR)
		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_ONE);
			pAttr->SetBlendOperation( I3G_BLEND_OP_ADD);

			m_pMissionObjectWavRoot->AddAttr( pAttr);
		}


		m_pMissionObjectWave	= i3Sprite2D::new_object();
		m_pMissionObjectWave->SetTexture( m_pHUDTexture);
		m_pMissionObjectWave->Create( 2, true, true);

		m_pMissionObjectWave->SetEnable(0, false);
		m_pMissionObjectWave->SetTextureCoord(0, 115.0f, 52.0f, 149.0f, 85.0f);
		m_pMissionObjectWave->SetColor( 0, 255, 255, 255, 255);

		m_pMissionObjectWave->SetEnable(1, false);
		m_pMissionObjectWave->SetTextureCoord(1, 78.0f, 52.0f, 112.0f, 85.0f);		
		m_pMissionObjectWave->SetColor( 1, 255, 255, 255, 255);

		m_pMissionObjectWavRoot->AddChild( m_pMissionObjectWave);
	}

	m_pAttrRoot->AddChild( m_pMissionObjectWavRoot);
	m_pAttrRoot->AddChild( m_pMissionObject);

	return true;
}

bool	CBattleHUD_MiniMap::_CreateDefenceMissionObject( void)
{
	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEFENCE) ) return false;

	m_pMissionObject = i3Sprite2D::new_object();
	m_pMissionObject->SetTexture( m_pHUDTexture);

	m_pMissionObject->Create( 4, true, true);

	CGameMissionManager * pMng = CGameMissionManager::i(); 

	i3Object * pObject0 = pMng->getDefenceObj( 0);
	I3ASSERT( pObject0 != nullptr);

	i3Object * pObject1 = pMng->getDefenceObj( 1);
	I3ASSERT( pObject1 != nullptr);

	VEC3D * vAPos = pObject0->GetPos();
	VEC3D * vBPos = pObject1->GetPos();

	{
		m_pMissionObject->SetEnable(0, true);
		//m_pMissionObject->SetTextureCoord( 0, 125.0f, 90.0f, 145.0f, 110.0f);
		m_pMissionObject->SetTextureCoord( 0, 154.0f, 42.0f, 174.0f, 62.0f);
		m_pMissionObject->SetColor( 0, 255, 255, 255, 255);

		m_pMissionObject->SetEnable(1, true);
		m_pMissionObject->SetTextureCoord( 1, 176.0f, 42.0f, 196.0f, 62.0f);
		m_pMissionObject->SetColor( 1, 255, 255, 255, 255);

		m_pMissionObject->SetRect( 0,	m_rHalfScreenX - 10.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
										m_rHalfScreenY - 10.0f  * getY( &m_vMiniMapScale) + getY( vAPos) * m_rScaleFactoY,
										20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));


		m_pMissionObject->SetRect( 1,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
										m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
										20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
	}

	{
		m_pMissionObject->SetEnable(2, true);
		//m_pMissionObject->SetTextureCoord( 2, 71.0f, 93.0f, 85.0f, 107.0f);
		m_pMissionObject->SetTextureCoord( 2, 198.0f, 42.0f, 212.0f, 56.0f);
		m_pMissionObject->SetColor( 2, 255, 255, 255, 255);

		m_pMissionObject->SetEnable(3, true);
		m_pMissionObject->SetTextureCoord( 3, 212.0f, 42.0f, 224.0f, 56.0f);
		m_pMissionObject->SetColor( 3, 255, 255, 255, 255);

		m_pMissionObject->SetRect( 2,	m_rHalfScreenX - 7.0f  * getX( &m_vMiniMapScale) + getX( vAPos) * m_rScaleFactoX,
										m_rHalfScreenY - 7.0f  * getY( &m_vMiniMapScale)+ getY( vAPos) * m_rScaleFactoY,
										14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));

		m_pMissionObject->SetRect( 3,   m_rHalfScreenX - 7.0f * getX( &m_vMiniMapScale) + getX( vBPos) * m_rScaleFactoX,
										m_rHalfScreenY - 7.0f * getY( &m_vMiniMapScale) + getY( vBPos) * m_rScaleFactoY,
										14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
	}

	// 폭파오브젝트 설치파동 - 임시 이미지를 사용합니다.
	{
		m_pMissionObjectWavRoot	= i3AttrSet::new_object();
		
		// Blend Enable = true;
		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
			pAttr->Set( true);
			m_pMissionObjectWavRoot->AddAttr( pAttr);
		}

		// Blend Mode( I3G_BLEND_SRCCOLOR, I3G_BLEND_INVSRCCOLOR)
		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_ONE);
			pAttr->SetBlendOperation( I3G_BLEND_OP_ADD);

			m_pMissionObjectWavRoot->AddAttr( pAttr);
		}


		m_pMissionObjectWave	= i3Sprite2D::new_object();
		m_pMissionObjectWave->SetTexture( m_pHUDTexture);
		m_pMissionObjectWave->Create( 2, true, true);

		m_pMissionObjectWave->SetEnable(0, false);
		//m_pMissionObjectWave->SetTextureCoord(0, 78.0f, 52.0f, 112.0f, 85.0f);
		m_pMissionObjectWave->SetTextureCoord(0, 115.0f, 52.0f, 149.0f, 85.0f);
		m_pMissionObjectWave->SetColor( 0, 255, 255, 255, 255);

		m_pMissionObjectWave->SetEnable(1, false);
		//m_pMissionObjectWave->SetTextureCoord(1, 78.0f, 52.0f, 112.0f, 85.0f);
		m_pMissionObjectWave->SetTextureCoord(1, 115.0f, 52.0f, 149.0f, 85.0f);
		m_pMissionObjectWave->SetColor( 1, 255, 255, 255, 255);

		m_pMissionObjectWavRoot->AddChild( m_pMissionObjectWave);
	}

	m_pAttrRoot->AddChild( m_pMissionObjectWavRoot);
	m_pAttrRoot->AddChild( m_pMissionObject);

	return true;
}

bool	CBattleHUD_MiniMap::_InitMiniMapScaleButton( void)
{
	m_pMiniMapScaleButton = i3Sprite2D::new_object();
	m_pMiniMapScaleButton->SetTexture( m_pHUDTexture);

	m_pMiniMapScaleButton->Create( MINIMAP_SCALE_COUNT * 2, true, true);

	REAL32	rY = 165.0f;

	for( INT32 i = 0; i < MINIMAP_SCALE_COUNT * 2; ++i)
	{
		m_pMiniMapScaleButton->SetColor( i, 255, 255, 255, 255);
		m_pMiniMapScaleButton->SetEnable(i, true);

		m_pMiniMapScaleButton->SetRect( i,  128.0f, rY, 17.0f, 17.0f);
	}

	m_pMiniMapScaleButton->SetTextureCoord( 0, 274.0f, 67.0f, 291.0f, 84.0f);
	m_pMiniMapScaleButton->SetTextureCoord( 1, 274.0f, 88.0f, 291.0f, 105.0f);
	m_pMiniMapScaleButton->SetRect( 1,  146.5f, rY, 17.0f, 17.0f);

	m_pMiniMapScaleButton->SetTextureCoord( 2, 293.0f, 67.0f, 310.0f, 84.0f);
	m_pMiniMapScaleButton->SetTextureCoord( 3, 293.0f, 88.0f, 310.0f, 105.0f);
	m_pMiniMapScaleButton->SetRect( 3,  146.5f, rY, 17.0f, 17.0f);

	m_pMiniMapScaleButton->SetTextureCoord( 4, 312.0f, 67.0f, 329.0f, 84.0f);
	m_pMiniMapScaleButton->SetTextureCoord( 5, 312.0f, 88.0f, 329.0f, 105.0f);
	m_pMiniMapScaleButton->SetRect( 5,  146.5f, rY, 17.0f, 17.0f);

	return true;
}

void	CBattleHUD_MiniMap::_InitMiniMapLocationText(void)
{
	REAL32	rY = 169.0f;

	m_pCurLocationText = i3TextNodeDX2::new_object();
	m_pCurLocationText->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY);
	m_pCurLocationText->SetText(L"");
	m_pCurLocationText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pCurLocationText->setAutoWrapEnable( false);
	m_pCurLocationText->SetEnable(true);
	m_pCurLocationText->setPos(  9, (UINT32)rY);	
	m_pCurLocationText->setSize( 120, 17);
}

bool	CBattleHUD_MiniMap::_ResetCurrentMiniMapButton(bool bOrigin)
{
	if( m_pMiniMapScaleButton->GetEnable(0) != bOrigin)
	{
		m_pMiniMapScaleButton->SetEnable( 0, bOrigin);
	}

	if( m_pMiniMapScaleButton->GetEnable(1) != bOrigin)
	{
		m_pMiniMapScaleButton->SetEnable( 1, bOrigin);
	}

	for( INT32 i = 2; i < MINIMAP_SCALE_COUNT * 2; ++i)
	{
		if( m_pMiniMapScaleButton->GetEnable( i))
		{
			m_pMiniMapScaleButton->SetEnable( i, false);
		}
	}

	if( m_rMiniMapScale >= MINIMAP_SCALE_MAX)
	{
		if( bOrigin && !m_pMiniMapScaleButton->GetEnable(4))
		{
			m_pMiniMapScaleButton->SetEnable( 4, true);
		}
	}
	else if( m_rMiniMapScale <= MINIMAP_SCALE_MIN)
	{
		if( bOrigin && !m_pMiniMapScaleButton->GetEnable(5))
		{
			m_pMiniMapScaleButton->SetEnable( 5, true);
		}
	}

	return true;
}

bool	CBattleHUD_MiniMap::SetMiniMapMode( MINIMAP_MODE mode, bool force)
{
	if( m_nCurrentMode == mode && !force)
		return false;

	bool bEnableBKGnd = ( mode == MINIMAP_MODE_DEFAULT ) ? true : false;

	if( m_pSprBackGround->GetEnable(0) != bEnableBKGnd)
	{
		m_pSprBackGround->SetEnable( 0, bEnableBKGnd);
	}

	_ResetCurrentMiniMapButton( bEnableBKGnd);
	
	I3ASSERT(m_pViewport != nullptr);			// 어서트로 대체하고 널검사는 제거..(11.11.07.수빈)
//	if( m_pViewport != nullptr)
//	{
		if( mode == MINIMAP_MODE_DEFAULT )
		{
//			m_pViewport->SetViewportRect( 9, 9, 153, 153);
			m_bChangedViewport = true;			
		}
		else if( mode == MINIMAP_MODE_FULLSCREEN)
		{
			INT32 nStartX	= ( INT32)(i3Math::abs( (g_pViewer->GetViewHeight() - g_pViewer->GetViewWidth()) * 0.5f));
			INT32 nSize		= ( g_pViewer->GetViewHeight() < g_pViewer->GetViewWidth()) ? g_pViewer->GetViewHeight() : g_pViewer->GetViewWidth();

			m_pViewport->SetViewportRect( nStartX, 0, nSize, nSize);

			m_bChangedViewport = true;		
		}

		//뷰포트 사이즈가 바뀌었을때. 미니맵이 Enable상태면 이번프레임엔 꺼둔다.
		if( m_bChangedViewport == true )
		{
			i3Node * pNode = GetNode();
			if( pNode != nullptr && ( pNode->IsFlag( I3_NODEFLAG_DISABLE) == false))
			{
				pNode->AddFlag( I3_NODEFLAG_DISABLE);
			}
			else
			{
				m_bChangedViewport = false;
			}
		}
//	}

	m_nCurrentMode = mode;
	SetCurrentFloorIdx(m_idxFloor);
	_UpdateScale();

	if( mode == MINIMAP_MODE_DEFAULT)
	{
		_UpdateMiniMapMode( 0.0f);

		m_pCurLocationText->SetEnable( true);
	}
	else if( mode == MINIMAP_MODE_FULLSCREEN)
	{
		_UpdateFullScreenMode( 0.0f);

		m_pCurLocationText->SetEnable( false);
	}
	else
	{
		return false;
	}

	return true;
}

bool	CBattleHUD_MiniMap::CreateMiniMap( void)
{
	m_pHUDTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);

#if defined (MULTYLANGUAGE)
	i3::stack_string strLoadFilePath;
	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/HUD_03.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHUD3Texture	= g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
#else
#if defined(USE_LOCALE_UI)
	m_pHUD3Texture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud_03.i3i");
#else
	m_pHUD3Texture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/hud3.i3i");
#endif
#endif
		//crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);

	I3ASSERT( m_pHUDTexture != nullptr);
	I3ASSERT( m_pHUD3Texture != nullptr);

	_InitBGScene();
	_InitViewport();
	_InitStageImage();
	_CreateUnitTexture();

	if( m_pMapInfo != nullptr)
	{
		switch( MyRoomInfoContext::i()->getStage()->GetStageMode() )
		{
		case STAGE_MODE_BOMB :		_CreateMissionObject();			break;
		case STAGE_MODE_DESTROY :	_CreateDestroyMissionObject();	break;
		case STAGE_MODE_DEFENCE :	_CreateDefenceMissionObject();	break;
		case STAGE_MODE_CONVOY : 	_CreateUsurpationObject();		break;
		case STAGE_MODE_RUN_AWAY :									break;
		}
	}

	_CreateWeaponC5Wave();
	_InitMiniMapScaleButton();
	_InitMiniMapLocationText();

	SetMiniMapMode( MINIMAP_MODE_DEFAULT);

	return true;
}

void	CBattleHUD_MiniMap::SetEnableMiniMap( bool bEnable)
{
	i3Node * pNode = GetNode();
	if( pNode == nullptr)
		return;

	if( bEnable )
	{
		if( pNode->IsFlag( I3_NODEFLAG_DISABLE))
		{
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
		}

		SetEnable( true);
	}
	else
	{
		if( !pNode->IsFlag( I3_NODEFLAG_DISABLE))
		{
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		}

		SetEnable( false);
	}	
}

void	CBattleHUD_MiniMap::_UpdateMiniMapMode( REAL32 rDeltaSeconds)
{
	if( m_pMapInfo == nullptr)
		return;

	VEC3D vPos;
	MATRIX kMtx;
	REAL32 rTheta;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	// Observer Mode에서 문제가 될 수 있다.
	// 특정 캐릭터를 중심으로 Minimap을 표시해야하는가?
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		pMtx = g_pCamera->getCamMatrix();

		I3ASSERT( pMtx != nullptr);

		VEC3D v3Temp0;
		VEC3D v3Temp1;

		i3Vector::Set(&v3Temp0, 0.f, 0.f, 1.f);
		
		i3Vector::Set(&v3Temp1, getX( i3Matrix::GetAt( pMtx)), 0.f, getZ( i3Matrix::GetAt( pMtx)));
		i3Vector::Normalize(&v3Temp1, &v3Temp1);

		rTheta = i3Math::acos( i3Vector::Dot(&v3Temp0, &v3Temp1));
	}
	else
#endif
	{
		if( pChara != nullptr)
		{
			kMtx = *(MATRIX*)pChara->getSceneObject()->GetMatrix();
			rTheta = pChara->getBoneContext()->getTheta();
		}
		else
		{
			//LSR_AVATAR_SYSTEM
			rTheta = 0.0f;
		}
	}

	//LSR_AVATAR_SYSTEM
	i3Vector::Copy( &vPos, i3Matrix::GetPos( &kMtx));


	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		REAL32 rObjTheta = 0.f;
		i3Object * pObj = ((CGameCamera_OnObject*)g_pCamera)->getOnObject();
		if( pObj == nullptr)
			return;

		if( i3::same_of<CGameObjectRidingHelicopter*>(pObj))
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRidingHelicopter*)pObj)->getRootDummy()->GetCacheMatrix()));

			rObjTheta = i3Math::atan2( getX( i3Matrix::GetAt( ((CGameObjectRidingHelicopter*)pObj)->getRootDummy()->GetCacheMatrix())),
				getZ( i3Matrix::GetAt( ((CGameObjectRidingHelicopter*)pObj)->getRootDummy()->GetCacheMatrix())) );
		}
		else if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix()));

			rObjTheta = i3Math::atan2( getX( i3Matrix::GetAt( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix())),
				getZ( i3Matrix::GetAt( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix())) );
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "invalid object on weapon.");
			i3Vector::Zero( &vPos);
		}

		WeaponOnObject * pWeapon = ((CGameObjectRidingHelicopter*)pObj)->GetWeaponOnObject();
		rTheta = pWeapon->getTheta() - I3_PI + rObjTheta;
	}

	if( m_pSprMapImage != nullptr)
	{
		MATRIX mat;
		i3Matrix::SetRotateY( &mat, - rTheta);

		VEC3D	vTrans;
		if( !g_pEnvSet->m_bMiniMapMode)
		{
			i3Vector::TransformCoord( &vTrans, &vPos, &mat);
		}
		else
		{
			i3Vector::Copy( &vTrans, &vPos);
		}

		m_pSprMapImage->SetRect( 0, m_rMapCenterX + getX( &vTrans) * m_rScaleFactoX, 
									m_rMapCenterY + getZ( &vTrans) * m_rScaleFactoY,
									getX( &m_vMiniMapScale) * m_rMapWidth,
									getY( &m_vMiniMapScale) * m_rMapHeight);

		if( !g_pEnvSet->m_bMiniMapMode)
		{
			m_pSprMapImage->SetRotation( 0, - rTheta);
		}
		else
		{
			m_pSprMapImage->SetRotation( 0, 0.0f);
		}
	}

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe() == false)
#endif
	{
//		if( m_pUnitImage != nullptr)			// 널 체크 제거..(어차피 블록 외부에서 역참조 들어감)..( 11.11.07.수빈)
//		{
			m_pUnitImage->SetRect( 0,  m_rHalfScreenX - 8.5f * getX( &m_vMiniMapScale),
										m_rHalfScreenY - 17.25f * getY( &m_vMiniMapScale),
										17.0f * getX( &m_vMiniMapScale), 34.5f * getY( &m_vMiniMapScale));

			m_pUnitImage->SetColor( 0, 255, 255, 255, 255);

			if( !g_pEnvSet->m_bMiniMapMode)
			{
				m_pUnitImage->SetRotation( 0, 0.0f);
			}
			else
			{
				m_pUnitImage->SetRotation( 0, rTheta);
			}

			//생사 여부에 따른 색 변경
			bool bEnableUnit = ((pChara != nullptr) && !pChara->isCharaStateMask( CHARA_STATE_DEATH));
			COLOR col;

			if( bEnableUnit )
			{
				i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
			}
			else
			{
				i3Color::Set( &col, (UINT8) 160, 160, 160, 255);

				if( i3::same_of<CGameCamera_Kill*>(g_pCamera))
				{		
					CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;

					bEnableUnit = true;
					if( !pKillCam->isFPS())
					{
						bEnableUnit = false;
					}
				}
			}

			COLOR * pCol = m_pUnitImage->GetColor( 0);
			if( !i3Color::IsSame( pCol, &col))
			{
				m_pUnitImage->SetColor( 0, &col);
			}

			if( bEnableUnit != m_pUnitImage->GetEnable(0))
			{
				m_pUnitImage->SetEnable(0, bEnableUnit);
			}
//		}
	}

	_UpdateUnit( rTheta);
	_UpdateMissionObject( rTheta);
}

void	CBattleHUD_MiniMap::_UpdateFullScreenMode( REAL32 rDeltaSeconds)
{
	if( m_pMapInfo == nullptr)
		return;

	VEC3D vPos;
	MATRIX * pMtx = nullptr;
	REAL32 rTheta = 0.f;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if( pChara != nullptr)
	{
		pMtx = pChara->getSceneObject()->GetMatrix();
		rTheta = pChara->getBoneContext()->getTheta();
		i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));

	}
#if defined( DEF_OBSERVER_MODE)
	else
	{
		pMtx = g_pCamera->getCamMatrix();

		VEC3D v3Temp0;
		VEC3D v3Temp1;

		i3Vector::Set(&v3Temp0, 0.f, 0.f, 1.f);
		
		i3Vector::Set(&v3Temp1, getX( i3Matrix::GetAt( pMtx)), 0.f, getZ( i3Matrix::GetAt( pMtx)));
		i3Vector::Normalize(&v3Temp1, &v3Temp1);

		rTheta = i3Math::acos( i3Vector::Dot(&v3Temp0, &v3Temp1));

		i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));
	}
#else
	//LSR_AVATAR_SYSTEM
	//I3ASSERT( g_pPlayer != nullptr);
#endif
	
	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		i3Object * pObj = ((CGameCamera_OnObject*)g_pCamera)->getOnObject();
		if( i3::same_of<CGameObjectRidingHelicopter*>(pObj))
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRidingHelicopter*)pObj)->getRootDummy()->GetCacheMatrix()));
		}
		else if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix()));
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "invalid object on weapon.");
			i3Vector::Zero( &vPos);
		}

		WeaponOnObject * pWeapon = ((CGameObjectRidingHelicopter*)pObj)->GetWeaponOnObject();
		rTheta = pWeapon->getTheta() + I3_PI2;
	}


#ifdef UITEST_MINIMAP_MOVE
	if (GameContextUtil::UITest_IsMinimapMoveEnable())
	{
		i3InputKeyboard* key = g_pFramework->getKeyboard();

		if (key->GetPressState(I3I_KEY_I)) // 위쪽 이동
		{
			m_minimapMoveFactorY -= 2.5f;
		}
		else if (key->GetPressState(I3I_KEY_K)) // 아래쪽 이동
		{
			m_minimapMoveFactorY += 2.5f;
		}
		else if (key->GetPressState(I3I_KEY_J)) // 왼쪽 이동
		{
			m_minimapMoveFactorX -= 3.0f;
		}
		else if (key->GetPressState(I3I_KEY_L)) // 오른쪽 이동
		{
			m_minimapMoveFactorX += 3.0f;
		}
	}
#endif

	//맵 이미지
#ifdef UITEST_MINIMAP_MOVE
	if (GameContextUtil::UITest_IsMinimapMoveEnable())
	{
		m_pSprMapImage->SetRect( 0, m_rMapCenterX+m_minimapMoveFactorX, m_rMapCenterY+m_minimapMoveFactorY,	getX( &m_vMiniMapScale) * m_rMapWidth, getY( &m_vMiniMapScale) * m_rMapHeight);
		m_pSprMapImage->SetRotation( 0, 0.0f);
	}
	else
	{
		m_pSprMapImage->SetRect( 0, m_rMapCenterX, m_rMapCenterY,	getX( &m_vMiniMapScale) * m_rMapWidth, getY( &m_vMiniMapScale) * m_rMapHeight);
		m_pSprMapImage->SetRotation( 0, 0.0f);
	}
#else
	m_pSprMapImage->SetRect( 0, m_rMapCenterX, m_rMapCenterY,	getX( &m_vMiniMapScale) * m_rMapWidth, getY( &m_vMiniMapScale) * m_rMapHeight);
	m_pSprMapImage->SetRotation( 0, 0.0f);
#endif

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe() == false)
#endif
	{
//		if( m_pUnitImage != nullptr)			// 널 체크 제거..(어차피 블록 외부에서 역참조 들어감)..( 11.11.07.수빈)
//		{
			//pChara 이미지
			REAL32	x		= m_rHalfScreenX - 8.5f * getX( &m_vMiniMapScale) - getX( &vPos) * m_rScaleFactoX;
			REAL32	y		= m_rHalfScreenY - 17.25f * getY( &m_vMiniMapScale) - getZ( &vPos) * m_rScaleFactoY;
			REAL32	width	= 17.0f * getX( &m_vMiniMapScale);
			REAL32	height	= 34.5f * getY( &m_vMiniMapScale);

#ifdef UITEST_MINIMAP_MOVE
			if (GameContextUtil::UITest_IsMinimapMoveEnable())
				m_pUnitImage->SetRect( 0, x+m_minimapMoveFactorX, y+m_minimapMoveFactorY, width, height);
			else
				m_pUnitImage->SetRect( 0, x, y, width, height);
#else
			m_pUnitImage->SetRect( 0, x, y, width, height);
#endif

			m_pUnitImage->SetRotation( 0, rTheta);


			//생사 여부에 따른 색 변경
			bool bEnableUnit = ((pChara != nullptr) && !pChara->isCharaStateMask( CHARA_STATE_DEATH));

			COLOR col;
			if( bEnableUnit )
			{
				i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
			}
			else
			{
				i3Color::Set( &col, (UINT8) 255, 0, 0, 120);

				if( i3::same_of<CGameCamera_Kill*>(g_pCamera))
				{		
					CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;

					bEnableUnit = true;
					if( !pKillCam->isFPS())
					{
						bEnableUnit = false;
					}
				}
			}

			if( bEnableUnit != m_pUnitImage->GetEnable(0))
			{
				m_pUnitImage->SetEnable(0, bEnableUnit);
			}

			COLOR * pCol = m_pUnitImage->GetColor( 0);
			if( !i3Color::IsSame( pCol, &col))
			{
				m_pUnitImage->SetColor( 0, &col);
			}
//		}
	}

	_UpdateUnit( rTheta);	
	_UpdateMissionObject( rTheta);
}

void	CBattleHUD_MiniMap::_GetOuterLocation( VEC3D vIn, VEC2D * vOut, VEC2D	vImgSz)
{
	VEC3D	vNor;
	i3Vector::Normalize( &vNor, &vIn);

	if( (i3Math::abs( getX( &vNor)) / m_rHalfScreenX)  > (i3Math::abs( getZ( &vNor))/m_rHalfScreenY))
	{
		REAL32 real = m_rHalfScreenX / getX( &vNor);
		i3Vector::Scale(&vNor,  &vNor, i3Math::abs( real));
	}
	else
	{
		REAL32 real = m_rHalfScreenY / ( getZ( &vNor));// * (m_rHalfScreenY / m_rHalfScreenX));
		i3Vector::Scale( &vNor, &vNor, i3Math::abs( real));
	}
	
	REAL32	LastX = m_rHalfScreenX + getX( &vNor);
	REAL32	LastY = m_rHalfScreenY + getZ( &vNor);

	setX( vOut, MINMAX( getX( &vImgSz), LastX, m_rHalfScreenX * 2.0f - getX( &vImgSz)));
	setY( vOut, MINMAX( getY( &vImgSz), LastY, m_rHalfScreenY * 2.0f - getY( &vImgSz)));
}

void	CBattleHUD_MiniMap::_UpdateMissionObject( REAL32 rTheta)
{
	bool bCheckRedObjectHPZero = false;
	bool bCheckBlueObjectHPZero = false;

	CStageBattle * pStage = g_pFramework->GetStageBattle();
	if( pStage == nullptr)
		return;

	// 둘중 하나만 존재할수 있으며, 커버리티에서는 m_pMissionObject의 널값 유효성 문제를 제기함..
	if (m_pMissionObject == nullptr && m_pUsurpationObject == nullptr)
		return;
	
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if( pChara == nullptr)
		return;

	CGameMissionManager * pMng = CGameMissionManager::i(); 

	VEC3D * vAPos = nullptr;
	VEC3D * vBPos = nullptr;
	VEC3D* _objPos = nullptr;

	MATRIX kMtx;

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		pMtx = g_pCamera->getCamMatrix();
	else
#endif
	//LSR_AVATAR_SYSTEM
	kMtx = *pChara->GetMatrix();

	VEC3D *	pCharaPos = i3Matrix::GetPos(&kMtx);

	STAGE_MODE	StageType = MyRoomInfoContext::i()->getStage()->GetStageMode();

	switch( StageType )
	{
	case STAGE_MODE_DESTROY :
		{
			I3ASSERT_RETURN(m_pMissionObject);

			i3Object * pRedObject = pMng->getRedObject();
			I3ASSERT( pRedObject != nullptr);

			i3Object * pBlueObject = pMng->getBlueObject();
			I3ASSERT( pBlueObject != nullptr);

			vAPos = pRedObject->GetPos();
			vBPos = pBlueObject->GetPos();

			if( pRedObject->getCurrentHP() == 0.f)
			{
				if( m_pMissionObject->GetEnable( 0))		m_pMissionObject->SetEnable( 0, false);
				if( m_pMissionObject->GetEnable( 2))		m_pMissionObject->SetEnable( 2, false);

				bCheckRedObjectHPZero = true;
			}

			if( pBlueObject->getCurrentHP() == 0.f)
			{
				if( m_pMissionObject->GetEnable( 1))		m_pMissionObject->SetEnable( 1, false);
				if( m_pMissionObject->GetEnable( 3))		m_pMissionObject->SetEnable( 3, false);

				bCheckBlueObjectHPZero = true;
			}

			if( i3::same_of<CGameObjectRespawnHelicopter*>(pRedObject))
			{
				vAPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pRedObject)->getRootDummy()->GetCacheMatrix());
				
				//LSR_AVATAR_SYSTEM
				if(pChara != nullptr && (pChara->getAttachedObject() == pRedObject) )
					pCharaPos = vAPos;
			}

			if( i3::same_of<CGameObjectRespawnHelicopter*>(pBlueObject))
			{
				vBPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pBlueObject)->getRootDummy()->GetCacheMatrix());

				//LSR_AVATAR_SYSTEM
				if( pChara != nullptr &&(pChara->getAttachedObject() == pBlueObject) )
					pCharaPos = vBPos;
			}
		}
		break;
	case STAGE_MODE_DEFENCE :
		{
			I3ASSERT_RETURN(m_pMissionObject);

			i3Object * pDefObject0 = pMng->getDefenceObj( 0);
			I3ASSERT( pDefObject0 != nullptr);

			i3Object * pDefObject1 = pMng->getDefenceObj( 1);
			I3ASSERT( pDefObject1 != nullptr);

			vAPos = pDefObject0->GetPos();
			vBPos = pDefObject1->GetPos();

			if( pDefObject0->getCurrentHP() == 0.f)
			{
				if( m_pMissionObject->GetEnable( 0))		m_pMissionObject->SetEnable( 0, false);
				if( m_pMissionObject->GetEnable( 2))		m_pMissionObject->SetEnable( 2, false);

				bCheckRedObjectHPZero = true;
			}

			if( pDefObject1->getCurrentHP() == 0.f)
			{
				if( m_pMissionObject->GetEnable( 1))		m_pMissionObject->SetEnable( 1, false);
				if( m_pMissionObject->GetEnable( 3))		m_pMissionObject->SetEnable( 3, false);

				bCheckBlueObjectHPZero = true;
			}
		}
		break;
	case STAGE_MODE_BOMB :
		{
			//vAPos = C4Context::i()->getBombAreaPos( BOMB_AREA_A);
			BombInstallAreaProp* pPropA = C4Context::i()->FindAreaProp(BOMB_AREA_A);
			if (pPropA)
				vAPos = &pPropA->pos;

			if(	MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE &&
				(pMng->getMissionState() == GAME_MISSION_STATE_INSTALLED) && pMng->IsInstalledArea( 1) )
			{
				WeaponBase * pWeapon = nullptr;
#if defined( USE_EVENT_SHUTTLEX)
				pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex( WEAPON::SHUTTLEX));
#else
				pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
#endif

				if( pWeapon != nullptr)
					vBPos = i3Matrix::GetPos( pWeapon->getSceneObject()->GetCacheMatrix());
				else
				{
					//vBPos = C4Context::i()->getBombAreaPos( BOMB_AREA_B);
					BombInstallAreaProp* pPropB = C4Context::i()->FindAreaProp(BOMB_AREA_B);
					if (pPropB)
						vBPos = &pPropB->pos;
				}
			}
			else
			{
				//vBPos = C4Context::i()->getBombAreaPos( BOMB_AREA_B);
				BombInstallAreaProp* pPropB = C4Context::i()->FindAreaProp(BOMB_AREA_B);
				if (pPropB)
					vBPos = &pPropB->pos;
			}
		}
		break;

	case STAGE_MODE_CONVOY :
		{
			_objPos = (VEC3D*)pMng->getMissionObjPos();
			if(CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_ENDBATTLE
				&& (m_pCleaverKnife[0] != nullptr))
			{
				m_pCleaverKnife[0]->m_bCleaverKnifeUsed = false;
				m_pCleaverKnife[1]->m_bCleaverKnifeUsed = false;
			}

		}
		break;

	case STAGE_MODE_RUN_AWAY :
		break;
	default :	
		return;
		break;
	}

	if( m_nCurrentMode == MINIMAP_MODE_DEFAULT)
	{
		bool	bInsideMiniMap = true;
		MATRIX mat;
		VEC3D  vTrans;
		VEC3D  vCleaverTrans[2];

		// 탈취모드 미니맵UI:START
		if(StageType == STAGE_MODE_CONVOY)
		{
			if( (_objPos != nullptr) 
				&& (CGameMissionManager::i()->CheckCrownMinimap() == true))
			{
				I3ASSERT_RETURN(m_pUsurpationObject);

				VEC3D	vARelPos;
				i3Vector::Sub( &vARelPos, pCharaPos, _objPos);
				if( !g_pEnvSet->m_bMiniMapMode)
				{
					i3Matrix::SetRotateY( &mat, - rTheta);
					i3Vector::TransformCoord( &vTrans, &vARelPos, &mat);
				}
				else  
					i3Vector::Copy( &vTrans, &vARelPos);					

				REAL32	X = - 10.0f  * getX( &m_vMiniMapScale) + getX( &vTrans) * m_rScaleFactoX;
				REAL32	Y = - 10.0f  * getY( &m_vMiniMapScale) + getZ( &vTrans) * m_rScaleFactoY;
				bInsideMiniMap = ( i3Math::abs( X ) > m_rHalfScreenX || i3Math::abs( Y ) > m_rHalfScreenY) ? false : true;

				if( !bInsideMiniMap ) // 오브젝트가 미니맵에 없을 경우
				{
					if(m_pUsurpationObject->GetEnable(0)) 		m_pUsurpationObject->SetEnable(0, false);
				}
				else{
					if(!m_pUsurpationObject->GetEnable(0)) 		m_pUsurpationObject->SetEnable(0, true);
					
					m_pUsurpationObject->SetRect( 0,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( &vTrans) * m_rScaleFactoX,
							m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getZ( &vTrans) * m_rScaleFactoY,
							20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
				}
			}
			else
				if(m_pUsurpationObject->GetEnable(0) == true) 		
					m_pUsurpationObject->SetEnable(0, false);

			{ // 중식칼처리START

				VEC3D	vCleaverRelPos[MINIMAP_CLEAVERKNIFE_MAX];			
				for(int i=0; i<MINIMAP_CLEAVERKNIFE_MAX; i++) 
				{
					if(m_pCleaverKnife[i] == nullptr)
						continue;
					i3Vector::Sub( &vCleaverRelPos[i], pCharaPos, m_pCleaverKnife[i]->GetPos());
					if( !g_pEnvSet->m_bMiniMapMode)
					{
						i3Matrix::SetRotateY( &mat, - rTheta);
						i3Vector::TransformCoord( &vCleaverTrans[i], &vCleaverRelPos[i], &mat);
					}
					else
					{
						i3Vector::Copy( &vCleaverTrans[i], &vCleaverRelPos[i]);
					}

					REAL32	X = - 10.0f  * getX( &m_vMiniMapScale) + getX( &vCleaverTrans[i]) * m_rScaleFactoX;
					REAL32	Y = - 10.0f  * getY( &m_vMiniMapScale) + getZ( &vCleaverTrans[i]) * m_rScaleFactoY;

					bInsideMiniMap = ( i3Math::abs( X ) > m_rHalfScreenX || i3Math::abs( Y ) > m_rHalfScreenY) ? false : true;

					if( !bInsideMiniMap ) // 오브젝트가 미니맵에 없을 경우
					{	// m_pUsurpationObject 의 0번 인덱스는 탈취물이기 때문에 +1씩 더해줍니다.
						if(m_pUsurpationObject->GetEnable(i+1)) 		m_pUsurpationObject->SetEnable(i+1, false);
					}
					else{
						if(!m_pCleaverKnife[i]->m_bCleaverKnifeUsed)
						{
							// m_pUsurpationObject 의 0번 인덱스는 탈취물(수박 오브젝트)
							m_pUsurpationObject->SetEnable(i+1, true);
							m_pUsurpationObject->SetRect(i+1, m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( &vCleaverTrans[i]) * m_rScaleFactoX,
								m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getZ( &vCleaverTrans[i]) * m_rScaleFactoY,
								20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
						}
						else
							m_pUsurpationObject->SetEnable(i+1, false);

					}
				}
			}	// 중식칼처리END
			return; // 탈취모드의 경우, 이 이상 실행할 필요가 없음
		}// 탈취모드 미니맵UI:END

		if (vAPos)
		{
			I3ASSERT_RETURN(m_pMissionObject);

			VEC3D	vARelPos;
			i3Vector::Sub( &vARelPos, pCharaPos, vAPos);

			if( bCheckRedObjectHPZero == false)
			{
				if( !g_pEnvSet->m_bMiniMapMode)
				{
					i3Matrix::SetRotateY( &mat, - rTheta);

					i3Vector::TransformCoord( &vTrans, &vARelPos, &mat);
				}
				else
				{
					i3Vector::Copy( &vTrans, &vARelPos);
				}

				REAL32	X = - 10.0f  * getX( &m_vMiniMapScale) + getX( &vTrans) * m_rScaleFactoX;
				REAL32	Y = - 10.0f  * getY( &m_vMiniMapScale) + getZ( &vTrans) * m_rScaleFactoY;

				bInsideMiniMap = ( i3Math::abs( X ) > m_rHalfScreenX || i3Math::abs( Y ) > m_rHalfScreenY) ? false : true;

				if( !bInsideMiniMap)
				{
					if( !m_pMissionObject->GetEnable( 2))		m_pMissionObject->SetEnable( 2, true);
					if( m_pMissionObject->GetEnable( 0))		m_pMissionObject->SetEnable( 0, false);

					{
						VEC3D vTmp1;
						VEC2D vTmp2, vTmp3;

						i3Vector::Set( &vTmp1, X, 0.0f, Y);
						i3Vector::Set( &vTmp2, 5.5f * getX( &m_vMiniMapScale), 6.5f * getY( &m_vMiniMapScale));

						_GetOuterLocation( vTmp1, &vTmp3, vTmp2);

						m_pMissionObject->SetRect( 2, getX( &vTmp3) - getX( &vTmp2), getY( &vTmp3) - getY( &vTmp2), 14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
					}
				}
				else
				{
					if( !m_pMissionObject->GetEnable( 0))		m_pMissionObject->SetEnable( 0, true);
					if( m_pMissionObject->GetEnable( 2))		m_pMissionObject->SetEnable( 2, false);

					m_pMissionObject->SetRect( 0,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( &vTrans) * m_rScaleFactoX,
						m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getZ( &vTrans) * m_rScaleFactoY,
						20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
				}
			}
		}

		if (vBPos)
		{
			I3ASSERT_RETURN(m_pMissionObject);

			VEC3D	vBRelPos;
			i3Vector::Sub( &vBRelPos, pCharaPos, vBPos);

			if( bCheckBlueObjectHPZero == false)
			{
				if( !g_pEnvSet->m_bMiniMapMode)
				{
					i3Matrix::SetRotateY( &mat, - rTheta);
					i3Vector::TransformCoord( &vTrans, &vBRelPos, &mat);
				}
				else
				{
					i3Vector::Copy( &vTrans, &vBRelPos);
				}

				REAL32	X = getX( &vTrans) * m_rScaleFactoX;
				REAL32	Y = getZ( &vTrans) * m_rScaleFactoY;

				bInsideMiniMap = ( i3Math::abs( X ) > m_rHalfScreenX || i3Math::abs( Y ) > m_rHalfScreenY) ? false : true;

				if( !bInsideMiniMap)
				{
					if( !m_pMissionObject->GetEnable( 3))	m_pMissionObject->SetEnable( 3, true);
					if( m_pMissionObject->GetEnable( 1))	m_pMissionObject->SetEnable( 1, false);

					{
						VEC3D vTmp1;
						VEC2D vTmp2, vTmp3;

						i3Vector::Set( &vTmp1, X, 0.0f, Y);
						i3Vector::Set( &vTmp2, 5.5f * getX( &m_vMiniMapScale), 6.5f * getY( &m_vMiniMapScale));

						_GetOuterLocation( vTmp1, &vTmp3, vTmp2);

						m_pMissionObject->SetRect( 3, getX( &vTmp3) - getX( &vTmp2), getY( &vTmp3) - getY( &vTmp2), 14.0f * getX( &m_vMiniMapScale), 14.0f * getY( &m_vMiniMapScale));
					}
				}
				else
				{
					if( !m_pMissionObject->GetEnable( 1))	m_pMissionObject->SetEnable( 1, true);
					if( m_pMissionObject->GetEnable( 3))	m_pMissionObject->SetEnable( 3, false);

					m_pMissionObject->SetRect( 1, m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) + getX( &vTrans) * m_rScaleFactoX,
						m_rHalfScreenY - 10.0f * getY( &m_vMiniMapScale) + getZ( &vTrans) * m_rScaleFactoY,
						20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
				}
			}
		}
	}
	else if( m_nCurrentMode == MINIMAP_MODE_FULLSCREEN)
	{
		I3ASSERT_RETURN(m_pMissionObject);

		if (vAPos)
		{
			if( bCheckRedObjectHPZero == false)
			{
				if( !m_pMissionObject->GetEnable( 0))	m_pMissionObject->SetEnable( 0, true);
				m_pMissionObject->SetRect( 0,	m_rHalfScreenX - 10.0f  * getX( &m_vMiniMapScale) - getX( vAPos) * m_rScaleFactoX,
					m_rHalfScreenY - 10.0f  * getY( &m_vMiniMapScale) - getZ( vAPos) * m_rScaleFactoY,
					20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
			}
		}

		if (vBPos)
		{
			if( bCheckBlueObjectHPZero == false)
			{
				if( !m_pMissionObject->GetEnable( 1))	m_pMissionObject->SetEnable( 1, true);
				m_pMissionObject->SetRect( 1,   m_rHalfScreenX - 10.0f * getX( &m_vMiniMapScale) - getX( vBPos) * m_rScaleFactoX,
					m_rHalfScreenY - 10.5f * getY( &m_vMiniMapScale) - getZ( vBPos) * m_rScaleFactoY,
					20.0f * getX( &m_vMiniMapScale), 20.0f * getY( &m_vMiniMapScale));
			}
		}

		if( m_pMissionObject->GetEnable( 2))	m_pMissionObject->SetEnable( 2, false);
		if( m_pMissionObject->GetEnable( 3))	m_pMissionObject->SetEnable( 3, false);
	}

	CGameMissionManager * pMissionManager = CGameMissionManager::i(); 

	if( StageType == STAGE_MODE_BOMB)
	{
		I3ASSERT_RETURN(m_pMissionObject);

		// 공격 팀일 경우에만 설치 표시
		if( pMissionManager->IsInstalledArea( 0) &&
			((BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED) || pMissionManager->isOpenedLocationOfBomb()))
		{		
			m_pMissionObject->SetColor( 0, 255, 0, 0, 255);
			_SetWavEnable(0, true);
		}
		else
		{
			m_pMissionObject->SetColor( 0, 255, 255, 255, 255);
			_SetWavEnable(0, false);
		}


		// 공격 팀일 경우에만 설치 표시
		if( pMissionManager->IsInstalledArea( 1)&&
			//((0 == BattleSlotContext::i()->GetMySlotIdxEx() % 2) || pMissionManager->isOpenedLocationOfBomb()))
			((BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED) || pMissionManager->isOpenedLocationOfBomb()))
		{
			m_pMissionObject->SetColor( 1, 255, 0, 0, 255);
			_SetWavEnable(1, true);
		}
		else
		{
			m_pMissionObject->SetColor( 1, 255, 255, 255, 255);
			_SetWavEnable(1, false);
		}
		

		//층이 다른 미션 오브젝트는 표시하지 않음
		for( INT32 i = 0;i < MAX_BOMB_AREA_COUNT; ++i)
		{
			/*INT32 floorid = C4Context::i()->getBombFloorID( BOMB_AREA(i));
			if( floorid != m_idxFloor)
			{
				m_pMissionObjectWave->SetEnable( i, false);
				m_pMissionObject->SetEnable( i, false);
			}*/
			const BombInstallAreaProp* prop = C4Context::i()->FindAreaProp(static_cast<BOMB_AREA>(i));
			if (prop)
			{
				if (prop->floor != m_idxFloor)
				{
					m_pMissionObjectWave->SetEnable(i, false);
					m_pMissionObject->SetEnable(i, false);
				}
			}
			else
			{
				m_pMissionObjectWave->SetEnable(i, false);
				m_pMissionObject->SetEnable(i, false);
			}
		}
	}
}

void	CBattleHUD_MiniMap::SetDestroyObjectUnderAttack( UINT32 nidx, bool bEnable)
{
	m_DestroyObjectAttackState.SetUnderAttack(nidx, bEnable);
	_SetWavEnable(nidx, bEnable);
}

void	CBattleHUD_MiniMap::_UpdateUnit( REAL32 rTheta)
{
	INT32	nCnt	= 1;
	INT32	i		= 0;

	VEC3D	vPos;
	i3World * pWorld = m_pCurStage->getWorld();
	I3ASSERT( pWorld != nullptr);

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	i3MapSectorTable * pTable = pWorld->getMapSectorTable();
	
	INT32 myCharaIndex = g_pCharaManager->GetCharaIndex(pPlayer);

	for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
	{
		// 플레이어는 그리지 않습니다. 이전에 연두색으로 그렸기 때문입니다.
		if (i == myCharaIndex)
			continue;

		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if (pChara == nullptr || pChara->isReturnedInstance())
			continue;
		
		// 다른팀의 위치는 그리지 않습니다.
		if((pChara->getCharaTeam() != BattleSlotContext::i()->getMyTeam())) 
		{
			continue;
		}

		if( m_nCurrentMode == MINIMAP_MODE_DEFAULT)
		{
			MATRIX	mat;
			VEC3D	vTrans;

			//LSR_AVATAR_SYSTEM
			if( pPlayer != nullptr)
			{
				if( !g_pEnvSet->m_bMiniMapMode)
				{
					i3Matrix::SetRotateY( &mat, -rTheta);

					i3Vector::Sub( &vPos, pPlayer->GetPos(), pChara->GetPos());

					i3Vector::TransformCoord( &vTrans, &vPos, &mat);
				}
				else
				{
					i3Vector::Sub( &vTrans, pPlayer->GetPos(), pChara->GetPos());
				}
			}

			REAL32	X = getX( &vTrans) * m_rScaleFactoX;
			REAL32	Y = getZ( &vTrans) * m_rScaleFactoY;

			bool bInsideMiniMap = ( i3Math::abs( X ) > m_rHalfScreenX || i3Math::abs( Y ) > m_rHalfScreenY) ? false : true;

			if( bInsideMiniMap)
			{
				if( pChara->isVisible())
				{
					if( !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
					{
						if( !m_pUnitImage->GetEnable( nCnt))			m_pUnitImage->SetEnable( nCnt, true);
						if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
						if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
						if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);

						{// default
							m_pUnitImage->SetRect( nCnt, m_rHalfScreenX + getX( &vTrans) * m_rScaleFactoX - 5.0f * getX( &m_vMiniMapScale),
								m_rHalfScreenY + getZ( &vTrans) * m_rScaleFactoY - 11.0f * getY( &m_vMiniMapScale),
 								10.0f * getX( &m_vMiniMapScale), 22.0f * getY( &m_vMiniMapScale));
						}

						if( pChara->getBoneContext() != nullptr)
						{
							if( !g_pEnvSet->m_bMiniMapMode)
							{
								m_pUnitImage->SetRotation( nCnt, pChara->getBoneContext()->getTheta() - rTheta);
							}
							else
							{
								m_pUnitImage->SetRotation( nCnt, pChara->getBoneContext()->getTheta());
							}
						}
					}
					else
					{
						if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
						if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
						if( !m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, true);
						if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);

						m_pDeathUnitImage->SetRect( nCnt, m_rHalfScreenX + getX( &vTrans) * m_rScaleFactoX - 6.0f * getX( &m_vMiniMapScale),
							m_rHalfScreenY + getZ( &vTrans) * m_rScaleFactoY - 6.0f * getY( &m_vMiniMapScale),
							12.0f * getX( &m_vMiniMapScale), 12.0f * getY( &m_vMiniMapScale));
					}
				}
				else
				{
					if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
					if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
					if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
					if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);
				}
			}
			else
			{
				VEC3D vTmp1;
				VEC2D vTmp2, vTmp3;

				VEC3D	vNor;
				i3Vector::Set( &vNor, X, 0.0f, Y);
				i3Vector::Normalize( &vNor, &vNor);
				REAL32 theta = atan2( getX( &vNor), getZ( &vNor));

				if( pChara->isVisible())
				{
					if( !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
					{
						if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
						if( !m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, true);
						if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
						if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);

						i3Vector::Set( &vTmp1, X, 0.0f, Y);
						i3Vector::Set( &vTmp2, 5.0f * getX( &m_vMiniMapScale), 5.0f * getY( &m_vMiniMapScale));

						_GetOuterLocation( vTmp1, &vTmp3, vTmp2);

						m_pOuterUnitImage->SetRect( nCnt, getX( &vTmp3) - getX( &vTmp2), getY( &vTmp3) - getY( &vTmp2),	10.0f * getX( &m_vMiniMapScale), 10.0f * getY( &m_vMiniMapScale));
						m_pOuterUnitImage->SetRotation( nCnt, I3_DEG2RAD( 180.0f) + theta);
					}
					else
					{
						if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
						if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
						if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
						if( !m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, true);

						i3Vector::Set( &vTmp1, X, 0.0f, Y);
						i3Vector::Set( &vTmp2, 9.5f * getX( &m_vMiniMapScale), 9.5f * getY( &m_vMiniMapScale));

						_GetOuterLocation( vTmp1, &vTmp3, vTmp2);

						m_pDeathOuterUnitImage->SetRect( nCnt, getX( &vTmp3) - getX( &vTmp2), getY( &vTmp3) - getY( &vTmp2),	19.0f * getX( &m_vMiniMapScale), 19.0f * getY( &m_vMiniMapScale));
						m_pDeathOuterUnitImage->SetRotation( nCnt, I3_DEG2RAD( 180.0f) + theta);
					}
				}
				else
				{
					if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
					if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
					if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
					if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);
				}
			}
		}
		else if( m_nCurrentMode == MINIMAP_MODE_FULLSCREEN)
		{
			i3Vector::Copy( &vPos, pChara->GetPos());

			if( pChara->isVisible())
			{
				if( !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
				{
					if( !m_pUnitImage->GetEnable( nCnt))			m_pUnitImage->SetEnable( nCnt, true);
					if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
					if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
					if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);

					m_pUnitImage->SetRect( nCnt, m_rHalfScreenX - 5.0f * getX( &m_vMiniMapScale) - getX( &vPos) * m_rScaleFactoX,
						m_rHalfScreenY - 11.0f * getY( &m_vMiniMapScale) - getZ( &vPos) * m_rScaleFactoY,
						10.0f * getX( &m_vMiniMapScale), 22.0f * getY( &m_vMiniMapScale));

					m_pUnitImage->SetRotation( nCnt, pChara->getBoneContext()->getTheta());
				}
				else
				{
					if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
					if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
					if( !m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, true);
					if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);

					m_pDeathUnitImage->SetRect( nCnt, m_rHalfScreenX - 6.0f * getX( &m_vMiniMapScale) - getX( &vPos) * m_rScaleFactoX,
						m_rHalfScreenY - 6.0f * getY( &m_vMiniMapScale) - getZ( &vPos) * m_rScaleFactoY,
						12.0f * getX( &m_vMiniMapScale), 12.0f * getY( &m_vMiniMapScale));
				}
			}
			else
			{
				if( m_pUnitImage->GetEnable( nCnt))				m_pUnitImage->SetEnable( nCnt, false);
				if( m_pOuterUnitImage->GetEnable( nCnt))		m_pOuterUnitImage->SetEnable( nCnt, false);
				if( m_pDeathUnitImage->GetEnable( nCnt))		m_pDeathUnitImage->SetEnable( nCnt, false);
				if( m_pDeathOuterUnitImage->GetEnable( nCnt))	m_pDeathOuterUnitImage->SetEnable( nCnt, false);
			}
		}

		CGameCharaActionContext * pActionCtx = pChara->getActionContext();
		if (!pActionCtx) return;

		if( pActionCtx->isUpper( CHARA_UPPER_FIRE) ||
			pActionCtx->isUpper( CHARA_UPPER_SECONDARY_FIRE) ||
			pActionCtx->getUpperLeft() == CHARA_UPPER_FIRE )
		{
			m_pUnitImage->SetColor( nCnt, 255, 0, 0, 255);
		}
		else
		{
			m_pUnitImage->SetColor( nCnt, 255, 255, 255, 255);
		}

			
		//LSR_AVATAR_SYSTEM
		if( pTable != nullptr && (pChara->isLocal() == false) )
		{
			INT32 floor = pTable->getFloor( pChara->getPortalID());
			if( floor != m_idxFloor)
			{
				m_pUnitImage->SetEnable( nCnt, false);
				m_pOuterUnitImage->SetEnable( nCnt, false);
				m_pDeathUnitImage->SetEnable( nCnt, false);
				m_pDeathOuterUnitImage->SetEnable( nCnt, false);
			}
		}

		nCnt++;

	}

	for( i = nCnt; i < UNIT_MAX_COUNT; ++i)
	{
		if( m_pUnitImage->GetEnable( i))
		{
			m_pUnitImage->SetEnable( i, false);
		}

		if( m_pOuterUnitImage->GetEnable( i))
		{
			m_pOuterUnitImage->SetEnable( i, false);
		}

		if( m_pDeathUnitImage->GetEnable( i))
		{
			m_pDeathUnitImage->SetEnable( i, false);
		}

		if( m_pDeathOuterUnitImage->GetEnable( i))
		{
			m_pDeathOuterUnitImage->SetEnable( i, false);
		}
	}
}

#ifdef DOMI_DINO_MINIMAP_UNIT
void	CBattleHUD_MiniMap::_UpdateUnitDomiDino(REAL32 rTheta)
{
	VEC3D	vPos;
	CGameCharaBase* pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	for (INT32 i=0; i<MAX_COUNT_DOMI_DINO; i++)
	{
		CGameCharaBase* dino = g_pCharaManager->getCharaByNetIdx(i, true);
		if (dino == nullptr) continue;

		if (!dino->isVisible())
		{
			if( m_pOutlineUnitDomiDino->GetEnable( i)) m_pOutlineUnitDomiDino->SetEnable( i, false);
			if( m_pUnitDomiDino->GetEnable( i)) m_pUnitDomiDino->SetEnable( i, false);
			continue;
		}

		if (m_nCurrentMode == MINIMAP_MODE_DEFAULT)
		{
			MATRIX	mat;
			VEC3D	vTrans;

			if (pMyChara != nullptr)
			{
				if( !g_pEnvSet->m_bMiniMapMode)
				{
					i3Matrix::SetRotateY( &mat, -rTheta);

					i3Vector::Sub( &vPos, pMyChara->GetPos(), dino->GetPos());

					i3Vector::TransformCoord( &vTrans, &vPos, &mat);
				}
				else
				{
					i3Vector::Sub( &vTrans, pMyChara->GetPos(), dino->GetPos());
				}
			}

			REAL32	X = getX( &vTrans) * m_rScaleFactoX;
			REAL32	Y = getZ( &vTrans) * m_rScaleFactoY;

			bool bInsideMiniMap = ( i3Math::abs( X ) > m_rHalfScreenX || i3Math::abs( Y ) > m_rHalfScreenY) ? false : true;

			if( bInsideMiniMap)
			{
				if( !dino->isCharaStateMask( CHARA_STATE_DEATH) )
				{
					if( !m_pUnitDomiDino->GetEnable( i))			m_pUnitDomiDino->SetEnable( i, true);
					if( m_pOutlineUnitDomiDino->GetEnable( i))		m_pOutlineUnitDomiDino->SetEnable( i, false);

					m_pUnitDomiDino->SetRect( i, m_rHalfScreenX + getX( &vTrans) * m_rScaleFactoX - 5.0f * getX( &m_vMiniMapScale),
						m_rHalfScreenY + getZ( &vTrans) * m_rScaleFactoY - 5.0f * getY( &m_vMiniMapScale),
						10.0f * getX( &m_vMiniMapScale), 10.0f * getY( &m_vMiniMapScale));

					if( !g_pEnvSet->m_bMiniMapMode)
					{
						m_pUnitDomiDino->SetRotation( i, dino->getBoneContext()->getTheta() - rTheta);
					}
					else
					{
						m_pUnitDomiDino->SetRotation( i, dino->getBoneContext()->getTheta());
					}
				}
				else
				{
					if( m_pUnitDomiDino->GetEnable( i)) m_pUnitDomiDino->SetEnable( i, false);
				}
			} 
			else
			{
				VEC3D vTmp1;
				VEC2D vTmp2, vTmp3;

				VEC3D	vNor;
				i3Vector::Set( &vNor, X, 0.0f, Y);
				i3Vector::Normalize( &vNor, &vNor);
				REAL32 theta = atan2( getX( &vNor), getZ( &vNor));

				if( !dino->isCharaStateMask( CHARA_STATE_DEATH) )
				{
					if( !m_pOutlineUnitDomiDino->GetEnable( i))		m_pOutlineUnitDomiDino->SetEnable( i, true);
					if( m_pUnitDomiDino->GetEnable(i))				m_pUnitDomiDino->SetEnable( i, false);

					i3Vector::Set( &vTmp1, X, 0.0f, Y);
					i3Vector::Set( &vTmp2, 5.0f * getX( &m_vMiniMapScale), 5.0f * getY( &m_vMiniMapScale));

					_GetOuterLocation( vTmp1, &vTmp3, vTmp2);

					m_pOutlineUnitDomiDino->SetRect( i, getX( &vTmp3) - getX( &vTmp2), getY( &vTmp3) - getY( &vTmp2),	10.0f * getX( &m_vMiniMapScale), 10.0f * getY( &m_vMiniMapScale));
					m_pOutlineUnitDomiDino->SetRotation( i, I3_DEG2RAD( 180.0f) + theta);
				}
				else
				{
					if( m_pOutlineUnitDomiDino->GetEnable( i)) m_pOutlineUnitDomiDino->SetEnable( i, false);
				}
			}
		}
		else //full screen mode
		{
			i3Vector::Copy( &vPos, dino->GetPos());

			if( !dino->isCharaStateMask( CHARA_STATE_DEATH) )
			{
				if( !m_pUnitDomiDino->GetEnable( i))			m_pUnitDomiDino->SetEnable( i, true);
				if( m_pOutlineUnitDomiDino->GetEnable( i))		m_pOutlineUnitDomiDino->SetEnable( i, false);

				m_pUnitDomiDino->SetRect( i, m_rHalfScreenX - 5.0f * getX( &m_vMiniMapScale) - getX( &vPos) * m_rScaleFactoX,
					m_rHalfScreenY - 5.0f * getY( &m_vMiniMapScale) - getZ( &vPos) * m_rScaleFactoY,
					10.0f * getX( &m_vMiniMapScale), 10.0f * getY( &m_vMiniMapScale));

				m_pUnitDomiDino->SetRotation( i, dino->getBoneContext()->getTheta());
			}
			else
			{
				if( m_pUnitDomiDino->GetEnable( i))			m_pUnitDomiDino->SetEnable( i, false);
				if( m_pOutlineUnitDomiDino->GetEnable( i))		m_pOutlineUnitDomiDino->SetEnable( i, false);
			}
		}
	}
}
#endif

void	CBattleHUD_MiniMap::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate( rDeltaSeconds);

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

#if !defined( DEF_OBSERVER_MODE)
	if( pPlayer == nullptr)	return;
#endif

	//뷰포트 사이즈가 바뀌었을때. 미니맵이 Enable상태면 이번프레임엔 꺼둔다.
	if( m_bChangedViewport == true )
	{
		i3Node * pNode = GetNode();
		if( pNode != nullptr)
		{
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
		}

		m_bChangedViewport = false;
	}

	// 폭파 오브젝트 파동 업데이트
	_UpdateWav(rDeltaSeconds);

	//목표추적 강화 헤드기어 : 강화 헤드기어 사용자는 c5의 위치를 볼수 있다.
	//강화 헤드기어 사용자를 위한 C5 wave update
	//LSR_AVATAR_SYSTEM
	if( pPlayer != nullptr &&
#if defined( DEF_OBSERVER_MODE)
		BattleSlotContext::i()->isObserverMe() == false &&
#endif
		pPlayer->isEnableTargetChase() )
	{
		_UpdateWeaponC5Wave( rDeltaSeconds);
	}
	else
	{
		UINT32 total = 0;
		UINT32 itemCnt = WEAPON::getSettableItemCount();
		for( UINT32 i = 0; i < itemCnt; i++)
		{
			total += g_pWeaponManager->getThrowWeaponCount( WEAPON::getSettableItemIndex(i));
		}

		for( UINT32 i = 0; i < total; i++)
		{
			m_pWeaponC5WaveSprite->SetEnable( i, false);
		}
	}

	//
	_UpdateDeathUnit( rDeltaSeconds);

	if( m_nCurrentMode == MINIMAP_MODE_DEFAULT)
	{
		_UpdateMiniMapMode( rDeltaSeconds);
		_UpdateMiniMapButton( rDeltaSeconds);		
	}
	else if( m_nCurrentMode == MINIMAP_MODE_FULLSCREEN)
	{
		_UpdateFullScreenMode( rDeltaSeconds);
	}
	else
	{
		return;
	}	
}


void CBattleHUD_MiniMap::_SetWavEnable(UINT32 idxObject,bool bEnable)
{
	if( m_pMissionObjectWave->GetEnable(idxObject) != bEnable && bEnable)
	{
		m_rMissionObjectWavScale = 1.0f;
	}

	// ! 주의 - idx 오버플로우 검사 없음. 0, 1만 사용할것
	m_pMissionObjectWave->SetEnable(idxObject, bEnable);	
}

void CBattleHUD_MiniMap::_UpdateWav(REAL32 rDeltaSeconds)
{
	// 미션 아니면 처리하지 않는다
	if (false == m_pMissionObjectWave)	return;
	if( (m_pMissionObjectWave->GetEnable(0) == false) && (m_pMissionObjectWave->GetEnable(1) == false))	return;

	// 타이머 설정	
	static REAL32 rScaleTimer	= 0.0f;

	rScaleTimer = MIN(1.0f, rScaleTimer + rDeltaSeconds * m_rMissionObjectWavScale);

	// 이번 배율 계산
	static REAL32 rScaleMultiple = 5.0f;

	REAL32 sizeScale	= MAX(0.0f, rScaleMultiple * i3Math::sin(I3_PI2 * rScaleTimer));
	REAL32 alphaScale	= MAX(0.0f, i3Math::cos(I3_PI2 * rScaleTimer));


	// 타이머 리셋
	if (1.0f <= rScaleTimer)
	{
		rScaleTimer					=	0.0f;
		m_rMissionObjectWavScale	+=	0.01f;
	}

	VEC3D vPos;
	REAL32	rTheta = 0.f;

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if( pPlayer != nullptr)
	{
		i3Vector::Copy( &vPos, pPlayer->GetPos());
		rTheta = pPlayer->getBoneContext()->getTheta();
	}

	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		i3Object * pObj = ((CGameCamera_OnObject*)g_pCamera)->getOnObject();
		if( i3::same_of<CGameObjectRidingHelicopter*>(pObj))
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRidingHelicopter*)pObj)->getRootDummy()->GetCacheMatrix()));
		}
		else if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix()));
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "invalid object on weapon");
			i3Vector::Zero( &vPos);
		}

		WeaponOnObject * pWeapon = ((CGameObjectRidingHelicopter*)pObj)->GetWeaponOnObject();
		rTheta = pWeapon->getTheta() + I3_PI2;
	}

	// 위치 계산, 알파 계산
	{
		CGameMissionManager * pMng = CGameMissionManager::i(); 
	
		VEC3D * vAPos = nullptr;
		VEC3D * vBPos = nullptr;
		
		STAGE_MODE  StageType = MyRoomInfoContext::i()->getStage()->GetStageMode();
		switch( StageType )
		{
		case STAGE_MODE_DESTROY :
			{
				i3Object * pRedObject = pMng->getRedObject();
				I3ASSERT( pRedObject != nullptr);

				i3Object * pBlueObject = pMng->getBlueObject();
				I3ASSERT( pBlueObject != nullptr);

				vAPos = pRedObject->GetPos();
				vBPos = pBlueObject->GetPos();

				if( i3::same_of<CGameObjectRespawnHelicopter*>(pRedObject))
					vAPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pRedObject)->getRootDummy()->GetCacheMatrix());
				if( i3::same_of<CGameObjectRespawnHelicopter*>(pBlueObject))
					vBPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pBlueObject)->getRootDummy()->GetCacheMatrix());
			}
			break;
		case STAGE_MODE_BOMB :
			{
				BombInstallAreaProp* pPropA = C4Context::i()->FindAreaProp(BOMB_AREA_A);
				if (pPropA)
					vAPos = &pPropA->pos;

				//스톰 튜브맵은 폭탄 설치시 조금 예외처리가 필요한가 보다!
				if( MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE &&
					(pMng->getMissionState() == GAME_MISSION_STATE_INSTALLED) && pMng->IsInstalledArea( 1) )
				{
					WeaponBase * pWeapon = nullptr;
#if defined( USE_EVENT_SHUTTLEX)
					pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex( WEAPON::SHUTTLEX));
#else
					pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
#endif

					if( pWeapon != nullptr)
						vBPos = i3Matrix::GetPos( pWeapon->getSceneObject()->GetCacheMatrix());
					else
					{
						BombInstallAreaProp* pPropB = C4Context::i()->FindAreaProp(BOMB_AREA_B);
						if (pPropB)
							vBPos = &pPropB->pos;
					}
				}
				else
				{
					BombInstallAreaProp* pPropB = C4Context::i()->FindAreaProp(BOMB_AREA_B);
					if (pPropB)
						vBPos = &pPropB->pos;
				}
			}
			break;
		case STAGE_MODE_DEFENCE :
			{
				I3ASSERT( pMng->getDefenceObj( 0) != nullptr);
				I3ASSERT( pMng->getDefenceObj( 1) != nullptr);

				vAPos = pMng->getDefenceObj( 0)->GetPos();
				vBPos = pMng->getDefenceObj( 1)->GetPos();
			}
			break;
		default :
			return;
			break;
		}

		MATRIX mat;			
		i3Matrix::SetRotateY( &mat, - rTheta);

		VEC3D  vTrans;

		if (vAPos)
		{
			VEC3D	vARelPos;
			i3Vector::Sub( &vARelPos, &vPos, vAPos);

			if( m_pMissionObjectWave->GetEnable(0) == true)
			{
				if (MINIMAP_MODE_FULLSCREEN == m_nCurrentMode)
				{
					i3Vector::Copy(&vTrans, vAPos);
					i3Vector::Scale( &vTrans, &vTrans, - 1.0f);//기준이 틀려서 반대로 계산됩니다.
				}
				else
				{		
					if( !g_pEnvSet->m_bMiniMapMode)
					{
						i3Vector::TransformCoord( &vTrans, &vARelPos, &mat);
					}
					else
					{
						i3Vector::Copy(&vTrans, &vARelPos);
					}
				}

				// 위치
				m_pMissionObjectWave->SetCenter(0,	m_rHalfScreenX  + getX( &vTrans) * m_rScaleFactoX,
					m_rHalfScreenY + getZ( &vTrans) * m_rScaleFactoY,
					0.0f);

				// 크기, 알파 스케일
				m_pMissionObjectWave->SetSize(0, (35.0f * getX( &m_vMiniMapScale)) * sizeScale, (35.0f * getY( &m_vMiniMapScale)) * sizeScale);		
				m_pMissionObjectWave->SetColor(0, 255, 255, 255, (UINT8)(255 * alphaScale));
			}
		}

		if (vBPos)
		{
			VEC3D	vBRelPos;
			i3Vector::Sub( &vBRelPos, &vPos, vBPos);

			if( m_pMissionObjectWave->GetEnable(1) == true)
			{
				// B 지점
				if (MINIMAP_MODE_FULLSCREEN == m_nCurrentMode)
				{
					i3Vector::Copy(&vTrans, vBPos);
					i3Vector::Scale( &vTrans, &vTrans, - 1.0f);//기준이 틀려서 반대로 계산됩니다.
				}
				else
				{		
					if( !g_pEnvSet->m_bMiniMapMode)
					{
						i3Vector::TransformCoord( &vTrans, &vBRelPos, &mat);
					}
					else
					{
						i3Vector::Copy(&vTrans, &vBRelPos);
					}
				}		

				// 위치
				m_pMissionObjectWave->SetCenter(1,	m_rHalfScreenX + getX( &vTrans) * m_rScaleFactoX,
					m_rHalfScreenY + getZ( &vTrans) * m_rScaleFactoY,
					0.0f);

				// 크기, 알파 스케일
				m_pMissionObjectWave->SetSize(1, (35.0f * getX( &m_vMiniMapScale)) * sizeScale, (35.0f * getY( &m_vMiniMapScale)) * sizeScale);		
				m_pMissionObjectWave->SetColor(1, 255, 255, 255, (UINT8)(255 * alphaScale));
			}
		}
	}
}

bool CBattleHUD_MiniMap::_CreateWeaponC5Wave( void)
{
#if defined( ENABLE_C5WAVE)
	m_pWeaponC5WaveRoot	= i3AttrSet::new_object();
		
	// Blend Enable = true;
	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
		pAttr->Set( true);
		m_pWeaponC5WaveRoot->AddAttr( pAttr);
	}

	// Blend Mode( I3G_BLEND_SRCCOLOR, I3G_BLEND_INVSRCCOLOR)
	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_ONE);
		pAttr->SetBlendOperation( I3G_BLEND_OP_ADD);

		m_pWeaponC5WaveRoot->AddAttr( pAttr);
	}

	// 최대 사람수 16명 기준
	UINT32 Cnt = WEAPON::getSettableItemCount();
	if( Cnt > 0)
	{
		m_pWeaponC5WaveSprite	= i3Sprite2D::new_object();
		m_pWeaponC5WaveSprite->SetTexture( m_pHUDTexture);

		m_pWeaponC5WaveSprite->Create( Cnt * 32, true, true);

		for( UINT32 i = 0; i < Cnt * 32; i++)
		{
			m_pWeaponC5WaveSprite->SetEnable( i, false);
			m_pWeaponC5WaveSprite->SetTextureCoord( i, 78.0f, 52.0f, 112.0f, 87.0f);		
			m_pWeaponC5WaveSprite->SetColor( i, 255, 255, 255, 255);
		}
		
		m_pWeaponC5WaveRoot->AddChild( m_pWeaponC5WaveSprite);
	}

	m_pAttrRoot->AddChild( m_pWeaponC5WaveRoot);
#endif

	return true;
}

void CBattleHUD_MiniMap::_UpdateWeaponC5Wave( REAL32 rDeltaTime)
{
#if defined( ENABLE_C5WAVE )
	// 타이머 설정	
	m_rC5WaveScaleTimer = MIN(1.0f, m_rC5WaveScaleTimer + rDeltaTime );

	// 이번 배율 계산
	REAL32 sizeScale	= MAX(0.0f, m_rC5WaveScaleMultiple * i3Math::sin(I3_PI2 * m_rC5WaveScaleTimer));
	REAL32 alphaScale	= MAX(0.0f, i3Math::cos(I3_PI2 * m_rC5WaveScaleTimer));

	// 타이머 리셋
	if (1.0f <= m_rC5WaveScaleTimer)
	{
		m_rC5WaveScaleTimer					=	0.0f;
	}

	// 위치 계산, 알파 계산
	VEC3D vRelPos;
	VEC3D vTrans;
	MATRIX	mat;
	REAL32 rTheta = 0.f;
	
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	VEC3D * pCurPos = nullptr;
	I3ASSERT( pPlayer != nullptr)

	rTheta = pPlayer->getBoneContext()->getTheta();
	pCurPos = pPlayer->GetPos();
	
	//else
	//{	// Theta는 카메라로 계산을 해야한다.. 나중에...;;;;
	//	pCurPos = i3Matrix::GetPos( g_pCamera->getCamMatrix());
	//}

	i3Matrix::SetRotateY( &mat, -rTheta);

	INT32 SpriteIndex = 0;

	UINT32 Cnt = WEAPON::getSettableItemCount();
	for( UINT32 i = 0; i < Cnt; i++)
	{
		WEAPON::RES_ID resID = WEAPON::getSettableItemIndex(i);
		INT32 nCount = g_pWeaponManager->getThrowWeaponCount( resID);

		for( INT32 j = 0; j < nCount && (UINT32)SpriteIndex < (Cnt*16); j++, ++SpriteIndex)
		{
			WeaponC5 * pWeapon = (WeaponC5*) g_pWeaponManager->getThrowWeapon( resID, j);
			I3ASSERT( pWeapon != nullptr);

			MainWeapon_C5* mainWeapon = static_cast<MainWeapon_C5*>(pWeapon->GetMainWeapon());

			if( !mainWeapon->isBombState( WEAPON_BOMBSTATE_SETTLED) )
			{
				m_pWeaponC5WaveSprite->SetEnable( SpriteIndex, false);
				continue;
			}

			if( !pWeapon->isEnable())
				continue;
			
			i3Vector::Sub( &vRelPos, pCurPos, pWeapon->GetPos());
			if( i3Vector::Length( &vRelPos) > pPlayer->getTargetChaseRange())
			{
				m_pWeaponC5WaveSprite->SetEnable( SpriteIndex, false);
				continue;
			}
		
			if (MINIMAP_MODE_FULLSCREEN == m_nCurrentMode)
			{
				i3Vector::Copy( &vTrans, pWeapon->GetPos());
				i3Vector::Scale( &vTrans, &vTrans, - 1.0f);//기준이 틀려서 반대로 계산됩니다.
			}
			else
			{		
				if( !g_pEnvSet->m_bMiniMapMode)
				{
					i3Vector::TransformCoord( &vTrans, &vRelPos, &mat);
				}
				else
				{
					i3Vector::Copy( &vTrans, &vRelPos);
				}
			}

			// 위치
			m_pWeaponC5WaveSprite->SetCenter(SpriteIndex, m_rHalfScreenX + getX( &vTrans) * m_rScaleFactoX,
												m_rHalfScreenY + getZ( &vTrans) * m_rScaleFactoY,
												0.0f);

			// 크기, 알파 스케일
			m_pWeaponC5WaveSprite->SetSize( SpriteIndex, (20.0f * getX( &m_vMiniMapScale)) * sizeScale, (20.0f * getY( &m_vMiniMapScale)) * sizeScale);
			m_pWeaponC5WaveSprite->SetColor( SpriteIndex, 255, 255, 255, (UINT8)(255 * alphaScale));
			m_pWeaponC5WaveSprite->SetEnable( SpriteIndex, true);
		}
	}
#endif
}

void CBattleHUD_MiniMap::SetPosition(INT32 x, INT32 y)
{
	m_pViewport->setStartPosX(x);
	m_pViewport->setStartPosY(y);
}

void CBattleHUD_MiniMap::SetSize(INT32 w, INT32 h)
{
	m_pViewport->setWidth(w);
	m_pViewport->setHeight(h);
}


#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT

// 기획 문서 참조 : http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId=2269

DomiSkillObjOldMinimapUnitMgr::DomiSkillObjOldMinimapUnitMgr(CBattleHUD_MiniMap* minimap) 
: m_minimap(minimap) {}

DomiSkillObjOldMinimapUnitMgr::~DomiSkillObjOldMinimapUnitMgr() 
{
	for (size_t i=0; i<m_units.size(); i++)
	{
		I3_SAFE_DELETE(m_units[i]);
	}
	m_units.clear();
}

i3Sprite2D* DomiSkillObjOldMinimapUnitMgr::MakeSentrygunImage() 
{
	i3Sprite2D* image = i3Sprite2D::new_object();
	image->Create(IMAGE_SENTRY_DUMMY_MAX, true, true);

	i3Texture* pTex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
	image->SetTexture( pTex );	
	I3_SAFE_RELEASE(pTex);

	image->SetTextureCoord(IMAGE_SENTRY_DUMMY_DEACTIVATED, 653, 390, 673, 410);
	image->SetTextureCoord(IMAGE_SENTRY_DUMMY_ACTIVATED, 653, 342, 673, 362);
	image->SetTextureCoord(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING, 653, 366, 673, 386);

	for (INT32 i=0; i<IMAGE_SENTRY_DUMMY_MAX; i++)
	{
		image->SetColor(i, 255, 255, 255, 255);
		image->SetEnable(i, false);
		image->SetRect(i,  
			g_pViewer->GetViewWidth() * 0.5f - 10.0f * getX(&m_minimap->m_vMiniMapScale),
			g_pViewer->GetViewHeight() * 0.5f - 10.0f * getY(&m_minimap->m_vMiniMapScale),
			20.0f * getX( &m_minimap->m_vMiniMapScale), 
			20.0f * getY(&m_minimap->m_vMiniMapScale));
	}

	m_minimap->m_pAttrRoot->AddChild(image);

	return image;
}

i3Sprite2D* DomiSkillObjOldMinimapUnitMgr::MakeDummyImage() 
{
	i3Sprite2D* image = i3Sprite2D::new_object();
	image->Create(IMAGE_SENTRY_DUMMY_MAX, true, true);

	i3Texture* pTex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
	image->SetTexture( pTex );	
	I3_SAFE_RELEASE(pTex);

	image->SetTextureCoord(IMAGE_SENTRY_DUMMY_DEACTIVATED, 628, 390, 648, 410); // IMAGE_SENTRY_DUMMY_DEACTIVATED
	image->SetTextureCoord(IMAGE_SENTRY_DUMMY_ACTIVATED, 628, 342, 648, 362); // IMAGE_SENTRY_DUMMY_ACTIVATED
	image->SetTextureCoord(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING, 628, 366, 648, 386); // IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING

	for (INT32 i=0; i<IMAGE_SENTRY_DUMMY_MAX; i++)
	{
		image->SetColor(i, 255, 255, 255, 255);
		image->SetEnable(i, false);
		image->SetRect(i,  
			g_pViewer->GetViewWidth() * 0.5f - 10.0f * getX(&m_minimap->m_vMiniMapScale),
			g_pViewer->GetViewHeight() * 0.5f - 10.0f * getY(&m_minimap->m_vMiniMapScale),
			20.0f * getX( &m_minimap->m_vMiniMapScale), 
			20.0f * getY(&m_minimap->m_vMiniMapScale));
	}

	m_minimap->m_pAttrRoot->AddChild(image);

	return image;
}

i3Sprite2D* DomiSkillObjOldMinimapUnitMgr::MakeSkillShopImage() 
{
	i3Sprite2D* image = i3Sprite2D::new_object();
	image->Create(IMAGE_SKILLSHOP_MAX, true, true);

	i3Texture* pTex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);
	image->SetTexture( pTex );	
	I3_SAFE_RELEASE(pTex);

	image->SetTextureCoord(IMAGE_SKILLSHOP_DEACTIVATED, 603, 390, 623, 410); // IMAGE_SKILLSHOP_DEACTIVATED
	image->SetTextureCoord(IMAGE_SKILLSHOP_ACTIVATED, 603, 342, 623, 362); // IMAGE_SKILLSHOP_ACTIVATED

	for (INT32 i=0; i<IMAGE_SKILLSHOP_MAX; i++)
	{
		image->SetColor(i, 255, 255, 255, 255);
		image->SetEnable(i, false);
		image->SetRect(i,  
			g_pViewer->GetViewWidth() * 0.5f - 10.0f * getX(&m_minimap->m_vMiniMapScale),
			g_pViewer->GetViewHeight() * 0.5f - 10.0f * getY(&m_minimap->m_vMiniMapScale),
			20.0f * getX( &m_minimap->m_vMiniMapScale), 
			20.0f * getY(&m_minimap->m_vMiniMapScale));
	}

	m_minimap->m_pAttrRoot->AddChild(image);

	return image;
}

void DomiSkillObjOldMinimapUnitMgr::AddUnit(INT32 type, INT32 idx)
{
	Element* elem = new Element;
	MEMDUMP_NEW( elem, sizeof(Element));

	elem->type = type;
	elem->idx = idx;

	switch (type)
	{
	case DOMI_TYPE_SENTRYGUN:
		elem->image = MakeSentrygunImage();
		break;

	case DOMI_TYPE_DUMMY:
		elem->image = MakeDummyImage();
		break;

	case DOMI_TYPE_SKILLSHOP:
		elem->image = MakeSkillShopImage();
		break;
	}

	m_units.push_back(elem);
}

void DomiSkillObjOldMinimapUnitMgr::DeleteUnit(INT32 type, INT32 idx)
{
	for (size_t i=0; i<m_units.size(); i++)
	{
		Element* unit = m_units[i];

		if (unit->type == type && unit->idx == idx)
		{
		
			for (INT32 c=0; c<unit->image->GetCount(); c++)
				unit->image->SetEnable(c, false);
			
			I3_SAFE_RELEASE(unit->image);
			
			m_units.erase(m_units.begin()+i);
			I3_SAFE_DELETE( unit);

			break;
		}
	}
}

void DomiSkillObjOldMinimapUnitMgr::Update(REAL32 theta)
{
	INT32 type = 0;
	INT32 idx = 0;
	i3Sprite2D* image = 0;

	for (size_t i=0; i<m_units.size(); i++)
	{
		type = m_units[i]->type;
		idx = m_units[i]->idx;
		image = m_units[i]->image;

		dss::CIdentity* info = CDominationSkillSystem::i()->Find(type, idx);
		if (!info) continue;

		// 정상 / 수리 이미지 교체
		if (type == DOMI_TYPE_SENTRYGUN || type == DOMI_TYPE_DUMMY)
			UpdateSentrygunDummy(image, type, idx);

		// 활성화 / 비활성화 이미지 교체
		if (type == DOMI_TYPE_SKILLSHOP)
			UpdateSkillShop(image, type, idx);

		VEC3D pos = info->GetPos();
		SetPosition(theta, image, type, &pos);
	}
}

void DomiSkillObjOldMinimapUnitMgr::UpdateSentrygunDummy(i3Sprite2D* image, INT32 type, INT32 idx)
{
	dss::CIdentity* info = CDominationSkillSystem::i()->Find(type, idx);
	UINT32 currState = static_cast<DominationSkill::CServiceable*>(info)->GetState();

	switch (currState)
	{
	case DominationSkill::INSTALLABLE:
		{
			if (image->GetEnable(IMAGE_SENTRY_DUMMY_DEACTIVATED) == false)
				image->SetEnable(IMAGE_SENTRY_DUMMY_DEACTIVATED, true);

			if (image->GetEnable(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING) == true)
				image->SetEnable(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING, false);

			if (image->GetEnable(IMAGE_SENTRY_DUMMY_ACTIVATED) == true)
				image->SetEnable(IMAGE_SENTRY_DUMMY_ACTIVATED, false);
		}
		break;

	case DominationSkill::REPAIRABLE:
		{
			if (image->GetEnable(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING) == false)
				image->SetEnable(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING, true);

			if (image->GetEnable(IMAGE_SENTRY_DUMMY_DEACTIVATED) == true)
				image->SetEnable(IMAGE_SENTRY_DUMMY_DEACTIVATED, false);

			if (image->GetEnable(IMAGE_SENTRY_DUMMY_ACTIVATED) == true)
				image->SetEnable(IMAGE_SENTRY_DUMMY_ACTIVATED, false);
		}
		break;
	default:
		{
			if (image->GetEnable(IMAGE_SENTRY_DUMMY_ACTIVATED) == false)
				image->SetEnable(IMAGE_SENTRY_DUMMY_ACTIVATED, true);

			if (image->GetEnable(IMAGE_SENTRY_DUMMY_DEACTIVATED) == true)
				image->SetEnable(IMAGE_SENTRY_DUMMY_DEACTIVATED, false);

			if (image->GetEnable(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING) == true)
				image->SetEnable(IMAGE_SENTRY_DUMMY_REPAIR_REQUIRING, false);
		}
		break;
	}
}

void DomiSkillObjOldMinimapUnitMgr::UpdateSkillShop(i3Sprite2D* image, INT32 type, INT32 idx)
{
	dss::CIdentity* info = CDominationSkillSystem::i()->Find(type, idx);
	UINT32 currState = static_cast<DominationSkill::CServiceable*>(info)->GetState();

	if (currState == DominationSkill::SHOP_OPEN)
	{
		if (image->GetEnable(IMAGE_SKILLSHOP_ACTIVATED) == false)
			image->SetEnable(IMAGE_SKILLSHOP_ACTIVATED, true);

		if (image->GetEnable(IMAGE_SKILLSHOP_DEACTIVATED) == true)
			image->SetEnable(IMAGE_SKILLSHOP_DEACTIVATED, false);
	}
	else
	{			
		if (image->GetEnable(IMAGE_SKILLSHOP_DEACTIVATED) == false)
			image->SetEnable(IMAGE_SKILLSHOP_DEACTIVATED, true);

		if (image->GetEnable(IMAGE_SKILLSHOP_ACTIVATED) == true)
			image->SetEnable(IMAGE_SKILLSHOP_ACTIVATED, false);
	}
}

// CBattleHUD_MiniMap에 있는 소스 코드 그대로 옮겨와서 조금만 고침
void DomiSkillObjOldMinimapUnitMgr::SetPosition(REAL32 theta, i3Sprite2D* image, INT32 type, VEC3D* pos)
{
	VEC3D	vPos;
	CGameCharaBase* pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if (m_minimap->m_nCurrentMode == CBattleHUD_MiniMap::MINIMAP_MODE_DEFAULT)
	{
		MATRIX	mat;
		VEC3D	vTrans;
		
		if (pMyChara != nullptr)
		{
			if( !g_pEnvSet->m_bMiniMapMode)
			{
				i3Matrix::SetRotateY(&mat, -theta);

				i3Vector::Sub( &vPos, pMyChara->GetPos(), pos);

				i3Vector::TransformCoord( &vTrans, &vPos, &mat);
			}
			else
			{
				i3Vector::Sub( &vTrans, pMyChara->GetPos(), pos);
			}
		}

		REAL32	X = getX( &vTrans) * m_minimap->m_rScaleFactoX;
		REAL32	Y = getZ( &vTrans) * m_minimap->m_rScaleFactoY;

		bool bInsideMiniMap = ( i3Math::abs( X ) > m_minimap->m_rHalfScreenX || i3Math::abs( Y ) > m_minimap->m_rHalfScreenY) ? false : true;

		INT32 imageCount = 0;
		if (type == DOMI_TYPE_SENTRYGUN || type == DOMI_TYPE_DUMMY)
			imageCount = IMAGE_SENTRY_DUMMY_MAX;
		else if (type == DOMI_TYPE_SKILLSHOP)
			imageCount = IMAGE_SKILLSHOP_MAX;

		for (INT32 i=0; i<imageCount; i++)
		{
			if (bInsideMiniMap)
			{
				image->SetRect( i, m_minimap->m_rHalfScreenX + getX( &vTrans) * m_minimap->m_rScaleFactoX - 10.0f * getX( &m_minimap->m_vMiniMapScale),
					m_minimap->m_rHalfScreenY + getZ( &vTrans) * m_minimap->m_rScaleFactoY - 10.0f * getY( &m_minimap->m_vMiniMapScale),
					20.0f * getX( &m_minimap->m_vMiniMapScale), 20.0f * getY( &m_minimap->m_vMiniMapScale));
			}
			else
			{
				if (image->GetEnable(i) == true)
					image->SetEnable(i, false);
			}
		}
	}
}

#endif