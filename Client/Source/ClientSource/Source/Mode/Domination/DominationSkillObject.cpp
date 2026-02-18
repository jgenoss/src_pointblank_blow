#include "pch.h"
#include "DominationSkillObject.h"

#include "DominationSkillSystem.h"
#include "../../../Common/CommonSource/CommonDediCli.h"
#include "../../UI/UIBattleFrame.h"

#include "../../BattleHud.h"
#include "DomiSkillObjBulletinBoard.h"
#include "DomiSkillProperties.h"
#include "BattleSlotContext.h"

namespace
{
	bool __CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn )
	{
		VEC3D	vTemp;
		VEC3D	vTempIn;
		MATRIX	CamMat;
		VEC3D*	pvCamPos;
		VEC3D*	pvCamAt;
		VEC3D	vSub;
		REAL32	Height = 1.9f;

		pvCamPos = i3Matrix::GetPos( g_pCamera->getCamMatrix());
		pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

		i3Vector::Sub( &vSub, pvCamPos, pIn);
		i3Vector::Normalize( &vSub, &vSub );

		if( i3Vector::Dot( pvCamAt, &vSub) < 0.f ) 
			return false;

		i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

		i3Vector::Set( &vTempIn, i3Vector::GetX( pIn), i3Vector::GetY( pIn) + Height, i3Vector::GetZ( pIn));

		i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat);

		*pOutX	= i3Vector::GetX( &vTemp );
		*pOutY	= i3Vector::GetY( &vTemp );

		return true;
	}
}

namespace DominationSkill
{
	//----------------------------------------------
	// CIdentity : 스킬 오브젝트 고유 속성
	//----------------------------------------------
	CIdentity::CIdentity(void) : m_ItemID(0), m_TYPE_SLOT(-1), m_ObjIdx(-1)
	{
	}

	CIdentity::~CIdentity(void)
	{
	}

	void CIdentity::Instantiate( i3Chara* pChara)
	{
		MATRIX mat;
		I3_ONINSTANCING_INFO info;
		info._pBase = pChara;
		info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
		
		i3Matrix::SetPos( &mat, 0.f, g_pConfigEx->GetPhysix().HeightWeapon, 0.f);
		
		info._pMatrixStack->Push( &mat, nullptr);
		pChara->OnInstancing( &info);
		info._pMatrixStack->Pop();
	}

