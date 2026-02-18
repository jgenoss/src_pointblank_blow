#pragma once

#include "GameDropItem.h"
#include "DropItemHp.h"
#include "DropItemAmmo.h"
#include "DropItemGrenade.h"

namespace DropItem_Util
{
	struct DropItemInfo
	{
		INT32 nType = -1;
	
		i3::rc_wstring name;
		i3::rc_wstring res_path;
		i3::rc_wstring effect_path;

		i3GameResChara*	pRes = nullptr;

		DropItemInfo() {}
		DropItemInfo( const DropItemInfo& other)
		{
			*this = other;
		}
		DropItemInfo& operator=( const DropItemInfo& other)
		{
			nType = other.nType;
			pRes = other.pRes;
			name = other.name;
			res_path = other.res_path;
			effect_path = other.effect_path;
			return *this;
		}
	};

	struct IntrusionDropItem
	{
		UINT8	ui8DItemType = 0;
		INT16	i16RandCode = 0;
		UINT32	ui32DItemLifeTime = 0;
		VEC3D	vDItemPosition;
	};

	typedef i3::shared_ptr<IntrusionDropItem> IntrusionDropItemPtr;
};

using namespace DropItem_Util;

template<> struct is_tinst_manual<class CGameDropItemManager> : std::tr1::true_type {};

class CGameDropItem;

class CGameDropItemManager : public i3ElementBase
{
	I3_CLASS_DEFINE(CGameDropItemManager, i3ElementBase);

	virtual ~CGameDropItemManager();

	i3::vector<DropItemInfo*>				m_vecInfo;

	i3::vector<CGameDropItem*>				m_vecFree;
	i3::vector<CGameDropItem*>				m_vecActive;
																		
	void									_LoadItemInfo();
	void									_LoadItemResource();

	CGameDropItem*							_CreateDropItem( INT32 nType);
	void									_DestroyDropItem( CGameDropItem*) {}
public:
	static CGameDropItemManager* i() { return tinst<CGameDropItemManager>(); }

	void			OnCreate();
	void			OnDestroy();
	void			OnUpdate( REAL32 fDeltaTime);
	void			Process_PreStartRound();						//기존에 사용 하던 아이템 삭제

	/*Active 리스트에 있는 아템들 중에서 검색*/
	CGameDropItem*	FindDropItemByNetCode( INT32 nNetCode) const;		

	CGameDropItem*	GetFreeDropItem( INT32 nType);
	CGameDropItem*	DropItemFromAI( INT32 nCode, INT32 nType, VEC3D* vPos); 
	CGameDropItem*	DropItemFromAI( INT32 nCode, INT32 nType, VEC3D* vPos, REAL32 fSpeed); 
	void			ReturnDropItem( CGameDropItem* pItem);

	void			ClearActiveItems();
	void			DropItemDump() const;
};

tinst<CGameDropItemManager>;