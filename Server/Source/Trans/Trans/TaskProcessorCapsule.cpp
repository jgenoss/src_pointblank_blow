#include "pch.h"
#include "ModuleSIA.h"
#include "TaskProcessor.h"

boost::BOOST_RANDOM_ENGINE	g_RandomEngine;
		 
BOOL	CTaskProcessor::LoadCapsule( i3IniParser* pParser, CCapsuleItem* pCapsule )
{
	wchar_t wstrLog[MAX_PATH];
	UINT32	ui32ItemID				= 0;
	INT32	i32MaxProb				= 0;
	INT32	i32MaxCount				= 0;
	INT32	i32Probability			= 0;
	char	szCapsuleTemp[ MAX_PATH ];
	INT32	i32RewardCount			= 0;
	UINT32	ui32RewardItemID		= 0;
	UINT32	ui32RewardItemArgCount	= 0; // INI 2.0용	Count와 Period 형태의 두가지를 다 읽어들이도록 변경.	2011.09.05 안국정
	UINT32	ui32RewardItemArgPeriod	= 0; // INI 2.0용
	UINT32	ui32RewardItemArg		= 0; // INI 1.0용
	INT32	i32Jackpot				= 0;
	
	if (FALSE == pParser->ReadSection("Info"))
	{
		i3String::Format(wstrLog, MAX_PATH, L"[CTaskProcessor::LoadCapsule()]  Info Error FileName : %s", pParser->GetFileName());
		g_pLog->WriteLog(wstrLog);
		return FALSE;
	}	
	pParser->GetValue( "ItemID",	ui32ItemID,		&ui32ItemID );
	pParser->GetValue( "Count",		i32MaxCount,	&i32MaxCount );
	pParser->GetValue( "Prob_Max",	i32MaxProb,		&i32MaxProb );

	if (FALSE == pCapsule->Create(ui32ItemID, i32MaxProb, i32MaxCount, pParser->GetFileName()))
	{
		i3String::Format(wstrLog, MAX_PATH, L"[CTaskProcessor::LoadCapsule()]  pCapsule->Creat Error FileName : %s", pCapsule->GetName());
		g_pLog->WriteLog(wstrLog);
		return FALSE;
	}
	for( INT32 i = 0 ; i < i32MaxCount ; i++ )
	{
		// 확률 입력
		sprintf( szCapsuleTemp, "Prob_%02d", i );
		pParser->GetValue( szCapsuleTemp, i32Probability, &i32Probability);
		pCapsule->Insert( i32Probability, (UINT8)i );

		// 지급할 아이템 세팅
		sprintf( szCapsuleTemp, "CP_%02d_Count", i );
		pParser->GetValue( szCapsuleTemp, i32RewardCount, &i32RewardCount);
		sprintf( szCapsuleTemp, "CP_%02d_Jackpot", i );
		pParser->GetValue( szCapsuleTemp, i32Jackpot, &i32Jackpot);

		pCapsule->SetReward( i, i32RewardCount, i32Jackpot );
		for( INT32 j = 0 ; j < i32RewardCount ; j++ )
		{
			// 구버전과 신버전 공통부분
			sprintf( szCapsuleTemp, "CP_%02d_%02d_Item", i, j );
			pParser->GetValue( szCapsuleTemp, ui32RewardItemID, &ui32RewardItemID );

			// 구버전(1.0)방식으로 읽어보고 버전 판별
			sprintf( szCapsuleTemp, "CP_%02d_%02d_Arg", i, j );
			BOOL bResult = pParser->GetValue( szCapsuleTemp, ui32RewardItemArg, &ui32RewardItemArg );
			
			// 구버전 읽기가 성공시 1.0으로 판별
			if ( TRUE == bResult )
			{
				pCapsule->m_i8INIVersion = 1;
				pCapsule->InsertReward10( i, j, ui32RewardItemID, ui32RewardItemArg );
			}
			// 읽기 실패시 2.0으로 판별
			else
			{
				pCapsule->m_i8INIVersion = 2;
				
				// 포인트 아이템일 경우엔 ItemArg가 필요없으므로 더 이상 파싱하지 않습니다.
				if( ITEM_TYPE_POINT == GET_ITEM_TYPE( ui32RewardItemID ) )
				{
					pCapsule->InsertReward( i, j, ui32RewardItemID, 0, 0 );	
				}
				// 포인트 아이템이 아닐 경우엔 갯수제와 기간제의 2가지 형태로 두번 파싱합니다.
				else
				{
					sprintf( szCapsuleTemp, "CP_%02d_%02d_Arg_Count", i, j );
					pParser->GetValue( szCapsuleTemp, ui32RewardItemArgCount, &ui32RewardItemArgCount );

					sprintf( szCapsuleTemp, "CP_%02d_%02d_Arg_Period", i, j );
					pParser->GetValue( szCapsuleTemp, ui32RewardItemArgPeriod, &ui32RewardItemArgPeriod );

					pCapsule->InsertReward( i, j, ui32RewardItemID, ui32RewardItemArgCount, ui32RewardItemArgPeriod );
				}
			}
		}
	}
	if (i32MaxProb != pCapsule->GetCountMax())
	{
		i3String::Format(wstrLog, MAX_PATH, L"[CTaskProcessor::LoadCapsule()]  i32MaxProb != pCapsule->GetCountMax() FileName : %s", pCapsule->GetName());
		g_pLog->WriteLog(wstrLog);
		return FALSE;
	}
	pCapsule->ShuffleMix();

	return TRUE;
}

