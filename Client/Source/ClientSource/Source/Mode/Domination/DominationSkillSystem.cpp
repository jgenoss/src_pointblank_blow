#include "pch.h"
#include "DominationSkillSystem.h"

#include "StageBattle.h"
#include "i3Base/smart_ptr/scoped_ptr.h"

#include "../../../Common/CommonSource/CommonDediCli.h"
#include "DomiSkillObjBulletinBoard.h"
#include "DomiUserSkillMgr.h"
#include "BattleHUD_MiniMap.h"

#include "../../ui/hud/UIHUD_Domination_Manager.h"
#include "../../ui/hud/UIHudDomi_Indicator.h"
#include "../../ui/UIHUDTabMinimapDomination.h"
#include "../../ui/Unit.h"
#include "../../ui/UIBattleFrame.h"
#include "../../BattleHud.h"
#include "BattleSlotContext.h"

namespace DominationSkill
{
	template <class B, class D>
	struct Creator
	{
		static B* call()
		{
			B* p = new D;
			MEMDUMP_NEW( p, sizeof(D));
			return p;
		}
	};

	class Factory
	{
		typedef dss::CIdentity* (*CREATE_FUNC)();
		CREATE_FUNC m_Func[dss::type_max];

	public:
		Factory()
		{
			ZeroMemory( m_Func, sizeof(m_Func));
			
			m_Func[dss::Dummy_Transparent]		= &Creator<dss::CIdentity, dss::CRawObject>::call;
			m_Func[dss::Dummy_Lv1]				= &Creator<dss::CIdentity, dss::CDummy>::call;
			m_Func[dss::Dummy_Lv2]				= &Creator<dss::CIdentity, dss::CDummy>::call;
			m_Func[dss::Dummy_Lv3]				= &Creator<dss::CIdentity, dss::CDummy>::call;
			m_Func[dss::Dummy_Lv4]				= &Creator<dss::CIdentity, dss::CDummy>::call;

			m_Func[dss::SentryGun_Transparent]	= &Creator<dss::CIdentity, dss::CRawObject>::call;
			m_Func[dss::SentryGun_Lv1]			= &Creator<dss::CIdentity, dss::CSentryGun>::call;
			m_Func[dss::SentryGun_Lv2]			= &Creator<dss::CIdentity, dss::CSentryGun>::call;
			m_Func[dss::SentryGun_Lv3]			= &Creator<dss::CIdentity, dss::CSentryGun>::call;
			m_Func[dss::SentryGun_Lv4]			= &Creator<dss::CIdentity, dss::CSentryGun>::call;

			m_Func[dss::SupplyHP]				= &Creator<dss::CIdentity, dss::CSupply>::call;
			m_Func[dss::SupplyAmmo]				= &Creator<dss::CIdentity, dss::CSupply>::call;
			m_Func[dss::SkillShop]				= &Creator<dss::CIdentity, dss::CSupply>::call;

			for( UINT32 iCnt = 0; iCnt < dss::type_max; ++iCnt)
			{
				if( m_Func[iCnt] == nullptr) m_Func[iCnt] = &Creator<dss::CIdentity, dss::CIdentity>::call;
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
			{
				Default * p = new Default;
				MEMDUMP_NEW( p, sizeof( Default));
				ms_i.reset(p);
			}
			return ms_i.get();
		}
		static void			reset_i() { ms_i.reset(); }
		
		typedef i3::pair<dss::ENUM_TYPE, dss::ENUM_TYPE> type_pair;
		i3::vector<type_pair> m_List;

		Default()
		{
			//m_List.reserve(2);
			m_List.push_back( type_pair(dss::Dummy_Transparent, dss::Dummy_Lv1));
			m_List.push_back( type_pair(dss::SentryGun_Transparent, dss::SentryGun_Lv1));
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

namespace
{
	void EraseDomiMinimapUnit(INT32 type, INT32 idx)
	{
		// 탭미니맵
		UIHUDTabMinimapDomination* tabminimap = static_cast<UIHUDTabMinimapDomination*>(UIBattleFrame::i()->GetHUD(UIHUD_TABMINIMAP));
		i3::vector<minimap_new::Unit*>& units = tabminimap->GetUnitList();

		for (size_t i=0; i<units.size(); i++)
		{
			switch (type)
			{
			case DOMI_TYPE_DUMMY:
				{
					if (::strcmp((units[i])->GetName(), "UnitDummy") == 0)
					{
						if (static_cast<minimap_new::UnitDummy*>(units[i])->GetObjIdx() == idx)
						{
							I3_SAFE_DELETE( units[i]);
							units.erase(units.begin()+i);
							break;
						}
					}
				}
				break;

			case DOMI_TYPE_SENTRYGUN:
				{
					if (::strcmp((units[i])->GetName(), "UnitSentrygun") == 0)
					{
						if (static_cast<minimap_new::UnitSentrygun*>(units[i])->GetObjIdx() == idx)
						{
							I3_SAFE_DELETE( units[i]); units.erase(units.begin()+i); break;
						}
					}
				}
				break;

			case DOMI_TYPE_SKILLSHOP:
				{
					if (::strcmp((units[i])->GetName(), "UnitSkillShop") == 0)
					{
						if (static_cast<minimap_new::UnitSkillShop*>(units[i])->GetObjIdx() == idx)
						{
							I3_SAFE_DELETE( units[i]); units.erase(units.begin()+i); break;
						}
					}
				}
				break;
			}
		}

		// 구 미니맵(좌상단 미니맵)
#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
		CBattleHUD_MiniMap::instance()->GetSkillObjUnitMgr()->DeleteUnit(type, idx);
#endif
	}

	void AddDomiMinimapUnit(INT32 type, INT32 idx)
	{
		// 탭미니맵
		UIHUDTabMinimapDomination* tabminimap = static_cast<UIHUDTabMinimapDomination*>(UIBattleFrame::i()->GetHUD(UIHUD_TABMINIMAP));
		i3::vector<minimap_new::Unit*>& units = tabminimap->GetUnitList();

		switch (type)
		{
		case DOMI_TYPE_DUMMY:
			{
				minimap_new::UnitDummy * p = new minimap_new::UnitDummy(idx);
				MEMDUMP_NEW( p, sizeof( minimap_new::UnitDummy));
				units.push_back( p);
			}
			break;

		case DOMI_TYPE_SENTRYGUN:
			{
				minimap_new::UnitSentrygun * p = new minimap_new::UnitSentrygun(idx);
				MEMDUMP_NEW( p, sizeof( minimap_new::UnitSentrygun));
				units.push_back( p);
			}
			break;

		case DOMI_TYPE_SKILLSHOP:
			{
				minimap_new::UnitSkillShop * p = new minimap_new::UnitSkillShop(idx);
				MEMDUMP_NEW( p, sizeof( minimap_new::UnitSkillShop));
				units.push_back( p);
			}
			break;
		}

		// 구 미니맵(좌상단 미니맵)
#ifdef DOMI_SKILL_OBJ_MINIMAP_UNIT
		CBattleHUD_MiniMap::instance()->GetSkillObjUnitMgr()->AddUnit(type, idx);
#endif
	}
}


CDominationSkillSystem::CDominationSkillSystem() : m_CrossHited(nullptr), m_skill_shop_hud(nullptr), m_skill_shop_hud_obj(nullptr)
{
}

CDominationSkillSystem::~CDominationSkillSystem(void)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		m_SkillList[i]->OnDestroy();
		I3_SAFE_DELETE( m_SkillList[i]);
	}
	m_SkillList.clear();

	i3::vector_map<UINT32, dss::Info*>::iterator itr;
	for( itr = m_InfoList.begin(); itr != m_InfoList.end(); ++itr)
	{
		I3_SAFE_DELETE( itr->second);
	}

	m_InfoList.clear();
}

void CDominationSkillSystem::Initialize()
{
	//Load Basis Resource
	i3RegKey* pWeaponList = Config::_GetRegRoot( "Weapon/WeaponList");
	i3RegKey* pWeaponRoot = Config::FindKey( "Weapon", pWeaponList);

	i3RegKey* pItemRoot = Config::FindKey( "Bomb", pWeaponRoot);

	I3ASSERT_RETURN(pItemRoot);

	for( INT32 i = 0; i < pItemRoot->getChildCount(); ++i)
	{
		i3RegKey* pKey = (i3RegKey*)pItemRoot->getChild( i);
		i3::string Name = pKey->GetName();

		for( INT32 j = 0; j < dss::type_max; ++j)
		{
			if( Name.compare( dss::enum_Desc[j]) == 0)
			{
				T_ItemID ItemID = 0;
				INT32 Lv = 0;

				FIND_REG_DATA( pKey, "ITEMID", (INT32*) &ItemID);
				FIND_REG_DATA( pKey, "Level", &Lv);

				i3GameResChara* pRes = nullptr;
				if( j == dss::Dummy_Transparent || j == dss::SentryGun_Transparent)
				{//basis 리소스 만 로딩 한다.
					pRes = g_pWeaponManager->QueryResource( static_cast<T_ItemID>(ItemID));
					I3ASSERT( pRes != nullptr);
				}

				dss::Info * p = new dss::Info( dss::ENUM_TYPE(j), pRes, (UINT32)Lv);
				MEMDUMP_NEW( p, sizeof( dss::Info));
				i3::vector_map<UINT32, dss::Info*>::value_type val( static_cast<UINT32>(ItemID), p);
				m_InfoList.insert( val);
				break;
			}
		}
	}

	//Item ID 정보를 다 찾은 후...
	i3::vector_map<UINT32, dss::Info*>::iterator itr;
	for( itr = m_InfoList.begin(); itr != m_InfoList.end(); ++itr)
	{
		dss::ENUM_TYPE e = dss::Default::i()->GetPair( itr->second->eType);
			  //dss::g_Default.GetPair( itr->second->eType);
		if( e != dss::type_max)
		{
			i3::vector_map<UINT32, dss::Info*>::iterator itr_pair;
			for( itr_pair = m_InfoList.begin(); itr_pair != m_InfoList.end(); ++itr_pair)
			{
				if( itr_pair->second->eType == e) itr->second->UseItemID = itr_pair->first;
			}
		}
	}
}

bool CDominationSkillSystem::Install( T_ItemID ItemID, INT32 ObjType, INT32 ObjIdx, const VEC3D* pPos, CGameCharaBase* pChara /*= nullptr*/, REAL32 theta)
{
	i3::vector_map<UINT32, dss::Info*>::iterator itr = m_InfoList.find( ItemID);
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
		I3ASSERT( pWeapon != nullptr);
		p->OnCreate( pWeapon, ItemID, ObjType, ObjIdx);

		if (i3::vu::remove( g_pWeaponManager->m_CharaWeaponList, pWeapon) )
		{
			I3_SAFE_RELEASE(pWeapon);
		}
	}

