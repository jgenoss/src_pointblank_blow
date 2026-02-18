#include "pch.h"

#if JuuL_0

#include "ToolTip.h"

#include "GameGUI.h"

CToolTip::CToolTip()
{
	m_idxCurTooltip		= TIP_NULL;
	m_bEnableTooltip	= FALSE;
	m_pTooltipText		= NULL;
	m_pTooltipTexture	= NULL;
	m_pTooltipTexture2	= NULL;
	m_pTooltipSprite	= NULL;
	m_pTooltipSprite2	= NULL;

	m_CurrentGuiResolutionHeight = 0.f;
	m_CurrentGuiResolutionWidth = 0.f;
	m_TooltipHeightRatio = 1.f;
	m_TooltipHeight = 0.f;
	m_TooltipWidthRatio = 1.f;
	m_TooltipWidth = 0.f;
}


CToolTip::~CToolTip()
{

	if (m_pTooltipText)
	{
		g_pFramework->getMessagBoxLayer()->RemoveChild(m_pTooltipText);
		I3_SAFE_RELEASE(m_pTooltipText);
	}

	if (m_pTooltipSprite)
	{
		g_pFramework->getMessagBoxLayer()->RemoveChild(m_pTooltipSprite);
		I3_SAFE_RELEASE(m_pTooltipSprite);
	}
	if (m_pTooltipSprite2)
	{
		g_pFramework->getMessagBoxLayer()->RemoveChild(m_pTooltipSprite2);
		I3_SAFE_RELEASE(m_pTooltipSprite2);
	}

	I3_SAFE_RELEASE(m_pTooltipTexture);
	I3_SAFE_RELEASE(m_pTooltipTexture2);

}

const REAL32 CToolTip::_TooltipDefalultWidth = 56.f;
const REAL32 CToolTip::_TooltipDefalultHeight = 30.f;

void CToolTip::Init()
{

	REAL32 fontSize = (REAL32) GAME_FONT_DEFAULT_SIZE * i3GuiRoot::getGuiResolutionHeight();

	m_idxCurTooltip = 0;
	m_bEnableTooltip = FALSE;

	m_pTooltipTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Main/UI_Source03.i3i");
	m_pTooltipTexture2 = g_pFramework->GetResourceManager()->LoadTexture("Gui/Main/UI_Source01.i3i");

	m_pTooltipSprite = i3Sprite2D::new_object();
	m_pTooltipSprite->Create(1, TRUE, FALSE);
	m_pTooltipSprite->SetTexture(m_pTooltipTexture);
	m_pTooltipSprite->SetTextureCoord(0, 319.0f, 396.0f, 374.0f, 425.0f);
	m_pTooltipSprite->SetEnable(FALSE);

	m_pTooltipSprite2 = i3Sprite2D::new_object();
	m_pTooltipSprite2->Create(1, TRUE, FALSE);
	m_pTooltipSprite2->SetTexture(m_pTooltipTexture2);
	m_pTooltipSprite2->SetTextureCoord(0, 472.0f, 136.0f, 494.0f, 172.0f);
	m_pTooltipSprite2->SetEnable(FALSE);

	g_pFramework->getMessagBoxLayer()->AddChild(m_pTooltipSprite);
	g_pFramework->getMessagBoxLayer()->AddChild(m_pTooltipSprite2);

	m_pTooltipText = i3TextNodeDX2::new_object();
#if USE_VERTEX_BANK
	m_pTooltipText->Create( GetDefaultFontName(), MAX_STRING_COUNT, (INT32)fontSize, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, TRUE);
#else
	m_pTooltipText->Create( GetDefaultFontName(), MAX_STRING_COUNT, (INT32)fontSize, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, FALSE);
#endif
	m_pTooltipText->SetColor(GameGUI::GetColor(GCT_FOCUS));
	m_pTooltipText->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTooltipText->SetEnable(FALSE);

	g_pFramework->getMessagBoxLayer()->AddChild(m_pTooltipText);

	CalcTooltipSize();
}

