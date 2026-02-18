#include "pch.h"
#include "DominationSkillSystem.h"

#include "StageBattle.h"
#include "i3Base/smart_ptr/scoped_ptr.h"

#include "../../CommonSource/CommonDediCli.h"
#include "DominationSkillNotice.h"
#include "DominationUserSkillApply.h"

#include "../../ui/hud/UIHUD_Domination_Manager.h"
#include "../../ui/hud/UIHudDomi_Indicator.h"


namespace DominationSkill
{
	template <class B, class D>
	struct Creator
	{
		static B* call() { return new D; }
	};

	class Factory
	{
		typedef dss::CIdentity* (*CREATE_FUNC)();
		CREATE_FUNC m_Func[dss::type_max];

	public:
		Factory()
		{
			ZeroMemory( m_Func, sizeof(m_Func));
			
			m_Func[dss::Dummy_Basis]		= &Creator<dss::CIdentity, dss::CRawObject>::call;
			m_Func[dss::Dummy]				= &Creator<dss::CIdentity, dss::CDummy>::call;
			m_Func[dss::Dummy_Lv2]			= &Creator<dss::CIdentity, dss::CDummy>::call;
			m_Func[dss::Dummy_Lv3]			= &Creator<dss::CIdentity, dss::CDummy>::call;
			m_Func[dss::Dummy_Lv4]			= &Creator<dss::CIdentity, dss::CDummy>::call;

			m_Func[dss::SentryGun_Basis]	= &Creator<dss::CIdentity, dss::CRawObject>::call;
			m_Func[dss::SentryGun]			= &Creator<dss::CIdentity, dss::CSentryGun>::call;
			m_Func[dss::SentryGun_Lv2]		= &Creator<dss::CIdentity, dss::CSentryGun>::call;
			m_Func[dss::SentryGun_Lv3]		= &Creator<dss::CIdentity, dss::CSentryGun>::call;
			m_Func[dss::SentryGun_Lv4]		= &Creator<dss::CIdentity, dss::CSentryGun>::call;

			m_Func[dss::SupplyBase_HP]		= &Creator<dss::CIdentity, dss::CSupply>::call;
			m_Func[dss::SupplyBase_AMMO]	= &Creator<dss::CIdentity, dss::CSupply>::call;
			m_Func[dss::Skill_Upgrade_Base] = &Creator<dss::CIdentity, dss::CSupply>::call;

			for( UINT32 iCnt = 0; iCnt < dss::type_max; ++iCnt)
			{
				if( m_Func[iCnt] == NULL) m_Func[iCnt] = &Creator<dss::CIdentity, dss::CIdentity>::call;
			}
		}
		~Factory() {}

		CREATE_FUNC GetFunc( dss::ENUM_TYPE type) { return m_Func[type]; }
	}g_Factory;

	struct Default
	{
		static Default*		i() 
		{
			if ( !ms_i )
				ms_i.reset(new Default);
			return ms_i.get();
		}
		static void			reset_i() { ms_i.reset(); }
		
		typedef i3::pair<dss::ENUM_TYPE, dss::ENUM_TYPE> type_pair;
		i3::vector<type_pair> m_List;

		Default()
		{
			//m_List.reserve(2);
			m_List.push_back( type_pair(dss::Dummy_Basis, dss::Dummy));
			m_List.push_back( type_pair(dss::SentryGun_Basis, dss::SentryGun));
		}

		dss::ENUM_TYPE GetPair( dss::ENUM_TYPE e)
		{
			for( size_t i = 0; i < m_List.size(); ++i)
				if( m_List[i].first == e) return m_List[i].second;
			
			return dss::type_max;
		}
		static i3::scoped_ptr<Default> ms_i;
	}; //g_Default;
		
	i3::scoped_ptr<Default>	Default::ms_i;
};


CDominationSkillSystem::CDominationSkillSystem() : m_CrossHited(NULL), m_skill_shop_hud(NULL), m_skill_shop_hud_obj(NULL)
{
	m_skillNotice = new SkillNotice;
	m_userSkill = new UserSkillApply;
}

CDominationSkillSystem::~CDominationSkillSystem(void)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		m_SkillList[i]->OnDestroy();
		delete m_SkillList[i];
	}
	m_SkillList.clear();

	i3::map<UINT32, dss::Info*>::iterator itr;
	for( itr = m_InfoList.begin(); itr != m_InfoList.end(); ++itr)
		delete itr->second;

	m_InfoList.clear();

	delete m_skillNotice; m_skillNotice = 0;
	delete m_userSkill; m_userSkill = 0;
}