	p->OnInstall( pPos, theta, pChara);
	m_SkillList.push_back( p);

	AddDomiMinimapUnit(ObjType, ObjIdx);
	return true;
}

void CDominationSkillSystem::UnInstall( dss::CIdentity* p)
{
	I3ASSERT( p != nullptr);
	if( p)
	{
		if( m_CrossHited && m_CrossHited->IsObjectSame( p)) m_CrossHited = nullptr;

		p->OnUnInstall();
		WeaponBase* pWeapon = p->GetWeapon();
		if( pWeapon) 
		{
			g_pWeaponManager->_AddCharaWeapon( const_cast<WeaponBase*>(pWeapon));

			pWeapon->SetOwner( nullptr );
			pWeapon->SetEnable( false);
			
			VEC3D vec;
			i3Vector::Set( &vec, (REAL32)(pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)(pWeapon->getIdx() / 10) );
			MoveShapePosition( pWeapon, &vec);
		}

		i3::vu::remove( m_SkillList, p);

		I3_SAFE_DELETE( p);

		EraseDomiMinimapUnit(p->GetType(), p->GetID());
	}
}

void CDominationSkillSystem::Upgrade(const INT32 type, const INT32 idx, const INT32 customerIdx, REAL32 theta)
{
	dss::CIdentity* obj = Find(type, idx);
	if (!obj) return;

	WeaponBase* wp = obj->GetWeapon();
	if (!wp) return;

	if (wp->getWeaponInfo()->GetLevel() == wp->getWeaponInfo()->GetMaxLevel()) return; // 이럴 경우는 없다. 서버가 정확하다면...
	
	// upgrade info
	T_ItemID itemID = wp->getWeaponInfo()->GetItemID()+1;
	VEC3D pos; i3Vector::Copy(&pos, wp->GetPos());
	
	// destroy old
	wp->SetOwner(nullptr);
	wp->SetEnable(false);
	VEC3D vec; i3Vector::Set(&vec, (REAL32)(wp->getIdx()%10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)(wp->getIdx()/10));
	MoveShapePosition(wp, &vec);
	wp->ReturnInstance();
	i3::vu::remove(m_SkillList, obj);
	I3_SAFE_DELETE( obj);

	EraseDomiMinimapUnit(type, idx);

	// install new
	CGameCharaBase* installer = g_pCharaManager->getCharaByNetIdx(customerIdx);
	Install(itemID, type, idx, &pos, installer, theta);

	// 이전 프레임에서 생성한 것이 이곳에서 해제될수 있다.
	// 무조건 nullptr로 설정하여 참조하는곳에서 무시하도록...
	m_CrossHited = nullptr;
}

void CDominationSkillSystem::Fix(const INT32 type, const INT32 idx, const INT32 customerIdx)
{
}

dss::CIdentity*	CDominationSkillSystem::Find( T_ItemID ItemID, INT32 ObjType, INT32 ObjIdx)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if( m_SkillList[i]->IsItemIDSame( ItemID) && m_SkillList[i]->IsObjectSame(  ObjType, ObjIdx))
			return m_SkillList[i];
	}

	return nullptr;
}

