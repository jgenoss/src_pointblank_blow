#include "pch.h"
#include "ReadyBg.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE(CReadyBg, i3GameNode);

CReadyBg::CReadyBg()
{
	m_pAttrSet		= NULL;
	m_pLight		= NULL;
	m_pPointLight	= NULL;
	m_pBackground	= NULL;
	m_pCharacter	= NULL;
	
	m_currentViewType		= VT_DEFAULT;

	i3mem::FillZero(m_pCameraAnimation, sizeof(m_pCameraAnimation));

	m_pCameraRoot	= NULL;
	m_cameraState	= CAS_IDLE;

	m_lastViewTeam		= CHARACTER_TEAM_RED;
	m_lastWeaponUsage	= -1;

	i3mem::FillZero(m_pBGTransformDummy, sizeof(m_pBGTransformDummy));

	m_rCharaChangePoseTime	= 0.0f;
	m_timeLocal		= 0.0f;
	m_LightDist = 0.0f;
	m_LightTheta = 0.0f;
	m_LightRho = 0.0f;
}

CReadyBg::~CReadyBg()
{
	if( m_pCharacter != NULL)
	{
		_RemoveViewChara();
	}

	if (m_pCameraRoot)
	{
		m_pAttrSet->RemoveChild(m_pCameraRoot);
		m_pCameraRoot->RemoveChild(m_pCameraAnimation[m_cameraState]);
		I3_SAFE_RELEASE(m_pCameraRoot);
	}

	for(UINT32 i = 0; i < CAS_MAX; i++)
	{
		I3_SAFE_RELEASE(m_pCameraAnimation[i]);
	}

	if (m_pBackground)
	{
		m_pAttrSet->RemoveChild(m_pBackground);
		I3_SAFE_RELEASE(m_pBackground);

		for(INT32 i=0; i<BG_DUMMY_TRANSFORM_COUNT; i++)
		{
			m_pBGTransformDummy[i] = NULL;
		}
	}

	i3SceneGraphContext * pSgCtx = g_pFramework->getSgContext();
	if( pSgCtx != NULL)
	{
		if( m_pLight != NULL)		
			g_pFramework->removeLight(m_pLight);
		if( m_pPointLight != NULL)	
			g_pFramework->removeLight( m_pPointLight);
	}

	I3_SAFE_RELEASE(m_pLight);
	I3_SAFE_RELEASE( m_pPointLight);

	if (m_pAttrSet)
	{
		m_pAttrSet->RemoveFromParent();
		m_pAttrSet->RemoveAllAttrs();
		I3_SAFE_RELEASE( m_pAttrSet);
	}
}

void CReadyBg::Create(i3GameNode* pParentNode)
{
	i3GameNode::Create(pParentNode);

	_InitAttrSet();
	_InitLight();

	// 주의 - InitBackground()보다 위에 있어야합니다.
	_InitCamera();
	_InitBackground();

	i3TextureCube * pTex;

	// ReflectionMap bind
	char szPath[ MAX_PATH];
	sprintf_s( szPath, "Gui/Showroom/Gunshowcaseroom_Ref.dds");

	if( i3FileStream::isFile( szPath) == false )
		return;


	i3ImageFile file;
	pTex = (i3TextureCube*) file.Load( szPath);
	I3ASSERT( pTex != NULL);

	{
		i3ReflectMapBindAttr * pAttr = i3ReflectMapBindAttr::NewObjectRef();
		pAttr->SetTexture( pTex);
		pTex->Release();

		m_pAttrSet->AddAttr( pAttr);
	}
}

void CReadyBg::_UpdateLight(void)
{
	MATRIX mtx;

	i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, m_LightDist);
	i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( m_LightRho));
	i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( m_LightTheta));

	//I3TRACE( "%f, %f\n", m_LightTheta, m_LightRho);

	m_pLight->SetWorldSpaceTransform( &mtx);
}

void CReadyBg::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	static CameraAnimationState s_ReadyBg_CameraStateForChara = CAS_IDLE;
	i3GameNode::OnUpdate(rDeltaSeconds);

	m_timeLocal += rDeltaSeconds;
	if( m_timeLocal > 0.5f)
	{
		i3RenderContext * pCtx = g_pViewer->GetRenderContext();

		if( pCtx->GetPerformanceReport()->GetFPS() < 30.0f)
		{
			g_pFramework->removeLight( m_pPointLight);
		}
	}

	if (m_pCharacter)
	{
		if( !m_pCharacter->getActionContext()->isUpper( CHARA_UPPER_NONEBATTLEWEAPONSHOW) &&
			!m_pCharacter->getActionContext()->isCommon( CHARA_ACTION_COMMON_CEREMONY) )
		{
			m_rCharaChangePoseTime += rDeltaSeconds;
			if( m_rCharaChangePoseTime > i3Math::Randf() * 5.f + 10.f )
			{
				m_pCharacter->Cmd_Profile_Idle_B();

				m_rCharaChangePoseTime = 0.f;
			}
		}

		m_pCharacter->SetEnable( TRUE);	//SetVisible(TRUE);

		// Camera에 맞춰 캐릭터를 회전시킵니다.
		if( m_cameraState != s_ReadyBg_CameraStateForChara )
		{	// 한 프레임 튀는 것을 방지 하기 위해 스테이트가 바뀌는 시점에서는 업데이트 하지 않습니다.
			s_ReadyBg_CameraStateForChara = m_cameraState;
		}
		else
		{
			i3Camera * pCam = (i3Camera *) i3Scene::FindNodeByExactType( m_pCameraAnimation[m_cameraState], i3Camera::GetClassMeta());
			I3ASSERT(pCam != NULL);

			VEC3D vDir;
			i3Vector::Sub( &vDir, i3Matrix::GetPos( pCam->GetCacheMatrix()), m_pCharacter->GetPos() );

			// 카메라 방향으로 캐릭터Y축 회전
			REAL32 theta = i3Math::atan2( getX( &vDir), getZ( &vDir) );
			m_pCharacter->getBoneContext()->setTheta( theta + 0.65f);//dot * I3_PI);
		}
	}

	#if !defined( I3_NO_PROFILE)
	{
		bool bProcess = false;
		i3InputDeviceManager * pManager = g_pViewer->GetInputDeviceManager();
		i3InputKeyboard * pKey = pManager->GetKeyboard();

		if( pKey->GetPressState( I3I_KEY_LCTRL))
		{
			if( pKey->GetPressState( I3I_KEY_LSHIFT))
			{
				if( pKey->GetPressState( I3I_KEY_LEFT))
				{
					m_LightTheta -= 2.0f;	bProcess = true;
				}
				else if( pKey->GetPressState( I3I_KEY_RIGHT))
				{
					m_LightTheta += 2.0f;	bProcess = true;
				}

				if( pKey->GetPressState( I3I_KEY_UP))
				{
					m_LightRho += 2.0f; bProcess = true;
				}
				else if( pKey->GetPressState( I3I_KEY_DOWN))
				{
					m_LightRho -= 2.0f; bProcess = true;
				}
			}
			else
			{
				REAL32 x = 0.0f, y = 0.0f, z = 0.0f;

				if( pKey->GetPressState( I3I_KEY_LEFT))
				{
					x -= 0.05f;
				}
				else if( pKey->GetPressState( I3I_KEY_RIGHT))
				{
					x += 0.05f;
				}

				if( pKey->GetPressState( I3I_KEY_UP))
				{
					z -= 0.05f;
				}
				else if( pKey->GetPressState( I3I_KEY_DOWN))
				{
					z += 0.05f;
				}

				if( pKey->GetPressState( I3I_KEY_PGUP))
				{
					y += 0.05f;
				}
				else if( pKey->GetPressState( I3I_KEY_PGDN))
				{
					y -= 0.05f;
				}

				if( (x != 0.0f) || (y != 0.0f) || (z != 0.0f))
				{
					i3Matrix::PostTranslate( m_pPointLight->GetWorldSpaceTransform(), x, y, z);
				}
			}
		}

		if( bProcess)
		{
			_UpdateLight();
		}
	}
	#endif
}