void CDominationSkillSystem::Initialize()
{
	//Load Basis Resource
	i3RegKey* pWeaponList = Config::_GetRegRoot( "Weapon/WeaponList");
	i3RegKey* pWeaponRoot = Config::FindKey( "Weapon", pWeaponList);

	i3RegKey* pItemRoot = Config::FindKey( "Bomb", pWeaponRoot);

	for( INT32 i = 0; i < pItemRoot->getChildCount(); ++i)
	{
		i3RegKey* pKey = (i3RegKey*)pItemRoot->getChild( i);
		i3::string Name = pKey->GetName();

		for( INT32 j = 0; j < dss::type_max; ++j)
		{
			if( Name.compare( dss::enum_Desc[j]) == 0)
			{
				INT32 ItemID = 0;
				INT32 Lv = 0;

				FIND_REG_DATA( pKey, "ITEMID", &ItemID);
				FIND_REG_DATA( pKey, "Level", &Lv);

				i3GameResChara* pRes = NULL;
				if( j == dss::Dummy_Basis || j == dss::SentryGun_Basis)
				{//basis 리소스 만 로딩 한다.
					pRes = g_pWeaponManager->QueryResource( static_cast<TItemID>(ItemID));
					I3ASSERT( pRes != NULL);
				}

				i3::map<UINT32, dss::Info*>::value_type val( static_cast<UINT32>(ItemID), new dss::Info( dss::ENUM_TYPE(j), pRes, (UINT32)Lv));
				m_InfoList.insert( val);
				break;
			}
		}
	}

	//Item ID 정보를 다 찾은 후...
	i3::map<UINT32, dss::Info*>::iterator itr;
	for( itr = m_InfoList.begin(); itr != m_InfoList.end(); ++itr)
	{
		dss::ENUM_TYPE e = dss::Default::i()->GetPair( itr->second->eType);
			  //dss::g_Default.GetPair( itr->second->eType);
		if( e != dss::type_max)
		{
			i3::map<UINT32, dss::Info*>::iterator itr_pair;
			for( itr_pair = m_InfoList.begin(); itr_pair != m_InfoList.end(); ++itr_pair)
			{
				if( itr_pair->second->eType == e) itr->second->UseItemID = itr_pair->first;
			}
		}
	}
}

bool CDominationSkillSystem::Install( UINT32 ItemID, INT16 ObjType, INT16 ObjIdx, VEC3D* pPos, CGameCharaBase* pChara /*= NULL*/, REAL32 theta)
{
	i3::map<UINT32, dss::Info*>::iterator itr = m_InfoList.find( ItemID);
	if( itr == m_InfoList.end())
	{
		I3ASSERT_0;
		return false;
	}

	dss::CIdentity* p = (*dss::g_Factory.GetFunc( itr->second->eType))();
	if( itr->second->pRes)
	{//Basis
		p->OnCreate( itr->second->pRes, ItemID, ObjType, ObjIdx);
	}
	else
	{
		WeaponBase* pWeapon = g_pWeaponManager->QueryWeapon( pChara, ItemID);
		I3ASSERT( pWeapon != NULL);
		p->OnCreate( pWeapon, ItemID, ObjType, ObjIdx);

		i3::vu::remove( g_pWeaponManager->m_CharaWeaponList, pWeapon);
	}

	p->OnInstall( pPos, theta, pChara);
	m_SkillList.push_back( p);

	return true;
}

void CDominationSkillSystem::UnInstall( dss::CIdentity* p)
{
	I3ASSERT( p != NULL);
	if( p)
	{
		if( m_CrossHited && m_CrossHited->IsObjectSame( p)) m_CrossHited = NULL;

		p->OnUnInstall();
		WeaponBase* pWeapon = p->GetWeapon();
		if( pWeapon) 
		{
			g_pWeaponManager->_AddCharaWeapon( const_cast<WeaponBase*>(pWeapon));

			pWeapon->SetOwner( NULL );
			pWeapon->SetEnable( FALSE);
			
			VEC3D vec;
			i3Vector::Set( &vec, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)(pWeapon->getIdx() / 10) );
			MoveShapePosition( pWeapon, &vec);
		}

		i3::vu::remove( m_SkillList, p);

		delete p;
	}
}