dss::CIdentity*	CDominationSkillSystem::Find( INT32 ObjType, INT32 ObjIdx)
{
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if( m_SkillList[i]->IsObjectSame( ObjType, ObjIdx))
			return m_SkillList[i];
	}

	return nullptr;
}

bool CDominationSkillSystem::CheckCrossHit( CGameCharaBase* pChara)
{
	//벽 넘어에 있는지 체크 해야 합니다..
	for( size_t i = 0; i < m_SkillList.size(); ++i)
	{
		dss::CIdentity* identSkillObj = m_SkillList[i];
		dss::CServiceable* skill = static_cast<dss::CServiceable*>(identSkillObj);

		if (skill->GetState() == DominationSkill::NONE) 
		{
			DomiSkillObjBulletinBoard::i()->Show(pChara, skill);
			continue;
		}

		VEC3D pos = skill->GetPos();

		VEC3D dir;
		i3Vector::Sub( &dir, &pos, pChara->GetPos());
		REAL32 dist = i3Vector::Length( &dir);

		if( (dist < 3.f && (i3Math::abs( getY( &dir)) < 0.5f)) == false) continue;

		VEC3D size = skill->GetBoundSize();
		REAL32 radius = ((size.x+size.z)*0.5f) * 0.5f;
    
		if( dist <= (radius + RADIUS + 0.3f))
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
				//if( result != nullptr) collision_length = result->m_T;

				//result->m_T 가 3.423423e-006 ?? 이렇게 되는 경우도 있다. 0.f < m_T < 1.f

				//if( collision_length > (radius + RADIUS))
				{
					m_CrossHited = skill;

					// '변경가능한 상태 및 SP'를 UI로 보여준다.
					if (pChara == g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx()))
					{
						DomiSkillObjBulletinBoard::i()->Show(pChara, skill);
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
		m_CrossHited = nullptr;
	}*/

	DomiSkillObjBulletinBoard::i()->Hide();
	m_CrossHited = nullptr;
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

void CDominationSkillSystem::ChangeUseItemID( T_ItemID ItemID, UINT32 UseItemID)
{
	i3::vector_map<UINT32, dss::Info*>::iterator itr = m_InfoList.find( ItemID);
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
	if( pWeapon == nullptr )
		return;

	if( pWeapon->GetRefCount() < 2)
		return;

	i3Body * pBody = pWeapon->getSceneObject()->GetBody();

	if( pBody == nullptr)
		return;

	if( pBody->getFrameLOD() == nullptr)
		return;

	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	if( pSkel == nullptr)
		return;

	pWeapon->SetPos( pos );

	i3Matrix::SetPos( &mat, pos );
	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		if( pSkel->isShapeSet( i) )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != nullptr )
			{	
				pShapeSet->setShapeGlobalPose( &mat );
			}
		}
	}
}

