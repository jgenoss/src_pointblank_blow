#include "pch.h"
#include "TaskProcessor.h"
#include "UserFinder.h"
#include "ModuleDBShop.h"

bool CTaskProcessor::_UpdateNick()
{
	// 1 프레임당 한개씩만 처리합니다.
	S2RingBuffer* pRing = m_pDBNick->GetRingOut();
	if( 0 == pRing->GetBufferCount() )	return false;

	NICK_OUT* pNickOut = (NICK_OUT*)pRing->Pop(); 
	if( NULL == pNickOut )				return false;

	USER_NODE* pNode = pNickOut->pNode;
	if( NULL == pNode )					goto e_Exit;

	INT32 i32Rv = pNickOut->i32Rv; 

	switch( pNickOut->ui8Type )
	{
	case TYPE_NICK_T_FIND		:
		{
			i3NetworkPacket	SendPacket(PROTOCOL_BASE_CHECK_NICK_NAME_ACK);
			SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) ); 
			SendPacket.WriteData( &pNode->_i64UID,			sizeof( T_UID ) ); 
			SendPacket.WriteData( &i32Rv,			sizeof( INT32 ) ); 
			SendGamePacket(pNode->_i32Serveridx, &SendPacket);
		}
		break; 
	case TYPE_NICK_T_CHANGE	:
		{
			if( EV_SUCCESSED( i32Rv ) )
			{
				g_pUserFinder->DeleteNameHash( pNickOut->strOldNick );
				g_pUserFinder->InsertNameHash( pNode );

				DB_SHOP_IN DBShopIn;
				DBShopIn._ui8InsertType	= TYPE_SHOP_ITEM_AUTH;
				DBShopIn._i64UID		= pNode->_i64UID;
				DBShopIn._TWareDBIdx	= pNickOut->TItemWareIdx;
				DBShopIn._ui32ItemArg	= 1;

				if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem( &DBShopIn ) )
				{	// Error
					g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Item Auth Nick Change / UID : %I64d / ItemWareIdx : %d ", pNode->_i64UID, pNickOut->TItemWareIdx );	
					i32Rv = EVENT_ERROR_FAIL;
				}
			}

			if( EV_FAILED( i32Rv ) )
			{
				// 닉네임 변경은 아이템 인증에서만 일어나기 때문에 아이템 인증 실패를 내려준다.
				i3NetworkPacket	SendPacket( PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK );		
				SendPacket.WriteData( &pNode->_i32Sessionidx,	sizeof( INT32 ) ); 
				SendPacket.WriteData( &pNode->_i64UID,			sizeof( T_UID ) ); 
				SendPacket.WriteData( &pNickOut->i32Rv,			sizeof( INT32 ) ); 
				SendGamePacket(pNode->_i32Serveridx, &SendPacket);
			}
		}
		break; 
	case TYPE_NICK_T_GIFT	:
		{	
			if( EV_FAILED( i32Rv ) )
			{	// 선물 받는 유저 닉네임 검색 실패 
				for( UINT8 i = 0; i < pNickOut->m_SendGift.m_ui8GiftCount ; ++i ) pNickOut->m_SendGift.m_aGift[i].m_i32Rv = i32Rv;
				_SendGiftSender(pNode, &pNickOut->m_SendGift); 				
				break;
			}

			// 받는 사람 UID Update.
			pNickOut->m_SendGift.m_TReceiverUID = pNickOut->m_TUID_Receiver;

			i32Rv = _GiftInsertDB( &pNickOut->m_SendGift, ITEM_INSERT_REQUEST_USER_GIFT );
			if( EV_FAILED( i32Rv ) )	 
			{
				for( UINT8 i = 0; i < pNickOut->m_SendGift.m_ui8GiftCount ; ++i ) pNickOut->m_SendGift.m_aGift[i].m_i32Rv = i32Rv;
				_SendGiftSender(pNode, &pNickOut->m_SendGift); 				
				break;
			}
		}
		break;
	default					: 
		break; 
	}

	pNode->ReleaseReference();

e_Exit:

	pRing->PopIdx(); 

	return true;
}