	//------------------------------------------------------------------
	// CServiceable : 스킬 오브젝트 상태 및 요청자 SP에 따른 서비스 처리
	//------------------------------------------------------------------
	struct Actor
	{
		virtual ~Actor() {}
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) {}
	};

	struct Actor_None : Actor
	{	
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) {} 
	} g_actor_none;

	struct Actor_Install : Actor 
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			GAMEEVENT::Write_DomiSkillObjectInstall(customerIdx, objectIdx);
		}
	} g_actor_install;

	struct Actor_Upgrade : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			INT8 lv = o->GetWeapon() ? static_cast<INT8>(o->GetWeapon()->getWeaponInfo()->GetLevel()+1) : 0;
			GAMEEVENT::Write_DomiSkillObjectUpgrade(customerIdx, objectIdx, lv);
		}
	} g_actor_upgrade;

	struct Actor_Repair : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			GAMEEVENT::Write_DomiSkillObjectRepair(customerIdx, objectIdx);
		}
	} g_actor_repair;

	struct Actor_Supplement_HP_AMMO : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			GAMEEVENT::Write_DomiSkillObjectSupplyUse(customerIdx, objectIdx);
		}
	} g_actor_supplement_hp_ammo;

	struct Actor_UserSkill_Shop : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer)
		{
			I3TRACE("OPEN USER_SKILL_SHOP\n");
			// E버튼을 누름에 따라 상점 UI 출력
			UIBattleFrame::i()->OpenPopup(UBP_SKILL_SHOP);
		}
	} g_actor_user_skill_shop;

	CServiceable::CServiceable() : m_currState(NONE)
	{
		m_actors[NONE]				= &g_actor_none;
		m_actors[INSTALLABLE]		= &g_actor_install;
		m_actors[UPGRADABLE]		= &g_actor_upgrade;
		m_actors[REPAIRABLE]		= &g_actor_repair;
		m_actors[HP_RECOVERABLE]	= &g_actor_supplement_hp_ammo;
		m_actors[AMMO_RECOVERABLE]	= &g_actor_supplement_hp_ammo;
		m_actors[SHOP_OPEN]			= &g_actor_user_skill_shop;

		m_flagPermitUserIdx.Clear();
	}

	CServiceable::~CServiceable()
	{
		m_currState = NONE;
		m_flagPermitUserIdx.Clear();
	}

	void CServiceable::OnButtonClicked(CGameCharaBase* customer)
	{
		if (customer == nullptr) return;
		if (m_flagPermitUserIdx.TestBit(customer->getCharaNetIndex()) == false) return;

		(*m_actors[m_currState])(this, customer);
	}

	void CServiceable::SetState(UINT32 state)
	{
		if (m_currState != state)
		{
			m_currState = state;
			DomiSkillObjBulletinBoard::i()->Hide();
		}
	}

	//----------------------------------------------
	// CRawObject : 설치 전 투명 스킬 오브젝트
	//----------------------------------------------
	CRawObject::CRawObject(void) : m_pChara(nullptr), m_pAmbientAttr(nullptr), m_Info(nullptr)
	{
	}

	CRawObject::~CRawObject(void)
	{
		I3_SAFE_RELEASE( m_pChara);
	}

	void CRawObject::OnCreate( void* pRes, T_ItemID ItemID, INT32 Type, INT32 Idx)
	{
		CIdentity::OnCreate( pRes, ItemID, Type, Idx);

		i3GameResChara* p = static_cast<i3GameResChara*>(pRes);

		m_pChara = CSkillObjectModel::new_object();
		m_pChara->Create( g_pFramework, p);
		Instantiate( m_pChara);

		{// Alpha
			i3AttrSet* pAttrSet = i3AttrSet::new_object_ref();
			pAttrSet->SetPriority( 0);	

			while( m_pChara->getAttachNode()->GetChildCount() > 0)
			{
				i3Node * pChild = m_pChara->getAttachNode()->GetChild( 0);
				pChild->ChangeParent(pAttrSet);
			}

			m_pChara->getAttachNode()->AddChild( pAttrSet);		

			{
				i3BlendEnableAttr* pAttr = i3BlendEnableAttr::new_object_ref();
				pAttr->Set( true);
				pAttrSet->AddAttr( pAttr);
				pAttr->Set( true);
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
				pAttr->SetSource( I3G_BLEND_SRCALPHA);
				pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
				pAttrSet->AddAttr( pAttr);
			}
			
			{
				i3MaterialAttr* pAttr = i3MaterialAttr::new_object_ref();
				pAttrSet->AddAttr( pAttr);

				COLORREAL col;

				i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
				pAttr->SetDiffuse( &col);
				pAttr->SetSpecular( &col);

				pAttr->setValidEmissive( false);
				pAttr->setValidSpecular( true);
				pAttr->setValidShininess( false);

				i3Color::SetA( pAttr->GetDiffuse(), 0.5f);
				i3Color::SetA( pAttr->GetSpecular(), 0.5f);
			}

			{
				m_pAmbientAttr = i3AmbientAttr::new_object_ref();

				COLORREAL col;
				i3Color::Set( &col, 0.f, 0.7f, 0.f, 1.f);
				m_pAmbientAttr->Set( &col);

				pAttrSet->AddAttr( m_pAmbientAttr);
			}
		}

		i3::vector_map<UINT32, dss::Info*>::iterator itr = CDominationSkillSystem::i()->m_InfoList.find( m_ItemID);
		I3ASSERT( itr != CDominationSkillSystem::i()->m_InfoList.end());

		m_Info = itr->second;
	}

	void CRawObject::OnInstall( const VEC3D* pPos, REAL32 theta, CGameCharaBase* chara /*= nullptr*/)
	{
		m_pChara->SetPos( const_cast<VEC3D*>(pPos));
		m_pChara->AttachScene();
		m_pChara->SetEnable( true);
	}

	VEC3D CRawObject::GetBoundSize()
	{
		VEC3D size;
		m_pChara->getSceneObject()->GetBody()->GetBound()->getSize( &size);
		return size;
	}

	VEC3D CRawObject::GetPos() const
	{
		MATRIX mat;
		GlobalFunc::GetNowFrameMatrix( &mat, m_pChara->getSceneObject());

		VEC3D pos = *(i3Matrix::GetPos( &mat));
		return pos;
	}

	//----------------------------------------------
	// CCompletedObject : 설치된 스킬 오브젝트
	//----------------------------------------------
	CCompletedObject::CCompletedObject(void) : 
	m_pWeapon(nullptr), hud_x(0.f), hud_y(0.f), hud_len(0), hud_hp(nullptr)
