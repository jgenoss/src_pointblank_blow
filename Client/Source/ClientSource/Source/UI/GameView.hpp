#include "pch.h"
#include "UIPopupGachaWeaponDetail.h"

#include "GameViewAct.hpp"

#include "UITopMenu_V15.h"
#include "UIItemInfoUtil.h"

#include "ConfigRenderBattle.h"
#include "BattleHUD_Scope.h"

#include "GameCharaUpdateContext.h"
#include "GameCharaActionContext.h"
#include "Character/GameCharaUpdater.h"

namespace gwd
{
	struct game_view : gwd::view //실제 1인칭 game view
	{
		typedef gwd::game_view mine;

	public:
		virtual void create(UIPopupGachaWeaponDetail* parent, i3UI3DViewBox* p3DView, T_ItemID item_id);
		virtual void release();
		virtual void entrance();
		virtual void exit();
		virtual void update(REAL32 rDeltaSeconds);
		virtual void revive();

	private:
		void init_2d_layer(i3AttrSet * p2DLayer);

	private:
		gwd::ActStateManager* m_pActStateMgr;
		//i3Camera* m_p3DViewCamera;	// i3UI3DViewBox의 카메라 포인터는 디바이스 리셋 과정을 거치면 무효화 되기 때문에 포인터를 별도로 들고 있으면 안됨.
		CGameCameraManager* m_pGameCameraMgr;
		CHUD_Scope* m_pScope;
		i3Node *m_pEffectParentNode;
		i3Node *		m_pViewRoot;
		Avatar* m_Avatar;

		// 미사용 변수 제거.
		//REAL32		m_rCameraTimer;
		//REAL32		m_rTargetFOV;
		//REAL32		m_rCurrentFOV;

	} _first_person_view;
}

