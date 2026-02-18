#include "pch.h"
#include "GameCameraManager.h"
#include "GlobalVariables.h"
#include "GameCamera_OnObject.h"
#include "BattleSlotContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Scene/i3XrayScene.h"
#include "CrosshairMgr.h"


I3_CLASS_INSTANCE( CGameCameraManager);

CGameCameraManager::~CGameCameraManager(void)
{
	Destory();
	g_pCamera = nullptr;
}
void CGameCameraManager::Destory()
{
	i3::cu::for_each_safe_release_clear(m_CameraList, &CGameCamera::Release); 
	m_pCurCamera = nullptr;
}
void CGameCameraManager::setTargetCharacter( CGameCharaBase * pChar)
{
	CGameCharaBase* pPreviousChara = m_pTargetChar;

	I3_REF_CHANGE( m_pTargetChar ,pChar);

	if (pPreviousChara != nullptr && pPreviousChara != m_pTargetChar)
	{
		if (pPreviousChara->IsEnabledOccludee() == false)
		{
			pPreviousChara->EnableOccludee(true);
			pPreviousChara->EnableRenderAABB(true);

			if (pPreviousChara->GetCurrentCharaWeapon())
			{
				pPreviousChara->GetCurrentCharaWeapon()->EnableRenderAABB(true);
			}

		}
	}
	
	if( m_pCurCamera != nullptr)
	{
		m_pCurCamera->InitCamera( m_pTargetChar, m_pCurCamera->getInterpolateTerm() );

		if( i3::generic_is_iequal( m_pCurCamera->GetName(), "FPS") )
		{
			if( m_pTargetChar != nullptr)
			{  
				m_pTargetChar->getDBInfo()->GetFPSCamPos( &m_vFPSBasePosition);

				m_pCurCamera->SetCameraSide(  getX( &m_vFPSBasePosition));
				m_pCurCamera->SetCameraHeight(  getY( &m_vFPSBasePosition));
				m_pCurCamera->SetCameraDist(  getZ( &m_vFPSBasePosition));

				if (m_pTargetChar->IsEnabledOccludee() == true)
				{
					m_pTargetChar->EnableOccludee(false);
					m_pTargetChar->EnableRenderAABB(false);

					if (m_pTargetChar->GetCurrentCharaWeapon())
					{
						m_pTargetChar->GetCurrentCharaWeapon()->EnableRenderAABB(false);
					}
				}
			}
			else
			{
				i3Vector::Copy(&m_vFPSBasePosition, m_pCurCamera->getBasePosition());

			}
		}
		else if( i3::generic_is_iequal( m_pCurCamera->GetName(), "OBSERVER") )
		{
			if( m_pTargetChar == nullptr)
			{
				((CGameCamera_Observer*)m_pCurCamera)->CalculateNoneTargetPosition( pPreviousChara);
				//ChangeCamera( CAMERA_TYPE_KILL);
			}
		}
				
	}
}

CGameCharaBase * CGameCameraManager::getTargetCharacter()
{
	return m_pTargetChar;
}

static CAMERA_TYPE GetCameraNameToEnum(LPCTSTR str)
{
	typedef		std::pair<LPCTSTR, CAMERA_TYPE>		CameraNameEnum;
	CameraNameEnum		CamEnum[] = 
	{
		CameraNameEnum("FPS",		CAMERA_TYPE_FPS),
		CameraNameEnum("FOLLOW",	CAMERA_TYPE_FOLLOW),
		CameraNameEnum("FLY",		CAMERA_TYPE_FLY),
		CameraNameEnum("KILL",		CAMERA_TYPE_KILL),
		CameraNameEnum("OBSERVER",	CAMERA_TYPE_OBSERVER),
		CameraNameEnum("ONOBJECT",	CAMERA_TYPE_ONOBJECT),
		CameraNameEnum("OBSERVER2",	CAMERA_TYPE_OBSERVER2),
	};

	for ( int i = 0 ; i < _countof(CamEnum) ; ++i )
	{
		if ( i3::generic_is_iequal(CamEnum[i].first, str) )
		{
			return CamEnum[i].second;
		}
	}

	return CAMERA_TYPE_INVALID;
}

