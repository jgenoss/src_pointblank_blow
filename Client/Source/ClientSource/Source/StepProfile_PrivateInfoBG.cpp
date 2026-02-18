#include "pch.h"
#include "StepProfile_PrivateInfoBG.h"
#include "GuiNotifyReceiver.h"
#include "CommunityContext.h"
#include "TitleInfo.h"

I3_CLASS_INSTANCE( CStepProfile_PrivateInfoBG, i3ElementBase);

CStepProfile_PrivateInfoBG::CStepProfile_PrivateInfoBG()
{
	INT32 i;

	m_pNotifyBase = NULL;

	m_pWindowBG	= NULL;

	m_pCaption_Text = NULL;
	m_pUserNick = NULL;
	m_pUserRank = NULL;
	m_pUserClanMark = NULL;
	m_pStatic_Designation_Text = NULL;

	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		m_pDesignation_Text[i] = NULL;
		m_pDesignation_Image[i] = NULL;
		m_pDesignation_Image_Lock[i] = NULL;
	}

	m_pPopup_TargetList	= NULL;
	m_pCombo_Target		= NULL;
	m_pButton_Target	= NULL;

	m_pPopup_WhoList	= NULL;
	m_pStatic_WhoBG		= NULL;
	m_pCombo_Who		= NULL;
	m_pButton_WhoList	= NULL;

#if defined( ENABLE_VIEW_USER)
	for( i = 0; i < STEPPROFILE_USERLIST_VIEW_COUNT; i++)
	{
		m_User[ i]._pUserName = NULL;
		m_User[ i]._pButton	= NULL;
	}
#endif

	m_nCurrentTargetType	= _USER_LIST_TYPE_NONE;

	m_rWheelDelta		= 0.f;
	m_bIsMyInfoView	= true;
}

CStepProfile_PrivateInfoBG::~CStepProfile_PrivateInfoBG()
{
}

BOOL CStepProfile_PrivateInfoBG::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	INT32 i;
	char strName[ MAX_STRING_COUNT];

	m_pNotifyBase = pBase;
#if defined( ENABLE_VIEW_USER)
	LinkControlEx(plist, 	"prPrivateInfoBG",					m_pWindowBG,					-1);
	LinkControlEx(plist, 	"prPrivateInfoBG_Text",				m_pCaption_Text,				-1);
	LinkControlEx(plist, 	"prPrivateInfo_NickName",			m_pUserNick,					-1);
	LinkControlEx(plist, 	"prPrivateInfo_Rank",				m_pUserRank,					-1);
	LinkControlEx(plist, 	"prPrivateInfo_ClanMark",			m_pUserClanMark,				-1);
	LinkControlEx(plist, 	"prPrivateInfo_DesignationKey",		m_pStatic_Designation_Text,		-1);
	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		sprintf_s( strName, "prPrivateInfo_Designation_Slot%d_Text", i + 1);
		LinkControlEx(plist,  strName, m_pDesignation_Text[i], -1);

		sprintf_s(strName, "prPrivateInfo_Designation_Slot%d_Image", i + 1);
		LinkControlEx(plist, strName, m_pDesignation_Image[i], -1);

		sprintf_s(strName, "prPrivateInfo_Designation_Slot%d_Lock", i + 1);
		LinkControlEx(plist, strName, m_pDesignation_Image_Lock[i], -1);
	}

	LinkControlEx(plist, 	"prPrivateInfo_Target_Popup",		m_pPopup_TargetList,		GCI_PRS_L_PROFILE_TARGET);
	m_pPopup_TargetList->CreateTextEx( 3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );

	LinkControlEx(plist, 	"prPrivateInfo_Target_Combo",		m_pCombo_Target,			-1);
	LinkControlEx(plist, 	"prPrivateInfo_Target_Button",		m_pButton_Target,			GCI_PRS_B_PROFILE_TARGET);
	INIT_COMBO_EX( m_pButton_Target);
	LinkControlEx(plist, 	"prPrivateInfo_Who_Bg1",			m_pStatic_WhoBG,			-1);
	LinkControlEx(plist, 	"prPrivateInfo_Who_List",			m_pPopup_WhoList,			GCI_PRS_L_PROFILE_USERLIST);
	LinkControlEx(plist, 	"prPrivateInfo_Who_Combo",			m_pCombo_Who,				-1);
	LinkControlEx(plist,  "prPrivateInfo_Who_Button",			m_pButton_WhoList,			GCI_PRS_B_PROFILE_USERLIST);
	INIT_COMBO_EX( m_pButton_WhoList);

	for( i = 0; i < STEPPROFILE_USERLIST_VIEW_COUNT; i++)
	{
		sprintf_s( strName, "prPrivateInfoBG_UserList_Button%d", i);
		LinkControlEx(plist,  strName, m_User[ i]._pButton, GCI_PRS_B_PROFILE_USER_SLOT_1 + i);
		INIT_COMBO_EX( m_User[ i]._pButton);
		
		m_User[ i]._pButton->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_User[ i]._pButton->SetCaptionEnable( TRUE);
		m_User[ i]._pButton->SetCaptionAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_User[ i]._pButton->SetCaptionColor( GetColor(GCT_VALUE));
		m_pPopup_WhoList->AddElement( "");
	}
