#include "pch.h"
#include "UserGuideMgr.h"
#include "UI/UIMainFrame.h"
#include "UI/UIPhaseBase.h"
#include "UI/UISideMenu.h"
#include "UI/UISMenuBase.h"
#include "UI/UISMenu_Equip.h"
#include "UI/UITopMenu.h"
#include "UI/UIPhaseShop.h"
#include "UI/UIFloatCharacterView.h"
#include "UI/UIFloatUserList.h"
#include "UI/UIPhaseProfile.h"
#include "UI/UITabProfileMedal.h"

#include "UserInfoContext.h"
#include "LobbyContext.h"
#include "BattleSlotContext.h"
#include "ChannelContext.h"

struct _tagGuideEvent
{
	std::string _SceneName;
	std::string _CtrlName;

	INT32		_i32EventNum;
};

#define GUIDE_TYPE_MAX_COUNT	4

static const _tagGuideEvent s_tagGuideEvent[GS_MAX_COUNT][GUIDE_TYPE_MAX_COUNT] = {

	//// GS_JOIN_ROOM
	{
		{ "PointBlankRe_Lobby_RoomList",	"Radio_EnableRoom",			GUIDE_EVENT_VIEW_ROOM		},	
		{ "PointBlankRe_Lobby_RoomList",	"i3UIListView_Box_RL",		GUIDE_EVENT_TOP_ROOM		},	
		{ "PointBlankRe_Lobby_RoomList",	"Btn_JoinRoom",				GUIDE_EVENT_ENTER_ROOM		},	
		{ "",								"",							-1							},
	},

	// GS_CREATEROOM_DEATHMATCH
	{
		{ "PointBlankRe_Lobby_RoomList",	"Btn_CreateRoom",			GUIDE_EVENT_CREATE_ROOM		},	
		{ "PBRe_Lobby_CreatRoom",			"i3UIButtonComposed3_LCR",	GUIDE_EVENT_CREATE_ROOM_OK	},	
		{ "",								"",							-1							},
		{ "",								"",							-1							},
	},

	// GS_CHANGE_WEAPON
	{
		{ "PointBlankRe_SideMenu",			"i3UIButton",				GUIDE_EVENT_SIDE_EQUIP		},	
		{ "PointBlankRe_SideMenu_Slot2",	"i3UIFrameWnd0",			GUIDE_EVENT_1ST_P_WEAPON	},
		{ "PointBlankRe_SideMenu_Slot",		"i3UIFrameWnd0",			GUIDE_EVENT_2ND_P_WEAPON	},
		{ "PointBlankRe_SideMenu2",			"i3UIButtonC",				GUIDE_EVENT_SIDE_EQUIP_OK	},
	},

	// GS_CHANGE_CHARACTER_NAME
	{
		{ "PointBlankRe_Main",				"i3UIButton_MyInfo",		GUIDE_EVENT_TOP_MYINFO_NICK_CHANGE	},
		{ "PointBlankRe_Profile_Info",		"i3UIImageBox22",			GUIDE_EVENT_1ST_CHARA_NICK_CHANGE	},
		{ "",								"",							-1},
		{ "",								"",							-1},
	},

	// GS_CHANGE_CHARACTER
	{
		{ "PointBlankRe_CharaWindow",		"i3UIButtonNext",			GUIDE_EVENT_CHARA_VIEW_RIGHT	},
		{ "PointBlankRe_CharaWindow",		"i3UIButtonPre",			GUIDE_EVENT_CHARA_VIEW_LEFT		},
		{ "",								"",							-1								},
		{ "",								"",							-1								},
	},

	// GS_CREATEROOM_BOMB
	{
		{ "PointBlankRe_Lobby_RoomList",	"Btn_CreateRoom",			GUIDE_EVENT_CREATE_ROOM_RANK1	},
		{ "PBRe_Lobby_CreatRoom",			"i3UIListView_Box",			GUIDE_EVENT_MODE_BOMB			},
		{ "PBRe_Lobby_CreatRoom",			"i3UIButtonComposed3_LCR",	GUIDE_EVENT_MODE_BOMB_OK		},
		{ "",								"",							-1								},
	},

	// GS_ADD_FRIEND
	{
		{ "PointBlankRe_UserList",			"ListView_UserList",		GUIDE_EVENT_LOBBY_TOP_USER		},	 
		{ "PBRe_PopUp_UserInfo",			"Btn_Multi_Role1",			GUIDE_EVENT_ADD_FRIEND			},	 
		{ "PointBlankRe_PopUp_Default",		"btnOK",					GUIDE_EVENT_ADD_FRIEND_CLOSE	},	 
		{ "",								"",							-1								},
	},

	// GS_REQUEST_JOIN_CLAN
	{
		{ "PointBlankRe_Main",				"i3UIButton_Clan",			GUIDE_EVENT_TOP_CLAN	},
		{ "PointBlankRe_Clan_List",			"Btn_Join",					GUIDE_EVENT_JOIN_CLAN	},
		{ "",								"",							-1},
		{ "",								"",							-1},
	},

	// GS_SEND_MAIL
	{
		{ "PBRe_PopUp_MessageBox",			"btn_SendMail",				GUIDE_EVENT_SEND_MESSAGE	},
		{ "PBRe_PopUp_Message",				"btn_Send",					GUIDE_EVENT_SEND_MESSAGE_OK	},
		{ "",								"",							-1							},
		{ "",								"",							-1							},
	},

	// GS_RECEIVE_PRESENT
	{
		{ "PointBlankRe_Main",				"i3UIButton_Mail_Main",		GUIDE_EVENT_MESSAGE_BOX	},
		{ "PBRe_PopUp_MessageBox",			"tab_Gift",					GUIDE_EVENT_GIFT_BOX	},
		{ "PBRe_PopUp_MessageBox",			"i3UIListView_Box",			GUIDE_EVENT_TOP_MESSAGE	},
		{ "PBRe_PopUp_Message",				"btn_Send",					GUIDE_EVENT_RECV_GIFT	},
	},

	// GS_BUYANDEQUIP_WEAPON
	{
		{ "PointBlankRe_Main",				"i3UIButton_Shop",			GUIDE_EVENT_TOP_SHOP_BUY	},
		{ "PointBlankRe_Shop_Slot_V3",			"i3UIFrameWnd0",			GUIDE_EVENT_M4A1EXT			},
		{ "PointBlankRe_Inven_Menu_plus_finder",		"i3UIButtonComposed3",		GUIDE_EVENT_BUY_M4A1EXT		},
		{ "PBRe_PopUp_ShoppingCart",		"i3UIButtonComposed3",		GUIDE_EVENT_BUY_M4A1EXT_OK	},
	},

	// GS_EQUIP_SKILL
	{
		{ "PointBlankRe_Main",				"i3UIButton_MyInfo",		GUIDE_EVENT_TOP_MYINFO_SKILL	},
		{ "PointBlankRe_Profile_Menu",		"tab_skill",				GUIDE_EVENT_TAP_SKILL			},
		{ "",								"",							-1								},
		{ "",								"",							-1								},

	},

	// GS_BUY_CHARACTER
	{
		{ "PointBlankRe_Main",				"i3UIButton_Shop",			GUIDE_EVENT_TOP_SHOP_CHARA_BUY	},	
		{ "PointBlankRe_Inven_Menu_plus_finder",		"i3UIButton0",				GUIDE_EVENT_CATEGORY_CHARA		},	
		{ "",								"",							-1								},
		{ "",								"",							-1								},
	},

	// GS_RECEIVE_QUEST_REWARD
	{
		{ "PointBlankRe_Main",				"i3UIButton_MyInfo",		GUIDE_EVENT_TOP_MYINFO_QUEST	}, 
		{ "PointBlankRe_Profile_Menu",		"tab_quest",				GUIDE_EVENT_TAP_QUEST			}, 
		{ "PointBlankRe_Profile_Medal_Slot","i3UIFrameWnd_itemSlot_IIS",GUIDE_EVENT_1ST_MEDAL			}, 
		{ "PointBlankRe_Profile_Medal",		"i3UIButton_Lv1",			GUIDE_EVENT_MEDAL_LV1			}, 
	},

	// GS_CHANGE_ROOMSET
	{
		{ "PointBlankRe_ReadyRoom_MapInfo",	"Btn_ML_Arrow",				GUIDE_EVENT_CHANGE_STAGE}, 
		{ "",								"",							-1						},
		{ "",								"",							-1						},
		{ "",								"",							-1						},
	},
};