void CReadyBg::CharaChangeClanMarkInReadyBg()
{
	I3ASSERT(m_pCharacter != NULL);
	switch(m_currentViewType)
	{
	case VT_DEFAULT:	//	Don't break! VT_SLOT과 동일합니다.
		{
			USER_INFO_BASIC		MyUserInfoBasic;
			g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
			m_pCharacter->CharaChangeClanMark(MyUserInfoBasic._clanMark);
		}
		break;
	case VT_SLOT:
		{
			m_pCharacter->CharaChangeClanMark(g_pGameContext->getSlotInfo( m_viewCharaInfo._charaInfo.GetNetIdx() )->_clanMark);
		}
		break;
	case VT_OTHER:
		{
			USER_INFO_BASIC		MyUserInfoBasic;
			g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
			m_pCharacter->CharaChangeClanMark(MyUserInfoBasic._clanMark);
		}
		break;
	default:
		I3TRACE("[WARNING] CReadyBg::UpdateCharacter() - Unknown viewType");
		break;
	}
}

BOOL CReadyBg::UpdateCharacter(ViewType viewType,BOOL bWeaponLimit, VIEW_CHARA_INFO * pNewCharaInfo)
{
	BOOL ret = FALSE;
// 	NET_CHARA_INFO* pNetCharaInfo = NULL;
// 	INT32 selectUsage = m_viewCharaInfo._weaponSlot;
// 	INT32 idxSlot = -1;
// 	BOOL bReadyRoom = (PHASE_READY_ROOM == ((CStageReady*)getParent())->GetCurrentPhaseType());
// 	BOOL bRecreateChara = FALSE;
// 	BOOL bRecreateWeapon = FALSE;
// 	BOOL bRecreateEquip = FALSE;
// 	BOOL bChangeAction = FALSE;
// 
// 
// 	// 초도보급 전이라면, 업데이트 하지 않는다
// 	if ( ! g_pGameContext->HaveNickName() )
// 	{
// 		return FALSE;
// 	}
// 
// 
// 	//// 1.5 Check Weapon
// 	//// 무기가 설정되어있지 않다면, 기본지급무기로 설정한다.
// 	//NET_CHARA_INFO* pMyInfo = g_pGameContext->SetMyCharaInfo();
// 
// 	//if (0 == pMyInfo->_WeaponList[0] &&
// 	//	0 == pMyInfo->_WeaponList[1] &&
// 	//	0 == pMyInfo->_WeaponList[2] &&
// 	//	0 == pMyInfo->_WeaponList[3] &&
// 	//	0 == pMyInfo->_WeaponList[4] )
// 	//{
// 	//	pMyInfo->_WeaponList[0] = DEFAULT_WEAPON_ASSAULT;
// 	//	pMyInfo->_WeaponList[1] = DEFAULT_WEAPON_HANDGUN;
// 	//	pMyInfo->_WeaponList[2] = DEFAULT_WEAPON_KNIFE;
// 	//	pMyInfo->_WeaponList[3] = DEFAULT_WEAPON_THROWING1;
// 	//	pMyInfo->_WeaponList[4] = DEFAULT_WEAPON_THROWING2;
// 	//}
// 
// 	NET_CHARA_INFO* pMyInfo = g_pGameContext->SetMyCharaInfo();
// 
// 	if (0 == pMyInfo->getWeaponItemID(0) &&
// 		0 == pMyInfo->getWeaponItemID(1) &&
// 		0 == pMyInfo->getWeaponItemID(2) &&
// 		0 == pMyInfo->getWeaponItemID(3) &&
// 		0 == pMyInfo->getWeaponItemID(4) )
// 	{
// 		pMyInfo->setWeaponItemID(0, DEFAULT_WEAPON_ASSAULT);
// 		pMyInfo->setWeaponItemID(1, DEFAULT_WEAPON_HANDGUN);
// 		pMyInfo->setWeaponItemID(2, DEFAULT_WEAPON_KNIFE);
// 		pMyInfo->setWeaponItemID(3, DEFAULT_WEAPON_THROWING1);
// 		pMyInfo->setWeaponItemID(4, DEFAULT_WEAPON_THROWING2);
// 	}
// 
// 	m_currentViewType = viewType;
// 
// 	// 캐릭터 정보 설정. 기존 캐릭터랑 같으면, 새로 생성하지 않는다.
// 	{
// 		switch(m_currentViewType)
// 		{
// 		case VT_DEFAULT:	bRecreateChara = _RecreateDefaultChara();				break;
// 		case VT_SLOT:		bRecreateChara = _RecreateSlotChara();					break;
// 		case VT_OTHER:		bRecreateChara = _RecreateOtherChara(pNewCharaInfo);	break;
// 		default:
// 			I3TRACE("[WARNING] CReadyBg::UpdateCharacter() - Unknown viewType");
// 			break;
// 		}
// 
// 
// 
// 		idxSlot = m_viewCharaInfo._charaInfo.GetNetIdx();
// 		I3ASSERT(idxSlot >= 0);
// 
// 		// 무기 정보 설정
// 		pNetCharaInfo = g_pGameContext->getNetCharaInfo(idxSlot);
// 
// 		switch(m_currentViewType)
// 		{
// 		case VT_DEFAULT:	//	Don't break! VT_SLOT과 동일합니다.
// 		case VT_SLOT:
// 			{
// 				bRecreateWeapon = _RecreateDefaultAndSlotWeapon(pNetCharaInfo);
// 				bRecreateEquip = _RecreateDefaultAndSlotEquip(pNetCharaInfo);
// 			}
// 			break;
// 		case VT_OTHER:
// 			{
// 				bRecreateWeapon = _RecreateOtherWeapon(pNewCharaInfo);
// 				bRecreateEquip = _RecreateOtherEquip(pNewCharaInfo);
// 			}
// 			break;
// 		default:
// 			I3TRACE("[WARNING] CReadyBg::UpdateCharacter() - Unknown viewType");
// 			break;
// 		}
// 		
// 		// 캐릭터 생성
// 		if (bRecreateChara)
// 		{
//  			if( m_pCharacter != NULL)
// 				_RemoveViewChara();
// 
// 			_CreateViewChara(&m_viewCharaInfo);
// 			CharaChangeClanMarkInReadyBg();
// 
// 			if( selectUsage == -1)
// 				selectUsage = 0;
// 		}
// 		else
// 		{
// 			CharaChangeClanMarkInReadyBg();
// 		}
// 
// 		//	무기 구성. 기존 무기랑 같으면, 새로 생성하지 않는다 (캐릭터 생성 이후에 호출되야 합니다)
// 		if( bRecreateWeapon || bRecreateChara)
// 		{
// 			//	슬롯이 있는 캐릭터
// 			if( m_currentViewType != VT_OTHER)
// 			{
// 				//g_pCharaManager->SetCharaWeaponSetWithoutLock(m_pCharacter, pNetCharaInfo);
// 				g_pCharaManager->SetCharaWeaponSet(m_pCharacter, pNetCharaInfo);
// 			}
// 			//	가상 캐릭터
// 			else
// 			{
// 				for(INT32 i=WEAPON_SLOT_COUNT - 2; i>=0; i--)
// 				{
// 					if( m_viewCharaInfo._weapon[i] != 0)
// 					{
// 						m_pCharacter->Cmd_CreateWeapon( i, (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS(m_viewCharaInfo._weapon[i]), GET_ITEM_FLAG_NUMBER(m_viewCharaInfo._weapon[i]));
// 					}
// 				}
// 			}
// 		}
// 
// 		//	장비 구성. 기존 장비랑 같으면, 새로 생성하지 않는다 (캐릭터 생성 이후에 호출되야 합니다)
// 		if( bRecreateEquip || bRecreateChara)
// 		{
// 			if( m_currentViewType != VT_OTHER)
// 			{
// 				
// 				//// 1.5 Check Equip
// 				//// 장비 구성
// 				//g_pCharaManager->SetCharaEquipSet(m_pCharacter, &m_viewCharaInfo._charaInfo, pNetCharaInfo->_EquipCharacter.GetHairNum(), pNetCharaInfo->_EquipCharacter.GetItemNum());
// 		
// 				TItemID ui32HelmetID = pNetCharaInfo->_CharacterParts.GetHeadgearItemID();
// 				// 헬멧이 없으면 마스크에서 찾아야함
// 				if(ui32HelmetID == 0)
// 					ui32HelmetID = pNetCharaInfo->_CharacterParts.GetFacegearItemID();
// 
// 				TItemID ui32BeretID = pNetCharaInfo->_CharacterParts.GetItemItemID();
// 
// 				//	장비 구성
// 				g_pCharaManager->SetCharaEquipSet(m_pCharacter, &m_viewCharaInfo._charaInfo, ui32HelmetID, ui32BeretID);
// 			}
// 			else
// 			{
// 				//	장비 구성
// 				g_pCharaManager->SetCharaEquipSet(m_pCharacter, &m_viewCharaInfo._charaInfo, m_viewCharaInfo._head, m_viewCharaInfo._item);
// 			}
// 		}
// 	}
// 
// 	ROOM_INFO_BASIC roomInfo;
// 	g_pGameContext->getMyRoomInfo(&roomInfo);
// 
// 	if (bWeaponLimit && bReadyRoom)
// 	{
// 		for(UINT32 i = 0; i < 4; i++)
// 		{
// 			if (GlobalFunc::CheckAvailableWeaponUsage(roomInfo._WeaponFlag, (WEAPON_SLOT_TYPE)i))
// 			{
// 				selectUsage = i;
// 
// 				break;
// 			}
// 		}
// 	}
// 
// 	//	레디룸에서 무기 제한이 걸렸다면 무기 슬롯을 변경
// 	// RPG7 선택
// 	if( TESTBIT( roomInfo._WeaponFlag, LOCK_RPG7))
// 	{
// 		selectUsage = 0;
// 
// 		// 기본 RPG7 제공
// 		m_pCharacter->getWeaponContext()->CreateRPG7ForGameMode();
// 	}
// 	//	샷건전
// 	else if( TESTBIT( roomInfo._WeaponFlag, LOCK_SHOTGUN))
// 	{
// 		selectUsage = 0;
// 
// 		//	로비 캐릭터가 샷건이 장착하고 있지 않으면 강제로 생성
// 		if( GET_ITEM_FLAG_CLASS( m_viewCharaInfo._weapon[0]) != WEAPON_CLASS_SHOTGUN)
// 		{
// 			// 샷건전에서 기본 샷건 제공
// 			m_pCharacter->getWeaponContext()->CreateDefaultShotGunForGameMode();
// 		}
// 	}
// 	//	저격전
// 	else if (TESTBIT( roomInfo._WeaponFlag, LOCK_SNIPER))
// 	{
// 		selectUsage = 0;
// 
// 		//	로비 캐릭터가 저격총이 장착하고 있지 않으면 강제로 생성
// 		if( GET_ITEM_FLAG_CLASS( m_viewCharaInfo._weapon[0]) != WEAPON_CLASS_SNIPER)
// 		{
// 			// 저격전에서 기본 저격총 제공
// 			m_pCharacter->getWeaponContext()->CreateDefaultSniperForGameMode();
// 		}
// 	}
// 	//	너클
// 	else if (TESTBIT( roomInfo._WeaponFlag, LOCK_KNUCKLE))
// 	{
// 		selectUsage = 0;
// 
// 		//	로비 캐릭터가 너클을 장착하고 있지 않으면 강제로 생성
// 		if( GET_ITEM_FLAG_CLASS( m_viewCharaInfo._weapon[WEAPON_SLOT_MELEE]) != WEAPON_CLASS_KNUCKLE)
// 		{
// 			// 너클전에서 기본 맨주먹 제공
// 			m_pCharacter->getWeaponContext()->CreateDefaultKnucleForGameMode();
// 		}
// 	}
// 	else
// 	{
// 		switch(m_currentViewType)
// 		{
// 		case VT_DEFAULT:	//	Don't break! VT_SLOT과 동일합니다.
// 		case VT_SLOT:
// 			{
// 				// 무기를 변경하고 변경된 무기 슬롯을 받는다.
// 				selectUsage = m_pCharacter->getWeaponContext()->ChangeToEquipedWeapon( idxSlot, roomInfo._WeaponFlag);	
// 
// 				if( selectUsage >= 0)
// 					bChangeAction = TRUE;
// 			}
// 			break;
// 		case VT_OTHER:
// 			{
// 			}
// 			break;
// 		default:
// 			I3TRACE("[WARNING] CReadyBg::UpdateCharacter() - Unknown viewType");
// 			break;
// 		}
// 		
// 	}
// 
// 	//	뷰 캐릭터의 현재 무기 슬롯이 변경됐다면 보여지는 무기 슬롯 변경
// 	if(m_currentViewType == VT_OTHER)
// 	{
// 		if(pNewCharaInfo->_weaponSlot != -1 && selectUsage != pNewCharaInfo->_weaponSlot)
// 		{
// 			selectUsage = pNewCharaInfo->_weaponSlot;
// 
// 			bChangeAction = TRUE;
// 		}
// 	}
// 	else
// 	{
// 		if(m_viewCharaInfo._weaponSlot != selectUsage)
// 		{
// 			bChangeAction = TRUE;
// 		}
// 	}
// 
// 	if( bRecreateChara || bRecreateWeapon || (bWeaponLimit && bReadyRoom))
// 	{
// 		bChangeAction = TRUE;
// 	}
// 
// 	//	모든 설정이 끝나고 마지막에 호출되야 함
// 	if(bChangeAction)
// 	{
// 		m_viewCharaInfo._weaponSlot = selectUsage;
// 
// 		if( m_pCharacter->Cmd_ChangeWeapon( selectUsage, true))
// 			m_pCharacter->getAnimContext()->ChangeAnimationContext();
// 
// 		// KrissSuperV extension on
// 		if( m_pCharacter->getWeaponContext()->getCurrent() != NULL)
// 		{
// 			if( m_pCharacter->getWeaponContext()->getCurrent()->IsExactTypeOf( WeaponKrissSuperV::GetClassMeta()))
// 			{
// 				//m_pCharacter->getWeaponContext()->getCurrent()->ExtensionActivate( TRUE);
// 				((WeaponKrissSuperV*)m_pCharacter->getWeaponContext()->getCurrent())->SetEnableDualWeapon( true, false);
// 			}
// 		}
// 
// 		if( m_pCharacter->getActionContext()->isUpper( CHARA_UPPER_NONEBATTLEIDLE ))
// 			m_pCharacter->Cmd_Profile_Idle_A();
// 		else
// 			m_pCharacter->Cmd_Profile_Idle_B();
// 
// 		m_lastWeaponUsage = selectUsage;
// 
// 		ret = TRUE;
// 	}

	return ret;
}

