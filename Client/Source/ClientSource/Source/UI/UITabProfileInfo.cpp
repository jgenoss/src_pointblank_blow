#include "pch.h"
#include "UITabProfileInfo.h"
#include "UIMainFrame.h"
#include "UIPhaseProfile.h"
#include "UITopMenu.h"
#include "UIUtil.h"
#include "UIMath.h"
#include "UICharaInfoMgr.h"
#include "UIFloatCharacterView.h"
#include "UIPhaseShop.h"

#include "CommunityContext.h"
#include "ItemImageMgr.h"
#include "UIUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "MedalContext.h"
#include "../MedalContext.h"
#include "GameContextUtil.h"

#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/ftoa.h"

#include "ClientStageInfoUtil.h"

static float tickTime = 0.0f;

namespace
{
	UIClanMark::UIClanMark(void)
	{
		m_pClanMark = nullptr;
		m_pDefaultClanMark = nullptr;
	};

	UIClanMark::~UIClanMark(void)
	{
		m_pClanMark = nullptr;
		m_pDefaultClanMark = nullptr;
	};

	void UIClanMark::SetClanMarkCtrl( i3UIImageBoxEx * pClanMark, i3UIImageBox * pDefClanMark )
	{
		m_pClanMark = pClanMark;
		m_pDefaultClanMark = pDefClanMark;
	}

	void UIClanMark::SetClanMarkFlag( UINT32 ui32MarkFlag )
	{
		if( m_pClanMark != nullptr )
		{
			GameUI::SetClanTexture(m_pClanMark, CLAN_MARK_MEDIUM, ui32MarkFlag);
			m_pClanMark->SetShapeSize(45, 45);

			if( m_pDefaultClanMark != nullptr )
				m_pDefaultClanMark->EnableCtrl( !m_pClanMark->isEnable() );
		}
	}
};

UITabProfileInfo::CCharaSlot::CCharaSlot()
{
	Clear();
}

UITabProfileInfo::CCharaSlot::~CCharaSlot()
{

}

bool	UITabProfileInfo::CCharaSlot::Init( i3UIFrameWnd* wnd )
{
	m_wnd = wnd;
	I3ASSERT( nullptr != wnd );

	if( nullptr != m_wnd )
	{
		const char* name = m_wnd->GetName();

		char szImg[ MAX_NAME ];
		char szName[ MAX_NAME ];
		char szDate[ MAX_NAME ];
		char szEditName[ MAX_NAME ];
		char szSkill[ MAX_NAME ];
		char szFocus[ MAX_NAME ];
		char szClose[ MAX_NAME ];
		char szDamage[ MAX_NAME ];
		char szDefence[ MAX_NAME ];
		char szMovement[ MAX_NAME ];
		char szAbility[ MAX_NAME ];

		if( name )
		{
			i3::snprintf( szImg,		MAX_NAME, "%s_Img", name );
			i3::snprintf( szName,		MAX_NAME, "%s_Name", name );
			i3::snprintf( szDate,		MAX_NAME, "%s_Date", name );
			i3::snprintf( szEditName,	MAX_NAME, "%s_EditName", name );
			i3::snprintf( szSkill,		MAX_NAME, "%s_Skill", name );
			i3::snprintf( szFocus,		MAX_NAME, "%s_Focus", name );
			i3::snprintf( szClose,		MAX_NAME, "%s_CloseFrame", name );
			i3::snprintf( szDamage,		MAX_NAME, "%s_Damage", name );
			i3::snprintf( szDefence,	MAX_NAME, "%s_Defence", name );
			i3::snprintf( szMovement,	MAX_NAME, "%s_Movement", name );
			i3::snprintf( szAbility,	MAX_NAME, "%s_AbilityFrameWnd", name );
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "invalid UIControl name" );
			return false;
		}

		// 캐릭터 이미지 처리 방법?

		m_img			= (i3UIButton*)			m_wnd->FindChildByName( szImg );
		m_name			= (i3UIEditBox*)		m_wnd->FindChildByName( szName );
		m_date			= (i3UIStaticText*)		m_wnd->FindChildByName( szDate );
		m_editName		= (i3UICheckBox*)		m_wnd->FindChildByName( szEditName );
		m_skill			= (i3UIButtonImageSet*)	m_wnd->FindChildByName( szSkill );
		m_focus			= (i3UIFrameWnd*)		m_wnd->FindChildByName( szFocus	);
		m_close			= (i3UIFrameWnd*)		m_wnd->FindChildByName( szClose );

		m_Damage			= (i3UIStaticText*)		m_wnd->FindChildByName( szDamage );
		m_Defence			= (i3UIStaticText*)		m_wnd->FindChildByName( szDefence );
		m_Movement			= (i3UIStaticText*)		m_wnd->FindChildByName( szMovement );

		m_Ability = (i3UIFrameWnd*) m_wnd->FindChildByName( szAbility );

		EnableEditNickName( false );		

		//SetPlayType( CHARA_STATE_NONE);

		SetEnable( false );

		I3ASSERT( m_img );
		I3ASSERT( m_name );
		I3ASSERT( m_date );
		I3ASSERT( m_editName );
		I3ASSERT( m_skill );
		I3ASSERT( m_focus );
		I3ASSERT( m_close );
		I3ASSERT( m_Damage );
		I3ASSERT( m_Defence );
		I3ASSERT( m_Movement );
		I3ASSERT( m_Ability );
	}

	return true;
}

void	UITabProfileInfo::CCharaSlot::SetEnable( bool bEnable )
{
	if( m_wnd )
		m_wnd->EnableCtrl( bEnable ? true : false );

	/*if( m_img )
		m_img->setInputDisable( true );

	if( m_name )
		m_name->setInputDisable( true );

	if( m_point )
		m_point->setInputDisable( true );

	if( m_date )
		m_date->setInputDisable( true );

	if( m_skill )
		m_skill->setInputDisable( true );

	if( m_pointNum )
		m_pointNum->setInputDisable( true );*/

	if( m_focus )
		m_focus->setInputDisable( true );

	if( m_close )
		m_close->setInputDisable( true );
}

void	UITabProfileInfo::CCharaSlot::EnableEditNickName( bool bEdit )
{
	if( m_name )
	{
		bool is_read_mode = !bEdit;

		m_name->setInputDisable( is_read_mode );
		m_name->SetReadOnly( is_read_mode );		
		m_name->SetEllipsis( is_read_mode );
	}

	if( true == bEdit )
	{ 
		i3UIManager * pMgr = i3UI::getManager();

		if( pMgr )
			pMgr->SetFocusControl( m_name );
	}

	if( m_editName )
	{
		//m_editName->SkipUnselect( true );

		if( bEdit != m_editName->isSelected() )
		{
			if( m_editName->isSelected() )
				m_editName->removeState( I3UI_STATE_SELECTED );
			else						
				m_editName->addState( I3UI_STATE_SELECTED );

			m_editName->setModifyState();
		}
	}
}