// 가이드 시스템별 이벤트 갯수입니다. (4개를 넘지 않습니다.)
static const INT32 s_i32GuideCount[GS_MAX_COUNT] = 
{
	3, 2, 4, 2, 2,
	3, 3, 3, 2, 4, 
	4, 2, 2, 4, 1,
};

//////////////////////////////////////////////////////////////////////////////////////////////////

UserGuideMgr::UserGuideMgr(void)
{
	m_GuideTargetList.clear();
	m_EventQueue.clear();

	m_GuideSlotList.clear();

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
	m_SkipCondition = false;
#endif
}

UserGuideMgr::~UserGuideMgr(void)
{
	RemoveUserGuideList();
}

void UserGuideMgr::_AddGuideTargetInfo(GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	i3::vector<GuideTargetInfo>::iterator iter = m_GuideTargetList.begin();
	bool bInsert = true;

	while( iter != m_GuideTargetList.end() )
	{
		if( iter->isSameEvent(pAttr._i32EventIdx) )
		{
			bInsert = false;
			break;
		}

		++iter;
	}

	if(bInsert)
	{
		USER_INFO_BASIC MyInfoBasic; 
		UserInfoContext::i()->GetMyUserInfoBasic(&MyInfoBasic);
		bool bClearGuide = _IsClearGuide(MyInfoBasic.m_ui64GuideComplete, pAttr._i32EventIdx);

		m_GuideTargetList.push_back( GuideTargetInfo(pAttr, bClearGuide) );
	}
}


void UserGuideMgr::_RemoveGuideTargetInfo(GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	i3::vector<GuideTargetInfo>::iterator iter = m_GuideTargetList.begin();

	while( iter != m_GuideTargetList.end() )
	{
		if( iter->isSameAttr(&pAttr) )
		{
			m_GuideTargetList.erase(iter);
			break;
		}

		++iter;
	}
}

GuideTargetInfo * UserGuideMgr::_FindGuideTargetInfo(INT32 i32Type, INT32 i32Number)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return nullptr;

	i3::vector<GuideTargetInfo>::iterator iter = m_GuideTargetList.begin();

	while( iter != m_GuideTargetList.end() )
	{
		if( iter->isSameGuideType(i32Type) && iter->isSameGuideNumber(i32Number) )
		{
			return iter;
		}

		++iter;
	}

	return nullptr;
}

