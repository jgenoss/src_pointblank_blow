#include "pch.h"
#include "ClanDef.h"
#include "GlobalVariables.h"
#include "GameGUI.h"
#include "ServerInfo.h"
#include "i3Base/string/ext/generic_string_cat.h"

// GUI Shpae offst
#define GUI_SHAPE_OFFSET			0

//	클랜 등급 경험치
UINT32 g_ClanExp[CLAN_RANK_MAX] = 
{
	DF_CLAN_RANK_NEW,	//	신생
	DF_CLAN_RANK_A1,	//	훈련 1단계
	DF_CLAN_RANK_A2,	//	훈련 2단계
	DF_CLAN_RANK_B1,	//	견습 1단계
	DF_CLAN_RANK_B2,	//	견습 2단계
	DF_CLAN_RANK_C1,	//	지원 1단계
	DF_CLAN_RANK_C2,	//	지원 2단계
	DF_CLAN_RANK_C3,	//	지원 3단계
	DF_CLAN_RANK_D1,	//	우수 1단계
	DF_CLAN_RANK_D2,	//	우수 2단계
	DF_CLAN_RANK_D3,	//	우수 3단계
	DF_CLAN_RANK_D4,	//	우수 4단계
	DF_CLAN_RANK_E1,	//	전투 1단계
	DF_CLAN_RANK_E2,	//	전투 2단계
	DF_CLAN_RANK_E3,	//	전투 3단계
	DF_CLAN_RANK_E4,	//	전투 4단계
	DF_CLAN_RANK_F1,	//	선봉 1단계
	DF_CLAN_RANK_F2,	//	선봉 2단계
	DF_CLAN_RANK_F3,	//	선봉 3단계
	DF_CLAN_RANK_F4,	//	선봉 4단계
	DF_CLAN_RANK_F5,	//	선봉 5단계
	DF_CLAN_RANK_G1,	//	기동 1단계
	DF_CLAN_RANK_G2,	//	기동 2단계
	DF_CLAN_RANK_G3,	//	기동 3단계
	DF_CLAN_RANK_G4,	//	기동 4단계
	DF_CLAN_RANK_G5,	//	기동 5단계
	DF_CLAN_RANK_H1,	//	수색 1단계
	DF_CLAN_RANK_H2,	//	수색 2단계
	DF_CLAN_RANK_H3,	//	수색 3단계
	DF_CLAN_RANK_H4,	//	수색 4단계
	DF_CLAN_RANK_H5,	//	수색 5단계
	DF_CLAN_RANK_I1,	//	특전 1단계
	DF_CLAN_RANK_I2,	//	특전 2단계
	DF_CLAN_RANK_I3,	//	특전 3단계
	DF_CLAN_RANK_I4,	//	특전 4단계
	DF_CLAN_RANK_I5,	//	특전 5단계
	DF_CLAN_RANK_J1,	//	특공 1단계
	DF_CLAN_RANK_J2,	//	특공 2단계
	DF_CLAN_RANK_J3,	//	특공 3단계
	DF_CLAN_RANK_J4,	//	특공 4단계
	DF_CLAN_RANK_J5,	//	특공 5단계
	DF_CLAN_RANK_K1,	//	특수 1단계
	DF_CLAN_RANK_K2,	//	특수 2단계
	DF_CLAN_RANK_K3,	//	특수 3단계
	DF_CLAN_RANK_K4,	//	특수 4단계
	DF_CLAN_RANK_K5,	//	특수 5단계
	DF_CLAN_RANK_L1,	//	특무 1단계
	DF_CLAN_RANK_L2,	//	특무 2단계
	DF_CLAN_RANK_L3,	//	특무 3단계
	DF_CLAN_RANK_L4,	//	특무 4단계
	DF_CLAN_RANK_L5,	//	특무 5단계
};

//	클랜 부대 조건
INT32 g_ClanUnitCondition[CLAN_UNIT_MAX] = 
{
	DF_CLAN_UNIT_SQUARD,			//	분대
	DF_CLAN_UNIT_PLATOON,			//	소대
	DF_CLAN_UNIT_COMPANY,			//	중대
	DF_CLAN_UNIT_BATTALION,			//	대대
	DF_CLAN_UNIT_REGIMENT,			//	연대
	DF_CLAN_UNIT_BRIGADE,			//	여단
	DF_CLAN_UNIT_DIVISION,			//	사단
	DF_CLAN_UNIT_CORPS,				//	군단
};