void	UITabProfileInfo::CCharaSlot::SetCreateDate( UINT32 uDate )
{
	if( m_date )
	{
		// 날짜 타입을 UINT32 형식으로 변경 (((년 - 2000) * 100000000) + (월 * 1000000) + (일 * 10000) + (시* 100) + 분)

		UINT32 uYear	= (uDate / 100000000);
		UINT32 uMonth	= ((uDate % 100000000) / 1000000);
		UINT32 uDay		= ((uDate % 1000000) / 10000);
		//UINT32 uHour	= ((uDate % 10000) / 100);
		//UINT32 uMin		= (uDate % 100);

		char sz[ MAX_STRING_COUNT ];
		i3::snprintf( sz, MAX_STRING_COUNT, "Created\n%d.%d.%d", uYear, uMonth, uDay );

		m_date->SetTextMB( sz );
	}
}

void	UITabProfileInfo::CCharaSlot::SetNickName( const TTCHAR* name )
{
	if( m_name )
	{
		m_name->SetText(name);

		// Caret Pos : Edit Mode = Last, Read Mode = First 
		i3UIText * pUIText = m_name->getUIText();
		INT32 caret_pos = 0;

		if( m_name->isReadOnly() )
		{
			if(m_name->IsVisibleEllipsis())
			{
				i3::wstring stringname;
				i3::sprintf(stringname, L"%s", name);
				m_name->SetToolTipMsg(stringname.c_str());
			}
		}
		//Edit Mode
		else
		{
			caret_pos = i3::generic_string_size( name);
			m_name->SetIMEText( pUIText->getText());
			m_name->SetToolTipMsg(L"");
		}

		m_name->SetTextCaretPos( caret_pos);
	}

}

void	UITabProfileInfo::CCharaSlot::SetCharaImageID( int id )
{

	const CHARA_PARTS_INFO* parts = CCharaInfoContext::i()->GetCharaPartsInfo( id );

	if (parts == nullptr)
		return;

	// vv3 - check
	T_ItemID chara_id = parts->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

	ChangeCharaImageTemplate(chara_id);

	if ( m_img )
	{
		if ( CCharaInfoContext::i()->GetMainCharaIdx() == id || (m_img->isPushFlag() && CCharaInfoContext::i()->GetMainCharaIdx() != id) )
			m_img->UnSelectedNormalState();
	}
}

void	UITabProfileInfo::CCharaSlot::ChangeCharaImageTemplate( T_ItemID itemId )
{
	int nCharaImageID = -1;

	nCharaImageID = GameUI::GetCharaImageIndex( itemId);

	char szImgTemplate[MAX_STRING_COUNT];
	
	nCharaImageID + 1 >= 10 ?
		i3::snprintf(szImgTemplate, MAX_STRING_COUNT, "4T_Bt_myinfo_cha%d", nCharaImageID + 1) :
		i3::snprintf(szImgTemplate, MAX_STRING_COUNT, "4T_Bt_myinfo_cha0%d", nCharaImageID + 1);

	if( m_img )
	{
		m_img->SetTemplateByName(szImgTemplate);
	}
}

void	UITabProfileInfo::CCharaSlot::SetCharaSkillID( int id )
{
	// 동작 하는지 확인 필요?네오프랜
	if( m_skill )
	{
		if( id < 0 || m_skill->getShapeCount() <= id )
			m_skill->EnableCtrl( false );
		else
		{
			m_skill->SetShapeIdx( id );
			m_skill->EnableCtrl( true );
		}
	}
}

void	UITabProfileInfo::CCharaSlot::SetAbilityFrameWnd(bool val)
{
	m_Ability->EnableCtrl(val);
}

void	UITabProfileInfo::CCharaSlot::SetAbility( int idx )
{
	if( CCharaInfoContext::i() == nullptr)	return;
	
	INT32 chara_idx = idx;

	CharaInfoMgr* chara_mgr = CharaInfoMgr::instance();
	const ITEM_INFO & item_info = chara_mgr->find(chara_idx, item_def::SCTG_WEAPON, item_def::WT_PRIMARY);
	CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( item_info.m_TItemID );

	i3::stack_string stack_str;

	// Damage
	INT32 damage = 0;
	if( pWeaponInfo != nullptr)
		damage = pWeaponInfo->GetDamage();


	i3::itoa( damage, stack_str);

	m_Damage->SetText(stack_str.c_str());

	REAL32 ability_value[ EQUIP::eABILITY_COUNT ];
	memset(ability_value, 0, sizeof(REAL32) * EQUIP::eABILITY_COUNT ); 
	for(size_t i = item_def::PT_HEADGEAR; i<item_def::PT_MAX; ++i)
	{
		const ITEM_INFO & info = chara_mgr->find(chara_idx, item_def::SCTG_PARTS, i);
		if( info.m_TItemID == 0 ) continue;

		const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(info.m_TItemID);
		if(pInfo == nullptr)
			continue;
		for(size_t j = EQUIP::eABILITY_DAMAGE; j<EQUIP::eABILITY_COUNT; ++j)
		{
			ability_value[j] += pInfo->GetAbilityValue( (EQUIP::eABILITY)j );
		}
	}

	REAL32 defence = ability_value[EQUIP::eABILITY_DEFENSE_BODY] + ability_value[EQUIP::eABILITY_DEFENSE_ARM] + ability_value[EQUIP::eABILITY_DEFENSE_LEG];
	defence = (defence == 0.f) ? 100.f : (defence * 100.f) + 100.f;

	i3::ftoa( defence, stack_str, 1);

	m_Defence->SetText(stack_str.c_str());

	// vv3 - check
	const CHARA_PARTS_INFO* parts = CCharaInfoContext::i()->GetCharaPartsInfo( chara_idx );
	T_ItemID chara_id = parts->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

	const CCharaInfo * pCharInfo = g_pCharaInfoDataBase->GetCharaDBInfo( chara_id);

	//movement
	REAL32 movement = ( pCharInfo->GetMoveSpeed(MST_RUN, MDT_FRONT) + ability_value[EQUIP::eABILITY_MOVESPEED_FRONT]);
	if( pWeaponInfo != nullptr)
		movement = movement * pWeaponInfo->GetMoveNormalStand() * 0.1f;

	i3::ftoa( movement, stack_str, 1);

	m_Movement->SetText(stack_str.c_str());

/*
	i3::itoa( damage, stack_wstr);
	i3Lua::PushStringUTF16To8(L, stack_wstr);

	//i3Lua::PushStringUTF16To8(L, L"--");
	i3Lua::PushStringUTF16To8(L, L"--");
	i3Lua::PushStringUTF16To8(L, L"--");

	_EndLuaFunction(L, 3);*/
}