bool UserGuideMgr::_IsPossibleGuide(GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return false;

	// 현재 미구현된 가이드에 대해서는 임시로 건너띕니다.
	if(pAttr._i32GuideType == GS_RECEIVE_PRESENT || pAttr._i32GuideType == GS_EQUIP_SKILL)
		return false;

	// 예외상황에 대해 TargetScene을 변경해줍니다.
	_ChangeTargetScene(pAttr);

	//pScene이 존재할때만 처리
	if(pAttr._pScene != nullptr)
	{
		_FindTargetCrtlFromScene(pAttr);

		if( i3::same_of<i3UIListView_Box*>(pAttr._pCtrl) )
		{
			_FindTargetCtrlFromListView(i3::kind_cast<i3UIListView_Box*>(pAttr._pCtrl), pAttr);
		}
	}

	return pAttr._pCtrl != nullptr;
}

bool UserGuideMgr::_IsClearGuide(UINT64 ui64UserData, INT32 i32EventNum)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return false;

	return (ui64UserData >> i32EventNum) & 0x01;
}

bool UserGuideMgr::_IsClearPrevGuide(INT32 i32Type, INT32 i32Number)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return false;

	USER_INFO_BASIC MyInfoBasic; 
	UserInfoContext::i()->GetMyUserInfoBasic(&MyInfoBasic);

	for(INT32 num = 0; num < i32Number ; ++num)
	{
		if( _IsClearGuide(MyInfoBasic.m_ui64GuideComplete, s_tagGuideEvent[i32Type][num]._i32EventNum) == false )
			return false;
	}

	return true;
}

bool UserGuideMgr::_IsSameScene(const char * pStr1, const char * pStr2)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return false;

	return i3::generic_is_iequal(pStr1, pStr2);
}

void UserGuideMgr::_FindTargetCrtlFromScene(GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	_tagGuideEvent tagEvnet = s_tagGuideEvent[pAttr._i32GuideType][pAttr._i32GuideNum];

	if(_IsSameScene(pAttr._pScene->GetName(), tagEvnet._SceneName.c_str()) == true)
	{
		pAttr._pCtrl = i3::kind_cast<i3UIControl*>(pAttr._pScene->FindChildByName( tagEvnet._CtrlName.c_str()) );
	}
	else
		pAttr._pCtrl = nullptr;
}
void UserGuideMgr::_FindTargetCtrlFromListView(i3UIListView_Box * pLvBox, GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	if(pLvBox != nullptr)
	{
		switch(pAttr._i32GuideType)
		{
		case GS_JOIN_ROOM:
			{
				if(LobbyContext::i()->getLobbyRoomCount() > 0)
				{
					i3::vector<i3UIListView_Item*> pList = 	pLvBox->getItemList();
					pAttr._pCtrl= pList[0];
				}
				else
					pAttr._pCtrl = nullptr;
			}
			break;
		case GS_CREATEROOM_BOMB:
			{
				for(INT32 i32Idx = 0 ; i32Idx < pLvBox->getDataCount(); ++i32Idx)
				{
					i3::wstring wstrGameMode;	
					pLvBox->getDataFromCell(i32Idx, 0, wstrGameMode);

					if( i3::generic_is_iequal(wstrGameMode, GAME_RCSTRING("STR_HEADER_MODE_EXPLOSION")) )
					{
						i3::vector<i3UIListView_Item*> pList = pLvBox->getItemList();
						pAttr._pCtrl = pList[i32Idx];
						break;
					}
					else
						pAttr._pCtrl = nullptr;
				}
			}
			break;
		case GS_ADD_FRIEND:
			{
				const i3::rc_wstring wstrUserInfoMyNick = UserInfoContext::i()->GetMyNickName();

				for(INT32 i32Idx = 0 ; i32Idx < pLvBox->getDataCount(); ++i32Idx)
				{
					i3::wstring wstrNick;	
					pLvBox->getDataFromCell(i32Idx, 2, wstrNick);

					if(i3::generic_string_size(wstrNick) > 0)
					{
						if(i3::generic_is_iequal(wstrNick, wstrUserInfoMyNick ) == false)
						{
							i3::vector<i3UIListView_Item*> pList = pLvBox->getItemList();
							pAttr._pCtrl = pList[i32Idx];
							break;
						}
						else
							pAttr._pCtrl = nullptr;
					}
					else
						pAttr._pCtrl = nullptr;
				}
			}
			break;
		case GS_RECEIVE_PRESENT:
			{
				i3::vector<i3UIListView_Item*> pList = 	pLvBox->getItemList();
				pAttr._pCtrl= pList[0];
			}
			break;
		default:
			break;
		}
	}

	// ListView_Box는 대상에서 제외
	if( i3::same_of<i3UIListView_Box*>(pAttr._pCtrl) )
		pAttr._pCtrl = nullptr;
}