void CGameCameraManager::Create( i3GameNode * pParentNode, CGameCharaBase * pTargetChar)
{
// 	if( pParentNode != nullptr)
// 	{
// 		pParentNode->AddChild( this);
// 	}

	setTargetCharacter( pTargetChar);

	//Camera Data
	//char	szDefaultCam[64];
	i3::rc_wstring	wstrDefaultCam;
	i3::string		strDefaultCam;
	i3::rc_wstring wstrClassName;
	i3::string		strClassName;


	REAL32	Near = 0.1f, Far = 100.0f;
	REAL32	SidePos = 0.1f, Height = 0.5f, Distance = 0.5f;

	m_CameraList.resize( CAMERA_TYPE_COUNT);

	i3RegKey * pKey = Config::_GetRegRoot("Basic/Camera");
	if( pKey != nullptr)
	{
		FIND_REG_DATA( pKey, "DefaultCam", wstrDefaultCam );

		INT32 i;
		INT32 nCount = pKey->getChildCount();

		// 잠시 주석처리
		//if( nCount > CAMERA_TYPE_COUNT)
		{
			//I3PRINTLOG(I3LOG_FATAL,  "Camera count invalid.");
		//	I3TRACE( "Camera count invalid.\n");
		//	nCount = CAMERA_TYPE_COUNT;
		}

		for( i = 0; i < nCount; ++i)
		{
			i3RegKey * pChildKey = (i3RegKey*)pKey->getChild(i);
			if( pChildKey != nullptr)
			{
				CAMERA_TYPE	TypeIndex = GetCameraNameToEnum(pChildKey->GetName());
				if ( TypeIndex == CAMERA_TYPE_INVALID )
				{
					//I3ASSERT_0;
					continue;
				}

				//Create
				FIND_REG_DATA( pChildKey, "Class", wstrClassName );
				
				i3::utf16_to_mb(wstrClassName, strClassName);

				i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassName);	// i3ClassMeta::FindClassMetaByName( szClassName);
				CGameCamera * pCamera = (CGameCamera*)pMeta->create_instance(); //CREATEINSTANCE( pMeta);

				pCamera->SetName( pChildKey->GetName());

				//FIND_REG_DATA( pChildKey, "FOV",		&Fov);
				FIND_REG_DATA( pChildKey, LS_STR("Near"),		&Near);
				FIND_REG_DATA( pChildKey, LS_STR("Far"),		&Far);
				FIND_REG_DATA( pChildKey, LS_STR("SidePos"),	&SidePos);
				FIND_REG_DATA( pChildKey, LS_STR("Height"),		&Height);
				FIND_REG_DATA( pChildKey, LS_STR("Distance"),	&Distance);

				//pCamera->SetCameraFOV( Fov);
				//pCamera->SetCameraDefaultFOV( Fov);
				if( i3::generic_is_iequal( pCamera->GetName(), "FPS") )
				{
					// FPS카메라에만 Option에서의 FOV값을 적용합니다.
					pCamera->SetCameraFOV( g_pEnvSet->m_fFovValue);
					pCamera->SetCameraDefaultFOV( g_pEnvSet->m_fFovValue);
				}
				else
				{
					pCamera->SetCameraFOV( DEF_CAM_FOV);
					pCamera->SetCameraDefaultFOV( DEF_CAM_FOV);
				}
				pCamera->SetCameraNear( Near);
				pCamera->SetCameraFar( Far);
				pCamera->SetCameraSide( SidePos);
				pCamera->SetCameraHeight( Height);
				pCamera->SetCameraDist( Distance);

				if( i3::generic_is_iequal( pCamera->GetName(), "FLY") )
				{
					REAL32 rSpeed = 6.7f;

					FIND_REG_DATA( pChildKey, "MoveSpeed", &rSpeed);
					if (rSpeed == 0.0f)
						rSpeed = 6.7f;
					((CGameCamera_FLY*)pCamera)->setMoveSpeed( rSpeed);

					rSpeed = 0.1f;
					FIND_REG_DATA( pChildKey, "RotateSpeed", &rSpeed);
					if (rSpeed == 0.0f)
						rSpeed = 0.1f;
					((CGameCamera_FLY*)pCamera)->setRotateSpeed( rSpeed);

					rSpeed = 6.7f;
					FIND_REG_DATA( pChildKey, "MoveSpeedMax", &rSpeed);
					if (rSpeed == 0.0f)
						rSpeed = 6.7f;
					((CGameCamera_FLY*)pCamera)->setMoveSpeedMax( rSpeed);
				}

				if( i3::generic_is_iequal( pCamera->GetName(), "FPS") )
				{
					if( pTargetChar != nullptr)
					{
						const CCharaInfo* pCharDB = pTargetChar->getDBInfo();
						if (pCharDB)
							pCharDB->GetFPSCamPos( &m_vFPSBasePosition);

						pCamera->SetCameraSide(  getX( &m_vFPSBasePosition));
						pCamera->SetCameraHeight(  getY( &m_vFPSBasePosition));
						pCamera->SetCameraDist(  getZ( &m_vFPSBasePosition));
					}
					else
					{
						i3Vector::Set( &m_vFPSBasePosition, SidePos, Height, Distance);
					} 
				}


				i3::vu::set_value_force(m_CameraList, TypeIndex, pCamera);				// 확보된 사이즈보다 많을지도 몰라서..다른 함수로처리..
				I3_MUST_ADDREF( pCamera);
			}
		}

		i3::utf16_to_mb(wstrDefaultCam, strDefaultCam);
		ChangeCamera( strDefaultCam.c_str(), 0.0f);
	}
}