void CToolTip::Set( UINT32 idxTooltip )
{


	if (m_idxCurTooltip != idxTooltip)
	{
		m_idxCurTooltip = idxTooltip;

		const char * strTooltip = "";

		switch(m_idxCurTooltip)
		{
		case TIP_CLAN			:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_CLAN");				/*클랜*/		break;
		case TIP_PROFILE		:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_PROFILE");			/*임무/호칭*/	break;
		case TIP_INVENTORY		:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_INVENTORY");		/*인벤토리*/	break;
		case TIP_SHOP			:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_SHOP");				/*상점*/		break;
		case TIP_COMMUNITY		:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_COMMUNITY");		/*커뮤니티*/	break;
		case TIP_OPTION			:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_OPTION");			/*환경설정*/	break;
		case TIP_EXIT			:	strTooltip = GAME_STRING("STR_TBL_GUI_READYROOM_EXIT");				/*종료*/		break;
		case TIP_CHANGE_SERVER	:	strTooltip = GAME_STRING("STR_TBL_GUI_LOBBY_CHANGE_SERVER");		/*서버변경*/	break;
		case TIP_ENTER_ROOM		:	strTooltip = GAME_STRING("STR_TBL_GUI_LOBBY_ENTER_ROOM_TEXT");		/*입장하기*/	break;
		case TIP_REFRESH		:	strTooltip = GAME_STRING("STR_TBL_GUI_LOBBY_REFRESH_TEXT");			/*새로고침*/	break;
		case TIP_CREATE_ROOM	:	strTooltip = GAME_STRING("STR_TBL_GUI_LOBBY_CREATE_ROOM_TEXT");		/*방만들기*/	break;
		case TIP_TEAM_BALANCE	:	strTooltip = GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_TOOLTIP");	/*팀발란스*/	break;
		case TIP_VIDEO_CAPTURE	:	strTooltip = GAME_STRING("STR_TBL_GUI_LOBBY_VIDEO_CAPTURE_TOOLTIP");	/*팀발란스*/	break;
		}

		m_pTooltipText->SetText(strTooltip);

		CalcTooltipSize();
		CalcPosition();
	}

	m_bEnableTooltip = TRUE;
}



void CToolTip::Update( REAL32 rDeltaSeconds )
{
	if (m_bEnableTooltip)
	{
		CalcPosition();
		if((m_idxCurTooltip == TIP_TEAM_BALANCE) || (m_idxCurTooltip == TIP_VIDEO_CAPTURE))
			m_pTooltipSprite2->SetEnable(TRUE);
		else
			m_pTooltipSprite->SetEnable(TRUE);
		m_pTooltipText->SetEnable(TRUE);		
		m_bEnableTooltip = FALSE;
	}
	else
	{
		m_pTooltipSprite->SetFlip(0, FALSE, FALSE);
		m_pTooltipSprite2->SetFlip(0, FALSE, FALSE);
		m_pTooltipSprite->SetEnable(FALSE);
		m_pTooltipSprite2->SetEnable(FALSE);
		m_pTooltipText->SetEnable(FALSE);		
	}
}

void CToolTip::CalcTooltipSize()
{
// 툴팁 크기 변경 하기 m_TooltipWidth에 원하는 크기를 넣어 주면 된다.
#if defined(LOCALE_RUSSIA)
	m_TooltipWidth = 156.f;
	m_TooltipHeight = 30.f;
#else 
	m_TooltipWidth = _TooltipDefalultWidth;
	m_TooltipHeight =_TooltipDefalultHeight;
#endif
	if((m_idxCurTooltip == TIP_TEAM_BALANCE) || (m_idxCurTooltip == TIP_VIDEO_CAPTURE))
	{
		m_TooltipWidth = 300.0f;
		m_TooltipHeight = 70.f;	
#if defined(LOCALE_RUSSIA)
		m_TooltipWidth = 440.0f;
#else if(LOCALE_LATINAMRICA)
		m_TooltipWidth = 440.0f;
#endif
	}
	m_TooltipWidthRatio = m_TooltipWidth / _TooltipDefalultWidth;
	m_TooltipHeightRatio = m_TooltipHeight / _TooltipDefalultHeight;

	m_CurrentGuiResolutionWidth = i3GuiRoot::getGuiResolutionWidth();
	m_CurrentGuiResolutionHeight = i3GuiRoot::getGuiResolutionHeight();

	REAL32 width = m_TooltipWidth * i3GuiRoot::getGuiResolutionWidth();
	REAL32 height = m_TooltipHeight * i3GuiRoot::getGuiResolutionHeight();

	m_pTooltipSprite->SetRect(0, 0.0f, 0.0f, width, height);
	m_pTooltipSprite2->SetRect(0, 0.0f, 0.0f, width, height);
	m_pTooltipText->setSize((UINT32)width, (UINT32)height);


}