void CReadyBg::PlayCeremonyCharacter()
{
	if( m_pCharacter != NULL)
	{
		m_pCharacter->Cmd_Profile_Ceremony();
		m_rCharaChangePoseTime = 0.f;
	}
}

void CReadyBg::_CreateViewChara(VIEW_CHARA_INFO * pInfo)
{
	VEC3D vPos;
//	INT32 idxSlot = -1;

	m_pCharacter = g_pCharaManager->CreateViewChara(pInfo);

	I3ASSERT( getParent() != NULL);
	I3_GAMENODE_ADDCHILD( getParent(), m_pCharacter );

	I3ASSERT( m_pAttrSet != NULL);
	m_pAttrSet->AddChild( m_pCharacter->GetNode());

	m_pCharacter->SetMatrix( &I3_IDENTITYMATRIX);
	i3Vector::Set( &vPos, 0.453f, 0.0f, 0.0f);
    m_pCharacter->forceSetPos( &vPos);

	if( m_pCharacter->getWeaponContext()->getCurrent() != NULL)
	{
		if( m_pCharacter->getWeaponContext()->getCurrent()->IsExactTypeOf( WeaponKrissSuperV::GetClassMeta()))
		{
			//m_pCharacter->getWeaponContext()->getCurrent()->ExtensionActivate( TRUE);
			((WeaponKrissSuperV*)m_pCharacter->getWeaponContext()->getCurrent())->SetEnableDualWeapon( true, false);
		}
	}

	m_pCharacter->Cmd_Profile_Idle_A();
	m_pCharacter->SetViewerChara();
}