void CDominationSkillSystem::Upgrade(const INT8 type, const INT8 idx, const INT8 customerIdx)
{
	dss::CIdentity* obj = Find(type, idx);
	if (!obj) return;

	WeaponBase* wp = obj->GetWeapon();
	if (!wp) return;

	if (wp->getWeaponInfo()->GetLevel() == wp->getWeaponInfo()->GetMaxLevel()) return; // 이럴 경우는 없다. 서버가 정확하다면...
	
	// upgrade info
	P_BATTLE_SERVER_INSTALL_UNINSTALL_ACK upgradeInfo;
	upgradeInfo._i16ObjType = static_cast<INT16>(wp->getObjectType());
	upgradeInfo._i16ObjIdx = wp->getNetworkIdx();
	upgradeInfo._i16SlotType = 0;
	upgradeInfo._i16SlotIdx = customerIdx;
	upgradeInfo._r16RotateAxis = 0.0f;
	upgradeInfo._r32VecPos[0] = wp->GetPos()->x;
	upgradeInfo._r32VecPos[1] = wp->GetPos()->y;
	upgradeInfo._r32VecPos[2] = wp->GetPos()->z;
	upgradeInfo._ui32ItemID = wp->getWeaponInfo()->GetItemID()+1;
	
	// destroy old
	wp->SetOwner(NULL);
	wp->SetEnable(FALSE);
	VEC3D vec; i3Vector::Set(&vec, (REAL32)(wp->getIdx()%10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)(wp->getIdx()/10));
	MoveShapePosition(wp, &vec);
	wp->ReturnInstance();
	i3::vu::remove(m_SkillList, obj);
	delete obj;

	// install new
	CGameCharaBase* installer = g_pCharaManager->getCharaByNetIdx(upgradeInfo._i16SlotIdx);
	VEC3D pos; i3Vector::Set(&pos, upgradeInfo._r32VecPos[0], upgradeInfo._r32VecPos[1], upgradeInfo._r32VecPos[2]);
	tinst<CDominationSkillSystem>()->Install(upgradeInfo._ui32ItemID, upgradeInfo._i16ObjType, upgradeInfo._i16ObjIdx, &pos, installer);

	// 이전 프레임에서 생성한 것이 이곳에서 해제될수 있다.
	// 무조건 NULL로 설정하여 참조하는곳에서 무시하도록...
	m_CrossHited = NULL;
}

void CDominationSkillSystem::Fix(const INT8 type, const INT8 idx, const INT8 customerIdx)
{
}

dss::CIdentity*	CDominationSkillSystem::Find( UINT32 ItemID, INT16 ObjType, INT16 ObjIdx)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if( m_SkillList[i]->IsItemIDSame( ItemID) && m_SkillList[i]->IsObjectSame(  ObjType, ObjIdx))
			return m_SkillList[i];
	}

	return NULL;
}

dss::CIdentity*	CDominationSkillSystem::Find( INT16 ObjType, INT16 ObjIdx)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if( m_SkillList[i]->IsObjectSame( ObjType, ObjIdx))
			return m_SkillList[i];
	}

	return NULL;
}

bool CDominationSkillSystem::CheckCrossHit( CGameCharaBase* pChara)
{
	//벽 넘어에 있는지 체크 해야 합니다..
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		dss::CIdentity* identSkillObj = m_SkillList[i];
		dss::CServiceable* skill = static_cast<dss::CServiceable*>(identSkillObj);
		if (skill->GetState() == DominationSkill::STATE_NONE) 
		{
			m_skillNotice->Show(pChara, skill);
			continue;
		}

		VEC3D pos = skill->GetPos();

		VEC3D dir;
		i3Vector::Sub( &dir, &pos, pChara->GetPos());
		REAL32 dist = i3Vector::Length( &dir);

		if( (dist < 3.f && (i3Math::abs( getY( &dir)) < 0.05f)) == false) continue;

		VEC3D size = skill->GetBoundSize();
		REAL32 radius = ((size.x+size.z)*0.5f) * 0.5f;
    
		if( dist <= (radius + RADIUS))
		{
			i3Vector::Sub( &dir, &pos, pChara->GetPos());
			i3Vector::Normalize( &dir, &dir);

			REAL32 dot = i3Vector::Dot( &dir, i3Matrix::GetAt( pChara->GetMatrix()));
			if( dot > 0.7f)
			{
				//벽 있는지 체크.
				//i3CollideeLine line;
				//line.SetStart( pChara->GetPos());
				//line.SetDir( &dir);

				//REAL32 collision_length = 1000000.f;
				//I3_PHYSIX_HIT_RESULT* result = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
				//if( result != NULL) collision_length = result->m_T;

				//result->m_T 가 3.423423e-006 ?? 이렇게 되는 경우도 있다. 0.f < m_T < 1.f

				//if( collision_length > (radius + RADIUS))
				{
					m_CrossHited = skill;

					// '변경가능한 상태 및 SP'를 UI로 보여준다.
					if (pChara == g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx()))
					{
						m_skillNotice->Show(pChara, skill);
					}

					return true;
				}
			}
			skill->ClearCharaIdx();
		}
	}

	/*if( m_CrossHited)
	{
		m_CrossHited->OnCancel();
		m_CrossHited = NULL;
	}*/

	m_skillNotice->Hide();
	return false;
}

void CDominationSkillSystem::OnButtonClicked(CGameCharaBase* customer)
{	
	// 설치, 업그레이드, 수리 : 서버 요청
	if (m_CrossHited)
		static_cast<dss::CServiceable*>(m_CrossHited)->OnButtonClicked(customer);
}

