#include "pch.h"

#include "UIHudDomi_Indicator.h"
#include "../../Mode/Domination/GameMode_Domination.h"
#include "../../Mode/Domination/DominationSkillSystem.h"

#include "../../BattleHUD.h"
#include "../../ConfigRenderBattle.h"
#include "../UIMath.h"
#include "BattleSlotContext.h"

namespace UI_Util
{
	//---------------------
	//CIndicate HUD class
	//---------------------
	void CIndicateHUD::OnCreate( i3AttrSet* pHUDRoot, i3Texture* pTexture)
	{
		m_pSprite = i3Sprite2D::new_object();
		m_pSprite->SetTexture( pTexture);
		m_pSprite->Create( WAVE_MISSION_MAX, true, true);
		m_pSprite->SetEnable( false);

		for( UINT8 cnt = 0; cnt < WAVE_MISSION_MAX; cnt++)
		{
			m_pSprite->SetRect( cnt, 0.0f, 0.0f, 47.0f, 47.0f);
			m_pSprite->SetColor( cnt, 255, 255, 255, 255);
			m_pSprite->SetEnable( cnt, false );
		}

		m_pSprite->SetTextureCoord( WAVE_MISSION_ELIMINATE,	482.f, 290.f, 528.f, 336.f );
		m_pSprite->SetTextureCoord( WAVE_MISSION_RECOVERY,	430.f, 392.f, 476.f, 438.f );
		m_pSprite->SetTextureCoord( WAVE_MISSION_RECOVERY2, 482.f, 392.f, 528.f, 438.f );
		m_pSprite->SetTextureCoord( WAVE_MISSION_BOMB,		430.f, 341.f, 476.f, 388.f );
		m_pSprite->SetTextureCoord( WAVE_MISSION_DESTROY,	534.f, 442.f, 582.f, 498.f );
		m_pSprite->SetTextureCoord( WAVE_MISSION_DEFENCE,	482.f, 341.f, 528.f, 388.f );
		m_pSprite->SetTextureCoord( WAVE_MISSION_BLOCK	,	430.f, 442.f, 476.f, 498.f );


		m_pText	= i3TextNodeDX2::new_object();
		m_pText->Create( BATTLE_CHARA_SET, FONT_COUNT_32, 15, FW_NORMAL, false, false, ANTIALIASED_QUALITY);
		m_pText->setSize( 70, 50);
		m_pText->SetEnable( false);

		pHUDRoot->AddChild( m_pSprite);
		pHUDRoot->AddChild( m_pText);
	}

	void CIndicateHUD::SetEnable( bool bEnable, INT32 iType /*=0*/)
	{
		if( bEnable) m_pSprite->SetEnable( iType, true);
		else for( UINT8 cnt = 0; cnt < WAVE_MISSION_MAX; ++cnt) m_pSprite->SetEnable( cnt, false);

		m_pText->SetEnable( bEnable);
	}

	/*virtual*/ void CIndicateObject::Create( i3GameObj* pObj, INT32 iType)
	{
		m_pObj = pObj;
		m_nHudType = iType;
		m_pHud->SetEnable( false);
		m_pHud->SetEnable( true, m_nHudType);

		_Init();
	}
	//InstallObject class
	/*virtual*/ bool CIndicateInstallObject::IsSame( INT32 idx, INT32 nObjectType)
	{
		if( nObjectType != INSTALL_OBJECT) return false;

		WeaponBase* pWeapon = static_cast<WeaponBase*>(m_pObj);
		if( pWeapon == nullptr) return false;

		return (pWeapon->getNetworkIdx() == idx);
	}

	//StageObject class
	/*virtual*/ bool CIndicateStageObject::IsSame( INT32 idx, INT32 nObjectType)
	{
		if( nObjectType != STAGE_OBJECT) return false;

		i3Object* pStageObject = static_cast<i3Object*>(m_pObj);
		if( pStageObject == nullptr) return false;

		return (pStageObject->getNetIndex() == idx);
	}

	//RespawnObject class
	/*virtual*/ bool CIndicateRespawnObject::IsSame( INT32 idx, INT32 nObjectType)
	{
		if( nObjectType != RESPAWN_OBJECT) return false;

		i3RespawnObj* pRespawnObject = static_cast<i3RespawnObj*>(m_pObj);
		if( pRespawnObject == nullptr) return false;

		i3RespawnObj* p = reinterpret_cast<i3RespawnObj*>(idx);

		return (pRespawnObject == p);
	}

