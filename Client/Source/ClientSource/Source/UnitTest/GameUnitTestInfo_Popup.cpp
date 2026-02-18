#include "pch.h"
#if ENABLE_UNIT_TEST

#include "GameUnitTestInfo_Popup.h"

#include "../UI/UIMainFrame.h"
#include "../UI/UIPopupCreateRoom.h"
#include "../UI/UIOptionBase.h"
#include "../UI/UIPopupOption.h"
#include "../UI/UIPopupReceiveMail.h"

#include "../UI/UITabProfileMedal.h"
#include "../UI/UITabProfileInfo.h"

#include "../UI/UIPopupBuyItem.h"
#include "../UI/UIPopupShopExtension.h"
#include "../UI/UIPopupCouponList.h"

#include "../UI/UIPopupUserProfile.h"
#include "../UI/UIPhaseReadyRoom.h"
#include "../UI/UIPopupInviteLobbyUser.h"

#include "../UI/UIPhaseLobby.h"
#include "../UI/UIPopupQueryPassword.h"
#include "../UI/UIPopupCreateClan.h"
#include "../UI/UIPopupClanNameSearch.h"
#include "../UI/UIPopupJoinClan.h"
#include "../UI/UIPopupRankup.h"
#include "../UI/UIPopupPCCafe.h"
#include "../UI/UIPopupAddBlockList.h"

#include "../UI/UIPopupQueryItem.h"
#include "../UI/UIPopupSkillOpen.h"
#include "../UI/Useable_Item_Def.h"
#include "../UI/UIPopupResultUseItem.h"
#include "../UI/UIPopupUseItemChangeNick.h"
#include "../UI/UIPopup_Clan_ChangeName.h"
#include "../UI/UIPopupUseItemChangeColorNick.h"
#include "../UI/UIPopupUseItemChangeCrosshairColor.h"
#include "../UI/UIPopupUseItemDisguiseRank.h"
#include "../UI/UIPopupUseItemChangeClanMark.h"

#include "../UI/UIPopupCouponInput.h"
#include "../UI/UIPopupDiscountCoupon.h"
#include "../UI/UIPopupEvent.h"
#include "../UI/UIPopupExit.h"
#include "../UI/UIPopupTSReward.h"

#include "../UI/UIPopupVisitEvent.h"
#include "../UI/UIPopupAttendanceCheck.h"
#include "../UI/UIPopupAttendanceComplete.h"
#include "../UI/UIPopupClanHonor.h"
#include "../UI/UIPhaseNonClanMember.h"
#include "../UI/UIPhaseClanMember.h"
#include "../UI/UIPopupMedalReward.h"
#include "../UI/UIPopupBreakIntoGame.h"
#include "../UI/UIPopupWebBox.h"
#include "../UI/UIPopupDomiScore.h"
#include "../UI/UIPopupDomiRound.h"
#include "../UI/UIPopupDomiEnter.h"

#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "InvenDef.h"

static void _JoinRoomPasswordCallback( bool OK, const i3::string & Password, void * pUserContext)
{
	if ( OK )
	{
	
	
		I3ASSERT(Password.size() <= sizeof(char) * NET_ROOM_PW);

		if( Password.size() > 0)
		{
			std::size_t cnt = sizeof(char) * NET_ROOM_PW;
			if ( Password.size() < sizeof(char) * NET_ROOM_PW)
				cnt = Password.size();

			MyRoomInfoContext::i()->SetMyRoomPW( Password.c_str(), cnt);
		}
		else
		{
			MyRoomInfoContext::i()->ClearMyRoomPW();
		}


		UIPhaseLobby *	pLobby = (UIPhaseLobby *)pUserContext;

		if( MyRoomInfoContext::i()->JoinRoom(&pLobby->GetJoinTargetRoom()) == true)
			i3UI::setDisableUIInput(true);
	}
}

namespace GameUnitTest
{
	namespace Popup
	{
		factory g_factory;

		void TesterBase::Close()
		{
			g_pFramework->GetUIMainframe()->ClosePopup( UIPOPUPWIN(m_type));
		}

