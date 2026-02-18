#include "pch.h"
#include "GameDropItemManager.h"

#include "i3Base/itl/map.h"

#include "../../../Common/CommonSource/CommonDediCli.h"
#include "BattleSlotContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"

namespace DropItem_Util
{	
	const UINT32 DROP_ITEM_MAX = 10;
	const UINT32 DROP_ITEM_AVERAGE_COUNT = 16; 
};

template <class B, class D>
struct Creator
{
	static B* Call() { return D::new_object(); }
};

class DropItem_Factory
{
	typedef CGameDropItem* (*CREATE_FUNC)();
	
	CREATE_FUNC m_Func[DINOKILLITEM_COUNT];

public:
	DropItem_Factory()
	{
		ZeroMemory( m_Func, sizeof(m_Func));

		m_Func[DINOKILLITEM_HP] = &Creator<CGameDropItem, CDropItemHp>::Call;
		m_Func[DINOKILLITEM_AMMO] = &Creator<CGameDropItem, CDropItemAmmo>::Call;
		m_Func[DINOKILLITEM_GRENADE] = &Creator<CGameDropItem, CDropItemGrenade>::Call;

		for( UINT32 iCnt = 0; iCnt < DINOKILLITEM_COUNT; ++iCnt)
		{
			if( m_Func[iCnt] == nullptr) m_Func[iCnt] = &Creator<CGameDropItem, CGameDropItem>::Call;
		}
	}
	~DropItem_Factory() {}

	CREATE_FUNC GetFunc( INT32 nType) { return m_Func[nType]; }
}g_DropItemFactory;

I3_CLASS_INSTANCE(CGameDropItemManager);

CGameDropItemManager::~CGameDropItemManager(void)
{
	OnDestroy();
}

void CGameDropItemManager::_LoadItemInfo()
{
	i3RegKey* pWeaponList = Config::_GetRegRoot( "Weapon/WeaponList");
	i3RegKey* pWeaponRoot = Config::FindKey( "Weapon", pWeaponList);

	i3RegKey* pItemRoot = Config::FindKey( "DropItem", pWeaponRoot);
	
	I3ASSERT_RETURN(pItemRoot);

	//nType에 따라 정렬되어 insert됨.
	i3::vector_map<INT32, DropItemInfo*> mapTemp;	

	for( INT32 nCnt = 0; nCnt < pItemRoot->getChildCount(); ++nCnt)
	{
		i3RegKey* pKey = (i3RegKey*)pItemRoot->getChild( nCnt);
		DropItemInfo Info;

		FIND_REG_DATA( pKey, "_ResPath", Info.res_path);
		FIND_REG_DATA( pKey, "_effect", Info.effect_path);
		FIND_REG_DATA( pKey, "_Name", Info.name);
		FIND_REG_DATA( pKey, "_Type", &Info.nType);

		i3::vector_map<INT32, DropItemInfo*>::value_type val( Info.nType, nullptr);

		i3::pair<i3::vector_map<INT32, DropItemInfo*>::iterator, bool> pr = mapTemp.insert( val);

		if( pr.second == false)
		{
			I3PRINTLOG(I3LOG_WARN, "Weapon.pef->WeaponList->Weapon->Item에 중복된 타입이 있습니다.");
		}
		else
		{
			pr.first->second = new DropItemInfo( Info);
			MEMDUMP_NEW( pr.first->second, sizeof( DropItemInfo));
		}
	}
	
	for( size_t cnt = 0; cnt < m_vecInfo.size(); ++cnt)
	{
		I3_SAFE_DELETE( m_vecInfo[cnt]);
	}
	
	m_vecInfo.clear();

	struct fn
	{
		fn(i3::vector<DropItemInfo*>* p) : pv(p) {}
		void operator()(DropItemInfo* i) { pv->push_back(i); }
		i3::vector<DropItemInfo*>* pv;
	};

	i3::cu::for_each_container( mapTemp, fn(&m_vecInfo) );
}

void CGameDropItemManager::_LoadItemResource()
{
	//Weapon.pef에서 Item Info 로딩
	_LoadItemInfo();

	//리소스 포인터만 저장. 리소스는 스테이지 끝나면 리소스 메니져에서 삭제 함.

	i3::string strResPath;
	i3::vector<DropItemInfo*>::iterator itr;

	for( itr = m_vecInfo.begin(); itr != m_vecInfo.end(); ++itr)
	{
		DropItemInfo* info = *itr;

		i3::utf16_to_mb(info->res_path, strResPath);

		i3GameResChara* pResChara = (i3GameResChara*)g_pFramework->LoadResource( strResPath.c_str(), 0);
		
		if( pResChara == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN,  "  -- Can not load Drop Item Res(%s)", strResPath );
		}
		else
		{
			info->pRes = pResChara;
		}
	}
}

CGameDropItem* CGameDropItemManager::_CreateDropItem( INT32 nItemType)
{	
	I3_BOUNDCHK( nItemType, static_cast<INT32>(m_vecInfo.size()));

	CGameDropItem* pItem = (*g_DropItemFactory.GetFunc( nItemType))();

	pItem->Create( m_vecInfo[nItemType]->pRes);
	pItem->SetItemType( nItemType);
	
	i3::string strEffectPath;
	i3::utf16_to_mb( m_vecInfo[nItemType]->effect_path, strEffectPath );

	INT32 effed_idx = g_pEffectManager->FindEffect( strEffectPath.c_str());
	pItem->SetEffectIdx( effed_idx);

	return pItem;
}