i3::rc_wstring	GetClanMemberColor(INT32 Level)
{
	//	클랜원 직급에 해당하는 색상
	static const i3::rc_wstring s_pMemberColor[CLAN_MEMBER_LEVEL_MAX] = 
	{
		L"{col:255,255,255,255}%s{/col}",	//	CLAN_MEMBER_LEVEL_UNKNOWN
		L"{col:255,211,3,255}%s{/col}",		//	CLAN_MEMBER_LEVEL_MASTER
		L"{col:0,153,0,255}%s{/col}",		//	CLAN_MEMBER_LEVEL_STAFF
		L"{col:173,173,173,255}%s{/col}",	//	CLAN_MEMBER_LEVEL_REGULAR
	};

	if ( Level >= 0 && Level < CLAN_MEMBER_LEVEL_MAX )
	{
		return s_pMemberColor[Level];
	}

	return s_pMemberColor[CLAN_MEMBER_LEVEL_UNKNOWN];
}



//	클랜 등급
CLAN_RANK GetClanRank(UINT32 nClanExp)
{
	for(INT32 i=CLAN_RANK_TOP; i>=0; i--)
	{
		if(nClanExp >= g_ClanExp[i])
			return (CLAN_RANK) i;
	}

	return CLAN_RANK_UNKNOWN;
}

//	클랜 최대 인원수
INT32 GetClanStaffMax(CLAN_UNIT nUnit)
{
	if( (INT32)nUnit < 0 || (INT32)nUnit >= (INT32)CLAN_UNIT_MAX)
		return -1;

	return g_ClanUnitCondition[ nUnit];
}

//	클랜 접미사
CLAN_UNIT GetClanUnit(INT32 nClanStaffNum)
{
	for(INT32 i=CLAN_UNIT_TOP; i>=0; i--)
	{
		if(nClanStaffNum >= g_ClanUnitCondition[i])
			return (CLAN_UNIT) i;
	}

	return CLAN_UNIT_UNKNOWN;
}

//	클랜 레벨업 필요 경험치
UINT32 GetClanNextRankExp(UINT32 nCurExp)
{
	//	최고 레벨을 넘어섰다.
	if( nCurExp >= g_ClanExp[CLAN_RANK_TOP])
	{
		return g_ClanExp[CLAN_RANK_TOP];
	}

	for(INT32 i=CLAN_RANK_TOP-1; i>=0; i--)
	{
		if(nCurExp >= g_ClanExp[i])
		{
			return g_ClanExp[i + 1];	//	현재 레벨에서 한단계 위 레벨 경험치가 필요하다.
		}
	}

	return 0;
}

//	클랜 레벨업 필요 경험치
UINT32 GetClanBeforeRankExp(UINT32 nCurExp)
{
	//	최고 레벨을 넘어섰다.
	if( nCurExp >= g_ClanExp[CLAN_RANK_TOP])
	{
		return g_ClanExp[CLAN_RANK_TOP-1];
	}

	for( INT32 i = 1; i <= CLAN_RANK_TOP ; i++ )
	{
		if(nCurExp < g_ClanExp[i])
		{
			return g_ClanExp[i - 1];	//	현재 레벨에서 한단계 아래 레벨 경험치가 필요하다.
		}
	}

	return 0;
}
//	클랜 접두사 가져오기
void GetClanRankPreName(i3::rc_wstring& out, UINT32 nClanExp)
{
	
	CLAN_RANK nRank = GetClanRank(nClanExp);

	GetClanRankPreName2(out, (UINT8) nRank);
}