//------------------------------------------------------------------------------//
//gwd::game_view
void gwd::game_view::create(UIPopupGachaWeaponDetail* parent, i3UI3DViewBox* p3DView, T_ItemID item_id)
{
	m_pParent = parent; m_p3DView = p3DView; m_item_id = item_id;

	//node
	m_pViewRoot = i3Node::new_object();

	i3AttrSet * p3DRoot = i3AttrSet::new_object_ref();
#if defined( I3_DEBUG)
	p3DRoot->SetName( "3D Root for Gacha");
#endif
	m_pViewRoot->AddChild( p3DRoot);

	//world
	{
		i3SceneFile worldFile;
		UINT32 resFile = worldFile.Load( "World/NPort/NPort_world.i3s" );
		I3ASSERT(resFile != STREAM_ERR);

		if (resFile != STREAM_ERR)
			p3DRoot->AddChild(  worldFile.GetSceneGraph());

		i3SceneFile SkyFile;
		resFile = SkyFile.Load( "World/NPort/NPort_Sky.i3s");
		I3ASSERT(resFile != STREAM_ERR);

		if (resFile != STREAM_ERR)
			p3DRoot->AddChild( SkyFile.GetSceneGraph());

		{
			i3ReflectMapBindAttr * pAttr = i3ReflectMapBindAttr::new_object_ref();
			i3ImageFile refFile;
			i3Texture * pTex = refFile.Load( "World/NPort/NPort_Ref.DDS");
			pAttr->SetTexture( pTex);
			p3DRoot->AddAttr( pAttr);
			I3_MUST_RELEASE( pTex);
		}

		{
			i3AmbientAttr * pAttr = i3AmbientAttr::new_object_ref();
			pAttr->Set( const_cast<COLORREAL*>(g_pFramework->getSgContext()->getAmbient()));
			p3DRoot->AddAttr( pAttr);
		}
	}

	//avater
	{
		NET_CHARA_INFO net_chara_info;
		net_chara_info.setCharaParts( CCharaInfoContext::i()->GetMainCharaPartsInfo() );
		//vv3 - check
		Avatar * pAvatar = g_pCharaManager->CreateAvatarPlayer( nullptr, 0, &net_chara_info, CHARA::TYPE_FIRST_VIEW_PLAYER, true, false);
		if( pAvatar != nullptr)
			m_Avatar = pAvatar;

		g_pCharaManager->SetLobbyAvatar( m_Avatar );

		I3ASSERT( m_Avatar != nullptr);

		m_Avatar->ResetChara();
		m_Avatar->Cmd_CreateWeapon( WEAPON_SLOT_PRIMARY, m_item_id); //avater에 무기 생성
		p3DRoot->AddChild( m_Avatar->getSceneObject());

		g_pCharaManager->SetTempAvatar(0, nullptr); //battle에서 생성하면 자동 호출 된다. 수동 생성이기 때문에 호출

		VEC3D vPos;
		i3Vector::Set( &vPos, -3.37f, 0.0f, -24.10f);
		m_Avatar->SetPos( &vPos);
	}

	//camera
	m_pGameCameraMgr = CGameCameraManager::new_object();
	tinst<CGameCameraManager>() = m_pGameCameraMgr;
	m_pGameCameraMgr->Create( m_pParent, m_Avatar);
	m_pGameCameraMgr->ChangeCamera(CAMERA_TYPE_FPS);

	//hud texture
	config_render_battle::i()->ProcessCreateStage();

	// effect
	m_pEffectParentNode = g_pEffectManager->GetNode()->GetParent();

	i3LayerRootNode * pRoot = g_pFramework->GetLayerRoot( "EffectRoot");
	i3LayerRootNode * pNewRoot = i3LayerRootNode::new_object_ref();
	pRoot->CopyTo( pNewRoot, I3_COPY_INSTANCE);

	g_pEffectManager->GetNode()->ChangeParent( pNewRoot);
	m_pViewRoot->AddChild( pNewRoot);

	//scope
	m_pScope = CHUD_Scope::new_object();
	i3AttrSet * p2DLayer = i3AttrSet::new_object_ref();
	init_2d_layer(p2DLayer);
	m_pViewRoot->AddChild( p2DLayer);
	m_pScope->CreateScopeHUD( p2DLayer);

	i3ParticleRenderNode * pParticleNode = i3ParticleRenderNode::new_object_ref();
	m_pViewRoot->AddChild( pParticleNode);

	//ActStateManager
	m_pActStateMgr = new ActStateManager;
	MEMDUMP_NEW( m_pActStateMgr, sizeof(ActStateManager) );

	WeaponBase * pWeapon = m_Avatar->GetCurrentCharaWeapon();
	if( pWeapon != nullptr)
		m_pActStateMgr->m_pWeaponBase = pWeapon->getUsedWeapon();
	else
		m_pActStateMgr->m_pWeaponBase = nullptr;
	m_pActStateMgr->m_Avatar = m_Avatar;
	//m_pActStateMgr->m_pScope = m_pScope;
}

void gwd::game_view::release()
{
	//ActStateManager
	I3_SAFE_DELETE( m_pActStateMgr);

	//effect
	g_pEffectManager->GetNode()->ChangeParent( m_pEffectParentNode);

	//camera
	m_pGameCameraMgr->setTargetCharacter(nullptr);
	I3_SAFE_RELEASE(m_pGameCameraMgr);
	UITopMenu_V15* Temp_TopMenu = g_pFramework->GetUIMainframe()->GetTopMenu_V15();
	if(Temp_TopMenu != nullptr ) 
		Temp_TopMenu->SetOriginalCamera();

	//scope
	I3_SAFE_RELEASE( m_pScope);

	//hud texture
	config_render_battle::i()->ProcessDestroyStage();

	//node
	I3_SAFE_RELEASE(m_pViewRoot);
}