void CReadyBg::_RemoveViewChara(void)
{
	/*if( m_pCharacter != NULL)
	{
		i3GameNode * pParent = m_pCharacter->getParent();
		if( pParent != NULL)
			pParent->RemoveChild( m_pCharacter);

		m_pCharacter->GetNode()->RemoveFromParent();
		m_pCharacter->getCollisionContext()->DetachCollisionNode();
		m_pCharacter->Destroy();
		//m_pCharacter->RemoveAllChild();
		I3_SAFE_RELEASE(m_pCharacter);
	}
	*/


	if( m_pCharacter != NULL)
	{
		g_pCharaManager->RemoveCharaOnlyInLobby( m_pCharacter );
		m_pCharacter = NULL;
	}
}

void CReadyBg::MoveCamera(CameraLook lookState)
{
	CameraAnimationState newState = CAS_IDLE;

	REAL32 rPlayTime = ((i3Transform*)(m_pCameraAnimation[m_cameraState]->GetChild(0)))->getPlayTime();
	REAL32 rDuration = ((i3Transform*)(m_pCameraAnimation[m_cameraState]->GetChild(0)))->GetDuration();

	switch(m_cameraState)
	{
	case CAS_IDLE:				// fall throught
	case CAS_LEFT_TO_IDLE:		// fall throught
	case CAS_RIGHT_TO_IDLE:
		switch(lookState)
		{
		case CL_IDLE:	return;
		case CL_RECORD:	newState = CAS_IDLE_TO_LEFT;	break;
		case CL_WEAPON: newState = CAS_IDLE_TO_RIGHT;	break;
		}
		break;
	case CAS_IDLE_TO_LEFT:		// fall throught
	case CAS_RIGHT_TO_LEFT:
		switch(lookState)
		{
		case CL_IDLE:	newState = CAS_LEFT_TO_IDLE;	break;
		case CL_RECORD:	return;
		case CL_WEAPON: newState = CAS_LEFT_TO_RIGHT;	break;
		}
		break;
	case CAS_IDLE_TO_RIGHT:		// fall throught
	case CAS_LEFT_TO_RIGHT:
		switch(lookState)
		{
		case CL_IDLE:	newState = CAS_RIGHT_TO_IDLE;	break;
		case CL_RECORD:	newState = CAS_RIGHT_TO_LEFT;	break;
		case CL_WEAPON:	return;
		}
		break;
	default:
		I3ASSERT_0;	// 카메라 스테이트가 잘못되었습니다.
		break;
	}

	// 애니메이션 중이라면 변경하지 않는다.
	if (rPlayTime > rDuration)
	{
		//	카메라 이동시 화면에 블러 효과
		//i3RasterizeChain_SimpleMotionBlur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
		//pRasterizeBlur->StartEffect(1.5f, 0.7f, 0.99f);

		// 애니메이션을 교체한다
		m_pCameraRoot->RemoveChild(m_pCameraAnimation[m_cameraState]);

		m_cameraState = newState;

		m_pCameraRoot->AddChild(m_pCameraAnimation[m_cameraState]);
		((i3Transform*)(m_pCameraAnimation[m_cameraState]->GetChild(0)))->SetPlayMode(I3_ANIM_PLAY_MODE_CLAMP);
		((i3Transform*)(m_pCameraAnimation[m_cameraState]->GetChild(0)))->SetPlayCount(0);
		((i3Transform*)(m_pCameraAnimation[m_cameraState]->GetChild(0)))->SetPlayTime(0.0f);

		{
			i3Camera * pCam = (i3Camera *) i3Scene::FindNodeByExactType( m_pCameraRoot, i3Camera::GetClassMeta());
			I3ASSERT( pCam != NULL);

			g_pFramework->getSgContext()->setCurrentCamera( pCam);

			// Aspect Ratio의 조정
			pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pViewer->GetViewWidth(), (REAL32) g_pViewer->GetViewHeight(), pCam->GetNear(), pCam->GetFar());
		}
	}
}

