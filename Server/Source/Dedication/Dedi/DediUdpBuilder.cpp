#include "pch.h"

#include "DediUdpBuilder.h"
#include "StageInfoTimer.h"
#include "InBattleDef.h"
#include "DediRoom.h"


#ifdef  CHKPOS
#undef	CHKPOS
#endif
#define CHKPOS( myType )	if( ui32SendPos + sizeof( #myType ) > (UINT)ui32BufLen )			\
							{	DSWARNING("[UDPCSBuilder::ServerPacketPacking] " #myType "\n");	\
								return 0; }

////////////////////////////////////////////////////////////////////////////////////
////								Define											//
////////////////////////////////////////////////////////////////////////////////////

// 한 패킷에 맥시멈으로 보낼 수 있는 오브젝트 카운트입니다.
#define UDPSEND_GRENADEOBJ_COUNT	6			// (Head(2) + 8) * 6 bytes	= 60
#define UDPSEND_C5OBJ_COUNT			2			// (Head(2) + 14) * 2 bytes = 32	// 92
#define UDPSEND_ANIMOBJ_COUNT		1			// (Head(2) + 7) * 1 bytes	= 9		// 101
#define UDPSEND_STATICOBJ_COUNT		4			// (Head(2) + 2) * 4 bytes	= 16	// 117
#define UDPSEND_MOVEOBJ_COUNT		1			// (Head(2) + 8) * 1 bytes	= 10	// 127
#define UDPSEND_DYNAMICOBJ_COUNT	2			// (Head(2) + 16) * 2 bytes	= 36	// 163
#define UDPSEND_WEAPONOBJ_COUNT		4			// (Head(2) + 16) * 4 bytes	= 72	// 235
#define UDPSEND_NPC_COUNT			2			// (Head(2) + 8) * 2 bytes	= 20	// 255
#define UDPSEND_CHARA_COUNT			2			

// CharaInfo 250 +
//   Object Info 255	= 505	// 한 패킷으로 보낼 수 있는 맥스 사이즈가 됩니다.
// 한 패킷에서 아래의 용량을 초과하지 않았다면 그만큼의 여유가 생깁니다. 이를 위한 사이즈입니다.
#define PACKET_RESERVESIZE_CHARACTER		250

// 블랙팬저 탱크 주면 방어벽 예외처리를 위한 테이블 (stage info - state를 설정하기 위해 사용됨
// 2011년 6월 29일.
/*
baricate_01
baricate_010
baricate_0100
baricate_01000
baricate_010000
baricate_0100000
baricate_01000000
baricate_010000000
baricate_0100000000
baricate_010000001
baricate_01000001
baricate_010000010

상태값

hp : 100% - 통상
hp : 50% - 반파
hp : 0% - 완파
*/

// ServerPacketPacking : 이 함수만 사용합니다
// pBroadcastInfo는 in/out 용도로 사용합니다
// Broadcast에서 copy
UINT32 UDPCSBuilder::PacketPackingEvent( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo, CDediRoom* pDediRoom )
{
	INT32			i32Idx				= 0;							// slot, objectIdx, ...
	UINT32			ui32WrittenBytes	= 0;							//  _Write...함수의 리턴 값
	UINT32			ui32SendPos			= 0;		// write position 조정
	BOOL			bSend				= FALSE;						// 데이터 1개 이상 구성 완료
	CDSObject*		pObject;

	// 순서대로 패키징을 해야 합니다
	
	//우선순위 높은 것부터 팩합니다. 
	//	1. P2P_SUB_HEAD_USER
	//	2. P2P_SUB_HEAD_GRENADE 
	//	3. P2P_SUB_HEAD_OBJECT_ANIM 
	//	4. P2P_SUB_HEAD_OBJECT_STATIC		
	//	5. P2P_SUB_HEAD_OBJECT_MOVE 		
	//	6. P2P_SUB_HEAD_OBJECT_DYNAMIC
	//	7. P2P_SUB_HEAD_DROPEDWEAPON 
	//	8. P2P_SUB_HEAD_NPC 

	// 1. 캐릭터 
	for( i32Idx = pBroadcastInfo->m_pNextSend_Idx[P2P_SUB_HEAD_USER]; i32Idx < SLOT_MAX_COUNT; i32Idx++)
	{
		pBroadcastInfo->m_pNextSend_Idx[P2P_SUB_HEAD_USER] = i32Idx + 1;	// 다음에 보낼 인덱스

		if( FALSE == pBroadcastInfo->m_pCastCheck_Chara[ i32Idx ].bCheckBroadcast ) continue;

		ui32WrittenBytes = _WriteGameInfo_Chara( pOutBuf + ui32SendPos, ui32BufLen, i32Idx, pBroadcastInfo, pDediRoom );

		pDediRoom->PacketDataReset( i32Idx );

		if ( ui32WrittenBytes > 0)
		{
			// Next Idx가 MAX라면 다음 프레임에서 패키징 대상이 아니다
			bSend = TRUE;
			ui32SendPos += ui32WrittenBytes;
			ui32BufLen  -= ui32WrittenBytes;

			pBroadcastInfo->m_pCastCheck_Chara[ i32Idx ].bCheckBroadcast = false;
		}
		else
		{
			if( bSend ) goto write_header;
		}
	}

	// 2. 수류탄
	ui32WrittenBytes = pBroadcastInfo->m_pThrowPacket->MakePacket( &pOutBuf[ui32SendPos], ui32BufLen );
	//if( 0 == ui32WrittenBytes )	goto write_header;
	ui32SendPos += ui32WrittenBytes;
	ui32BufLen  -= ui32WrittenBytes;
	
	// 3.0.1 Object
	for( i32Idx = 0 ; i32Idx < *pBroadcastInfo->m_pi32GameObjectCount ; i32Idx++ )
	{
		pObject	= &pBroadcastInfo->m_pGameObjects[ i32Idx ];

		if( FALSE == pObject->m_bUDPSend ) continue;
		
		if( FALSE == pObject->MakePacket( &pOutBuf[ui32SendPos], ui32BufLen, ui32WrittenBytes, SEND_PACKET_TYPE_LIVE ) )
		{
			if( bSend ) goto write_header;
		}
		
		// Event 발생 완료. 이벤트가 발생한 Idx를 기록 합니다. and Send 비활성화
		pBroadcastInfo->m_pObjectManager->RenewalObject( i32Idx );

		if( 0 < ui32WrittenBytes )
		{
			ui32SendPos += ui32WrittenBytes;
			ui32BufLen  -= ui32WrittenBytes;
		
			bSend = TRUE;
		}
	}
	
	ui32WrittenBytes = pBroadcastInfo->m_pDropPacket->MakePacket( &pOutBuf[ui32SendPos], ui32BufLen );
	//if( 0 == ui32WrittenBytes )	goto write_header;
	ui32SendPos += ui32WrittenBytes;
	ui32BufLen  -= ui32WrittenBytes;

#ifndef __USE_PACKET_OPTIMIZE__
	// 패킷의 남는 공간을 스테이지 정보로 채우기 (테스트 버전)
	ui32WrittenBytes = PacketPackingSync( &pOutBuf[ui32SendPos], ui32BufLen, pBroadcastInfo, pDediRoom, -1 );
	if ( (INT16)ui32WrittenBytes > 0)
	{
		ui32SendPos += ui32WrittenBytes;
		ui32BufLen	-= ui32WrittenBytes;
		bSend = TRUE;
	}
#endif

write_header:

	// 인덱스가 맥스를 가리키는지 검사
	if (	(pBroadcastInfo->m_pNextSend_Idx[P2P_SUB_HEAD_USER]				>= SLOT_MAX_COUNT) )
	{
		pBroadcastInfo->m_pNextSend_Idx[P2P_SUB_HEAD_USER]				= 0;
	}

	return ui32SendPos;	// 실패

}

// 캐릭터 정보 기록
UINT32 UDPCSBuilder::_WriteGameInfo_Chara( char* pOutBuf, UINT32 ui32BufLen, UINT32 ui32SlotIdx, DS_BROADCASTINFOEX * pBroadcastInfo, CDediRoom* pDediRoom )
{
	I3ASSERT( ui32BufLen > sizeof( N_GAME_SUBHEAD ) + sizeof( N_PCINFO_DEF2 ) );
	
	N_GAME_SUBHEAD*		pUserHeader = (N_GAME_SUBHEAD*)pOutBuf;
	UINT32				ui32SendPos = sizeof(N_GAME_SUBHEAD);
	UINT32				ui32WriteSize;
	UINT32				ui32InfoFlagWritten  = 0;
	CCharacterCastPacket* pCastPacket = &pBroadcastInfo->m_pCastPacket[ ui32SlotIdx ];

	// ---------------------------------------------------------------------------
	// iInfoFlag 검사 : DROPWEAPON과 FIRE데이터는 동시에 보내서는 안된다
	// ---------------------------------------------------------------------------
	UINT32&	ui32InfoFlag = pBroadcastInfo->m_pCastCheck_Chara[ui32SlotIdx].nP2PInfoFlag;
	
	if ( ui32InfoFlag & P2P_INFO_FLAG_DROPWEAPON )
		ui32InfoFlag &= ~P2P_INFO_FLAG_FIRE;

	if ( pBroadcastInfo->m_pCastCheck_bWeaponSync[ui32SlotIdx] )
	{
		pBroadcastInfo->m_pCastCheck_bWeaponSync[ui32SlotIdx] = FALSE;
		ui32InfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
	}
	
	// ---------------------------------------------------------------------------
	// default data	
	// ---------------------------------------------------------------------------
	CHKPOS( N_PCINFO_DEF2 );
	N_PCINFO_DEF2  charaDef;
	UINT32 ui32InfoPos = ui32SendPos; // 마지막에 저장하기 위함.
	ui32SendPos += sizeof( N_PCINFO_DEF2 );
	
	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_CHARASTATE
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_CHARASTATE )
	{
		CHKPOS( N_PCINFO_CHARA_STATE );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_State[ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		ui32InfoFlagWritten |= P2P_INFO_FLAG_CHARASTATE;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_ONLOADOBJECT
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_ONLOADOBJECT )
	{
		ui32WriteSize = pCastPacket->m_TakingObject.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_ONLOADOBJECT;
	}

	// ---------------------------------------------------------------------------
	// N_PCINFO_POSROTATION
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_CHARAPOS )
	{
		CHKPOS( N_PCINFO_POSROTATION );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_PosRot[ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		ui32InfoFlagWritten |= P2P_INFO_FLAG_CHARAPOS;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_ACTIONKEY
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_ACTIONKEY )
	{
		ui32WriteSize = pCastPacket->m_ActionKey.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_ACTIONKEY;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_SYNCOBJECTSTATE
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE )
	{
		ui32WriteSize = pCastPacket->m_SyncObject.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_RADIO
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_RADIO )
	{
		ui32WriteSize = pCastPacket->m_RadioChat.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_RADIO;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_SYNCWEAPON
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_SYNCWEAPON )
	{
		CHKPOS( N_PCINFO_WEAPON );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_Weapon[ ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		pBroadcastInfo->m_pCastPacket_Chara_Weapon[ ui32SlotIdx].SetIsSwap(false);
		ui32InfoFlagWritten |= P2P_INFO_FLAG_SYNCWEAPON;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_WEAPONPARAM
	// ---------------------------------------------------------------------------
	// 클라이언트에 중계할 필요 없습니다.
	/*if( ui32InfoFlag & P2P_INFO_FLAG_WEAPONPARAM )
	{
		CHKPOS( N_PCINFO_WEAPON_PARAM );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_WeaponParam[ ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		ui32InfoFlagWritten |= P2P_INFO_FLAG_WEAPONPARAM;
	}
	*/

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_MOVE_POS
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_MOVE_POS )
	{
		CHKPOS( N_PCINFO_COMMAND_MOVE );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_CommandMove[ ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		ui32InfoFlagWritten |= P2P_INFO_FLAG_MOVE_POS;
	}
	


	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_CS_LIFE
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_CS_LIFE )
	{
		CHKPOS( N_PCINFO_LIFE );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_Life[ ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		ui32InfoFlagWritten |= P2P_INFO_FLAG_CS_LIFE;
	}
	
	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_SUICIDEDAMAGE
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE )
	{
		ui32WriteSize = pCastPacket->m_SuicideDamage.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_SUICIDEDAMAGE;
	}
	
	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_MISSION
	// ---------------------------------------------------------------------------
	// 미션 정보가 있으면 미션 정보만 보낸다. 
	if( ui32InfoFlag & P2P_INFO_FLAG_MISSION )
	{
		ui32WriteSize = pCastPacket->m_Mission.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_MISSION;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_DROPWEAPON
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_DROPWEAPON )
	{
		ui32WriteSize = pCastPacket->m_DropWeapon.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_DROPWEAPON;
	}
	
	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_GETWEAPON
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_GETWEAPON )
	{
		ui32WriteSize = pCastPacket->m_GetWeapon.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_GETWEAPON;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_DIRECTPICKUP
	// ---------------------------------------------------------------------------
	if (ui32InfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
	{
		ui32WriteSize = pCastPacket->m_DirectPickUp.Pack(&pOutBuf[ui32SendPos], ui32BufLen - ui32SendPos);
		if (0 == ui32WriteSize)	return 0;

		ui32SendPos += ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_DIRECTPICKUP;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_FIRE
	// ---------------------------------------------------------------------------
	if( ui32InfoFlag & P2P_INFO_FLAG_FIRE )
	{
		ui32WriteSize = pCastPacket->m_Fire.Pack( &pOutBuf[ui32SendPos], ui32BufLen-ui32SendPos );
		if( 0 == ui32WriteSize )	return 0;

		ui32SendPos			+= ui32WriteSize;
		ui32InfoFlagWritten |= P2P_INFO_FLAG_FIRE;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_HIT
	// ---------------------------------------------------------------------------
	// Hit
	if ( ui32InfoFlag & P2P_INFO_FLAG_HITBYCHARA )	
	{
		// 수류탄일 때 안갑니다. (받은게 없음)
		//I3ASSERT( uInfoFlag & P2P_INFO_FLAG_SYNCWEAPON );
		// Attacker의 HP는 보내지 않습니다
		//I3ASSERT( uInfoFlag & P2P_INFO_FLAG_CS_LIFE );
		UINT8 nHitCount = (UINT8) pBroadcastInfo->m_pCastPacket_Chara_HitCount_ByChara[ ui32SlotIdx];

		if( nHitCount > 0 && nHitCount <= MAX_FIREHIT_COUNT )
		{
			CHKPOS(UINT8);
			*(UINT8*)(pOutBuf + ui32SendPos) = (UINT8)nHitCount;
			ui32SendPos++;
		
			//Set Data
			for( UINT32 i = 0; i < nHitCount; i++ )
			{
				CHKPOS( N_PCINFO_HIT_DEDICATED2 );
				ui32SendPos += pBroadcastInfo->m_ppCastPacket_Chara_Hit_ByChara[ui32SlotIdx][i].CopyToPacket(&pOutBuf[ui32SendPos]);

			}

			ui32InfoFlagWritten |= P2P_INFO_FLAG_HITBYCHARA;
		}
	}

	if ( ui32InfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION )
	{
		UINT8 nHitCount = (UINT8) pBroadcastInfo->m_pCastPacket_Chara_HitCount_ByExplosion[ui32SlotIdx];
		if( nHitCount > 0 && nHitCount <= MAX_FIREHIT_COUNT )
		{
			CHKPOS(UINT8);
			*(UINT8*)(pOutBuf + ui32SendPos) = (UINT8)nHitCount;
			ui32SendPos++;
		
			//Set Data
			for( UINT32 i = 0; i < nHitCount; i++ )
			{
				CHKPOS( N_PCINFO_HIT_BYEXPLOSION );
				ui32SendPos += pBroadcastInfo->m_ppCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i].CopyToPacket(&pOutBuf[ui32SendPos] );
			}

			ui32InfoFlagWritten |= P2P_INFO_FLAG_HITBYEXPLOSION;
		}
	}
	
	if ( ui32InfoFlag & P2P_INFO_FLAG_HITBYOBJECT )
	{
		/* INT16 hp = pBroadcastInfo->m_pCastPacket_Chara_Life[ ui32SlotIdx].iLife;
		I3TRACE("P2P_INFO_FLAG_HITBYOBJECT MERGED: slot index = %d, hp=%d\n", ui32SlotIdx, hp); 
		*/
		UINT8 nHitCount = (UINT8) pBroadcastInfo->m_pCastPacket_Chara_HitCount_ByObject[ ui32SlotIdx];

		if( nHitCount > 0 && nHitCount <= MAX_FIREHIT_COUNT )
		{
			CHKPOS(UINT8);
			*(UINT8*)(pOutBuf + ui32SendPos) = (UINT8)nHitCount;
			ui32SendPos++;
		
			//Set Data
			for( UINT32 i = 0; i < nHitCount; i++ )
			{
				CHKPOS( N_PCINFO_HIT_BYOBJECT );
				ui32SendPos += pBroadcastInfo->m_ppCastPacket_Chara_Hit_ByObject[ui32SlotIdx][i].CopyToPacket(&pOutBuf[ui32SendPos] );
			}

			ui32InfoFlagWritten |= P2P_INFO_FLAG_HITBYOBJECT;
		}
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_CS_DEATH
	// ---------------------------------------------------------------------------
	// Death
	if( ui32InfoFlag & P2P_INFO_FLAG_CS_DEATH )
	{
		I3ASSERT( (ui32InfoFlag & P2P_INFO_FLAG_CS_LIFE) != 0);

		CHKPOS( N_PCINFO_DEATH );
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_Death[ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
		ui32InfoFlagWritten |= P2P_INFO_FLAG_CS_DEATH;
	}

	// ---------------------------------------------------------------------------
	// P2P_INFO_FLAG_CS_SUFFERING
	// ---------------------------------------------------------------------------
	// Suffering
	if( ui32InfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
	{	
		if ( (ui32InfoFlag & P2P_INFO_FLAG_CS_DEATH) == 0 )
		{
			CHKPOS( N_PCINFO_SUFFER );
			ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_Suffering[ ui32SlotIdx].CopyToPacket(&pOutBuf[ui32SendPos] );
			ui32InfoFlagWritten |= P2P_INFO_FLAG_CS_SUFFERING;

			I3TRACE("MERGE: suffering \n");
		}
	}

	// info flag
	charaDef.iInfoFlag = ui32InfoFlagWritten;
	charaDef.CopyToPacket( &pOutBuf[ ui32InfoPos] );
	
	// ---------------------------------------------------------------------------
	// Header
	// ---------------------------------------------------------------------------
	pUserHeader->_Type	= P2P_SUB_HEAD_USER;
	pUserHeader->_Index	= (UINT8) ui32SlotIdx;						// 캐릭터의 경우 슬롯 번호가 된다
	pUserHeader->_Code  = static_cast<UINT16>(ui32SendPos);			// 서브 패킷의 사이즈

	ui32InfoFlag = 0; // reference type!

	
	
	return ui32SendPos;
}

UINT32 UDPCSBuilder::PacketPackingSync( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo, CDediRoom* pDediRoom, INT32 i32SlotIdx )
{
	UINT32 ui32SendPos = 0;
	UINT32 ui32Bytes = 0;
	
	CStageInfoTimer* pTimer = pDediRoom->getStageInfoTimer();

	// 오브젝트 정보를 전송
	// 보낼 필요가 없는 경우에도 0을 리턴한다
	UINT32 ui32Flag = pTimer->SelectTimer( pBroadcastInfo->m_r32GameTime );
	
	if( ui32Flag & ( 1 << SEND_HEAD_TYPE_CHAR ))
	{
		ui32Bytes = _MakeStageInfo_Chara( &pOutBuf[ui32SendPos], ui32BufLen, pBroadcastInfo );
		ui32SendPos += ui32Bytes;
		ui32BufLen  -= ui32Bytes;
	}

	if( pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_CHARA ] == SLOT_MAX_COUNT )
	{
		pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_CHARA ]	= 0; 
	}
	
	ui32Bytes = _MakeStageInfo_Object( ui32Flag, &pOutBuf[ui32SendPos], ui32BufLen, pBroadcastInfo, pDediRoom, i32SlotIdx );
	ui32SendPos += ui32Bytes;
	ui32BufLen  -= ui32Bytes;	

	if( StageID::GetStageMode( pBroadcastInfo->m_ui32StageID ) == STAGE_MODE_BOMB )
	{
		if( ui32Flag & ( 1 << SEND_HEAD_TYPE_MISSION ) )
		{
			ui32Bytes = _MakeStageInfo_Mission		( &pOutBuf[ui32SendPos], ui32BufLen, pBroadcastInfo );	
			ui32SendPos += ui32Bytes;
			ui32BufLen  -= ui32Bytes;
		}
	}	
	
	if( pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_MISSION ]	== 1 )   // 완료
	{
		pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_MISSION ]	= 0; 
	}

	return ui32SendPos;
}

UINT32 UDPCSBuilder::PacketPackingHead( char* pOutBuf, UINT32 ui32SendPos, DS_BROADCASTINFOEX* pBroadcastInfo )
{
	I3ASSERT( ui32SendPos > sizeof(N_GAME_HEADER) );

	N_GAME_HEADER* pHead = (N_GAME_HEADER*) pOutBuf;

	// 헤더
	pHead->Reset();
	pHead->_ProtocolID			= UDPPROTOCOL_CS_INFO;
	pHead->_Index				= SLOT_DEDICATION_IDX;				// 방장인덱스는 항상 0. dedication client는 방을 만든 유저 인덱스를 유지해야 함.
	pHead->_GameTime			= pBroadcastInfo->m_r32GameTime;
	pHead->_RoundNum			= pBroadcastInfo->m_ui8RoundNum;
	pHead->_Size				= (UINT16)ui32SendPos;
#ifdef __USE_PACKET_OPTIMIZE__
	pHead->m_ui8PacketUpdateIdx = pBroadcastInfo->m_ui8PacketUpdateIdx;
#endif

	return 0;
}

UINT32	UDPCSBuilder::_MakeStageInfo_Chara( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo )
{
	UINT32	ui32SendPos = 0;
	UINT16	ui16Written = 0;
	INT32	i32Count = 0;
	INT32	i32MaxCount = UDPSEND_CHARA_COUNT; // 한번에 2개만 보냄
	N_GAME_SUBHEAD* pUserHeader = NULL;

	for (INT32 i32Idx = pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_CHARA ]; i32Idx < SLOT_MAX_COUNT; i32Idx++ )
	{
		if( UDP_UTIL::IsPackSize( P2P_SUB_HEAD_STAGEINFO_CHARA, (UINT16)ui32SendPos, (UINT16)ui32BufLen ) == false)
		{
			pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_CHARA ] = i32Idx;
			return ui32SendPos;
		}
	
		// position과 life를 보낸다
		// life만 보내도록 수정.
		pUserHeader = (N_GAME_SUBHEAD*)&pOutBuf[ui32SendPos ];					// save pos
		ui32SendPos += sizeof(N_GAME_SUBHEAD);

		N_PCINFO_DEF2  charaDef;

		//charaDef.iInfoFlag = ( P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CS_LIFE);	// def
		charaDef.iInfoFlag = P2P_INFO_FLAG_CS_LIFE;
		ui32SendPos += charaDef.CopyToPacket( &pOutBuf[ ui32SendPos ] );
					
		//ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_PosRot[ i32Idx ].CopyToPacket( &pOutBuf[ ui32SendPos ] );	//pos
		ui32SendPos += pBroadcastInfo->m_pCastPacket_Chara_Life[ i32Idx ].CopyToPacket  ( &pOutBuf[ ui32SendPos ] );	//life
					
		// build header
		ui16Written = (UINT16)( &pOutBuf[ui32SendPos] - (char*)pUserHeader);
		pUserHeader->_Code  = ui16Written;
		pUserHeader->_Index = (UINT16) i32Idx;
		pUserHeader->_Type  = P2P_SUB_HEAD_STAGEINFO_CHARA;

		ui32BufLen -= ui16Written;
		//I3TRACE("added stageinfo: chara[%d] hp=%d\n", i32Idx, pBroadcastInfo->m_pCastPacket_Chara_Life[ i32Idx ].iLife );
		
		// write next index
		pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_CHARA ] = i32Idx + 1;
		
		if ( ++i32Count > i32MaxCount) break;
	}

	//I3TRACE( "send intrude stageinfo static object ok.(count:%d)\n", ui32Count );

	return ui32SendPos;
}

