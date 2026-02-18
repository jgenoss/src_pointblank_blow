#include "pch.h"
#include "InvenList.h"

#include "InvenDB.h"
#include "Shop.h"
#include "ui/UIItemInfoUtil.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"

#include "i3Base/string/ext/lrtrim.h"
#include "i3Base/string/algorithm/to_lower.h"

#include <functional>

namespace
{



//#define MAX_STRING	2048

	static i3FileStream* pi3FileStream = 0;
	static UINT32 gStringReaded = (UINT32)-1;

	bool LoadFileStream(i3FileStream * pFileStream, const i3::string & FileName, 
		bool CheckUniCodeBOM, UINT32 ReadFlag = STREAM_READ | STREAM_SHAREREAD)
	{
		if( !pFileStream->Open(FileName.c_str(), ReadFlag) )
		{
#ifdef NC_BUILD
			I3PRINTLOG(I3LOG_FATAL,  "Stream IO Error! %s", FileName );
#else
			I3PRINTLOG(I3LOG_FATAL,  "%s 파일을 열 수 없습니다.", FileName );
#endif
			return false;
		}

		if( CheckUniCodeBOM )
		{
			char szUnicodeBuffer[ 2 ] = "";

			pFileStream->Read( szUnicodeBuffer, sizeof( WCHAR16) );

			if( szUnicodeBuffer[0] != 0xFEFF )
			{
#ifdef NC_BUILD
				I3PRINTLOG(I3LOG_FATAL,  "FileStream is not Unicode! %s", FileName );
#else
				I3PRINTLOG(I3LOG_FATAL,  "%s 은 유니코드 형식이 아니다.", FileName );
#endif
				return false;
			}
		}

		gStringReaded = (UINT32)-1;

		pi3FileStream = pFileStream;

		return true;
	}

//	bool IsEof()
//	{
//		return ( gStringReaded == 0 );
//	}

//	i3::string ReadLine()
//	{
//		char szBuffer[MAX_STRING] = "";
//
//		gStringReaded = pi3FileStream->ReadLine( szBuffer, sizeof( szBuffer ) );
//
//		return i3::string( szBuffer );
//	}

//	i3::wstring ReadLineW()
//	{
//		WCHAR16 szBuffer[MAX_STRING] = L"";
//
//		gStringReaded = pi3FileStream->ReadLineW( szBuffer, sizeof( szBuffer ) );
//
//		return i3::wstring( szBuffer );
//	}

}

/**************************//**************************/
namespace ivu
{
	CInvenList* g_this = 0;

	void dump(const i3::vector<INVEN_BUFFER*>& list, const char* title)
	{
		I3TRACE("\n");		I3TRACE(title);		I3TRACE("\n");

		char str[256] = "";
		for(size_t i=0; i<list.size(); i++)
		{
			const INVEN_BUFFER* pBuffer = list[i];

			i3::snprintf(str, 256, "%03d, ItemId:%d, WareDBIdx:%I64d, ItemType:%d, ItemArg:%d\n",
				i, pBuffer->_ui32ItemID, pBuffer->_TItemWareDBIdx, pBuffer->_ui8ItemType, pBuffer->_ui32ItemArg);

			I3TRACE( str);
		}

		I3TRACE("\n");
	}

	struct Fn_ItemId
	{
		UINT32 _ui32ItemID;
		Fn_ItemId(UINT32 i) : _ui32ItemID(i) {}

		bool operator()(INVEN_BUFFER* pBuffer) const
		{
			return (pBuffer->_ui32ItemID == _ui32ItemID);
		}
	};
	struct Fn_Default_ItemId
	{
		UINT32 _ui32ItemID;
		Fn_Default_ItemId(UINT32 i) : _ui32ItemID(i) {}

		bool operator()(INVEN_BUFFER* pBuffer) const
		{
			return (pBuffer->_ui32ItemID == _ui32ItemID && pBuffer->_TItemWareDBIdx == 0);
		}
	};

	struct Fn_ItemId_Unused
	{
		UINT32 _ui32ItemID;
		Fn_ItemId_Unused(UINT32 i) : _ui32ItemID(i) {}

		bool operator()(INVEN_BUFFER* pBuffer) const
		{
			return (pBuffer->_ui32ItemID == _ui32ItemID && 
				pBuffer->_ui8ItemType == ITEM_ATTR_TYPE_BUY);
		}
	};

	struct Fn_WareDBIdx
	{
		T_ItemDBIdx _TItemWareDBIdx;
		Fn_WareDBIdx(T_ItemDBIdx w) : _TItemWareDBIdx(w) {}

		bool operator()(INVEN_BUFFER* pBuffer) const
		{
			return (pBuffer->_TItemWareDBIdx == _TItemWareDBIdx);
		}
	};
}

/**************************//**************************/
I3_CLASS_INSTANCE(CInvenList);//,i3ElementBase);