void CGameCameraManager::RespawnCameraMgr()
{
	
	const size_t nCount = m_CameraList.size();

	for(size_t i = 0; i < nCount; ++i)
	{
		CGameCamera * pCam = m_CameraList[i];
		pCam->RespawnCamera();
	}
}

void CGameCameraManager::ApplyEnvSetFOV(void)
{

	const size_t nCount = m_CameraList.size();

	for(size_t i=0; i<nCount; i++)
	{
		CGameCamera* pCam = m_CameraList[i];

		if( i3::same_of<CGameCamera_FPS*>(pCam))
		{
			pCam->SetCameraFOV( g_pEnvSet->m_fFovValue);
			pCam->SetCameraDefaultFOV( g_pEnvSet->m_fFovValue);

			return;
		}
	}
}

void CGameCameraManager::ResetCameraMgr()
{
	
	const size_t nCount = m_CameraList.size();

	for(size_t i = 0; i < nCount; ++i)
	{
		CGameCamera * pCam = m_CameraList[i];
		pCam->ResetCamera();
	}

	m_pOldCamera = nullptr;
}

void CGameCameraManager::SetStageCamFar( const char * pszStageName)
{
	REAL32	rStageCamFar = 0.0f;
	{
		i3RegKey * pKey = Config::_GetRegRoot( "Stage/Stage");
		if( pKey != nullptr)
		{
			i3RegKey * pStageKey = Config::FindKey( pszStageName, pKey);
			if( pStageKey == nullptr)
			{
				return;
			}

			FIND_REG_DATA( pStageKey, "CamFar", &rStageCamFar);
		}
	}


	const size_t nCount = m_CameraList.size();
	for(size_t i = 0; i < nCount; ++i)
	{
		CGameCamera * pCam = m_CameraList[i];
		if( pCam != nullptr)
		{
			pCam->SetCameraFar( rStageCamFar);
		}
	}
}

