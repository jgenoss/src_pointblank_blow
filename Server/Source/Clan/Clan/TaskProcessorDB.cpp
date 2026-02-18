#include "pch.h"
#include "TaskProcessor.h"
#include "Gateway.h"
//#include "LogFile.h"

void	CTaskProcessor::_DoTaskForDB( RING_OUT_DB* pOutDB )
{
	switch( pOutDB->_ui8Type )
	{
	case RING_DB_USER_LOGIN:
		{
			UINT32	ui32CID		= 0;
			UINT8	ui8Level	= CLAN_MEMBER_LEVEL_UNKNOWN;
			CLAN_DETAIL_INFO_SERVER* pstClan = NULL;
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				pOutDB->Reset();
				pOutDB->Read( &ui32CID,		sizeof(UINT32) );
				pOutDB->Read( &ui8Level,	sizeof(UINT8) );

				pstClan = FindClan2( ui32CID );
				if( NULL == pstClan )
				{
					pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

					g_pLog->WriteLog( L"NotFindClan in RING_DB_USER_LOGIN(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, ui32CID );
				}

				if( EV_SUCCESSED( pOutDB->_i32Result ) )
				{
					switch( ui8Level )
					{
					case CLAN_MEMBER_LEVEL_MASTER:
					case CLAN_MEMBER_LEVEL_STAFF:
					case CLAN_MEMBER_LEVEL_REGULAR:
						{
							CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pOutDB->_i64UID );
							if( pstMember )
							{
								SetClanChangeStateDate( pstClan );

								CLAN_USER_INFO UserInfo;	
								UserInfo.i64UID = pOutDB->_i64UID;
								UserInfo.ui32ClanDBIdx = ui32CID;
								UserInfo.ui32ServerIdx = pOutDB->_ui32ServerIdx;

								pstMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pOutDB->_ui32ServerIdx, 0, 0 );
								pstMember->_session  	= pOutDB->_i32SessionIdx;
								pstMember->_pagein		= 0;

								CLAN_USER_INFO stUserInfo;
								stUserInfo.ui32ServerIdx	= pOutDB->_ui32ServerIdx;		
								stUserInfo.i32SessionIdx	= pOutDB->_i32SessionIdx;
								stUserInfo.i64UID			= pOutDB->_i64UID;
								stUserInfo.ui32ClanDBIdx	= ui32CID;

								SendMemberInfo( &stUserInfo, pstClan );
								SendClanMemberStateChange( pstClan, pstMember );
							}
							else
							{
								pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

								g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, ui32CID );
							}
						}
						break;
					case CLAN_MEMBER_LEVEL_UNKNOWN:
						{
							CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pOutDB->_i64UID );
							if( pstRequest )
							{
								pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pOutDB->_ui32ServerIdx, 0, 0 );
								pstRequest->_sessionIdx	= pOutDB->_i32SessionIdx;
							}
							else
							{
								pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

								g_pLog->WriteLog( L"NotFindRequest(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, ui32CID );
							}
						}
					}
				}
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_USER_LOGIN_ACK );
			SendPacket.WriteData( &pOutDB->_i32SessionIdx,		sizeof(INT32) );
			SendPacket.WriteData( &pOutDB->_i64UID,				sizeof(T_UID) );
			SendPacket.WriteData( &pOutDB->_i32Result,			sizeof(INT32) );
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT8 ui8NameSize = (UINT8)i3String::Length( pstClan->_ClanDetailInfo.m_name )+1;
				SendPacket.WriteData( &ui32CID,					sizeof(UINT32));
				SendPacket.WriteData( &ui8Level,				sizeof(UINT8));
				if( CLAN_MEMBER_LEVEL_UNKNOWN < ui8Level )
				{
					SendPacket.WriteData( &pstClan->_ClanDetailInfo.m_expRank,		sizeof(UINT8));
					SendPacket.WriteData( &pstClan->_ClanDetailInfo.m_personRank,	sizeof(UINT8));
					SendPacket.WriteData( &pstClan->_ClanDetailInfo.m_mark,			sizeof(UINT32));
					SendPacket.WriteData( &ui8NameSize,								sizeof(UINT8));
					SendPacket.WriteData( pstClan->_ClanDetailInfo.m_name,			sizeof(TTCHAR) * ui8NameSize);
				}
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_CREATE:
		{
			UINT16				ui16ClanIdx;

			pOutDB->Reset();
			pOutDB->Read( &ui16ClanIdx,		sizeof(UINT16) );

			CLAN_DETAIL_INFO_SERVER* pstClan = NULL;

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{  
				UINT32				ui32CID;
				UINT32				ui32Date;
				CLAN_CS_CREATE_INFO stCreateInfo;

				pOutDB->Read( &ui32CID,			sizeof(UINT32) );
				pOutDB->Read( &stCreateInfo,	sizeof(CLAN_CS_CREATE_INFO) );
				pOutDB->Read( &ui32Date,		sizeof(UINT32) );

				pstClan = PopTempClan();
				if( NULL == pstClan ) break;

				pstClan->_ClanDetailInfo.m_ui32CID		= ui32CID;
				pstClan->_ClanDetailInfo.m_date			= ui32Date;
				pstClan->_statedate						= ui32Date;
				pstClan->_ClanDetailInfo.m_mark			= stCreateInfo.wMark;
				pstClan->_ClanDetailInfo.m_i64masterUID	= pOutDB->_i64UID;
				pstClan->_ClanDetailInfo.m_masterRank	= (UINT8)stCreateInfo.ui32Rank;
				pstClan->_ClanDetailInfo.m_maxPerson		= stCreateInfo.m_ui8MemberCount;
				pstClan->_ClanDetailInfo.m_masterNickColor = stCreateInfo.m_ui8NickColor;

				i3String::Copy( pstClan->_ClanDetailInfo.m_name,		stCreateInfo.szClanName, NET_CLAN_NAME_SIZE );
				i3String::Copy( pstClan->_ClanDetailInfo.m_intro,		stCreateInfo.szClanIntro, NET_CLAN_INTRO_SIZE );
				i3String::Copy( pstClan->_ClanDetailInfo.m_masterNick,	stCreateInfo.strNick, NET_NICK_NAME_SIZE );

				if( g_pConfig->m_bAzitActive )
					i3String::Copy( pstClan->_ClanDetailInfo.m_azitURL,		stCreateInfo.szAzitURL, NET_CLAN_AZIT_URL_SIZE );

				m_pListClan->Add( pstClan );

				m_ClanListContext._totalCount++;
				SetClanListContect();

				CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->GetItem( 0 );
				if( NULL == pstMember )
				{
					g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, ui32CID );
					break;
				}

				pstMember->_i64UID		= pOutDB->_i64UID;
				pstMember->_rank		= (UINT8)stCreateInfo.ui32Rank;
				pstMember->_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_ONLINE, pOutDB->_ui32ServerIdx, stCreateInfo.wChannelNum, stCreateInfo.nRoomIdx );
				pstMember->_session		= pOutDB->_i32SessionIdx;
				pstMember->_level		= CLAN_MEMBER_LEVEL_MASTER;
				i3String::Copy( pstMember->_nick, stCreateInfo.strNick, NET_NICK_NAME_SIZE );
				pstMember->_color		= stCreateInfo.m_ui8NickColor;

				pstClan->_stMemberContext._totalCount++;
				SetMemberListContect( pstClan );
				/*
#ifdef I3_DEBUG
				I3TRACE("------------------------------------\n");
				I3TRACE( "Count:%d\n", m_pListClan->GetCount() ) ;
				for( INT32 i = 0 ; i < m_pListClan->GetCount() ; i++ )
				{
					CLAN_DETAIL_INFO_SERVER* pstClan2 = (CLAN_DETAIL_INFO_SERVER*)m_pListClan->GetItem( i );
					I3TRACE("Clan:%d (%d, %s)\n", i, pstClan2->_ClanDetailInfo.m_ui32CID, pstClan2->_ClanDetailInfo.m_name );
				}

				I3TRACE("------------------------------------\n");
				I3TRACE( "Count2:%d\n", m_ClanListContext._totalCount );
				for( UINT32 i = 0 ; i < m_ClanListContext._totalCount ; i++ )
				{
					I3TRACE("Clan2:%d (%d, %s)\n", i, m_pClanDetailInfo[i]._ClanDetailInfo.m_ui32CID, m_pClanDetailInfo[i]._ClanDetailInfo.m_name );
				}

				I3TRACE("------------------------------------\n");
				I3TRACE( "Count3:%d\n", m_pListClanTemp->GetCount() );
				for( INT32 i = m_pListClanTemp->GetCount()-1 ; i >= 0 && i >= m_pListClanTemp->GetCount()-5 ; i-- )
				{
					CLAN_DETAIL_INFO_SERVER* pstClan2 = (CLAN_DETAIL_INFO_SERVER*)m_pListClanTemp->GetItem( i );
					I3TRACE("Clan3:%d (%d, %s)\n", i, pstClan2->_ClanDetailInfo.m_ui32CID, pstClan2->_ClanDetailInfo.m_name );
				}
#endif
				*/
			}
			else
			{
				m_i32ClanTempCount--;
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_CREATE_CLAN_ACK );
			SendPacket.WriteData( &pOutDB->_i32SessionIdx,		sizeof(INT32) );
			SendPacket.WriteData( &pOutDB->_i64UID,				sizeof(T_UID) );
			SendPacket.WriteData( &pOutDB->_i32Result,			sizeof(INT32) );
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				SendPacket.WriteData( pstClan,					sizeof(CLAN_DETAIL_INFO));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_DEATROY:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			UINT8 ui8ClanRank;
			UINT32 ui32ClanExp;
			REAL64 r64ClanPoint = 0.f;
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_DEATROY(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );

				break;
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				ui8ClanRank =  pstClan->_ClanDetailInfo.m_expRank;
				ui32ClanExp =  pstClan->_ClanDetailInfo.m_exp;

				r64ClanPoint = pstClan->_ClanDetailInfo.GetClanPoint();

				// 클랜 신청서 낸 놈들에게 삭제되었다고 보내기
				for( INT32 i = 0 ; i < pstClan->_pRequest->GetCount() ; i++ )
				{
					CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->GetItem( i );
					if( NULL == pstRequest )	continue;				// 나올수 없는 상황입니다.

					if( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstRequest->_state ) && pstRequest->_sessionIdx != -1 ) 
					{
						i3NetworkPacket SendResultPacket( PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK );	
						SendResultPacket.WriteData(&pstRequest->_sessionIdx,	sizeof(INT32));
						SendResultPacket.WriteData(&pstRequest->_i64UID,		sizeof(T_UID));
						g_pGateway->SendPacket( GET_FRIEND_SERVER( pstRequest->_state ) , &SendResultPacket );
					}
					DeleteRequest( pstClan, pstRequest->_i64UID );
				}

				// Log 남기기
				g_pLog->WriteLog( L"Clan Close - no:%d, Name:%s, mark:%d, exp:%d, maxper:%d, match:%d, win:%d, lose:%d, buildate:%d, url:%s, muid:%I64d", 
					pstClan->_ClanDetailInfo.m_ui32CID, pstClan->_ClanDetailInfo.m_name, pstClan->_ClanDetailInfo.m_mark, pstClan->_ClanDetailInfo.m_exp,
					pstClan->_ClanDetailInfo.m_maxPerson, pstClan->_ClanDetailInfo.m_match, pstClan->_ClanDetailInfo.m_win, pstClan->_ClanDetailInfo.m_lose,
					pstClan->_ClanDetailInfo.m_date, pstClan->_ClanDetailInfo.m_azitURL, pstClan->_ClanDetailInfo.m_i64masterUID );

				// 클랜 삭제
				PushTempClan( pstClan );
				m_ClanListContext._totalCount--;
				SetClanListContect();
				m_i32ClanTempCount--;
				/*
#ifdef I3_DEBUG
				I3TRACE("------------------------------------\n");
				I3TRACE( "Count:%d\n", m_pListClan->GetCount() );
				for( INT32 i = 0 ; i < m_pListClan->GetCount() ; i++ )
				{
					CLAN_DETAIL_INFO_SERVER* pstClan2 = (CLAN_DETAIL_INFO_SERVER*)m_pListClan->GetItem( i );
					I3TRACE("Clan:%d (%d, %s)\n", i, pstClan2->_ClanDetailInfo.m_ui32CID, pstClan2->_ClanDetailInfo.m_name );
				}

				I3TRACE("------------------------------------\n");
				I3TRACE( "Count2:%d\n", m_ClanListContext._totalCount );
				for( UINT32 i = 0 ; i < m_ClanListContext._totalCount ; i++ )
				{
					I3TRACE("Clan2:%d (%d, %s)\n", i, m_pClanDetailInfo[i]._ClanDetailInfo.m_ui32CID, m_pClanDetailInfo[i]._ClanDetailInfo.m_name );
				}

				I3TRACE("------------------------------------\n");
				I3TRACE( "Count3:%d\n", m_pListClanTemp->GetCount() );
				for( INT32 i = m_pListClanTemp->GetCount()-1 ; i >= 0 && i >= m_pListClanTemp->GetCount()-5 ; i-- )
				{
					CLAN_DETAIL_INFO_SERVER* pstClan2 = (CLAN_DETAIL_INFO_SERVER*)m_pListClanTemp->GetItem( i );
					I3TRACE("Clan3:%d (%d, %s)\n", i, pstClan2->_ClanDetailInfo.m_ui32CID, pstClan2->_ClanDetailInfo.m_name );
				}
#endif
				*/
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_CLOSE_CLAN_ACK );
			SendPacket.WriteData( &pOutDB->_i32SessionIdx,		sizeof(INT32) );
			SendPacket.WriteData( &pOutDB->_i64UID,				sizeof(T_UID) );
			SendPacket.WriteData( &pOutDB->_i32Result,			sizeof(INT32) );

			// ZLog 에 남기기 위하여
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				REAL32 r32ClanPoint = (REAL32)r64ClanPoint;

				SendPacket.WriteData( &ui8ClanRank,			sizeof(UINT8) );
				SendPacket.WriteData( &ui32ClanExp,			sizeof(UINT32) );
				SendPacket.WriteData( &r32ClanPoint,		sizeof(REAL32) );
			}

			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;

	case RING_DB_REQUEST_JOIN:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REQUEST_JOIN(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}
			else
			{
				if( EV_SUCCESSED( pOutDB->_i32Result ) )
				{
					UINT32 ui32Date;
					CLAN_CS_JOIN_INFO stClanJoinInfo;

					pOutDB->Reset();
					pOutDB->Read( &ui32Date,		sizeof(UINT32) );
					pOutDB->Read( &stClanJoinInfo,	sizeof(CLAN_CS_JOIN_INFO) );
					stClanJoinInfo.strNick[ NET_NICK_NAME_SIZE-1 ]		= '\0';
				
					CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pOutDB->_i64UID );
					if( NULL == pstRequest )
					{
						g_pLog->WriteLog( L"NotFindRequest(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
						break;				// 나올수 없는 상황입니다.
					}
					pstRequest->_ui8NickColor = stClanJoinInfo.ui8NickColor;
					pstRequest->_date	= ui32Date;

					pstRequest->_ui32Kill			= stClanJoinInfo._ui32Killcount;
					pstRequest->_ui32Death			= stClanJoinInfo._ui32Death;
					pstRequest->_ui32Assist			= stClanJoinInfo._ui32AssistCount;
					pstRequest->_ui32Match			= stClanJoinInfo._ui32Match;
					pstRequest->_ui32Win			= stClanJoinInfo._ui32Win;
					pstRequest->_ui32Lose			= stClanJoinInfo._ui32Lose;
					i3String::Copy( pstRequest->_strContents, stClanJoinInfo.szContents, NET_CLAN_REQUEST_SIZE );
					pstRequest->_strContents[NET_CLAN_REQUEST_SIZE -1] = '\0';

					pstClan->_stRequestContext._totalCount++;

					SetRequestListContect( pstClan );

					// LodD
					//LOG_SEND_USERDATA LogUserData;
					//LogUserData._wServerID	= pOutDB->_ui32ServerIdx;
					//LogUserData._i64UID		= pOutDB->_i64UID;
					//i3String::Copy( LogUserData._szNick, pstRequest->_nick );
					//게임 서버로 뺌
					//g_pModuleLog->SendLogMessage( LOG_MESSAGE_CLAN_APPLY, &LogUserData, pstClan, stClanJoinInfo.szContents );
				}
				else
				{
					CLAN_REQUEST_SERVER_INFO* pstTempRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->DeleteEx( pOutDB->_i64UID );
					PushTempRequest( pstTempRequest );
				}
			}

			// Ack
			i3NetworkPacket SendPacket( PROTOCOL_CS_JOIN_REQUEST_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				SendPacket.WriteData(&pOutDB->_ui32ClanDBIdx,	sizeof(UINT32));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket);
		}
		break;
	case RING_DB_REQUEST_ACCEPT:
		{
			INT32 i32Count = 0;
			T_UID ai64UID[ MAX_REQUEST_COUNT ];

			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REQUEST_ACCEPT(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			UINT8	ui8Count;

			pOutDB->Reset();
			pOutDB->Read( &ui8Count,		sizeof(UINT8) );
			pOutDB->Read( ai64UID,			sizeof(T_UID)*ui8Count );

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT32	ui32Date;

				i32Count = (INT32)ui8Count;
				pOutDB->Read( &ui32Date,		sizeof(UINT32) );

				for( INT32 i = 0 ; i < ui8Count ; i++ )
				{
					CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( ai64UID[i] );
					if( NULL == pstMember )
					{
						g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, ai64UID[i], pOutDB->_ui32ClanDBIdx );
						break;				// 나올수 없는 상황입니다.
					}

					CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( ai64UID[i] );
					if( NULL == pstRequest )
					{
						g_pLog->WriteLog( L"NotFindRequest(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, ai64UID[i], pOutDB->_ui32ClanDBIdx );
						break;				// 나올수 없는 상황입니다.
					}

					pstMember->_session		= pstRequest->_sessionIdx;
					pstMember->_state		= pstRequest->_state;
					pstMember->_rank		= pstRequest->_rank;
					pstMember->_date		= ui32Date;
					i3String::Copy( pstMember->_nick, pstRequest->_nick, NET_NICK_NAME_SIZE );
					pstMember->_color		= pstRequest->_ui8NickColor;

					pstClan->_ClanDetailInfo.m_currentPerson++;
					pstClan->_stMemberContext._totalCount++;

					// 가입신청서 삭제
					DeleteRequest( pstClan, ai64UID[i] );

					// 커뮤니티 전송
					SendMemberInfo_Insert( pstClan, pstMember );

					if( ( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstMember->_state ) ) && ( -1 != pstMember->_session ) )
					{
						CLAN_USER_INFO stUserInfo;
						stUserInfo.ui32ServerIdx	= GET_FRIEND_SERVER( (UINT32)pstMember->_state );		
						stUserInfo.i32SessionIdx	= pstMember->_session;
						stUserInfo.i64UID			= pstMember->_i64UID;
						stUserInfo.ui32ClanDBIdx	= pOutDB->_ui32ClanDBIdx;

						i3NetworkPacket SendPacket( PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK );
						SendPacket.WriteData(&stUserInfo.i32SessionIdx,		sizeof(INT32));
						SendPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID));
						SendPacket.WriteData(pstClan,						sizeof(CLAN_DETAIL_INFO));
						g_pGateway->SendPacket( stUserInfo.ui32ServerIdx , &SendPacket );

						// 클랜 커뮤니티 전송
						SendMemberInfo( &stUserInfo, pstClan );
					}
				}

				// 클랜 변화
				//SetClanChangeStateDate( i32ClanIdx );

				// Member Context Change
				SetMemberListContect( pstClan );

				ClanChangePerson( pstClan );
			}
			else
			{
				for( INT32 i = 0 ; i < ui8Count ; i++ )
				{
					if( 0 == ai64UID[i] ) continue;

					CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( ai64UID[i] );
					if( NULL != pstMember )
					{	// 이미 클랜원이 된 경우입니다.
						DeleteRequest( pstClan, ai64UID[i] );	// 가입 신청서 삭제
						continue;	
					}

					CLAN_MEMBER_DETAIL_INFO* pstTempMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->DeleteEx( ai64UID[i] );
					PushTempMember( pstTempMember );
				}

				i32Count = EVENT_ERROR_CLAN_MEMBER;
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_ACCEPT_REQUEST_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&i32Count,						sizeof(INT32));
			if( 0 < i32Count )
			{
				SendPacket.WriteData( ai64UID,					(UINT16)(sizeof(T_UID)*i32Count));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_REQUEST_CANCEL:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan;
			pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REQUEST_CANCEL(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				DeleteRequest( pstClan, pOutDB->_i64UID );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_CANCEL_REQUEST_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,			sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket);
		}
		break;
	case RING_DB_REQUEST_DENIAL:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );;
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REQUEST_DENIAL(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			INT32 i32Count = 0;
			T_UID pi64UID[ MAX_REQUEST_COUNT ]; 
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT8 ui8Count;

				pOutDB->Reset();
				pOutDB->Read( &ui8Count,	sizeof(UINT8) );			LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );
				pOutDB->Read( pi64UID,		sizeof(T_UID)*ui8Count );

				i32Count = (INT32)ui8Count;

				for( INT32 i = 0 ; i < ui8Count ; i++ )
				{
					CLAN_REQUEST_SERVER_INFO* pstRequest = (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( pi64UID[i] );
					if( NULL == pstRequest )
					{
						g_pLog->WriteLog( L"NotFindRequest(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pi64UID[i], pOutDB->_ui32ClanDBIdx );
						continue;
					}

					if( ( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstRequest->_state ) ) && ( -1 != pstRequest->_sessionIdx ) ) 
					{
						i3NetworkPacket SendResultPacket( PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK );	
						SendResultPacket.WriteData(&pstRequest->_sessionIdx,	sizeof(INT32));
						SendResultPacket.WriteData(&pstRequest->_i64UID,		sizeof(T_UID));
						g_pGateway->SendPacket( GET_FRIEND_SERVER( pstRequest->_state ) , &SendResultPacket );
					}

					DeleteRequest( pstClan, pi64UID[i] );
				}
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_DENIAL_REQUEST_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&i32Count,						sizeof(INT32));
			if( 0 < i32Count )
			{
				SendPacket.WriteData( pi64UID,					(UINT16)(sizeof(T_UID)*i32Count) );
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket);
		}
		break;

	case RING_DB_MEMBER_SECESSION:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_MEMBER_SECESSION(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pOutDB->_i64UID );
				if( NULL == pstMember )
				{
					g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
					break;					// 나올수 없는 상황입니다.
				}

				pstClan->_ClanDetailInfo.m_currentPerson--;
				pstClan->_stMemberContext._totalCount--;
				
				CLAN_MEMBER_DETAIL_INFO* pstTempMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->DeleteEx( pOutDB->_i64UID );
				PushTempMember( pstTempMember );

				m_ClanListContext._version	= i3ThreadTimer::StandTime().GetDate();
				SetMemberListContect( pstClan );

				// Log 남기기
				g_pLog->WriteLog( L"Clan Secession - ClanName:%s, Idx:%I64d", pstClan->_ClanDetailInfo.m_name, pOutDB->_i64UID );
				// Log 남기기
			
				// 커뮤니티에서 삭제
				SendMemberInfo_Delete( pstClan, pstMember );

				// 인원수 변경 유무 체크 
				ClanChangePerson( pstClan );
			}
			i3NetworkPacket SendPacket( PROTOCOL_CS_SECESSION_CLAN_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				SendPacket.WriteData(&pstClan->_ClanDetailInfo.m_i64masterUID,	sizeof(T_UID));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_MEMBER_DEPORTATION:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_MEMBER_DEPORTATION(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			INT32 i32Count = 0;
			T_UID pi64UID[ MAX_MEMBER_COUNT ];

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT8 ui8Count;
				pOutDB->Reset();
				pOutDB->Read( &ui8Count,	sizeof(UINT8) );			LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );
				pOutDB->Read( pi64UID,		sizeof(T_UID)*ui8Count );
				i32Count = (INT32)ui8Count;

				for( INT32 i = 0 ; i < ui8Count ; i++ )
				{
					CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pi64UID[i] );
					if( NULL == pstMember )
					{
						g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pi64UID[i], pOutDB->_ui32ClanDBIdx );
						break;					// 나올수 없는 상황입니다.
					}

					// 보내줄 정보
					if( GET_FRIEND_STATE( pstMember->_state ) != FRIEND_STATE_OFFLINE && ( -1 != pstMember->_session ) )
					{
						i3NetworkPacket SendResultPacket( PROTOCOL_CS_DEPORTATION_RESULT_ACK );
						SendResultPacket.WriteData( &pstMember->_session,	sizeof(INT32) );
						SendResultPacket.WriteData( &pstMember->_i64UID,	sizeof(T_UID) );
						g_pGateway->SendPacket( GET_FRIEND_SERVER( (UINT32)pstMember->_state ) , &SendResultPacket );
					}

					// LodD
					// 게임서버로 뺌
					//LOG_SEND_USERDATA LogUserData;
					//i3String::Copy( LogUserData._szNick, pstMember->_nick );
					//LogUserData._szNick[ NET_NICK_NAME_SIZE-1 ] = '\0';
					//LogUserData._wServerID	= pOutDB->_ui32ServerIdx;
					//LogUserData._i64UID		= pOutDB->_i64UID;	
					//g_pModuleLog->SendLogMessage( LOG_MESSAGE_CLAN_SECEDE, &LogUserData, pstClan );

					pstClan->_ClanDetailInfo.m_currentPerson--;
					pstClan->_stMemberContext._totalCount--;

					// 커뮤니티에서 삭제
					SendMemberInfo_Delete( pstClan, pstMember );

					CLAN_MEMBER_DETAIL_INFO* pstTempMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->DeleteEx( pi64UID[i] );
					PushTempMember( pstTempMember );

					// Log 남기기
					g_pLog->WriteLog( L"Clan Deportation - ClanName:%s, User Idx:%I64d, Deportation Idx:%I64d", 
						pstClan->_ClanDetailInfo.m_name, pOutDB->_i64UID, pi64UID[i] );
				}
				
				// 인원수 변경 유무 체크
				ClanChangePerson( pstClan );

				m_ClanListContext._version	= i3ThreadTimer::StandTime().GetDate();
				SetMemberListContect( pstClan );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_DEPORTATION_ACK );
			SendPacket.WriteData( &pOutDB->_i32SessionIdx,		sizeof(INT32) );
			SendPacket.WriteData( &pOutDB->_i64UID,				sizeof(T_UID) );
			SendPacket.WriteData( &i32Count,					sizeof(INT32) );
			if( 0 < i32Count )
			{
				SendPacket.WriteData( pi64UID,					(UINT16)(sizeof(T_UID)*i32Count) );
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;

	case RING_DB_INVITE:
		{
			T_UID i64UID;
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				pOutDB->Reset();
				pOutDB->Read( &i64UID, sizeof(T_UID) );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_INVITE_ACK );
			SendPacket.WriteData( &pOutDB->_i32SessionIdx,		sizeof(INT32) );
			SendPacket.WriteData( &pOutDB->_i64UID,				sizeof(T_UID) );
			SendPacket.WriteData( &pOutDB->_i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				//SendPacket.WriteData( &pOutDB->_ui32ClanDBIdx,	sizeof(UINT32));	
				SendPacket.WriteData( &i64UID,					sizeof(T_UID));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_INVITE_ACCEPT:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_INVITE_ACCEPT(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );

				i3NetworkPacket SendPacket( PROTOCOL_CS_INVITE_ACCEPT_ACK );
				SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
				SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
				SendPacket.WriteData(&pOutDB->_i32Result,			sizeof(INT32));
				g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
				break;
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT32	ui32Date;
				pOutDB->Reset();
				pOutDB->Read( &ui32Date,		sizeof(UINT32) );

				CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pOutDB->_i64UID );
				if( NULL == pstMember )
				{
					g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
					break;					// 나올수 없는 상황입니다.
				}

				pstMember->_date		= ui32Date;

				// LodD
				//LOG_SEND_USERDATA LogUserData;
				//LogUserData._wServerID	= pOutDB->_ui32ServerIdx;
				//LogUserData._i64UID		= pOutDB->_i64UID;
				//i3String::Copy( LogUserData._szNick, pstMember->_nick );
				//g_pModuleLog->SendLogMessage( LOG_MESSAGE_CLAN_JOIN, &LogUserData, pstClan );


				CLAN_USER_INFO stUserInfo;
				stUserInfo.ui32ServerIdx	= (UINT32)GET_FRIEND_SERVER( pstMember->_state );
				stUserInfo.i32SessionIdx	= pstMember->_session;
				stUserInfo.i64UID			= pstMember->_i64UID;
				stUserInfo.ui32ClanDBIdx	= pOutDB->_ui32ClanDBIdx;

				// 커뮤니티 전송
				SendMemberInfo_Insert( pstClan, pstMember );

				m_ClanListContext._version	= i3ThreadTimer::StandTime().GetDate();
				// 클랜 변화
				SetClanChangeStateDate( pstClan );

				pstClan->_ClanDetailInfo.m_currentPerson++;
				pstClan->_stMemberContext._totalCount++;

				// 클랜 커뮤니티 전송
				SendMemberInfo( &stUserInfo, pstClan );

				// Member Context Change
				SetMemberListContect( pstClan );

				ClanChangePerson( pstClan );
			}
			else
			{
				CLAN_MEMBER_DETAIL_INFO* pstTempMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->DeleteEx( pOutDB->_i64UID );
				PushTempMember( pstTempMember );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_INVITE_ACCEPT_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,			sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_ui32ClanDBIdx,		sizeof(UINT32));
			SendPacket.WriteData(&pstClan->_ClanDetailInfo.m_i64masterUID,		sizeof(T_UID));
			SendPacket.WriteData(pstClan,						sizeof(CLAN_DETAIL_INFO));

			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;

	case RING_DB_COMMISSION_MASTER:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_COMMISSION_MASTER(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			T_UID i64UID;

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				pOutDB->Reset();
				pOutDB->Read( &i64UID,		sizeof(T_UID) );

				CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( i64UID );
				if( NULL == pstMember )
				{
					g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, i64UID, pOutDB->_ui32ClanDBIdx );
					break;					// 나올수 없는 상황입니다.
				}

				CLAN_MEMBER_DETAIL_INFO* pstOrgMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pOutDB->_i64UID );
				if( NULL == pstOrgMember )
				{
					g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
					break;					// 나올수 없는 상황입니다.
				}

				// Log 남기기
				g_pLog->WriteLog( L"Clan MasterCommission - ClanName:%s, Master Idx:%I64d, Commission Master Idx:%I64d", 
					pstClan->_ClanDetailInfo.m_name, pOutDB->_i64UID, i64UID );

				// 바뀐 방장에게 통보
				if( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstMember->_state ) && ( -1 != pstMember->_session ) ) 
				{
					i3NetworkPacket SendResultPacket( PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK );
					SendResultPacket.WriteData(&pstMember->_session,	sizeof(INT32));
					SendResultPacket.WriteData(&pstMember->_i64UID,		sizeof(T_UID));
					g_pGateway->SendPacket( GET_FRIEND_SERVER( (UINT32)pstMember->_state) , &SendResultPacket );	
				}

				// 클랜장 바꿔준다.
				pstClan->_ClanDetailInfo.m_i64masterUID	= pstMember->_i64UID;
				pstClan->_ClanDetailInfo.m_masterRank	= pstMember->_rank;
				pstClan->_ClanDetailInfo.m_masterNickColor = pstMember->_color;
				i3String::Copy( pstClan->_ClanDetailInfo.m_masterNick, pstMember->_nick, NET_NICK_NAME_SIZE );

				pstOrgMember->_level	= CLAN_MEMBER_LEVEL_STAFF;
				pstMember->_level		= CLAN_MEMBER_LEVEL_MASTER;

				pstClan->_stMemberContext._version = i3ThreadTimer::StandTime().GetDate();
			}
			// Ack
			i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_MASTER_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				SendPacket.WriteData(&i64UID,					sizeof(T_UID));
			}

			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_COMMISSION_STAFF:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_COMMISSION_STAFF(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			INT32	i32Count = 0;
			T_UID	pi64UID[ MAX_MEMBER_COUNT ];

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT8	ui8Count;
				pOutDB->Reset();
				pOutDB->Read( &ui8Count,	sizeof(UINT8) );
				pOutDB->Read( pi64UID,		sizeof(T_UID)*ui8Count );
				i32Count = (INT32)ui8Count;

				for( INT32 i = 0 ; i < ui8Count ; i++ )
				{
					CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( pi64UID[i] );
					if( NULL == pstMember )
					{
						g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pi64UID[i], pOutDB->_ui32ClanDBIdx );
						continue;					// 나올수 없는 상황입니다.
					}

					pstMember->_level = CLAN_MEMBER_LEVEL_STAFF;

					if( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstMember->_state ) && ( -1 != pstMember->_session ) ) 
					{
						i3NetworkPacket SendResultPacket( PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK );
						SendResultPacket.WriteData(&pstMember->_session,	sizeof(INT32));
						SendResultPacket.WriteData(&pstMember->_i64UID,		sizeof(T_UID));
						g_pGateway->SendPacket( (UINT32)GET_FRIEND_SERVER( pstMember->_state) , &SendResultPacket );
					}
				}
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_STAFF_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&i32Count,						sizeof(INT32));
			if( 0 < i32Count )
			{
				SendPacket.WriteData( pi64UID,					(UINT16)(sizeof(T_UID)*i32Count));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_COMMISSION_REGULAR:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_COMMISSION_REGULAR(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			INT32	i32Count = 0;
			T_UID	ai64UID[ MAX_MEMBER_COUNT ];

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT8	ui8Count;
				pOutDB->Reset();
				pOutDB->Read( &ui8Count,	sizeof(UINT8) );
				pOutDB->Read( ai64UID,		sizeof(T_UID)*ui8Count );
				i32Count = (INT32)ui8Count;

				for( INT32 i = 0 ; i < ui8Count ; i++ )
				{
					CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( ai64UID[i] );
					if( NULL == pstMember )
					{
						g_pLog->WriteLog( L"NotFindMember(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, ai64UID[i], pOutDB->_ui32ClanDBIdx );
						continue;					// 나올수 없는 상황입니다.
					}

					pstMember->_level = CLAN_MEMBER_LEVEL_REGULAR;

					if( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE( pstMember->_state ) && ( -1 != pstMember->_session ) ) 
					{
						i3NetworkPacket SendResultPacket( PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK );
						SendResultPacket.WriteData(&pstMember->_session,	sizeof(INT32));
						SendResultPacket.WriteData(&pstMember->_i64UID,		sizeof(T_UID));
						g_pGateway->SendPacket( (UINT32)GET_FRIEND_SERVER( pstMember->_state) , &SendResultPacket );
					}
				}
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_REGULAR_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,				sizeof(T_UID));
			SendPacket.WriteData(&i32Count,						sizeof(INT32));
			if( 0 < i32Count )
			{
				SendPacket.WriteData( ai64UID,					(UINT16)(sizeof(T_UID)*i32Count));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;

	case RING_DB_GET_MEMBER_INFO:
		{
			USER_INFO_RECORD	stUserInfo;

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				pOutDB->Reset();
				 
				// USER_INFO_RECORD 구조체를 수정하지 않기 위해 s가 붙은 변수에 받습니다. 2014.11.04 노준영
				pOutDB->Read( &stUserInfo._swin, 			sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._slose, 			sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._skillcount,		sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._sdeath, 			sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._sheadshot,		sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._smatch, 			sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._sAssistCount,	sizeof(UINT32) );

				/*pOutDB->Read( &stUserInfo._win,				sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._lose,			sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._killcount,		sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._death, 			sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._headshot, 		sizeof(UINT32) );
				pOutDB->Read( &stUserInfo._match, 			sizeof(UINT32) );
*/
				stUserInfo._sdraw	= stUserInfo._smatch - (stUserInfo._swin + stUserInfo._slose);
				//stUserInfo._draw	= stUserInfo._match - (stUserInfo._win + stUserInfo._lose);
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				SendPacket.WriteData(&stUserInfo,			sizeof(USER_INFO_RECORD));
			}
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_GET_REQUEST_INFO:
		{
			CLAN_REQUEST_DETAIL_INFO stRequest;
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				pOutDB->Reset();
				pOutDB->Read( &stRequest, 			sizeof(CLAN_REQUEST_DETAIL_INFO) );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_REQUEST_INFO_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				SendPacket.WriteData(&stRequest,			sizeof(CLAN_REQUEST_DETAIL_INFO));
			}
            g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket);
		}
		break;

	case RING_DB_REPLACE_NOTICE:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REPLACE_NOTICE(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				TTCHAR strNotice[ NET_CLAN_NOTICE_SIZE ];

				pOutDB->Reset();
				pOutDB->Read( strNotice, sizeof(TTCHAR) * NET_CLAN_NOTICE_SIZE );
				strNotice[ NET_CLAN_NOTICE_SIZE-1 ] = '\0';

				i3String::Copy( pstClan->_ClanDetailInfo.m_announce, strNotice, NET_CLAN_NOTICE_SIZE );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_NOTICE_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_REPLACE_INTRO:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REPLACE_INTRO(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				TTCHAR strIntro[ NET_CLAN_INTRO_SIZE ];

				pOutDB->Reset();
				pOutDB->Read( strIntro, sizeof(TTCHAR) * NET_CLAN_INTRO_SIZE );
				strIntro[ NET_CLAN_INTRO_SIZE-1 ] = '\0';

				i3String::Copy( pstClan->_ClanDetailInfo.m_intro, strIntro, NET_CLAN_INTRO_SIZE );
			}

			m_ClanListContext._version	= i3ThreadTimer::StandTime().GetDate();

			i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_INTRO_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_REPLACE_CLANNAME:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REPLACE_CLANNAME(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				TTCHAR wstrName[ NET_CLAN_NAME_SIZE ];

				pOutDB->Reset();
				pOutDB->Read( wstrName, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );

				// Log 남기기
				g_pLog->WriteLog( L"REPLACE ClanName UID:%I64d, CID:%d, %s->%s", pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx, pstClan->_ClanDetailInfo.m_name, wstrName );
				// Log 남기기

				i3String::Copy( pstClan->_ClanDetailInfo.m_name, wstrName, NET_CLAN_NAME_SIZE );

				// 게임서버에 바뀐 정보 보내주기
				SendClanBasicInfo( pstClan );
				// 바뀐 정보 보내주기
				ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_NAME_RESULT_ACK, pstClan, pstClan->_ClanDetailInfo.m_name, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE, PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK);
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_NAME_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_REPLACE_MARK:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REPLACE_MARK(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT32 ui32Mark;

				pOutDB->Reset();
				pOutDB->Read( &ui32Mark, sizeof(UINT32) );

				// Log 남기기
				g_pLog->WriteLog( L"REPLACE ClanMark UID:%I64d, CID:%d, %d->%d", pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx, pstClan->_ClanDetailInfo.m_mark, ui32Mark );
				// Log 남기기

				pstClan->_ClanDetailInfo.m_mark = ui32Mark;

				// 게임서버에 바뀐 정보 보내주기
				SendClanBasicInfo( pstClan );
				// 바뀐 정보 보내주기
				ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_MARK_RESULT_ACK, pstClan, &pstClan->_ClanDetailInfo.m_mark, sizeof( UINT32 ), PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK);
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MARK_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_REPLACE_PERSONMAX:
		{
			UINT8 ui8ClanCount = 0;
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REPLACE_PERSONMAX(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				UINT16	ui16AddPersonCount = 0;
				pOutDB->Reset();
				pOutDB->Read( &ui16AddPersonCount, sizeof(UINT16) );

				// Log 남기기
				g_pLog->WriteLog( L"REPLACE ClanPersonMax UID:%I64d, CID:%d, %d", pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx, pstClan->_ClanDetailInfo.m_maxPerson + ui16AddPersonCount );
				// Log 남기기				

				pstClan->_ClanDetailInfo.m_maxPerson += (UINT8)ui16AddPersonCount;
				ui8ClanCount		= pstClan->_ClanDetailInfo.m_maxPerson;

				// 게임서버에 바뀐 정보 보내주기
				SendClanBasicInfo( pstClan );
				// 바뀐 정보 보내주기
				//ClanDetailInfo( pUserInfo, 0 );
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_PERSONMAX_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			
			if(EV_SUCCESSED(pOutDB->_i32Result))
			{
				SendPacket.WriteData(&ui8ClanCount,				sizeof(UINT8));
			}

			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_REPLACE_MANAGEMENT:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_REPLACE_MANAGEMENT(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				CLAN_MANAGEMENT stManagement;

				pOutDB->Reset();
				pOutDB->Read( &stManagement, sizeof(CLAN_MANAGEMENT) );

				pstClan->_ClanDetailInfo.m_authstaff	= stManagement._authority;
				pstClan->_ClanDetailInfo.m_ranklock	= stManagement._limitRank;
				pstClan->_ClanDetailInfo.m_lagelock	= stManagement._limitAgeLow;
				pstClan->_ClanDetailInfo.m_hagelock	= stManagement._limitAgeHigh;

				// 스탭 권한이 바뀌었다면 스탭틀에게 바뀐 정보 보내주기
				if( pstClan->_ClanDetailInfo.m_authstaff != stManagement._authority )
				{
					ClanSendConnectMemberChangeInfo( PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK, pstClan, &stManagement._authority, sizeof( UINT8 ) );
				}
			}

			i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MANAGEMENT_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_CHECK_NAME:
		{
			i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_DUPLICATE_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_CHECK_AZIT:
		{
			i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK );
			SendPacket.WriteData(&pOutDB->_i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&pOutDB->_i32Result,		sizeof(INT32));
			g_pGateway->SendPacket( pOutDB->_ui32ServerIdx, &SendPacket );
		}
		break;
	case RING_DB_ITEM_RESETRECORD:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_ITEM_RESETRECORD(Type:%d,UID:%I64d,CID:%d)", pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				INT32 i32OldMatch = pstClan->_ClanDetailInfo.m_match;
				INT32 i32OldWin = pstClan->_ClanDetailInfo.m_win;
				INT32 i32OldLose = pstClan->_ClanDetailInfo.m_lose;

				pstClan->_ClanDetailInfo.m_match = 0;
				pstClan->_ClanDetailInfo.m_win = 0;
				pstClan->_ClanDetailInfo.m_lose = 0;

				// Log 남기기
				g_pLog->WriteLog( L"ITEM ResetRecord UID:%I64d, CID:%d, Item Use Before : match:%d, win:%d, lose:%d / After : match:%d, win:%d, lose:%d",
					pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx, i32OldMatch, i32OldWin, i32OldLose, pstClan->_ClanDetailInfo.m_match, pstClan->_ClanDetailInfo.m_win, pstClan->_ClanDetailInfo.m_lose);

				//ClanDetailInfo( pUserInfo, 0 );
				// 바뀐 정보 보내주기
				ClanSendConnectMemberChangeInfo( PROTOCOL_CS_RECORD_RESET_RESULT_ACK, pstClan, NULL, 0 );
			
				SetClanListContect();
			}
		}
		break;
	case RING_DB_ITEM_RESETPOINT:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan )
			{
				pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

				g_pLog->WriteLog( L"NotFindClan in RING_DB_ITEM_RESETPOINT(Type:%d,UID:%I64d,CID:%d)", 
					pOutDB->_ui8Type, pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx );
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				double rOldPoint = pstClan->_ClanDetailInfo.GetClanPoint();

				pstClan->_ClanDetailInfo.ResetClanPoint();

				g_pLog->WriteLog( L"ITEM ResetPoint UID:%I64d, CID:%d, Old Point:%f / New Point:%f", 
					pOutDB->_i64UID, pOutDB->_ui32ClanDBIdx, rOldPoint, pstClan->_ClanDetailInfo.GetClanPoint() );

//				pstClan->_ClanDetailInfo.m_clanpoint = CLAN_BASIC_POINT;
				

				//ClanDetailInfo( pUserInfo, 0 );
				// 바뀐 정보 보내주기
				ClanSendConnectMemberChangeInfo( PROTOCOL_CS_POINT_RESET_RESULT_ACK, pstClan, NULL, 0 );
			}
		}
		break;
	case RING_DB_MEMBER_ORDER:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( pOutDB->_ui32ClanDBIdx );
			if( NULL == pstClan ) 
			{
				g_pLog->WriteLog( L"NotFindClan in RING_DB_MEMBER_ORDER(Type:%d,CID:%d)", pOutDB->_ui8Type, pOutDB->_ui32ClanDBIdx );
				break;
			}

			if( EV_SUCCESSED( pOutDB->_i32Result ) )
			{
				T_UID i64ExpUID, i64WinUID, i64KillUID, i64HeadUID, i64ConnectUID ;

				pOutDB->Reset();
				pOutDB->Read( &i64ExpUID,		sizeof(T_UID) );
				pOutDB->Read( &i64WinUID,		sizeof(T_UID) ); 
				pOutDB->Read( &i64KillUID,		sizeof(T_UID) );
				pOutDB->Read( &i64HeadUID,		sizeof(T_UID) );
				pOutDB->Read( &i64ConnectUID,	sizeof(T_UID) );
				
				pstClan->_bClanOrder = FALSE;
				pstClan->_ClanTopMember.m_stopexp		= i64ExpUID; 
				pstClan->_ClanTopMember.m_stopwin		= i64WinUID; 
				pstClan->_ClanTopMember.m_stopkill		= i64KillUID; 
				pstClan->_ClanTopMember.m_stophead		= i64HeadUID; 
				pstClan->_ClanTopMember.m_stopconnect	= i64ConnectUID; 
				pstClan->_ClanTopMember.m_topexp		= i64ExpUID; 
				pstClan->_ClanTopMember.m_topwin		= i64WinUID; 
				pstClan->_ClanTopMember.m_topkill		= i64KillUID; 
				pstClan->_ClanTopMember.m_tophead		= i64HeadUID; 
				pstClan->_ClanTopMember.m_topconnect	= i64ConnectUID; 
			}
		}
		break;
	case RING_DB_CLAN_FIND_USER_INFO:
		{
			if( pOutDB->_i32Result == EVENT_ERROR_SUCCESS )
			{
				PACKET_GC_NEW_CLAN_USER_INFO_ACK	PacketAckGC;
				PacketAckGC.m_i32SessionIdx			= pOutDB->_i32SessionIdx;
				PacketAckGC.m_i64UID				= pOutDB->_i64UID;			
				PacketAckGC.m_i32SessionIdx			= pOutDB->_i32SessionIdx;
				PacketAckGC.m_ui32ClanDBIdx			= pOutDB->_ui32ClanDBIdx;

				pOutDB->Reset();
				pOutDB->Read(&PacketAckGC.m_i64FindUID,		sizeof(T_UID));
				pOutDB->Read(&PacketAckGC.m_clanMark,		sizeof(UINT32));
				pOutDB->Read(&PacketAckGC.m_strClanName,	sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
				PacketAckGC.m_strClanName[NET_CLAN_NAME_SIZE-1] = 0;
				SendClanFindUserInfo(pOutDB->_ui32ServerIdx, &PacketAckGC);
			}
		}
		break;

	case RING_DB_LOAD_MERCENARY:
		{
			T_MerID TMerID;
			pOutDB->Reset();
			pOutDB->Read(&TMerID, sizeof(T_MerID));

			i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK);
			stPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			stPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			
			if( EV_SUCCESSED(pOutDB->_i32Result ) )
			{
				INT32 i32MerRank;
				MERCENARY_RECORD stRecord; 
				pOutDB->Read(&i32MerRank,	sizeof(INT32));
				pOutDB->Read(&stRecord,		sizeof(MERCENARY_RECORD));

				CMercenary* pMercenary = m_pMercenaryMgr[pOutDB->_ui32ServerIdx]->SetMercenaryRecord(TMerID, &stRecord);
				if( pMercenary )
				{
					// 용병 랭크업 체크
					/*UINT8 ui8NewMerRank = pMercenary->CheckMerRank();
					if( i32MerRank < ui8NewMerRank )
					{	
						i32MerRank = ui8NewMerRank;
					}*/ 

					pMercenary->SetMerRank( (UINT8)i32MerRank );

					// 용병 등록 성공
					stPacket.WriteData(&pOutDB->_i32Result,	sizeof(T_RESULT));

					pMercenary->MakePacketMercenary(&stPacket);

					g_pGateway->SendPacket(pOutDB->_ui32ServerIdx, &stPacket);
					break;
				} 
				else
				{
					pOutDB->_i32Result = EVENT_ERROR_FAIL_REGIST_MERCENARY;
				}
			}

			// 용병 등록 실패
			m_pMercenaryMgr[pOutDB->_ui32ServerIdx]->RemoveMercenary(TMerID);
				
			stPacket.WriteData(&pOutDB->_i32Result, sizeof(T_RESULT));

			g_pGateway->SendPacket(pOutDB->_ui32ServerIdx, &stPacket);
		}
		break;

	case RING_DB_UPDATE_CLAN_MATCH_RECORD:
		{
			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2(pOutDB->_ui32ClanDBIdx);
			if (NULL == pstClan)
			{
				g_pLog->WriteLog(L"NotFindClan in RING_DB_MEMBER_ORDER(Type:%d,CID:%d)", pOutDB->_ui8Type, pOutDB->_ui32ClanDBIdx);
				break;
			}

			if (EV_SUCCESSED(pOutDB->_i32Result))
			{
				CLAN_WAR_RESULT	WarResult;

				pOutDB->Reset();
				pOutDB->Read(&WarResult, sizeof(CLAN_WAR_RESULT));

				pstClan->InsertClanWarResult(&WarResult);
			}
		}
		break;
	case RING_DB_LOAD_MERCENARY_PENALTY_TIME:
		{
			DATE32 dtMerPenaltyTime;
			pOutDB->Reset();
			pOutDB->Read( &dtMerPenaltyTime, sizeof(DATE32) );

			i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_ACK);
			stPacket.WriteData(&pOutDB->_i32SessionIdx,		sizeof(INT32));
			stPacket.WriteData(&pOutDB->_i64UID,			sizeof(T_UID));
			stPacket.WriteData(&dtMerPenaltyTime,			sizeof(DATE32));

			g_pGateway->SendPacket(pOutDB->_ui32ServerIdx, &stPacket);
		}
		break;
	}
}