void CDominationSkillSystem::CreateSkillShopHud( INT32 Idx)
{
	dss::CIdentity* identObj = CDominationSkillSystem::i()->Find( DOMI_TYPE_SKILLSHOP, Idx);
	I3ASSERT( identObj != nullptr);
	WeaponBase* p = identObj->GetWeapon();
	I3ASSERT( p != nullptr);

	p->setNetworkIdx( static_cast<INT16>(Idx));

	CUIHudDomi_Indicator* hud = (CUIHudDomi_Indicator*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_INDICATOR);
	m_skill_shop_hud_obj =  hud->InsertIndication( static_cast<i3GameObj*>(p), WAVE_MISSION_DEFENCE, UI_Util::INSTALL_OBJECT);
	if (m_skill_shop_hud_obj)
	{
		m_skill_shop_hud = m_skill_shop_hud_obj->GetHud();
		I3ASSERT( m_skill_shop_hud != nullptr);

		SetEnableSkillShopHud( false, false);
	}
}

void CDominationSkillSystem::DeleteSkillShopHud()
{
	if( m_skill_shop_hud_obj)
	{
		WeaponSupply* p = (WeaponSupply*)m_skill_shop_hud_obj->GetObject();
		I3ASSERT( p != nullptr);

		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_DELETE_INSTALL_OBJECT, p->getNetworkIdx());

		m_skill_shop_hud = nullptr;
		m_skill_shop_hud_obj = nullptr;
	}
}