CGameCamera * CGameCameraManager::ChangeCamera( const char * pszCamName, REAL32 rTime)
{
	I3ASSERT( pszCamName != nullptr);

	
	const size_t nCount = m_CameraList.size();
	for(size_t i = 0; i < nCount; ++i)
	{
		CGameCamera * pCam = m_CameraList[i];
		if( i3::generic_is_iequal( pCam->GetName(), pszCamName) )
		{
			if( m_pTargetChar != nullptr)
			{
				pCam->InitCamera( m_pTargetChar, rTime);
				m_pCurCamera = pCam; 

				if( i3::same_of<CGameCamera_FPS*>(pCam))
				{
					m_pTargetChar->getDBInfo()->GetFPSCamPos( &m_vFPSBasePosition);

					pCam->SetCameraSide(  getX( &m_vFPSBasePosition));
					pCam->SetCameraHeight(  getY( &m_vFPSBasePosition));
					pCam->SetCameraDist(  getZ( &m_vFPSBasePosition));
				}
			}
			else
			{
				if( i3::same_of<CGameCamera_FPS*>(pCam))
				{
					if (pCam->getCameraObject() == nullptr)
						pCam->InitCamera(nullptr,rTime);
					i3Vector::Copy(&m_vFPSBasePosition, pCam->getBasePosition());
				}
				else
				{
					// I3ASSERT( i3::kind_of<CGameCamera_FLY*>(pCam));
					pCam->InitCamera( nullptr, rTime);
					m_pCurCamera = pCam; 
				}
			}
		}
	}

	
	//FLY카메라에서는 XRay 켜준다
	if (i3::same_of<CGameCamera_FLY*>(m_pCurCamera))
	{
		//FLY카메라에서는 XRay 켜준다
		if (g_pFramework->getSgContext()->GetXrayScene()->IsXrayState())
			g_pCharaManager->SetXrayState(getTargetCharacter(), true);
	}
	else
	{
		g_pCharaManager->SetXrayState(getTargetCharacter(), false);
	}


	g_pFramework->getSgContext()->setCurrentCamera( m_pCurCamera->getCameraObject());
	m_pOldCamera = g_pCamera;
	g_pCamera = m_pCurCamera;

	//TraceCamera();

	return m_pCurCamera;
}

void CGameCameraManager::TraceCamera()
{
	if(i3::same_of<CGameCamera_FLY*>(g_pCamera))
	{
		GlobalFunc::PB_TRACE("CGameCamera_FLY \n");
	}
	else if(i3::same_of<CGameCamera_Follow*>(g_pCamera))
	{
		GlobalFunc::PB_TRACE("CGameCamera_Follow \n");
	}
	else if(i3::same_of<CGameCamera_FPS*>(g_pCamera))
	{
		GlobalFunc::PB_TRACE("CGameCamera_FPS \n");
	}
	else if(i3::same_of<CGameCamera_Kill*>(g_pCamera))
	{
		GlobalFunc::PB_TRACE("CGameCamera_Kill \n");
	}
	else if(i3::same_of<CGameCamera_Observer*>(g_pCamera))
	{
		GlobalFunc::PB_TRACE("CGameCamera_Observer \n");
	}
	else if(i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		GlobalFunc::PB_TRACE("CGameCamera_OnObject \n");
	}
}

CGameCamera * CGameCameraManager::ChangeToOldCamera(REAL32 rTime)
{
	if(m_pOldCamera == nullptr)
	{
		return nullptr;
	}

	m_pCurCamera = m_pOldCamera;
	g_pCamera = m_pOldCamera;

	//TraceCamera();

	return m_pCurCamera;
}

