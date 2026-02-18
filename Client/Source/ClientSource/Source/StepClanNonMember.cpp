#include "pch.h"
#include "GlobalVariables.h"
#include "StepClanNonMember.h"
#include "ClanDef.h"


I3_CLASS_INSTANCE(CStepClanNonMember, CStepBase);

CStepClanNonMember::CStepClanNonMember(void)
{
	INT32 i;
	m_pClanContext = g_pFramework->GetClanContext();

	m_nClanListStartIndex	= 0;
	m_nClanListPageIndex	= 0;

	m_nUserListStartIndex	= 0;
	m_nUserListPageIndex	= 0;

	m_pClanNonMemberRoot	= NULL;

	m_pClanSubcribeWin		= NULL;	//	미가입자/클랜정보 창
	m_pClanNonMemberCaption	= NULL;	//	'클랜 찾기'

	m_pClanSerchBar			= NULL; // 클랜찾기 프로그래스바
	m_pClanSerchGage		= NULL;	// 클랜찾기 프로그래스바 게이지
	m_pClanSerchText		= NULL;	// 클랜찾기 프로그래스바 텍스트

	m_pClanSubscription		= NULL;	// 클랜 가입
	m_pClanCreate			= NULL;	// 클랜 생성


	m_pClanMark				= NULL;	// 클랜마크
	m_pClanMarkFrame		= NULL;	// 클랜마크 프레임

	m_pClanNameStatic		= NULL;	// 클랜명 :
	m_pClanNameValue		= NULL;	// 클랜명

	m_pClanStaffStatic		= NULL;	// 인원수
	m_pClanStaffValue		= NULL;

	m_pClanMasterStatic		= NULL;	// 클랜 마스터 :
	m_pClanMasterValue		= NULL;	// 클랜장
	m_pClanMasterRankImage	= NULL;	// 클랜 마스터의 등급 이미지 표시


	m_pClanRankStatic		= NULL;	// 클랜 순위 :
	m_pClanRankValue		= NULL;	// 100등
	m_pClanRankImage		= NULL;	// 클랜의 등급 이미지 표시

	m_pClanExpStatic		= NULL;	// 클랜 경험치
	m_pClanExpValue			= NULL;	//
	
	m_pClanOrderStatic		= NULL;	// 클랜 순위
	m_pClanOrderValue		= NULL;			

	m_pClanExpBarFrame		= NULL;	// 클랜 경험치 바 프레임
	m_pClanExpBar			= NULL;

	m_pClanAddressStatic	= NULL;	// 클랜 아지트 주소 URL
	m_pClanAddressValue		= NULL;	// 클랜 아지트 주소 URL

	m_pClanIntroduceFrame	= NULL;	// 클랜 소개 바탕
	m_pClanIntroduceValue	= NULL;	// 클랜 소개 글

	m_pClanSearchBullet		= NULL;
	m_pClanSearchStatic		= NULL;
	m_pClanSearchEdit		= NULL;
	m_pClanSearchButton		= NULL;

	//클랜
	m_pClanListWin			= NULL;	//	클랜정보 창
	m_pClanListCaption		= NULL;	//	'클랜'

	m_pClanListTab			= NULL;	// 클랜 리스트 버튼
	m_pUserListTab			= NULL;	// 유저 리스트 버튼

	m_pClanListPhase		= NULL;	// 클랜리스트 페이지
	m_pUserListPhase		= NULL;	// 유저 리스트 페이지

	m_pClanListScrollList	= NULL;	// 클랜 리스트 스크롤 
	m_pUserListScrollList	= NULL;	// 유저 리스트 스크롤

	m_pFocusedSlot			= NULL;
	m_pSelect				= NULL;

	for( i = 0; i < CLANFIND_LIST_ROWMAX; ++i)	//클랜 리스트내 각 슬롯의 정보
	{
		m_ClanInfo[i]._ClanEntryDay		= NULL;
		m_ClanInfo[i]._ClanMarkImage	= NULL;
		m_ClanInfo[i]._ClanMember		= NULL;
		m_ClanInfo[i]._ClanName			= NULL;
		m_ClanInfo[i]._pRank			= NULL;
		m_ClanInfo[i]._slot				= NULL;
	}

	//클랜 리스트 헤더
	m_pClanListHeadRank		= NULL;	// 클랜 리스트 헤더 : 클랜등급
	m_pClanListHeadClanName	= NULL;	// 클랜 리스트 헤더 : 클랜 이륾
	m_pClanListHeadEntryDay	= NULL;	// 클랜 리스트 헤더 : 클랜 개설일
	m_pClanListHeadMember	= NULL;	// 클랜 리스트 헤더 : 클랜 멤버수

	//유저 리스트 헤더
	m_pUserListHeadTitle	= NULL;		// 유저 리스트 헤더 : 타이틀

	// 나가기
	m_pLeaveButton			= NULL;

	m_nCurFocusSlotIdx				= -1;
	m_nCurSelectedSlotIdx			= -1;
	m_nCurDisplayingClanDetailInfo	= -1;
	m_bNeedUpdateClanList			= FALSE;

	i3mem::FillZero( &m_SelectedClanInfo, sizeof(SORT_CLAN_INFO));

	m_bChangedClanSlot = FALSE;
}

CStepClanNonMember::~CStepClanNonMember(void)
{
}

void CStepClanNonMember::OnPreGuiNotify(void)
{
	m_pFocusedSlot->SetEnable(FALSE);
}