#else
	LinkControlEx(plist,  "prDesigzationMyInfoBG",				m_pWindowBG,					-1);
	LinkControlEx(plist, 	"prDesigzationMyInfoBG_Text",				m_pCaption_Text,				-1);
	LinkControlEx(plist, 	"prDesigzationMyInfo_NickName",			m_pUserNick,					-1);
	LinkControlEx(plist, 	"prDesigzationMyInfo_Rank",				m_pUserRank,					-1);
	LinkControlEx(plist, 	"prDesigzationMyInfo_ClanMark",			m_pUserClanMark,				-1);
	LinkControlEx(plist, 	"prDesigzationMyInfo_DesignationKey",		m_pStatic_Designation_Text,		-1);
	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		sprintf_s( strName, "prDesigzationMyInfo_Slot%d_Text", i + 1);
		LinkControlEx(plist,  strName, m_pDesignation_Text[i], -1);

		sprintf_s(strName, "prDesigzationMyInfo_Slot%d_Image", i + 1);
		LinkControlEx(plist, strName, m_pDesignation_Image[i], -1);

		sprintf_s(strName, "prDesigzationMyInfo_Slot%d_Lock", i + 1);
		LinkControlEx(plist, strName, m_pDesignation_Image_Lock[i], -1);
	}

	LinkControlEx(plist, 	"prDesigzationMyInfo_Target_Popup",		m_pPopup_TargetList,		GCI_PRS_L_PROFILE_TARGET);
	m_pPopup_TargetList->CreateTextEx( 3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pPopup_TargetList->SetEnable( FALSE);

	LinkControlEx(plist, 	"prDesigzationMyInfo_Target_Combo",		m_pCombo_Target,			-1);
	LinkControlEx(plist, 	"prDesigzationMyInfo_Who_Bg1",			m_pStatic_WhoBG,			-1);
	m_pStatic_WhoBG->SetEnable( FALSE);
	LinkControlEx(plist, 	"prDesigzationMyInfo_Who_List",			m_pPopup_WhoList,			GCI_PRS_L_PROFILE_USERLIST);
	LinkControlEx(plist, 	"prDesigzationMyInfo_Who_Combo",		m_pCombo_Who,				-1);
#endif
	
	return TRUE;
}

