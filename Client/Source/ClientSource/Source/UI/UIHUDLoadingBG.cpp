#include "pch.h"
#include "UIHUDLoadingBG.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UIUtil.h"
#include "i3Base/string/ext/utf16_to_mb.h"

namespace
{
	UIHUDLoadingBG* g_this = 0;
}

UIHUDLoadingBG* UIHUDLoadingBG::instance()
{
	return g_this;
}

LuaFuncReg UIHUDLoadingBG_Glue[] = {
	{nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UIHUDLoadingBG);//, UIHUDBase);

UIHUDLoadingBG::UIHUDLoadingBG()
{
	g_this			= this;
	m_visible		= true;
	m_pFrame		= nullptr;
	m_pLoading		= nullptr;
	m_pProgressbar	= nullptr;
	m_pBottomImg	= nullptr;
	m_iTeam			= 0;
	m_StageType		= STAGE_MODE_NA;
	m_sTeam.clear();
	m_pMsg1			= nullptr;
	m_pMsg2			= nullptr;
}

UIHUDLoadingBG::~UIHUDLoadingBG()
{
}

/*virtual*/ void UIHUDLoadingBG::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);

	m_pFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd");
	I3ASSERT( m_pFrame != nullptr);

	m_pProgressbar = (i3UIProgressBar*)GetScene(0)->FindChildByName( "i3UIProgressBar" );
	I3ASSERT( m_pProgressbar != nullptr);

	m_pBottomImg = (i3UIImageBoxEx*)GetScene(0)->FindChildByName( "i3UIImageBox1" );
	I3ASSERT( m_pBottomImg != nullptr);
	
	m_pFrameMsg = (i3UIFrameWnd*)GetScene(0)->FindChildByName( "i3UIFrameWndMsg" );
	I3ASSERT( m_pFrameMsg != nullptr);

	m_pMsg1 = (i3UIStaticText*)GetScene(0)->FindChildByName( "i3UIStaticTextMsg1" );
	m_OrgSize = *m_pMsg1->getSize();
	I3ASSERT( m_pFrameMsg != nullptr);

	m_pMsg2 = (i3UIStaticText*)GetScene(0)->FindChildByName( "i3UIStaticTextMsg2" );
	I3ASSERT( m_pFrameMsg != nullptr);

}
void	UIHUDLoadingBG::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate(pParent);

	// Load Scene
	AddScene("Scene/HUD/PBRe_Ingame_Loading.i3UIs", UIHUDLoadingBG_Glue, true, false);
}

bool	UIHUDLoadingBG::OnEntranceStart( void * pArg1, void * pArg2)
{
	if (UIBase::OnEntranceStart(pArg1, pArg2) == false ) return false;

	SetTexture();
	
	INT32 icross = 0;

	if (m_StageType == STAGE_MODE_TUTORIAL) // 튜토리얼
	 	icross = 1; 
	else if (m_StageType == STAGE_MODE_CROSSCOUNT)
		icross = 2;
	else if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE)) // AI모드 
		icross = 3; 

	m_pProgressbar->setProgress(0.f);

	i3::rc_wstring wstrName;

	LuaState * L = _CallLuaFunction( "SetWorld");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, icross);
		i3Lua::PushInteger( L, m_iTeam);
		//GetStageMapName(wstrName);
		GetMapLabel(MAP_STRING_NAME, wstrName);
		i3Lua::PushStringUTF16To8( L, wstrName);

		GetMapLabel(MAP_STRING_MISSION, wstrName);
		i3Lua::PushStringUTF16To8( L, wstrName);

		GetMapLabel(MAP_STRING_TEAM, wstrName);
		i3Lua::PushStringUTF16To8( L, wstrName);

		GetMapLabel(MAP_STRING_DESCRIPTION, wstrName);
		i3Lua::PushStringUTF16To8( L, wstrName);

		GetStageTip(wstrName);
		i3Lua::PushStringUTF16To8( L, wstrName);
		_EndLuaFunction( L, 7);
	}


	return true;
}

void UIHUDLoadingBG::GetStageMapName( i3::rc_wstring& out_wstrMapName)
{
	out_wstrMapName.clear();
	const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();
	i3::mb_to_utf16(pszStageName, out_wstrMapName);
	//out_wstrMapName = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
}

void UIHUDLoadingBG::GetStageTip( i3::rc_wstring& out_wstrMapName)
{
	out_wstrMapName.clear();
	char szTemp[256];
	INT32 iRand = 0;

	iRand = i3Math::Rand() % 13;
	if (iRand > 3) {
		_snprintf(szTemp, 256, "STBL_IDX_TIP_LOADING_SINGLE_%d", iRand - 4);
	}
	else {
		_snprintf(szTemp, 256, "STBL_IDX_TIP_LOADING_DUAL_%d", iRand);
	}

	if (GAME_RCSTRING(szTemp).size() != 0)
	{
		 out_wstrMapName = GAME_RCSTRING(szTemp);
	}
}