		void TesterBase::Open()
		{
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type));
		}

		void TesterBase::Process()
		{
			//기본적으로 팝업을 닫는 기능...
			if( i3Math::Rand()%2)
				Close();
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void CreateRoomLobby::Open()
		{
			bool isLobby = true;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &isLobby);
		}

		void CreateRoomLobby::Process()
		{
			/*
			UIPopupCreateRoom* p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));
			
			I3ASSERT_RETURN(p);

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0: p->ChangeObjectCondition( i3Math::Rand()); break;
			case 1: p->ChangeTimeLimit( i3Math::Rand()); break;
			case 3: p->ChangeAICount( i3Math::Rand()); break;
			case 4: p->ChangeAILevel( i3Math::Rand()); break;	
			case 5: p->MapListScroll(); break;
			case 6: p->MapListWheel( i3Math::Rand()); break;
			case 7: p->MapListDBClick( i3Math::Rand()); break;
			case 8: p->ClickRandomMap(); break;
			case 9: p->ClickMapSlot(i3Math::Rand()%UIPopupCreateRoomEx_Stage::MAPLIST_SLOT_COUNT); break;
			case 10:
				{		
					bool is_special = (i3Math::Rand()%2 == 1);
					INT32 _count = is_special ? p->GetModeListSpecialSize() : p->GetModeListNormalSize();
					
					p->SelectModeType( i3Math::Rand()%_count, is_special);
				}
				break;
			case 11: p->OnMouseOverSlot( i3Math::Rand()%UIPopupCreateRoomEx_Stage::MAPLIST_SLOT_COUNT, i3Math::Rand()%2 == 1); break;
			case 12: break;
			case 13: if( i3Math::Rand()%2) { Close();} break;
			}
			*/
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void Option::Close()
		{
			UIPopupOption* p = static_cast<UIPopupOption*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));
			if( p != nullptr)
				p->GetCurrentOption()->key_escape();
			g_pFramework->GetUIMainframe()->ClosePopup( UIPOPUPWIN(m_type));
		}

		void Option::Process()
		{
			UIPopupOption* p = static_cast<UIPopupOption*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2: case 16:
				{
					m_tab = i3Math::Rand()%MAX;
					if( p != nullptr)
						p->clicked( m_tab);
				}
				break;
			case 4:case 5: case 6: case 7: case 8: case 9:
			case 10:case 11: case 12: case 13: case 14: case 15: case 17: case 18: case 19:
				{
					INT32 _idx = i3Math::Rand()%m_event_count[m_tab];
					if( p != nullptr)
						p->GetCurrentOption()->clicked( _idx, i3Math::Rand());
				}
				break;
			case 20: if( i3Math::Rand()%2) { Close();} break;
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void ReceiveMail::Open()
		{
			UIPopupReceiveMail::OPEN_DATA data;
			data.mailType			= (UIPopupReceiveMail::MAILTYPE)(i3Math::Rand() % UIPopupReceiveMail::MAX_MAILTYPE);
			data.noteAppendData		= 0;
			data.noteIdx			= 0;
			data.wstrAttachedItem		= L"unit_test_gift";
			data.wstrMessage			= L"unit_testing";
			data.wstrNickName			= L"unit_test";

			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &data);
		}

		void ReceiveMail::Process()
		{
			UIPopupReceiveMail* p = static_cast<UIPopupReceiveMail*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:
				case 1: p->CheckID(); break;
				case 2: 
				case 3: p->SendClanJoinAsk( i3Math::Rand()%2 ? true : false); break;
				case 4: if( i3Math::Rand()%2) { Close();} break;
				case 5:
				case 6: p->OnKey_Enter(); break;
				case 7:
				case 8: p->OnKey_Escape(); break;
				}
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void MailBox::Process()
		{
			UIPopupMailBox* p = static_cast<UIPopupMailBox*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0: p->ChangeTab( i3Math::Rand()); break;
				case 1: p->SetReadedMail(); break;
				case 2: p->DeleteMail(); break;
				// 차후 수정 예정 - 양승천
				//case 3: p->SendMail( i3Math::Rand()); break;
				//case 4: p->ReadMail( i3Math::Rand()); break;
				//case 5: p->SortMailList(); break;
				case 6: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void Profile::Process()
		{
			UIPhaseProfile* p = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:
				m_tab = i3Math::Rand()%UIPhaseProfile::TAB_MAX;
				if( p != nullptr)
					p->ChangeTab( UIPhaseProfile::PROFILETAB(m_tab));
				break;

			case 3:
				if( p != nullptr)
					p->GotoBackTab();
				break;

			case 4:case 5:case 6:case 7:case 8:case 9:
				if( p != nullptr)
				{
					switch( p->GetCurrentTabType())
					{
					//medal
					case UIPhaseProfile::TAB_MEDAL:
						{
							UITabProfileMedal* tab = (UITabProfileMedal*)p->GetTab( UIPhaseProfile::TAB_MEDAL);

							INT32 _idx = i3Math::Rand()%8;
							switch( _idx)
							{
							case 0: tab->ChangeTab( MEDALTAB(i3Math::Rand())); break;
							case 1: tab->ChangeSortType( i3Math::Rand()); break;
							case 2: tab->ChangeLevel( i3Math::Rand(), i3Math::Rand()%2 == 1); break;
							case 3: tab->GetReward( i3Math::Rand()); break;
							case 4: tab->NotifySelected( i3Math::Rand()); break;
							case 5: tab->NotifySelectedClear( i3Math::Rand()); break;
							case 6: tab->NotifyAllClear(); break;
							case 7: tab->UpdateScroll(0); break;
							}
						}
						break;

					//profile info
					case UIPhaseProfile::TAB_INFO:
						{
							UITabProfileInfo* tab = (UITabProfileInfo*)p->GetTab( UIPhaseProfile::TAB_INFO);

							INT32 _idx = i3Math::Rand()%5;
							switch( _idx)
							{
							case 0: tab->EnableEditNickName( i3Math::Rand(), i3Math::Rand()%2 == 1); break;
							case 1: tab->EditNickName( i3Math::Rand(), i3Math::Rand()); break;
							case 2: tab->UpdateScroll(); break;
							case 3: tab->UpdateWheel( i3Math::Rand()); break;
							case 4: tab->PlayBtnClick( i3Math::Rand()); break;
							}
						}
						break;

					}
				}
				break;
				
			case 10: if( i3Math::Rand()%2) { Close();} break;
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void BuyItem::Process()
		{
			UIPopupBuyItem* p = static_cast<UIPopupBuyItem*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
				if( p != nullptr)
					p->clicked( i3Math::Rand()%CART_MAX);
				break;

			case 10: if( i3Math::Rand()%2) { Close();} break;
			}
		}




//-----------------------------------------------------------------------------------------------------------------------------------
		void ShopExtension::Open()
		{
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), m_pack);
		}
		
		void ShopExtension::Process()
		{
			UIPopupShopExtension* p = static_cast<UIPopupShopExtension*>( GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
				if( p != nullptr)
					p->clicked( i3Math::Rand()%EXT_MAX);
				break;

			case 10: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void UserProFile::Open()
		{
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), "Unit_Test");
		}

		void UserProFile::Process()
		{
			UIPopupUserProfile* p = static_cast<UIPopupUserProfile*>( GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:
					{
						INT32 cnt = 0;
						for( INT32 i = 0; i < 3; ++i) 
							if( p->m_BtnState[i] != UIP_BTN_STATE_NONE) cnt++;
						
						if( cnt > 0)
							p->ClickButton( i3Math::Rand()%cnt); 
					}
					break; //..
				case 2:case 3: p->ClickWhisper(); break;
				case 4:case 5: p->ClickDetailInfo(); break;
				case 6:case 7: p->OnCloseButton(); break;
				case 8:case 9: p->ChangeHost(); break;
				case 10:case 11: p->ClickUserInfo(); break;
				}
			}
		}


		
		