void GetClanRankPreName2(i3::rc_wstring& out, UINT8 nClanExpRank)
{
	CLAN_RANK nRank = (CLAN_RANK) nClanExpRank;

	switch(nRank)
	{
	case CLAN_RANK_NEW:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_NEW");	break;	// "신생"
	case CLAN_RANK_A1:
	case CLAN_RANK_A2:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_A");		break;	// "훈련"
	case CLAN_RANK_B1:	
	case CLAN_RANK_B2:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_B");		break;	// "견습"
	case CLAN_RANK_C1:
	case CLAN_RANK_C2:
	case CLAN_RANK_C3:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_C");		break;	// "지원"
	case CLAN_RANK_D1:
	case CLAN_RANK_D2:
	case CLAN_RANK_D3:
	case CLAN_RANK_D4:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_D");		break;	// "우수"
	case CLAN_RANK_E1:
	case CLAN_RANK_E2:
	case CLAN_RANK_E3:
	case CLAN_RANK_E4:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_E");		break;	// "전투"
	case CLAN_RANK_F1:
	case CLAN_RANK_F2:
	case CLAN_RANK_F3:
	case CLAN_RANK_F4:
	case CLAN_RANK_F5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_F");		break;	// "선봉"
	case CLAN_RANK_G1:
	case CLAN_RANK_G2:
	case CLAN_RANK_G3:
	case CLAN_RANK_G4:
	case CLAN_RANK_G5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_G");		break;	// "기동"
	case CLAN_RANK_H1:
	case CLAN_RANK_H2:
	case CLAN_RANK_H3:
	case CLAN_RANK_H4:
	case CLAN_RANK_H5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_H");		break;	// "수색"
	case CLAN_RANK_I1:
	case CLAN_RANK_I2:
	case CLAN_RANK_I3:
	case CLAN_RANK_I4:
	case CLAN_RANK_I5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_I");		break;	// "특전"
	case CLAN_RANK_J1:
	case CLAN_RANK_J2:
	case CLAN_RANK_J3:
	case CLAN_RANK_J4:
	case CLAN_RANK_J5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_J");		break;	// "특공"
	case CLAN_RANK_K1:
	case CLAN_RANK_K2:
	case CLAN_RANK_K3:
	case CLAN_RANK_K4:
	case CLAN_RANK_K5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_K");		break;	// "특수"
	case CLAN_RANK_L1:
	case CLAN_RANK_L2:
	case CLAN_RANK_L3:
	case CLAN_RANK_L4:
	case CLAN_RANK_L5:	out = GAME_RCSTRING("STBL_IDX_CLAN_RANK_L");		break;	// "특무
	default:
		I3ASSERT_0;
		break;
	}
}

//	클랜이름 접미사 가져오기
void GetClanRankPostName(i3::rc_wstring& out, INT32 nClanStaffNum)
{
	CLAN_UNIT nUnit = GetClanUnit(nClanStaffNum);

	GetClanRankPostName2(out, (UINT8) nUnit);
}

void GetClanRankPostName2(i3::rc_wstring& out, UINT8 nClanPerRank)
{
	CLAN_UNIT nUnit = (CLAN_UNIT) nClanPerRank;

	switch(nUnit)
	{
	case CLAN_UNIT_SQUARD:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_SQUARD");	break;	// "분대"
	case CLAN_UNIT_PLATOON:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_PLATOON");	break;	// "소대"
	case CLAN_UNIT_COMPANY:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_COMPANY");	break;	// "중대"
	case CLAN_UNIT_BATTALION:	out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_BATTALION");	break;	// "대대"
	case CLAN_UNIT_REGIMENT:	out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_REGIMENT");	break;	// "연대"
	case CLAN_UNIT_BRIGADE:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_BRIGADE");	break;	// "여단"
	case CLAN_UNIT_DIVISION:	out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_DIVISION");	break;	// "사단"	
	case CLAN_UNIT_CORPS:		out = GAME_RCSTRING("STBL_IDX_CLAN_UNIT_CORPS");		break;	// "군단"
	default:
		I3ASSERT_0;
		break;
	}
}

//	클랜 등급별 풀네임 가져오기
void GetClanFullName(i3::wstring& out, UINT32 nClanExp, INT32 nClanStaffNum)
{

	i3::rc_wstring wstrTemp;
	//	클랜 접두사
	GetClanRankPreName(wstrTemp, nClanExp);
	out = wstrTemp;

	//	클랜 접미사
	GetClanRankPostName(wstrTemp, nClanStaffNum);
	out.append(wstrTemp.begin(), wstrTemp.end());

}

void GetClanFullName2(i3::wstring& out, UINT8 nClanExpRank, UINT8 nClanPerRank)
{

	i3::rc_wstring wstrTemp;

	//	클랜 접두사
	GetClanRankPreName2(wstrTemp, nClanExpRank);
	out = wstrTemp;

	//	클랜 접미사
	GetClanRankPostName2(wstrTemp, nClanPerRank);
	out.append(wstrTemp.begin(), wstrTemp.end());
}

