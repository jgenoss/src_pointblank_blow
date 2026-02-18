#include "pch.h"
#include "UIUtil.h"
#include "UIClanRankSlot.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/format_string.h"
#include "ClanGameContext.h"

UIClanRankSlot :: UIClanRankSlot()
: m_pClanRank		(nullptr)
, m_pClanMenberNum	(nullptr)
, m_pClanScore		(nullptr)
{

};

UIClanRankSlot::~UIClanRankSlot()
{

}

bool UIClanRankSlot::Init(i3UIFrameWnd * pWnd, INT32 idx)
{
	if(pWnd != nullptr)
	{
		if( g_pEnvSet->m_nUIVersion != UFV_1_5 )
		{
			m_nClanMarkSize = 20;
		}
		else
		{
			m_nClanMarkSize = 26;
		}

		char szClanName[ MAX_NAME ] = {0,};
		char szClanMark[ MAX_NAME ] = {0,};
		char szVisitBtn [ MAX_NAME ] = {0,};
		char szClanRank[ MAX_NAME ] = {0,};
		char szClanMemberNum[ MAX_NAME ] = {0,};
		char szClanScore[ MAX_NAME ] = {0,};

		i3::snprintf(szClanName,		MAX_NAME, "StaticText_ClanName%d",	idx);
		i3::snprintf(szClanMark,		MAX_NAME, "ImgBox_ClanMark%d",		idx);
		i3::snprintf(szVisitBtn,		MAX_NAME, "Btn_VisitClan%d",		idx);
		i3::snprintf(szClanRank,		MAX_NAME, "StaticText_Rank%d",		idx);
		i3::snprintf(szClanMemberNum,	MAX_NAME, "StaticText_MemberNum%d",	idx);
		i3::snprintf(szClanScore,		MAX_NAME, "StaticText_Score%d",		idx);

		m_pClanName = (i3UIStaticText *) pWnd->FindChildByName(szClanName);
		m_pClanMark = (i3UIImageBoxEx *) pWnd->FindChildByName(szClanMark);
		m_pVisitBtn = (i3UIButton *) pWnd->FindChildByName(szVisitBtn);

		m_pClanRank = (i3UIStaticText *) pWnd->FindChildByName(szClanRank);
		m_pClanMenberNum = (i3UIStaticText *) pWnd->FindChildByName(szClanMemberNum);
		m_pClanScore = (i3UIStaticText *) pWnd->FindChildByName(szClanScore);


		I3ASSERT( m_pClanName );
		I3ASSERT( m_pClanMark );
		I3ASSERT( m_pVisitBtn );
		I3ASSERT( m_pClanRank );
		I3ASSERT( m_pClanMenberNum );
		I3ASSERT( m_pClanScore );

		return true;
	}
	else
	{
		m_pClanName = nullptr;
		m_pClanMark = nullptr;
		m_pVisitBtn = nullptr;
		m_pClanRank = nullptr;
		m_pClanMenberNum = nullptr;
		m_pClanScore = nullptr;
	}

	return false;
}

void UIClanRankSlot::SetSlotData(SORT_CLAN_INFO * pInfo, INT32 i32SelSlot, INT32 i32Rank, CLAN_SOCORE_VIEW view_type)
{
	I3ASSERT_RETURN(m_pClanName != nullptr);

	if(pInfo != nullptr )
	{
		m_i32ClanIdx = (INT32)pInfo->_info._idx;

		i3::wliteral_range wstrName = pInfo->_info._name;

		m_pClanName->SetText(wstrName);

		i3::wstring wstrPerson = i3::format_string(L"%d/%d",pInfo->_info._person,pInfo->_info._maxperson);
		m_pClanMenberNum->SetText(wstrPerson);

		i3::wstring wstrScore = GetScoreString(pInfo, view_type);
		
		m_pClanScore->SetText(wstrScore);

		//i3::wstring wstrRank = i3::format_string(L"%d",pInfo->_info._rank);
		i3::wstring wstrRank = i3::format_string(L"%d", i32Rank +1);
		m_pClanRank->SetText(wstrRank);


		GameUI::SetClanTexture(m_pClanMark, CLAN_MARK_MEDIUM, pInfo->_info._mark);

		// 여기서 사용되는 클랜마크 사이즈는 26 x 26
		m_pClanMark->SetShapeSize( (REAL32)m_nClanMarkSize, (REAL32)m_nClanMarkSize);

		SetSlotEnable(true, i32SelSlot);

		CLAN_DETAIL_INFO	* pinfo = ClanGameContext::i()->getMyClanDetailInfo();
		if( pinfo )
		{
			if( i3::wstring(wstrName) == i3::wstring(pinfo->m_name))
			{
				m_pVisitBtn->OnActivate(false);
				m_pVisitBtn->setModifyState();
			}
			else
			{
				m_pVisitBtn->OnActivate(true);
			}
		}
	}
	else
	{
		m_i32ClanIdx = -1;
		SetSlotEnable(false, i32SelSlot);
	}
}

void UIClanRankSlot::SetSlotEnable(bool bEnable, INT32 i32SelSlot)
{
	if( m_pClanMark != nullptr)
	{
		m_pClanMark->EnableCtrl(bEnable);
		m_pClanName->EnableCtrl(bEnable);
		m_pVisitBtn->EnableCtrl(bEnable);

		m_pClanMenberNum->EnableCtrl(bEnable);
		m_pClanScore->EnableCtrl(bEnable);
		m_pClanRank->EnableCtrl(bEnable);
	}
}

i3::rc_wstring UIClanRankSlot::GetScoreString(SORT_CLAN_INFO * pInfo, CLAN_SOCORE_VIEW view_type)
{
	i3::rc_wstring wstrScore = L"";

	switch(view_type)
	{
	case CSV_RECORD:
		{
			wstrScore = i3::format_string(L"%dM / %dW / %dL",pInfo->_RecordInfo.m_match,pInfo->_RecordInfo.m_win,pInfo->_RecordInfo.m_lose);
		}
		break;
	case CSV_CUR_CLANPOINT:
		{
			wstrScore = i3::format_string(L"%d", pInfo->_SortInfo.m_clanpoint );
		}
		break;
	case CSV_PRE_CLANPOINT:
		{
			wstrScore = i3::format_string(L"%d", pInfo->_SortInfo.m_Preclanpoint);
		}
		break;
	}

	return wstrScore;
}