void CStepProfile_PrivateInfoBG::Init( void)
{
	INT32 i;

	I3ASSERT( m_pNotifyBase != NULL);
	I3ASSERT( m_pCaption_Text != NULL);

	const char * pText = NULL;
#if defined( ENABLE_VIEW_USER)
	pText = GAME_STRING("STR_TBL_DESIGNATION_PRIVATE_INFO_TEXT");	/*개인정보*/
#else
	pText = GAME_STRING("STR_TBL_DESIGNATION_MYINFO_TEXT");		/*내정보*/
#endif
	INIT_WIN_CAPTION2(m_pCaption_Text, pText, GetColor(GCT_DEFAULT));

	m_pUserNick->SetTextColor( GetColor(GCT_DEFAULT));
	m_pUserNick->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pStatic_Designation_Text->SetTextColor( GetColor(GCT_DEFAULT));
	m_pStatic_Designation_Text->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStatic_Designation_Text->SetText( GAME_STRING("STR_TBL_DESIGNATION_TEXT"));	/*호칭*/

	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{	
		m_pDesignation_Text[i]->SetTextColor( GetColor(GCT_DEFAULT));
		m_pDesignation_Text[i]->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	}

#if defined( ENABLE_VIEW_USER)
	m_pPopup_TargetList->SetSelColor( 100, 100, 100, 100);
	m_pPopup_TargetList->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPopup_TargetList->SetExclusiveEnableControl( TRUE);
	m_pPopup_TargetList->Clear();
	m_pPopup_TargetList->AddElement( GAME_STRING("STR_TBL_DESIGNATION_MYINFO_TEXT"));		/*내정보*/
	m_pPopup_TargetList->AddElement( GAME_STRING("STR_TBL_DESIGNATION_CLAN_TEXT"));		/*클랜*/
	m_pPopup_TargetList->AddElement( GAME_STRING("STR_TBL_DESIGNATION_FRIEND_TEXT"));		/*친구*/
	m_pPopup_TargetList->SetSelectedElement( 0);
	m_pPopup_TargetList->SetEnable( FALSE);

	m_pCombo_Target->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCombo_Target->SetText( GAME_STRING("STR_TBL_DESIGNATION_MYINFO_TEXT"));			/*내정보*/

	m_pPopup_WhoList->CreateTextEx( 5,  GetDefaultFontName());
	m_pPopup_WhoList->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPopup_WhoList->SetSelColor( 0, 0, 0, 0);

	m_pCombo_Who->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCombo_Who->SetText( "-----");
	m_pStatic_WhoBG->SetEnable( FALSE);
#else 
	m_pCombo_Target->SetEnable(FALSE);
	m_pCombo_Who->SetEnable(FALSE);
#endif

	__UpdateDesignation( true);
	__UpdateUserInfo( true);

	m_bIsMyInfoView = true;

	m_rWheelDelta = 0.f;
}

void CStepProfile_PrivateInfoBG::Update( REAL32 tm)
{
#if defined( ENABLE_VIEW_USER)
	WheelUpdate();
#endif
}

void CStepProfile_PrivateInfoBG::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch( pNotify->m_nID )
	{
	case GCI_PRS_L_PROFILE_TARGET :
		if( pNotify->m_nEvent == I3GUI_POPUPLIST_NOTIFY_CLICKED)
		{	
			__OnSelectionTargetList( (_USER_LIST_TYPE) pNotify->m_nPriParam);
		}
		break;
	case GCI_PRS_B_PROFILE_TARGET :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( m_pPopup_TargetList->isEnable() )	m_pPopup_TargetList->SetEnable( FALSE);
			else
			{
				m_pNotifyBase->ExclusionPopup( m_pPopup_TargetList);
				m_pPopup_TargetList->SetEnable( TRUE);
			}
		}
		break;
	case GCI_PRS_L_PROFILE_USERLIST :		break;
	case GCI_PRS_B_PROFILE_USERLIST :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( m_pStatic_WhoBG->isEnable() )
			{
				m_pStatic_WhoBG->SetEnable( FALSE);
			}
			else
			{
				if( m_UserList.GetCount() > 0 )
					m_pStatic_WhoBG->SetEnable( TRUE);
			}
		}
		break;

	case GCI_PRS_B_PROFILE_USER_SLOT_1 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	OnUserSelect( pNotify->m_nID - GCI_PRS_B_PROFILE_USER_SLOT_1);		break;
	case GCI_PRS_B_PROFILE_USER_SLOT_2 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	OnUserSelect( pNotify->m_nID - GCI_PRS_B_PROFILE_USER_SLOT_1);		break;
	case GCI_PRS_B_PROFILE_USER_SLOT_3 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	OnUserSelect( pNotify->m_nID - GCI_PRS_B_PROFILE_USER_SLOT_1);		break;
	case GCI_PRS_B_PROFILE_USER_SLOT_4 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	OnUserSelect( pNotify->m_nID - GCI_PRS_B_PROFILE_USER_SLOT_1);		break;
	case GCI_PRS_B_PROFILE_USER_SLOT_5 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	OnUserSelect( pNotify->m_nID - GCI_PRS_B_PROFILE_USER_SLOT_1);		break;
	}
}