// 부득의하게 예외처리를 통하여 Scene을 변경
void UserGuideMgr::_ChangeTargetScene(GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	_tagGuideEvent tagEvent = s_tagGuideEvent[pAttr._i32GuideType][pAttr._i32GuideNum];

	switch(pAttr._i32GuideType)
	{
	case GS_CHANGE_WEAPON:
		{
			UISideMenu * pUIFrame = GetMainFrame()->GetSideMenu();

			if( pUIFrame != nullptr)
			{
				if(pAttr._i32GuideNum == 1)
				{
					SMenuEquip * pEquip = static_cast<SMenuEquip*>(pUIFrame->get_list_ptr(0));

					pAttr._pScene = pEquip->GetMainSlotScene(0);
				}
				else if(pAttr._i32GuideNum == 2)
				{
					SMenuEquip * pEquip = static_cast<SMenuEquip*>(pUIFrame->get_list_ptr(0));

					if(pEquip->IsSelectedSlot(0) == true)
						pAttr._pScene = pEquip->GetSubSlotScene(1);
					else
						pAttr._pScene = nullptr;
				}
				else if(pAttr._i32GuideNum == 3)
				{
					pAttr._pScene = pUIFrame->GetScene(tagEvent._SceneName.c_str() );
				}
			}
		}
		break;
	case GS_CHANGE_CHARACTER_NAME:
	case GS_REQUEST_JOIN_CLAN:
	case GS_RECEIVE_PRESENT:
	case GS_EQUIP_SKILL:
		{
			if(pAttr._i32GuideNum == 0)
			{
				UITopMenu * pUIFrame = i3::kind_cast<UITopMenu *>(GetMainFrame()->GetTopMenu());
				if(pUIFrame != nullptr)
					pAttr._pScene = pUIFrame->GetScene(tagEvent._SceneName.c_str() );
			}
		}
		break;
	case GS_CHANGE_CHARACTER:
		{
			UIFloatCharacterView * pUIFrame = i3::kind_cast<UIFloatCharacterView *>(GameUI::GetFloating(UFW_CHARACTER));
			if( pUIFrame != nullptr)
				pAttr._pScene = pUIFrame->GetScene(tagEvent._SceneName.c_str() );
		}
		break;
	case GS_BUYANDEQUIP_WEAPON:
		{
			if(pAttr._i32GuideNum == 0)
			{
				UITopMenu * pUIFrame = i3::kind_cast<UITopMenu *>(GetMainFrame()->GetTopMenu());
				if(pUIFrame != nullptr)
					pAttr._pScene = pUIFrame->GetScene(tagEvent._SceneName.c_str() );
			}
			else if(pAttr._i32GuideNum == 1)
			{
				UIPhaseShop * pUIFrame = i3::kind_cast<UIPhaseShop*>(GetMainFrame()->GetSubPhase(USP_SHOP));
				pAttr._pScene = pUIFrame->GetItemSlotScene(0);
			}
			else if(pAttr._i32GuideNum == 2)
			{
				UIPhaseShop * pUIFrame = i3::kind_cast<UIPhaseShop*>(GetMainFrame()->GetSubPhase(USP_SHOP));
				pAttr._pScene = pUIFrame->GetMainTabScene();
			}
		}
		break;
	case GS_BUY_CHARACTER:
		{
			if(pAttr._i32GuideNum == 0)
			{
				UITopMenu * pUIFrame = i3::kind_cast<UITopMenu *>(GetMainFrame()->GetTopMenu());
				if(pUIFrame != nullptr)
					pAttr._pScene = pUIFrame->GetScene(tagEvent._SceneName.c_str() );
			}
			else
			{
				UIPhaseShop * pUIFrame = i3::kind_cast<UIPhaseShop*>(GetMainFrame()->GetSubPhase(USP_SHOP));
				pAttr._pScene = pUIFrame->GetMainTabScene();
			}
		}
		break;
	case GS_RECEIVE_QUEST_REWARD:
		{
			UIBase * pFrame = nullptr;
		
			if(pAttr._i32GuideNum == 0)
			{
				pFrame = i3::kind_cast<UIBase*>(GetMainFrame()->GetTopMenu());
			}
			else if(pAttr._i32GuideNum == 1)
			{
				pFrame = i3::kind_cast<UIBase*>(GetMainFrame()->GetPopup(UPW_PROFILE));
			}
			else
			{
				UIPhaseProfile * pProfile = i3::kind_cast<UIPhaseProfile*>(GetMainFrame()->GetPopup(UPW_PROFILE));
				pFrame = i3::kind_cast<UIBase*>(pProfile->GetTab(UIPhaseProfile::TAB_MEDAL));
			}

			if(pFrame != nullptr)
				pAttr._pScene = pFrame->GetScene(tagEvent._SceneName.c_str() );
		}
		break;
	default:
		break;
	}
}

bool UserGuideMgr::_CheckDuplicateCtrl(i3UIControl * pCtrl)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return false;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		UIUserGuideSlot * pSlot = _GetGuideSlot(idx);	
		if( pSlot->isFrameEnable() == false )
			continue;

		if(pSlot->isSameTargetCtrl(pCtrl) == true)
			return true;
	}

	return false;
}