//-----------------------------------------------------------------------------------------------------------------------------------
		void UserAction::Open()
		{
			UIPhaseReadyRoom* ready_room = (UIPhaseReadyRoom*)g_pFramework->GetUIMainframe()->GetPhase( UIPHASE_READY_ROOM);
			INT32 slot = i3Math::Rand()%SLOT_MAX_COUNT;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), ready_room, &slot);
		}
	





//-----------------------------------------------------------------------------------------------------------------------------------
		void InviteLobbyUser::Process()
		{
			UIPopupInviteLobbyUser* p = static_cast<UIPopupInviteLobbyUser*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:case 4:case 5:
				if( p != nullptr)
					p->SetCheckUser( i3Math::Rand());
				break;

			case 6:case 7:case 8:case 9:
				if( p != nullptr)
					p->CheckAllUser( i3Math::Rand()%2 ? true : false);
				break;
			
			case 10: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void QueryPassword::Open()
		{
			QPParam Param;
			Param.wstrMsg			= GAME_RCSTRING("STBL_IDX_EP_LOBBY_JOIN_ROOM_Q");
			Param.LimitCount	= NET_ROOM_PW;
			Param.OnlyNumber	= true;
			Param.ShowPassword	= true;
			Param.Callback		= _JoinRoomPasswordCallback;
			Param.pUserContext	= this;

			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &Param);
		}
		
		void QueryPassword::Process()
		{
			UIPopupQueryPassword* p = static_cast<UIPopupQueryPassword*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));
			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:case 4:case 5:
				if( p != nullptr)
					p->OnOKButton();
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void ClanCreate::Process()
		{
			UIPopupCreateClan* p = static_cast<UIPopupCreateClan*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:case 2:case 9:		p->CheckClanName();		break;
				case 3:case 4:case 5:case 10:		p->CheckClanURL();		break;
				case 6:case 7:case 8:case 11:		p->CreateClan();		break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}