/*void	UITabProfileInfo::CCharaSlot::SetPoint( int nPoint )
{
	if( m_pointNum )
	{
		char sz[256];
		i3::snprintf( sz, 256, "%d", nPoint );
		m_pointNum->SetText( sz );
	}
}*/

void	UITabProfileInfo::CCharaSlot::SetFocus( bool bfocus )
{
	m_focus->EnableCtrl(bfocus);
}

bool	UITabProfileInfo::CCharaSlot::GetFocus(void)
{
	return (m_focus->isEnable()==true)?true:false;
}

void	UITabProfileInfo::CCharaSlot::SetCheckBoxEnable( bool bEnable)
{
	m_editName->EnableCtrl(bEnable);
}

void	UITabProfileInfo::CCharaSlot::SetClose( bool bClose )
{
	m_close->EnableCtrl(bClose);
/*	if(bClose)
	{
		m_close->EnableCtrl(false);
	}
	else
	{
		m_close->EnableCtrl(true);
		m_close->SetShapeIdx(2);
	}*/
}

void	UITabProfileInfo::CCharaSlot::Clear()
{
	m_wnd			= nullptr;
	m_img			= nullptr;
	m_skill			= nullptr;
	m_name			= nullptr;
	//m_point			= nullptr;
	m_date			= nullptr;
	m_editName		= nullptr;
	//m_pointNum		= nullptr;
	m_focus			= nullptr;
	m_close			= nullptr;
	m_Damage		= nullptr;
	m_Defence		= nullptr;
	m_Movement		= nullptr;
}


static UITabProfileInfo * GetThis()
{
	UIPhaseProfile* pSubPhase = static_cast<UIPhaseProfile*>(GameUI::GetPopup(UPW_PROFILE));
	if( pSubPhase != nullptr)
	{
		return static_cast<UITabProfileInfo*>(pSubPhase->GetTab( UIPhaseProfile::TAB_INFO));
	}

	return nullptr;
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int		UITabProfileInfo_EnableEditNickName( LuaState* L )	
	{
		// 검토 필요
		INT32 nSel	= i3Lua::GetIntegerArg( L, 1 );
		bool bEnable = i3Lua::GetBooleanArg( L, 2 );

		UITabProfileInfo * pTab = GetThis();
		if( pTab != nullptr)
		{
			pTab->PlayBtnClick( nSel );
			pTab->EnableEditNickName( nSel, bEnable ? true :false );
		}
		return 0;
	}

	int		UITabProfileInfo_EditNickName( LuaState* L )
	{
		// 검토 필요
		INT32 nSel = i3Lua::GetIntegerArg( L, 1 );
		INT32 nKey = i3Lua::GetIntegerArg( L, 2 );

		UITabProfileInfo * pTab = GetThis();
		if( pTab != nullptr)
		{
			pTab->PlayBtnClick( nSel );
			pTab->EditNickName( nSel, nKey );
		}
		return 0;
	}

	int		UITabProfileInfo_UpdateScroll( LuaState* L )
	{
		// 검토 필요
		UITabProfileInfo * pTab = GetThis();
		if( pTab != nullptr)
			pTab->UpdateScroll();
		return 0;
	}

	int		UITabProfileInfo_UpdateWheel( LuaState* L )
	{
		// 검토 필요
		INT32 nDelta = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileInfo * pTab = GetThis();
		if( pTab != nullptr)
			pTab->UpdateWheel( nDelta );
		return 0;
	}

	int		UITabProfileInfo_PlayBtnClick( LuaState* L )
	{
		// 검토 필요
		INT32 nSel = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileInfo * pTab = GetThis();
		if( pTab != nullptr)
			pTab->PlayBtnClick( nSel );
		return 0;
	}

	int		UITabProfileInfo_SelectCharacter( LuaState* L )
	{
		INT32 nSel = i3Lua::GetIntegerArg( L, 1 );

		UITabProfileInfo * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SelectCharacter(nSel);

		return 0;
	}

	int		UITabProfileInfo_ClanJoin( LuaState* L )
	{
		UIPhaseProfile * pPhaseProfile = (UIPhaseProfile*)g_pFramework->GetUIMainframe()->GetPopup(UPW_PROFILE);
		
		if (pPhaseProfile != nullptr)
			pPhaseProfile->OnEscapeKey();

		UITopMenu * pMenu = g_pFramework->GetUIMainframe()->GetTopMenu();
		if( pMenu != nullptr)
			pMenu->Clicked(3);

		return 0;
	}

	int		UITabProfileInfo_CharaPurchasing( LuaState* L )
	{
		UIPhaseProfile * pPhaseProfile = (UIPhaseProfile*)g_pFramework->GetUIMainframe()->GetPopup(UPW_PROFILE);

		if (pPhaseProfile != nullptr)
			pPhaseProfile->OnEscapeKey();

		if (g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() != USP_SHOP)
		{
			UITopMenu * pMenu = g_pFramework->GetUIMainframe()->GetTopMenu();
			if( pMenu != nullptr)
				pMenu->Clicked(0);

			// 캐릭터 구매이므로 상점의 캐릭터 탭으로 이동
			UIPhaseShop * pPhaseShop = (UIPhaseShop*)g_pFramework->GetUIMainframe()->GetSubPhase(USP_SHOP);
			if( pPhaseShop != nullptr)
				pPhaseShop->SetEnteranceTab(1);
		}
		else
		{
			// 이미 상점이지만 캐릭터 탭이 아닌 경우 탭 이동만 한다
			UIPhaseShop * pPhaseShop = (UIPhaseShop*)g_pFramework->GetUIMainframe()->GetSubPhase(USP_SHOP);
			if( pPhaseShop != nullptr && pPhaseShop->get_current_menu_idx() != 1)
			{
				pPhaseShop->clicked_menu(1);
				pPhaseShop->clicked_tap(1, -1);
				pPhaseShop->SelectedEnteranceTab(1);
				pPhaseShop->set_current_menu_idx(1);
			}
		}

		return 0;
	}
}