bool UserGuideMgr::_CheckGuideCondition(GuideTargetAttr & pAttr)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return false;

	if(pAttr._pScene == nullptr || pAttr._pCtrl == nullptr)
		return false;

	struct PhaseCondition
	{
		UIPHASE		_phase;
		UISUBPHASE	_subphase;

		PhaseCondition(UIPHASE phase, UISUBPHASE subphase)
		{
			_phase = phase;
			_subphase = subphase;
		}

		bool IsSamePhase(PhaseCondition & condition)
		{
			return (_phase == condition._phase) && (_subphase == condition._subphase);
		}
	};

	PhaseCondition CurPhase( GetMainFrame()->GetCurrentPhaseType(), GetMainFrame()->GetCurrentSubPhaseType() );
	PhaseCondition ComparePhase(UIPHASE_START, USP_NONE);
	
	switch(pAttr._i32GuideType)
	{
	case GS_JOIN_ROOM:
		{
			// 클랜 채널에서는 GUIDE_EVENT_VIEW_ROOM는 활성화 시키지 않습니다.
			if(pAttr._i32GuideNum == 0)
			{
				if(ChannelContext::i()->GetBattleChannel() == false)
					ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
			else if( pAttr._i32GuideNum == 1 )
			{
				if( LobbyContext::i()->getLobbyRoomCount() > 0)
					ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
			else
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
				
		}
		break;
	case GS_CREATEROOM_DEATHMATCH:
		{
			const USER_INFO_RECORD * pMyRecord = UserInfoContext::i()->GetMyUserInfoRecord();
			INT32 i32RealMatch = pMyRecord->_match - pMyRecord->_dis;

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( i32RealMatch >= 3 || m_SkipCondition)
#else
			if( i32RealMatch >= 3)
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
		}
		break;
	case GS_CHANGE_WEAPON:
		{
			// 로그인 3회 / 주무기 2개 이상일 경우

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( UserInfoContext::i()->getLoginCount() >= 3 &&
				CInvenList::i()->GetItemCount( INVEN::DB_WEAPON, WEAPON_SLOT_PRIMARY) >= 2 ||
				m_SkipCondition )
#else
			if( UserInfoContext::i()->getLoginCount() >= 3 &&
				CInvenList::i()->GetItemCount( INVEN::DB_WEAPON, WEAPON_SLOT_PRIMARY) >= 2)
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}			
		}
		break;
	case GS_CHANGE_CHARACTER_NAME:
		{
#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if (CCharaInfoContext::i()->GetHasCharacterCount() >= 2 || m_SkipCondition )
#else
			if( CCharaInfoContext::i()->GetHasCharacterCount() >= 2)
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
		}
		break;
	case GS_CHANGE_CHARACTER:
		{
#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( CCharaInfoContext::i()->GetHasCharacterCount() >= 2 || m_SkipCondition )
#else
			if( CCharaInfoContext::i()->GetHasCharacterCount() >= 2)
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
		}
		break;
	case GS_CREATEROOM_BOMB:
		{

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( UserInfoContext::i()->GetMyRank() >= RANK_01 || m_SkipCondition )
#else
			if( UserInfoContext::i()->GetMyRank() >= RANK_01)
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
		}
		break;
	case GS_ADD_FRIEND:
		{
			if( pAttr._i32GuideNum == 0 && LobbyContext::i()->GetLobbyUserCount() > 1 )
			{			
				UIFloatUserList * pUIFrame = i3::kind_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
				if( pUIFrame != nullptr && pAttr._pCtrl->isEnable() && pUIFrame->getCurrentUserTab() == USERLIST_TAB_CURRENT )
					ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
			else
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
		}
		break;
	case GS_REQUEST_JOIN_CLAN:
		{
			USER_INFO_BASIC MyInfo;
			UserInfoContext::i()->GetMyUserInfoBasic(&MyInfo);

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( (MyInfo.m_ui32Rank >= RANK_13 && MyInfo.m_ui32Clanidx == 0 && MyInfo.m_ui32ClanState == 0) || m_SkipCondition )
#else
			if( MyInfo.m_ui32Rank >= RANK_13 && MyInfo.m_ui32Clanidx == 0 && MyInfo.m_ui32ClanState == 0 )
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
			}
		}
		break;
	case GS_SEND_MAIL:
		{
			ComparePhase = PhaseCondition(UIPHASE_LOBBY, USP_NONE);
		}
		break;
	case GS_RECEIVE_PRESENT:
		{

		}
		break;
	case GS_BUYANDEQUIP_WEAPON:
		{
			const USER_INFO_RECORD * pMyRecord = UserInfoContext::i()->GetMyUserInfoRecord();
			INT32 i32RealMatch = pMyRecord->_match - pMyRecord->_dis;

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( i32RealMatch >= 1 || m_SkipCondition )
#else
			if( i32RealMatch >= 1 )
#endif
			{
				if(pAttr._i32GuideNum == 0)
				{
					ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_NONE);
				}
				else if(pAttr._i32GuideNum == 1)
				{
					ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_SHOP);

					if( ( CurPhase.IsSamePhase(ComparePhase) == true)
						&& (GetMainFrame()->GetCurrentSubPhaseState() == PS_ENTRANCE_END))
					{	
						UIPhaseShop * pPhase = i3::kind_cast<UIPhaseShop*>(GetMainFrame()->GetCurrentSubPhase());
						pPhase->SetListFrontItemByItemID(103003);
					}
				}
				else
				{
					ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_SHOP);
				}
			}
		}
		break;
	case GS_EQUIP_SKILL:
		{
			// 현재 사용하지 않음.
			//ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_NONE);
		}
		break;	
	case GS_BUY_CHARACTER:
		{

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( UserInfoContext::i()->GetMyRank() >= RANK_10 || m_SkipCondition )
#else
			if( UserInfoContext::i()->GetMyRank() >= RANK_10 )
#endif
			{
				if(pAttr._i32GuideNum == 0)
					ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_NONE);
				else
					ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_SHOP);
			}
		}
		break;	
	case GS_RECEIVE_QUEST_REWARD:
		{
			INT32 i32Count = CMedalManager::i()->getMedalSetCount(MEDAL_SET_TYPE_NORMAL);
			bool bCheckAcquireMedal = false;

			// 획득한 메달이 있는지 확인
			for(INT32 i = 0 ; i < i32Count ; ++i)
			{
				if(CMedalManager::i()->isAcquired(MEDAL_SET_TYPE_NORMAL, i))
				{
					bCheckAcquireMedal = true;
					break;
				}
			}

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( bCheckAcquireMedal == true || m_SkipCondition )
#else
			if( bCheckAcquireMedal == true )
#endif
			{
				UIPhaseProfile * pUIFrame = i3::kind_cast<UIPhaseProfile*>(GetMainFrame()->GetPopup(UPW_PROFILE));
				UITabProfileMedal * pTabFrame = i3::kind_cast<UITabProfileMedal*>(pUIFrame->GetTab(UIPhaseProfile::TAB_MEDAL));
				if(pTabFrame->isEnable() && pAttr._i32GuideNum == 2)
				{
					pTabFrame->ChangeSortType(MEDAL_SORT_ACQUIRE);
					pTabFrame->SetComboEditBoxText((INT32)MEDAL_SORT_ACQUIRE);
				}

				ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_NONE);
			}
		}
		break;	
	case GS_CHANGE_ROOMSET:
		{
#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			if( (UserInfoContext::i()->GetMyRank() >= RANK_04 && 
				BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx())	|| m_SkipCondition )