//-----------------------------------------------------------------------------------------------------------------------------------
		void ClanNameCheck::Process()
		{
			UIPopupClanNameSearch* p = static_cast<UIPopupClanNameSearch*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:case 2:		p->__OverLappingCheck();	break;
				case 3:case 4:case 5:		p->__OverLappingOK();		break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void ClanJoin::Open()
		{
			INT32 idx = g_pFramework->GetClanContext()->GetJoinClanIdx();
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &idx);
		}

		void ClanJoin::Process()
		{
			UIPopupJoinClan* p = static_cast<UIPopupJoinClan*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:case 2: case 3:case 4:
					p->JoinSend();
					break;

				case 5: p->Close(); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void RankUp::Process()
		{
			UIPopupRankup* p = static_cast<UIPopupRankup*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:case 2: case 3:case 4: 
					__noop;
					break;

				case 5: p->Confirm(); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void PcCafe::Open()
		{
			USER_INFO_BASIC InfoBasic;
			UserInfoContext::i()->GetMyUserInfoBasic( &InfoBasic);

			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &InfoBasic);
		}

		void PcCafe::Process()
		{
			//UIPopupPCCafe* p = static_cast<UIPopupPCCafe*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:
				__noop;
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}




//-----------------------------------------------------------------------------------------------------------------------------------
		void AddBlock::Open()
		{
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), "UnitTest");
		}

		void AddBlock::Process()
		{
			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:
				{
					UIPopupAddBlockList* p = static_cast<UIPopupAddBlockList*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));
					if( p != nullptr)
						p->OnOKButton();
				}
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void QueryItem::Open()
		{

			INT32 _style = i3Math::Rand()%(IQS_REMOVE+1);
			if( _style == IQS_USE )
			{
				INT32 _item_id = i3Math::Rand();
				g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_style);
			}
			else
			{
				ITEM_INFO item_info;
				item_info.m_TItemID = i3Math::Rand();
				item_info.m_TItemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID( item_info.m_TItemID);
				g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &item_info, &_style);
			}
			
		}

		void QueryItem::Process()
		{
			UIPopupQueryItem* p = static_cast<UIPopupQueryItem*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:
				if( p != nullptr)
					p->clicked( i3Math::Rand());
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}






//-----------------------------------------------------------------------------------------------------------------------------------
		void ResultUseItem::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void ResultUseItem::Process()
		{
			//UIPopupResultUseItem* p = static_cast<UIPopupResultUseItem*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3: __noop; break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}




//-----------------------------------------------------------------------------------------------------------------------------------
		void SkillOpen::Open()
		{

			INT32 _style = i3Math::Rand()%(IQS_REMOVE+1);
			if( _style == IQS_USE )
			{
				INT32 _item_id = i3Math::Rand();
				g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_style);
			}
			else
			{
				ITEM_INFO item_info;
				item_info.m_TItemID = i3Math::Rand();
				item_info.m_TItemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID( item_info.m_TItemID);
				g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &item_info, &_style);
			}

		}

		void SkillOpen::Process()
		{
			UIPopupSkillOpen* p = static_cast<UIPopupSkillOpen*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3:
				if( p != nullptr)
					p->clicked( i3Math::Rand());
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}