void CStepClanNonMember::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	//	클랜 소트 헤더 초기화
	{
		m_pClanListHeadRank->SetTextColor( GetColor(GCT_DEFAULT));
		m_pClanListHeadClanName->SetTextColor( GetColor(GCT_DEFAULT));
		m_pClanListHeadEntryDay->SetTextColor( GetColor(GCT_DEFAULT));
		m_pClanListHeadMember->SetTextColor( GetColor(GCT_DEFAULT)); 
	}

	switch(pNotify->m_nID)
	{
	case GCI_CLAN_B_NONMEMBER_CREATE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_OnClanCreate();
		break;
	case GCI_CLAN_B_NONMEMBER_SUBSCRIPTION:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_OnClanSubscription();
		break;
	case GCI_CLAN_B_NONMEMBER_CLANLIST_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_OnLeave();
		break;
	case GCI_CLAN_L_CLANLIST_INFO_SLOT1:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT2:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT3:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT4:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT5:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT6:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT7:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT8:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT9:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT10:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT11:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT12:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT13:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT14:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT15:
	case GCI_CLAN_L_CLANLIST_INFO_SLOT16:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) _OnClanListOnMouse( pNotify->m_nID - GCI_CLAN_L_CLANLIST_INFO_SLOT1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) _OnClanListOnClicked( pNotify->m_nID - GCI_CLAN_L_CLANLIST_INFO_SLOT1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) _OnClanListDoubleClick( pNotify->m_nID - GCI_CLAN_L_CLANLIST_INFO_SLOT1);
		break;
	case GCI_CLAN_E_NONMEMBER_HEAD_RANK:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanListByRank();
		break;
	case GCI_CLAN_E_NONMEMBER_HEAD_NAME:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanListByName();
		break;
	case GCI_CLAN_E_NONMEMBER_HEAD_DAY:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanListByDay();
		break;
	case GCI_CLAN_E_NONMEMBER_HEAD_MEMBER:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) OnSortClanListByMember();
		break;
	case GCI_CLAN_L_NONMEMBER_CLANLIST_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) _OnClanListScroll();
		break;
	case GCI_CLAN_B_NONMEMBER_USERLIST_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OnUserListTab();
		break;
	case GCI_CLAN_L_NONMEMBER_USERLIST_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) _OnUserListScroll();
		break;
	case GCI_CLAN_B_NONMEMBER_CLANLIST_SEARCH:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSearchClanList();
		break;
	case GCI_CLAN_B_NONMEMBER_VIEW_ALL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnViewAll();
		break;
	}
}

void CStepClanNonMember::_OnClanListScroll(void)
{
	m_nClanListStartIndex = m_pClanListScrollList->GetCurrentTop();

	INT32 hilightslot = m_nCurSelectedSlotIdx - m_nClanListStartIndex;

	if( hilightslot < 0 || hilightslot >= CLANFIND_LIST_ROWMAX)
	{
		m_pSelect->SetEnable( FALSE);
	}
	else
	{
		m_pSelect->setPositionY( hilightslot * 27.0f + 1.0f);
		m_pSelect->SetEnable( TRUE);
	}

	m_bNeedUpdateClanList = TRUE;
}

void CStepClanNonMember::_OnUserListScroll(void)
{
	m_nUserListStartIndex = m_pUserListScrollList->GetCurrentTop();

	INT32 hilightslot = m_nCurSelectedSlotIdx - m_nUserListStartIndex;

	if( hilightslot < 0 || hilightslot >= CLANFIND_LIST_ROWMAX)
	{
		m_pSelect->SetEnable( FALSE);
	}
	else
	{
		m_pSelect->setPositionY( hilightslot * 27.0f + 1.0f);
		m_pSelect->SetEnable( TRUE);
	}
}

void CStepClanNonMember::_OnClanListOnMouse( INT32 idx)
{
	I3ASSERT( idx >= 0 && idx < CLANFIND_LIST_ROWMAX);

	m_nCurFocusSlotIdx = idx;

	m_pFocusedSlot->setPositionY( idx * 27.0f + 1.0f);
	m_pFocusedSlot->SetEnable( TRUE);
}

void CStepClanNonMember::_OnClanListOnClicked( INT32 idx)
{
	INT32 slotid = idx + m_nClanListStartIndex + CLANFIND_LIST_ROWMAX * m_nClanListPageIndex;
	// 클릭한걸 또 클릭해도 또 보냄.. 이 부분 같을경우 리턴코드 필요	-- 영권작업 --
	//if( m_nCurSelectedSlotIdx == slotid)
	//	return;

	{// 클랜리스트 밖의 목록을 선택하였는지의 여부체크
		if( slotid >= m_pClanContext->GetClanListCount())
			return;
	}

	PlaySound(GUI_SND_ITEM_SELECTING);

#if defined TEST_CLAN_LIST
	STRING_POPUP("TEST_CLAN_LIST");
#else
	{
		I3_BOUNDCHK(slotid, m_pClanContext->GetClanListCount());
		SORT_CLAN_INFO * pSortClanInfo = m_pClanContext->GetClanList(slotid);

		UINT8 cType = 1;
		g_pGameContext->SetEvent( EVENT_CLAN_DETAIL_INFO_N, &pSortClanInfo->_info._idx, &cType );
	}

	_SetClanListSelectedHilight( slotid);
#endif
}

void CStepClanNonMember::_SetClanListSelectedHilight( INT32 idx)
{
	if( idx < 0)
	{
		i3mem::FillZero( &m_SelectedClanInfo, sizeof(SORT_CLAN_INFO));
	}
	else
	{
		SORT_CLAN_INFO * pSelected = m_pClanContext->GetClanList(idx);
		I3ASSERT( pSelected != NULL);

		i3mem::Copy( &m_SelectedClanInfo, pSelected, sizeof(SORT_CLAN_INFO));		
	}

	if( idx == -1)
	{
		m_pClanIntroduceValue->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanIntroduceValue->SetTextColor( GetColor( GCT_DEFAULT));
	}
	else if( m_nCurSelectedSlotIdx == -1 && idx >= 0)
	{
		m_pClanIntroduceValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pClanIntroduceValue->SetTextColor( GetColor( GCT_VALUE));
	}

	m_nCurSelectedSlotIdx	= idx;

	INT32 hilightslot = idx - m_nClanListStartIndex - CLANFIND_LIST_ROWMAX * m_nClanListPageIndex;

	if( hilightslot < 0 || hilightslot >= CLANFIND_LIST_ROWMAX)
	{
		m_pSelect->SetEnable( FALSE);
	}
	else
	{
		m_pSelect->setPositionY( hilightslot * 27.0f + 1.0f);
		m_pSelect->SetEnable( TRUE);
	}
}

void CStepClanNonMember::_OnClanListDoubleClick( INT32 idx)
{
}

void CStepClanNonMember::_OnUserListTab(void)
{
	STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));		// 차후 구현 예정입니다
	return;
}

void CStepClanNonMember::_OnClanCreate()
{
	if ( ! g_pFramework->GetClanContext()->HasClanCreationInfo() )
	{
		g_pGameContext->SetEvent(EVENT_CLAN_CREATION_INFO);
		return;
	}

	((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_CREATE);
}

void CStepClanNonMember::_OnClanSubscription()
{
	if( m_nCurSelectedSlotIdx == -1)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_CHOOSE_CLAN"));		// 가입하려는 클랜을 선택해주세요
		return;
	}

	//	클랜에 가입하기 전에 가입조건에 맞는지 먼저 서버에 요청합니다.
	INT32 idx = g_pGameContext->getClanDetailInfo()->_idx;
	m_pClanContext->SendRequestJoining( idx);

}

void CStepClanNonMember::_OnLeave()
{
	OnExitKey();
}

