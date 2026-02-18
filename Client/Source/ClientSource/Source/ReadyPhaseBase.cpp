#include "pch.h"
#include "ReadyPhaseBase.h"
#include "GlobalVariables.h"
#include "StageGui/Popup/PopupUseItem.h"
#include "ShopDef.h"
#include "CommunityContext.h"

I3_ABSTRACT_CLASS_INSTANCE(CReadyPhaseBase, CPhaseBase);

CReadyPhaseBase::CReadyPhaseBase()
{
	InitStageLimitTimeList();
}

CReadyPhaseBase::~CReadyPhaseBase()
{
}

void CReadyPhaseBase::OnGameEvent(INT32 event,INT32 arg)
{
	CPhaseBase::OnGameEvent(event, arg);

	switch(event) 
	{
	case EVENT_ENTER_INVENTORY:
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
		((CStageReady*)m_pParent)->ChangeStep(STEP_INVENTORY);
		i3GuiRoot::setGuiDisableInput(FALSE);

		g_pGameContext->BackupEquipment();
		break;
	case EVENT_LEAVE_INVENTORY:
		if (EV_SUCCESSED(arg))
		{
			// Do nothing
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_EQUIP_USER_TITLE_NOT_ALLOWED))
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_NO_EQUIP_PRE_DESIGNATION"));/*호칭을 획득하기전엔 장비할 수 없습니다.*/
			}
			else// if (EV_IS_ERROR(arg, EVENT_ERROR_SUCCESS))
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_EQUIP_FAIL_BY_NETWORK_PROBLEM"));/*네트워크 문제로 장비할 수 없습니다.*/
			}
			
			UpdateCharacter(VT_DEFAULT);
		}

		((CStageReady*)m_pParent)->EnterDelayedEventStep();
		break;
	case EVENT_ITEM_AUTH:	// 갯수제 아이템 인증
	case EVENT_USE_ITEM_CHECK_NICK:
		{
			i3GuiRoot::setGuiDisableInput(FALSE);

			//	아이템 인증 (무기는 장비)
			((CStageReady*)m_pParent)->AuthStartSuccess(arg);

			//	캐시 아이템 사용 창에 이벤트를 처리
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
		}
		break;
	case EVENT_AUTH_DELETE:
		{
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;

#if defined(ENABLE_MISSION_CARD)
	case EVENT_ENTER_INFO:
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
		((CStageReady*)m_pParent)->ChangeStep(STEP_PROFILE);
		i3GuiRoot::setGuiDisableInput(FALSE);
		break;

	case EVENT_LEAVE_INFO:
		((CStageReady*)m_pParent)->EnterDelayedEventStep();
		break;
#endif

	case EVENT_ENTER_SHOP:	
		{
			g_pFramework->GetShopContext()->OnGameEvent(event, arg);
				
			((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
			((CStageReady*)m_pParent)->ChangeStep(STEP_SHOP);

			i3GuiRoot::setGuiDisableInput(FALSE);
		}
		break;
	case EVENT_LEAVE_SHOP:
		((CStageReady*)m_pParent)->EnterDelayedEventStep();
		break;
	case EVENT_REQUEST_GOODS_LIST:
	case EVENT_BUY_GOODS:
	case EVENT_AUTH_GIFT_GOODS:
	case EVENT_CHECK_GIFT_GOODS:
	case EVENT_ITEM_REPAIR:
		{
			g_pFramework->GetShopContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;
	case EVENT_GET_GIFT:
		STRING_POPUP(GAME_STRING("STR_SHOP_INFORM_RECEIVE_GIFT"));	//선물을 받았습니다.\n 쪽지를 확인해주세요.
		break;


	case EVENT_CLAN_ENTER:
		{
			StepType stepType = STEP_NULL;

			if(arg == 1)
			{
				stepType = STEP_CLAN_MEMBER;			//	클랜 가입자 
			}
			else if(arg == 0)
			{
				stepType = STEP_CLAN_NON_MEMBER;		//	클랜 미가입자
			}

#if defined TEST_CLAN_MEMBER
			stepType = STEP_CLAN_MEMBER;			//	클랜 가입자 
			arg = 1;
#endif

			g_pFramework->GetClanContext()->OnGameEvent(event, arg);

			((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
			
			if (((CStageReady*)m_pParent)->GetCurrentStepType() != stepType)
			{
				((CStageReady*)m_pParent)->ChangeStep(stepType);
			}
			
			i3GuiRoot::setGuiDisableInput(FALSE);
		}
		break;
	case EVENT_CLAN_LEAVE:
		// 모든 클랜 메시지는 ClanContext에서 처리해야하나,
		// 이 경우 CStageReady에 접근할 수 없어, ClanContext 전달 전에 처리한다.
		((CStageReady*)m_pParent)->EnterDelayedEventStep();		
		// Fall through
	case EVENT_CLAN_CREATE:
	case EVENT_CLAN_DUPLICATE_NAME:
	case EVENT_CLAN_DUPLICATE_ADDRESS:
	case EVENT_CLAN_DUPLICATE_MARK:
	case EVENT_CLAN_MENAGEMENT:
	case EVENT_CLAN_CHANGE_MARK:
	case EVENT_CLAN_JOIN_REQUEST:	
	case EVENT_CLAN_MEMBER_CONTEXT:
	case EVENT_CLAN_MEMBER_LIST:
	case EVENT_CLAN_REQUEST_CONTEXT:
	case EVENT_CLAN_ACCEPT_REQUEST:
	case EVENT_CLAN_DENIAL_REQUEST:	
	case EVENT_CLAN_DISMISSALMEMBER:
	case EVENT_CLAN_COMMISSIONSTAFF:
	case EVENT_CLAN_COMMISSION_REGULAR:
	case EVENT_CLAN_NOTICE_MODIFY:
	case EVENT_CLAN_GUIDE_MODIFY:
	case EVENT_MATCH_TEAM_CREATE:			// 클랜전 팀 만들기
	case EVENT_MATCH_TEAM_JOIN:				// 클랜전 팀 참가
	case EVENT_MATCH_TEAM_LEAVE:			// 클랜전 팀 떠나기
	case EVENT_MATCH_TEAM_INFO_N:			// 클랜전 팀 정보
	case EVENT_MATCH_TEAM_CONTEXT:			// 클랜전 팀목록 Context
	case EVENT_MATCH_TEAM_ALL_CONTEXT:		// 클랜전 팀목록 Context
	case EVENT_MATCH_TEAM_LIST_N:			// 클랜전 팀목록 받기
	case EVENT_MATCH_TEAM_ALL_LIST_N:		// 클랜전 모든 팀 목록 받기
	case EVENT_MATCH_TEAM_DETAIL_INFO:
	case EVENT_MATCH_FIGHT_QUICK_REQUEST:	// 클랜전 빠른 신청
	case EVENT_MATCH_FIGHT_REQUEST:			// 클랜전 전투신청
	case EVENT_M_MATCH_FIGHT_RESULT:		// 클랜전 전투신청 응답
	case EVENT_MATCH_FIGHT_ACCECT:			// 클랜전 전투신청 수락
	case EVENT_M_MATCH_FIGHT_ACCECT_RESULT:
	case EVENT_MATCH_TEAM_CHANGE_PER:
	case EVENT_MATCH_TEAM_CHANGE_PER_RESULT_N:
	case EVENT_M_MATCH_FIGHT_CHANGE_INFO_RESULT:
	case EVENT_CLAN_LIST_CONTEXT:
	case EVENT_CLAN_CREATION_INFO:
	case EVENT_CLAN_PERSON_PLUS:
		{
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;		
	case EVENT_CLAN_JOINCHECK_AUTHORITY:
		{
			if( EV_SUCCESSED( arg ) )
			{
				INT32 nClanIdx = g_pFramework->GetClanContext()->GetJoinClanIdx();

				//	서버로부터 클랜 가입 조건이 인증되어서 클랜 가입 요청서를 띄웁니다.
				((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_CREATE, &nClanIdx);
			}
			else
			{
				switch( arg )
				{
				case EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL:
					CAPTION_POPUP( GAME_STRING("STR_TBL_GUI_BASE_CLAN_JOIN_ABORTED_BY_AGE"),
						GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*가입 신청 실패*//*해당 클랜에 가입할 수 있는 연령과 맞지 않아 클랜 가입 신청이 취소 되었습니다.*/
					break;
				case EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL:
					CAPTION_POPUP( GAME_STRING("STR_TBL_GUI_BASE_CLAN_JOIN_ABORTED_BY_GRADE"),
						GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*가입 신청 실패*//*해당 클랜에 가입할 수 있는 계급과 맞지 않아 클랜 가입 신청이 취소 되었습니다.*/
					break;
				default:
					{
						char szTemp[MAX_PATH];
						sprintf_s( szTemp, GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST_BY_SYSTEM_PROBLEM"), arg );/*\n가입 신청에 실패하였습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오.\n [Errcode : 0x%08x]*/

						CAPTION_POPUP( szTemp, GAME_STRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*가입 신청 실패*/
					}
					break;
				}
			}
		}
		break;
	case EVENT_CLAN_DISSOLUTION:
	case EVENT_CLAN_SECESSION:
		{			
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);

			//	클랜 회원 탈퇴가 성공하면 바로 클랜 미가입자 페이지로 넘어갑니다.
			if( EV_SUCCESSED(arg))
			{
				USER_INFO_BASIC		MyUserInfoBasic;
				g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
				MyUserInfoBasic._clanidx		= 0;
				MyUserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
				g_pGameContext->SetMyUserInfoBasic(MyUserInfoBasic);

				g_pCommunity->ClearClanMember();
				((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
				((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);
			}
		}
		break;
	case EVENT_CLAN_COMMISSIONMASTER:
		{
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);

			//	마스터위임이 성공하면 바로 클랜 미가입자 페이지로 넘어갑니다.
			if( EV_SUCCESSED(arg))
			{
				((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
				((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_MEMBER);
			}
		}
		break;
	case EVENT_CLAN_CANCEL_REQUEST:
		{
			g_pFramework->GetClanContext()->OnGameEvent(event, arg);
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);

			if( EV_SUCCESSED(arg))
			{
				USER_INFO_BASIC		MyUserInfoBasic;
				g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
				MyUserInfoBasic._clanidx = 0;
				g_pGameContext->SetMyUserInfoBasic(MyUserInfoBasic);
			}
		}
		break;
	case EVENT_M_CLAN_MEMBER_INFO:
		{
			// 알람
			((CStageReady*)m_pParent)->StartAlarmCommunity();
		}
		break;
	case EVENT_CLAN_MEMBER_INSERT:
		{
			// 알람
			((CStageReady*)m_pParent)->StartAlarmCommunity();
		}
		break;
	case EVENT_CLAN_INVITE_ACCEPT_N:	//	커뮤니티 쪽지로 클랜 가입을 할 경우 이미 가입되어 있는 경우
		{	
			//STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_ALREADY_JOINED_STATE"));/*이미 클랜에 가입되어 있는 상태입니다.*/

			switch( arg ) 
			{
			case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n클랜을 찾을수 없습니다."
					//	클랜 가입 승인 실패
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	
				}
				break;
			case EVENT_ERROR_CLAN_NOMEMBER:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n클랜원이 아닙니다."
					//	클랜 가입 승인 실패
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_MEMBER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n클랜 마스터가 아닙니다."
					//	클랜 가입 승인 실패
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_MASTER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MEMBER:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n이미 클랜에 가입되어있습니다."
					//	클랜 가입 승인 실패
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_MEMBER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MAXMEMBER:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n더이상 가입 할 수 없습니다."
					//	클랜 가입 승인 실패
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_REQUESTIDX:
				{
					//	"\n가입 신청 실패하였습니다.\n이미 다른 클랜에 가입되어 있거나 가입 요청한 상태입니다."
					//	클랜 가입 승인 실패
					STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_SUBSCRIPTION"), GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			default:
				{
					char szTemp[MAX_PATH];
					//	""\n클랜 가입 처리가 실패했습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오."
					sprintf_s( szTemp, "%s\n [Errcode : 0x%08x]", GAME_STRING("STBL_IDX_CLAN_MESSAGE_ASK_FOR_INFO"), arg );

					STRING_POPUP( szTemp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	//	클랜 가입 승인 실패
				}
				break;
			}

		}
		break;

	case EVENT_DELETE_NOTE_ASK:
		{
			PhaseType ePhaseType = ((CStageReady*)m_pParent)->GetCurrentPhaseType();
			if (PHASE_LOBBY == ePhaseType)
			{
				STRING_POPUP(GAME_STRING("STR_TBL_NETWORK_RECEIVE_CLAN_NOTE"));/*클랜 전체 쪽지가 도착했습니다. 쪽지함이 가득차 있으면 오래된 쪽지를 삭제합니다.*/ 				
			}
		}
		break;
	case EVENT_RECEIVE_NOTE:
		{
			// 새쪽지 알람
			((CStageReady*)m_pParent)->StartAlarmNote();	
		}
		break;
	case EVENT_CLAN_NOTE:
		{
			((CStageReady*)m_pParent)->TransferrToPopupGameEvent( event, arg);
		}
		break;
	case EVENT_INSERT_FRIEND:
		if (EV_SUCCESSED(arg))
		{
			INT32 realIdx = g_pCommunity->GetViewIndexOfRealFriend(arg);
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(realIdx);			

			if (node)
			{
				if (FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
				{
					// 상대가 신청한 경우
					((CStageReady*)m_pParent)->StartAlarmCommunity();
				}
				else
				{
					// 자신이 신청한 경우
					char message[MAX_STRING_COUNT];
					sprintf_s(message, GAME_STRING("STR_TBL_GUI_BASE_NOTICE_ADD_FRIEND_COMPLETE"), node->_nick);/*[%s]님이 친구목록에 추가되었습니다.*/
					STRING_POPUP(message);
				}
			}
			else
			{
				I3TRACE("[ERROR] EVENT_INSERT_FRIEND - Invalid friend idx(%d)\n", g_pCommunity->GetRealIndexOfViewFriend(arg));
			}
		}
		else
		{
			char message[MAX_STRING_COUNT] = {0, };

			switch(TESTBIT(arg, EVENT_ERROR_MASK_FAIL))
			{
			case EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT );//유저를 찾을 수 없습니다.
				break;
			case EVENT_ERROR_EVENT_FRIEND_INSERT_FULL:
				if (FRIEND_MAX_COUNT <= g_pCommunity->GetFriendCount())
				{
					// 자신이 친구를 추가할 수 없다.
					i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_MORE_GET_FRIEND_STATE"), MAX_STRING_COUNT);//더이상 친구를 추가할 수 없습니다.
				}
				else
				{
					// 상대가 친구를 추가할 수 없다.
					i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND_BY_LIMIT"), MAX_STRING_COUNT);//상대방이 더 이상 친구를 추가할 수 없는 상태입니다.
				}
				break;
			case EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT);//유저를 찾을 수 없습니다.
				break;
			case EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_ALREADY_REGIST_FRIEND_LIST"), MAX_STRING_COUNT);//이미 친구목록에 등록되어 있습니다.
				break;				
			case EVENT_ERROR_EVENT_FRIEND_INSERT_NOT_FIND_NICK:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT);//유저를 찾을 수 없습니다.
				break;
			default:
				i3String::Copy(message, GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"), MAX_STRING_COUNT);//유저를 찾을 수 없습니다.
				break;
			}

			STRING_POPUP(message);
		}
		break;
	case EVENT_ACCEPT_FRIEND:
		if (EV_SUCCESSED(arg))
		{
			// 친구 수락이 성공하면 친구 정보창을 닫아 친구의 상태를 새로 볼 수 있도록 한다.
			g_pCommunity->SetCommunityUserInfo(FALSE);
		}
		else
		{
			// 상황에 따른 알맞은 메시지 필요함. 현재 메시지는 너무 포괄적임.
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND"));/*친구요청수락에 실패하였습니다.*/
		}
		break;
	case EVENT_DELETE_FRIEND:
		if (EV_SUCCESSED(arg))
		{
			const FRIEND_NODE* node = g_pCommunity->GetLastDeleteFriend();

			char message[MAX_STRING_COUNT];
			sprintf_s(message, GAME_STRING("STR_TBL_GUI_BASE_NOTICE_DELETE_FRIEND_COMPLETE"), node->_nick);/*[%s]님이 친구 목록에서 삭제되었습니다.*/
			STRING_POPUP(message);
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_DELETE_FRIEND"));/*친구삭제에 실패하였습니다.*/
		}
		break;
	case EVENT_CLAN_INVITE:
		if (EV_SUCCESSED(arg))
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_COMPLETE_NORMAL"));/*정상적으로 처리되었습니다.*/
		}
		else if( EV_IS_ERROR( arg, EVENT_ERROR_CLAN_MEMBER ) ) 
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_ALREADY_JOINED"));/*이미 클랜에 가입한 유저입니다.*/
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_SEND_NOTE"));/*쪽지 보내기에 실패하였습니다.*/
		}
		break;
	case EVENT_QUEST_BUY_CARD_SET:	((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);	break;
	case EVENT_QUEST_DELETE_CARD_SET:	((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);	break;
	case EVENT_QUEST_CHANGE_ACTIVE_IDX:		break;

	case EVENT_GET_USERINFO :
	//// 1.5 Check Designation
	//case EVENT_DESIGNATION_GET :
	//case EVENT_DESIGNATION_CHANGE:
	//case EVENT_DESIGNATION_RELEASE:
		{	//	호칭 해제
			((CStageReady*)m_pParent)->TransferrToStepGameEvent( event, arg);
		}
		break;

	default:
		/* Do nothing */
		break;
	}
}

void CReadyPhaseBase::ApplyMsgReadyBox()
{
	if( g_pGameContext->EmptyReadyBoxMsg() )
		return;

	size_t size = g_pGameContext->GetReadyBoxMsg_Size();

	for(size_t i = 0; i<size; ++i)
	{
		char NameBuffer[MAX_STRING_COUNT] = {0};

		UINT32 ItemID = g_pGameContext->GetReadyBoxMsg_ItemID( i );
		g_pShopItemInfoDataBase->GetItemName(ItemID, NameBuffer, MAX_STRING_COUNT);

		//채팅창에 자랑한다.
		//알림 : {col:240, 30, 40, 255}%d{/col} 번방 {col:240, 30, 40, 255}%s{/col} 님이 {col:240, 30, 40, 255}%s{/col}를 획득하셨습니다.
		char strBuf[MAX_STRING_COUNT];

		memset(strBuf, 0, sizeof(strBuf));
		_snprintf(strBuf, MAX_STRING_COUNT-1, 
			GAME_STRING("STR_READYBOX_COMMENT"), 
			g_pGameContext->GetReadyBoxMsg_RoomIdx( i ) + 1,
			g_pGameContext->GetReadyBoxMsg_Nick( i ).c_str(),
			NameBuffer	);

		//채팅창에 먹은 사람 자랑한다.
		g_pFramework->getChatBox()->PutSystemChatting( strBuf );
	}

	g_pGameContext->ClearReadyBoxMsg();
}

void CReadyPhaseBase::GetStageMapName(BOOL useRandomMap, STAGE_ID stageID,
									  std::string & strMapName)
{
#if defined (USE_RANDOMMAP_MODE)				
	if(useRandomMap)
	{
		strMapName = GAME_STRING("STR_POPUP_RANDOMMAP");
	}
	else
	{
		char MapName[MAX_STRING_COUNT] = "";
		getStageDisplayName(MapName, stageID);
		strMapName = MapName;
	}
#else
	char MapName[MAX_STRING_COUNT] = "";
	getStageDisplayName(MapName, stageID);
	strMapName = MapName;
#endif
}

void CReadyPhaseBase::GetStageModeName(STAGE_TYPE stageType,
								   std::string & strModeName, BOOL & bAllowChangeMode)
{
	strModeName = getStageTypeName(stageType);

	switch( stageType)
	{
	case STAGE_TYPE_DEATHMATCH:		//단체전
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_HEAD_HUNTER:	//헤드헌터
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_HEAD_KILLER:	//헤드킬러
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_SUDDEN_DEATH:	//서든데쓰
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_BOMB:	//폭파미션
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_DESTROY:	//파괴미션
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_DEFENCE :	//방어미션
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_ANNIHILATION:		//섬멸전
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_ESCAPE:	//탈출미션
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_TUTORIAL:	//튜토리얼
		bAllowChangeMode = FALSE;
		break;
	}
}

void CReadyPhaseBase::GetEnableMap(STAGEID_INFO_LIST::EnableStageList & EnableMap,
								   STAGE_TYPE StageType, 
								   V_STAGEOPTION StageOptionType)
{
	EnableMap.clear();

	if( StageOptionType == STAGE_OPTION_AI )
	{
		if( g_pGameContext->getAiMode() == AI_MODE_STATE_DIEHARD )
		{
			getChallengeDieHardStage( EnableMap );
		}
	}
	else
	{
		s_StageInfoTableList.GetEnableActiveStage(EnableMap, StageType, STAGE_OPTION_AI );
	}
}

void CReadyPhaseBase::InitEnableMapList(STAGE_TYPE type, i3GuiPopupList* m_pMapList)
{
	COLOR color;
	i3Color::Set(&color, (I3COLOR*)GameGUI::GameModeColor(type));

	V_STAGEOPTION option = g_pGameContext->GetStageOption();
	//색상을 지정한다.
	switch( option )
	{
	case STAGE_OPTION_AI:	
		i3Color::Set(&color, GetColor(GCT_TRAINING_MODE));		break;
	case STAGE_OPTION_KNUCKLE:	
		i3Color::Set(&color, GetColor(GCT_KNUCKLE_MODE));		break;
	case STAGE_TYPE_ESCAPE:	
		i3Color::Set(&color, GetColor(GCT_BOSS_MODE));	break;
	case STAGE_OPTION_SNIPER:	
		i3Color::Set(&color, GetColor(GCT_SNIPER_MODE));	break;
	case STAGE_OPTION_SHOTGUN:	
		break;
	case STAGE_OPTION_NONE:
		break;
	}
	s_StageInfoTableList.GetEnableActiveStage(m_EnableStageList, type, option );

	size_t size = m_EnableStageList.size();

	m_pMapList->Clear();
	m_pMapList->SetDisplayCount( size );
	m_pMapList->SetFixedElementCount(FALSE);

	for( size_t i = 0; i < size; ++i )
	{
		m_pMapList->AddElement( m_EnableStageList[i].first.c_str() );
		m_pMapList->SetTextColor(i, &color);
	}
}


void CReadyPhaseBase::InitStageLimitTimeList()
{
	std::vector< UINT > type1, type2, type3, type4, type5;

	type1.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_3) );
	type1.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_5) );
	type1.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_7) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_BOMB, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_DESTROY, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_ANNIHILATION, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_SUDDEN_DEATH, type1) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_HEAD_KILLER, type1) );

	type2.push_back( HINIBBLE(BATTLE_TIME_MISSION_TYPE_3) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_DEFENCE, type2) );

	type3.push_back( HINIBBLE(BATTLE_TIME_TYPE_5) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_ESCAPE, type3) );

	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_3) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_5) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_10) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_15) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_20) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_25) );
	type4.push_back( HINIBBLE(BATTLE_TIME_TYPE_30) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_DEATHMATCH, type4) );

	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_5) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_10) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_15) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_20) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_25) );
	type5.push_back( HINIBBLE(BATTLE_TIME_TYPE_30) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_HEAD_HUNTER, type5) );
	m_LimitTimeList.insert( LimitTimeList::value_type(STAGE_TYPE_MAX, type5) );	//default로 대체된다.
}

