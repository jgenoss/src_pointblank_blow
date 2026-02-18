#include "pch.h"
#include "UserInven.h"

CUserInven::CUserInven()
{
}

CUserInven::~CUserInven()
{
}

BOOL CUserInven::InsertItem( INVEN_BUFFER* pInvenItem )
{
	//INT32 i32BlankIdx = -1;
	//// ID 가 0 이면 삭제된 아이템이다.
	//if( 0 == pInvenItem->_ui32ItemID )	return;

	//// 케릭터의 경우 인벤토리에 집어 넣지 않는다.
	//switch( GET_ITEM_TYPE( pInvenItem->_ui32ItemID ) )
	//{
	//case ITEM_TYPE_CHARA:				return;
	//}

	//BOOL bNew = TRUE;
	//for( INT32 j = 0 ; j < MAX_INVEN_COUNT ; j++ )
	//{
	//	if( 0 == m_Inven[j]._i64ItemWareDBIdx )
	//	{
	//		if( -1 == i32BlankIdx )
	//		{
	//			i32BlankIdx = j;
	//		}
	//	}
	//	else if( m_Inven[j]._i64ItemWareDBIdx == pInvenItem->_i64ItemWareDBIdx )
	//	{
	//		i32BlankIdx = j;
	//		bNew = FALSE;
	//		break;
	//	}
	//}
	//// 아이템 복사
	//if( -1 != i32BlankIdx )
	//{
	//	i3mem::Copy( &m_Inven[ i32BlankIdx ], pInvenItem, sizeof(INVEN_BUFFER));
	//	if( bNew )
	//	{
	//		pUserNode->_ui16InvenCount++;
	//	}
	//}

	return TRUE;
}

BOOL CUserInven::DeleteItem( T_ItemDBIdx& TItemDBIdx )
{
	//for( INT32 j = 0 ; j < MAX_INVEN_COUNT ; j++ )
	//{
	//	if( TItemDBIdx == pUserNode->_Inven[j]._i64ItemWareDBIdx )
	//	{
	//		UINT32 ui32ItemID = m_Inven[j]._ui32ItemID;
	//		i3mem::FillZero( &m_Inven[j], sizeof( INVEN_BUFFER ) );
	//		pUserNode->_ui16InvenCount--;
	//		return ui32ItemID;
	//	}
	//}

	return TRUE;
}