LuaFuncReg UITabProfileInfo_Glue[] =
{
	{"UpdateScroll",		UITabProfileInfo_UpdateScroll },
	{"UpdateWheel",			UITabProfileInfo_UpdateWheel },
	{"EditNickName",		UITabProfileInfo_EditNickName },
	{"EnableEditNickName",	UITabProfileInfo_EnableEditNickName },	
	{"PlayBtnClick",		UITabProfileInfo_PlayBtnClick },
	{"SelectCharacter",		UITabProfileInfo_SelectCharacter },
	{"ClanJoin",			UITabProfileInfo_ClanJoin },
	{"CharaPurchasing",		UITabProfileInfo_CharaPurchasing },
	{nullptr,	nullptr},
};

I3_CLASS_INSTANCE( UITabProfileInfo);//, UITabProfileBase);

UITabProfileInfo::UITabProfileInfo() : m_pCharaScroll(nullptr), m_pInfoWaitIcon(nullptr), m_pWaitMessageBox(nullptr), m_pLastGameImage(nullptr), m_iCurrentEditCharaSlot(-1)//, m_pMedalInfoBoard(nullptr)//, m_pMedalInfoBoardBtn(nullptr), m_bMedalBtn(false), m_bMedalBtnisOn(false)
{
	m_ClanMark.SetClanMarkCtrl(nullptr, nullptr);
}

UITabProfileInfo::~UITabProfileInfo()
{
	m_ClanMark.SetClanMarkCtrl(nullptr, nullptr);
	m_pCharaScroll	= nullptr;
}


void	UITabProfileInfo::OnCreate( i3GameNode * pParent)
{
	UITabProfileBase::OnCreate( pParent);
	AddScene( "Scene/Main/PointBlankRe_Profile_Info.i3UIs", UITabProfileInfo_Glue);
}

/*virtual*/ void UITabProfileInfo::OnUpdate(REAL32 rDeltaSeconds)
{
	tickTime += rDeltaSeconds;

	if(MedalContext::i()->GetReceivedMyMedal() == true)
	{
		if(m_pInfoWaitIcon != nullptr)
			GameUI::DisableLoadingIcon(m_pInfoWaitIcon);
		m_pInfoWaitIcon = nullptr;
		SetInputDisable(false);
		UIPhaseProfile * pParent = (UIPhaseProfile *)m_pParent;
		pParent->SetInputDisable(false);
		tickTime = 0.0f;
	}
	if(tickTime > 5.0f)
	{
		tickTime = 0.0f;
		UIPhaseProfile * pParent = (UIPhaseProfile *)m_pParent;
		pParent->OnEscapeKey();
	}
	
	UITabProfileBase::OnUpdate(rDeltaSeconds);
/*	if(m_pMedalInfoBoardBtn != nullptr)
	{
		if(m_bMedalBtn == true)
		{
			m_pMedalInfoBoardBtn->SetButtonShape(I3UI_BUTTON_SHAPE_PUSHED);
		}
		else
		{
			if(m_bMedalBtnisOn == true)
				m_pMedalInfoBoardBtn->SetButtonShape(I3UI_BUTTON_SHAPE_ONMOUSED);
			else
				m_pMedalInfoBoardBtn->SetButtonShape(I3UI_BUTTON_SHAPE_NORMAL);
		}
	}*/
}

/*virtual*/ bool UITabProfileInfo::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
{

	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		
		if( pParam->m_nEvent == I3UI_EVT_CLICKED || pParam->m_nEvent == I3UI_EVT_DRAGSTART )
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			
			bool bNoClickNickEdit = true;
			for( INT32 i = 0; i < MAX_UICHARA_SLOT; i++)
			{
				if( (( pParam->m_pCtrl == m_CharaSlot[i].m_editName ) || ( pParam->m_pCtrl == m_CharaSlot[i].m_name )) 
					&& ( m_iCurrentEditCharaSlot == i ) )
				{
					bNoClickNickEdit = false;
				}
			}
			
			if( bNoClickNickEdit )
				_UpdateUserCharaList_EditingName();
		}
	}

	return UITabProfileBase::OnEvent( event, pObj, param2, code);
}

void	UITabProfileInfo::OnUnloadAllScenes()
{
	UITabProfileBase::OnUnloadAllScenes();
	I3_SAFE_RELEASE(m_pLastGameImage);
}

bool	UITabProfileInfo::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( false == UITabProfileBase::OnEntranceStart( pArg1, pArg2) )
	{
		return false;
	}

	CLAN_DETAIL_INFO * pInfo = nullptr;

	m_pLastGameImage = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet");

	// 자신의 정보 or 다른 유저 정보인지에 따라 처리를 새로해야합니다.
	if(CCommunityContext::i()->BeCommunityUserInfo() == true)
	{
		_InitUserInfoData();
		pInfo = ClanGameContext::i()->getClanDetailInfo();
	}
	else
	{
		_InitMyInfoData();
		pInfo = ClanGameContext::i()->getMyClanDetailInfo();
	}
	//UpdateProperty(0);

	tickTime = 0.0f;
	i3UIControl * pCtrl = (i3UIControl*) GetScene(0)->FindChildByName( "i3UIFrameWnd6");
	m_pInfoWaitIcon = GameUI::EnableLoadingIcon( this, pCtrl);

	static_cast<UIPhaseProfile *>(GetMainFrame()->GetPopup(UPW_PROFILE))->setEscape(true);
	
	m_iCurrentEditCharaSlot = -1;

	return true;
}