//-----------------------------------------------------------------------------------------------------------------------------------
		void SkillOpenResult::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void SkillOpenResult::Process()
		{
			//UIPopupResultUseItem* p = static_cast<UIPopupResultUseItem*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:case 2:case 3: __noop; break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void CheckNick::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void CheckNick::Process()
		{
			UIPopupUseItemChangeNick* p = static_cast<UIPopupUseItemChangeNick*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:case 2:case 3: p->DuplicateChangeName(); break;
				case 4:case 5: p->ResetChangeNameCheck(); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void ChangeClanName::Open()
		{
			INT32 _item_id = i3Math::Rand();
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id);
		}

		void ChangeClanName::Process()
		{
#if legacy_gui_scene
			UIPopup_Clan_ChangeName* p = static_cast<UIPopup_Clan_ChangeName*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->ChangeName_Cancel(); break;
				case 2:case 3: p->ChangeName_Confirm(); break;
				case 4:case 5: p->ChangeName_Duplicate(); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
#endif
		}


		
		
//-----------------------------------------------------------------------------------------------------------------------------------
		void ChangeColorNick::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void ChangeColorNick::Process()
		{
			//UIPopupUseItemChangeColorNick* p = static_cast<UIPopupUseItemChangeColorNick*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 6: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void ChangeCrossHairColor::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void ChangeCrossHairColor::Process()
		{
			//UIPopupUseItemChangeCrosshairColor* p = static_cast<UIPopupUseItemChangeCrosshairColor*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 6: if( i3Math::Rand()%2) { Close();} break;
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void DisguiseRank::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void DisguiseRank::Process()
		{
			//UIPopupUseItemDisguiseRank* p = static_cast<UIPopupUseItemDisguiseRank*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 6: if( i3Math::Rand()%2) { Close();} break;
			}
		}







//-----------------------------------------------------------------------------------------------------------------------------------
		void ChangeClanMark::Open()
		{
			INT32 _item_id = i3Math::Rand();
			INT32 _type = i3Math::Rand()%USE_ITEM_POPUP_MAX;
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), &_item_id, &_type);
		}

		void ChangeClanMark::Process()
		{
			UIPopupUseItemChangeClanMark* p = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->ChangePrevBGImage(); break;
				case 2:case 3: p->ChangeNextBGImage(); break;
				case 4:case 5: p->ChangePrevShapeImage(); break;
				case 6:case 7: p->ChangeNextShapeImage(); break;
				case 8:case 9: p->ChangeBackImageColor( i3Math::Rand()); break;
				case 10:case 11: p->ChangeShapeColor( i3Math::Rand()); break;
				case 12:case 13: p->CheckDuplicateMark(); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}







//-----------------------------------------------------------------------------------------------------------------------------------
		void CouponInput::Process()
		{
			UIPopupCouponInput* p = static_cast<UIPopupCouponInput*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 0:case 1:
				if( p != nullptr)
					p->Confirm();
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}