// 폭탄만 사용합니다. 
UINT32 UDPCSBuilder::_MakeStageInfo_Mission( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo )
{
	
	UINT32	ui32SendPos = 0;
	UINT32	ui32Written = 0;

	UINT16  ui16SlotIdx = 0;
	UINT8	ui8State = 0;
	//VEC3D*	pPos = NULL;
	UINT32	ui32Count = 0;
	
	// check count
	if ( pBroadcastInfo->m_pStageObj_Count[ P2P_SUB_HEAD_STAGEINFO_MISSION ] <= 0 ) return 0;

	// 1개만 사용된다
	for (INT32 i32Idx = pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_MISSION ];
		 i32Idx < pBroadcastInfo->m_pStageObj_Count[ P2P_SUB_HEAD_STAGEINFO_MISSION ]; i32Idx++ )
	{
		if( UDP_UTIL::IsPackSize( P2P_SUB_HEAD_STAGEINFO_MISSION, (UINT16)ui32SendPos, (UINT16)ui32BufLen ) == false)
		{
			pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_MISSION ] = i32Idx;
			return ui32SendPos;
		}

		ui8State  = pBroadcastInfo->m_pMissionIntruder->getInstallMode();
		if( ui8State == 0)
		{
			pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_MISSION ] = i32Idx + 1;
			return ui32SendPos;
		}
		
		ui32Written = pBroadcastInfo->m_pMissionIntruder->PackFull( &pOutBuf[ ui32SendPos], ui32BufLen, P2P_SUB_HEAD_STAGEINFO_MISSION, ui16SlotIdx );
		ui32BufLen  -= ui32Written;
		ui32SendPos += ui32Written;		

		pBroadcastInfo->m_pNextSend_Idx[ P2P_SUB_HEAD_STAGEINFO_MISSION ] = i32Idx + 1;

		ui32Count++;		
	}

	//I3TRACE( "send intrude stageinfo mission ok.(count:%d)\n", ui32Count );
	
	return ui32SendPos;
}