	//Character class
	/*virtual*/ bool CIndicateCharacter::IsSame( INT32 idx, INT32 nObjectType)
	{
		if( nObjectType != CHARACTER_OBJECT) return false;

		CGameCharaBase* pCharacter = static_cast<CGameCharaBase*>(m_pObj);
		if( pCharacter == nullptr) return false;
		
		//pCharacter Info 가 Null 일수도있어서..
		if( m_nNetIdx == idx) return true;
		
		return (pCharacter->getCharaNetIndex() == idx);
	}
	/*virtual*/ void CIndicateCharacter	::_Init()
	{
		CGameCharaBase* pCharacter = static_cast<CGameCharaBase*>(m_pObj);
		m_nNetIdx = pCharacter->getCharaNetIndex();
	}

	//Dot class
	/*virtual*/ bool CIndicateDot::IsSame( INT32 idx, INT32 nObjectType)
	{
		if( nObjectType != DOT_OBJECT) return false;
		return (m_nIdx == idx);
	}

	/*virtual*/ bool CIndicateMeatBomb::IsSame( INT32 netIdx, INT32 nObjectType)
	{
		if( nObjectType != MEATBOMB) return false;

		return (m_networkIdx == netIdx);
	}
};


I3_CLASS_INSTANCE(CUIHudDomi_Indicator);//, UIHUDDominationBase);

CUIHudDomi_Indicator::CUIHudDomi_Indicator(void) : m_pDomination( nullptr ),
	m_pHUDRoot( nullptr ), m_pTexture( nullptr ), m_dot_count(0)
{
}
CUIHudDomi_Indicator::~CUIHudDomi_Indicator(void)
{
	_ClearIndications();					//먼저 호출.

	i3::vector<UI_Util::CIndicateHUD*>::iterator itrHud = m_vecFreeHuds.begin();
	while( itrHud != m_vecFreeHuds.end())	//삭제.
	{
		I3_SAFE_DELETE( *(itrHud++));
	}


	m_vecFreeHuds.clear();
	m_vecIndications.clear();
	m_dot_idx_list.clear();

	I3_SAFE_RELEASE( m_pTexture );
	I3_SAFE_NODE_RELEASE( m_pHUDRoot );
}


void CUIHudDomi_Indicator::OnCreate( i3GameNode* pParent )
{
	UIHUDDominationBase::OnCreate( pParent );

	m_pDomination	= (gmode_domination*) gmode::i();

	// 오브젝트 출력 HUD Root, Texture 생성
	m_pHUDRoot		= i3AttrSet::new_object();
	m_pTexture		= crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3 );

	//32개 정도 미리 만들어 놓는다.
	for( INT8 cnt = 0; cnt < MAX_COUNT_DOMI_DINO; ++cnt)
	{
		UI_Util::CIndicateHUD* pHud = new UI_Util::CIndicateHUD();
		MEMDUMP_NEW( pHud, sizeof( UI_Util::CIndicateHUD));
		pHud->OnCreate( m_pHUDRoot, m_pTexture);

		m_vecFreeHuds.push_back( pHud);	
	}
}

bool CUIHudDomi_Indicator::OnEntranceStart( void* arg1, void* arg2 )
{
	if ( UIHUDDominationBase::OnEntranceStart( arg1, arg2) == false) return false;

	// 반드시 HUD가 생성된 뒤에 해야한다.
	// HudManager 의 Create한다음에 Initialize를 하는 이유가 있다.
	if(CHud::i())
		CHud::i()->GetNode()->AddChild( m_pHUDRoot );

	return true;
}

bool CUIHudDomi_Indicator::OnExitStart(void)
{
	if( UIHUDDominationBase::OnExitStart() == false ) return false;
	
	m_pHUDRoot->RemoveFromParent();
	return true;
}

void CUIHudDomi_Indicator::_DeleteIndication( INT32 idx, INT32 nObjectType)
{
	i3::vector<UI_Util::CIndicateObject*>::iterator itr;
	itr = m_vecIndications.begin();
	while( itr != m_vecIndications.end())
	{
		if( (*itr)->IsSame( idx, nObjectType)) 
		{
			(*itr)->GetHud()->SetEnable( false);			//
			m_vecFreeHuds.push_back( (*itr)->GetHud());		//Hud

			I3_SAFE_DELETE( *itr);
			m_vecIndications.erase( itr);
			return;
		}
		else
		{
			I3TRACE("  -- Can not find Indication Object....(%d)\n", idx);
		}

		++itr;
	}
}