CInvenList::CInvenList()
{
	ivu::g_this = this;

	m_InvenDB[INVEN::DB_WEAPON] = new WeaponDB;
	m_InvenDB[INVEN::DB_CHARA] = new CharacterDB;
	m_InvenDB[INVEN::DB_PARTS] = new PartsDB;
	m_InvenDB[INVEN::DB_SKIN] = new SkinDB;
	m_InvenDB[INVEN::DB_CASHITEM] = new CashItemDB;
	m_InvenDB[INVEN::DB_COUPON] = nullptr;

	m_CouponDB = new CouponBufferDB;

	MEMDUMP_NEW( m_InvenDB[INVEN::DB_WEAPON], sizeof(WeaponDB));
	MEMDUMP_NEW( m_InvenDB[INVEN::DB_CHARA], sizeof(CharacterDB));
	MEMDUMP_NEW( m_InvenDB[INVEN::DB_PARTS], sizeof(PartsDB));
	MEMDUMP_NEW( m_InvenDB[INVEN::DB_SKIN], sizeof(SkinDB));
	MEMDUMP_NEW( m_InvenDB[INVEN::DB_CASHITEM], sizeof(CashItemDB));
	MEMDUMP_NEW( m_CouponDB, sizeof(CouponBufferDB));

	m_latestUsedCashItemID = 0;
	m_bUseBattleGiveupCashItem = false;
}

CInvenList::~CInvenList()
{
	struct Fn
	{
		void operator()(iInvenDB* p)
		{
			I3_SAFE_DELETE( p);
		}
	};
	std::for_each(m_InvenDB, m_InvenDB+INVEN::DB_MAX_COUNT, Fn() );

	I3_SAFE_DELETE( m_CouponDB);
}

CInvenList* CInvenList::i()
{
	return ivu::g_this;
}



/**********************/
const i3::vector<INVEN_BUFFER*>& CInvenList::get_main_list(INVEN::DB_TYPE db_type) const
{
	const i3::vector<INVEN_BUFFER*>* db_list = nullptr;

	if( db_type == INVEN::DB_CASHITEM)
		db_list = &static_cast< CashItemDB* >( m_InvenDB[db_type] )->m_Spreaded_Main_list;
	else
		db_list = &m_InvenDB[db_type]->m_Main_list;

	return *db_list;
}

const i3::vector<INVEN_BUFFER*>& CInvenList::get_ref_list(INVEN::DB_TYPE db_type, INT32 usage_type) const
{
	const i3::vector<INVEN_BUFFER*> * db_list = nullptr;

	switch( db_type )
	{
	case INVEN::DB_WEAPON:	db_list = static_cast< WeaponDB* >( m_InvenDB[db_type] )->get_list( (WEAPON_SLOT_TYPE)usage_type );			break;
	case INVEN::DB_CHARA:	db_list = static_cast< CharacterDB* >( m_InvenDB[db_type] )->get_list( (CHARACTER_CLASS_TYPE)usage_type );	break;
	case INVEN::DB_PARTS:	db_list = static_cast< PartsDB* >( m_InvenDB[db_type] )->get_list( (CHARACTER_CLASS_TYPE)usage_type );		break;
	case INVEN::DB_SKIN:		db_list = static_cast< SkinDB* >( m_InvenDB[db_type] )->get_list( usage_type );								break;
	case INVEN::DB_CASHITEM:	db_list = static_cast< CashItemDB* >( m_InvenDB[db_type] )->get_list( (ITEM_TYPE)usage_type );				break;
	}

	return *db_list;
}

const  INVEN_BUFFER* CInvenList::GetItem( INVEN::DB_TYPE db_type, INT32 idx, INT32 usage) const
{
	if( db_type == INVEN::DB_COUPON)
	{
		return __GetCoupon( idx, (usage > -1));
	}
	else
	{
		if( idx < 0 ) return 0;

		const i3::vector<INVEN_BUFFER*>& db_ref_list = get_ref_list(db_type, usage);
		if( db_ref_list.size() <= (UINT32)idx)	return 0;

		return db_ref_list[idx];
	}
}

bool CInvenList::IsValidItem(INVEN::DB_TYPE db_type, T_ItemID ItemId) const
{
	I3ASSERT( db_type != INVEN::DB_COUPON);

	const i3::vector<INVEN_BUFFER*>& db_list = get_main_list(db_type);

	i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_ItemId(ItemId) );

	if( it == db_list.end() )
		return false;

	return true;
}

bool CInvenList::IsValidItem(INVEN::DB_TYPE db_type, T_ItemDBIdx ware_idx) const
{
	I3ASSERT( db_type != INVEN::DB_COUPON);

	const i3::vector<INVEN_BUFFER*>& db_list = get_main_list(db_type);

	i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(ware_idx) );

	if( it == db_list.end() )
		return false;

	return true;
}

const char * CInvenList::__getDBTypeString( INVEN::DB_TYPE type)
{
	switch( type)
	{
	case INVEN::DB_INVALID :	return "Invalid InvenDB";
	case INVEN::DB_WEAPON :		return "Weapon InvenDB";
	case INVEN::DB_CHARA :		return "Chara InvenDB";
	case INVEN::DB_PARTS :		return "Parts InvenDB";
	case INVEN::DB_SKIN :		return "Skin InvenDB";
	case INVEN::DB_CASHITEM :	return "CashItem InvenDB";
	case INVEN::DB_COUPON :		return "Coupon InvenDB";
	default :					return "Unknown InvenDB";
	}
}


/**************************//**************************/
//Weapon