#else
			if( UserInfoContext::i()->GetMyRank() >= RANK_04 && 
				BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx() )
#endif
			{
				ComparePhase = PhaseCondition(UIPHASE_READY_ROOM, USP_NONE);
			}
		}
		break;	
	default:
		break;
	}

	return CurPhase.IsSamePhase(ComparePhase);
}


void UserGuideMgr::_PlayStartMessage(INT32 i32GuideType, INT32 i32GuideNum)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	if(i32GuideNum > 0)
		return;

	bool bPlaySound = true;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		if( idx == (UINT32)i32GuideType)
			continue;

		UIUserGuideSlot * pSlot = _GetGuideSlot(idx);	

		// 이미 사운드를 플레이한 다른 개체가 있으면, 시작 사운드를 출력 안함.
		if(pSlot->isFrameEnable() && pSlot->GetGuideNumber() == 0)
		{
			bPlaySound = false;
			break;
		}
	}

	if(bPlaySound == true)
	{
		UIUserGuideSlot * pSlot = _GetGuideSlotInfo(i32GuideType)->_pSlot;
		pSlot->PlaySound(UISND_USER_GUIDE_START);
	}

	i3::string strGuideMessage;
	i3::rc_wstring wstrGuideMessage; 

	i3::sprintf(strGuideMessage, "STR_USER_GUIDE_SYSTEM%d", (i32GuideType + 1));
	i3::sprintf(wstrGuideMessage, GAME_RCSTRING("STR_USER_GUIDE_CHAT_MESSAGE"), GAME_RCSTRING(strGuideMessage));

	g_pFramework->getChatBox()->PutSystemChatting(wstrGuideMessage);
}


void UserGuideMgr::InitUserGuideList(void)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	UIMainFrame * pFrame = GetMainFrame();

	for(INT32 i = 0 ; i < GS_MAX_COUNT; ++i)
	{
		GuideSlotInfo pSlotInfo(nullptr);		
		pSlotInfo._pSlot = UIUserGuideSlot::new_object();
		pSlotInfo._pSlot->OnCreate(pFrame);
		bool resSlot = pSlotInfo._pSlot->OnEntranceStart(nullptr, nullptr);
		I3ASSERT(resSlot);
		pSlotInfo._pSlot->ExitSlot();

		m_GuideSlotList.push_back(pSlotInfo);
	}
}

void UserGuideMgr::RemoveUserGuideList(void)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		GuideSlotInfo * pSlotInfo = _GetGuideSlotInfo(idx);	
		pSlotInfo->_pSlot->Destroy();
		pSlotInfo->_pSlot = nullptr;
	}

	m_GuideSlotList.clear();
	m_GuideTargetList.clear();
}

void UserGuideMgr::OnLoadGuideTargetInfo(UIBase * pTargetFrame)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	if(GetMainFrame() == nullptr || (g_pEnvSet->m_nUIVersion != UFV_1_5))
		return;

	if((ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN) &&
		UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
		return;

	for(INT32 type = 0 ; type < GS_MAX_COUNT ; ++type)
	{
		for(INT32 num = 0 ; num < s_i32GuideCount[type]; ++num)
		{
			_tagGuideEvent tagEvent = s_tagGuideEvent[type][num];

			if(tagEvent._i32EventNum < 0)	
				continue;

			GuideTargetAttr TargetAttr;
			TargetAttr.Reset();

			TargetAttr._pScene = pTargetFrame->GetScene(tagEvent._SceneName.c_str());
			TargetAttr._i32GuideType = type;
			TargetAttr._i32GuideNum = num;
			TargetAttr._i32EventIdx = tagEvent._i32EventNum;

			if( _IsPossibleGuide(TargetAttr) )
				_AddGuideTargetInfo(TargetAttr);
		}
	}
}


void UserGuideMgr::UnLoadGuideTargetInfo(UIBase * pTargetFrame)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	if(GetMainFrame() == nullptr || m_GuideTargetList.size() == 0)
		return; 

	for(INT32 type = 0 ; type < GS_MAX_COUNT ; ++type)
	{
		for(INT32 num = 0 ; num < s_i32GuideCount[type]; ++num)
		{
			_tagGuideEvent tagEvent = s_tagGuideEvent[type][num];

			if(tagEvent._i32EventNum < 0)	
				continue;

			GuideTargetAttr TargetAttr;
			TargetAttr.Reset();

			TargetAttr._pScene = pTargetFrame->GetScene(tagEvent._SceneName.c_str());
			TargetAttr._i32GuideType = type;
			TargetAttr._i32GuideNum = num;
			TargetAttr._i32EventIdx = tagEvent._i32EventNum;

			if( _IsPossibleGuide(TargetAttr) )
				_RemoveGuideTargetInfo(TargetAttr);
		}
	}

	StopGuideSystem(pTargetFrame);
}