void CStepProfile_PrivateInfoBG::__AddUserList( _USER_LIST_TYPE nUserListType)
{
	UINT32 i;

	m_UserList.Clear();
	
	switch( nUserListType )
	{
	case _USER_LIST_TYPE_CLAN :
		for( i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
		{
			const CLAN_MEMBER_NODE * pClanMember = g_pCommunity->GetViewClanMember( i);
			//if( GET_FRIEND_STATE( pClanMember->_state ) == FRIEND_STATE_ONLINE)
				m_UserList.Add( (void*) pClanMember->_uid);
		}
		break;

	case _USER_LIST_TYPE_FRIEND :
		for( i = 0; i < g_pCommunity->GetFriendCount(); i++)
		{
			const FRIEND_NODE * pFriend = g_pCommunity->GetFriend( i);
			//if( GET_FRIEND_STATE( pFriend->_state ) == FRIEND_STATE_ONLINE)
				m_UserList.Add( (void*) pFriend->_uid);
		}
		break;
	}
}

void CStepProfile_PrivateInfoBG::__OnSelectionTargetList( _USER_LIST_TYPE nUserListType)
{
	m_nCurrentTargetType = nUserListType;

	m_pCombo_Who->SetText( "-----");

	__AddUserList( nUserListType );

	// 유저 선택에 대한 정보를 초기화 해서 보여줍니다.
	OnUserSelect( 0);

	switch( nUserListType)
	{
		case _USER_LIST_TYPE_NONE :	m_pCombo_Target->SetText( GAME_STRING("STR_TBL_DESIGNATION_MYINFO_TEXT"));	break;		/*내정보*/
		case _USER_LIST_TYPE_CLAN :	m_pCombo_Target->SetText( GAME_STRING("STR_TBL_DESIGNATION_CLAN_TEXT"));		break;		/*클랜*/
		case _USER_LIST_TYPE_FRIEND : m_pCombo_Target->SetText( GAME_STRING("STR_TBL_DESIGNATION_FRIEND_TEXT"));	break;		/*친구*/
	}

	__UpdateWhoListBox();
}

void CStepProfile_PrivateInfoBG::__UpdateUserInfo( bool bMy)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	
	
	if( bMy)	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	else		MyUserInfoBasic = *g_pGameContext->GetUserInfoBasic();

	// 닉네임
	SetNameUsingColorTable( m_pUserNick, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	// 계급
	GameGUI::SetRankStatic(m_pUserRank);
	m_pUserRank->SetShapeUVIndex(MyUserInfoBasic._rank);

	// 클랜 마크
#if defined( USE_CLAN_MARK)
	if( ::SetClanMarkTexEx( m_pUserClanMark, CLAN_MARK_SMALL, MyUserInfoBasic._clanMark) )
		m_pUserClanMark->SetEnable( TRUE);
	else
		m_pUserClanMark->SetEnable( FALSE);
#endif
}

void CStepProfile_PrivateInfoBG::__UpdateComboWhoBox( INT32 iUserIdx)
{
	if( m_UserList.GetCount() > 0)
	{
		UINT32 selectedIdx = (UINT32) m_UserList.GetItem( iUserIdx);

		switch( m_nCurrentTargetType )
		{
		case _USER_LIST_TYPE_CLAN :
			{
				const CLAN_MEMBER_NODE * pClanMember = g_pCommunity->FindClanMemberByUserID( selectedIdx);
				if( pClanMember != NULL)
				{
					m_pCombo_Who->SetText( (char*) pClanMember->_nick);
					break;
				}
			}
			break;

		case _USER_LIST_TYPE_FRIEND :
			{
				const FRIEND_NODE * pFriend = g_pCommunity->FindFriendByUserID( selectedIdx);
				if( pFriend != NULL)
				{
					m_pCombo_Who->SetText( (char*) pFriend->_nick);
					break;
				}
			}
			break;
		}
	}
}

void CStepProfile_PrivateInfoBG::__UpdateWhoListBox( void)
{
#if defined( ENABLE_VIEW_USER)
	INT32 i;
	INT32 iUserTopStart = m_pPopup_WhoList->GetCurrentTop();
	INT32 iCount = 0;

	for( i = iUserTopStart; i < m_UserList.GetCount(); i++)
	{
		UINT32 userId = (UINT32) m_UserList.GetItem( i);

		switch( m_nCurrentTargetType )
		{
		case _USER_LIST_TYPE_CLAN :
			{
				const CLAN_MEMBER_NODE * pClanMember = g_pCommunity->FindClanMemberByUserID( userId);
				if( pClanMember != NULL)
				{
					m_User[ iCount]._pButton->SetEnable( TRUE);
					m_User[ iCount]._pButton->SetCaption( (const char*) pClanMember->_nick);
					iCount++;
					if( iCount >= STEPPROFILE_USERLIST_VIEW_COUNT)
						return;
					break;
				}
			}
			break;

		case _USER_LIST_TYPE_FRIEND :
			{
				const FRIEND_NODE * pFriend = g_pCommunity->FindFriendByUserID( userId);
				if( pFriend != NULL)
				{
					m_User[ iCount]._pButton->SetEnable( TRUE);
					m_User[ iCount]._pButton->SetCaption( (const char*) pFriend->_nick);
					iCount++;
					if( iCount >= STEPPROFILE_USERLIST_VIEW_COUNT)
						return;
					break;
				}
			}
			break;
		}
	}

	for( i = iCount; i < STEPPROFILE_USERLIST_VIEW_COUNT; i++)
		m_User[ i]._pButton->SetEnable( FALSE);
#endif
}

void CStepProfile_PrivateInfoBG::OnUserSelect( INT32 iUserIdx)
{
	INT32 top = (INT32) m_pPopup_WhoList->GetCurrentTop();
	if( m_UserList.GetCount() > top + iUserIdx)
	{
		// 유저의 DB id를 보내줍니다.
		UINT32 iUserID = (UINT32) m_UserList.GetItem( top + iUserIdx);
		g_pGameContext->SetEvent(EVENT_GET_USERINFO, &iUserID);

		m_pPopup_WhoList->setSelectedElement( iUserIdx);

		__UpdateDesignation( false);
		__UpdateUserInfo( false);
		__UpdateComboWhoBox( iUserIdx);

		m_bIsMyInfoView = false;
	}
	else
	{
		__UpdateDesignation( true);
		__UpdateUserInfo( true);

		INT64 iUserID = g_pGameContext->GetMyUID();
		g_pGameContext->SetEvent(EVENT_GET_USERINFO, &iUserID);

		m_bIsMyInfoView = true;
	}

	m_pStatic_WhoBG->SetEnable( FALSE);
}

void CStepProfile_PrivateInfoBG::OnGameEvent( INT32 event, INT64 arg)
{
	
	switch( event)
	{
	//// 1.5 Check Designation
	//case EVENT_DESIGNATION_GET :		__UpdateDesignation( true);		break;
	//case EVENT_DESIGNATION_CHANGE:		__UpdateDesignation( true);		break;
	//case EVENT_DESIGNATION_RELEASE:		__UpdateDesignation( true);		break;
	case EVENT_GET_USERINFO :
		{
			if( arg == g_pGameContext->GetMyUID() )
			{
				__UpdateDesignation( true);
				__UpdateUserInfo( true);
			}
			else
			{
				__UpdateDesignation( false);
				__UpdateUserInfo( false);
			}
		}
		break;
	}
}

void CStepProfile_PrivateInfoBG::__UpdateDesignation( bool bMy)
{

	const CTitleInfo & MyTitleInfo = g_pGameContext->GetTitleInfo(bMy);
	INT32 iLimitTitleCount = MyTitleInfo.GetLimitCount();

	I3ASSERT( iLimitTitleCount <= MAX_EQUIP_USER_TITLE);

	for( INT32 i = 0; i < iLimitTitleCount; i++)
	{
		m_pDesignation_Image_Lock[i]->SetEnable(FALSE);
		m_pDesignation_Image[i]->SetAllShapeEnable( FALSE);
		m_pDesignation_Text[i]->SetText("");
	}

	for( INT32 i = iLimitTitleCount; i < MAX_EQUIP_USER_TITLE; i++)
	{
		m_pDesignation_Image_Lock[i]->SetEnable( TRUE);
		m_pDesignation_Image[i]->SetAllShapeEnable( FALSE);
		m_pDesignation_Text[i]->SetText("");
	}

	// 호칭이미지 및 호칭이름.
	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		UINT8 Title = MyTitleInfo.GetEquipTitle(i);
		if( m_pDesignation_Image_Lock[i]->isEnable())
			continue;
		if( (Title <= 0) || (Title > MAX_DEFAULT_USER_TITLE_COUNT))
			continue;

		for( INT32 j = 0; j < MAX_DEFAULT_USER_TITLE_COUNT; j++)
		{
			if( g_pDesignationValueSet->getDesignationSet( j)->getType() != Title)
				continue;

			char szName[ MAX_STRING_COUNT];
			m_pDesignation_Image[i]->SetShapeEnable( j, TRUE);
			g_pDesignationValueSet->getDesignationSet( j)->getName( szName);
			m_pDesignation_Text[i]->SetText( szName);
			break;

		}
	}
}