void CInvenList::AddItem(INVEN_BUFFER* pBuffer,INT32 nCount)
{
	for( INT32 i = 0 ; i < nCount ; i++ )
	{
		if( pBuffer[i]._ui32ItemID == 0)
		{	// 삭제되는 아이템
			if( pBuffer[i]._TItemWareDBIdx > -1)
				DeleteInvenBuffer( pBuffer[i]._TItemWareDBIdx);
		}
		else
		{
			ITEM_TYPE itemTypeForCheck = (ITEM_TYPE) GET_ITEM_TYPE(  pBuffer[i]._ui32ItemID);

			if (itemTypeForCheck == ITEM_TYPE_QUESTCARD)		// 임무카드의 경우 일단 무시처리(2015.07.01.수빈)
				continue;	

			if (g_pEnvSet->IsV2Version() == false)
			{
				if (pBuffer[i]._ui32ItemID == static_cast<T_ItemID>(MAKE_DEFAULT_PARTS_ITEMID(ITEM_TYPE_HEADGEAR))
					|| pBuffer[i]._ui32ItemID == static_cast<T_ItemID>(MAKE_DEFAULT_PARTS_ITEMID(ITEM_TYPE_FACEGEAR)))
					continue;
			}

			INVEN::DB_TYPE dbType = INVEN::ItemID2DBType( pBuffer[i]._ui32ItemID );
		
			if( dbType == INVEN::DB_COUPON)
				m_CouponDB->AddInvenBufferToList( &pBuffer[i], 1 );
			else
			{
				if ( dbType < 0 || dbType >= INVEN::DB_MAX_COUNT )
				{
					I3_BOUNDCHK(dbType, INVEN::DB_MAX_COUNT);
				}
				else
				{
					m_InvenDB[dbType]->AddInvenBufferToList(&pBuffer[i], 1);
				}
			}
		}
	}
}

bool CInvenList::GetItemTypeByItemID( INVEN::DB_TYPE dbType, T_ItemID itemID, UINT8 * pOutType, UINT32 * pOutArg)
{
	if( dbType == INVEN::DB_COUPON)
	{
		I3ASSERT_0;	// 코드 작성 필요
	}
	else
	{
		const i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[ dbType]->m_Main_list;
		i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_ItemId( itemID) );

		if( it == db_list.end() ) return false;

		if( pOutType != nullptr)	*pOutType = (*it)->_ui8ItemType;
		if( pOutArg != nullptr)	*pOutArg = (*it)->_ui32ItemArg;
	}

	return true;
}

bool CInvenList::GetItemTypeByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx, UINT8* pOutType, UINT32* pOutArg)
{
	if( dbType == INVEN::DB_COUPON)
	{
		if( pOutType) *pOutType = 0;
		if (pOutArg) *pOutArg = 0;

		I3ASSERT_0;	// 코드 작성 필요
	}
	else
	{
		const i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[ dbType]->m_Main_list;
		i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx( ware_idx) );

		if( it == db_list.end() )
		{
			if( pOutType) *pOutType = 0;
			if (pOutArg) *pOutArg = 0;
			return false;
		}

		if( pOutType != nullptr)	*pOutType = (*it)->_ui8ItemType;
		if( pOutArg != nullptr)	*pOutArg = (*it)->_ui32ItemArg;
	}

	return true;
}

bool CInvenList::GetItemTypeByIdx( INVEN::DB_TYPE dbType, INT32 idx, UINT8* pOutType, UINT32* pOutArg)
{
	if( dbType == INVEN::DB_COUPON)
	{
		I3ASSERT_0;	// 코드 작성 필요
	}
	else
	{
		const INVEN_BUFFER * pBuffer = GetItem( dbType, idx);
		if( pBuffer == nullptr) return false;

		if( pOutType != nullptr)	*pOutType = pBuffer->_ui8ItemType;
		if( pOutArg != nullptr)	*pOutArg = pBuffer->_ui32ItemArg;
	}

	return true;
}

UINT32 CInvenList::GetItemCount( INVEN::DB_TYPE dbType, INT32 usage)
{
	I3ASSERT( dbType != INVEN::DB_COUPON);

	const i3::vector<INVEN_BUFFER*>& db_list = get_ref_list( dbType, usage);
	return db_list.size();
}

UINT32 CInvenList::FindItemCount( T_ItemID itemID)
{
	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType( itemID);

	if (dbType < 0 || dbType >= INVEN::DB_COUPON)
	{
		I3_BOUNDCHK(dbType, INVEN::DB_COUPON);
		return 0;
	}

	const i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[ dbType]->m_Main_list;
	return std::count_if(db_list.begin(), db_list.end(), ivu::Fn_ItemId( itemID) );
}

INT32 CInvenList::FindItemIndexByItemID( INVEN::DB_TYPE dbType, T_ItemID itemID, INT32 usage )
{
	const i3::vector<INVEN_BUFFER*>& db_list = get_ref_list( dbType, usage);
	I3ASSERT( dbType != INVEN::DB_COUPON);

	i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_ItemId( itemID) );

	if( it == db_list.end() )
	{
		//I3PRINTLOG(I3LOG_WARN,  "Not exist %s (Usage: %d, ItemId: %d)\n", __getDBTypeString( dbType), usage, itemID);
		return -1;
	}

	return it - db_list.begin();
}