BOOL CStepClanNonMember::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	INT32 i;
	char strName[MAX_STRING_COUNT] = "";

	LinkControlEx(plist, "ClanNonMemberWin", m_pClanNonMemberRoot, -1);

	//	클랜 찾기 윈도우
	{
		LinkControlEx(plist,  "clanSubscriberWin", m_pClanSubcribeWin, -1);
		LinkControlEx(plist,  "clanNonMemberCaption", m_pClanNonMemberCaption, -1);
		LinkControlEx(plist,  "clanNonMemberSearchText", m_pClanNonMembeSerchText, -1);		

		LinkControlEx(plist,  "clanNonSerchText", m_pClanSerchText, -1);		
		LinkControlEx(plist,  "clanNonSerchGage", m_pClanSerchGage, -1);
		LinkControlEx(plist,  "clanNonSerchBar", m_pClanSerchBar, -1);

		{
			LinkControlEx(plist,  "clanNonMemberClanSubscription", m_pClanSubscription, GCI_CLAN_B_NONMEMBER_SUBSCRIPTION);	// 클랜 가입
			LinkControlEx(plist,  "clanNonMemberClanCreate", m_pClanCreate, GCI_CLAN_B_NONMEMBER_CREATE);						// 클랜 생성

			//
			LinkControlEx(plist,  "clanNonMemberClanMark", m_pClanMark, -1);						// 클랜마크
			LinkControlEx(plist,  "clanNonMemberClanMarkFrame", m_pClanMarkFrame, -1);			// 클랜마크 프레임
			
			LinkControlEx(plist,  "clanNonMemberClanNameStatic", m_pClanNameStatic, -1);			// 클랜명 :
			LinkControlEx(plist,  "clanNonMemberClanNameValue", m_pClanNameValue, -1);				// 클랜명

			LinkControlEx(plist,  "clanNonMemberClanStaffStatic", m_pClanStaffStatic, -1);			// 인원수
			LinkControlEx(plist,  "clanNonMemberClanStaffValue", m_pClanStaffValue, -1);

			LinkControlEx(plist,  "clanNonMemberClanMasterStatic", m_pClanMasterStatic, -1);		// 클랜 마스터 :
			LinkControlEx(plist,  "clanNonMemberClanMasterValue", m_pClanMasterValue, -1);			// 클랜장
			LinkControlEx(plist,  "clanNonMemberClanMasterRankImage", m_pClanMasterRankImage, -1);// 클랜 마스터의 등급 이미지 표시


			LinkControlEx(plist,  "clanNonMemberClanRankStatic", m_pClanRankStatic, -1);			// 클랜 순위 :
			LinkControlEx(plist,  "clanNonMemberClanRankValue", m_pClanRankValue, -1);				// 100등
			LinkControlEx(plist,  "clanNonMemberClanRankImage", m_pClanRankImage, -1);			// 클랜의 등급 이미지 표시

			LinkControlEx(plist,  "clanNonMemberClanExpStatic", m_pClanExpStatic, -1);				// 클랜 경험치
			LinkControlEx(plist,  "clanNonMemberClanExpValue", m_pClanExpValue, -1);				//

			LinkControlEx(plist,  "clanNonMemberClanOrderStatic", m_pClanOrderStatic, -1);			// 클랜 순위
			LinkControlEx(plist,  "clanNonMemberClanOrderValue", m_pClanOrderValue, -1);			

			LinkControlEx(plist,  "clanNonMemberClanExpFrame", m_pClanExpBarFrame, -1);				// 클랜 경험치 바 프레임
			LinkControlEx(plist,  "clanNonMemberClanExpBar", m_pClanExpBar, -1);

			LinkControlEx(plist,  "clanNonMemberAddressStatic", m_pClanAddressStatic, -1);			// 클랜 아지트
			LinkControlEx(plist,  "clanNonMemberAddressValue", m_pClanAddressValue, -1);			// 클랜 아지트 주소 URL
			
			LinkControlEx(plist,  "clanNonMemberClanIntroduceFrame", m_pClanIntroduceFrame, -1);	// 클랜 소개 바탕
			LinkControlEx(plist,  "clanNonMemberClanIntroduceValue", m_pClanIntroduceValue, -1);	// 클랜 소개 글

			//	클랜 찾기
			LinkControlEx(plist,  "clanNonMemberSearchBullet", m_pClanSearchBullet, -1);
			LinkControlEx(plist,  "clanNonMemberSearchStatic", m_pClanSearchStatic, -1);		//	'클랜명 검색'
			LinkControlEx(plist,  "clanNonMemberSearchEdit", m_pClanSearchEdit, -1);
			LinkControlEx(plist,  "clanNonMemberSearchButton", m_pClanSearchButton, GCI_CLAN_B_NONMEMBER_CLANLIST_SEARCH);

			//	클랜 채팅 (임시)
			{
				LinkControlEx(plist,  "clanNonMemberChatPopup", m_pChatComboPopup, -1);
				m_pChatComboPopup->SetEnable(FALSE);	//	임시

				/*LinkControlEx(plist,  "clanNonMemberChatCombo", m_pChatCombo, -1);
				LinkControlEx(plist,  "clanNonMemberChatButton", m_pChatComboButton, -1);*/
				
				for(INT32 i=0; i<4; i++)
				{
					i3GuiButton * pButton = NULL;

					sprintf_s(strName, "clanNonMemberChatTab%d", i + 1);
					LinkControlEx(plist, strName, pButton, -1);

					pButton->SetEnable(FALSE);
				}
			}
		}
	}

	// 클랜 리스트 윈도우
	{
		LinkControlEx(plist,  "clanClanListWin", m_pClanListWin, -1);			//	클랜정보 창
		LinkControlEx(plist,  "clanClanListCaption", m_pClanListCaption, -1);			//	'클랜'

		{
			LinkControlEx(plist,  "clanClanListTab", m_pClanListTab, GCI_CLAN_B_NONMEMBER_CLANLIST_TAB);				// 클랜 리스트 버튼
			LinkControlEx(plist,  "clanUserListTab", m_pUserListTab, GCI_CLAN_B_NONMEMBER_USERLIST_TAB);				// 유저 리스트 버튼
			LinkControlEx(plist,  "clanClanListViewAll", m_pViewAllButton, GCI_CLAN_B_NONMEMBER_VIEW_ALL);			//	전체보기

			LinkControlEx(plist,  "clanClanListPhase", m_pClanListPhase, -1);			// 클랜리스트 페이지
			LinkControlEx(plist,  "clanUserListPhase", m_pUserListPhase, -1);			// 유저 리스트 페이지

			LinkControlEx(plist, "clanClanListScrollList", m_pClanListScrollList, GCI_CLAN_L_NONMEMBER_CLANLIST_SCROLL);// 클랜 리스트 스크롤 
			{
				for( i = 0; i < CLANFIND_LIST_ROWMAX; ++i)
				{
					sprintf_s(strName, "clanClanListSlot%d", i + 1);
					LinkControlEx(plist,  strName, m_ClanInfo[i]._slot, GCI_CLAN_L_CLANLIST_INFO_SLOT1 + i);			// 유저 리스트 페이지

					sprintf_s(strName, "clanClanListRank%d", i +1);
					LinkControlEx(plist,  strName, m_ClanInfo[i]._pRank, -1);

					sprintf_s(strName, "clanClanListClanMarkImage%d", i +1);
					LinkControlEx(plist,  strName, m_ClanInfo[i]._ClanMarkImage, -1);

					sprintf_s(strName, "clanClanLisClanName%d", i +1);
					LinkControlEx(plist,  strName, m_ClanInfo[i]._ClanName, -1);

					sprintf_s(strName, "clanClanListEntryDay%d", i +1);
					LinkControlEx(plist,  strName, m_ClanInfo[i]._ClanEntryDay, -1);

					sprintf_s(strName, "clanClanListEntryMember%d", i +1);
					LinkControlEx(plist,  strName, m_ClanInfo[i]._ClanMember, -1);
				}
			}

			LinkControlEx(plist, "clanUserListScrollList", m_pUserListScrollList, GCI_CLAN_L_NONMEMBER_USERLIST_SCROLL);// 클랜 리스트 스크롤 

			//클랜 리스트 헤더
			LinkControlEx(plist,  "clanClanListHeadRank", 			m_pClanListHeadRank, GCI_CLAN_E_NONMEMBER_HEAD_RANK);		// 클랜 리스트 헤더 : 클랜 등급
			LinkControlEx(plist,  "clanClanListHeadClanName", 		m_pClanListHeadClanName, GCI_CLAN_E_NONMEMBER_HEAD_NAME);	// 클랜 리스트 헤더 : 클랜 이름
			LinkControlEx(plist,  "clanClanListHeadEntryDay", 		m_pClanListHeadEntryDay, GCI_CLAN_E_NONMEMBER_HEAD_DAY);	// 클랜 리스트 헤더 : 클랜 개설일
			LinkControlEx(plist,  "clanClanListHeadEntryMember", 	m_pClanListHeadMember, GCI_CLAN_E_NONMEMBER_HEAD_MEMBER);	// 클랜 리스트 헤더 : 클랜 멤버수

			//유저 리스트 헤더
			LinkControlEx(plist,  "clanUserListHeadTitle", 	m_pUserListHeadTitle, -1);		// 유저 리스트 헤더 : 타이틀
		}
	}

	// 나가기 버튼
	LinkControlEx(plist,  "clanClanListLeaveButton", m_pLeaveButton,GCI_CLAN_B_NONMEMBER_CLANLIST_LEAVE);

	//
	LinkControlEx(plist,  "clanListfocus", m_pFocusedSlot, -1);
	LinkControlEx(plist,  "clanListSelect", m_pSelect, -1);

	return FALSE;
}