void CUIHudDomi_Indicator::_ClearIndications( tmp_uu::OBJECT_TYPE obj_type /*= tmp_uu::NONE*/)
{
	i3::vector<UI_Util::CIndicateObject*>::iterator itr = m_vecIndications.begin();

	tmp_uu::CIndicateObject* ui_obj = nullptr;
	while( itr != m_vecIndications.end())
	{
		ui_obj = *itr;
		if( obj_type == tmp_uu::NONE || ui_obj->GetObjectType() == obj_type)
		{
			m_time_event_list.pop_event( ui_obj->event_ptr);

			ui_obj->GetHud()->SetEnable( false);			//
			m_vecFreeHuds.push_back( ui_obj->GetHud());		//Hud

			I3_SAFE_DELETE( ui_obj);
			itr = m_vecIndications.erase( itr);

			continue;
		}
		++itr;
	}

	if( obj_type == tmp_uu::DOT_OBJECT)
		m_dot_idx_list.clear();
}

UI_Util::CIndicateObject* CUIHudDomi_Indicator::InsertIndication( i3GameObj* pObject, INT32 nHudType, INT32 nObjectType)
{
	if( m_vecFreeHuds.empty())
	{
		I3TRACE("  -- Run out Indicate Hud..\n");
		return nullptr;
	}

	i3::vector<UI_Util::CIndicateHUD*>::iterator itrHud = m_vecFreeHuds.begin();	

	UI_Util::CIndicateObject* pIndicate = nullptr;
	switch( nObjectType)
	{
	case UI_Util::INSTALL_OBJECT:	
		pIndicate = new UI_Util::CIndicateInstallObject( *itrHud); 
		MEMDUMP_NEW( pIndicate, sizeof(UI_Util::CIndicateInstallObject) );
		break;
	case UI_Util::STAGE_OBJECT:	
		pIndicate = new UI_Util::CIndicateStageObject( *itrHud);   
		MEMDUMP_NEW( pIndicate, sizeof(UI_Util::CIndicateStageObject) );
		break;
	case UI_Util::RESPAWN_OBJECT:	
		pIndicate = new UI_Util::CIndicateRespawnObject( *itrHud); 
		MEMDUMP_NEW( pIndicate, sizeof(UI_Util::CIndicateRespawnObject) );
		break;
	case UI_Util::CHARACTER_OBJECT:
		pIndicate = new UI_Util::CIndicateCharacter( *itrHud);
		MEMDUMP_NEW( pIndicate, sizeof(UI_Util::CIndicateCharacter) );
		break;
	case tmp_uu::DOT_OBJECT:
		pIndicate = new UI_Util::CIndicateDot( *itrHud);
		MEMDUMP_NEW( pIndicate, sizeof(UI_Util::CIndicateDot) );
		break;
	case tmp_uu::MEATBOMB:
		pIndicate = new UI_Util::CIndicateMeatBomb( *itrHud);
		MEMDUMP_NEW( pIndicate, sizeof(UI_Util::CIndicateMeatBomb) );
		break;
	default:										
		I3ASSERT_0;
		break;
	}
	if( pIndicate == nullptr) return nullptr;
	
	pIndicate->Create( pObject, nHudType);
	m_vecIndications.push_back( static_cast<UI_Util::CIndicateObject*>(pIndicate));	

	m_vecFreeHuds.erase( itrHud);

	return pIndicate;
}

UI_Util::CIndicateObject* CUIHudDomi_Indicator::Find( INT32 Idx, INT32 nObjectType)
{
	tmp_uu::CIndicateObject* obj = nullptr;

	i3::vector<UI_Util::CIndicateObject*>::iterator itr;
	itr = m_vecIndications.begin();
	while( itr != m_vecIndications.end())
	{
		obj = *itr;
		if( obj->IsSame( Idx, nObjectType)) 
			return obj;

		itr++;
	}

	return nullptr;
}