CGameCamera * CGameCameraManager::ChangeCamera( CAMERA_TYPE nCameraType, REAL32 rTime)
{
	//CHAR_SOLO_RENDER
	/*if (nCameraType == CAMERA_TYPE_FPS)
		g_pViewer->SetDepthClear1PV(true);
	else
		g_pViewer->SetDepthClear1PV(false);*/

	I3ASSERT( (INT32)nCameraType < (INT32)m_CameraList.size());

	CGameCamera * pCam = m_CameraList[nCameraType];
	if( m_pTargetChar != nullptr)
	{
		pCam->InitCamera( m_pTargetChar, rTime);

		if( nCameraType == CAMERA_TYPE_FPS )
		{
			m_pTargetChar->getDBInfo()->GetFPSCamPos( &m_vFPSBasePosition);

			pCam->SetCameraSide(  getX( &m_vFPSBasePosition));
			pCam->SetCameraHeight(  getY( &m_vFPSBasePosition));
			pCam->SetCameraDist(  getZ( &m_vFPSBasePosition));
		}
	}
	else
	{
		if( nCameraType == CAMERA_TYPE_FPS)
		{
			i3Vector::Copy(&m_vFPSBasePosition, pCam->getBasePosition());
		}
		else
		{
			pCam->InitCamera( nullptr, rTime);
		}
	}

	m_pCurCamera = pCam;


	if (i3::same_of<CGameCamera_FLY*>(m_pCurCamera))
	{
		//FLY카메라에서는 XRay 켜준다
		if (g_pFramework->getSgContext()->GetXrayScene()->IsXrayState())
			g_pCharaManager->SetXrayState(getTargetCharacter(), true);
	}
	else
	{
		g_pCharaManager->SetXrayState(getTargetCharacter(), false);
	}

	g_pFramework->getSgContext()->setCurrentCamera( m_pCurCamera->getCameraObject());
	m_pOldCamera = g_pCamera;
	g_pCamera = m_pCurCamera;

	CrosshairMgr::i()->enableCrossHair(false);

	//TraceCamera();

	return m_pCurCamera;
}


//카메라의 속성 Reload - 새로이 생성하거나 지우는 경우에대해선 처리하지 않았습니다.
//순서대로 업데이트 하기때문에 중간에 끼워 놓는경우나 지워서 서로 맞지않는경우
//문제가 생길수 있으므로, 지우거나 중간에 끼워 넣은경우에는 게임을 다시 시작하시기 바랍니다.
#if !defined( I3_NO_PROFILE)
void CGameCameraManager::OnReloadProperty()
{
	//I3ASSERT( g_pRegistry != nullptr);

	i3::rc_wstring	wstrDefaultCam;
	i3::string		strDefaultCam;

	//REAL32	Fov, Near, Far;
	REAL32	Near = 0.1f, Far = 100.0f;
	REAL32	SidePos = 0.1f, Height = 0.5f, Distance = 0.5f;

	i3RegKey * pKey = Config::_GetRegRoot("Basic/Camera");
	if( pKey != nullptr)
	{
		FIND_REG_DATA( pKey, "DefaultCam", wstrDefaultCam );

		INT32 i;
		INT32 nCount = pKey->getChildCount();
		for( i = 0; i < nCount; ++i)
		{
			i3RegKey * pChildKey = (i3RegKey*)pKey->getChild(i);
			if( pChildKey != nullptr)
			{
				CAMERA_TYPE	TypeIndex = GetCameraNameToEnum(pChildKey->GetName());
				if ( TypeIndex == CAMERA_TYPE_INVALID )
				{
					I3ASSERT_0;
					continue;
				}

				CGameCamera * pCam = m_CameraList[TypeIndex];
				if( i3::generic_is_iequal( pCam->GetName(), pChildKey->GetName()) )
				{
					//FIND_REG_DATA( pChildKey, "FOV",		&Fov);
					FIND_REG_DATA( pChildKey, LS_STR("Near"),		&Near);
					FIND_REG_DATA( pChildKey, LS_STR("Far"),		&Far);
					FIND_REG_DATA( pChildKey, LS_STR("SidePos"),	&SidePos);
					FIND_REG_DATA( pChildKey, LS_STR("Height"),		&Height);
					FIND_REG_DATA( pChildKey, LS_STR("Distance"),	&Distance);

					//pCam->SetCameraFOV( Fov);
					//pCam->SetCameraDefaultFOV( Fov);
					if( i3::generic_is_iequal( pCam->GetName(), "FPS") )
					{
						// FPS카메라에만 Option에서의 FOV값을 적용합니다.
						pCam->SetCameraFOV( g_pEnvSet->m_fFovValue);
						pCam->SetCameraDefaultFOV( g_pEnvSet->m_fFovValue);
					}
					else
					{
						pCam->SetCameraFOV( DEF_CAM_FOV);
						pCam->SetCameraDefaultFOV( DEF_CAM_FOV);
					}

					pCam->SetCameraNear( Near);
					pCam->SetCameraFar( Far);
					pCam->SetCameraSide( SidePos);
					pCam->SetCameraHeight( Height);
					pCam->SetCameraDist( Distance);

					if( i3::generic_is_iequal( pCam->GetName(), "FPS") )
					{
						if( m_pTargetChar != nullptr)
						{
							m_pTargetChar->getDBInfo()->GetFPSCamPos( &m_vFPSBasePosition);

							pCam->SetCameraSide(  getX( &m_vFPSBasePosition));
							pCam->SetCameraHeight(  getY( &m_vFPSBasePosition));
							pCam->SetCameraDist(  getZ( &m_vFPSBasePosition));
						}
						else
						{
							i3Vector::Set( &m_vFPSBasePosition, SidePos, Height, Distance);
						}
					}
				}
			}
		}
		
		i3::utf16_to_mb(wstrDefaultCam, strDefaultCam);

		ChangeCamera( strDefaultCam.c_str() , 0.0f);
	}
	
	i3GameNode::OnReloadProperty();
}
#endif