void CStepClanNonMember::OnInitControl(void)
{
	m_pClanNonMemberRoot->SetEnable(FALSE);

	_InitFindClanPage();
	_InitClanPage();

	CStepBase::OnInitControl();
}

void CStepClanNonMember::EnterServer(void)
{
	m_nClanListStartIndex = 0;
	m_nUserListStartIndex = 0;
	m_pClanListScrollList->SetCurValue(0.0f);
	m_pUserListScrollList->SetCurValue(0.0f);

	m_pClanContext->EnterClanList();

	if( !m_pClanContext->IsEnter())
	{
		m_pClanContext->EnterServer();		
	}
} 

void CStepClanNonMember::LeaveServer(void)
{
	if( m_pClanContext->IsEnter())
	{
		m_pClanContext->LeaveServer();		
	}
}


void CStepClanNonMember::OnGameEvent( INT32 event,INT32 arg )
{
	switch(event)
	{
	case EVENT_CLAN_CREATION_INFO:
		((CStageReady*)m_pParent)->TogglePopup( POPUP_CLAN_CREATE);
		break;
	}
}


#define LIST_UPDATE_TIME	2.0f 
void CStepClanNonMember::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	if( m_pClanListPhase != NULL && m_pClanListPhase->isEnable())
	{
		//	클랜 리스트를 업데이트한다.
		BOOL ret = m_pClanContext->UpdateClanList(rDeltaSeconds);
		if( ret)
		{
			// 스크롤바 재설정
			_RebuildClanListScrollBar();

			//	클랜 리스트가 변경됬으므로 선택된 슬롯을 다시 처리한다.
			_SearchHilightClanList();

			m_bNeedUpdateClanList = TRUE;
		}

		//	GUI 업데이트
		_UpdateClanList( rDeltaSeconds);

		_UpdateClanDetailedInfo();

		// 클랜찾기 프로그래스바 업데이트
		_UpdateClanSerchRate(rDeltaSeconds);
	}

	CStepBase::OnUpdate(rDeltaSeconds);
}

BOOL CStepClanNonMember::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager * pMng = (i3InputDeviceManager*) pObj;
			i3InputKeyboard * pKeyboard = pMng->GetKeyboard();

			//	클랜 검색일때
			if( m_pClanSearchEdit->getIsFocus())
			{
				if(pKeyboard->GetStrokeState(I3I_KEY_ENTER))
				{
					OnSearchClanList();
					return TRUE;
				}
			}
		}
	}

	return CStepBase::OnEvent(event, pObj, param2, code);
}

void CStepClanNonMember::OnStart(void)
{
	CStepBase::OnStart();

	SetPushedStateButton(MBT_CLAN, TRUE);

	i3mem::FillZero( &m_SelectedClanInfo, sizeof(SORT_CLAN_INFO));
}

void CStepClanNonMember::OnEnd(void)
{
	CStepBase::OnEnd();
	
	SetPushedStateButton(MBT_CLAN, FALSE);
}