void CReadyBg::_InitAttrSet(void)
{
	I3ASSERT(NULL == m_pAttrSet);

	m_pAttrSet = i3AttrSet::NewObject();

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
		pAttr->Set( TRUE);

		m_pAttrSet->AddAttr( pAttr);

		g_pFramework->Get3DRoot()->AddChild( m_pAttrSet);
	}
}

void CReadyBg::_InitLight(void)
{
	I3ASSERT(NULL == m_pLight);

	COLOR		col;
	VEC4D		vd_diffuse, vd_specular, vp_diffuse, vp_specular;

	COLORREAL	ambient;
	COLORREAL	d_diffuse, d_specular;
	COLORREAL	p_diffuse, p_specular;
	REAL32		p_range = 6.0f;
	VEC3D		p_pos,d_rot;


	//Load Lobby Light Info
	{
		i3RegKey * pRoot = Config::_GetRegRoot("Stage/Stage");
		I3ASSERT( pRoot != NULL);

		i3RegKey * pKey = Config::FindKey( "Lobby", pRoot);
		if( pKey != NULL)
		{
			FIND_REG_DATA( pKey, "Ambient", &col);
			i3Color::Set( &ambient, i3Color::GetR(&col),i3Color::GetG(&col),i3Color::GetB(&col),i3Color::GetA(&col) );

			//directional light
			FIND_REG_DATA( pKey, "D_Light_Diffuse", &vd_diffuse);
			i3Color::Set( &d_diffuse, getX( &vd_diffuse), getY( &vd_diffuse), getZ( &vd_diffuse),getW( &vd_diffuse));

			FIND_REG_DATA( pKey, "D_Light_Specular", &vd_specular);
			i3Color::Set( &d_specular, getX( &vd_specular), getY( &vd_specular), getZ( &vd_specular),getW( &vd_specular));

			FIND_REG_DATA( pKey, "D_Light_Rotate",	&d_rot);

			//pointlight
			FIND_REG_DATA( pKey, "P_Light_Diffuse", &vp_diffuse);
			i3Color::Set( &p_diffuse, getX( &vp_diffuse), getY( &vp_diffuse), getZ( &vp_diffuse),getW( &vp_diffuse));

			FIND_REG_DATA( pKey, "P_Light_Specular", &vp_specular);
			i3Color::Set( &p_specular, getX( &vp_specular), getY( &vp_specular), getZ( &vp_specular),getW( &vp_specular));

			FIND_REG_DATA( pKey, "P_Light_Range",	&p_range);
			FIND_REG_DATA( pKey, "P_Light_Position", &p_pos);
		}
		else
		{
			i3Color::Set( &ambient, 0.4f, 0.4f, 0.4f, 1.0f);
			i3Color::Set( &d_diffuse, 2.0f, 2.0f, 2.0f, 1.0f);
			i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);
			i3Color::Set( &p_diffuse, 0.4f, 0.5f, 1.0f, 1.0f);
			i3Color::Set( &p_specular, 1.0f, 1.0f, 1.0f, 1.0f);

			p_range = 6.0f;
			i3Vector::Set( &p_pos, -0.100000f, 0.800000f, -0.450000f);
			i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);
		}
	}

	{
		m_pLight = i3Light::NewObject();
		m_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		{
			i3GfxCaps * pCaps = i3RenderContext::GetCaps();

			switch( pCaps->getGPUClass())
			{
				case I3G_GPU_CLASS_F :
				case I3G_GPU_CLASS_E :
				case I3G_GPU_CLASS_D :
					break;

				default:
					m_pLight->setShadowMapSize( 1024);
					m_pLight->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);
					break;
			}
		}

		m_pLight->SetDiffuse( &d_diffuse);
		m_pLight->SetSpecular( &d_specular);

		g_pFramework->addLight( m_pLight);
	}

	{
		m_pPointLight = i3Light::NewObject();
		m_pPointLight->SetLightType( I3G_LIGHT_POINT);
		m_pPointLight->setRange( p_range);

		m_pPointLight->SetDiffuse( &p_diffuse);
		m_pPointLight->SetSpecular( &p_specular);

		g_pFramework->addLight( m_pPointLight);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, &p_pos);
		m_pPointLight->SetWorldSpaceTransform( &mtx);
	}

	g_pFramework->getSgContext()->setAmbient( &ambient);

	// -12.000000, -34.000000
	m_LightTheta = getX( &d_rot);
	m_LightRho = getY( &d_rot);
	m_LightDist = getZ( &d_rot);

	_UpdateLight();
}