BOOL	CTaskProcessor::CreateCapsule()
{
	i3IniParser Parser;
	char		strDir[SERVER_STRING_COUNT];
	char		strFile[SERVER_STRING_COUNT];
	char		strSearch[SERVER_STRING_COUNT];
	HANDLE		hFile;
	WIN32_FIND_DATA	FileInformation;
	wchar_t wstrLog[MAX_PATH];

	i3String::Format( strDir, SERVER_STRING_COUNT, "%s\\Capsule", g_pCCurrentPath );
	i3String::Format( strSearch, SERVER_STRING_COUNT, "%s\\*.ini", strDir );

	m_i32CapsuleCount	= 0;
	hFile = ::FindFirstFile( strSearch, &FileInformation );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
			m_i32CapsuleCount++;
		} while( ::FindNextFile( hFile, &FileInformation ) );

		::FindClose( hFile );
	}
	else
	{
		i3String::Format(wstrLog, MAX_PATH, L"[CTaskProcessor::CreateCapsule()]  hFile == INVALID_HANDLE_VALUE / filecount : %d / FileName : %s", m_i32CapsuleCount, FileInformation.cFileName);
		g_pLog->WriteLog(wstrLog);
		return FALSE;
	}

#ifdef I3_DEBUG
	m_pi32CapsuleIdx	= (INT32*)i3MemAlloc(sizeof(INT32) * m_i32CapsuleCount);
#endif
	m_ppCapsule			= (CCapsuleItem**)i3MemAlloc(sizeof(CCapsuleItem*) * m_i32CapsuleCount);
	// MagaBomb
	hFile = ::FindFirstFile( strSearch, &FileInformation );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		INT32 i = 0;
		do
		{
			if ( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
			i3String::Format( strFile, SERVER_STRING_COUNT, "%s\\%s", strDir, FileInformation.cFileName );
			if( Parser.OpenFromFile( strFile ))
			{		
				m_ppCapsule[i] = CCapsuleItem::NewObject();
				if( FALSE == LoadCapsule( &Parser, m_ppCapsule[i] ) )		
				{
					wchar_t wstrFileName[ MAX_PATH ];
					MultiByteToWideChar( CP_ACP, 0, FileInformation.cFileName, -1, wstrFileName, MAX_PATH );

					
					i3String::Format( wstrLog, MAX_PATH, L"[CTaskProcessor::CreateCapsule()] Fail LoadCapsule : %d / CapsuleFileName : %s", i, wstrFileName );
					g_pLog->WriteLog( wstrLog  );
					return FALSE;
				}
				Parser.Close();
			}
			i++;
		} while( ::FindNextFile( hFile, &FileInformation ) );

		::FindClose( hFile );
	}
	else
	{
		i3String::Format(wstrLog, MAX_PATH, L"[CTaskProcessor::CreateCapsule()]  hFile == INVALID_HANDLE_VALUE 22 / CapsuleFileName : %s ", FileInformation.cFileName);
		g_pLog->WriteLog(wstrLog);
		return FALSE;
	}

	g_RandomEngine.seed( GetTickCount() );

	SetCapsuleLoad(CAPSULE_LOAD_INI);

	return TRUE;
}