INT32 CInvenList::FindItemIndexByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx, INT32 usage )
{
	if( dbType == INVEN::DB_COUPON)
	{
		const i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;

		i3::vector<CouponBufferDB::COUPON_BUFFER*>::const_iterator it =
			std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx( ware_idx));

		return it - db_list.begin();
	}
	else
	{
		const i3::vector<INVEN_BUFFER*>& db_list = get_ref_list( dbType, usage);

		i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(ware_idx) );
		if( it == db_list.end() )
		{
			//I3PRINTLOG(I3LOG_WARN,  "Not exist %s (Usage: %d, Parts_WareDxIdx: %d)\n", __getDBTypeString(dbType), usage, ware_idx);
			return -1;
		}

		return it - db_list.begin();
	}
}

UINT32 CInvenList::GetItemArg( INVEN::DB_TYPE dbType, INT32 idx, INT32 usage )
{
	if( dbType == INVEN::DB_COUPON)
	{
		
	}
	else
	{
		const INVEN_BUFFER* pBuffer = GetItem( dbType, idx, usage);
		if( pBuffer != nullptr)
			return pBuffer->_ui32ItemArg;
	}

	return 0xFFFFFFFF;	// 에러값
}

T_ItemID CInvenList::GetItemID( INVEN::DB_TYPE dbType, INT32 idx, INT32 usage)
{
	if( dbType == INVEN::DB_COUPON)
	{
		const CouponBufferDB::COUPON_BUFFER * pBuffer = m_CouponDB->m_Main_list[idx];
		return pBuffer->_ui32ItemID;
	}
	else
	{
		const INVEN_BUFFER* pBuffer = GetItem( dbType, idx, usage);
		if( pBuffer != nullptr)
			return pBuffer->_ui32ItemID;
	}

	return 0;
}

T_ItemID CInvenList::FindItemIDByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx)
{
	I3ASSERT( dbType != INVEN::DB_COUPON);

	INT32 idx = FindItemIndexByDBIdx( dbType, ware_idx);
	return GetItemID( dbType, idx);
}

T_ItemDBIdx CInvenList::FindWareDBIdx_ByItemID( T_ItemID ItemId)
{
	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType( ItemId);
	if ( -1 == dbType )
		return INVALID_WAREDB_INDEX;

	INT32 usage_type = -1;

	switch( dbType )
	{
	case INVEN::DB_WEAPON:		usage_type = WEAPON::ItemID2Slot( ItemId);	break;
	case INVEN::DB_CHARA:		
	case INVEN::DB_PARTS:		
	case INVEN::DB_SKIN:		usage_type = CHARA::ItemID2Class(ItemId);	break;
	case INVEN::DB_CASHITEM:	usage_type = GET_ITEM_TYPE(ItemId);			break;
	case INVEN::DB_COUPON:		I3ASSERT( dbType != INVEN::DB_COUPON );		break;
	}

	INT32 idx = FindItemIndexByItemID( dbType, ItemId, usage_type);
	return GetDBIdxByIndex( dbType, idx, usage_type );
}

T_ItemDBIdx CInvenList::FindWareDBIdx_ByItemID_ForBattle( T_ItemID ItemId)
{
	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType( ItemId);
	I3ASSERT( dbType != INVEN::DB_COUPON);

	INT32 idx = FindItemIndexByItemID( dbType, ItemId, WEAPON::ItemID2Slot( ItemId));

	return GetDBIdxByIndex( dbType, idx, WEAPON::ItemID2Slot( ItemId));
}

T_ItemDBIdx CInvenList::GetDBIdxByIndex( INVEN::DB_TYPE dbType, INT32 idx, INT32 usage)
{
	const INVEN_BUFFER* pBuffer = GetItem( dbType, idx, usage);
	if( pBuffer != nullptr )
		return pBuffer->_TItemWareDBIdx;

	return INVALID_WAREDB_INDEX;
}

bool CInvenList::HaveItemByItemID( T_ItemID itemID)
{
	return (FindItemIndexByItemID( INVEN::ItemID2DBType( itemID), itemID) > -1);
}

bool CInvenList::HaveItemByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx)
{
	return (FindItemIndexByDBIdx( dbType, ware_idx) > -1);
}

bool CInvenList::HaveItemBySubtype( INVEN::DB_TYPE dbType, INT32 type)
{
	I3ASSERT( dbType != INVEN::DB_COUPON);

	const i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[ dbType]->m_Main_list;

	UINT32 Count = db_list.size();
	for( UINT32 i = 0 ; i < Count ; i++)
	{
		INVEN_BUFFER* pBuffer = db_list[i];
		I3ASSERT( pBuffer != nullptr);

		INT32 subType = (INT32) GET_ITEM_SUBTYPE( pBuffer->_ui32ItemID);
		if( subType == type)
			return true;
	}

	return false;	//	없다.
}


/**************************//**************************/
//CashItem


INVEN_BUFFER* CInvenList::FindCashItem(INT32 ItemID)
{
	const i3::vector<INVEN_BUFFER*>& db_list = get_main_list(INVEN::DB_CASHITEM);
	i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_ItemId(ItemID) );
	if( it == db_list.end() ) return 0;
	return (* it);
}