UINT8 CReadyPhaseBase::_RotateTimeIndex(STAGE_TYPE stageType, UINT8 subType, BOOL bNext)
{
	LimitTimeList::iterator It = m_LimitTimeList.find( stageType );

	if( It == m_LimitTimeList.end() )
	{
		It = m_LimitTimeList.find( STAGE_TYPE_MAX );
	}

	UINT8 gameTime = HINIBBLE(subType);
	UINT8 gameType = LONIBBLE(subType);

	size_t size= It->second.size();

	if( size > 1 )
	{
		for(size_t i = 0; i < size; ++i)
		{
			if( It->second[i] == gameTime )
			{
				if( i == 0 )
				{
					if (bNext)
						gameTime = (UINT8)It->second[ i + 1 ] ;
					else
						gameTime = (UINT8)It->second[ size - 1 ] ;
				}
				else if( i == size - 1 )
				{
					if (bNext)
						gameTime = (UINT8)It->second[ 0 ] ;
					else
						gameTime = (UINT8)It->second[ i - 1 ] ;
				}
				else
				{
					if (bNext)
						gameTime = (UINT8)It->second[ i + 1 ] ;
					else
						gameTime = (UINT8)It->second[ i - 1 ] ;
				}
				break;
			}
		}
	}

	return MAKEBYTE(gameTime, gameType);
}

void CReadyPhaseBase::createRandomMap(STAGE_TYPE type)
{
		g_RandomMap.CreateRandomMapList( type, g_pGameContext->GetStageOption() );
}