void UserGuideMgr::PlayGuideSystem(INT32 i32GuideType)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	UIUserGuideSlot * pSlot = _GetGuideSlot(i32GuideType);

	if( pSlot->isFrameEnable() == false)
	{
		for(INT32 i32GuideNum = 0 ; i32GuideNum < s_i32GuideCount[i32GuideType]; ++i32GuideNum)
		{
			GuideTargetInfo * pInfo = _FindGuideTargetInfo(i32GuideType, i32GuideNum);

			if(pInfo == nullptr || pInfo->_bCleard == true) 
				continue;

			GuideTargetAttr TargetAttr = pInfo->_GuideTargetAttr;

			if( _CheckGuideCondition(TargetAttr) && _IsClearPrevGuide(i32GuideType, i32GuideNum) )
			{
				bool bDuplicate = _CheckDuplicateCtrl(TargetAttr._pCtrl);

				// 임시 예외 처리.. (방장 가이드가 사라지는건지 확인 필요)
				if(TargetAttr._i32GuideType == GS_CHANGE_ROOMSET && TargetAttr._pCtrl->isVisible() == false )
					continue;

				// 활성화된 컨트롤만 가이드를 띄웁니다.
				// 단, 캐릭터 교체 가이드는, 대상 컨트롤이 마우스 오버 상태에 따라 Disable 되므로, 
				// isActivated로 활성 여부를 판단합니다.
				bool isChangeCharaGuide = (TargetAttr._pCtrl->isActivated() && TargetAttr._i32GuideType == GS_CHANGE_CHARACTER);

				if(TargetAttr._pCtrl->isEnable() || isChangeCharaGuide )
				{
					if(TargetAttr._i32EventIdx == 29 && g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_BUYITEM))
						bDuplicate = true;

					pSlot->EntranceSlot(&TargetAttr, bDuplicate);
					_PlayStartMessage(i32GuideType, i32GuideNum);
				}
				break;
			}
			else
			{
				if( pInfo->_bCleard == false)
					break;
			}
		}
	}
}

void UserGuideMgr::StopGuideSystem(UIBase * pTargetFrame)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		UIUserGuideSlot * pSlot = _GetGuideSlot(idx);
		// 사용중인 코인 데이터가 Exit된 Scene과 같다면 함께 Exit
		if(pSlot->isFrameEnable())
		{
			const GuideTargetAttr * pAttr = pSlot->GetTargetAttr();

			if(i3::kind_of<UISideMenu*>(pTargetFrame) == true)
			{
				if(pAttr->_i32GuideType == GS_CHANGE_WEAPON && pAttr->_i32GuideNum > 0)
					pSlot->ExitSlot();
			}
			else if(i3::kind_of<UIPhaseShop*>(pTargetFrame) == true)
			{
				if(pAttr->_i32GuideType == GS_BUYANDEQUIP_WEAPON && pAttr->_i32GuideNum == 1)
					pSlot->ExitSlot();
			}
			else
			{
				_tagGuideEvent tagEvent = s_tagGuideEvent[pAttr->_i32GuideType][pAttr->_i32GuideNum];

				if(pTargetFrame->GetScene(tagEvent._SceneName.c_str()) == pAttr->_pScene)
				{
					pSlot->ExitSlot();
				}
			}
		}
	}
}

void UserGuideMgr::ChangeNextGuide(GuideSlotInfo * pSlotInfo)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	// 유저가이드 타입과, 타입 내 이벤트의 차례를 가져옴
	USER_INFO_BASIC MyInfoBasic; 
	UserInfoContext::i()->GetMyUserInfoBasic(&MyInfoBasic);

	const GuideTargetAttr * pCurAttr = pSlotInfo->_pSlot->GetTargetAttr();
	GuideTargetInfo * pCurInfo = _FindGuideTargetInfo(pCurAttr->_i32GuideType, pCurAttr->_i32GuideNum);
	if(pCurInfo != nullptr)
		pCurInfo->_bCleard = true;

	GuideTargetInfo * pNextInfo = _FindGuideTargetInfo(pCurAttr->_i32GuideType, (pCurAttr->_i32GuideNum + 1) );
	if(pNextInfo != nullptr)
	{
		GuideTargetAttr NextAttr = pNextInfo->_GuideTargetAttr;

		if( _CheckGuideCondition(NextAttr) )
		{
			bool bDuplicate = _CheckDuplicateCtrl(NextAttr._pCtrl);
			pSlotInfo->_pSlot->SetGuideAttr(&NextAttr, bDuplicate);
		}
		else
		{
			pSlotInfo->_pSlot->ExitSlot();
		}
	}
	else
	{
		pSlotInfo->_pSlot->ExitSlot();
	}

	pSlotInfo->_bSendEvent = false;
}

void UserGuideMgr::DisableUserGuideAll(void)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		UIUserGuideSlot * pSlot = _GetGuideSlot(idx);
		if(pSlot->isFrameEnable())
		{
			pSlot->ExitSlot();
		}
	}
}