/**************************//**************************/
//CashItem
void CInvenList::UpdateInvenBuffer(INVEN_BUFFER* pBuffer)
{
	INVEN::DB_TYPE type = INVEN::ItemID2DBType( pBuffer->_ui32ItemID);

	ITEM_TYPE itemTypeForCheck = (ITEM_TYPE) GET_ITEM_TYPE(  pBuffer->_ui32ItemID);

	if (itemTypeForCheck == ITEM_TYPE_QUESTCARD)		// 임무카드의 경우 일단 무시처리(2015.07.01.수빈)
		return;	


	if( type == INVEN::DB_COUPON )
	{
		//coupon 타입 따로 처리한다.
		m_CouponDB->UpdateInvenBufferToList(pBuffer);
	}
	else
	{
		if (type < 0 || type >= INVEN::DB_MAX_COUNT)
		{
			I3_BOUNDCHK(type, INVEN::DB_MAX_COUNT);
		}
		else
		{
			m_InvenDB[type]->UpdateInvenBufferToList(pBuffer);
		}
	}
}


void	CInvenList::DeleteInvenBuffer(T_ItemDBIdx WareDBIdx)
{
	for( UINT32 type = INVEN::DB_WEAPON; type < INVEN::DB_MAX_COUNT; ++type )
	{
		if( type == INVEN::DB_COUPON)
		{
			DeleteCouponBuffer(WareDBIdx);
		}
		else
		{
			i3::vector<INVEN_BUFFER*>& db_list = const_cast< i3::vector<INVEN_BUFFER*>& >( m_InvenDB[type]->m_Main_list );
			i3::vector<INVEN_BUFFER*>::iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(WareDBIdx) );

			if( it != db_list.end() )
			{
				I3MEM_SAFE_FREE( *it );
				db_list.erase(it);
				m_InvenDB[type]->ReReferenceUsage();
			}
		}
	}
}

void	CInvenList::DeleteInvenBuffer_only_DefaultParts( T_ItemID ItemId)
{
	i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[INVEN::DB_PARTS]->m_Main_list;
	i3::vector<INVEN_BUFFER*>::iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_Default_ItemId(ItemId) );
	if( it == db_list.end() ) return;

	I3MEM_SAFE_FREE( *it );
	db_list.erase(it);

	m_InvenDB[INVEN::DB_PARTS]->ReReferenceUsage();
}

void CInvenList::DeleteInvenBuffer_only_Chara( T_ItemDBIdx WareDBIdx)
{
	i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[INVEN::DB_CHARA]->m_Main_list;
	i3::vector<INVEN_BUFFER*>::iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(WareDBIdx) );
	if( it == db_list.end() ) return;
	
	I3MEM_SAFE_FREE( *it );
	db_list.erase(it);

	m_InvenDB[INVEN::DB_CHARA]->ReReferenceUsage();
}

INVEN::DB_TYPE	CInvenList::FindInvenDBTypeByDBIdx(T_ItemDBIdx WareDBIdx)
{
	for( UINT32 type = INVEN::DB_WEAPON; type < INVEN::DB_MAX_COUNT; ++type )
	{
		const i3::vector<INVEN_BUFFER*>& db_list = m_InvenDB[ type ]->m_Main_list;
		i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(WareDBIdx) );
		if( it != db_list.end() )	
			return (INVEN::DB_TYPE)type;
	}
	return INVEN::DB_INVALID;
}

void	CInvenList::DeleteInvenAllClear()
{
	struct Fn
	{
		void operator()(iInvenDB* p)
		{
			i3::cu::for_each_SafeFree_clear(p->m_Main_list);
			p->ReReferenceUsage();
		}
	};
	std::for_each(m_InvenDB, m_InvenDB+INVEN::DB_MAX_COUNT, Fn() );

	i3::cu::for_each_SafeFree_clear(m_CouponDB->m_Main_list);
}

bool CInvenList::IsUsingCashItem( T_ItemID ItemID)
{
	//	사용중인 key 아이템을 찾고
	INT32 KeyItemID = GetCashItemByGroup( ItemID);

	//	보유중이면 
	UINT8 Type = 0;	UINT32 Arg = 0;
	if( !GetItemTypeByItemID( INVEN::DB_CASHITEM, KeyItemID, &Type, &Arg) )
		return false;

	// 기간제인데 구입만한 아이템은 사용중이 아닌 아이템이다.
	UINT8 AuthType =  CShop::i()->GetAuthType(KeyItemID);
	if (AuthType == ITEM_TYPE_TIME && Type == ITEM_ATTR_TYPE_BUY)
		return false;

	return true;
}

//bool CInvenList::IsUsingCashItem_Maintenance(CASHITEM_GROUP_TYPE type)
//{
//	T_ItemID ItemID = MAKE_CASH_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, type, CASHITEM_DAY_NO );
//	return IsUsingCashItem(ItemID);
//}
//
//bool CInvenList::IsUsingCashItem_Period(CASHITEM_GROUP_TYPE type)
//{
//	T_ItemID ItemID = MAKE_CASH_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_WRAP_PERIOD, type, CASHITEM_DAY_NO );
//	return IsUsingCashItem(ItemID);
//}
//
//bool CInvenList::IsUsingCashItem_Count(CASHITEM_GROUP_TYPE type)
//{
//	T_ItemID ItemID = MAKE_CASH_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_WRAP_COUNT, type, CASHITEM_DAY_NO );
//	return IsUsingCashItem(ItemID);
//}

void CInvenList::SetUseBattleGiveupCashItem(bool bVal)
{ 
	// 튜토리얼 모드에서는 자유 이동 등의 아이템이 사용되어 졌다는 팝업이 뜨면 안된다.
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) 
	{ 
		m_bUseBattleGiveupCashItem = false;
	}
	else
	{
		m_bUseBattleGiveupCashItem = bVal; 
	}
}