BOOL CReadyBg::_InitBackground(void)
{
	// 3D 배경 읽기
	i3SceneFile showroom;

	UINT32 rv = showroom.Load("Gui/Showroom/Gunshowcaseroom_Lighting.i3s");

	if (STREAM_ERR == rv)
	{
		I3WARN("Couldn't find %s. Load Default Setting", "Gunshowcaseroom_Lighting.i3s");
		return FALSE;
	}

	m_pBackground = showroom.GetSceneGraph();

	if (m_pBackground == NULL)
	{
		I3WARN( "Invalid I3S file (%s)", "Gunshowcaseroom_Lighting.i3s");
		return FALSE;
	}

	m_pBackground->AddRef();

	//	배경 모델의 환경 사운드용 더미들
	m_pBGTransformDummy[BG_DUMMY_TRANSFORM_CHOPPER] =	(i3Transform*) i3Scene::FindNodeByName(m_pBackground, "Dummy_Ch-47");
	I3ASSERT( m_pBGTransformDummy[BG_DUMMY_TRANSFORM_CHOPPER] != NULL &&
			m_pBGTransformDummy[BG_DUMMY_TRANSFORM_CHOPPER]->IsExactTypeOf( i3Transform::GetClassMeta()));

	m_pBGTransformDummy[BG_DUMMY_TRANSFORM_TRUCK] =		(i3Transform*) i3Scene::FindNodeByName(m_pBackground, "Dummy_Car_Humvee");
	I3ASSERT( m_pBGTransformDummy[BG_DUMMY_TRANSFORM_TRUCK] != NULL &&
			m_pBGTransformDummy[BG_DUMMY_TRANSFORM_TRUCK]->IsExactTypeOf( i3Transform::GetClassMeta()));

	m_pBGTransformDummy[BG_DUMMY_TRANSFORM_GUNFIRE1] =	(i3Transform*) i3Scene::FindNodeByName(m_pBackground, "Dummy_Gunfire_1");
	I3ASSERT( m_pBGTransformDummy[BG_DUMMY_TRANSFORM_GUNFIRE1] != NULL &&
			m_pBGTransformDummy[BG_DUMMY_TRANSFORM_GUNFIRE1]->IsExactTypeOf( i3Transform::GetClassMeta()));

	m_pBGTransformDummy[BG_DUMMY_TRANSFORM_GUNFIRE2] =	(i3Transform*) i3Scene::FindNodeByName(m_pBackground, "Dummy_Gunfire_2");
	I3ASSERT( m_pBGTransformDummy[BG_DUMMY_TRANSFORM_GUNFIRE2] != NULL &&
			m_pBGTransformDummy[BG_DUMMY_TRANSFORM_GUNFIRE2]->IsExactTypeOf( i3Transform::GetClassMeta()));

	m_pBGTransformDummy[BG_DUMMY_TRANSFORM_EMERGENCY] = (i3Transform*) i3Scene::FindNodeByName(m_pBackground, "Dummy_Siren");
	I3ASSERT( m_pBGTransformDummy[BG_DUMMY_TRANSFORM_EMERGENCY] != NULL &&
			m_pBGTransformDummy[BG_DUMMY_TRANSFORM_EMERGENCY]->IsExactTypeOf( i3Transform::GetClassMeta()));

	m_pAttrSet->AddChild(m_pBackground);

	return TRUE;
}

void CReadyBg::_InitCamera(void)
{
	// 기본 카메라 초기화
	i3Camera* pCamera = g_pFramework->GetDefaultCamera();
	i3Matrix::Identity(pCamera->GetMatrix());


	// 카메라용
	m_pCameraRoot = i3Node::NewObject();
	m_pAttrSet->AddChild(m_pCameraRoot);


	// 카메라 애니메이션 노드 불러오기
	for(UINT32 i = 0; i < CAS_MAX; i++)
	{
		i3SceneFile camFile;
		UINT32 rv = camFile.Load(_GetCameraAnimationFileName((CameraAnimationState)i));

		if (STREAM_ERR ==  rv)
		{
			I3WARN( "Invalid I3S file (%s)", _GetCameraAnimationFileName((CameraAnimationState)i));
			return;
		}

		m_pCameraAnimation[i] = camFile.GetSceneGraph();
		m_pCameraAnimation[i]->AddRef();

		((i3Transform*)(m_pCameraAnimation[i]->GetChild(0)))->SetPlayMode(I3_ANIM_PLAY_MODE_CLAMP);

		// 임시 코드
		{
			// MAX에서 설정된 Camera의 FOV는 세로 FOV가 아니라 가로 FOV이다.
			// 그러나 i3Export에서 가로 FOV를 그냥 세로 FOV값으로 설정하면서, 실제로는 더욱 큰 FOV로 바뀌는
			// 버그를 피하기 위해, 강제로 일정한 값으로 설정함.
			i3Camera * pCam = (i3Camera *) i3Scene::FindNodeByExactType( m_pCameraAnimation[i], i3Camera::GetClassMeta());
			I3ASSERT( pCam != NULL);

			pCam->SetFOV( I3_DEG2RAD( 34.5f));
		}
	}

	m_cameraState = CAS_IDLE;
	m_pCameraRoot->AddChild(m_pCameraAnimation[m_cameraState]);

	{
		i3Camera * pCam = (i3Camera *) i3Scene::FindNodeByExactType( m_pCameraAnimation[m_cameraState], i3Camera::GetClassMeta());
		I3ASSERT( pCam != NULL);

		g_pFramework->getSgContext()->setCurrentCamera( pCam);

		// Aspect Ratio의 조정
		pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pEnvSet->m_nScreenWidth, (REAL32) g_pEnvSet->m_nScreenHeight, pCam->GetNear(), pCam->GetFar());
	}
}

const char* CReadyBg::_GetCameraAnimationFileName(CameraAnimationState state) const
{
	char* strTable[CAS_MAX] = {
        "Gui/Showroom/Camera_Idle.I3S",
		"Gui/Showroom/Camera_IdleToLeft.I3S",
		"Gui/Showroom/Camera_IdleToRight.I3S",
		"Gui/Showroom/Camera_LeftToIdle.I3S",
		"Gui/Showroom/Camera_LeftToRight.I3S",
		"Gui/Showroom/Camera_RightToIdle.I3S",
		"Gui/Showroom/Camera_RightToLeft.I3S",
	};

	return strTable[state];
}

