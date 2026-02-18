#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "DataBase.h"
#include "Receiver.h"
#include "ConnectedUser.h"
#include "NoteBox.h"
#if defined( USE_BLOCK_LIST )
void CTaskProcessor::_WorkForBlockList(void)
{
	OUT_BLOCK_LIST* pData = g_pDataBase->GetOutBlockListData();	

	if( NULL == pData ) return;

	USER_NODE node;
	if( FALSE == m_pUserFinder->FindUid(pData->_i64uid, &node) )
	{
		g_pLog->WriteLog( L"[ERROR] _WorkForBlockList() FindUid Failed / uid(%I64d))", pData->_i64uid );
		return;
	}

	// 필요 없다
	//// 차단 리스트 Reset
	//for( UINT8 i = 0 ; i < BLOCK_MAX_COUNT ; i ++ )
	//{
	//	node._pBlock[i]._i64buid = 0;
	//	node._pBlock[i]._strComment[0] = '\0';
	//	node._pBlock[i]._strNick[0] = '\0';
	//	node._pBlock[i]._ui8rank = 0;
	//}
 //

	// 해당 유저의 차단 리스트를 내부 노드에 추가한다.
	node._ui32BlockCount = MIN(pData->_ui32blockCount, BLOCK_MAX_COUNT);
	i3mem::Copy( node._pBlock, pData->_pBlockNode, (sizeof( BLOCK_NODE)*node._ui32BlockCount) );
	m_pUserFinder->UpdateByUid(&node);
 
	UINT8 ui8BlockCount = (UINT8)node._ui32BlockCount;	

	i3NetworkPacket packet(PROTOCOL_MESSENGER_BLOCK_INFO_ACK);
	MakePacket_OGame2Mess_Block_Info_Ack(&packet,
										  node._sessionidx,
										  node._i64UID,
										  ui8BlockCount,
										  node._pBlock,
										  pData->_pBlockComment);

	// Send Game Server
	g_pGateway->SendPacket(node._serveridx, &packet);
	// m_pUserFinder->UpdateByUid(&node);

	return;
}

void CTaskProcessor::_WorkForBlockControl(void)
{
	OUT_BLOCK_CONTROL* pData = g_pDataBase->GetOutBlockControlData();

	switch(pData->_i32type)
	{

	case BLOCK_CONTROL_ADD:
		{
			USER_NODE node;
			
			if( TRUE == m_pUserFinder->FindUid(pData->_i64uid, &node))	// 차단 유저 추가 성공
			{
				UINT8 ui8Nicklength = (UINT8)(i3String::Length(pData->_pBlockNode._strNick) + 1);
				UINT8 ui8Commentlength = (UINT8)(i3String::Length(pData->_strComment) + 1);
 
				i3NetworkPacket packet(PROTOCOL_MESSENGER_BLOCK_INSERT_ACK);
				MakePacket_OGame2Mess_Block_Insert_Ack(&packet,
														node._sessionidx,
														node._i64UID,
														pData->_i32result,
														pData->_pBlockNode._i32Idx,
														pData->_pBlockNode._ui8rank,
														ui8Nicklength,
														pData->_pBlockNode._strNick,
														ui8Commentlength,
														pData->_strComment
														);

				// Send Game Server
				SendPacketMessage(node._serveridx, &packet);

				i3mem::Copy(&node._pBlock[node._ui32BlockCount], &pData->_pBlockNode, sizeof(BLOCK_NODE));

				node._ui32BlockCount++;

				m_pUserFinder->UpdateByUid(&node);
			}		
			else	// 유저 노드 검색 실패시 로그만 남긴다.( 해당 유저의 게임서버 Idx를 알수가 없으므로 )
			{

				g_pLog->WriteLog( L"[ERROR] _WorkForBlockControl() ADD User Node Failed/ uid(%I64d))", pData->_i64uid );
			}
		}
		break;

	
	case BLOCK_CONTROL_DELETE:
		{
			USER_NODE node;

			if( TRUE == m_pUserFinder->FindUid(pData->_i64uid, &node))
			{
				// 차단 유저 삭제 성공시 node._pBlock 배열에 가장 뒤에 있는 정보를 빈자리에 채워넣는다.
				INT32 i32Temp = -1;

				// 삭제된 차단 유저의 DB Index를 찾는다.
				for( INT32 i = 0 ; i < (INT32)node._ui32BlockCount ; i++ )
				{
					if( node._pBlock[i]._i32Idx == pData->_i32Idx ) 
					{
						i32Temp = i; 
						break;
					}
				}

				// 검색한 빈자리 체크
				if( i32Temp < 0 || i32Temp >= BLOCK_MAX_COUNT )
				{
					g_pLog->WriteLog( L"[ERROR] _WorkForBlockControl() Delete DB Success, MessengerServer Failed/ uid(%I64d)) / i32Temp(%d)", node._i64UID, i32Temp );

					i3NetworkPacket packet(PROTOCOL_MESSENGER_BLOCK_DELETE_ACK);
					MakePacket_OGame2Mess_Block_Delete_Ack(&packet,
														node._sessionidx,
														node._i64UID,
														EVENT_ERROR_EVENT_BLOCK_UNKOWN,
														0);	// 실패이므로 의미 없는 값 보냄.

					// 차단 유저 삭제 결과를 게임서버로 보낸다.
					SendPacketMessage(node._serveridx, &packet);
				}
				else	// 차단 유저 삭제 최종 성공
				{
					// 일어날 리는 없지만 혹시 모르니.
					if( node._ui32BlockCount > 0 )
					{
						i3mem::Copy( &node._pBlock[i32Temp], &node._pBlock[node._ui32BlockCount-1],  sizeof( BLOCK_NODE) );	// Copy

						// delete. 필요하나..??
						node._pBlock[node._ui32BlockCount-1]._i32Idx = 0; 
						node._pBlock[node._ui32BlockCount-1]._ui8rank = 0; 
						node._pBlock[node._ui32BlockCount-1]._i64buid = 0;
						////node._pBlock[node._ui32BlockCount-1]._strComment[0] = 0;
						node._pBlock[node._ui32BlockCount-1]._strNick[0] = 0;


						node._ui32BlockCount--;


						m_pUserFinder->UpdateByUid(&node);


						i3NetworkPacket packet(PROTOCOL_MESSENGER_BLOCK_DELETE_ACK);
						MakePacket_OGame2Mess_Block_Delete_Ack(&packet,
							node._sessionidx,
							node._i64UID,
							pData->_i32result,
							pData->_i32Idx);

						// 차단 유저 삭제 결과를 게임서버로 보낸다.
						SendPacketMessage(node._serveridx, &packet);
					}
				}
			}
			else	// 유저 노드 검색 실패시 로그만 남긴다.( 해당 유저의 게임서버 Idx를 알수가 없으므로 )
			{

				g_pLog->WriteLog( L"[ERROR] _WorkForBlockControl() Delete User Node Failed/ uid(%I64d))", pData->_i64uid );
			}
		}
		break;

	
	}

	return; 
}
#endif