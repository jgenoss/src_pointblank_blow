#include "pch.h"
#include "UICharaViewer1PV.h"

#include "../UI/UIMainFrame.h"
#include "../UI/UIPopupStageViewer.h"
#include "UserInfoContext.h"
#include "GameContextUtil.h"

I3_CLASS_INSTANCE( UICharaViewer1PV);//, i3GameNode);

static i3Light * s_pLight = NULL;

UICharaViewer1PV::UICharaViewer1PV()
{
	m_p3DView = NULL;
	m_pLight = NULL;
	m_rCharaChangePoseTime = 0.f;
	m_LightTheta = 0.f;
	m_LightRho = 0.f;
	m_LightDist = 0.f;
	m_timeLocal = 0.f;

	m_viewCharaInfo._charaInfo.SetType( CHARA::TYPE_PLAYER);
	m_viewCharaInfo._charaInfo.SetHP(100.f);
	m_pCharacter = NULL;
}

UICharaViewer1PV::~UICharaViewer1PV()
{
	_ReleaseLight();
}

/*virtual*/ void UICharaViewer1PV::OnCreate( i3GameNode * pParent)
{

}

void UICharaViewer1PV::_AddLight( void)
{
	VEC3D d_rot;
	COLORREAL d_diffuse, d_specular;

	if( s_pLight == NULL)
	{
		s_pLight = i3Light::new_object();
		s_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3Color::Set( &d_diffuse, 1.2f, 1.2f, 1.2f, 1.0f);
		i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);
		i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);

		s_pLight->SetDiffuse( &d_diffuse);
		s_pLight->SetSpecular( &d_specular);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		//I3TRACE( "%f, %f\n", m_LightTheta, m_LightRho);

		s_pLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( s_pLight);
	}
	else
		I3_SAFE_ADDREF( s_pLight);

	g_pFramework->setShadowQuality( 0);
}

void UICharaViewer1PV::_ReleaseLight( void)
{
	if( g_pFramework && s_pLight)
	{
		s_pLight->Release();

		if( s_pLight->GetRefCount() == 1)
		{
			g_pFramework->removeLight( s_pLight);
			s_pLight = NULL;
		}
	}
}

void UICharaViewer1PV::ReleaseRoulette( void)
{
	_ReleaseLight();
}

void UICharaViewer1PV::Create3DViewBox( i3UIControl * pParent)
{
	I3ASSERT( pParent != NULL);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);

	m_FPSCamera = CGameCamera_FPS::new_object();
}

void UICharaViewer1PV::Delete3DViewBox( void)
{
	if( m_p3DView != NULL)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}

void UICharaViewer1PV::CreateViewStage()
{
	/*VEC3D vPos;
	m_pRoulette = RouletteObject::new_object();
	
	m_pRoulette->Create(g_pFramework,NULL);
	m_pRoulette->OnBindResource();

	I3_GAMENODE_ADDCHILD( g_pFramework->GetUIMainframe(), m_pRoulette );
*/
	m_p3DView->Create3DView();
	//m_p3DView->Set3DView( m_pRoulette->getSceneObject());
	_AddLight();

	m_p3DView->OnVisibleScene();


	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.17f, 0.92f, 90.0f, 1.0f);


	 m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 41.5f));
		 //	i3Matrix::PostRotateYDeg( pCamMtx, -5.0f);

	
	//m_pRoulette->SetMatrix( &I3_IDENTITYMATRIX);
		
	//i3Vector::Set( &vPos, 0.f, 0.0f, -7.0f);
	//m_pRoulette->SetPos( &vPos);

	
}



void UICharaViewer1PV::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);
	if(m_p3DView != NULL)
		m_p3DView->OnUpdate(rDeltaSeconds);
}

bool UICharaViewer1PV::OnKeyInput( i3InputDeviceManager * pInputMng)
{
/*
	i3InputKeyboard * pKey = pInputMng->GetKeyboard();

	if( m_p3DView)
	{

		MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();

		if( pKey->GetStrokeState(I3I_KEY_LEFT))
		{	

			i3Matrix::PostRotateYDeg( pCamMtx, -5.f);
			I3TRACE( "CamRotLeft \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;
		}
		else if( pKey->GetStrokeState( I3I_KEY_RIGHT ))
		{
			i3Matrix::PostRotateYDeg( pCamMtx, 5.f);
			I3TRACE( "CamRotRight \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;

		}

		else if( pKey->GetStrokeState( I3I_KEY_DOWN ))
		{

			i3Matrix::PostTranslate(pCamMtx,0.f,0.0f,5.f);
			//	i3Matrix::PostTranslate(pCamMtx,5.f);
			//	i3Matrix::PostRotateYDeg( pCamMtx, 5.f);
			I3TRACE( "CamRotRight \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;

		}

		else if( pKey->GetStrokeState( I3I_KEY_UP ))
		{

			i3Matrix::PostTranslate(pCamMtx,0.f,0.0f,-5.f);

			//			i3Matrix::PostRotateYDeg( pCamMtx, 5.f);
			I3TRACE( "CamRotRight \n");
			i3Vector::Dump( i3Matrix::GetAt( pCamMtx));
			return true;

		}

	}
	*/

	return false;

}