void CUIHudDomi_Indicator::OnUpdate( REAL32 rDeltaSec )
{
	UIHUDDominationBase::OnUpdate( rDeltaSec);

	//_FindInstallObject();

	m_time_event_list.OnUpdate( rDeltaSec);

	i3::vector<UI_Util::CIndicateObject*>::iterator itr;
	itr = m_vecIndications.begin();
	while( itr != m_vecIndications.end())
	{	
		UI_Util::CIndicateHUD* pHud = (*itr)->GetHud();
		INT32 nType = (*itr)->GetHudType();

		VEC3D vPos;
		i3Vector::Copy( &vPos, (*itr)->GetObjPos());
	
		switch( nType)
		{
		case	WAVE_MISSION_RECOVERY	:	i3Vector::AddY( &vPos, 1.f);		break;
		case	WAVE_MISSION_RECOVERY2	:	i3Vector::AddY( &vPos, 1.7f);		break;
		case	WAVE_MISSION_DESTROY	:	i3Vector::AddY( &vPos, 2.0f);		break;
		case	WAVE_MISSION_DEFENCE	:	i3Vector::AddY( &vPos, 2.0f);		break;
		case	WAVE_MISSION_ELIMINATE	:	i3Vector::AddY( &vPos, 1.f);		break;
		default:							i3Vector::AddY( &vPos, 0.f);		break;
		}

		(*itr)->m_IdcCur = _ProcessIndicate( &vPos, pHud);

		if( pHud->m_vPrePos != pHud->m_vPos)
		{
			pHud->m_fInterpolateTime += (rDeltaSec * 1.5f);
			if( pHud->m_fInterpolateTime >= 1.f)
				pHud->m_fInterpolateTime = 1.f;

			pHud->m_vPrePos.x = uiMath::interpolate( pHud->m_fInterpolateTime, (REAL32)pHud->m_vPrePos.x, (REAL32)pHud->m_vPos.x);
			pHud->m_vPrePos.y = uiMath::interpolate( pHud->m_fInterpolateTime, (REAL32)pHud->m_vPrePos.y, (REAL32)pHud->m_vPos.y);
		}

		pHud->SetRect( nType, pHud->m_vPrePos.x, pHud->m_vPrePos.y, pHud->m_fSize, pHud->m_fSize);

		i3::stack_wstring	wstrTemp;
		i3::sprintf( wstrTemp, L"%.1f M", pHud->m_fLengh);
		pHud->SetText( wstrTemp);

		if ((*itr)->GetObjectType() == UI_Util::MEATBOMB)
		{
			UI_Util::CIndicateMeatBomb* mb = static_cast<UI_Util::CIndicateMeatBomb*>(*itr);
			i3TextNodeDX2* textExplosionCount = mb->GetExplosionCountText();
			
			i3::stack_wstring	wstrTemp2;
			REAL32 explosionRemainTime = mb->DecreaseExplosionRemainTime(rDeltaSec);
			i3::sprintf( wstrTemp2, L"%1d", static_cast<INT32>(explosionRemainTime)+1);
			textExplosionCount->SetText( wstrTemp2);

			textExplosionCount->setPos(static_cast<UINT32>(pHud->m_vPrePos.x + 19.0f), 
				static_cast<UINT32>(pHud->m_vPrePos.y - 20.0f));

			if (textExplosionCount->GetEnable() == false)
				textExplosionCount->SetEnable(true);
		}

		++itr;
	}
}

//=========================================
// 이벤트 함수들
//=========================================
bool CUIHudDomi_Indicator::SetHudEvent( INT32 nEventNum, INT32 arg )
{
	if( nEventNum < 0 || nEventNum >= HUD_DMN_EVT_OBJ_MAX )
	{
		// 잘못된 이벤트
		I3TRACE("Wron Event - CUIHudDomi_Indicator\n");
		return false;
	}

	switch( nEventNum)
	{
	case HUD_DMN_EVT_CLEAR_INDICATIONS:		//모든 Indication UI 지움.
		_ClearIndications( static_cast<tmp_uu::OBJECT_TYPE>(arg));
		break;

	case HUD_DMN_EVT_DELETE_INSTALL_OBJECT:
		_DeleteIndication( arg, UI_Util::INSTALL_OBJECT); //(arg : Weapon NetIdx)
		break;

	case HUD_DMN_EVT_DELETE_STAGE_OBJECT:
		_DeleteIndication( arg, UI_Util::STAGE_OBJECT); //(arg : Object NetIdx)
		break;

	case HUD_DMN_EVT_DELETE_CHARACTER_OBJECT:
		_DeleteIndication( arg, UI_Util::CHARACTER_OBJECT); //(arg : Chara NetIdx)
		break;

	case HUD_DMN_EVT_DELETE_RESPAWN_OBJECT:
		_DeleteIndication( arg, UI_Util::RESPAWN_OBJECT); //(arg : Respawn Object*)
		break;

	case HUD_DMN_EVT_DELETE_MEATBOMB:
		_DeleteIndication( arg, tmp_uu::MEATBOMB);
		break;
	}

	return false;
}