void gwd::game_view::entrance()
{
	//캐릭터 설정
	m_Avatar->SetEnable( true);
	m_Avatar->Cmd_Idle();

	//3d view 설정
	m_p3DView->Create3DView();
	m_p3DView->Set3DView( m_pViewRoot );
	m_p3DView->OnVisibleScene();

	//3d view camera 설정
	//m_p3DViewCamera = m_p3DView->get3DCamera();
	m_p3DView->get3DCamera()->SetMatrix( g_pCamera->getCamMatrix() );
	VEC2D * pSize = m_p3DView->GetSprite()->GetSize(0);

	REAL32 w = getY( pSize) * 4 / 3;
	REAL32 h = getY( pSize);

	m_p3DView->get3DCamera()->SetPerspective( 
		I3_DEG2RAD( 70.0f), w, h,
		g_pCamera->GetCameraNear(), g_pCamera->GetCameraFar() );

	//m_rCurrentFOV = m_rTargetFOV = I3_DEG2RAD(g_pCamera->GetCameraFOV());

	//무기 행동 manager 설정
	m_pActStateMgr->m_pWeaponBase->setTotalBulletCount( m_pActStateMgr->m_pWeaponBase->getWeaponInfo()->GetMaxBullet() );

	//무기 타입 별로 시작 행동 설정
	switch( item_def::get_weapon_class_type(m_item_id) )
	{
	case WEAPON_SUBCLASS_SNIPER :
	case WEAPON_CLASS_SNIPER: m_pActStateMgr->start_action(gwd::at_sniper_scope_show); break;
	case WEAPON_SUBCLASS_SHOTGUN:
	case WEAPON_CLASS_SHOTGUN: m_pActStateMgr->start_action(gwd::at_shotgun_shot); break;
	case WEAPON_CLASS_KNIFE: m_pActStateMgr->start_action(gwd::at_melee_shot); break;
	default: m_pActStateMgr->start_action(gwd::at_normal_shot); break;
	}
}

void gwd::game_view::exit()
{
	m_p3DView->ClearView();
	m_p3DView->SetVisible(false);

	m_pScope->SetEnableSinperScope( false);
}

void gwd::game_view::update(REAL32 rDeltaSeconds)
{
	m_Avatar->OnUpdate( rDeltaSeconds);
	m_Avatar->getUpdateContext()->getUpdater()->OnFirstUpdate(rDeltaSeconds);

	m_pScope->UpdateSniperScope( rDeltaSeconds);

	// updated camera
	{
		g_pCamera->OnUpdate(rDeltaSeconds);
		m_p3DView->get3DCamera()->SetMatrix( g_pCamera->getCamMatrix() );
		m_p3DView->get3DCamera()->SetFOV( g_pCamera->getCameraObject()->GetFOV());
	}

	m_pActStateMgr->update(rDeltaSeconds);
}

void gwd::game_view::revive()
{
	exit();
	entrance();
}

//------------------------------------------------------------------------------//
void gwd::game_view::init_2d_layer(i3AttrSet * p2DLayer) 
{
	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
		pAttr->Set( true);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set( true);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set( true);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
		pAttr->Set( I3G_FACE_CULL_NONE);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set( false);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
		pAttr->Set( true);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();
		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
		pAttr->Set( true);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3ProjectMatrixAttr * pAttr = i3ProjectMatrixAttr::new_object_ref();
		MATRIX mtx;
		
		pAttr->SetMatrix( &mtx);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3ViewMatrixAttr * pAttr = i3ViewMatrixAttr::new_object_ref();
		MATRIX mtx;
		
		pAttr->SetMatrix( &mtx);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3TextureFunctionAttr * pAttr = i3TextureFunctionAttr::new_object_ref();
		pAttr->SetFunction( I3G_TEXTURE_FUNC_MODULATE);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_POINT);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_POINT);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3TextureWrapAttr * pAttr = i3TextureWrapAttr::new_object_ref();

		pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_CLAMP);
		pAttr->SetVertWrap( I3G_TEXTURE_WRAP_CLAMP);
		p2DLayer->AddAttr( pAttr);
	}

	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
		COLOR col;
		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
		pAttr->Set( &col);
		p2DLayer->AddAttr( pAttr);
	}
}