//-----------------------------------------------------------------------------------------------------------------------------------
		void DiscountCoupon::Process()
		{
			UIPopupDiscountCoupon* p = static_cast<UIPopupDiscountCoupon*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->OnClickButton(); break;
				case 2:case 3: p->ChangeCurrnetSlot( i3Math::Rand()); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void Event::Process()
		{
			//UIPopupEvent* p = static_cast<UIPopupEvent*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 4: if( i3Math::Rand()%2) { Close();} break;
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void Exit::Process()
		{
			//UIPopupExit* p = static_cast<UIPopupExit*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 4: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void TS::Process()
		{
			UIPopupTSReward* p = static_cast<UIPopupTSReward*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->SelectItem( i3Math::Rand()); break;
				case 2:case 3: p->Confirm(); break;
				default: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}







//-----------------------------------------------------------------------------------------------------------------------------------
		void VisitEvent::Process()
		{
			UIPopupVisitEvent* p = static_cast<UIPopupVisitEvent*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 3:
				if( p != nullptr)
					p->ReceiveItem();
				break;
			default: if( i3Math::Rand()%2) { Close();} break;
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void AttendanceCheck::Process()
		{
			UIPopupAttendanceCheck* p = static_cast<UIPopupAttendanceCheck*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->CheckStamp(); break;
				case 2:case 3: p->UpdateItemView( i3Math::Rand()); break;
				case 4: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}




//-----------------------------------------------------------------------------------------------------------------------------------
		void AttendanceComplete::Process()
		{
			UIPopupAttendanceComplete* p = static_cast<UIPopupAttendanceComplete*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 3: p->ReceiveItem(); break;
				case 4: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void ClanHonor::Process()
		{
			//UIPopupClanHonor* p = static_cast<UIPopupClanHonor*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 4: if( i3Math::Rand()%2) { Close();} break;
			}
		}






//-----------------------------------------------------------------------------------------------------------------------------------
		void ClanNonMember::Process()
		{
			UIPhaseNonClanMember* p = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->JoinClan(); break;
				case 2:case 3: p->CreateClan(); break;
				case 4:case 5: p->SearchClan(); break;
				case 6:case 7: p->ClickClanSlot( i3Math::Rand()); break;
				case 8:case 9: p->VisitClan( i3Math::Rand()); break;
				case 10:case 11: p->ClanListScroll(); break;
				case 12:case 13: p->ClanListWheel( i3Math::Rand()); break;
				case 14: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}




//-----------------------------------------------------------------------------------------------------------------------------------
		void ClanMember::Process()
		{
			UIPhaseClanMember* p = static_cast<UIPhaseClanMember*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: p->ChangeTab( UITabClanType(i3Math::Rand()%UTC_MAX)); break;
				case 2:case 3: p->ClickButton(); break;
				case 4:case 5: p->OnBackPhase(); break;
				case 6: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void MedalReward::Process()
		{
			//UIPopupMedalReward* p = static_cast<UIPopupMedalReward*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 6: if( i3Math::Rand()%2) { Close();} break;
			}
		}




//-----------------------------------------------------------------------------------------------------------------------------------
		void BreakIntoGame::Open()
		{
			UIPhaseReadyRoom* ready_room = (UIPhaseReadyRoom*)g_pFramework->GetUIMainframe()->GetPhase( UIPHASE_READY_ROOM);
			g_pFramework->GetUIMainframe()->OpenPopup( UIPOPUPWIN(m_type), ready_room);
		}

		void BreakIntoGame::Process()
		{
			UIPopupBreakIntoGame* p = static_cast<UIPopupBreakIntoGame*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1:case 2:case 3:case 4:case 5: p->clicked( i3Math::Rand()); break;
				case 6: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}






//-----------------------------------------------------------------------------------------------------------------------------------
		void WebBox::Process()
		{
			UIPopupWebBox* p = static_cast<UIPopupWebBox*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0: p->OnQueryJScript(); break;
				case 6: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void DomiScore::Process()
		{
			//UIPopupDomiScore* p = static_cast<UIPopupDomiScore*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 6: if( i3Math::Rand()%2) { Close();} break;
			}
		}





//-----------------------------------------------------------------------------------------------------------------------------------
		void DomiRound::Process()
		{
			UIPopupDomiRound* p = static_cast<UIPopupDomiRound*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			if( p != nullptr)
			{
				INT32 idx = i3Math::Rand()%EVENT_COUNT;
				switch( idx)
				{
				case 0:case 1: case 2: p->ChangeRound(); break;
				case 3:case 4: case 5: p->Confirm(); break;
				case 6: if( i3Math::Rand()%2) { Close();} break;
				}
			}
		}



//-----------------------------------------------------------------------------------------------------------------------------------
		void DomiEnter::Process()
		{
			//UIPopupDomiEnter* p = static_cast<UIPopupDomiEnter*>(GameUI::GetPopup( UIPOPUPWIN(m_type)));

			INT32 idx = i3Math::Rand()%EVENT_COUNT;
			switch( idx)
			{
			case 6: if( i3Math::Rand()%2) { Close();} break;
			}
		}
	};
};

#endif //end of ENABLE_UNIT_TEST