void	CTaskProcessor::SetCapsule( USER_NODE* pUserNode, UINT32 ui32ItemID )
{
	CCapsuleItem* pCapsule	= NULL;
#ifdef I3_DEBUG
	INT32	i32DebugIdx = 0;
#endif
	for( INT32 i = 0 ; i < m_i32CapsuleCount ; i++ )
	{
		if( NULL == m_ppCapsule[i] )						continue;
		if( ui32ItemID != m_ppCapsule[i]->GetItemID() )		continue;
		pCapsule	= m_ppCapsule[i];
#ifdef I3_DEBUG
		i32DebugIdx	= m_pi32CapsuleIdx[i];
		m_pi32CapsuleIdx[i]++;
		m_pi32CapsuleIdx[i] %= pCapsule->GetItemCount();
#endif
		break;
	}
	
	if( NULL == pCapsule )	return;

	UINT8	ui8CapsuleIdx	= pCapsule->Rand();
#ifdef I3_DEBUG
	ui8CapsuleIdx = (UINT8)i32DebugIdx;
#endif
	UINT32	ui32Point		= 0;
	UINT8	ui8RewardCount	= (UINT8)pCapsule->GetRewardCount( ui8CapsuleIdx );
	INT32	i32JackPot		= pCapsule->GetRewardJackPot( ui8CapsuleIdx );
	
	ST_ITEM*	pDBItem;

	UINT8		ui8RewardItemCount = 0;
	ST_ITEM		pRewardItem[ MAX_SHOP_DB_ITEM_COUNT ];

	UINT8		ui8PacketItemCount = 0;
	ST_ITEM		pPacketItem[MAX_SHOP_DB_ITEM_COUNT];

	for( INT32 i = 0 ; i < ui8RewardCount ; i++ )
	{
		pDBItem = &pRewardItem[ ui8RewardItemCount ];

		pDBItem->m_TItemID	= pCapsule->GetRewardItemID( ui8CapsuleIdx, i );

		if( ITEM_TYPE_POINT == GET_ITEM_TYPE( pDBItem->m_TItemID ) )
		{
			ui32Point				+= GET_POINTITEM( pDBItem->m_TItemID );
			pDBItem->m_TItemArg		= 1;
		}
		else
		{
			SHOP_ITEM_BASE* pItemData = g_pShop->GetItemData( pDBItem->m_TItemID );
			if ( pItemData == NULL ) 
			{
				wchar_t wstrName[ MAX_PATH ];
				MultiByteToWideChar( CP_ACP, 0, pCapsule->GetName(), -1, wstrName, MAX_PATH );

				g_pLog->WriteLog( L"[CAPSULE_REWARD_ERROR] UID:%I64d, CapsuleName:%s, CapsuleItemID:%u, ItemID:%u, ArgCount:%u, ArgPeriod:%u, RandIdx:%u, Num:%d/%d",
					pUserNode->_i64UID, wstrName, ui32ItemID, pDBItem->m_TItemID, pCapsule->GetRewardItemArgCount( ui8CapsuleIdx, i ), pCapsule->GetRewardItemArgPeriod( ui8CapsuleIdx, i ), ui8CapsuleIdx, i+1, ui8RewardCount );
				continue;
			}

			switch( pItemData->_ui8AuthType )
			{
			case AUTH_COUNT:
				pDBItem->m_TItemArg = pCapsule->GetRewardItemArgCount( ui8CapsuleIdx, i );
				break;
			case AUTH_SEC:
				pDBItem->m_TItemArg = pCapsule->GetRewardItemArgPeriod( ui8CapsuleIdx, i ); 
				break;
			default:
				g_pLog->WriteLog( L"[CAPSULE_PAYMENT_ERROR]ItemID:%d's AuthType must be %d or %d. but Now its AuthType is %d.(CapsuleItemID:%d)",
					pDBItem->m_TItemID, AUTH_COUNT, AUTH_SEC, pItemData->_ui8AuthType, ui32ItemID );
				continue;
				break;
			}

			pPacketItem[ ui8PacketItemCount ].m_TItemID					= pDBItem->m_TItemID;
			pPacketItem[ ui8PacketItemCount ].m_TItemArg				= pDBItem->m_TItemArg;
			ui8PacketItemCount++;
		}
		ui8RewardItemCount++;
	}

	_ItemInsertDB( pUserNode->_i64UID, ITEM_INSERT_REQUEST_CAPSULE, ui8PacketItemCount, pPacketItem, ui32ItemID);
	
	i3NetworkPacket	SendPacket( PROTOCOL_AUTH_SHOP_CAPSULE_ACK );
	SendPacket.WriteData( &pUserNode->_i32Sessionidx,	sizeof( INT32 )	);
	SendPacket.WriteData( &pUserNode->_i64UID,			sizeof( T_UID )	);
	SendPacket.WriteData( &ui32ItemID,					sizeof( UINT32 ));
	SendPacket.WriteData( &ui8CapsuleIdx,				sizeof( UINT8 )	);
	SendPacket.WriteData( &ui8RewardItemCount,			sizeof( UINT8 )	);
	SendPacket.WriteData( pRewardItem,					sizeof( ST_ITEM )*ui8RewardItemCount );
	SendGamePacket( pUserNode->_i32Serveridx, &SendPacket );

	if( i32JackPot )
	{
		UINT8 ui8Size = (UINT8)i3String::Length( pUserNode->_sBasic.m_strNick )+1;
		i3NetworkPacket	SendPacket2( PROTOCOL_AUTH_SHOP_JACKPOT_ACK );
		SendPacket2.WriteData( &ui8Size,						sizeof( UINT8 )	);
		SendPacket2.WriteData( pUserNode->_sBasic.m_strNick,	sizeof( TTCHAR) * ui8Size	);
		SendPacket2.WriteData( &ui32ItemID,						sizeof( UINT32 ) );
		SendPacket2.WriteData( &ui8CapsuleIdx,					sizeof( UINT8 )	);
		SendAllGamePacket( &SendPacket2 );
	}

	if( 0 < ui32Point )
	{
		pUserNode->_sBasic.m_ui32Point	+= ui32Point;
		_PlusPoint( MODULE_SIA_THREAD_IDX, pUserNode->_i32Serveridx, pUserNode->_i32Sessionidx, pUserNode->_i64UID, ui32Point, GET_POINT_ROUTE_ITEM );
	}
}