//	계급 설정
void SetRankImage(i3GuiStatic * pRankStatic, UINT8 nRank)
{
	I3ASSERT( pRankStatic != nullptr);

	pRankStatic->SetEnable(true);

	GameGUI::SetRankStatic(pRankStatic);
	pRankStatic->SetShapeUVIndex(nRank);		

	pRankStatic->OnUpdate(0.0f);
}

void SetClanRankImage(i3GuiStatic * pRankStatic, UINT8 nRank)
{
	I3ASSERT( pRankStatic != nullptr);

	pRankStatic->SetEnable(true);
	pRankStatic->SetAllShapeEnable(false);
	pRankStatic->SetShapeEnable(GUI_SHAPE_OFFSET + nRank, true);
	pRankStatic->OnUpdate(0.0f);
}

CLAN_MEMBER_LEVEL_TYPE GetMemberLevel(UINT8	nLevel)
{
	I3_BOUNDCHK((INT32) nLevel, (INT32) CLAN_MEMBER_LEVEL_MAX);

	return (CLAN_MEMBER_LEVEL_TYPE) nLevel;
}

void GetMemberLevelName(CLAN_MEMBER_LEVEL_TYPE level, i3::wstring& out)
{	
//	I3ASSERT(pBuf != nullptr);

//	i3::rc_wstring temp[MAX_STRING_COUNT] = "";
	i3::rc_wstring wstrColorFormat = GetClanMemberColor(level);

	switch( level)
	{
	case CLAN_MEMBER_LEVEL_MASTER:	i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_MASTER_STATIC"));		break;	// "마스터"
	case CLAN_MEMBER_LEVEL_STAFF:	i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_STAFF_STATIC"));		break;	// "스태프"
	case CLAN_MEMBER_LEVEL_REGULAR:	i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_MEMBERSHIP_STATIC"));	break;	// "일반회원"
	default:
		{
			// "알수없음"
			i3::generic_string_cat(wstrColorFormat, L"%d");
			i3::sprintf(out, wstrColorFormat, GAME_RCSTRING("STBL_IDX_CLAN_UNKNOWN"), level);
		}
		break;
	}
}

void GetMemberLevelName2(UINT8 nLevel, i3::wstring& out)
{
	I3_BOUNDCHK((INT32) nLevel, (INT32) CLAN_MEMBER_LEVEL_MAX);

	return GetMemberLevelName( GetMemberLevel(nLevel), out);
}

FRIEND_STATE GetMemberConnectState(UINT32 nState)
{
	return (FRIEND_STATE) GET_FRIEND_STATE( nState);
}

INT32 GetMemberConnectChannel(UINT32 nState)
{
	return GET_FRIEND_CHANNEL( nState);
}

INT32 GetMemberConnectRoomNum(UINT32 nState)
{
	return GET_FRIEND_ROOMIDX( nState);
}

void GetMemberConnectStateName(UINT32 nState, i3::rc_wstring& out )
{
	switch(GetMemberConnectState(nState))
	{
	case FRIEND_STATE_REQUEST:		out = GAME_RCSTRING("STBL_IDX_CLAN_SECRET");	break;	// 비공개
	case FRIEND_STATE_HOLD:			out = GAME_RCSTRING("STBL_IDX_CLAN_SECRET");	break;	// 비공개
	case FRIEND_STATE_OFFLINE:		out = GAME_RCSTRING("STBL_IDX_CLAN_OFFLINE");	break;	// 오프라인
	case FRIEND_STATE_ONLINE:		out = GAME_RCSTRING("STR_POPUP_READY_STATE");	break;	// 온라인
	case FRIEND_STATE_LOBBY:		out = GAME_RCSTRING("STBL_IDX_CLAN_ONLINE");	break;	// 온라인
	case FRIEND_STATE_ROOM:			FormatLocation(nState, out);					break;
	default:						out = GAME_RCSTRING("STBL_IDX_CLAN_UNKNOWN");	break;	// 알수없음
	}
}

bool IsOnlineUser(UINT32 nState)
{
	if(GetMemberConnectState(nState) == FRIEND_STATE_OFFLINE)
		return false;

	return true;
}

UINT32 GetEntryYear(UINT32 nDate)
{
	return (nDate / 10000);
}

UINT32 GetEntryMonth(UINT32 nDate)
{
	return ((nDate % 10000) / 100);
}

UINT32 GetEntryDay(UINT32 nDate)
{
	return (nDate % 100);
}