void UIHUDLoadingBG::GetStageMission( i3::rc_wstring& out_wstrMapName)
{
	out_wstrMapName.clear();
	char szTemp[256] = {0,};
	char sTeam[256] = {0,};

	if( !(BattleSlotContext::i()->getMySlotIdx()%2))/*Red팀*/
	{
		i3::safe_string_copy( sTeam, "RED", sizeof( sTeam));
	}
	else
	{
		i3::safe_string_copy( sTeam, "BLUE", sizeof( sTeam));
	}

	switch( m_StageType)
	{
	case STAGE_MODE_DEATHMATCH:
	case STAGE_MODE_ANNIHILATION:
	case STAGE_MODE_CROSSCOUNT:
		_snprintf(szTemp, 256, "STR_STAGE_LOADING__MISSION_TYPE_ANNIHILATE_DEATHMATCH");
		break;
	case STAGE_MODE_DESTROY:
		if( MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_BREAKDOWN )
			_snprintf(szTemp, 256, "STR_STAGE_LOADING_%s_MISSION_TYPE_DESTROY_BREAKDOWN", sTeam);
		else
			_snprintf(szTemp, 256, "STR_STAGE_LOADING_%s_MISSION_TYPE_DESTROY_HELISPOT", sTeam);
		break;
	case STAGE_MODE_BOMB:
		_snprintf(szTemp, 256, "STR_STAGE_LOADING_%s_MISSION_TYPE_BOMB", sTeam);
		break;
	case STAGE_MODE_DEFENCE:
		_snprintf(szTemp, 256, "STR_STAGE_LOADING_%s_MISSION_TYPE_DEFENCE", sTeam);
		break;
	case STAGE_MODE_ESCAPE:
		_snprintf(szTemp, 256, "STR_STAGE_LOADING_%s_MISSION_TYPE_DINOSOUR_ESCAPE", sTeam);
		break;
	case STAGE_MODE_CONVOY:
		_snprintf(szTemp, 256, "STR_STAGE_LOADING_%s_MISSION_TYPE_CONVOY", sTeam);
		break;
	case STAGE_MODE_TUTORIAL:
		_snprintf(szTemp, 256, "STR_STAGE_LOADING_MISSION_TYPE_TUTORIAL");
		break;
	case STAGE_MODE_RUN_AWAY:
	default:
		break;
	}

	if( GAME_RCSTRING(szTemp).size() != 0 ) 
	{
		out_wstrMapName = GAME_RCSTRING(szTemp);
	}
	else
	{
		out_wstrMapName = L"";
	}
}

void	UIHUDLoadingBG::OnEntranceEnd()
{
	UIBase::OnEntranceEnd();

}

void	UIHUDLoadingBG::OnExitEnd()
{
	UIBase::OnExitEnd();
}

// 활성화된 상태에서만 야상입력 가능..
bool UIHUDLoadingBG::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIBase::OnKeyInput( pInputMng) )
		return true;

	return false;
}


/*virtual*/ void UIHUDLoadingBG::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);
}

void UIHUDLoadingBG::SetVisible(bool visible)
{
	GetScene(0)->EnableScene( visible );
	m_visible = visible;
}


void UIHUDLoadingBG::SetTexture()
{
	char conv[MAX_PATH];
	const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();
	m_StageType		= MyRoomInfoContext::i()->getStage()->GetStageMode();

	if( !(BattleSlotContext::i()->getMySlotIdx()%2))/*Red팀*/
	{
		m_sTeam = L"RED";

		m_iTeam = 1;
	}
	else
	{
		m_sTeam = L"BLUE";

		m_iTeam = 0;
	}

	if( pszStageName != nullptr)
	{
		if(  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEATHMATCH))
		{
			i3::sprintf( conv, "World/%s/Images_NoText/load_%s_DM_%s.i3i", pszStageName, pszStageName, m_sTeam);

			if( i3FileStream::isFile( conv) == false)
			{
				i3::sprintf( conv, "World/%s/Images_NoText/load_%s_%s.i3i", pszStageName, pszStageName, m_sTeam);

				if( i3FileStream::isFile( conv) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "not exist file %s!!", conv);
					return;
				}
			}
		}
		else
		{
			i3::sprintf( conv, "World/%s/Images_NoText/load_%s_%s.i3i", pszStageName, pszStageName, m_sTeam);

			if( i3FileStream::isFile( conv) == false)
			{
				I3PRINTLOG(I3LOG_FATAL,  "no exist file %s!!", conv);
				return;
			}
		}

		i3Texture * pTexture = g_pFramework->GetResourceManager()->LoadTexture( conv);

		if(m_pLoading == nullptr)
		{
			m_pLoading = i3UIImageBoxEx::new_object_ref();
			m_pFrame->AddChildToHead(m_pLoading);
		}

		m_pLoading->DeleteShapeAll();
		m_pLoading->CreateBaseSceneObjects();
		m_pLoading->AddTexture(pTexture, 1024.f, 768.f);
		m_pLoading->SetCurrentImage(0);
		m_pLoading->prepareSprites();

		I3_MUST_RELEASE(pTexture);
	}
	_Resize();
}