bool UICharaViewer1PV::UpdateCharacter( INT32 idx, bool bFirstUpdate, WEAPON_SLOT_TYPE mustUpdateSlot)
{
	// 초도보급 전이라면, 업데이트 하지 않는다
	if( UserInfoContext::i()->HaveNickName() == false)
		return false;

	NET_CHARA_INFO * pNetInfo = (NET_CHARA_INFO*) UserInfoContext::i()->GetMyCharaInfo();;
	m_viewCharaInfo._charaInfo.SetNetIdx(0);

	// 예외 처리
	// 모든 무기가 설정이 안되어 있으면 기본 무기로 설정
#ifdef IMPROVEMENT_MULTI_SLOT
	if( CHARA::WEAPON::IsZero( pNetInfo->GetCharaWeapon( WEAPON_SLOT_PRIMARY)))
	{
		pNetInfo->GetCharaWeapon( WEAPON_SLOT_PRIMARY)->m_TItemID = DEFAULT_WEAPON_ASSAULT;
		pNetInfo->GetCharaWeapon( WEAPON_SLOT_SECONDARY)->m_TItemID = DEFAULT_WEAPON_HANDGUN;
		pNetInfo->GetCharaWeapon( WEAPON_SLOT_MELEE)->m_TItemID = DEFAULT_WEAPON_KNIFE;
		pNetInfo->GetCharaWeapon( WEAPON_SLOT_THROWING1)->m_TItemID = DEFAULT_WEAPON_THROWING1;
		pNetInfo->GetCharaWeapon( WEAPON_SLOT_THROWING2)->m_TItemID = DEFAULT_WEAPON_THROWING2;
	}

	ITEM_INFO* pParts = pNetInfo->GetCharaPart( EQUIPMENT_SLOT_PRIMARY);
	_RecreateSlotChara( pParts);
	_CheckEquip( pParts);
#else
	if (0 == pNetInfo->getWeaponItemID( WEAPON_SLOT_PRIMARY) && 0 == pNetInfo->getWeaponItemID( WEAPON_SLOT_SECONDARY) &&
		0 == pNetInfo->getWeaponItemID( WEAPON_SLOT_MELEE) &&	0 == pNetInfo->getWeaponItemID( WEAPON_SLOT_THROWING1) &&
		0 == pNetInfo->getWeaponItemID( WEAPON_SLOT_THROWING2) )
	{
		pNetInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_ASSAULT);
		pNetInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, DEFAULT_WEAPON_HANDGUN);
		pNetInfo->setWeaponItemID( WEAPON_SLOT_MELEE, DEFAULT_WEAPON_KNIFE);
		pNetInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, DEFAULT_WEAPON_THROWING1);
		pNetInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, DEFAULT_WEAPON_THROWING2);
	}

	const CHARA_PARTS_INFO* pPartsInfo = pNetInfo->getCharaParts();
	_RecreateSlotChara(pPartsInfo);
	_CheckEquip( pPartsInfo);
#endif
	
	_CheckWeapon( pNetInfo, mustUpdateSlot);
	
	//_RecreateSlotChara( pPartsInfo);
	m_viewCharaInfo._charaInfo.SetType( CHARA::TYPE_PLAYER);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	if( m_pCharacter == NULL)
	{
#ifdef IMPROVEMENT_MULTI_SLOT
		CHARA_RES_ID resID = CHARA::ItemID2ResID( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_CHARA)->m_TItemID);
#else
		CHARA_RES_ID resID = CHARA::ItemID2ResID( m_viewCharaInfo._charaInfo.GetParts()->GetCharaItemID());