void SetCheckBox(i3GuiCheckBox * pControl, bool bChecked)
{
	pControl->SetEnable(true);
	pControl->setCheckBoxState(bChecked);

	pControl->OnUpdate(0.0f);
}

void SetClanExpText(i3GuiEditBox * pControl, UINT32 nCurExp, UINT32 nMaxExp)
{
	UINT32 nRate = 0;

	if( nMaxExp <= 0)  
	{
		pControl->SetText(L"");
		return;
	}
	else
	{
		nRate = (nCurExp * 100 / nMaxExp);
	}

	//	문자
	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, L"%u / %u (%u%%)", nCurExp, nMaxExp, nRate );
	pControl->SetText(wstrTemp);
}

void SetClanExpGauge(i3GuiStatic * pExpBar, REAL32 fResolutionWidth, UINT32 nCurExp, UINT32 nMaxExp)
{
	UINT32 nRate = 0;
	REAL32 fOriginWidth = 0.0f;
	i3GuiImage * pExpImage = nullptr;

	if( nMaxExp <= 0) 
	{
		pExpBar->SetEnable(false);
		pExpBar->OnUpdate(0.0f);
		return;		
	}
	else
	{
		pExpBar->SetEnable(true);
		nRate = (nCurExp * 100 / nMaxExp);
	}

	//	게이지	
	pExpImage = pExpBar->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);		
	fOriginWidth = pExpImage->getOriginalWidth() * fResolutionWidth;

	pExpImage->setWidth( ((REAL32)nRate / 100.0f) * fOriginWidth);

	pExpBar->OnUpdate(0.0f);
}

void SetClanStaff(i3GuiEditBox * pControl, INT32 nCurrentNum, INT32 nTotalNum)
{
	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, L"%d / %d", nCurrentNum, nTotalNum);

	pControl->SetText(wstrTemp);
}

/*
void SetClanMaster(i3GuiEditBox * pName, i3GuiStatic * pRank, const char * pszName, UINT8 nRank)
{
	pName->SetText(pszName);

	//	계급장
	if(pszName == nullptr || strlen(pszName) == 0)
	{
		pRank->SetEnable(false);
	}
	else
	{
		SetRankImage(pRank, nRank);
	}
}
*/
/*
void SetClanRank(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank)
{
	char szClanName[MAX_STRING_COUNT] = "";

	if( nClanStaffNum > 0)
	{
		//	클랜 등급 호칭
		GetClanFullName(szClanName, MAX_STRING_COUNT, nClanExp, nClanStaffNum);

		//	클랜 등급 이미지
		pRankImage->SetEnable(true);
		SetClanRankImage(pRankImage, nClanRank);
	}
	else
	{
		pRankImage->SetEnable(false);
	}

	pRankName->SetText(szClanName);
}
*/
/*
void SetClanRank2(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT8 nClanExpRank, UINT8 nClanPerRank, UINT8 nClanRank)
{
	char szClanName[MAX_STRING_COUNT] = "";

	//	클랜 등급 호칭
	GetClanFullName2(szClanName, MAX_STRING_COUNT, nClanExpRank, nClanPerRank);

	//	클랜 등급 이미지
	pRankImage->SetEnable(true);
	SetClanRankImage(pRankImage, nClanRank);

	pRankName->SetText(szClanName);
}
*/
void SetClanOrder(i3GuiEditBox * pControl, INT32 nCurOrder, INT32 nPrevOrder)
{
	/*char szTemp[MAX_STRING_COUNT] = "";

	INT32 nGapOrder = nCurOrder - nPrevOrder;

	if(nGapOrder > 0)
		sprintf_s(szTemp, "%d%s (▲%d)", nCurOrder, GAME_RCSTRING("STBL_IDX_CLAN_LADDER"), i3Math::abs(nGapOrder));
	else if(nGapOrder < 0)
		sprintf_s(szTemp, "%d%s (▼%d)", nCurOrder, GAME_RCSTRING("STBL_IDX_CLAN_LADDER"), i3Math::abs(nGapOrder));
	else
		sprintf_s(szTemp, "%d%s (- %d)", nCurOrder, GAME_RCSTRING("STBL_IDX_CLAN_LADDER"), 0);

	pControl->SetText(szTemp);*/

	pControl->SetText(GAME_RCSTRING("STBL_IDX_CLAN_PLEASE_CHECK_WEB"));/*PB 홈페이지에서 확인해 주십시오*/
}