// 업데이트를 UIMainFrame부터 시작하여, 종료시 애니메이션 컨트롤
void UserGuideMgr::OnUpdate(REAL32 rDeltaSeconds)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	for(INT32 idx = 0 ; idx < (INT32)m_GuideSlotList.size() ; ++idx)
	{
		GuideSlotInfo * pSlotInfo = _GetGuideSlotInfo(idx);
		UIUserGuideSlot * pSlot = pSlotInfo->_pSlot;

		if( pSlot->isFrameEnable() )
		{
			// 코인이 정상적으로 사라졌으면 상태를 변경, 아니면 애니메이션 계속 진행
			if(pSlot->isEndAnimation() == true)
			{				
				ChangeNextGuide(pSlotInfo);
			}
			else
			{
				const GuideTargetAttr * pAttr = pSlot->GetTargetAttr();

				GuideTargetAttr chk_attr;
				chk_attr.CopyFrom(pAttr);

				bool is_update_slot = _CheckGuideCondition(chk_attr);

				if( is_update_slot && pAttr->_pCtrl != nullptr && (i3::kind_of<i3UIControl*>(pAttr->_pCtrl)))
				{
					bool isChangeCharaGuide = (pAttr->_pCtrl->isActivated() && pAttr->_i32GuideType == GS_CHANGE_CHARACTER);

					if(pAttr->_pCtrl->isEnable() || isChangeCharaGuide )
						pSlot->OnUpdateGuideSlot(rDeltaSeconds);
					else
						pSlot->ExitSlot();
				}
				else
					pSlot->ExitSlot();
			}
		}
		else
			PlayGuideSystem(idx);
	}
}

void UserGuideMgr::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	if( event == I3_EVT_UI_NOTIFY)
	{
		if(pObj != nullptr)
		{
			I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;
			i3UIControl * pCtrl = pNotify->m_pCtrl;

			if( pCtrl != nullptr)
			{
				switch( pNotify->m_nEvent)
				{	
				case I3UI_EVT_BTN_PUSHED:
					{
						if(i3::kind_cast<i3UICheckBox*>(pCtrl))
							OnClick(pCtrl, false);
					}
					break;
				case I3UI_EVT_CLICKED :		
					{
						OnClick(pCtrl, false);
					}
					break;
				case I3UI_EVT_DBL_CLICKED :
					{
						OnClick(pCtrl, true);
					}
					break;
				case I3UI_EVT_SCROLL:
					{
						OnScroll(pCtrl);
					}
					break;
				}
			}
		}
	}
#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
	else if( event == I3_EVT_INPUT)
	{
		i3InputKeyboard* pKeyboard = i3::kind_cast<i3InputDeviceManager*>(pObj)->GetKeyboard();

		if( pKeyboard->GetPressState( I3I_KEY_LCTRL))
		{
			if( pKeyboard->GetStrokeState( I3I_KEY_PGDN) )
			{
				m_SkipCondition = true;
			}
			else if( pKeyboard->GetStrokeState( I3I_KEY_PGUP) )
			{
				m_SkipCondition = false;
			}
		}
	}
#endif
}

void UserGuideMgr::OnClick(i3UIControl * pCtrl, bool bDBclick)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		GuideSlotInfo * pSlotInfo = _GetGuideSlotInfo(idx);	

		if( pSlotInfo->_pSlot->isFrameEnable() == false )
			continue;
		
		if(pSlotInfo->_bSendEvent == true)
			continue;

		if( pSlotInfo->_pSlot->isSameTargetCtrl(pCtrl) )
		{
			if( pSlotInfo->_pSlot->SendClickEvent(bDBclick) )
				pSlotInfo->_bSendEvent = true;
		}
	}
}

void UserGuideMgr::OnScroll(i3UIControl * pCtrl)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
	{
		UIUserGuideSlot * pSlot = _GetGuideSlot(idx);	

		if(pSlot->isFrameEnable() == false)
			continue;

		if( i3::same_of<i3UIListView_Box*>(pCtrl) )
		{			
			pSlot->ProcessLvBoxScroll((i3UIListView_Box*)pCtrl);
		}
		else if( i3::same_of<i3UIScrollBar*>(pCtrl) )
		{
			i3UIScrollBar * pScroll = (i3UIScrollBar*)pCtrl;

			if(pSlot->GetEventIndex() == GUIDE_EVENT_M4A1EXT ||
				pSlot->GetEventIndex() == GUIDE_EVENT_1ST_MEDAL)
			{
				if(pScroll->getScrollPos() == 0)
					pSlot->ProcessSlotScroll(true);
				else
					pSlot->ProcessSlotScroll(false);
			}
		}
	}
}

void UserGuideMgr::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	if ( g_pEnvSet && g_pEnvSet->m_nUIVersion != UFV_1_5)
		return;

	switch(evt)
	{
	case EVENT_USER_GUIDE:
		{
			if( EV_SUCCESSED(arg))
			{
				for(size_t idx = 0 ; idx < m_GuideSlotList.size() ; ++idx)
				{
					GuideSlotInfo * pSlotInfo = _GetGuideSlotInfo(idx);
					UIUserGuideSlot * pSlot = pSlotInfo->_pSlot;	

					if( pSlot->isSameTargetEvent(m_EventQueue[0]._ui32EventIdx) )
					{
						if( pSlot->isFrameEnable() && pSlot->isDuplicate() == false)
							pSlot->ProcessGameEvent(evt, arg, UserData);
						else
							ChangeNextGuide(pSlotInfo);

						break;
					}
				}

				i3::vu::erase_index(m_EventQueue, 0);
			}
		}
		break;
	default:
		break;
	}
}