void CStepClanNonMember::OnInitRun(void)
{
	CStepBase::OnInitRun(); 

	{
		m_nClanListStartIndex	= 0;
		m_nClanListPageIndex	= 0;

		m_nUserListStartIndex	= 0;
		m_nUserListPageIndex	= 0;

		m_nCurFocusSlotIdx				= -1;
		m_nCurSelectedSlotIdx			= -1;
		m_nCurDisplayingClanDetailInfo	= -1;
		m_bNeedUpdateClanList			= TRUE;

		_SetClanListSelectedHilight( -1);	

		m_pClanSearchEdit->SetIMEText("");
		m_pClanSearchEdit->SetFocus(TRUE);

		DEACTIVATE_BUTTON(m_pViewAllButton);
	}

	MoveCamera(CL_RECORD); 
}

BOOL CStepClanNonMember::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CStepBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;
	}

	// 시간에 따른 기준위치 지정
	m_pClanNonMemberRoot->setPositionY(CStepBase::GetSlideY());


	// 퇴장 시, 비활성화. 그외 활성화
	BOOL enableWindow = TRUE;

	if (SS_OUT == CStepBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pClanNonMemberRoot->SetEnable(enableWindow);

	return TRUE;
}

void CStepClanNonMember::OnExitKey(void)
{
	((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);
}

void CStepClanNonMember::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			if( m_pClanListPhase->isEnable())
			{
				m_pClanListScrollList->MovePrev();
			}
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			if( m_pClanListPhase->isEnable())
			{
				m_pClanListScrollList->MoveNext();
			}
		}			
	}	

	if( m_pClanListPhase->isEnable())
	{
		_OnClanListScroll();

		/*
		if( m_nClanListStartIndex >= CLANFIND_LIST_ROWMAX)
		{
			CLAN_CONTEXT * pClanCtx = g_pGameContext->getClanContext();
			if( pClanCtx->_totalCount > m_nClanListStartIndex + CLANFIND_LIST_ROWMAX * (m_nClanListPageIndex + 1))
			{
				m_nClanListStartIndex = m_nClanListStartIndex - CLANFIND_LIST_ROWMAX;
				m_nClanListPageIndex++;

				m_pClanListScrollList->SetCurValue( 0.0f);
			}
		}
		else if( m_nClanListStartIndex == 0)
		{
			if( m_nClanListPageIndex > 0)
			{
				m_nClanListStartIndex = m_nClanListStartIndex + CLANFIND_LIST_ROWMAX;
				m_nClanListPageIndex--;

				m_pClanListScrollList->SetCurValue( m_pClanListScrollList->getMaxValue());
			}
		}
		*/
		
		_SetClanListSelectedHilight( m_nCurSelectedSlotIdx);	

		m_bNeedUpdateClanList = TRUE;
	}

	if( m_pUserListPhase->isEnable())
	{
		_OnUserListScroll();
	}
}

void CStepClanNonMember::_RebuildClanListScrollBar(void)
{
	INT32 nNewCount = m_pClanContext->GetClanListCount();

	// 새로 생성될 클랜의 갯수가 기존보다 많으면 이상태에서 추가만
	if( m_pClanListScrollList->GetElementCount() < nNewCount)
	{
		INT32 nAddCount = nNewCount - m_pClanListScrollList->GetElementCount();

		for( INT32 i=0; i<nAddCount; ++i)
		{
			m_pClanListScrollList->AddElement( "");
		}
	}
	// 새로 생성될 클랜의 갯수가 기존보다 작으면 그냥 리스트를 초기화후 다시 넣는다.
	else if( m_pClanListScrollList->GetElementCount() > nNewCount) 
	{
		m_pClanListScrollList->Clear();
		for( INT32 i=0; i<nNewCount; ++i)
		{
			m_pClanListScrollList->AddElement( "");
		}
	}
}