void CDominationSkillSystem::Update( REAL32 fElapsedTime)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		m_SkillList[i]->OnUpdate( fElapsedTime);
	}
}	

void CDominationSkillSystem::ChangeUseItemID( UINT32 ItemID, UINT32 UseItemID)
{
	i3::map<UINT32, dss::Info*>::iterator itr = m_InfoList.find( ItemID);
	if( itr == m_InfoList.end())
	{
		I3ASSERT_0;
		return;
	}
	itr->second->UseItemID = UseItemID;
}

void CDominationSkillSystem::MoveShapePosition( WeaponBase* pWeapon, VEC3D* pos)
{
	MATRIX mat;
	if( pWeapon == NULL )
		return;

	if( pWeapon->GetRefCount() < 2)
		return;

	i3Body * pBody = pWeapon->getSceneObject()->GetBody();

	if( pBody == NULL)
		return;

	if( pBody->getFrameLOD() == NULL)
		return;

	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	if( pSkel == NULL)
		return;

	pWeapon->SetPos( pos );

	i3Matrix::Identity( &mat );
	i3Matrix::SetPos( &mat, pos );
	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		if( pSkel->isShapeSet( i) )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != NULL )
			{	
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
}

void CDominationSkillSystem::CreateSkillShopHud( INT16 Idx)
{
	dss::CIdentity* identObj = tinst<CDominationSkillSystem>()->Find( DOMI_TYPE_SKILLSHOP, Idx);
	I3ASSERT( identObj != NULL);
	WeaponBase* p = identObj->GetWeapon();
	I3ASSERT( p != NULL);

	p->setNetworkIdx( Idx);

	CUIHudDomi_Indicator* hud = (CUIHudDomi_Indicator*)tinst<UIHUDDominationMgr>()->GetHUD( HUD_DOMI_INDICATOR);
	m_skill_shop_hud_obj =  hud->InsertIndication( static_cast<i3GameObj*>(p), WAVE_MISSION_DEFENCE, UI_Util::INSTALL_OBJECT);

	m_skill_shop_hud = m_skill_shop_hud_obj->GetHud();
	I3ASSERT( m_skill_shop_hud != NULL);

	SetEnableSkillShopHud( false, false);
}

void CDominationSkillSystem::DeleteSkillShopHud()
{
	if( m_skill_shop_hud_obj)
	{
		WeaponSupply* p = (WeaponSupply*)m_skill_shop_hud_obj->GetObject();
		I3ASSERT( p != NULL);

		tinst<UIHUDDominationMgr>()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_DELETE_INSTALL_OBJECT, p->getNetworkIdx());

		m_skill_shop_hud = NULL;
		m_skill_shop_hud_obj = NULL;
	}
}

void CDominationSkillSystem::SetEnableSkillShopHud( bool inter_mission, bool open_skill_popup)
{
	if( m_skill_shop_hud)
	{
		WeaponSupply* p = (WeaponSupply*)m_skill_shop_hud_obj->GetObject();
		I3ASSERT( p != NULL);

		
		CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx( g_pGameContext->getMySlotIdx());
		I3ASSERT( player != NULL);
		if( player->isCharaStateMask( CHARA_STATE_DEATH))
		{
			m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
			p->m_pWeaponTag->SetEnable( 0, FALSE);
			return;
		}

		if( inter_mission)
		{					
			if( open_skill_popup)
			{
				m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
			}
			else
			{
				VEC3D dir;
 				i3Vector::Sub( &dir, p->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()));
				REAL32 dist = i3Vector::Length( &dir);
				dist -= 0.5f; //조금 더 앞에 있다고 가정 함..

				//i3Vector::Normalize( &dir, &dir);		//1.f


				bool wall = false;
		
				{
					//벽 있는지 체크.
					i3CollideeLine line;
					line.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
					line.SetDir( &dir);

					REAL32 collision_length = 1000000.f;
					I3_PHYSIX_HIT_RESULT* result = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
					if( result != NULL) collision_length = result->m_T;

					//result->m_T 가 3.423423e-006 ?? 이렇게 되는 경우도 있다. 0.f < m_T < 1.f

					if( collision_length < dist)	//벽이 있음..
						wall = true;

					g_pFramework->resetRaycastResult();
				}
				

				if( m_skill_shop_hud_obj->m_IdcCur == tmp_uu::IDC_OUT_SCREEN || wall)
				{
					m_skill_shop_hud->SetEnable( true, WAVE_MISSION_DEFENCE);
					p->m_pWeaponTag->SetEnable( 0, FALSE);
				}
				else
				{
					m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
					p->m_pWeaponTag->SetEnable( 0, TRUE);
				}
			}
		}
		else
		{
			m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
			p->m_pWeaponTag->SetEnable( 0, FALSE);
		}
	}
}