BOOL CReadyBg::_RecreateDefaultChara(void)
{
	// 슬롯 인덱스 강제 적용
	if (-1 == g_pGameContext->GetMySlotIdxEx())
	{
		g_pGameContext->setMySlotIdx(0);
	}

	USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
	g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);

	m_viewCharaInfo._charaInfo.SetNetIdx(MAX(g_pGameContext->GetMySlotIdxEx(), 0));
	m_viewCharaInfo._charaInfo.SetType(CHARA_TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	//	리소스와 팀을 설정
	if (CHARACTER_TEAM_RED == m_lastViewTeam)
	{
		m_viewCharaInfo._charaInfo.SetCharaID(InfoEquipChara.GetRedNum());
		m_viewCharaInfo._charaInfo.SetTeam(CHARACTER_TEAM_RED);
	}
	else
	{
		m_viewCharaInfo._charaInfo.SetCharaID(InfoEquipChara.GetBlueNum());
		m_viewCharaInfo._charaInfo.SetTeam(CHARACTER_TEAM_BLUE);
	}

	return TRUE;
}

BOOL CReadyBg::_RecreateSlotChara(void)
{
	const USER_INFO_EQUIPMENT_CHARA* pEquipCharacter = NULL;

	//	디폴트가 설정되어 있지 않거나 현재 자신과 같은 슬롯을 선택한 경우
	if (-1 == g_pGameContext->GetCurrentInfoSlot() || g_pGameContext->GetMySlotIdxEx() == g_pGameContext->GetCurrentInfoSlot())
	{
		if (-1 == g_pGameContext->GetMySlotIdxEx() || 0 == g_pGameContext->GetMySlotIdxEx() % 2)
		{
			m_lastViewTeam = CHARACTER_TEAM_RED;
		}
		else
		{
			m_lastViewTeam = CHARACTER_TEAM_BLUE;
		}

		//	디폴트 캐릭터 호출
		return _RecreateDefaultChara();
	}

	pEquipCharacter = g_pGameContext->GetUserInfoEquipmentChara();
	I3ASSERT(pEquipCharacter->GetRedNum() != NULL);
	I3ASSERT(pEquipCharacter->GetBlueNum() != NULL);
	if (pEquipCharacter->GetRedNum() == NULL || pEquipCharacter->GetBlueNum() == NULL)
	{
		// 캐릭터 아이디가 NULL이면 다은유저 정보를 받지 않았기 때문에
		// 캐릭터를 만들 수 없다. 그러므로 실패.
		return FALSE;
	}


	m_viewCharaInfo._charaInfo.SetNetIdx(g_pGameContext->GetCurrentInfoSlot());
	m_viewCharaInfo._charaInfo.SetType(CHARA_TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	//	리소스와 팀을 설정
	if (0 == g_pGameContext->GetCurrentInfoSlot() % 2)
	{
		m_viewCharaInfo._charaInfo.SetCharaID(pEquipCharacter->GetRedNum());
		m_viewCharaInfo._charaInfo.SetTeam(CHARACTER_TEAM_RED);
	}
	else
	{
		m_viewCharaInfo._charaInfo.SetCharaID(pEquipCharacter->GetBlueNum());
		m_viewCharaInfo._charaInfo.SetTeam(CHARACTER_TEAM_BLUE);
	}

	return TRUE;
}

BOOL CReadyBg::_RecreateOtherChara(VIEW_CHARA_INFO * pViewCharaInfo)
{

	I3ASSERT(pViewCharaInfo != NULL);

	m_viewCharaInfo._charaInfo.SetType(CHARA_TYPE_PLAYER_NET);
	m_viewCharaInfo._charaInfo.SetTeam(pViewCharaInfo->_charaInfo.GetTeam());
	m_viewCharaInfo._charaInfo.SetNetIdx(0);
	m_viewCharaInfo._charaInfo.SetHP(100.f);

	if ( pViewCharaInfo->_charaInfo.GetCharaID() != CHARACTER_RES_ID_UNKNOWN )
		m_viewCharaInfo._charaInfo.SetCharaID(pViewCharaInfo->_charaInfo.GetCharaID());

	m_lastViewTeam = pViewCharaInfo->_charaInfo.GetTeam();

	return TRUE;
}

BOOL CReadyBg::_RecreateDefaultAndSlotWeapon(NET_CHARA_INFO* pNetCharaInfo)
{
	INT32 idxSlot = m_viewCharaInfo._charaInfo.GetNetIdx();
	BOOL bRecreate = FALSE;

	// 1.5 Check Weapon
	//USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	//if (VT_DEFAULT == m_currentViewType || g_pGameContext->GetMySlotIdxEx() == idxSlot)
	//{
	//	g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);
	//}
	//else
	//{
	//	InfoEquipWeapon = *g_pGameContext->GetUserInfoEquipmentWeapon();
	//}

	//pNetCharaInfo->_WeaponList[0] = InfoEquipWeapon._noprim;
	//pNetCharaInfo->_WeaponList[1] = InfoEquipWeapon._nosub;
	//pNetCharaInfo->_WeaponList[2] = InfoEquipWeapon._nomelee;
	//pNetCharaInfo->_WeaponList[3] = InfoEquipWeapon._nothrow;
	//pNetCharaInfo->_WeaponList[4] = InfoEquipWeapon._noitem;

	// 이부분 나중에 어떻게 가져갈지 다시 생각해봐야할것같다.
	//if (VT_DEFAULT == m_currentViewType || g_pGameContext->GetMySlotIdxEx() == idxSlot)
	//{
	g_pFramework->GetCharaInfoContext()->SetCurrentSlotWeaponInfo( pNetCharaInfo->getWeaponList(0));
	//}

	if (VT_DEFAULT == m_currentViewType || g_pGameContext->GetMySlotIdxEx() == idxSlot)
	{
		g_pGameContext->SetMyCharaInfo()->SetWeaponList( pNetCharaInfo->getWeaponList(0));

		NET_CHARA_INFO * pMyCharaInfo = (NET_CHARA_INFO*) g_pGameContext->GetMyCharaInfo();
		g_pGameContext->CheckWeaponList( pMyCharaInfo->getWeaponList(0) );
	}

	TItemID ui32WeaponItemID = 0;

	//	weapons
	for(INT32 i=0; i<WEAPON_SLOT_COUNT; i++)
	{
		ui32WeaponItemID = pNetCharaInfo->getWeaponItemID( i );
		
		//	네트워크 무기 정보와 무기 컨텍스트의 정보가 다를 경우 싱크를 맞춘다. (배경 캐릭터가 자신일 경우 한함)
		if( m_currentViewType == VT_DEFAULT)
		{
			if( m_pCharacter != NULL && m_pCharacter->getWeaponContext() != NULL &&
				m_pCharacter->getWeaponContext()->getWeapon(i) != NULL)
			{
				WeaponBase * pWeapon = m_pCharacter->getWeaponContext()->getWeapon(i);
				CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

				UINT32 ItemClass = GetItemType_TempFunc(pWeaponInfo->GetTypeClass());
				UINT32 weaponFlag = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ItemClass,
														pWeaponInfo->GetTypeClass(),
														pWeaponInfo->GetNumber());

				
				//// 1.5 Check Weapon
				//if( weaponFlag == pNetCharaInfo->_WeaponList[i])
				//{
				//	bRecreate = TRUE;

				//	m_viewCharaInfo._weapon[i] = pNetCharaInfo->_WeaponList[i];
				//}

				if( weaponFlag == ui32WeaponItemID)
				{
					bRecreate = TRUE;

					m_viewCharaInfo._weapon[i] = ui32WeaponItemID;
				}

			}
		}

		//// 1.5 Check Weapon
		////	네트워크 무기 정보와 배경 캐릭터 무기 정보가 다를 경우 싱크를 맞춘다.
		//if( (pNetCharaInfo->_WeaponList[i] != 0) && (pNetCharaInfo->_WeaponList[i] != m_viewCharaInfo._weapon[i]))
		//{
		//	bRecreate = TRUE;

		//	m_viewCharaInfo._weapon[i] = pNetCharaInfo->_WeaponList[i];
		//}
			
		//	네트워크 무기 정보와 배경 캐릭터 무기 정보가 다를 경우 싱크를 맞춘다.
		if( (ui32WeaponItemID != 0) && (ui32WeaponItemID != m_viewCharaInfo._weapon[i]))
		{
			bRecreate = TRUE;

			m_viewCharaInfo._weapon[i] = ui32WeaponItemID;
		}
	}

	return bRecreate;
}