void CGameDropItemManager::OnCreate()
{
	//리소스 로드
	_LoadItemResource();

	CGameDropItem* pItem = _CreateDropItem( DINOKILLITEM_HP);
	m_vecFree.push_back( pItem);
	CGameDropItem::s_effect_idx[DINOKILLITEM_HP] = pItem->GetEffectIdx();

	pItem = _CreateDropItem( DINOKILLITEM_AMMO);
	m_vecFree.push_back( pItem);
	CGameDropItem::s_effect_idx[DINOKILLITEM_AMMO] = pItem->GetEffectIdx();

	pItem = _CreateDropItem( DINOKILLITEM_GRENADE);
	m_vecFree.push_back( pItem);
	CGameDropItem::s_effect_idx[DINOKILLITEM_GRENADE] = pItem->GetEffectIdx();
}	

void CGameDropItemManager::OnDestroy()
{
	i3::vector<CGameDropItem*>::iterator itr;

	itr = m_vecFree.begin();
	while( itr != m_vecFree.end())
	{
		CGameDropItem* p = *itr;
		p->destroy_instance();
		//delete p;
		++itr;
	}
	
	itr = m_vecActive.begin();
	while( itr != m_vecActive.end())
	{
		CGameDropItem* p = *itr;
		p->destroy_instance();
		//delete p;
		++itr;
	}

	i3::vector<DropItemInfo*>::iterator itr_Info;
	itr_Info = m_vecInfo.begin();
	while( itr_Info != m_vecInfo.end())
	{
		I3_SAFE_DELETE( *itr_Info);
		++itr_Info;
	}

	m_vecFree.clear();
	m_vecActive.clear();
	m_vecInfo.clear();
}

void CGameDropItemManager::OnUpdate( REAL32 fDeltaTime)
{
	i3::vector<CGameDropItem*>::iterator itr_Item;
	itr_Item = m_vecActive.begin();
	while( itr_Item != m_vecActive.end())
	{
		(*itr_Item)->OnUpdate( fDeltaTime);

		if( (*itr_Item)->GetFree())
		{
			CGameDropItem* pItem = *itr_Item;

			ReturnDropItem( pItem);

			itr_Item = m_vecActive.erase( itr_Item);
			continue;
		}

		++itr_Item;
	}
}

void CGameDropItemManager::Process_PreStartRound()
{
	ClearActiveItems();
}

CGameDropItem* CGameDropItemManager::FindDropItemByNetCode( INT32 nNetCode) const
{
	i3::vector<CGameDropItem*>::const_iterator itr;
	for( itr = m_vecActive.begin(); itr != m_vecActive.end(); ++itr)
	{
		if( (*itr)->GetCode() == nNetCode) return (*itr);
	}

	return nullptr;
}

CGameDropItem* CGameDropItemManager::GetFreeDropItem( INT32 nType)
{
	I3_BOUNDCHK( nType, DINOKILLITEM_COUNT);

	i3::vector<CGameDropItem*>::iterator itr_Free;
	for( itr_Free = m_vecFree.begin(); itr_Free != m_vecFree.end(); ++itr_Free)
	{
		if( (*itr_Free)->GetItemType() == nType)
		{
			CGameDropItem* pItem = *itr_Free;
			m_vecFree.erase(itr_Free);
			return pItem;
		}
	}

	return _CreateDropItem( nType);
}

CGameDropItem* CGameDropItemManager::DropItemFromAI( INT32 nCode, INT32 nType, VEC3D* vPos)
{
	return DropItemFromAI( nCode, nType, vPos, 2.5f);
}

CGameDropItem* CGameDropItemManager::DropItemFromAI( INT32 nCode, INT32 nType, VEC3D* vPos, REAL32 fSpeed)
{
	CGameDropItem* pItem = nullptr;//FindDropItemByNetCode( nCode);
	//if( pItem)
	//return;

	pItem = GetFreeDropItem( nType);
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());


	I3ASSERT( pItem != nullptr);
	if( pItem == nullptr)
	{
		I3TRACE("  -- Can not Create Drop Item! \n");
		return nullptr;
	}


	pItem->SetCode( nCode);
	pItem->SetMatrix( myPlayer->GetMatrix());
	pItem->SetFalling( &I3_YAXIS, fSpeed, vPos, vPos);
	pItem->SetFree( false);

	m_vecActive.push_back( pItem);

	return pItem;
}

void CGameDropItemManager::ReturnDropItem( CGameDropItem* pItem)
{
	if( m_vecFree.size() > DROP_ITEM_MAX)
	{
		//delete pItem;
		pItem->destroy_instance();
	}
	else
	{
		m_vecFree.push_back( pItem);
	}
}

void CGameDropItemManager::ClearActiveItems()
{
	i3::vector<CGameDropItem*>::iterator itr_Item;
	itr_Item = m_vecActive.begin();
	while( itr_Item != m_vecActive.end())
	{
		CGameDropItem* pItem = *itr_Item;
		pItem->SetFree( true);
		
		ReturnDropItem( pItem);

		itr_Item = m_vecActive.erase( itr_Item);
	}
}

void CGameDropItemManager::DropItemDump() const
{
	I3TRACE("Drop Item Dump>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	i3::vector<CGameDropItem*>::const_iterator itr_Active;
	for( itr_Active = m_vecActive.begin(); itr_Active != m_vecActive.end(); ++itr_Active)
		I3TRACE("  -- Active DropItem(%d)\n", (*itr_Active)->GetCode());

	I3TRACE("  ----------------->  Active DropItem Count : %d\n", m_vecActive.size());

	I3TRACE("  ----------------->  Reserve DropItem Count : %d\n", m_vecFree.size());

	I3TRACE("Drop Item Dump<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
}