void SetClanPoint(i3GuiEditBox * pControl, REAL32 fPoint)
{
	I3ASSERT( pControl != nullptr);

	i3::stack_wstring wstr_temp;
	i3::sprintf(wstr_temp, L"%d", (INT32) (fPoint + 0.5f));	//	클랜 포인트를 소수점 반올림해서 정수로 변환
	pControl->SetText(wstr_temp);
}

bool SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pControl, type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
}

bool SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pSprite, nSpriteIdx, type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
}

bool SetClanMarkTexEx(i3GuiStatic * pControl, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pControl, type, 
									CLANMARK_FRAME_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_FRAME_COLOR_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_COLOR_IDX(nClanMarkFlag));
}

bool SetClanMarkTexEx(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetClanMarkTex( pSprite, nSpriteIdx, type, 
									CLANMARK_FRAME_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_IMAGE_IDX(nClanMarkFlag),
									CLANMARK_FRAME_COLOR_IDX(nClanMarkFlag),
									CLANMARK_CONTENT_COLOR_IDX(nClanMarkFlag));
}

bool SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetDefaultClanMarkTex( pControl, type);
}

bool SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->SetDefaultClanMarkTex( pSprite, nSpriteIdx, type);
}

bool MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, 
					 INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, INT32 u, INT32 v)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->MakeClanMarkTex( pOutTex, type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx, u, v);
}

bool MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 u, INT32 v)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->MakeClanMarkTex( pOutTex, type, markFlag, u, v);
}

bool MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE LayerType, 
							INT32 nImageIdx, INT32 nColorIdx, INT32 u, INT32 v)
{
	I3ASSERT( g_pClanMarkManager != nullptr);

	return g_pClanMarkManager->MakeClanMarkShapeTex( pOutTex, type, LayerType, nImageIdx, nColorIdx, u, v);
}