UINT32 UDPCSBuilder::_MakeStageInfo_Object( UINT32 ui32CastFlag, char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo, CDediRoom* pDediRoom, INT32 i32SlotIdx )
{
	UINT32		ui32SendPos			= 0;
	UINT32		ui32WrittenByte;
	CDSObject*	pObject;

#ifdef __USE_PACKET_OPTIMIZE__

	UINT32 ui32UpdateTime	= pDediRoom->GetUpdateIdx( i32SlotIdx );
	ui32WrittenByte = pBroadcastInfo->m_pObjectManager->MakePacket( &pOutBuf[ ui32SendPos ], ui32BufLen, SEND_PACKET_TYPE_SYNC, ui32UpdateTime );
	
	ui32BufLen  -= ui32WrittenByte;
	ui32SendPos += ui32WrittenByte;

	pBroadcastInfo->m_ui8PacketUpdateIdx = pDediRoom->SetUpdateIdx( i32SlotIdx, ui32UpdateTime );

#else
	while( pBroadcastInfo->m_i32NextSendIdx < *pBroadcastInfo->m_pi32GameObjectCount )
	{
		pObject			= &pBroadcastInfo->m_pGameObjects[ pBroadcastInfo->m_i32NextSendIdx ];

		if( ui32CastFlag & ( 1 << pObject->m_eSendHeadType ) )
		{
			if( FALSE == pObject->MakePacket( &pOutBuf[ui32SendPos], ui32BufLen, ui32WrittenByte, SEND_PACKET_TYPE_SYNC ) )
			{
				break;
			}

			ui32BufLen  -= ui32WrittenByte;
			ui32SendPos += ui32WrittenByte;
		}
		
		pBroadcastInfo->m_i32NextSendIdx++;
	}

	if( pBroadcastInfo->m_i32NextSendIdx >= *pBroadcastInfo->m_pi32GameObjectCount )
	{
		pBroadcastInfo->m_i32NextSendIdx = 0;
	}
#endif
	return ui32SendPos;
}