#include "pch.h"
#include "TaskProcessor.h"
#include "UserFinder.h"
#include "ModuleSIA.h"

// 사용안함
//bool CTaskProcessor::_UpdateNickChange()
//{
//	if( 0 == m_pDBWorker->OutGetNickChangeCount() )	return false;
//		
//	NICK_CHANGE_OUT* pNickChangeOut = m_pDBWorker->OutGetNickChange(); 
//
//	USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( pNickChangeOut->i64UID ); 
//	if( NULL == pNode )								goto e_Exit;
//
//	switch( pNickChangeOut->ui8Type )
//	{
//	case TYPE_USERINFO_NICK_CHANGE_ITEM	:
//		{//현우
//			 사용 안함.
//			INT32 i32Rv = pNickChangeOut->i32Rv;//EVENT_ERROR_FAIL;
//			if( EV_SUCCESSED( i32Rv ) ) 
//			{
//				i3String::Copy( pNode->_sBasic.m_strNick, pNickChangeOut->_strNick, sizeof(pNode->_sBasic.m_strNick) );
//				if( FALSE == g_pModuleSIA->ShopItemAuth_T( pNode->_i64UID, pNickChangeOut->_i64ItemWareIdx, 1 ) )
//				{
//					i32Rv = EVENT_ERROR_FAIL;
//				}
//			}
//			if( EV_FAILED( i32Rv ) )
//			{
//				// 닉네임 변경은 아이템 인증에서만 일어나기 때문에 아이템 인증 실패를 내려준다.
//				i3NetworkPacket	SendPacket( PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK );		
//				SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) ); 
//				SendPacket.WriteData( &pNode->_i64UID,			sizeof( INT64 ) ); 
//				SendPacket.WriteData( &i32Rv,					sizeof( INT32 ) ); 
//				SendGamePacket(pNode->_i32Serveridx, &SendPacket);
//			}
//		}
//		break; 
//	case TYPE_USERINFO_NICK_CHANGE_NEW	: 
//		{
//			i3String::Copy( pNode->_sBasic.m_strNick, pNickChangeOut->_strNick, sizeof(pNode->_sBasic.m_strNick) ); 
//			i3NetworkPacket	SendPacket( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
//			SendPacket.WriteData( &pNode->_i32Sessionidx,				sizeof( INT32 ) ); 
//			SendPacket.WriteData( &pNode->_i64UID,					sizeof( INT64 ) ); 
//			SendPacket.WriteData( &pNickChangeOut->i32Rv,			sizeof( INT32 ) ); 
//			if( EV_SUCCESSED( pNickChangeOut->i32Rv ) )
//			{
//				UINT8 ui8NickSize = (UINT8)i3String::Length( pNode->_sBasic.m_strNick )+1;
//				SendPacket.WriteData( &ui8NickSize,					sizeof( UINT8 ) ); 
//				SendPacket.WriteData( pNode->_sBasic.m_strNick,		ui8NickSize ); 
//
//				
//				// 닉네임 생성이 성공한 유저(신규 유저)에게 아이템 지급
//				InsertEventItemNewUser( pNode );
//			}
//			SendGamePacket(pNode->_i32Serveridx, &SendPacket);
//		}
//		break; 
//	}
//
//e_Exit:
//
//	m_pDBWorker->OutGetNickChangeIdx(); 
//		
//	return true;
//}