bool SetClanMarkTex(i3GuiStatic * pControl, SLOT_INFO * pInfo)
{
	if( pInfo && pControl)
	{
		if( pInfo->_clanidx > 0 && pInfo->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
		{	
			UINT32 flag = pInfo->_clanMark;

			pControl->SetEnable(true);

			SetClanMarkTexEx( pControl, CLAN_MARK_SMALL, flag);

			return true;
		}
		else
		{
			pControl->SetEnable(false);

			return false;
		}
	}

	return false;
}

INT32 CompareString( const char * str1, const char * str2, bool bAsc)
{	
	I3ASSERT( str1 != nullptr && str2 != nullptr);

	INT32 comp = i3::generic_icompare( str1, str2);

	if(bAsc)
		return (comp == 0 ? 0 : (comp > 0 ? 1 : -1));	//	오름차순
	else
		return (comp == 0 ? 0 : (comp > 0 ? -1 : 1));	//	내림차순
}

void GetBattleTeamName(i3::wstring& out, INT32 idx, bool bIncludeTeamString)
{
	i3::sprintf(out, GAME_STRING("STBL_IDX_CLAN_BATTLE_SQUAD "), idx+1);/*%02d 분대*/
}

	//INT32 cnt = idx / maxNamingCount;
	//INT32 mod = idx % maxNamingCount;

	//switch( mod)
	//{
	//case 0:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_0");/*알파*/		break;
	//case 1:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_1");/*브라보*/	break;
	//case 2:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_2");/*찰리*/		break;
	//case 3:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_3");/*델타*/		break;
	//case 4:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_4");/*에코*/		break;
	//case 5:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_5");/*폭스*/		break;
	//case 6:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_6");/*골프*/		break;
	//case 7:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_7");/*호텔*/		break;
	//case 8:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_8");/*인디아*/	break;
	//case 9:		pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_9");/*줄리엣*/	break;
	//case 10:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_10");/*킬로*/		break;
	//case 11:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_11");/*러브*/		break;
	//case 12:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_12");/*마이크*/	break;
	//case 13:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_13");/*노멤버*/	break;
	//case 14:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_14");/*오스카*/	break;
	//case 15:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_15");/*파파*/		break;
	//case 16:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_16");/*퀸*/		break;
	//case 17:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_17");/*로미오*/	break;
	//case 18:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_18");/*슈가*/		break;
	//case 19:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_19");/*탱고*/		break;
	//case 20:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_20");/*언클*/		break;
	//case 21:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_21");/*빅터*/		break;
	//case 22:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_22");/*위스키*/	break;
	//case 23:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_23");/*엑스레이*/ break;
	//case 24:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_24");/*양키*/		break;
	//case 25:	pszNaming = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_NAMING_25");/*줄루*/		break;
	//}

	//if(bIncludeTeamString)
	//{
	//	if( cnt == 0)
	//		sprintf_s(pBuf, sizeBufMax, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_WITHOUT_NUM"), pszNaming, cnt);/*%s 팀*/
	//	else if( cnt < 10)
	//		sprintf_s(pBuf, sizeBufMax, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_WITH_NUM2"), pszNaming, cnt);/*%s%02d 팀*/
	//	else
	//		sprintf_s(pBuf, sizeBufMax, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_TEAM_WITH_NUM1"), pszNaming, cnt);/*%s%d 팀*/
	//}
	//else
	//{
	//	if( cnt == 0)
	//		sprintf_s(pBuf, sizeBufMax, "%s", pszNaming, cnt);
	//	else if( cnt < 10)
	//		sprintf_s(pBuf, sizeBufMax, "%s%02d", pszNaming, cnt);
	//	else
	//		sprintf_s(pBuf, sizeBufMax, "%s%d", pszNaming, cnt);
	//}

//}
/*
INT32 ContainString( const char * pStr1, const char * pStr2)
{    
	INT32 Len;
	INT32 Idx = 0, StartIdx = 0;

	Len = i3::generic_string_size( pStr2);

	while( *pStr1)
	{
		char c1 = ((*pStr1 >= 'a' && *pStr1 <= 'z') ? (*pStr1 - ('a' - 'A')) : *pStr1);
		char c2 = ((pStr2[Idx] >= 'a' && pStr2[Idx] <= 'z') ? (pStr2[Idx] - ('a' - 'A')) : pStr2[Idx]);

		if( c1 == c2)
		{
			Idx++;

			if( Idx >= Len)
			{
				return StartIdx + 1 - Len;
			}
		}
		else
		{
			if( Idx != 0 )
			{
				pStr1--;
			}
			Idx = 0;
		}

		pStr1++;
		StartIdx++;
	}

	return -1;
}
*/

const wchar_t* GetClanBattleMatchType(CLAN_MATCH_TYPE type)
{
	switch( type)
	{
		//case CLAN_MATCH_4:			return "4 VS 4";	// revision 37397
		case CLAN_MATCH_5:			return L"5 VS 5";
		case CLAN_MATCH_6:			return L"6 VS 6";
		case CLAN_MATCH_7:			return L"7 VS 7";
		case CLAN_MATCH_8:			return L"8 VS 8";
		case CLAN_MATCH_UNKNOWN:	
		default:
			return L"0 VS 0";
	}
}

UINT8 GetClanBattleMatchMember(CLAN_MATCH_TYPE type)
{
	switch( type)
	{
		case CLAN_MATCH_4:			return 4;
		case CLAN_MATCH_5:			return 5;
		case CLAN_MATCH_6:			return 6;
		case CLAN_MATCH_7:			return 7;
		case CLAN_MATCH_8:			return 8;
		case CLAN_MATCH_UNKNOWN:	
		default:
			return 0;
	}
}

CLAN_MATCH_TYPE	ConvertClanBattleMatchType( INT32 nMax)
{
	switch( nMax)
	{
	//case 4:		return CLAN_MATCH_4;
	case 5:		return CLAN_MATCH_5;
	case 6:		return CLAN_MATCH_6;
	case 7:		return CLAN_MATCH_7;
	case 8:		return CLAN_MATCH_8;
	}

	return CLAN_MATCH_UNKNOWN;
}


bool	CanClanInvite()
{
	if ( ! g_pConfigEx->GetCommunity().Enable_Note )
		return false;

	//	클랜과 쪽지 기능이 선행되어야 합니다.
	if ( ! g_pConfigEx->GetClan().Enable )
		return false;

	return true;
}

bool	CanClanMemberSendNoteAll()
{
	//
	// 이전 define : ENABLE_CLAN_MEMBER_SEND_NOTE
	// 이 define을 데이터로 변경한 것 입니다.
	if ( ! g_pConfigEx->GetCommunity().Enable_Note )
		return false;

	//	클랜과 쪽지 기능이 선행되어야 합니다.
	if ( ! g_pConfigEx->GetClan().Enable )
		return false;

	return true;
}