//=========================================
// 기타 함수들
//=========================================
bool CUIHudDomi_Indicator::_CalcProjectionPos( REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn)
{
	VEC3D	vTemp;
	VEC3D	vTempIn;
	MATRIX	CamMat;
	VEC3D*	pvCamAt;
	VEC3D	vSub;


	pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

	i3Vector::Sub( &vSub, i3Matrix::GetPos( g_pCamera->getCamMatrix()), vIn);
	i3Vector::Normalize( &vSub, &vSub );
   
	REAL32 rDot = i3Vector::Dot( pvCamAt, &vSub );
	if( rDot <= 0.0f)
	{
		return false;
	}

	
	i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

	i3Vector::Copy( &vTempIn, vIn);
	i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );


	*fOutX	= vTemp.x * 0.5f + 0.5f;
	*fOutY	= vTemp.y * -0.5f + 0.5f;

  
	return true;
}

tmp_uu::IDC_STATE CUIHudDomi_Indicator::_ProcessIndicate( VEC3D* pPos, UI_Util::CIndicateHUD* pHud)
{
	REAL32 X, Y;
	X = Y = -100.f;
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	i3Viewer*	pViewer			= g_pFramework->GetViewer();
	INT32		nScreenWidth	= pViewer->GetViewWidth();
	INT32		nScreenHeight	= pViewer->GetViewHeight();

	VEC3D vLength;
	i3Vector::Sub( &vLength, myPlayer->GetPos(), pPos);
	pHud->m_fLengh				= i3Vector::Length( &vLength);

	//REAL32 rate					= 1.1f - (pHud->m_fLengh / 10.0f) * 0.1f;
	pHud->m_fSize				= 46.f;/*MINMAX( 26.5f, 53.0f * rate, 53.0f)*/;
   
	_CalcProjectionPos( &X, &Y, pPos);

	tmp_uu::IDC_STATE re = tmp_uu::IDC_OUT_SCREEN;
	if( X < 0 || X >= 1.f || Y < 0 || Y >= 1.f)
	{
		MATRIX inv, local;
		VEC3D vDir;

		i3Matrix::Inverse( &inv, nullptr, myPlayer->GetMatrix());

		i3Matrix::SetPos( &local, pPos);
		i3Matrix::Mul( &local, &local, &inv);

		i3Vector::Copy( &vDir, i3Matrix::GetPos( &local));
		REAL32 fTheta = i3Math::atan2( vDir.x, vDir.z);

		REAL32 x1 = (nScreenWidth * 0.45f) * i3Math::sin( fTheta);
		REAL32 y1 = (nScreenHeight * 0.45f) * i3Math::cos( fTheta);


		pHud->m_vPos.x = (nScreenWidth * 0.5f) - (x1 + pHud->m_fSize * 0.5f);
		pHud->m_vPos.y = (nScreenHeight * 0.5f) - (y1 + pHud->m_fSize * 0.5f);

		if( pHud->m_bInScreen == true) pHud->m_fInterpolateTime = 0.f;
		pHud->m_bInScreen = false;

		re = tmp_uu::IDC_OUT_SCREEN;
	}
	else
//#endif
	{
		pHud->m_vPos.x = nScreenWidth * X - pHud->m_fSize * 0.5f;
		pHud->m_vPos.y = nScreenHeight * Y - pHud->m_fSize * 0.5f;

		if( pHud->m_bInScreen == false) pHud->m_fInterpolateTime = 0.f;
		pHud->m_bInScreen = true;

		re = tmp_uu::IDC_IN_SCREEN;
	}

	return re;
}

void CUIHudDomi_Indicator::cb_time_event( void* p1, void* p2)
{
	//UI_Util::CIndicateObject* ui_obj = (tmp_uu::CIndicateObject*)(p2);
	//I3ASSERT( ui_obj != nullptr);

	i3::vector<UI_Util::CIndicateObject*>::iterator itr = m_vecIndications.begin();
	for( ; itr != m_vecIndications.end(); ++itr)
	{
		UI_Util::CIndicateObject* p = (UI_Util::CIndicateObject*)*itr;
		if( p2 == p)
		{
			tmp_uu::CIndicateDot* ui_dot = static_cast<tmp_uu::CIndicateDot*>(p);


			i3::vector_map<INT32, VEC3D>::iterator _dot = m_dot_idx_list.find( ui_dot->GetDotIdx());
			if( _dot != m_dot_idx_list.end())
				m_dot_idx_list.erase( _dot);


			(*itr)->GetHud()->SetEnable( false);			//
			m_vecFreeHuds.push_back( (*itr)->GetHud());		//Hud

			I3_SAFE_DELETE( *itr);
			m_vecIndications.erase( itr);

			break;
		}
	}
}