void CDominationSkillSystem::SetEnableSkillShopHud( bool isIntermission, bool open_skill_popup)
{
	if( m_skill_shop_hud)
	{
		WeaponSupply* p = (WeaponSupply*)m_skill_shop_hud_obj->GetObject();
		I3ASSERT( p != nullptr);

		p->m_pWeaponTag->SetEnable( 0, true);
		
		CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		I3ASSERT( player != nullptr);
		if( player->isCharaStateMask( CHARA_STATE_DEATH))
		{
			m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
			//p->m_pWeaponTag->SetEnable( 0, false);
			return;
		}

		if( isIntermission)
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
					if( result != nullptr) collision_length = result->m_T;

					//result->m_T 가 3.423423e-006 ?? 이렇게 되는 경우도 있다. 0.f < m_T < 1.f

					if( collision_length < dist)	//벽이 있음..
						wall = true;

					g_pFramework->resetRaycastResult();
				}
				

				if( m_skill_shop_hud_obj->m_IdcCur == tmp_uu::IDC_OUT_SCREEN || wall)
				{
					m_skill_shop_hud->SetEnable( true, WAVE_MISSION_DEFENCE);
					//p->m_pWeaponTag->SetEnable( 0, false);
				}
				else
				{
					m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
					//p->m_pWeaponTag->SetEnable( 0, true);
				}
			}
		}
		else
		{
			m_skill_shop_hud->SetEnable( false, WAVE_MISSION_DEFENCE);
			//p->m_pWeaponTag->SetEnable( 0, false);
		}
	}
}