const INVEN_BUFFER* CInvenList::__GetCoupon(INT32 Idx, bool bCheckAllData) const
{
	// 쿠폰 Index가 -1 이면 nullptr 반환
	if(Idx < 0)	return nullptr;

	CouponBufferDB::COUPON_BUFFER* pBuffer = nullptr;

	// 인벤 전체의 쿠폰 리스트에서 검색
	if(bCheckAllData)
	{
		pBuffer = m_CouponDB->m_Main_list[Idx] ;
	}
	else
	{
		// 사용중이 아닌 쿠폰 내에서만 검색
		INT32 slot = 0;
		for(INT32 i = 0 ;  i <= Idx; i++)
		{		
			pBuffer = m_CouponDB->m_Main_list[slot];

			if(pBuffer->m_bUsed == true)
				i--;

			slot++;
		}
	}

	return pBuffer;
}

bool CInvenList::IsUsedCoupon(T_ItemDBIdx CouponDBIdx)
{
	const i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;

	i3::vector<CouponBufferDB::COUPON_BUFFER*>::const_iterator it = 
		std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(CouponDBIdx));

	return (it == db_list.end()) ? false : (*it)->m_bUsed;
}


void CInvenList::SetUsedCoupon(T_ItemDBIdx CouponDBIdx , bool bUsed)
{
	const i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;

	i3::vector<CouponBufferDB::COUPON_BUFFER*>::const_iterator it = 
		std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(CouponDBIdx));

	if( it != db_list.end() )
	{
		(*it)->m_bUsed = bUsed;
	}
}

void CInvenList::DeleteAllCouponBuffer()
{
	i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;
	i3::cu::for_each_SafeFree_clear(db_list);
}