#endif

		g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback) sAfterLoadChara, 0, &m_viewCharaInfo._charaInfo, resID, CHARA::TYPE_PLAYER);

		return true;
	}
	else
	{
		if( m_pCharacter != NULL)
		{
			// 장비 설정
			if( true )
			{
#ifdef IMPROVEMENT_MULTI_SLOT
				ITEM_INFO* pEquip = m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_PRIMARY);
				CHARA_RES_ID charaID = CHARA::ItemID2ResID( pEquip[EQUIPMENT_SLOT_CHARA].m_TItemID);

				I3ASSERT( pEquip[EQUIPMENT_SLOT_HEAD].m_TItemID > 0);
				m_pCharacter->Cmd_CreateEquip( charaID, pEquip[EQUIPMENT_SLOT_HEAD].m_TItemID);

				if( pEquip[EQUIPMENT_SLOT_FACE].m_TItemID > 0)
					m_pCharacter->Cmd_CreateEquip( charaID, pEquip[EQUIPMENT_SLOT_FACE].m_TItemID);
				else
					m_pCharacter->Cmd_DeleteEquip( EQUIP::ePART_FACEGEAR);

				I3ASSERT( pEquip[EQUIPMENT_SLOT_HOLSTER].m_TItemID > 0);
				m_pCharacter->Cmd_CreateEquip( charaID, pEquip[EQUIPMENT_SLOT_HOLSTER].m_TItemID);

				I3ASSERT( pEquip[EQUIPMENT_SLOT_BELT].m_TItemID > 0);
				m_pCharacter->Cmd_CreateEquip( charaID, pEquip[EQUIPMENT_SLOT_BELT].m_TItemID);
#else
				const CHARA_PARTS_INFO * pPartsInfo = m_viewCharaInfo._charaInfo.GetParts();
				CHARA_RES_ID charaID = CHARA::ItemID2ResID( pPartsInfo->GetCharaItemID());

				I3ASSERT( pPartsInfo->GetHeadgearItemID() > 0);
				m_pCharacter->Cmd_CreateEquip( charaID, pPartsInfo->GetHeadgearItemID() );

				if( pPartsInfo->GetFacegearItemID() > 0)
					m_pCharacter->Cmd_CreateEquip( charaID, pPartsInfo->GetFacegearItemID() );
				else
					m_pCharacter->Cmd_DeleteEquip( EQUIP::ePART_FACEGEAR);

				I3ASSERT( pPartsInfo->GetHolsterItemID() > 0);
				m_pCharacter->Cmd_CreateEquip( charaID, pPartsInfo->GetHolsterItemID() );

				I3ASSERT( pPartsInfo->GetBeltItemID() > 0);
				m_pCharacter->Cmd_CreateEquip( charaID, pPartsInfo->GetBeltItemID() );
#endif

				//m_bChangeAction = true;
				m_pCharacter->SetViewerChara();

				// 장비에 맞게 머리를 교체한다
				((Avatar*)m_pCharacter)->SetCharaHead( true);
			}

			// 모드에 따라 무기를 설정해야 하는 경우가 있다. 처리!!!
			//INT32 selectUsage = _CheckLimitWeapon();

			//	뷰 캐릭터의 현재 무기 슬롯이 변경됐다면 보여지는 무기 슬롯 변경
			//if(m_viewCharaInfo._weaponSlot != selectUsage)
			{
				//m_bChangeAction = true;
			}

			// 무기 설정
			//_RecreateWeapon();

			// 애니메이션을 다시 설정
			//_Reaction();
			m_pCharacter->Cmd_ChangeView(CHARA_VIEW_1PV);
		}
	}

	return true;
}


void UICharaViewer1PV::SetLoadedChara( INT32 idx, Avatar * pLoadedAvatar)
{
	pLoadedAvatar->ApplyLoadedRes();
	//// 머리 붙이기
	pLoadedAvatar->SetCharaHead( true);
	m_pCharacter = pLoadedAvatar;
	m_pCharacter->SetEnable( TRUE);
	m_pCharacter->SetPos( &m_CharacterPosition);

	I3_GAMENODE_ADDCHILD( g_pFramework->GetUIMainframe(), m_pCharacter );

	m_p3DView->Create3DView();
	g_pCharaManager->CreateViewChara( &m_viewCharaInfo);
	m_p3DView->Set3DView(m_pCharacter->GetNode());
	m_p3DView->OnVisibleScene();

	i3TextureCube * pTex;

	// ReflectionMap bind
	char szPath[ MAX_PATH];
	sprintf_s( szPath, "Gui/Showroom/Gunshowcaseroom_Ref.dds");

	i3ImageFile file;
	pTex = (i3TextureCube*) file.Load( szPath);
	I3ASSERT( pTex != NULL);

	{
		i3ReflectMapBindAttr * pAttr = i3ReflectMapBindAttr::new_object_ref();
		pAttr->SetTexture( pTex);
		pTex->Release();

		m_p3DView->getAttrSetNode()->AddAttr( pAttr);
	}

	// Light 설정
	//_AddLight();

	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.0f, 1.00f, 3.5f, 1.0f);

	i3Matrix::PostRotateYDeg( pCamMtx, -35.0f);

	m_pCharacter->SetEnable( true);

	m_pCharacter->SetMatrix( &I3_IDENTITYMATRIX);
	//i3Vector::Set( &vPos, 0.453f, 0.0f, 0.0f);
	VEC3D vPos;
	i3Vector::Set( &vPos, 0.f, 0.0f, 0.0f);
	m_pCharacter->forceSetPos( &vPos);


	I3ASSERT( m_pCharacter != NULL);