void CGameCameraManager::OnUpdate( REAL32 rDeltaSeconds)
{
	if( m_pCurCamera != nullptr)
	{
		m_pCurCamera->OnUpdate( rDeltaSeconds);
	}

	i3GameNode::OnUpdate( rDeltaSeconds);

#ifdef I3_DEBUG//디버그 키
	i3InputDeviceManager	*	pMng	= g_pViewer->GetInputDeviceManager();
	i3InputKeyboard			*	pKey	= pMng->GetKeyboard();
	if( pKey->GetPressState( I3I_KEY_LCTRL))
	{
		if( pKey->GetPressState( I3I_KEY_HOME))
		{
			g_pEnvSet->m_fFovValue += 1.0f;
			if( g_pEnvSet->m_fFovValue > MAX_CAM_FOV) g_pEnvSet->m_fFovValue = MAX_CAM_FOV;

			CGameCamera * pCam = m_CameraList[0];
			pCam->SetCameraFOV( g_pEnvSet->m_fFovValue);
			pCam->SetCameraDefaultFOV( g_pEnvSet->m_fFovValue);
		}

		if( pKey->GetPressState( I3I_KEY_END))
		{			
			g_pEnvSet->m_fFovValue -= 1.0f;
			if( g_pEnvSet->m_fFovValue < MIN_CAM_FOV) g_pEnvSet->m_fFovValue = MIN_CAM_FOV;

			CGameCamera * pCam = m_CameraList[0];
			pCam->SetCameraFOV( g_pEnvSet->m_fFovValue);
			pCam->SetCameraDefaultFOV( g_pEnvSet->m_fFovValue);
		}
	}	
#endif
}

void CGameCameraManager::OnChangeResolution( i3Viewer * pViewer)
{
	CGameCamera * pCam;

	UINT32 cnt = m_CameraList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		pCam = m_CameraList[i];

		pCam->SetCameraProjection();
	}
}

void CGameCameraManager::InitCameraByGiveupChara( CGameCharaBase * pChara)
{
	if(pChara == nullptr)
	{
		return;
	}

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	UINT32 cnt = m_CameraList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		CGameCamera * pCamera = m_CameraList[i];
		if( pCamera != nullptr )
		{
			if( (pCamera->getCurTarget() != pMyChara) && (pCamera->getCurTarget() == pChara)
#if defined( DEF_OBSERVER_MODE)
				&& pMyChara != nullptr
#endif
				)
			{
				pCamera->InitTremble();
				pCamera->SetTargetChara( nullptr);
				if(g_pCamera == pCamera)
				{
					setTargetCharacter( nullptr);
				}
			}
		}
	}
}

void CGameCameraManager::RemoveTargetChara(CGameCharaBase* pChara)
{
	if(pChara == nullptr)
	{
		return;
	}

	if(pChara == m_pTargetChar) 
	{
		I3_SAFE_RELEASE( m_pTargetChar);
	}

	UINT32 cnt = m_CameraList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		CGameCamera * pCamera = m_CameraList[i];
		if( pCamera != nullptr )
		{
			if( pCamera->getCurTarget() == pChara )
			{
				pCamera->SetTargetChara( nullptr);
			}
		}
	}
}