#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		, m_lv(0), m_hp(0)
#endif
	{
	}

	CCompletedObject::~CCompletedObject(void)
	{
		I3_SAFE_DELETE( hud_hp);
	}
	
	void CCompletedObject::OnCreate( void* pRes, T_ItemID ItemID, INT32 Type, INT32 Idx)
	{
		CIdentity::OnCreate( pRes, ItemID, Type, Idx);

		WeaponBase* pWeapon = static_cast<WeaponBase*>(pRes);

		m_pWeapon = pWeapon;
		m_pWeapon->setNetworkIdx( static_cast<INT16>(Idx));
		m_pWeapon->setObjectType( static_cast<DOMI_TYPE>(Type));
		m_pWeapon->SetEnable( true);
	}

	void CCompletedObject::OnDestroy()
	{
		m_pWeapon->ReturnInstance();
	}
	
	void CCompletedObject::OnInstall( const VEC3D* pPos, REAL32 theta, CGameCharaBase* chara /*= nullptr*/)
	{
		i3Stage* pStage = g_pFramework->GetCurrentStage();
		i3World* pWolrd = pStage->getWorld();
		I3ASSERT( pWolrd != nullptr);

		m_pWeapon->SetOwner( chara);
		m_pWeapon->InstallToWorld( pWolrd->getSceneObject(), pWolrd, const_cast<VEC3D*>(pPos), true, nullptr, theta);

		InitializeAtInstall( chara);
	}

	VEC3D CCompletedObject::GetBoundSize()
	{
		VEC3D size;
		m_pWeapon->getSceneObject()->GetBody()->GetBound()->getSize( &size);
		return size;
	}

	VEC3D CCompletedObject::GetPos() const
	{
		MATRIX mat;
		GlobalFunc::GetNowFrameMatrix( &mat, m_pWeapon->getSceneObject());

		VEC3D pos = *(i3Matrix::GetPos( &mat));
		return pos;
	}

	void CCompletedObject::InitializeAtInstall( CGameCharaBase* chara /*= nullptr*/)
	{
		m_Nick = L"Lv ";

		//level
		wchar_t buf[16] = {0,};
		i3::vector_map<UINT32, dss::Info*>::iterator itr = CDominationSkillSystem::i()->m_InfoList.find( m_ItemID);
		I3ASSERT( itr != CDominationSkillSystem::i()->m_InfoList.end());
		if( itr != CDominationSkillSystem::i()->m_InfoList.end())
			_itow( itr->second->Level, buf, 10);

#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		m_lv = itr->second->Level;
#endif

		//
		m_Nick += buf;


		//chara nick
		m_owner_nick.clear();
		if( chara != nullptr)
			m_owner_nick = BattleSlotContext::i()->getNickForSlot( chara->getCharaNetIndex());

		//hp hud 생성.
		hud_hp = new dui::skill_hp();
		MEMDUMP_NEW( hud_hp, sizeof( dui::skill_hp));
	}

	bool CCompletedObject::_calc_hud()
	{
		VEC3D pos = GetPos();
		if( __CalcProjectionPos( &hud_x, &hud_y, &pos))
		{
			hud_x *= 0.5f;		hud_x += 0.5f;
			hud_y *= -0.5f;		hud_y += 0.5f;

			CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
			I3ASSERT( player != nullptr);

			VEC3D vTemp;
			i3Vector::Sub( &vTemp, &pos, player->GetPos());
			hud_len = (INT32)i3Vector::Length(&vTemp);
			if( hud_len > 9000 || hud_len < -9000) hud_len = 0;

			return true;
		}
		return false;
	}

	//----------------------------------------------
	// 실제 오브젝트들...
	//----------------------------------------------
	void CDummy::OnUnInstall()
	{	
		if( WeaponDinoDummy* p = i3::same_cast<WeaponDinoDummy*>(m_pWeapon))
			p->Uninstall();

		hud_hp->set_enable( false);
	}

	void CDummy::InitializeAtInstall( CGameCharaBase* chara /*= nullptr*/)
	{
		CCompletedObject::InitializeAtInstall( chara);

		m_Nick += L' ';
		m_Nick.append(m_owner_nick.begin(), m_owner_nick.end());
		m_Nick += L" 's Dummy";

#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		wchar_t hp[256] = {0,};
		INT16 maxHP = domi::SkillProp::i()->GetDummy(m_lv).max_hp->GetVal();
		i3::snprintf(hp, 256, L"[HP : %d]", maxHP);
		m_Nick += hp;
#endif
	}

	void CDummy::ShowHud()
	{
		if( _calc_hud() )
		{
			if( CHud::i() != nullptr)
				CHud::i()->SetCharaNick( m_Nick, true, hud_x, hud_y, (REAL32)hud_len);

			if (GetState() == REPAIRABLE)
			{
				if( WeaponDinoDummy* p = i3::same_cast<WeaponDinoDummy*>(m_pWeapon))
				{
					hud_hp->set_enable( true);
					hud_hp->set_ratio( p->GetHPRatio());
					hud_hp->set_pos( hud_x, hud_y, (REAL32)(hud_len - 15));
				}
			}
			else
				hud_hp->set_enable( false);

#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			WeaponDinoDummy* p = i3::same_cast<WeaponDinoDummy*>(m_pWeapon);
			if (p && m_hp != p->GetHP())
			{
				INT32 i = m_Nick.find(L'[');
				m_Nick.erase(i);
				wchar_t hp[256] = {0,};
				i3::snprintf(hp, 256, L"[HP : %d]", p->GetHP());
				I3TRACE("hp : %d", p->GetHP());
				m_Nick += hp;
				m_hp = p->GetHP();
			}
#endif
		}
		else
		{
			hud_hp->set_enable( false);
		}
	}




	void CSentryGun::OnUnInstall()
	{
		if( WeaponSentryGun* p = i3::same_cast<WeaponSentryGun*>(m_pWeapon))
			p->Uninstall();

		hud_hp->set_enable( false);
	}

	void CSentryGun::InitializeAtInstall( CGameCharaBase* chara /*= nullptr*/)
	{
		CCompletedObject::InitializeAtInstall( chara);

		m_Nick += L' ';
		m_Nick.append(m_owner_nick.begin(), m_owner_nick.end());
		m_Nick += L" 's SentryGun";	

#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		wchar_t hp[256] = {0,};
		INT32 maxHP = domi::SkillProp::i()->GetSentrygun(m_lv).max_hp->GetVal();
		i3::snprintf(hp, 256, L"[HP : %d]", maxHP);
		m_Nick += hp;
#endif
	}

	void CSentryGun::ShowHud()
	{
		if( _calc_hud())
		{
			if(CHud::i() != nullptr)
				CHud::i()->SetCharaNick( m_Nick.c_str(), true, hud_x, hud_y, (REAL32)hud_len);

			if (GetState() == REPAIRABLE)
			{
				if( WeaponSentryGun* p = i3::same_cast<WeaponSentryGun*>(m_pWeapon))
				{
					hud_hp->set_enable( true);
					hud_hp->set_ratio( p->GetHPRatio());
					hud_hp->set_pos( hud_x, hud_y, (REAL32)(hud_len - 15));
				}
			}
			else
				hud_hp->set_enable( false);

#if defined (I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			WeaponSentryGun* p = i3::same_cast<WeaponSentryGun*>(m_pWeapon);
			if (p && m_hp != p->GetHP())
			{
				INT32 i = m_Nick.find(L'[');
				m_Nick.erase(i);
				wchar_t hp[256] = {0,};
				i3::snprintf(hp, 256, L"[HP : %d]", p->GetHP());
				I3TRACE("hp : %d", p->GetHP());
				m_Nick += hp;
				m_hp = p->GetHP();
			}
#endif
		}
		else
		{
			hud_hp->set_enable( false);
		}
	}


	void CSupply::OnUnInstall()
	{
		if( WeaponSupply* p = i3::same_cast<WeaponSupply*>(m_pWeapon))
			p->Uninstall();
	}
};