#if defined( I3_DEBUG)
	m_pCharacter->getSceneObject()->SetName( "3D Chara");
#endif

//	UITopMenu * pMenu = g_pFramework->GetUIMainframe()->GetTopMenu();
	//pMenu->Get3DRoot()->AddChild( m_pCharacter->getSceneObject());

	//	무기 구성. 기존 무기랑 같으면, 새로 생성하지 않는다
	//_RecreateWeapon();

	//	장비 구성. 기존 장비랑 같으면, 새로 생성하지 않는다
	//if( m_bRecreateEquip && !m_bRecreateChara)
	{
		g_pCharaManager->SetCharaEquipSet(m_pCharacter, &m_viewCharaInfo._charaInfo);
	}

	// 무기가 없는 경우..
	// 선택 가능한 무기를 찾는다.
	if( m_pCharacter->GetCurrentCharaWeapon() == NULL)
	{	
		for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			if( m_pCharacter->Cmd_ChangeWeapon( i) == true)
			{
				break;
			}
		}
	}

	// 애니메이션 다시 설정
	m_pCharacter->Cmd_Profile_Idle_Stand();
	ChangeCamera();
}

void UICharaViewer1PV::sAfterLoadChara( void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind)
{
	if( g_pFramework->GetUIMainframe() == NULL)
		return;
	
#if defined(TEST_STAGE_VIEWER)
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UIPopupStageViewer * pStageViewer = (UIPopupStageViewer*) pFrame->GetPopup( UPW_STAGE_VIEWER);
	if( pStageViewer != NULL)
	{
		pStageViewer->LoadCompleteChara( idx, (Avatar*) pAvatar);
	}
#endif
}

#ifdef IMPROVEMENT_MULTI_SLOT
bool UICharaViewer1PV::_CheckEquip( ITEM_INFO* pParts)
{
	T_ItemID id = 0;
	bool bRecreate = false;

	id = pParts[EQUIPMENT_SLOT_HEAD].m_TItemID;
	if( id != 0 && id != m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_HEAD)->m_TItemID)
	{
		bRecreate = true;
		pParts[EQUIPMENT_SLOT_HEAD].CopyTo( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_HEAD));
	}

	id = pParts[EQUIPMENT_SLOT_BELT].m_TItemID;
	if( id != 0 && id != m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_BELT)->m_TItemID)
	{
		bRecreate = true;
		pParts[EQUIPMENT_SLOT_BELT].CopyTo( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_BELT));
	}

	id = pParts[EQUIPMENT_SLOT_FACE].m_TItemID;
	if( id != m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_FACE)->m_TItemID)
	{
		bRecreate = true;
		pParts[EQUIPMENT_SLOT_FACE].CopyTo( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_FACE));
	}

	id = pParts[EQUIPMENT_SLOT_HOLSTER].m_TItemID;
	if( id != 0 && id != m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_HOLSTER)->m_TItemID)
	{
		bRecreate = true;
		pParts[EQUIPMENT_SLOT_HOLSTER].CopyTo( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_HOLSTER));
	}

	id = pParts[EQUIPMENT_SLOT_ITEM].m_TItemID;
	if( id != 0 && id != m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_ITEM)->m_TItemID)
	{
		bRecreate = true;
		pParts[EQUIPMENT_SLOT_ITEM].CopyTo( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_ITEM));
	}

	return bRecreate;
}

