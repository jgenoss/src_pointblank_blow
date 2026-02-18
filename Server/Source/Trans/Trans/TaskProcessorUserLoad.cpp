#include "pch.h"
#include "UserFinder.h"
#include "TaskProcessor.h"

bool CTaskProcessor::_UpdateUserLoad()
{
	// 1 프레임당 한개씩만 처리합니다.
	if( 0 == m_pDBUserLoad->GetBufferCount() )	return false;

	LOAD_USER_INFO_OUT* pDBRecv = m_pDBUserLoad->OutGetLoadInfo();

	USER_NODE * pNode = pDBRecv->pNode;
	if( 0 != pNode->_i64UID )
	{
		// DB 로드 후 계정 세팅
		if( EV_SUCCESSED( pDBRecv->i32Rv ) )
		{
			g_pUserFinder->InsertNameHash( pNode );

			INT32 i32MainSlotIdx = -1;

			for (UINT16 i = 0; i < pNode->_ui16DeleteCharaCount; ++i)
			{	// 만료된 캐릭터를 멀티 슬롯에서 삭제해 줍니다.
				INT32 i32DeleteSlotIdx = pNode->m_MultiSlot.FindSlotIdxByCharaDBIdx(pNode->_DeleteCharaDBIdx[i]);

				if (EV_SUCCESSED(DeleteCharaDB(pNode->_i64UID, i32DeleteSlotIdx, pNode->_DeleteCharaDBIdx[i])))
				{	// 아래쪽에서 캐릭터 정보를 내려줘야 하기 때문에, 멀티 슬롯에서 미리 제거 합니다.  
					pNode->m_pDesign->DeleteChara(pNode->m_MultiSlot, i32DeleteSlotIdx, i32MainSlotIdx);
					if (i32MainSlotIdx != -1)
					{
						pNode->_ui32UpdateInfo |= SAVE_DATABASE_CHARA_COMMON;
					}
				}
			}

			// 금일 최초 접속
			if( pNode->m_dt32LastLogInDate.GetDateTimeBin( TIME_ATTENDANCE_CHECK ) < i3ThreadTimer::LocalTime().GetDateTimeBin( TIME_ATTENDANCE_CHECK ) )
			{
				i3mem::FillZero( &pNode->_sDaily, sizeof(USER_INFO_DAILY) );	// 당일 전적 정보 초기화
				pNode->m_stUserInfoServer.m_ui8ReportCount = 0;					// ReportCount 신고 횟수는 매일 초기화 해줍니다.
				pNode->m_stUserInfoServer.m_ui8VoteCount = 0;					// VoteCount 투표 제한 횟수는 매일 초기화 해줍니다.
				pNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD;
			}

			// 출석체크 세팅 합니다.
			_SetAttendanceInfo( pNode );

			_CreateDefaultChara(pNode);
		}

		i3NetworkPacket	SendPacket(PROTOCOL_BASE_GET_USER_INFO_ACK);
		SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
		SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
		SendPacket.WriteData( &pDBRecv->i32Rv,				sizeof( INT32 ) );
		if( EV_SUCCESSED( pDBRecv->i32Rv ) )
		{
			_MakeUserInfoPacket( &SendPacket, pNode, FALSE);	// 로그인시에는 클랜 Load X.

			_SendUserCharaPacket( pNode);
			_SendUserInvenPacket( pNode, pNode->_ui16InvenCount);	// 인벤토리 정보

			// 기존 유저 이벤트 아이템 지급
			InsertEventItemOldUser( pNode );

			// 인벤토리에만 있고 멀티슬롯에 없는 캐릭터를 추가해줍니다.
			INT64 i64AddCharaSlot = 0;
			for (INT32 i = 0; i < MAX_INVEN_COUNT; ++i)
			{
				// 인벤토리에만 있고 멀티슬롯에 없는 캐릭터를 추가해줍니다.
				_SyncChara(pNode, &pNode->_Inven[i], i64AddCharaSlot);
			}
		}
		pNode->_ui32State = USER_STATE_PLAYING;		// 로드 완료. Playing 상태로 전환

		//Send InfoPacket 
		SendGamePacket( pNode->_i32Serveridx, &SendPacket );
	}

	m_pDBUserLoad->OutGetLoadInfoIdx(); 

	return true;
}