void CToolTip::CalcPosition()
{
	REAL32 widthOffset = 13.0f * i3GuiRoot::getGuiResolutionWidth() * m_TooltipWidthRatio;
	REAL32 heightOffset = 32.0f * i3GuiRoot::getGuiResolutionHeight() * m_TooltipHeightRatio;

	REAL32 rX = g_pFramework->getMouse()->GetX() - widthOffset;
	REAL32 rY = g_pFramework->getMouse()->GetY() - heightOffset;
	REAL32 rWidth = 0;
	REAL32 rHeight = 0;
	if((m_idxCurTooltip == TIP_TEAM_BALANCE) || (m_idxCurTooltip == TIP_VIDEO_CAPTURE))
	{
		rWidth = m_pTooltipSprite2->GetSize(0)->x;
		rHeight = m_pTooltipSprite2->GetSize(0)->y;
	}
	else
	{
		rWidth = m_pTooltipSprite->GetSize(0)->x;
		rHeight = m_pTooltipSprite->GetSize(0)->y;
	}

	BOOL flipX = FALSE;
	BOOL flipY = FALSE;

	if (rX + rWidth >  g_pViewer->GetViewWidth())
	{
		rX = rX - (29.0f * i3GuiRoot::getGuiResolutionWidth() * m_TooltipWidthRatio);
		flipX = TRUE;
	}

	if (rY - rHeight < 0.0f)
	{
		rY = rY + (34.0f * i3GuiRoot::getGuiResolutionHeight() );
		flipY = TRUE;
	}

	m_pTooltipSprite->SetFlip(0, flipX, flipY);				
	m_pTooltipSprite->SetRect(0, rX, rY, rWidth, rHeight);

	m_pTooltipSprite2->SetFlip(0, flipX, flipY);				
	m_pTooltipSprite2->SetRect(0, rX, rY, rWidth, rHeight);

	m_pTooltipText->setPos((UINT32)rX, (UINT32)rY);

}

void CToolTip::OnChangeVideo()
{
	REAL32 width = 56.0f * i3GuiRoot::getGuiResolutionWidth();
	REAL32 height = 30.0f * i3GuiRoot::getGuiResolutionHeight();
	REAL32 fontSize = (REAL32) GAME_FONT_DEFAULT_SIZE * i3GuiRoot::getGuiResolutionHeight();

	m_pTooltipSprite->SetRect(0, 0.0f, 0.0f, width, height);

	if (m_pTooltipText)
	{
		g_pFramework->getMessagBoxLayer()->RemoveChild(m_pTooltipText);
		I3_SAFE_RELEASE(m_pTooltipText);
	}

	m_pTooltipText = i3TextNodeDX2::new_object();
#if USE_VERTEX_BANK
	m_pTooltipText->Create( GetDefaultFontName(), MAX_STRING_COUNT, (INT32)fontSize, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, TRUE);
#else
	m_pTooltipText->Create( GetDefaultFontName(), MAX_STRING_COUNT, (INT32)fontSize, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, FALSE);
#endif
	m_pTooltipText->setSize((UINT32)width, (UINT32)height);	
	m_pTooltipText->SetColor(GameGUI::GetColor(GCT_FOCUS));
	m_pTooltipText->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTooltipText->SetEnable(TRUE);

	g_pFramework->getMessagBoxLayer()->AddChild(m_pTooltipText);
}
#endif