void	UITabProfileInfo::_SetBasicData( const USER_INFO_BASIC* info, const USER_INFO_INVITEM_DATA* invData )
{
	if( nullptr == info )
	{
		return;
	}

	if(nullptr == invData)
		return; 

	const I3COLOR* pClanCr = GetClanNameColor( info );
	const I3COLOR* pNickCr = GetNickColor( invData );

	i3::rc_wstring wstrRankDesc;
	i3::stack_wstring wstrExp;

	UINT32 nRank = info->m_ui32Rank;
	INT32 nCurrentExp = info->m_ui32Exp - GameUI::GetMaxExp(nRank);
	nCurrentExp = MAX(nCurrentExp, 0);	// 음수값은 0
	INT32 nExpRate = 100;

	// 상대평가라도 목표 경험치 추가
	if (GameUI::is_hero_rank(nRank, true) == true)
	{
		i3::sprintf( wstrExp, L"%s %d", GAME_RCSTRING("STR_TBL_GUI_ETC_CUMULATIVE_EXP"), info->m_ui32Exp );
	}
	else
	{
		INT32 nMaxExp = GameUI::GetMaxExp(nRank + 1) - GameUI::GetMaxExp(nRank);
		//if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

		//nExpRate = MINMAX(0, nExpRate, 100);	// 100%가 넘을 경우 100%으로 고정 출력
		nExpRate = GameUI::GetExpRate(nRank, nCurrentExp);
		i3::sprintf( wstrExp, L"%d/%d%s (%d%%)", nCurrentExp, nMaxExp, GAME_RCSTRING("STR_TBL_MYINFO_EXP"), nExpRate );
	}
	
	if( 51 == nRank )
	{
		wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_HERO");//영웅
	}
	else if( 50 == nRank )
	{
		wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED5");//상위 1% 랭킹의 원수 계급 유지 중
	}
	else if( 45 < nRank )
	{
		if( UserInfoContext::i()->GetMaxExpByRank( 50 ) <= (signed)info->m_ui32Exp )
		{
			wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED3");//원수 진급은 상위 1% 랭킹 필요			
		}
		else if( UserInfoContext::i()->GetMaxExpByRank( 49 ) <= (signed) info->m_ui32Exp )
		{
			wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED6");//대장 진급은 상위 2% 랭킹 필요
		}
		else if( UserInfoContext::i()->GetMaxExpByRank( 48 ) <= (signed) info->m_ui32Exp)
		{
			wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED1");//중장 진급은 상위 3% 랭킹 필요
		}
		else if( UserInfoContext::i()->GetMaxExpByRank( 47 ) <= (signed) info->m_ui32Exp)
		{
			wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED4");//소장 진급은 상위 4% 랭킹 필요
		}
		else if( UserInfoContext::i()->GetMaxExpByRank( 46 ) <= (signed) info->m_ui32Exp)
		{
			wstrRankDesc = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED2");//준장 진급은 상위 5% 랭킹 필요
		}
		else
		{
			wstrRankDesc.clear();
		}
	}
	else
	{
		wstrRankDesc.clear();
	}

	// SetBasicData
	{
		USER_INFO_INVITEM_DATA myUserInfoInvItemData;

		UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

		LuaState* L = _CallLuaFunction( "SetBasicData" );

		if( nullptr != L )
		{
			i3Lua::PushStringUTF16To8(L, info->m_strNick );	
			i3Lua::PushInteger( L, pNickCr ? pNickCr->GetRGBA() : 0xffffffff );
			i3Lua::PushNumber( L, nExpRate * 0.01f );
			i3Lua::PushStringUTF16To8( L, wstrExp );

			if(CCommunityContext::i()->BeCommunityUserInfo() == false)
			{
				if (myUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
					i3Lua::PushInteger( L, myUserInfoInvItemData._wAprilFoolRank);
				else if (myUserInfoInvItemData._wRank != RANK_NONE)
					i3Lua::PushInteger( L, myUserInfoInvItemData._wRank);
				else
					i3Lua::PushInteger( L, nRank );
			}
			else
				i3Lua::PushInteger( L, nRank );

			i3Lua::PushStringUTF16To8( L, wstrRankDesc );

			_EndLuaFunction( L, 6 );
		}
	}

	// SetClanData
	{
		LuaState* L = _CallLuaFunction( "SetClanData" );

		if( nullptr != L )
		{
			bool is_clan_member = (i3::generic_string_size(info->m_strClanName) > 0);
			bool is_other_user_info =  CCommunityContext::i()->BeCommunityUserInfo() ? true : false;
			bool join_btn_enable = ( (is_other_user_info == false) && (is_clan_member == false) );

			i3::rc_wstring wstr_clan_name;

			if( is_clan_member )
			{
				wstr_clan_name = info->m_strClanName;
			}
			else
			{
				i3::sprintf(wstr_clan_name, GAME_RCSTRING("STBL_IDX_MINIPROFILE_CLAN_NOTJOIN") );
			}

			i3Lua::PushStringUTF16To8(L, wstr_clan_name );							// Clan Name		
			i3Lua::PushInteger( L, pClanCr ? pClanCr->GetRGBA() : 0xffffffff );		// Clan Name Color
			i3Lua::PushBoolean( L , join_btn_enable ? true : false);				// Join Btn Enable
			_EndLuaFunction( L, 3 );

			// Set Clan Mark
			m_ClanMark.SetClanMarkFlag(info->m_ui32ClanMark);
		}
	}	
}

void	UITabProfileInfo::_SetMyLastGameRecord()
{
	// TODO : 클라에서 마지막 배틀 정보 기록해야함
	i3::rc_wstring wstrStageName;
	i3::rc_wstring wstrModeName;

	const CCharaInfoContext::LRInfo* pInfo = CCharaInfoContext::i()->getMyLastRecord();

	LuaState* L = _CallLuaFunction( "SetLastGameRecord" );
	if(pInfo->_Acvite)
	{
		wstrStageName	=	CStageInfoUtil::GetMapData(pInfo->_lastStageID)->getDisplayName();
		wstrModeName	=	CStageInfoUtil::GetRuleData(pInfo->_lastStageID)->getRuleName();

		GetItemImageMgr()->SelectImage(m_pLastGameImage, UserInfoContext::i()->GetLastGameWeapon());
		// LastGameWeapon의 이름 셋팅하는 부분 필요

		CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( UserInfoContext::i()->GetLastGameWeapon(), true);
		i3::rc_wstring wstrTemp;
		 pWeaponInfo->GetWeaponName(wstrTemp);

		i3Lua::PushBoolean( L, true);	
		i3Lua::PushStringUTF16To8( L, wstrModeName	);		// Game Mode
		i3Lua::PushStringUTF16To8( L, wstrStageName );		// Game Stage			// TODO : 서버 협업 후 교체 (2014.07.24.수빈)	
		i3Lua::PushInteger( L, pInfo->_lastKill );		// Kill Count
		i3Lua::PushInteger( L, pInfo->_lastDeath);		// Death Count
		i3Lua::PushInteger( L, pInfo->_exp + pInfo->_addExp );	// Gain Exp
		i3Lua::PushInteger( L, pInfo->_point	);			// Gain Point
		i3Lua::PushInteger( L, pInfo->_addPoint );			// Bonus Point
		i3Lua::PushStringUTF16To8( L, wstrTemp);
		_EndLuaFunction( L, 9 );
	}
	else
	{
		//GetItemImageMgr()->SelectImage(m_pLastGameImage, 103004);
		m_pLastGameImage->SetEnable(false);
		i3Lua::PushBoolean( L, false);	
		i3Lua::PushStringUTF16To8( L, L"-" );	// Game Mode
		i3Lua::PushStringUTF16To8( L, L"-" );	// Game Stage
		i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Kill Count
		i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Death Count
		i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Gain Exp
		i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Gain Point
		i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Bonus Point
		i3Lua::PushStringUTF16To8( L, L"" );//i3Lua::PushInteger( L, 0 );			// LastGameWeaponName
		_EndLuaFunction( L, 9 );
	}
}

void	UITabProfileInfo::_SetUserLastGameRecord()
{	
	m_pLastGameImage->SetEnable(false);
	LuaState* L = _CallLuaFunction( "SetLastGameRecord" );
	i3Lua::PushBoolean( L, false);	
	i3Lua::PushStringUTF16To8( L, L"-" );	// Game Mode
	i3Lua::PushStringUTF16To8( L, L"-" );	// Game Stage
	i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Kill Count
	i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Death Count
	i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Gain Exp
	i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Gain Point
	i3Lua::PushStringUTF16To8( L, L"-" );//i3Lua::PushInteger( L, 0 );			// Bonus Point
	i3Lua::PushStringUTF16To8( L, L"" );//i3Lua::PushInteger( L, 0 );			// LastGameWeaponName
	_EndLuaFunction( L, 9 );
}

void	UITabProfileInfo::_SetKillDeathData( const USER_INFO_RECORD* info )
{
	if( nullptr == info )
	{
		return;
	}
	
	INT32 kdRate = 0;
	if(info->_killcount + info->_death == 0)
	{
		kdRate = 0;
	}
	else
	{
		kdRate = GameContextUtil::getKillRate(info->_killcount, info->_death);
		//kdRate = (INT32)(((float)info->_killcount / (float)(info->_killcount + info->_death)) * 100.0f);
	}

	LuaState* L = _CallLuaFunction( "SetKillDeathData" );
	if( nullptr != L )
	{
		i3Lua::PushInteger( L, info->_killcount );
		i3Lua::PushInteger( L, info->_AssistCount);
		i3Lua::PushInteger( L, info->_death );
		i3Lua::PushInteger( L, kdRate );
		i3Lua::PushInteger( L, info->_headshot );

		_EndLuaFunction( L, 5 );
	}
}

void	UITabProfileInfo::_SetBattleRecordData( const USER_INFO_RECORD* info )
{
	if( nullptr == info )
	{
		return;
	}

	LuaState* L = _CallLuaFunction( "SetBattleRecordData" );

	if( nullptr != L )
	{
		i3Lua::PushInteger( L, info->_match );
		i3Lua::PushInteger( L, info->_win );
		i3Lua::PushInteger( L, info->_lose );
		i3Lua::PushInteger( L, info->_draw );
		i3Lua::PushInteger( L, info->_dis );

		_EndLuaFunction( L, 5 );
	}
}

void	UITabProfileInfo::_SetMedalNumberOfHoldings()
{
	LuaState* L = _CallLuaFunction("SetMedalNumHoldings");
	
	if (nullptr != L)
	{
		i3Lua::PushInteger(L, MedalContext::i()->getMaster());
		i3Lua::PushInteger(L, MedalContext::i()->getMiniature());
		i3Lua::PushInteger(L, MedalContext::i()->getInsignia());
		i3Lua::PushInteger(L, MedalContext::i()->getOrder()); 
		_EndLuaFunction(L, 4);
	}
}

void	UITabProfileInfo::_InitializeAtLoad( i3UIScene * pScene )
{
	UITabProfileBase::_InitializeAtLoad( pScene );

	if( pScene )
	{
		i3UIImageBoxEx * pClanMark = static_cast<i3UIImageBoxEx*>( pScene->FindChildByName("ImgBox_ClanMark") );
		i3UIImageBox * pDefClanMark = static_cast<i3UIImageBox*>( pScene->FindChildByName("ImgBox_DefaultMark") );
		m_ClanMark.SetClanMarkCtrl(pClanMark, pDefClanMark);

		m_pCharaScroll	= (i3UIScrollBar*)	pScene->FindChildByName("i3UIScrollBar");
		I3ASSERT( nullptr != m_pCharaScroll );

		char szControl[ MAX_NAME ] = {0,};
		i3UIFrameWnd* pSlot = nullptr;

		for( int i = 0; i < MAX_UICHARA_SLOT; ++i )
		{
			i3::snprintf( szControl, MAX_NAME, "chara%d", i );			
			pSlot = (i3UIFrameWnd*) pScene->FindChildByName( szControl );

			m_CharaSlot[i].Init( pSlot );
		}
	}
}

void	UITabProfileInfo::UpdateCharaList()
{
	// 자신의 정보 or 다른 유저 정보인지에 따라 처리를 새로해야합니다.
	if(CCommunityContext::i()->BeCommunityUserInfo() == true)
	{
		_UpdateUserCharaList();
	}
	else
	{
		_UpdateMyCharaList();
	}
}

void	UITabProfileInfo::EnableEditNickName( int nSel, bool bEnable )
{
	if( nSel < 0 || MAX_UICHARA_SLOT <= nSel )
	{
		return;
	}

	int idx = nSel;

	if( m_pCharaScroll )
		idx += m_pCharaScroll->getScrollPos();

	if( true == bEnable )
	{
		_UpdateUserCharaList_EditingName();

		for( int i = 0; i < MAX_UICHARA_SLOT; ++i )
		{
			m_CharaSlot[ i ].EnableEditNickName(  i == nSel );
			m_iCurrentEditCharaSlot = nSel;
		}
		if ( CCharaInfoContext::i() )		
			m_CharaSlot[nSel].SetNickName(CCharaInfoContext::i()->GetNickName(idx));
	}
	else
	{
		const WCHAR16* szName = m_CharaSlot[ nSel ].GetNickName();	

		if( CCharaInfoContext::i() )
			CCharaInfoContext::i()->SetNickName( idx, szName );

		m_CharaSlot[ nSel ].EnableEditNickName( false );
		m_CharaSlot[nSel].SetNickName(szName);
		
		m_iCurrentEditCharaSlot = -1;

		_SetFocus_ScrollBar();
	}
}

void	UITabProfileInfo::EditNickName( int nSel, int nKey )
{
	if( CCharaInfoContext::i() == nullptr)
	{
		return;
	}

	if( nSel < 0 || MAX_UICHARA_SLOT <= nSel )
	{
		return;
	}

	int idx = nSel;

	if( m_pCharaScroll )
		idx += m_pCharaScroll->getScrollPos();

	switch( nKey )
	{
	case	VK_RETURN:
		{
			_UpdateUserCharaList_EditingName();
			_SetFocus_ScrollBar();
		}
		break;
	case	VK_ESCAPE:
		{
			m_CharaSlot[ nSel ].SetNickName( CCharaInfoContext::i()->GetNickName( idx ) );
			m_CharaSlot[ nSel ].EnableEditNickName( false );
			m_iCurrentEditCharaSlot = -1;
			_SetFocus_ScrollBar();
		}
		break;
	}
}

bool	UITabProfileInfo::OnExitStart()
{
	if( false == UITabProfileBase::OnExitStart() )
	{
		return false;
	}

	for( int i = 0; i < MAX_UICHARA_SLOT; ++i )
	{
		m_CharaSlot[i].Clear();
	}

	//m_pMedalInfoBoardBtn = nullptr;
	//m_pMedalInfoBoard = nullptr;

	return true;
}

void	UITabProfileInfo::UpdateScroll()
{
	UpdateCharaList();
}

void	UITabProfileInfo::PlayBtnClick( int nSel )
{
	if( nullptr == CCharaInfoContext::i() )
	{
		return;
	}

	if( nSel < 0 || MAX_UICHARA_SLOT <= nSel || nSel >= CCharaInfoContext::i()->GetHasCharacterCount() )
	{
		return;
	}

	int idx = nSel;

	if( m_pCharaScroll )
		idx += m_pCharaScroll->getScrollPos();

	//UpdateProperty(nSel);

	//UpdateCharaList();

	_SetFocus_ScrollBar();
}

bool	 UITabProfileInfo::RequestExitTab()
{
	m_bChangeTab = false;
	return _SendEvent_ChangeCharaNick();
}

bool	UITabProfileInfo::RequestChangeTab()
{
	m_bChangeTab = true;
	return _SendEvent_ChangeCharaNick();
}

void	UITabProfileInfo::SetChangeCharaNick( int arg )
{
	if( EV_SUCCESSED( arg ) )
	{
		if( m_bChangeTab )
			UITabProfileBase::RequestChangeTab();
		/*else
			GameEventSender::i()->SetEventOnce( EVENT_LEAVE_INFO );*/
	}

	if( CCharaInfoContext::i() )
		CCharaInfoContext::i()->SetChangeCharaNickName( arg );

	if( nullptr != m_pWaitMessageBox )
	{
		GameUI::CloseMsgBox( m_pWaitMessageBox );
		m_pWaitMessageBox = nullptr;
	}

	i3UI::setDisableUIInput( false );
}

void	UITabProfileInfo::OnEntranceEnd()
{
	UITabProfileBase::OnEntranceEnd();
	SetInputDisable(true);
	UIPhaseProfile * pParent = (UIPhaseProfile *)m_pParent;
	pParent->SetInputDisable(true);

	UpdateCharaList();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

bool	UITabProfileInfo::_SendEvent_ChangeCharaNick()
{
	bool bChangedNick = false;

	if( CCharaInfoContext::i() )
	{
		if( CCharaInfoContext::i()->GetHasCharacterCount() < 1 )
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_PROFILE_CHARA_SLOTCHECK_TWOMORE") );
			UpdateCharaList();
			return false;
		}

		bChangedNick = CCharaInfoContext::i()->IsChangedNickName();
		CCharaInfoContext::i()->SetChangeMultiSlotIdx();
	}	

	if( false == bChangedNick )
	{
		if( m_bChangeTab )
			return UITabProfileBase::RequestChangeTab();
		else
			return true;
	}	

	if( nullptr != m_pWaitMessageBox )
	{
		if( m_pWaitMessageBox->IsClosing() )
			GameUI::CloseMsgBox( m_pWaitMessageBox );
	}

	m_pWaitMessageBox = GameUI::MsgBox( MSG_TYPE_SYSTEM_WAIT, GAME_RCSTRING("STBL_IDX_EP_LOBBY_WAIT_ROOM_LIST_W") );

	i3UI::setDisableUIInput( true );
	GameEventSender::i()->SetEvent( EVENT_CHANGE_CHARA_NICK );

	return m_bChangeTab ? true : false;
}

void	UITabProfileInfo::_InitMyInfoData( void)
{
	if( UserInfoContext::i() )
	{
		USER_INFO_BASIC infoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic( &infoBasic );

		USER_INFO_INVITEM_DATA invData;
		UserInfoContext::i()->GetMyUserInfoInvItemData( &invData );

		const USER_INFO_RECORD* pInfoRecord = UserInfoContext::i()->GetMyUserInfoRecord();

		_SetBasicData( &infoBasic , &invData );
		_SetKillDeathData( pInfoRecord );
		_SetBattleRecordData( pInfoRecord );
		_SetMyLastGameRecord();
		_SetMedalNumberOfHoldings();
	}

	if( CCharaInfoContext::i() )
	{
		if( m_pCharaScroll )
		{
			if( MAX_UICHARA_SLOT < CCharaInfoContext::i()->GetHasCharacterCount() )
			{
				m_pCharaScroll->EnableCtrl( true );
				m_pCharaScroll->setTickSize( 3 );
				m_pCharaScroll->setPageSize( MAX_UICHARA_SLOT );
				m_pCharaScroll->setScrollRange( 0, CCharaInfoContext::i()->GetHasCharacterCount() );
				m_pCharaScroll->setScrollPos( 0 );
			}
			else
			{
				m_pCharaScroll->EnableCtrl( true );
				m_pCharaScroll->setTickSize( 3 );
				m_pCharaScroll->setPageSize( MAX_UICHARA_SLOT );
				m_pCharaScroll->setScrollRange( 0, MAX_UICHARA_SLOT );
				m_pCharaScroll->setScrollPos( 0 );
			}
		}
	}
}


void	UITabProfileInfo::_InitUserInfoData( void)
{
	USER_INFO_BASIC infoBasic;

	infoBasic.Reset();
	CCommunityContext::i()->GetUserSimpleInfo()->CopyToUserInfo(&infoBasic);

	//타유저 정보임.
	USER_INFO_INVITEM_DATA invData;
	const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();

	//아직까진 닉칼라만 쓰므로
	invData.Reset();
	invData._ui8NickColor = pInfo->_UserInfo.m_nickColor;

	const USER_INFO_RECORD* pInfoRecord = CCommunityContext::i()->GetUserRecordInfo();

	_SetBasicData( &infoBasic , &invData );
	_SetKillDeathData( pInfoRecord );
	_SetBattleRecordData( pInfoRecord );

	_SetUserLastGameRecord();
	_SetMedalNumberOfHoldings();

	if( m_pCharaScroll )
	{
		INT32 i32CharaCount = CCommunityContext::i()->GetUserDetailInfo()->_UserInfo.m_iCount;
		if( MAX_UICHARA_SLOT < i32CharaCount )
		{
			m_pCharaScroll->EnableCtrl( true );
			m_pCharaScroll->setTickSize( 3 );
			m_pCharaScroll->setPageSize( MAX_UICHARA_SLOT );
			m_pCharaScroll->setScrollRange( 0, i32CharaCount );
			m_pCharaScroll->setScrollPos( 0 );
		}
		else
		{
			m_pCharaScroll->EnableCtrl( true );
			m_pCharaScroll->setTickSize( 3 );
			m_pCharaScroll->setPageSize( MAX_UICHARA_SLOT );
			m_pCharaScroll->setScrollRange( 0, MAX_UICHARA_SLOT );
			m_pCharaScroll->setScrollPos( 0 );
		}
	}
}


void	UITabProfileInfo::SetCharaFocus(INT32 idx)
{
	for(INT32 i=0; i < MAX_UICHARA_SLOT; i++)
	{
		m_CharaSlot[i].SetFocus(false);
		m_CharaSlot[i].EnableEditNickName(false);
		m_iCurrentEditCharaSlot = -1;
	}
	m_CharaSlot[idx].SetFocus(true);
}

void	UITabProfileInfo::_UpdateMyCharaList( void)
{
	if( nullptr == CCharaInfoContext::i() )
	{
		return;
	}

	int idx;
	int nScrollPos = 0;

	if( m_pCharaScroll && CCharaInfoContext::i()->GetHasCharacterCount() > MAX_UICHARA_SLOT)
		nScrollPos = m_pCharaScroll->getScrollPos();


	for( int i = 0; i < MAX_UICHARA_SLOT; ++i )
	{
		idx = nScrollPos + i;

		if( idx < CCharaInfoContext::i()->GetHasCharacterCount() )
		{
			m_CharaSlot[i].SetEnable( true );

			m_CharaSlot[i].SetNickName( CCharaInfoContext::i()->GetNickName( idx ) );
			m_CharaSlot[i].SetCreateDate( CCharaInfoContext::i()->GetCreateDate( idx ) );
			m_CharaSlot[i].SetCharaImageID( idx );
			m_CharaSlot[i].SetCharaSkillID( CCharaInfoContext::i()->GetCharaSkill( idx )->m_PrimarySkill );
			m_CharaSlot[i].SetAbilityFrameWnd(true);
			m_CharaSlot[i].SetAbility( idx );
			m_CharaSlot[i].SetCheckBoxEnable(true);
			m_CharaSlot[i].SetClose(false);
		}
		else
		{
			m_CharaSlot[i].SetEnable( true );
			m_CharaSlot[i].SetClose(true);
			m_CharaSlot[i].SetFocus(false);
		}
	}
}

// 자신 외 유저들은 어떻게 처리할지?
void	UITabProfileInfo::_UpdateUserCharaList( void)
{
	int idx;
	int nScrollPos = 0;

	const USER_DETAIL_INFO * pDtailInfo = CCommunityContext::i()->GetUserDetailInfo();

	if( m_pCharaScroll)
		nScrollPos = m_pCharaScroll->getScrollPos();

	for( int i = 0; i < MAX_UICHARA_SLOT; ++i )
	{
		idx = nScrollPos + i;

		if( idx < pDtailInfo->GetCharaCount())
		{

			m_CharaSlot[i].SetEnable( true );
			m_CharaSlot[i].SetNickName( pDtailInfo->GetCharaName( idx ) );
			m_CharaSlot[i].SetCreateDate( pDtailInfo->GetCreateDate( idx ) );
			m_CharaSlot[i].ChangeCharaImageTemplate( pDtailInfo->GetCharaItemID(idx) );
			m_CharaSlot[i].SetCharaSkillID( pDtailInfo->GetCharaSkill(idx)->m_PrimarySkill );
			m_CharaSlot[i].SetAbilityFrameWnd(false);
			m_CharaSlot[i].SetCheckBoxEnable(false);
			m_CharaSlot[i].SetClose(false);
		}
		else
		{
			m_CharaSlot[i].SetEnable( true );
			m_CharaSlot[i].SetClose(false);
			m_CharaSlot[i].SetFocus(false);
		}
	}
}

void	UITabProfileInfo::_UpdateUserCharaList_EditingName( void)
{
	if( m_iCurrentEditCharaSlot != -1 )
	{
		int idx = m_iCurrentEditCharaSlot;

		if( m_pCharaScroll )
			idx += m_pCharaScroll->getScrollPos();

		const WCHAR16* szName = m_CharaSlot[ m_iCurrentEditCharaSlot ].GetNickName();	

		if(i3::generic_string_size(szName) > 0)
		{
			if( CCharaInfoContext::i() )
				CCharaInfoContext::i()->SetNickName( idx, szName );

			m_CharaSlot[ m_iCurrentEditCharaSlot ].EnableEditNickName( false );
			m_CharaSlot[ m_iCurrentEditCharaSlot ].SetNickName(szName);
			m_iCurrentEditCharaSlot = -1;
		}else
		{
			if( CCharaInfoContext::i() )
				m_CharaSlot[ m_iCurrentEditCharaSlot ].SetNickName(CCharaInfoContext::i()->GetNickName(idx) );
		}
	}
}

void	UITabProfileInfo::UpdateWheel( int nDelta )
{
	if( m_pCharaScroll )
	{
		int nTerm	= 0 < nDelta ? 1 : -1;
		int nPos	= m_pCharaScroll->getScrollPos() + nTerm * m_pCharaScroll->getTickSize();

		m_pCharaScroll->setScrollPos( nPos );
	}

	UpdateCharaList();
}

void	UITabProfileInfo::_SetFocus_ScrollBar()
{
	i3UIManager* pMgr = i3UI::getManager();

	if( pMgr )
		pMgr->SetFocusControl( m_pCharaScroll );
}

void	UITabProfileInfo::SelectCharacter(INT32 idx)
{
	if(CCommunityContext::i()->BeCommunityUserInfo() == true)
	{
		_SetFocus_ScrollBar();
		return;
	}

	idx += m_pCharaScroll->getScrollPos();

	if ( CCharaInfoContext::i()->GetMainCharaIdx() == idx)
		return;

	CCharaInfoContext::i()->SetMainCharaIdx(idx);

	for( int i = 0; i < MAX_UICHARA_SLOT; ++i )
		m_CharaSlot[i].SetCharaImageID(i + m_pCharaScroll->getScrollPos());

	UIFloatCharacterView * pCharaView = (UIFloatCharacterView*) g_pFramework->GetUIMainframe()->GetFloating(UFW_CHARACTER);
	if( pCharaView != nullptr)
		pCharaView->UpdateCharaSlot();

	_SetFocus_ScrollBar();
}