void UIHUDLoadingBG::_Resize( void)
{
	if( m_pLoading == nullptr)
		return;

	i3UIManager * pMng = i3UI::getManager();

	// 배경
	//REAL32 w = (REAL32)g_pViewer->GetViewHeight() * 16.f / 9.f;
	REAL32 scaleh = (REAL32)g_pViewer->GetViewHeight() / (REAL32)pMng->getActualHeight();
	REAL32 scalew = (REAL32)g_pViewer->GetViewWidth() / (REAL32)pMng->getActualWidth();

	REAL32 Offsetx = 0.f;

	m_pLoading->SetShapeSize( 1024 * scalew, 768.f * scaleh);

	{
		REAL32 rateX = pMng->getScreenStretchRatioX();
		REAL32 rateY = pMng->getScreenStretchRatioY();
		//REAL32 offset = (REAL32) pMng->getDisplayOffsetX();
		//REAL32 x = ((g_pViewer->GetViewWidth() - w) * 0.5f - (offset)) * rateX;
		Offsetx =  -(REAL32)pMng->getDisplayOffsetX() * rateX;
		m_pLoading->setPos( Offsetx, -(REAL32)pMng->getDisplayOffsetY() * rateY);
	}

	VEC2D* tSize = m_pBottomImg->getSize();
	VEC3D* tPos = m_pBottomImg->getPos();
	
	tSize->x = 1024 * scalew; 
	tPos->x = Offsetx;
	m_pBottomImg->setSize(tSize);
	m_pBottomImg->setPos(tPos);

	tSize = m_pProgressbar->getSize();
	tPos = m_pProgressbar->getPos();
	tSize->x = (1024 * scalew) - (72 * scalew); 
	tPos->x = Offsetx + (36 * scalew);
	m_pProgressbar->setSize(tSize);
	m_pProgressbar->setPos(tPos);

	tPos = m_pFrameMsg->getPos();
	tPos->x = Offsetx + (39 * scalew);
	m_pFrameMsg->setPos(tPos);

	tSize = m_pMsg1->getSize();
	tSize->x = m_OrgSize.x * scalew;
	tSize->y = tSize->y * scaleh;
	m_pMsg1->setSize(tSize);

	tSize = m_pMsg2->getSize();
	tSize->x = m_OrgSize.x * scalew;
	tSize->y = tSize->y * scaleh;
	m_pMsg2->setSize(tSize);
}

void UIHUDLoadingBG::GetMapString(i3::rc_wstring& _outstr, char* _str)
{
	i3RegKey * pRoot = Config::FindKey("Stage");
	I3ASSERT( pRoot != nullptr);

	i3StageInfo* pInfo = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex());

	i3RegKey * pKey = Config::FindKey( pInfo->GetName(), pRoot);
	I3ASSERT(pKey != nullptr);

	if( pKey != nullptr)
	{
		i3::rc_wstring	strName;
		FIND_REG_DATA( pKey, (const char*)_str, strName);
		_outstr = strName;
	}

}

void UIHUDLoadingBG::GetMapLabel(MAP_STRING idx, i3::rc_wstring& _outstr)
{
	switch(idx)
	{
	case MAP_STRING_NAME:
		GetMapString(_outstr, "_LoadingName");
		break;
	case MAP_STRING_MISSION:
		GetStageMission(_outstr);
		break;
	case MAP_STRING_TEAM:
	{
		if (m_StageType == STAGE_MODE_ESCAPE)
		{
			if (m_iTeam == 1)
				_outstr = GAME_RCSTRING("STR_STAGE_LOADING_CAMP_DINOSOUR");
			else
				_outstr = GAME_RCSTRING("STR_STAGE_LOADING_CAMP_HUMAN");
		}
		else
			_outstr = m_sTeam;
	}
	break;
	case MAP_STRING_DESCRIPTION:
		GetMapString(_outstr, "Description");
		break;
	default:
		I3ASSERT_0;
	}
}

void UIHUDLoadingBG::UpdateProgress(REAL32 rValue)
{
	m_pProgressbar->setProgress(rValue);
}