bool UICharaViewer1PV::_RecreateSlotChara( const ITEM_INFO* pParts)
{
	I3ASSERT( pParts != NULL);

	m_viewCharaInfo._charaInfo.SetType( CHARA::TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	setTeam( TEAM_RED);
	
	if( CHARA::PARTS::SameBody( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_PRIMARY), pParts) == false)
	{
		CHARA::PARTS::Assign( m_viewCharaInfo._charaInfo.GetEquip( EQUIPMENT_SLOT_PRIMARY), pParts, EQUIPMENT_SLOT_COUNT);
		return true;
	}

	return false;
}
#else
bool UICharaViewer1PV::_CheckEquip( const CHARA_PARTS_INFO * pParts)
{
	bool bRecreate = false;

	//	head
	if( pParts->GetHeadgearItemID() != 0 && pParts->GetHeadgearItemID() != m_viewCharaInfo._charaInfo.GetParts()->GetHeadgearItemID() )
	{
		bRecreate = true;

		m_viewCharaInfo._charaInfo.SetParts()->SetHeadgearInfo( pParts->GetHeadgearInfo());
	}

	if( pParts->GetBeltItemID() != 0 && pParts->GetBeltItemID() != m_viewCharaInfo._charaInfo.GetParts()->GetBeltItemID() )
	{
		bRecreate = true;

		m_viewCharaInfo._charaInfo.SetParts()->SetBeltInfo( pParts->GetBeltInfo());
	}

	if( pParts->GetFacegearItemID() != m_viewCharaInfo._charaInfo.GetParts()->GetFacegearItemID() )
	{// facegear는 없을 수도 있다. 0은 제외
		bRecreate = true;

		m_viewCharaInfo._charaInfo.SetParts()->SetFacegearInfo( pParts->GetFacegearInfo());
	}

	if( pParts->GetHolsterItemID() != 0 && pParts->GetHolsterItemID() != m_viewCharaInfo._charaInfo.GetParts()->GetHolsterItemID() )
	{
		bRecreate = true;

		m_viewCharaInfo._charaInfo.SetParts()->SetHolsterInfo( pParts->GetHolsterInfo());
	}

	//	item
	if( pParts->GetItemItemID() != 0 && pParts->GetItemItemID() != m_viewCharaInfo._charaInfo.GetParts()->GetItemItemID() )
	{
		bRecreate = true;

		m_viewCharaInfo._charaInfo.SetParts()->SetItemInfo( pParts->GetItemInfo() );
	}

	return bRecreate;
}

bool UICharaViewer1PV::_RecreateSlotChara( const CHARA_PARTS_INFO * pPartsInfo)
{
	I3ASSERT( pPartsInfo != NULL);

	m_viewCharaInfo._charaInfo.SetType( CHARA::TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	setTeam( TEAM_RED);
	I3ASSERT( pPartsInfo != NULL);

	if( m_viewCharaInfo._charaInfo.GetParts()->IsSameBody( pPartsInfo) == false )
	{
		pPartsInfo->CopyTo( m_viewCharaInfo._charaInfo.SetParts());
		return true;
	}

	return false;
}

#endif


bool UICharaViewer1PV::_CheckWeapon( NET_CHARA_INFO * pNetInfo, WEAPON_SLOT_TYPE mustUpdateSlot)
{
	bool bRecreate = false;

	ITEM_INFO weapons[ WEAPON_SLOT_COUNT];

	INT32 i;

	for( i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
#ifdef IMPROVEMENT_MULTI_SLOT
		weapons[i].m_TItemID = pNetInfo->GetCharaWeapon( (WEAPON_SLOT_TYPE)i)->m_TItemID;
#else
		weapons[i].m_TItemID = pNetInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)i);
#endif
	}

	GameContextUtil::CheckWeaponList( weapons );

	//	weapons
	for(INT32 i = MAX_WEAPON_CHECK_SLOT_COUNT; i >= 0; i--)
	{
		//	네트워크 무기 정보와 배경 캐릭터 무기 정보가 다를 경우 싱크를 맞춘다.
		if( (weapons[i].m_TItemID != 0) && (weapons[i].m_TItemID != m_viewCharaInfo._weapon[i]))
		{
			bRecreate = true;
			m_viewCharaInfo._weapon[i] = weapons[i].m_TItemID;
		}
	}
	
	return bRecreate;
}

void UICharaViewer1PV::ChangeCamera()
{
	
	//VEC3D					pFPSBasePosition;
	//i3Camera * pCam =		m_p3DView->get3DCamera();
	//Matrix pCamMatrix		= pCam->GetMatrix();
	//Matrix pTargetMatrix	= m_pCharacter->GetMatrix();

	//m_pCharacter->Cmd_ChangeView( CHARA_VIEW_1PV)

	//m_pCharacter->getDBInfo()->GetFPSCamPos( &m_vFPSBasePosition);

	//pCam->SetCameraSide(  getX( &pFPSBasePosition));
	//pCam->SetCameraHeight(  getY( &pFPSBasePosition));
	//pCam->SetCameraDist(  getZ( &pFPSBasePosition));

}

void UICharaViewer1PV::InitCamera()
{
	m_FPSCamera->InitCamera(m_pCharacter,0.0f/* ,m_p3DView->get3DCamera()*/);	
}