BOOL CReadyBg::_RecreateDefaultAndSlotEquip(NET_CHARA_INFO* pNetCharaInfo)
{
//	INT32 idxSlot = m_viewCharaInfo._charaInfo.GetNetIdx();
	BOOL bRecreate = FALSE;

	//// 1.5 Check Equip
	//USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
	//if (VT_DEFAULT == m_currentViewType || g_pGameContext->GetMySlotIdxEx() == idxSlot)
	//{
	//	g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);
	//}
	//else
	//{
	//	InfoEquipChara = *g_pGameContext->GetUserInfoEquipmentChara();
	//}

	//InfoEquipChara.CopyTo( &pNetCharaInfo->_EquipCharacter );

	//if (VT_DEFAULT == m_currentViewType || g_pGameContext->GetMySlotIdxEx() == idxSlot)
	//{
	//	InfoEquipChara.CopyTo( &g_pGameContext->SetMyCharaInfo()->_EquipCharacter);
	//}

	////	head
	//if( pNetCharaInfo->_EquipCharacter.GetHairNum() != 0 && pNetCharaInfo->_EquipCharacter.GetHairNum() != m_viewCharaInfo._head)
	//{
	//	bRecreate = TRUE;

	//	m_viewCharaInfo._head = InfoEquipChara.GetHairNum();
	//}

	////	item
	//if( pNetCharaInfo->_EquipCharacter.GetItemNum() != 0xFFFFFFFF && pNetCharaInfo->_EquipCharacter.GetItemNum() != m_viewCharaInfo._item)
	//{
	//	bRecreate = TRUE;

	//	m_viewCharaInfo._item = InfoEquipChara.GetItemNum();
	//}

// 	const CHARA_PARTS_INFO * pPartsInfo = NULL;
// 	
// 	pPartsInfo = g_pFramework->GetCharaInfoContext()->GetCharaPartsInfo(g_pFramework->GetCharaInfoContext()->GetMainCharaIdx());
// 
// 	pPartsInfo->CopyTo(&pNetCharaInfo->_CharacterParts[pNetCharaInfo->_CurrentCharaIdx]);
// 
// 	if (VT_DEFAULT == m_currentViewType || g_pGameContext->GetMySlotIdxEx() == idxSlot)
// 	{
// 		pPartsInfo->CopyTo( &g_pGameContext->SetMyCharaInfo()->_CharacterParts);
// 	}
// 
// 	TItemID ui32Head = pNetCharaInfo->_CharacterParts.GetHeadgearItemID();
// 	
// 	if(ui32Head == 0)
// 		ui32Head = pNetCharaInfo->_CharacterParts.GetFacegearItemID();
// 
// 	if( ui32Head != 0 && ui32Head != m_viewCharaInfo._head)
// 	{
// 		bRecreate = TRUE;
// 
// 		m_viewCharaInfo._head = ui32Head;
// 	}
// 
// 	//	item
// 	TItemID ui32Item = pNetCharaInfo->_CharacterParts.GetItemItemID();
// 	if( ui32Item != 0xFFFFFFFF && ui32Item != m_viewCharaInfo._item)
// 	{
// 		bRecreate = TRUE;
// 
// 		m_viewCharaInfo._item = ui32Item;
// 	}

	return bRecreate;
}

BOOL CReadyBg::_RecreateOtherWeapon(VIEW_CHARA_INFO * pNewCharaInfo)
{
	BOOL bRecreate = FALSE;

	//	weapons
	for(INT32 i=0; i<WEAPON_SLOT_COUNT; i++)
	{
		if( pNewCharaInfo->_weapon[i] != 0 && pNewCharaInfo->_weapon[i] != m_viewCharaInfo._weapon[i] )
		{
			bRecreate = TRUE;

			m_viewCharaInfo._weapon[i] = pNewCharaInfo->_weapon[i];
		}
	}

	return bRecreate;
}

BOOL CReadyBg::_RecreateOtherEquip(VIEW_CHARA_INFO * pNewCharaInfo)
{
	BOOL bRecreate = FALSE;

	//	head
	if( pNewCharaInfo->_head != 0 && pNewCharaInfo->_head != m_viewCharaInfo._head)
	{
		bRecreate = TRUE;

		m_viewCharaInfo._head = pNewCharaInfo->_head;
	}

	//	item
	if( pNewCharaInfo->_item != 0xFFFFFFFF && pNewCharaInfo->_item != m_viewCharaInfo._item)
	{
		bRecreate = TRUE;

		m_viewCharaInfo._item = pNewCharaInfo->_item;
	}

	return bRecreate;
}

void CReadyBg::OnChangeVideo( i3VideoInfo * pInfo)
{

}