void CInvenList::DeleteOnlyUsedCouponBuffer()
{
	i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;

	i3::vector<CouponBufferDB::COUPON_BUFFER*>::iterator it = db_list.begin();
	while( it != db_list.end() )
	{
		if( (*it)->m_bUsed )
		{
			I3MEM_SAFE_FREE( *it );
			it = db_list.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void CInvenList::DeleteCouponBuffer(T_ItemDBIdx CouponDBIdx)
{
	i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;

	i3::vector<CouponBufferDB::COUPON_BUFFER*>::iterator it = 
		std::find_if(db_list.begin(), db_list.end(), ivu::Fn_WareDBIdx(CouponDBIdx));

	if( it != db_list.end() )
	{
		I3MEM_SAFE_FREE( *it );
		db_list.erase( it );
	}
}

void CInvenList::DeleteTimeOverCouponBuffers()
{
	INT32 nCurServerTime = UserInfoContext::i()->GetInvenServerTime();

	i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;

	i3::vector<CouponBufferDB::COUPON_BUFFER*>::iterator it = db_list.begin();
	while( it != db_list.end() )
	{
		INT32 nCouponLimitTime = (*it)->_ui32ItemArg;
		if(nCouponLimitTime - nCurServerTime <= 0)
		{
			I3MEM_SAFE_FREE( *it );
			it = db_list.erase( it );
		}
		else
		{
			++it;
		}
	}
}

INT32 CInvenList::GetDiscountCouponCount()
{
	struct Fn : std::unary_function<CouponBufferDB::COUPON_BUFFER*, bool>
	{
		bool operator()(CouponBufferDB::COUPON_BUFFER*p) const
		{
			return p->m_bUsed;
		}
	};

	// 사용된 쿠폰은 카운팅하지 않는다.
	const i3::vector<CouponBufferDB::COUPON_BUFFER*>& db_list = m_CouponDB->m_Main_list;
	return std::count_if(db_list.begin(), db_list.end(), std::not1( Fn() ) );
}

//#if 0
typedef i3::vector< UINT32 > WeaponDummy;

void _GetWeaponType(const i3::string & section,
					INT32 & itemClassType,
					INT32 & usageType,
					INT32 & weaponClassType)
{
	itemClassType = ITEM_EVENT_TYPE_NONE;
	usageType = ITEM_TYPE_UNKNOWN;
	weaponClassType = WEAPON_CLASS_UNKNOWN;

	//주무기
	if( section == "assault" )
	{
		usageType = ITEM_TYPE_PRIMARY;
		weaponClassType = WEAPON_CLASS_ASSAULT;
	}
	else if( section == "subassault" )
	{
		usageType = ITEM_TYPE_SECONDARY;
		weaponClassType = WEAPON_SUBCLASS_ASSAULT;
	}
	else if( section == "smg" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_SMG;
	}
	else if( section == "subsmg" )
	{
		usageType = ITEM_TYPE_SECONDARY;
		weaponClassType = WEAPON_SUBCLASS_SMG;
	}
	else if( section == "subbow")
	{
		usageType = ITEM_TYPE_SECONDARY;		
		weaponClassType = WEAPON_SUBCLASS_BOW;
	}
	else if( section == "bow" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_BOW;
	}
	else if( section == "dualsmg" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_DUALSMG;
	}
	else if( section == "dualshotgun" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_DUALSHOTGUN;
	}
	else if( section == "sniper" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_SNIPER;
	}
	else if( section == "subsniper")
	{
		usageType = ITEM_TYPE_SECONDARY;		
		weaponClassType = WEAPON_SUBCLASS_SNIPER;
	}
	else if( section == "rpg7" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_ROCKET_LAUNCHER;
	}
	else if( section == "shotgun" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_SHOTGUN;
	}
	else if( section == "subshotgun")
	{
		usageType = ITEM_TYPE_SECONDARY;		
		weaponClassType = WEAPON_SUBCLASS_SHOTGUN;
	}
	else if( section == "mg" )
	{
		usageType = ITEM_TYPE_PRIMARY;		
		weaponClassType = WEAPON_CLASS_MG;
	}
	//보조무기
	else if( section == "handgun" )	
	{
		usageType = ITEM_TYPE_SECONDARY;		
		weaponClassType = WEAPON_CLASS_HANDGUN;
	}
	else if( section == "dualhandgun" )	//보조무기
	{
		usageType = ITEM_TYPE_SECONDARY;		
		weaponClassType = WEAPON_CLASS_DUALHANDGUN;
	}
	else if( section == "cic" )	//보조무기
	{
		usageType = ITEM_TYPE_SECONDARY;		
		weaponClassType = WEAPON_CLASS_CIC;
	}
	//근접무기
	else if( section == "knife" )	
	{
		usageType = ITEM_TYPE_MELEE;		
		weaponClassType = WEAPON_CLASS_KNIFE;
	}
	else if( section == "dualknife" )	
	{
		usageType = ITEM_TYPE_MELEE;		
		weaponClassType = WEAPON_CLASS_DUALKNIFE;
	}
	else if( section == "knucle" )
	{
		usageType = ITEM_TYPE_MELEE;		
		weaponClassType = WEAPON_CLASS_KNUCKLE;
	}
	//투척무기
	else if( section == "throwing" )
	{
		usageType = ITEM_TYPE_THROWING1;		
		weaponClassType = WEAPON_CLASS_THROWING_GRENADE;
	}
	//특수무기
	else if( section == "item" )	
	{
		usageType = ITEM_TYPE_THROWING2;		
		weaponClassType = WEAPON_CLASS_THROWING_CONTAIN;
	}
	//mission 무기
	else if( section == "mission" )	
	{
		usageType = ITEM_TYPE_MISSION;		
		weaponClassType = WEAPON_CLASS_MISSION;
	}
}

void _LoadDummy(WeaponDummy & Weapon, WeaponDummy & Major, WeaponDummy & MajorType)
{
	i3FileStream LocaleStream;

	LoadFileStream( &LocaleStream, "config\\DebugDummy.txt", false );

	i3::string separators = "\t:= ";
	i3::string line, section, weapon, major;

	Weapon.clear();
	g_DebugSpaceMode = false;

	INT32 itemClassType = 0, usageType = 0, weaponClassType = 0, weaponIndex = 0;
	INT32 innerCount = 0;
	for(  ;  ; )
	{
//		line = StringUtil::ReadLine();

		UINT32 read_byte = LocaleStream.ReadLine(line);
		
		if (read_byte == 0 || read_byte == STREAM_ERR)
			break;
		
		size_t index = line.find("//");	//주석 처리
		if( index != i3::string::npos )
			line.erase( index );

		i3::lrtrim( line );

		if( line.empty() )
			continue;

		if( line.at( 0 ) == '[' && line.at( line.length() -1 ) == ']' )
		{
			section = line.substr( 1, line.length() -2 );
			i3::to_lower( section );

			i3::lrtrim( section );

			if( innerCount == 1 )
			{
				_GetWeaponType( section, itemClassType, usageType, weaponClassType);
			}
			else
			{
				continue;
			}
		}
		else
		{
			if( line.at( 0 ) == '{' )
			{
				++innerCount;
				continue;
			}
			else if( line.at( 0 ) == '}' )
			{
				--innerCount;
				continue;
			}				

			//첫번째 분활 문자 찾기 및 문자열 분활하기
			i3::string::size_type separator_pos = line.find_first_of(separators, 0);
			if (separator_pos != i3::string::npos)
			{
				weapon = line.substr(0, separator_pos);

				i3::string::size_type nonseparator_pos = line.find_first_not_of(separators, separator_pos);
				major = (nonseparator_pos == std::string::npos) ? "" : line.substr(nonseparator_pos);

				if( !major.empty() )
				{
					weaponIndex = GetItemIndex( weapon.c_str() );
					Major.push_back( weaponIndex );
					MajorType.push_back( weaponClassType );
				}
			}
			else
			{
				if( section == "startingmap" )
				{
					g_DebugStartingMap = line;
					continue;
				}
				if (section == "spacemodeon")
				{
					line == "ON" ? g_DebugSpaceMode = true : g_DebugSpaceMode = false;
					continue;
				}

				weaponIndex = GetItemIndex( line.c_str() );
			}

			Weapon.push_back( 
				MAKE_ITEM_FLAG( itemClassType, usageType, weaponClassType, weaponIndex ) );
		}
	}
}
//#endif

void CInvenList::SetWeaponListDummy(void)
{
//#if 0
	WeaponDummy WeaponDummy, MajorDummy, MajorTypeDummy;

	_LoadDummy(WeaponDummy, MajorDummy, MajorTypeDummy);

	size_t size = GET_ARRAY_COUNT( g_DebugWeaponSet );

	if( size != MajorDummy.size() )
	{
		I3PRINTLOG(I3LOG_FATAL,  "주 무기 설정이 잘 못 되었습니다. DebugDummy.txt 파일을 확인하세요." );
		return;
	}

	for(size_t i = 0; i < size; ++i)
	{
		g_DebugWeaponSet[i][0] = MajorTypeDummy[i];
		g_DebugWeaponSet[i][1] = MajorDummy[i];
	}

	INT32 count = WeaponDummy.size();	//GET_ARRAY_COUNT(aDummy);
	INVEN_BUFFER *aDummy = new INVEN_BUFFER[ count ];
	MEMDUMP_NEW( aDummy, sizeof(INVEN_BUFFER) * count);

	// 실제 등록된 무기가 리소스로 존재하는지 검사하고 없다면 인벤토리에서 제거.
	char pszRes[256] = "";
	for(INT32 i=0; i<count; i++)
	{
		aDummy[i]._TItemWareDBIdx = i;
		aDummy[i]._ui32ItemID = WeaponDummy[ i ];
		aDummy[i]._ui8ItemType = 0;
		aDummy[i]._ui32ItemArg = 0;

		WEAPON_CLASS_TYPE nClass = WEAPON::ItemID2Class( aDummy[i]._ui32ItemID);
		WEAPON::RES_ID nNumber = WEAPON::ItemID2ResID( aDummy[i]._ui32ItemID);

		// pef에 없는 무기를 인벤토리에서 제거
		CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(nClass, nNumber);
		if (pWeaponInfo == nullptr)
		{
			aDummy[i]._ui32ItemID = 0;

			I3TRACE( "Weapon.pef 파일에 등록 안된 무기 : class %d number %d\n", nClass, nNumber);
			continue;
		}

		//i3::safe_string_copy( pszRes, pWeaponInfo->GetName());	// I3CHR 파일 이름
		INT32 len = i3::generic_string_size( pWeaponInfo->GetName());

		if( len > 0)
		{
			I3ASSERT( len < 256);

			GlobalFunc::DecryptValue( pszRes, pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);

			i3::generic_string_cat( pszRes, ".I3CHR");

			// 실제 리소스(I3CHR)가 없는 무기를 인벤토리에서 제거
			i3GameRes * pRes = g_pFramework->QuaryResource( pszRes);
			if( pRes == nullptr)
			{
				aDummy[i]._ui32ItemID = 0;

				I3TRACE( "I3CHR 파일 등록 안된 무기 : class %d number %d\n", nClass, nNumber);
				continue;
			}
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "no setting resource name weapon(%d, %d)", nClass, nNumber);
		}
		m_InvenDB[INVEN::DB_WEAPON]->AddInvenBufferToList(aDummy, count);
	}

	

	I3_SAFE_DELETE_ARRAY( aDummy);					// 커버러티 오류로 배열delete처리 (11.11.07.수빈)
//#endif
}

void CInvenList::SetCharacterListDummy(void)
{
//#if 0
	INT32 temp_idx = 10;
	INT32 time = 86400;
	INVEN_BUFFER aDummy[] = {
#if 1
		{ temp_idx++, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_HEADGEAR, CHARACTER_CLASS_UNKNOWN, HEAD_RES_ID_SANTA_HAT),					ITEM_ATTR_TYPE_AUTH, time},
#else
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_MALEDEFAULT),		CHARACTER_RES_ID_MALEDEFAULT),			ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_FEMALEDEFAULT),		CHARACTER_RES_ID_FEMALEDEFAULT),		ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_FEMALE_RED),			CHARACTER_RES_ID_FEMALE_RED),			ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_FEMALE_BLUE),		CHARACTER_RES_ID_FEMALE_BLUE),			ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_MALE_RED),			CHARACTER_RES_ID_MALE_RED),				ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_MALE_BLUE),			CHARACTER_RES_ID_MALE_BLUE),			ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_D_FOX),				CHARACTER_RES_ID_D_FOX),				ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_LEOPARD),			CHARACTER_RES_ID_LEOPARD),				ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(CHARACTER_RES_ID_REINFORCED_LEOPARD),	CHARACTER_RES_ID_REINFORCED_LEOPARD),	ITEM_ATTR_TYPE_AUTH, time},
#endif
	};
	INT32 DummyCount = GET_ARRAY_COUNT(aDummy);
	AddItem( aDummy, DummyCount);
//#endif
}

void CInvenList::SetCashItemListDummy(void)
{
//#if 0
	INT32 temp_idx = 10;
	INT32 time = 86400;
	INVEN_BUFFER aDummy[] =
	{
#if 0
#else
		{ temp_idx++, CASHITEM_ITEMID_XP_PLUS_110,		ITEM_ATTR_TYPE_AUTH, time},
		{ temp_idx++, CASHITEM_ITEMID_CHANGE_NICK,		ITEM_ATTR_TYPE_AUTH, time},
#endif
	};

	INT32 count = GET_ARRAY_COUNT(aDummy);

	m_InvenDB[INVEN::DB_CASHITEM]->AddInvenBufferToList(aDummy, count);
//#endif
}

INT32 CInvenList::GetCharaterSlotCount_V11()
{
	return static_cast<CharacterDB*>(m_InvenDB[INVEN::DB_CHARA])->m_nCharacterSlotCount;
}