void CStepClanNonMember::OnSortClanListByRank(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_RANK_DESC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_RANK_ASC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_RANK_DESC);
	}

	m_bNeedUpdateClanList = TRUE;

	//	하일라이트 변경
	_SearchHilightClanList();

	//	스크롤바 크기 새로 설정
	_RebuildClanListScrollBar();

	m_pClanListHeadRank->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanNonMember::OnSortClanListByName(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_NAME_ASC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_NAME_DESC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_NAME_ASC);
	}	

	m_bNeedUpdateClanList = TRUE;

	//	하일라이트 변경
	_SearchHilightClanList();

	//	스크롤바 크기 새로 설정
	_RebuildClanListScrollBar();

	m_pClanListHeadClanName->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanNonMember::OnSortClanListByDay(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_DATE_DESC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_DATE_ASC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_DATE_DESC);
	}

	m_bNeedUpdateClanList = TRUE;

	//	하일라이트 변경
	_SearchHilightClanList();

	//	스크롤바 크기 새로 설정
	_RebuildClanListScrollBar();

	m_pClanListHeadEntryDay->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanNonMember::OnSortClanListByMember(void)
{
	if( m_pClanContext->GetClanSortType() == CLAN_SORT_MEMBER_DESC)
	{
		m_pClanContext->SortClanList(CLAN_SORT_MEMBER_ASC);
	}
	else	//	default
	{
		m_pClanContext->SortClanList(CLAN_SORT_MEMBER_DESC);
	}

	m_bNeedUpdateClanList = TRUE;

	//	하일라이트 변경
	_SearchHilightClanList();

	//	스크롤바 크기 새로 설정
	_RebuildClanListScrollBar();

	m_pClanListHeadMember->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanNonMember::OnSearchClanList(void)
{
	const char * pszKeyword = m_pClanSearchEdit->getIMETextBuf();

	_SearchClanListView(pszKeyword);
}

void CStepClanNonMember::OnViewAll(void)
{
	_SearchClanListView("");
}

void CStepClanNonMember::_SearchClanListView(const char * pszKeyword)
{
	if( !m_pClanContext->SearchClanListView( pszKeyword))
	{
		return;
	}

	//	공백일 경우 다시 모든 클랜을 보여준다.
	if( i3String::Length(pszKeyword) == 0)
	{
		DEACTIVATE_BUTTON(m_pViewAllButton);
	}
	//	원하는 키워드로 클랜을 찾아 보여준다.
	else if( pszKeyword != NULL && i3String::Length(pszKeyword) > 0)
	{
		ACTIVATE_BUTTON(m_pViewAllButton);
	}

	m_bNeedUpdateClanList = TRUE;
	
	//	스크롤바 크기 새로 설정
	_RebuildClanListScrollBar();

	m_nClanListStartIndex = 0;
	m_pClanListScrollList->SetCurValue(0.0f);
}

void CStepClanNonMember::SetInputDisable(BOOL bDisable)
{
	m_pClanNonMemberRoot->setInputDisable(bDisable);
}

BOOL CStepClanNonMember::GetInputDisable(void)
{
	return m_pClanNonMemberRoot->getInputDisable();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	클랜 찾기 페이지 - 초기화
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CStepClanNonMember::_InitFindClanPage( void)
{
	//클랜 찾기
	INIT_NORMAL_CAPTION(m_pClanNonMemberCaption, GAME_STRING("STBL_IDX_CLAN_FINDING"));	// 클랜 찾기

	INIT_CAPTION_BUTTON( m_pClanSubscription, GAME_STRING("STBL_IDX_CLAN_JOINING"));	// 클랜 가입
	INIT_CAPTION_BUTTON( m_pClanCreate, GAME_STRING("STBL_IDX_CLAN_CREATING"));		// 클랜 생성

	INIT_STATIC_CAPTION( m_pClanNameStatic, GAME_STRING("STBL_IDX_CLAN_NAME"));		// "클랜명 :"
	INIT_STATIC_CAPTION( m_pClanStaffStatic, GAME_STRING("STBL_IDX_CLAN_PERSON"));	// "클랜 인원수 :"
	INIT_STATIC_CAPTION( m_pClanMasterStatic, GAME_STRING("STBL_IDX_CLAN_MASTER"));	// "클랜 마스터 :"
	INIT_STATIC_CAPTION( m_pClanRankStatic, GAME_STRING("STBL_IDX_CLAN_RANK"));		// "클랜 등급 :"
	INIT_STATIC_CAPTION( m_pClanExpStatic, GAME_STRING("STBL_IDX_CLAN_EXP"));		// "클랜 경험치 :
	INIT_STATIC_CAPTION( m_pClanOrderStatic, GAME_STRING("STBL_IDX_CLAN_ORDER"));	// "클랜 순위 :"
	INIT_STATIC_CAPTION( m_pClanAddressStatic, GAME_STRING("STBL_IDX_CLAN_AZIT"));	// "클랜 아지트 :"
	
	INIT_STATIC_CAPTION( m_pClanSearchStatic, GAME_STRING("STBL_IDX_CLAN_SEARCHING"));	//	"클랜명 검색"
	INIT_BUTTON_EX( m_pClanSearchButton);

	//	클랜명 검색 입력
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pClanSearchEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pClanSearchEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanSearchEdit->SetTextColor(GetColor(GCT_VALUE));
	m_pClanSearchEdit->SetText("");
	m_pClanSearchEdit->SetIMEMode( IME_MODE_NATIVE);
	
	INIT_VALUE_CAPTION( m_pClanNameValue, "");					// 클랜명
	INIT_VALUE_CAPTION( m_pClanStaffValue, "");
	INIT_VALUE_CAPTION( m_pClanMasterValue, "");				// 클랜장
	INIT_VALUE_CAPTION( m_pClanRankValue, "");					// 100등
	INIT_VALUE_CAPTION( m_pClanExpValue, "");					//
	INIT_VALUE_CAPTION( m_pClanOrderValue, "");			
	INIT_VALUE_CAPTION( m_pClanAddressValue, "");				//	클랜 아지트 주소		
	INIT_VALUE_CAPTION( m_pClanIntroduceValue, "");				// 클랜 소개 글

	m_pClanAddressValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

	I3COLOR color;
	i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
	m_pClanAddressValue->SetTextColor(&color);

#ifndef CLAN_ADDRESS_ENABLE
	{
		m_pClanAddressStatic->SetEnable( FALSE);
		m_pClanAddressValue->SetEnable( FALSE);
	}
#endif

	m_pClanIntroduceValue->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanIntroduceValue->SetTextColor( GetColor( GCT_DEFAULT));

	INIT_BUTTON_EX( m_pClanSubscription);
	INIT_BUTTON_EX( m_pClanCreate);

	SetClanOrder( 0, 0);
	SetClanRank( 0, 0, 0);
	SetClanMasterName( "", 0);
	SetClanStaff( 0, 0);
	SetClanName( "", 0);
	SetClanExp(0, 0);
	SetClanAddress("");
}

void CStepClanNonMember::_InitClanPage( void)
{
	//클랜
	INIT_WIN_CAPTION(m_pClanListCaption, GAME_STRING("STBL_IDX_CLAN_CLAN"));	// "클랜"

	//클랜 리스트 헤더
	INIT_NORMAL_CAPTION( m_pClanListHeadRank, GAME_STRING("STBL_IDX_CLAN_HEADER_RANK"));		// 클랜 리스트 헤더 : "등급"
	INIT_NORMAL_CAPTION( m_pClanListHeadClanName, GAME_STRING("STBL_IDX_CLAN_HEADER_NAME"));	// 클랜 리스트 헤더 : "클랜명"
	INIT_NORMAL_CAPTION( m_pClanListHeadEntryDay, GAME_STRING("STBL_IDX_CLAN_HEADER_ENTRY"));	// 클랜 리스트 헤더 : "개설일"
	INIT_NORMAL_CAPTION( m_pClanListHeadMember, GAME_STRING("STBL_IDX_CLAN_HEADER_PERSON"));	// 클랜 리스트 헤더 : "가입 인원"

	m_pClanListHeadRank->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanListHeadClanName->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanListHeadEntryDay->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanListHeadMember->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanListHeadMember->SetTextSpace( 0, -2);

	//유저 리스트 헤더
	INIT_VALUE_CAPTION( m_pUserListHeadTitle, GAME_STRING("STBL_IDX_CLAN_HEADER_USER_LIST")) ;		// 유저 리스트 헤더 : "접속 유저 리스트"

	INIT_TAB_EX(m_pClanListTab);
	INIT_TAB_EX(m_pUserListTab);
	INIT_CAPTION_BUTTON(m_pViewAllButton, GAME_STRING("STBL_IDX_CLAN_VIEW_ALL"));	//  "전체보기"

	m_pClanListTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	INIT_BUTTON_EX( m_pLeaveButton);

	_InitClanListTab();
	_InitUserListTab();

	m_pClanListPhase->SetEnable( TRUE);
	m_pUserListPhase->SetEnable( FALSE);

	m_pFocusedSlot->SetEnable( FALSE);
	m_pFocusedSlot->setInputDisable( TRUE);
	m_pSelect->SetEnable( FALSE);
	m_pSelect->setInputDisable( TRUE);

	m_pClanSerchGage->SetAllShapeEnable(FALSE);
	m_pClanSerchText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanSerchText->SetText("");	

	i3mem::FillZero( &m_SelectedClanInfo, sizeof(SORT_CLAN_INFO));
}

void CStepClanNonMember::_InitClanListTab( void)
{
	m_pClanListScrollList->CreateTextEx(16,  GetDefaultFontName());
	m_pClanListScrollList->SetSelColor(0, 0, 0, 0);

	for(UINT32 i = 0; i < CLANFIND_LIST_ROWMAX; i++)
	{
		m_ClanInfo[i]._ClanMarkImage->SetAllShapeEnable( FALSE);
		m_ClanInfo[i]._ClanMarkImage->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
		m_ClanInfo[i]._ClanMarkImage->setInputDisable( TRUE);

		m_ClanInfo[i]._pRank->SetAllShapeEnable( FALSE);
		m_ClanInfo[i]._pRank->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
		m_ClanInfo[i]._pRank->setInputDisable( TRUE);

		m_ClanInfo[i]._ClanEntryDay->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_ClanInfo[i]._ClanEntryDay->SetTextSpace( 0, -2);
		m_ClanInfo[i]._ClanEntryDay->SetTextColor(GetColor(GCT_VALUE));
		m_ClanInfo[i]._ClanEntryDay->setInputDisable( TRUE);

		m_ClanInfo[i]._ClanMember->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_ClanInfo[i]._ClanMember->SetTextSpace( 0, -2);
		m_ClanInfo[i]._ClanMember->SetTextColor(GetColor(GCT_VALUE));
		m_ClanInfo[i]._ClanMember->setInputDisable( TRUE);

		m_ClanInfo[i]._ClanName->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_ClanInfo[i]._ClanName->SetTextSpace( 0, -2);
		m_ClanInfo[i]._ClanName->SetTextColor(GetColor(GCT_VALUE));
		m_ClanInfo[i]._ClanName->setInputDisable( TRUE);
	}
}

void CStepClanNonMember::_InitUserListTab( void)
{
	m_pUserListScrollList->CreateTextEx(16,  GetDefaultFontName());
	m_pUserListScrollList->SetSelColor(0, 0, 0, 0);

	m_pUserListScrollList->Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// 클랜 찾기 페이지 - 설정 함수들 - protected

///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
// 클랜 찾기 페이지 - 설정 함수들 - public
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CStepClanNonMember::SetClanName(const char * pszString, UINT8 cColor )
{
	SetNameUsingColorTable( m_pClanNameValue, pszString, cColor );
}

void CStepClanNonMember::SetClanStaff(INT32 nNum, INT32 nTotal)
{
	//	클랜 인원수
	::SetClanStaff(m_pClanStaffValue, nNum, nTotal);
}

void CStepClanNonMember::SetClanMasterName(const char * pszString, INT32 nRank)
{
	//	클랜 마스터
	::SetClanMaster(m_pClanMasterValue, m_pClanMasterRankImage, pszString, (UINT8)nRank);
}

void CStepClanNonMember::SetClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank)
{
	//	클랜 등급
	::SetClanRank(m_pClanRankValue, m_pClanRankImage, nClanExp, nClanStaffNum, nClanRank);
}

void CStepClanNonMember::SetClanOrder(INT32 nCurValue, INT32 nPrevValue)
{
	//	클랜 순위
	::SetClanOrder(m_pClanOrderValue, nCurValue, nPrevValue);
}

void CStepClanNonMember::SetClanExp(UINT32 nCurValue, UINT32 nMaxValue)
{
	REAL32 rx = ((CStageReady*)m_pParent)->GetGuiRoot()->getGuiResolutionWidth();

	//	클랜 경험치
	SetClanExpText(m_pClanExpValue, nCurValue, nMaxValue);
	SetClanExpGauge(m_pClanExpBar, rx, nCurValue, nMaxValue);
}

void CStepClanNonMember::SetClanAddress(const char * pszString)
{
#ifdef CLAN_ADDRESS_ENABLE
	{
		m_pClanAddressStatic->SetEnable( TRUE);
		m_pClanAddressValue->SetEnable( TRUE);

		//	클랜 주소
		m_pClanAddressValue->SetText( pszString);
	}
#else
	{
		m_pClanAddressStatic->SetEnable( FALSE);
		m_pClanAddressValue->SetEnable( FALSE);
	}
#endif
}

void CStepClanNonMember::SetClanMark(UINT32 nFlag)
{
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(m_pClanMark, CLAN_MARK_MEDIUM, nFlag);
#endif
}

void CStepClanNonMember::_UpdateClanDetailedInfo()
{
	char temp[256] = "";

	if( m_nCurSelectedSlotIdx == -1)
	{
		m_nCurDisplayingClanDetailInfo = -1;

		m_pClanOrderValue->SetText(GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));

		SetClanRank( 0, 0, 0);
		SetClanMasterName( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"), 0);	// "알수없음"
		SetClanStaff( 0, 0);
		SetClanName( GAME_STRING("STBL_IDX_CLAN_UNDECIDED"), 0);	// "선택되지 않음"
		SetClanExp(0, 0);
		SetClanAddress( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "알수없음"
		SetClanMark(0);

		m_pClanIntroduceValue->SetText( GAME_STRING("STBL_IDX_CLAN_CHOOSE_CLAN_IN_LIST"));		// "클랜 리스트에서 클랜을 선택해주세요"
	}
	else
	{
		CLAN_DETAIL_INFO * pInfo = g_pGameContext->getClanDetailInfo();

		m_nCurDisplayingClanDetailInfo = pInfo->_idx;

		//SetClanOrder( pInfo->_clanRank, pInfo->_prevRank);
		SetClanOrder( 0, 0);
		SetClanRank( pInfo->_exp, pInfo->_currentPerson, pInfo->_expRank);
		SetClanMasterName( pInfo->_masterNick, pInfo->_masterRank);
		SetClanStaff( pInfo->_currentPerson, pInfo->_maxPerson);
		SetClanName( pInfo->_name, pInfo->_color );
		UINT32 wExp = pInfo->_exp;
		if( wExp > DF_CLAN_RANK_L5 ) wExp = DF_CLAN_RANK_L5;
		UINT32 BeforeExp = GetClanBeforeRankExp( wExp );
		SetClanExp(wExp - BeforeExp, GetClanNextRankExp(wExp) - BeforeExp);
		SetClanMark(pInfo->_mark);

		i3String::Copy(temp, g_pConfigEx->GetClan().AzitAddressBase, 256 );
		i3String::Concat(temp, pInfo->_azitURL);
		SetClanAddress(temp);

		m_pClanIntroduceValue->SetText( pInfo->_intro);
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CStepClanNonMember::_UpdateClanList( REAL32 rDeltaSeconds)
{
	if( m_bNeedUpdateClanList == FALSE)
		return;

	INT32 i = 0;
	char szString[MAX_PATH] = "";

	for(i = 0; i < CLANFIND_LIST_ROWMAX; i++)
	{
		m_ClanInfo[i]._ClanMarkImage->SetAllShapeEnable( FALSE);
		m_ClanInfo[i]._ClanMarkImage->SetEnable( FALSE);

		m_ClanInfo[i]._pRank->SetAllShapeEnable( FALSE);
		m_ClanInfo[i]._pRank->SetEnable( FALSE);

		m_ClanInfo[i]._ClanEntryDay->SetEnable( FALSE);
		m_ClanInfo[i]._ClanMember->SetEnable( FALSE);
		m_ClanInfo[i]._ClanName->SetEnable( FALSE);
	}
	
	if(m_pClanContext->GetClanListCount() == 0)
		return;

	{
		for(i = 0; i < CLANFIND_LIST_ROWMAX; i++)
		{
			INT32 idx = i + m_nClanListStartIndex + CLANFIND_LIST_ROWMAX * m_nClanListPageIndex;
			if( idx >= m_pClanContext->GetClanListCount())
				continue;

			SORT_CLAN_INFO * pSortClanInfo = m_pClanContext->GetClanList(idx);
			CLAN_BASIC_INFO * pInfo = &pSortClanInfo->_info;

			if( pInfo == NULL)
				continue;

			if( pInfo->_idx == 0)
				continue;

			//클랜 마크 이미지
			{
				m_ClanInfo[i]._ClanMarkImage->SetShapeEnable( GUI_SHAPE_OFFSET, TRUE);
				m_ClanInfo[i]._ClanMarkImage->SetEnable( TRUE);

#if defined( USE_CLAN_MARK)
				::SetClanMarkTexEx(m_ClanInfo[i]._ClanMarkImage, CLAN_MARK_SMALL, pInfo->_mark);
#endif
			}

			//클랜 등급
			{
				SetClanRankImage(m_ClanInfo[i]._pRank, pInfo->_rank);
			}

			//클랜 개설일
			{
				INT32 nYear = (INT32)(pInfo->_date / 10000);
				INT32 nMonth = (INT32)((pInfo->_date - nYear * 10000) / 100);
				INT32 nDay = (INT32)(pInfo->_date - nYear * 10000 - nMonth * 100);

				sprintf_s(szString, GAME_STRING("STBL_IDX_CLAN_DECLARE_DATE"), 
									nYear, nMonth, nDay);		// "%04d년 %02d월 %02d일"

				m_ClanInfo[i]._ClanEntryDay->SetText( szString);
				m_ClanInfo[i]._ClanEntryDay->SetEnable( TRUE);
			}

			//클랜 멤버수
			{
#if defined(LOCALE_RUSSIA)
				sprintf_s( szString, "%d", pInfo->_person);	// "%d" ->러시아의 경우 "명" 이라는 글자가 너무 김. 그래서 생략함.
#else
				sprintf_s( szString, GAME_STRING("STBL_IDX_CLAN_DECLARE_PERSON"), pInfo->_person);	// "%d 명"
#endif

				m_ClanInfo[i]._ClanMember->SetText( szString);
				m_ClanInfo[i]._ClanMember->SetEnable( TRUE);
			}

			//클랜 이름
			{
				sprintf_s( szString, "%s", pInfo->_name);

				SetNameUsingColorTable( m_ClanInfo[i]._ClanName, pInfo->_name, pInfo->_color );

				m_ClanInfo[i]._ClanName->SetEnable( TRUE);
			}
		}
	}

	m_bNeedUpdateClanList = FALSE;
}

void CStepClanNonMember::_UpdateClanSerchRate( REAL32 rDeltaSeconds)
{
	char szRate[MAX_PATH];
	INT32 nDotNumber = 0;
	INT32 nRate = m_pClanContext->GetClanSerchRate();
	INT32 nTotal = g_pGameContext->getClanContext()->_totalCount;
	static REAL32 fDeltaSec = 0;

	if( nRate >= 100)		nDotNumber = 20;
	else if( nRate <= 0)	nDotNumber = 0;
	else nDotNumber = nRate / 5;

	m_pClanSerchGage->SetAllShapeEnable(FALSE);
	
	for(INT32 i = 0; i < nDotNumber; i++)
	{
		m_pClanSerchGage->SetShapeEnable(GUI_SHAPE_OFFSET + i, TRUE);
	}

	if( nRate == 100)
	{
		char szTemp[256] = "";
		// 검색완료
		sprintf_s( szTemp, "%s   %d / %d", GAME_STRING("STBL_IDX_CLAN_SERCH_COMPLETE"), m_pClanContext->GetClanListCount(), nTotal);

		// 완료.
		m_pClanNonMembeSerchText->SetEnable(FALSE);
		m_pClanSerchText->SetText( szTemp);	// 검색완료
	}
	else
	{
		fDeltaSec += rDeltaSeconds;

		if( fDeltaSec > 1.0f)
		{
			if(m_pClanNonMembeSerchText->isEnable()) m_pClanNonMembeSerchText->SetEnable( FALSE);
			else m_pClanNonMembeSerchText->SetEnable( TRUE);

			fDeltaSec = 0.0f;
		}

		// 아직 진행중.
		m_pClanNonMembeSerchText->SetText( GAME_STRING("STBL_IDX_CLAN_WAIT_SERCH")); // "클랜리스트를 검색중입니다. 잠시만 기다려 주십시오."

		sprintf_s( szRate, GAME_STRING("STBL_IDX_CLAN_TRY_SERCH"), nRate); // "클랜리스트 검색 중(%d%%)"
		
		char szTemp[256] = "";
		sprintf_s( szTemp, "   %d / %d", m_pClanContext->GetClanListCount(), nTotal);
		i3String::Concat( szRate, szTemp);

		m_pClanSerchText->SetText(szRate);	
	}
}

BOOL CStepClanNonMember::_SearchHilightClanList(void)
{
	BOOL bFind = FALSE;

	//	하일라이트 클랜 정보를 찾아 매칭한다.
	if( m_SelectedClanInfo._info._idx > 0)
	{
		for(INT32 i=0; i<m_pClanContext->GetClanListCount(); i++)
		{
			SORT_CLAN_INFO * pInfo = (SORT_CLAN_INFO*) m_pClanContext->GetClanList(i);
			
			if( pInfo->_info._idx == m_SelectedClanInfo._info._idx)
			{
				_SetClanListSelectedHilight(i);

				bFind = TRUE;
				break;
			}
		}
	}

	if( !bFind)
	{	//	예전 목록에서 없어 졌다.
		_SetClanListSelectedHilight(-1);
	}

	return bFind;
}