void CTaskProcessor::SetCapsuleCount(USER_NODE * pUserNode, UINT32 ui32ItemID, TLoopCnt tCount)
{
	CCapsuleItem* pCapsule = nullptr;
#ifdef I3_DEBUG
	INT32	i32DebugIdx = 0;
#endif
	// Capsule을 픽하는 것이므로 루프 돌릴 필요가 없습니다.
	for (INT32 i = 0; i < m_i32CapsuleCount; i++)
	{
		if (nullptr		== m_ppCapsule[i])				continue;
		if (ui32ItemID	!= m_ppCapsule[i]->GetItemID())	continue;
		pCapsule	= m_ppCapsule[i];
#ifdef I3_DEBUG
		i32DebugIdx = m_pi32CapsuleIdx[i];
		m_pi32CapsuleIdx[i]++;
		m_pi32CapsuleIdx[i] %= pCapsule->GetItemCount();
#endif
		break;
	}

	if (nullptr == pCapsule)	return;

	T_UID   Tuid = pUserNode->_i64UID;
	UINT32	ui32Point		= 0;
	INT32	i32JackPot		= 0;

	// 여유 분으로 2배의 사이즈를 할당 합니다.
	const UINT8 MAX_CAPSULE_INS_BUFFER_COUNT = static_cast<UINT8>(MAX_SHOP_DB_ITEM_COUNT * 2);

	UINT8		ui8PacketIdx		= 0;
	UINT8		ui8PacketItemCount	= 0;
	ST_ITEM		pPacketItem[MAX_CAPSULE_INS_BUFFER_COUNT];
	i3mem::FillZero(pPacketItem, sizeof(pPacketItem));

	for (TLoopCnt TLoop = 0; TLoop < tCount; ++TLoop)
	{		
		bool bNewIdx = true;
		UINT8	ui8CapsuleIdx = pCapsule->Rand();
#ifdef I3_DEBUG
		ui8CapsuleIdx = (UINT8)i32DebugIdx;
#endif

		UINT8	ui8RewardCount	= (UINT8)pCapsule->GetRewardCount(ui8CapsuleIdx);
		i32JackPot			   += pCapsule->GetRewardJackPot(ui8CapsuleIdx);

		for (INT32 i = 0; i < ui8RewardCount; i++)
		{
			T_ItemID t_NewItemID = pCapsule->GetRewardItemID(ui8CapsuleIdx, i);

			// Packet Item Array에서 이전에 넣어둔 아이템 번호인지 검사합니다.
			for (UINT8 j = 0; j < ui8PacketItemCount; ++j)
			{
				if ( pPacketItem[j].m_TItemID == t_NewItemID )
				{
					ui8PacketIdx = j;
					bNewIdx = false;
					break;
				}
			}

			// 이전에 넣은적이 없는 아이템이라면, 새로이 인덱스를 받습니다.
			if ( bNewIdx )
			{
				// Array가 가득 찼는데, 새로운 타입의 아이템이면 Insert 패킷 전송해 버립니다.
				if ( ui8PacketItemCount == MAX_CAPSULE_INS_BUFFER_COUNT )
				{
					_InsertItemCountBuffer(Tuid, ITEM_INSERT_REQUEST_CAPSULE, pPacketItem, ui8PacketItemCount, MAX_SHOP_DB_ITEM_COUNT, ui32ItemID);

					ui8PacketItemCount = 0;
					i3mem::FillZero(pPacketItem, sizeof(pPacketItem));
				}
				// 새로운 인덱스 번호를 할당 받습니다. (새로 등록하는 아이템인 경우임. 포인트도 될 수 있다.)
				ui8PacketIdx = ui8PacketItemCount;
				// PacketItemCount 증가는 여기서 하지 않습니다. 포인트 타입 일 수 있습니다.
			}

			if ( ITEM_TYPE_POINT == GET_ITEM_TYPE(t_NewItemID) )
			{
				ui32Point += GET_POINTITEM(t_NewItemID);
			}
			else
			{
				// Idx는 유효한 상태입니다.
				ST_ITEM*	pDBItem = &pPacketItem[ui8PacketIdx];

				SHOP_ITEM_BASE* pItemData = g_pShop->GetItemData(t_NewItemID);
				if (pItemData == nullptr)
				{
					wchar_t wstrName[MAX_PATH];
					MultiByteToWideChar(CP_ACP, 0, pCapsule->GetName(), -1, wstrName, MAX_PATH);

					g_pLog->WriteLog(L"[CAPSULE_LOOP_REWARD_ERROR] UID:%I64d, CapsuleName:%s, CapsuleItemID:%u, ItemID:%u, ArgCount:%u, ArgPeriod:%u, RandIdx:%u, Num:%d/%d",
						pUserNode->_i64UID, wstrName, ui32ItemID, t_NewItemID, pCapsule->GetRewardItemArgCount(ui8CapsuleIdx, i), pCapsule->GetRewardItemArgPeriod(ui8CapsuleIdx, i), ui8CapsuleIdx, i + 1, ui8RewardCount);
					continue;
				}

				T_ItemArg tNewArg = 0;
				switch (pItemData->_ui8AuthType)
				{
				case AUTH_COUNT:
					tNewArg = pCapsule->GetRewardItemArgCount(ui8CapsuleIdx, i);
					break;
				case AUTH_SEC:
					tNewArg = pCapsule->GetRewardItemArgPeriod(ui8CapsuleIdx, i);
					break;
				default:
					g_pLog->WriteLog(L"[CAPSULE_LOOP_PAYMENT_ERROR]ItemID:%d's AuthType must be %d or %d. but Now its AuthType is %d.(CapsuleItemID:%d)",
						pDBItem->m_TItemID, AUTH_COUNT, AUTH_SEC, pItemData->_ui8AuthType, ui32ItemID);
					continue;
					break;
				}
				// 새로운 타입이 아이템이 맞으므로 count 증가 시켜줍니다.
				pDBItem->m_TItemArg += tNewArg;

				if (bNewIdx)
				{
					pDBItem->m_TItemID = t_NewItemID;
					ui8PacketItemCount++;
				}
				//g_pLog->WriteLog(L"Capsule On! (N:%d) [ID:%d] [Auth:%d / NewAuth:%d] ", bNewIdx, pDBItem->m_TItemID, pDBItem->m_TItemArg, tNewArg);
			} // is point?
		} // reward count
	} // tloop
	
	if (0 < ui8PacketItemCount)
		_InsertItemCountBuffer(Tuid, ITEM_INSERT_REQUEST_CAPSULE, pPacketItem, ui8PacketItemCount, MAX_SHOP_DB_ITEM_COUNT, ui32ItemID);

	// 여러 개 사용하므로 따로 Capsule Ack 보내지 않는다.
	if (0 < ui32Point)
	{
		pUserNode->_sBasic.m_ui32Point += ui32Point;
		_PlusPoint(MODULE_SIA_THREAD_IDX, pUserNode->_i32Serveridx, pUserNode->_i32Sessionidx, Tuid, ui32Point, GET_POINT_ROUTE_ITEM);
	}
}