void CStepProfile_PrivateInfoBG::WheelUpdate( void)
{
	INT32 i;

	// 마우스 휠버튼 검사
	VEC3D mouseDelta;
	i3Vector::Copy(&mouseDelta, g_pFramework->getMouseMoveDelta());

	m_rWheelDelta += mouseDelta.z;

	INT32 scrollby = (INT32)(m_rWheelDelta / WHEEL_DELTA);
	
	// 업데이트 되지 않았으면 처리하지 않는다
	if (0 == scrollby)
	{
		return;
	}

	if (0 < scrollby)
	{
		for( i = 0; i < scrollby; i++)
		{
			if( m_pPopup_WhoList->isEnable())
				m_pPopup_WhoList->MovePrev();
		}
	}
	else
	{
		for( i = 0; i > scrollby; i--)
		{
			if( m_pPopup_WhoList->isEnable())
				m_pPopup_WhoList->MoveNext();
		}
	}

	__UpdateWhoListBox();

	// 휠 델타 초기화 - WHEEL_DELTA보다 작게 움직였다면, 커질때까지 누적해야한다
	if (WHEEL_DELTA <= m_rWheelDelta || -WHEEL_DELTA >= m_rWheelDelta)
	{
		m_rWheelDelta = 0.0f;
	}
}