void CTaskProcessor::_InsertItemCountBuffer(T_UID ui64UID, ITEM_INSERT_REQUEST_TYPE req, ST_ITEM * pItemArr, UINT8 ui8BufferCount, UINT8 ui8BufferMaxCnt, UINT32 ui32CapsuleItemID)
{
	UINT8 ui8LeftBufferNum = ui8BufferCount;
	for (UINT8 iSend = 0; iSend < ui8BufferCount; iSend += ui8BufferMaxCnt)
	{
		if (ui8LeftBufferNum <= 0)
			break;

		ST_ITEM * pItem			= pItemArr + iSend;
		UINT8 ui8NowBufferNum	= i3::clamp<UINT8>(ui8LeftBufferNum, 0, ui8BufferMaxCnt);

#if 0
		g_pLog->WriteLog(L"Insert DB Arr! Count : %d <5> [ID:%d %d %d %d %d] [Auth:%d %d %d %d %d]",
			ui8NowBufferNum,
			pItem[0].m_TItemID,  pItem[1].m_TItemID,  pItem[2].m_TItemID,  pItem[3].m_TItemID,  pItem[4].m_TItemID,
			pItem[0].m_TItemArg, pItem[1].m_TItemArg, pItem[2].m_TItemArg, pItem[3].m_TItemArg, pItem[4].m_TItemArg
		);
#endif

		_ItemInsertDB(ui64UID, req, ui8NowBufferNum, pItem, ui32CapsuleItemID);
		ui8LeftBufferNum	-= ui8NowBufferNum;
	}
}
