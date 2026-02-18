#include "pch.h"

#if 지워질_것들

#include "BattleChatBox.h"
#include "GlobalVariables.h"
#include "BattleHud.h"
#include "GameMaster.h"
#include "ConfigRenderBattle.h"

I3_CLASS_INSTANCE(CBattleChatBox);//, i3GameObjBase);

CBattleChatBox::CBattleChatBox()
{
	m_pSceneNode = i3Node::new_object();

	m_pTexture = NULL;
	m_pSpriteChatOutput = NULL;

	m_pSpriteChat = NULL;
	m_pCaretSprite = NULL;
	m_pSpriteChatChannel = NULL;
	m_pTextBox = NULL;
	m_pTextChat = NULL;
	m_pTextTarget = NULL;
	for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++) m_pTextChatChannel[i] = NULL;

	m_bChatEnableWithGUI = FALSE;
	m_bChatScrollEnable = FALSE;
	m_bChatEnable = FALSE;

	m_pIME = NULL;
	m_CurrentIMEMode = IME_MODE_ALPHANUMERIC;

	m_nChatType = CHATTING_TYPE_ALL;
	m_nOldChatType = CHATTING_TYPE_ALL;
	m_bOnceChatType = FALSE;
	m_bOnceChatTypeAll = FALSE;

	m_nChatBoxOffsetHeight = 0;

	m_fChatScrollTimeDelta = 0.0f;
	m_CaretWidth = 0.0f;
	m_CaretHeight = 0.0f;
	m_CaretLocalTime = 0.f;

	m_bCapturedMouse = FALSE;
	m_nCurrentChatBufferType = CHATTING_TYPE_ALL;	
}

CBattleChatBox::~CBattleChatBox()
{
	I3_SAFE_RELEASE(m_pTextBox);
	I3_SAFE_RELEASE(m_pTextChat);
	I3_SAFE_RELEASE(m_pTextTarget);

	I3_SAFE_RELEASE( m_pIME);

	I3_SAFE_RELEASE(m_pSpriteChat);
	I3_SAFE_RELEASE(m_pCaretSprite);
	I3_SAFE_RELEASE(m_pSpriteChatChannel);
	I3_SAFE_RELEASE(m_pSpriteChatOutput);
	for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++) I3_SAFE_RELEASE(m_pTextChatChannel[i]);

	I3_SAFE_RELEASE(m_pTexture);
}

BOOL CBattleChatBox::Create(i3Node* pParentNode,BOOL bEnable)
{
	m_pTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);

	m_pSpriteChatOutput = i3Sprite2D::new_object();
	m_pSpriteChatOutput->Create(ST_MAX, TRUE, FALSE);
	m_pSpriteChatOutput->SetTexture(m_pTexture);
	
	for(UINT32 i = 0; i < ST_MAX; i++)
	{
		I3RECT rect;

		_GetTextrueRect(i, &rect);
		m_pSpriteChatOutput->SetTextureCoord(i, rect.left, rect.top, rect.right, rect.bottom);

		// ! praptor - right/bottom을 width/height로 사용합니다.
		_GetSpriteRect(i, &rect);
		m_pSpriteChatOutput->SetRect(i, rect.left, rect.top, rect.right, rect.bottom);
	}

	m_pSpriteChatOutput->SetEnable(FALSE);
	GetNode()->AddChild(m_pSpriteChatOutput);


	// Chat
	UINT32 nScreenHeight = 0;

	{		
		INT32 nPosY = 0;
		if (g_pViewer != NULL)
		{
			nScreenHeight = g_pFramework->GetViewer()->GetViewHeight(); 

			nPosY = (INT32)(nScreenHeight - 38*i3GuiRoot::getHeightRatioBy768());
		}
		
		m_pSpriteChat = i3Sprite2D::new_object();	
		m_pSpriteChat->SetTexture( m_pTexture );
		m_pSpriteChat->Create(HUD_SPRITE_CHAT_INPUT_COUNT, TRUE, TRUE);
	
		// Chat배경
		m_pSpriteChat->SetEnable( HUD_SPRITE_CHAT_INPUTBOX, TRUE);
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_INPUTBOX, (10.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY, (405.0f)*i3GuiRoot::getHeightRatioBy768(), (28.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChat->SetColor( HUD_SPRITE_CHAT_INPUTBOX, 255,255,255,255);
		m_pSpriteChat->SetTextureCoord( HUD_SPRITE_CHAT_INPUTBOX, (514.0f), (115.0f), (919.0f), (143.0f));

		// Chat Mode
		m_pSpriteChat->SetEnable( HUD_SPRITE_CHAT_MODE, TRUE);
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_MODE, (389.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY+4.0f*i3GuiRoot::getHeightRatioBy768(), (21.0f)*i3GuiRoot::getHeightRatioBy768(), (21.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChat->SetColor( HUD_SPRITE_CHAT_MODE, 255,255,255,255);
		m_pSpriteChat->SetTextureCoord( HUD_SPRITE_CHAT_MODE, (555.0f), (25.0f), (577.0f), (47.0f));

		// Chat Target
		m_pSpriteChat->SetEnable( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, TRUE);
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, (50.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY+4.0f*i3GuiRoot::getHeightRatioBy768(), (5.0f)*i3GuiRoot::getHeightRatioBy768(), (5.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChat->SetColor( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, 255,255,255,255);
		m_pSpriteChat->SetTextureCoord( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, (554.0f), (120.0f), (559.0f), (125.0f));

		m_pSpriteChat->SetEnable( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, TRUE);
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, (50.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY+18.0f*i3GuiRoot::getHeightRatioBy768(), (5.0f)*i3GuiRoot::getHeightRatioBy768(), (5.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChat->SetColor( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, 255,255,255,255);
		m_pSpriteChat->SetTextureCoord( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, (554.0f), (134.0f), (559.0f), (139.0f));

		GetNode()->AddChild(m_pSpriteChat);
	}

	// Chat Caret
	{
		m_pCaretSprite = i3Sprite2D::new_object();
		m_pCaretSprite->Create( 1, FALSE, TRUE);

		m_CaretWidth = 1.0f*i3GuiRoot::getHeightRatioBy768();
		m_CaretHeight = 18.0f*i3GuiRoot::getHeightRatioBy768();

		m_pCaretSprite->SetEnable(	0, FALSE);		
		m_pCaretSprite->SetSize(	0, m_CaretWidth, m_CaretHeight);
		m_pCaretSprite->SetColor(	0, 255, 255, 255, 150);
		
		GetNode()->AddChild(m_pCaretSprite);
	}

	// Chat Channel
	{
		INT32 nPosY = 0;
		if (g_pViewer != NULL)
		{
			nScreenHeight = g_pFramework->GetViewer()->GetViewHeight();

			nPosY = (INT32)(nScreenHeight - (56*i3GuiRoot::getHeightRatioBy768()));
		}
		
		m_pSpriteChatChannel = i3Sprite2D::new_object();	
		m_pSpriteChatChannel->SetTexture( m_pTexture );
		m_pSpriteChatChannel->Create(4, TRUE, TRUE);
	
		// 일반
		m_pSpriteChatChannel->SetEnable( 0, TRUE);
		m_pSpriteChatChannel->SetRect( 0, (12.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY, (41.0f)*i3GuiRoot::getHeightRatioBy768(), (18.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChatChannel->SetColor( 0, 255,255,255,255);
		m_pSpriteChatChannel->SetTextureCoord( 0, (861.0f), (38.0f), (902.0f), (56.0f));

		// 팀
		m_pSpriteChatChannel->SetEnable( 1, TRUE);
		m_pSpriteChatChannel->SetRect( 1, (54.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY, (41.0f)*i3GuiRoot::getHeightRatioBy768(), (18.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChatChannel->SetColor( 1, 255,255,255,255);
		m_pSpriteChatChannel->SetTextureCoord( 1, (861.0f), (38.0f), (902.0f), (56.0f));

		// 클랜
		m_pSpriteChatChannel->SetEnable( 2, TRUE);
		m_pSpriteChatChannel->SetRect( 2, (96.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY, (41.0f)*i3GuiRoot::getHeightRatioBy768(), (18.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChatChannel->SetColor( 2, 255,255,255,255);
		m_pSpriteChatChannel->SetTextureCoord( 2, (861.0f), (38.0f), (902.0f), (56.0f));

		// 귓말 & 답변
		m_pSpriteChatChannel->SetEnable( 3, TRUE);
		m_pSpriteChatChannel->SetRect( 3, (138.0f)*i3GuiRoot::getHeightRatioBy768(), (REAL32)nPosY, (41.0f)*i3GuiRoot::getHeightRatioBy768(), (18.0f)*i3GuiRoot::getHeightRatioBy768());
		m_pSpriteChatChannel->SetColor( 3, 255,255,255,255);
		m_pSpriteChatChannel->SetTextureCoord( 3, (861.0f), (38.0f), (902.0f), (56.0f));

		GetNode()->AddChild( m_pSpriteChatChannel);

		for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++)
		{
			m_pTextChatChannel[i] = i3TextNodeDX2::new_object();
			m_pTextChatChannel[i]->Create(  GetDefaultFontName(), FONT_COUNT_32, (INT32)(GAME_FONT_DEFAULT_SIZE*i3GuiRoot::getHeightRatioBy768()), 0, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextChatChannel[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);	
			m_pTextChatChannel[i]->SetEnable( TRUE);
			m_pTextChatChannel[i]->setPos( (UINT32)((12 + (42*i))*i3GuiRoot::getHeightRatioBy768()), nPosY);
			m_pTextChatChannel[i]->setSize( (UINT32)(41*i3GuiRoot::getHeightRatioBy768()), (UINT32)(18*i3GuiRoot::getHeightRatioBy768()));
			m_pTextChatChannel[i]->SetColor(204, 204, 204, 255);				
			GetNode()->AddChild( m_pTextChatChannel[i]);		

			switch((CHAT_CHANNEL_TYPE)i)
			{
				case CHAT_CHANNEL_TYPE_ALL:			m_pTextChatChannel[i]->SetText(GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_ALL"));/*전체*/		break;
				case CHAT_CHANNEL_TYPE_TEAM:		m_pTextChatChannel[i]->SetText(GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_TEAM"));/*팀*/		break;
				case CHAT_CHANNEL_TYPE_CLAN:		m_pTextChatChannel[i]->SetText(GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_CLAN"));/*클랜*/		break;
				case CHAT_CHANNEL_TYPE_WHISPER:		m_pTextChatChannel[i]->SetText(GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_WHISPER"));/*귓속말*/	break;
			}
		}
	}

	// HUD에서 사용하는 사용자 채팅 박스(다른 사용자들의 채팅 내용이 나오는 곳)
	{
		m_pTextBox = i3TextNodeDX2::new_object();
		m_pTextBox->Create(  GetDefaultFontName(), FONT_COUNT_1024, (INT32)(GAME_FONT_DEFAULT_SIZE*i3GuiRoot::getHeightRatioBy768()), 0, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextBox->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_BOTTOM);	
		m_pTextBox->SetEnable( TRUE);
		m_pTextBox->setPos( (UINT32)(12*i3GuiRoot::getHeightRatioBy768()), (UINT32)(nScreenHeight - (198*i3GuiRoot::getHeightRatioBy768())) );
		m_pTextBox->setSize( (UINT32)(370*i3GuiRoot::getHeightRatioBy768()), (UINT32)(152*i3GuiRoot::getHeightRatioBy768()) );
		m_pTextBox->SetColor(255, 255, 255, 255);	
		m_pTextBox->setAutoWrapEnable( TRUE);
		m_pTextBox->SetTextSpace( 0, (INT32)(5*i3GuiRoot::getHeightRatioBy768()) );
		m_pTextBox->setOutCharacterProcess(TRUE);
		GetNode()->AddChild( m_pTextBox);		
	}

	// HUD에서 사용하는 사용자 채팅창( 사용자가 입력하는곳)
	{
		m_pTextChat = i3TextNodeDX2::new_object();
		m_pTextChat->Create(  GetDefaultFontName(), FONT_COUNT_256,  (INT32)(GAME_FONT_DEFAULT_SIZE*i3GuiRoot::getHeightRatioBy768()), 0, FALSE, FALSE, DEFAULT_QUALITY, TRUE);
		m_pTextChat->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextChat->setAutoWrapEnable( FALSE);
		m_pTextChat->SetEnable( TRUE);
		m_pTextChat->setPos( (UINT32)(60*i3GuiRoot::getHeightRatioBy768()), (UINT32)(nScreenHeight - (29*i3GuiRoot::getHeightRatioBy768())) );
		m_pTextChat->setSize( (UINT32)(330*i3GuiRoot::getHeightRatioBy768()), (UINT32)(32*i3GuiRoot::getHeightRatioBy768()) );
		m_pTextChat->SetColor( 255, 255, 255, 255);			
		GetNode()->AddChild( m_pTextChat);		

		m_pTextTarget = i3TextNodeDX2::new_object();
		m_pTextTarget->Create(  GetDefaultFontName(), FONT_COUNT_32, (INT32)(GAME_FONT_DEFAULT_SIZE*i3GuiRoot::getHeightRatioBy768()), 0, FALSE, FALSE, DEFAULT_QUALITY, TRUE);
		m_pTextTarget->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextTarget->SetEnable( TRUE);
		m_pTextTarget->setPos( (UINT32)(18*i3GuiRoot::getHeightRatioBy768()), (UINT32)(nScreenHeight - (31*i3GuiRoot::getHeightRatioBy768())) );
		m_pTextTarget->SetColor( 255, 255, 255, 255);
		GetNode()->AddChild( m_pTextTarget);		
	}


	// IME 생성
	#define CHAT_MAX_LIMIT 80
	m_pIME = i3IME::new_object();

	if( m_pIME != NULL ) 
	{
		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;	
		ime._nTextLimitCount = CHAT_MAX_LIMIT- 20 - 2;
		ime._StyleIME = 0;
		m_pIME->CreateIME( &ime);
		m_pIME->setOwner( this);
		m_pIME->SetIMEMode( IME_MODE_NATIVE);		//	채팅시 기본 한글 모드
		m_CurrentIMEMode = m_pIME->GetIMEMode();

		i3GfxFontDX2 * pFont = m_pTextChat->getFont();
		if( pFont != NULL)
		{
			LOGFONT * pLogFont = pFont->getLogFont();
			if( pLogFont != NULL)
			{
				m_pIME->SetIMEFont( pFont->getHFont(), pLogFont);
			}
		}
	}
	I3ASSERT( m_pIME != NULL);

	SetChatEnable(FALSE);

	// 채팅 버퍼를 초기화 합니다.
	g_pFramework->getChatBox()->ResetChatBuffer();

	// 귓속말 버퍼를 업데이트 합니다.
	UpdateWhisperBackupBuffer();

	// 채팅 채널을 초기화합니다.
	_SetChatChannelType(CHAT_CHANNEL_TYPE_ALL);

	return i3GameObjBase::Create(pParentNode, bEnable);
}

void CBattleChatBox::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	// 채팅창이 활성화 되었을경우에는 항상 포커스를 가지게 합니다.
	if( m_bChatEnable)
	{		
		m_pIME->SetFocus( TRUE);
	}
	else
	{
		// 채팅 캐럿 비활성화
		m_pCaretSprite->SetEnable( 0, FALSE);
	}

	// Offset
	CHud * pHud = CHud::i();
	I3ASSERT( pHud != NULL);

	if( pHud->getHudMode() == HUD_MODE_MISSION_OBSERVER)
	{
		m_nChatBoxOffsetHeight = pHud->getChatOffsetPos() * (INT32)(i3GuiRoot::getHeightRatioBy768());

		_UpdateChatPos();
	}
	else
	{
		m_nChatBoxOffsetHeight = 0;
		_UpdateChatPos();
	}

	if (m_bChatEnable)
	{
		_ProcessChat(rDeltaSeconds);
		_UpdateChatCaret(rDeltaSeconds);
	}
	else
	{
		UINT64 nStrokeKey	= g_pFramework->getKeyStroke();

		// Chat Scroll
		if( nStrokeKey & GAME_KEY_FUNC_CHATSCROLL || g_pFramework->getSysKeyStroke() & GAME_KEY_FUNC_CHATSCROLL)
		{
			if( !(CBattleGui::i()->GetEnablePopup() ) )
			{
				// 미션전에서 폭탄 설치시 문제가 됩니다. 설치나 해체중이라면 작동하지 말아야합니다.
				if( g_pGameContext->IsBombMissionMode() &&
					(g_pGameContext->getMissionManager()->IsInstalling( g_pGameContext->getMySlotIdx() ) ||
					g_pGameContext->getMissionManager()->IsUninstalling( g_pGameContext->getMySlotIdx() ) ) )
				{
				}
				else
					SetEnableChatScroll(!m_bChatScrollEnable);
			}
		}

		if( m_bChatScrollEnable)
		{
			_ProcessChatScroll( rDeltaSeconds);
		}
	}

	_ProcessChatBox(rDeltaSeconds);	
	_ProcessMacro(rDeltaSeconds);

	// ChatList 마우스 캡처 유지 검사 - press가 끝나면 놓는다
	if (m_bCapturedMouse)
	{
		i3InputMouse* pMouse = g_pFramework->getMouse();
		UINT32 nState = pMouse->GetButtonState();

		if (FALSE == TESTBIT(nState, I3I_MOUSE_LBUTTON))
		{
			m_bCapturedMouse = FALSE;
		}
	}
}

BOOL CBattleChatBox::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_COMPOSITION_START:
			{
				i3GuiRoot::setGuiDisableInput( TRUE);
			}
			break;
		case EDIT_STATE_COMPOSITION_END:
			{
				i3GuiRoot::setGuiDisableInput( FALSE);

				i3Framework * pFramework  = i3Framework::getCurrentFramework();
				i3Viewer * pViewer = pFramework->GetViewer();
				if( pViewer != NULL)
				{
					i3InputDeviceManager * pDevMng = pViewer->GetInputDeviceManager();
					if( pDevMng != NULL)
					{
						pDevMng->Reset();
					}
				}
			}
			break;
		default:
			break;
		}
	}

	return FALSE;
}

void CBattleChatBox::SetEnableChatScroll(BOOL bFlag)
{ 
	char szTemp[256];

	if( bFlag)
	{	
		// "지난대화보기 인터페이스가 활성화 되었습니다."
		sprintf_s( szTemp, "{col:255,120,0,255}%s{/col}", GAME_STRING("STBL_IDX_CHATMESSAGE_HISTORY_ENABLE1"));	
		g_pFramework->getChatBox()->PutSystemChatting( szTemp);

		// "버튼을 사용하여 비활성화 시킬 수 있습니다."
		sprintf_s( szTemp, "{col:255,120,0,255}'%s'%s{/col}", _GetKeyboardValue(&g_pEnvSet->m_KeyMapArray[GAME_KEY_MAP_CHAT_HISTORY]), GAME_STRING("STBL_IDX_CHATMESSAGE_HISTORY_ENABLE2"));
		g_pFramework->getChatBox()->PutSystemChatting( szTemp);

		g_pFramework->getChatBox()->ResetBackupBuffer();
		
		I3RECT rect;

		_GetSpriteRect(ST_THUMB, &rect);
		m_pSpriteChatOutput->SetRect(ST_THUMB, rect.left, rect.top + 163.0f, rect.right, rect.bottom);
	}
	else
	{
		if( g_pGUIRoot && FALSE == i3GuiRoot::getGuiDisableInput() && FALSE == ((CHud*)m_pParent)->IsEnableGui())
		{
			g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
		}
	}

	m_bChatScrollEnable = bFlag;	
	m_pSpriteChatOutput->SetEnable(bFlag);

	//1.5 기획에 제외된 내용
	m_pSpriteChatChannel->SetEnable(FALSE);
	for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++) 
		m_pTextChatChannel[i]->SetEnable(FALSE);
}

BOOL CBattleChatBox::GetEnableChatScroll(void)
{
	return m_bChatScrollEnable;
}

void CBattleChatBox::SetChatEnable(BOOL bFlag)
{
	// 인게임 옵션 팝업창이 활성화 되어있을경우 채팅을 활성화 하지 않습니다.
	if( CBattleGui::i() )
	{
		if( CBattleGui::i()->GetEnablePopup() ) return;	
	}

	m_bChatEnable = bFlag;
    	
	switch(m_nChatType)
	{
	case CHATTING_TYPE_ALL:	
		m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));	
		break;		// 전체
	case CHATTING_TYPE_TEAM:	
		m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));	
		break;		// 팀
	case CHATTING_TYPE_CLAN:
		m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_CLAN"));	
		break;		// 클랜
	default: break;
	}

	if( !bFlag)
	{
		m_CurrentIMEMode = m_pIME->GetIMEMode();
		g_pFramework->getChatBox()->ResetHistoryBufferIndex();
		m_pIME->SetIMEMode( IME_MODE_ALPHANUMERIC);	//	Chat 종료후 IME 조합 메시지를 막기 위해... komet
		//m_pIME->SetIMEEnable( FALSE);
		m_pIME->SetFocus( FALSE);		
		
		g_pFramework->getChatBox()->SetEnable( FALSE);

		//if( g_pGUIRoot && FALSE == i3GuiRoot::getGuiDisableInput())
		if( g_pGUIRoot)
		{
			if( !g_pGUIRoot->isEnable())
			{
				g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
			}
			else
			{
				m_bChatEnableWithGUI = TRUE;
			}
		}

		m_pIME->InputDone();
	}
	else
	{
		m_pIME->SetFocus( TRUE);
		//m_pIME->SetIMEEnable( TRUE);	
		m_pIME->SetIMEMode( m_CurrentIMEMode);
		m_pIME->setEnterFeedCount( 0);

		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_MENU);
		g_pFramework->getChatBox()->SetEnable( TRUE);
	}
	
	m_pSpriteChat->SetEnable( bFlag);

#if defined ( LOCALE_NORTHAMERICA)
	m_pSpriteChat->SetEnable( HUD_SPRITE_CHAT_MODE, FALSE);		//	북미 요청으로 다국어, 영어 언어 마크 안보이게 처리
#endif
	
	m_pTextChat->SetText("");
	m_pTextChat->SetEnable( bFlag);	

	m_pTextTarget->SetEnable( bFlag);

	if( !m_bChatScrollEnable)
	{
		m_pSpriteChatChannel->SetEnable( bFlag);
		for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++) m_pTextChatChannel[i]->SetEnable(bFlag);
	}
}

// Chat이 가능한지 설정합니다.
// 현재의 Chat이 가능하지 설정을 반환합니다.
BOOL CBattleChatBox::GetChatEnable(void)
{
	return m_bChatEnable;
}

void CBattleChatBox::_GetTextrueRect(UINT32 idxSprite,I3RECT* pRect)
{
	I3ASSERT(idxSprite < ST_MAX);

	// UV 좌표. 순서 주의할 것
	I3RECT rectTable[ST_MAX] = 
	{
		{516.0f,  65.0f, 918.0f,  85.0f},		// ST_HEAD
		{516.0f,  87.0f, 918.0f,  88.0f},		// ST_BODY
		{516.0f,  90.0f, 918.0f, 111.0f},		// ST_TAIL
		{573.0f,   3.0f, 584.0f,  11.0f},		// ST_UP_SPIN_N
		{585.0f,   3.0f, 596.0f,  11.0f},		// ST_UP_SPIN_F
		{573.0f,  13.0f, 584.0f,  21.0f},		// ST_DOWN_SPIN_N
		{585.0f,  13.0f, 596.0f,  21.0f},		// ST_DOWN_SPIN_F
		{555.0f,   2.0f, 570.0f,  26.0f}		// ST_THUMB
	};

	// 출력
	i3mem::Copy(pRect, rectTable + idxSprite, sizeof(I3RECT));
}

void CBattleChatBox::_GetSpriteRect(UINT32 idxSprite,I3RECT* pRect)
{
	I3ASSERT(idxSprite < ST_MAX);

	// ! praptor - right/bottom을 width/height로 사용합니다.
	I3RECT rectTable[ST_MAX] = 
	{
		{  0.0f,   0.0f, 403.0f*i3GuiRoot::getHeightRatioBy768(),  21.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_HEAD
		{  0.0f,  21.0f*i3GuiRoot::getHeightRatioBy768(), 403.0f*i3GuiRoot::getHeightRatioBy768(), 114.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_BODY
		{  0.0f, 135.0f*i3GuiRoot::getHeightRatioBy768(), 403.0f*i3GuiRoot::getHeightRatioBy768(),  22.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_TAIL
		{388.0f*i3GuiRoot::getHeightRatioBy768(),   4.0f*i3GuiRoot::getHeightRatioBy768(),  11.0f*i3GuiRoot::getHeightRatioBy768(),   8.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_UP_SPIN_N
		{388.0f*i3GuiRoot::getHeightRatioBy768(),   4.0f*i3GuiRoot::getHeightRatioBy768(),  11.0f*i3GuiRoot::getHeightRatioBy768(),   8.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_UP_SPIN_F
		{388.0f*i3GuiRoot::getHeightRatioBy768(), 145.0f*i3GuiRoot::getHeightRatioBy768(),  11.0f*i3GuiRoot::getHeightRatioBy768(),   8.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_DOWN_SPIN_N
		{388.0f*i3GuiRoot::getHeightRatioBy768(), 145.0f*i3GuiRoot::getHeightRatioBy768(),  11.0f*i3GuiRoot::getHeightRatioBy768(),   8.0f*i3GuiRoot::getHeightRatioBy768()},		// ST_DOWN_SPIN_F
		{386.0f*i3GuiRoot::getHeightRatioBy768(),  14.0f*i3GuiRoot::getHeightRatioBy768(),  15.0f*i3GuiRoot::getHeightRatioBy768(),  23.0f*i3GuiRoot::getHeightRatioBy768()}		// ST_THUMB
	};

	// 출력
	i3mem::Copy(pRect, rectTable+ idxSprite, sizeof(I3RECT));

	// offset 적용
	pRect->left += 11.0f*i3GuiRoot::getHeightRatioBy768();
	pRect->top += (g_pFramework->GetViewer()->GetViewHeight() - 195.0f*i3GuiRoot::getHeightRatioBy768());
}

void CBattleChatBox::_ProcessChatScroll( REAL32 rDeltaSeconds)
{
	// Mouse로 Mode변경키를 눌렀을경우의 처리입니다.
	{
		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_MENU);

		i3InputMouse * pMouse = NULL;
		VEC3D *pVec = NULL;

		pMouse = g_pFramework->getMouse();

		VEC3D lt, lt2;
		VEC3D rb, rb2;
		m_pSpriteChatOutput->GetLeftTop(ST_UP_SPIN_N, &lt);
		m_pSpriteChatOutput->GetRightBottom(ST_UP_SPIN_N, &rb);
		m_pSpriteChatOutput->GetLeftTop(ST_DOWN_SPIN_N, &lt2);
		m_pSpriteChatOutput->GetRightBottom(ST_DOWN_SPIN_N, &rb2);

		UINT32 nState = pMouse->GetStrokeButtonState();

		// 채팅스크롤 버튼의 마우스 입력방식을 약간 변경합니다.(송명일)
		{
			UINT32 nPressState = pMouse->GetButtonState();
			if( nPressState & I3I_MOUSE_LBUTTON)
			{
				m_fChatScrollTimeDelta += rDeltaSeconds;
			}
			else
			{
				m_fChatScrollTimeDelta = 0.0f;
			}
		
			// 마우스버튼이 일정시간 계속눌려지고있다면 버튼업데이트를합니다.
			if( m_fChatScrollTimeDelta > 0.3f) nState |= I3I_MOUSE_LBUTTON;
		}

		// 채팅 스크롤 버튼이 눌려졌을경우의 처리입니다.
		if( nState & I3I_MOUSE_LBUTTON)
		{
			if( pMouse->GetX() >= i3Vector::GetX( &lt) && pMouse->GetX() <= i3Vector::GetX( &rb) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt) && pMouse->GetY() <= i3Vector::GetY( &rb) )
				{
					// 위쪽변경키
					g_pFramework->getChatBox()->DecreaseBackupBuffer(GAME_CHAT_LINE_COUNT);
				}
			}
			
			if( pMouse->GetX() >= i3Vector::GetX( &lt2) && pMouse->GetX() <= i3Vector::GetX( &rb2) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt2) && pMouse->GetY() <= i3Vector::GetY( &rb2) )
				{
					// 아래쪽변경키
					g_pFramework->getChatBox()->IncreaseBackupBuffer(GAME_CHAT_LINE_COUNT);
				}
			}
		}

		// Chat Channel
		if( nState & I3I_MOUSE_LBUTTON)
		{
			VEC3D lt, lt2, lt3, lt4;
			VEC3D rb, rb2, rb3, rb4;
			m_pSpriteChatChannel->GetLeftTop(0, &lt);
			m_pSpriteChatChannel->GetRightBottom(0, &rb);
			m_pSpriteChatChannel->GetLeftTop(1, &lt2);
			m_pSpriteChatChannel->GetRightBottom(1, &rb2);
			m_pSpriteChatChannel->GetLeftTop(2, &lt3);
			m_pSpriteChatChannel->GetRightBottom(2, &rb3);
			m_pSpriteChatChannel->GetLeftTop(3, &lt4);
			m_pSpriteChatChannel->GetRightBottom(3, &rb4);

			if( pMouse->GetX() >= i3Vector::GetX( &lt) && pMouse->GetX() <= i3Vector::GetX( &rb) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt) && pMouse->GetY() <= i3Vector::GetY( &rb) )
				{
					// 일반 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_ALL);
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt2) && pMouse->GetX() <= i3Vector::GetX( &rb2) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt2) && pMouse->GetY() <= i3Vector::GetY( &rb2) )
				{
					// 팀 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_TEAM);
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt3) && pMouse->GetX() <= i3Vector::GetX( &rb3) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt3) && pMouse->GetY() <= i3Vector::GetY( &rb3) )
				{
					// 클랜 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_CLAN);
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt4) && pMouse->GetX() <= i3Vector::GetX( &rb4) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt4) && pMouse->GetY() <= i3Vector::GetY( &rb4) )
				{
					// 귓말 & 답변 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_WHISPER);
				}
			}
		}

		// UpdateWheel
		m_pSpriteChatOutput->GetLeftTop(ST_HEAD, &lt);
		m_pSpriteChatOutput->GetRightBottom(ST_TAIL, &rb);
		if( pMouse->GetX() >= i3Vector::GetX( &lt) && pMouse->GetX() <= i3Vector::GetX( &rb) )
		{
			if( pMouse->GetY() >= i3Vector::GetY( &lt) && pMouse->GetY() <= i3Vector::GetY( &rb) )
			{		
				REAL32 rWheel = g_pViewer->GetInputDeviceManager()->GetMouse()->GetDeltaZ();

				if( 0.0f < rWheel )
				{
					g_pFramework->getChatBox()->DecreaseBackupBuffer( GAME_CHAT_LINE_COUNT);
				}
				else if( 0.0f > rWheel )
				{
					g_pFramework->getChatBox()->IncreaseBackupBuffer(GAME_CHAT_LINE_COUNT);
				}
			}
		}

		// Update Bar
		// 하동익 20110408
		if(m_bCapturedMouse || pMouse->GetButtonState() & I3I_MOUSE_LBUTTON)
		{
			m_pSpriteChatOutput->GetLeftTop(ST_BODY, &lt);
			m_pSpriteChatOutput->GetRightBottom(ST_BODY, &rb);

			if( m_bCapturedMouse || (pMouse->GetX() >= i3Vector::GetX( &lt)+387.0f*i3GuiRoot::getHeightRatioBy768() && pMouse->GetX() <= i3Vector::GetX( &rb)))
			{
				if( m_bCapturedMouse || (pMouse->GetY() >= i3Vector::GetY( &lt) && pMouse->GetY() <= i3Vector::GetY( &rb)))
				{		
					REAL32 fValueY = pMouse->GetY();
					pVec = m_pSpriteChatOutput->GetCenter(ST_THUMB);

					if( fValueY < i3Vector::GetY( &lt)) fValueY = i3Vector::GetY( &lt);
					else if( fValueY > i3Vector::GetY( &rb)) fValueY = i3Vector::GetY( &rb);
                    
					i3Vector::SetY( pVec, fValueY);

					m_pSpriteChatOutput->SetCenter(ST_THUMB, pVec);

					REAL32 fVal = i3Vector::GetY( pVec) - i3Vector::GetY( &lt);

					fVal = (fVal/(114.0f*i3GuiRoot::getHeightRatioBy768()));

					g_pFramework->getChatBox()->CalcBackupBufferByRatio( fVal, GAME_CHAT_LINE_COUNT);	

					m_bCapturedMouse = TRUE;
				}
			}
		}
		
		m_pSpriteChatOutput->GetRightBottom(ST_BODY, &rb);
		REAL32 fValueY = i3Vector::GetY( &rb) - (111*g_pFramework->getChatBox()->GetBackUpBufferRatio()*i3GuiRoot::getHeightRatioBy768());
		pVec = m_pSpriteChatOutput->GetCenter(ST_THUMB);
		i3Vector::SetY( pVec, fValueY);
		m_pSpriteChatOutput->SetCenter(ST_THUMB, pVec);
	}
}

void CBattleChatBox::_ProcessChatBox( REAL32 rDeltaSeconds)
{		
	INT32 i = 0;
	char szTotalText[2048];
	szTotalText[0] = 0;

	if( !m_pTextBox->GetEnable())
	{
		m_pTextBox->SetEnable( TRUE);
	}


	// 일정 시간이 지나면 채팅 버퍼를 증가 시킴으로 자동 스크롤 시킵니다.
	{
		static REAL32 rLocalTime = 0.0f;

		if( g_pFramework->getChatBox()->GetBufferCount() > 0)
		{
			rLocalTime += rDeltaSeconds;
			if( rLocalTime > 5.0f)
			{			
				g_pFramework->getChatBox()->DeleteBufferIndex();	
				rLocalTime = 0.0f;
			}
		}
	}

	// 채팅버퍼가 화면에 보여주는 사이즈를 넘겼을 경우 자동 스크롤 시킵니다.
	if( g_pFramework->getChatBox()->GetBufferCount() > GAME_CHAT_LINE_COUNT)
	{
		g_pFramework->getChatBox()->DeleteBufferIndex();	
	}


	// Buffer를 돌면서 출력할 글자를 담습니다.
	{
		INT32 nLoopCount = 0;
		INT32 BufferIdx= 0;
		char szChatBoxText[1024] = {0,};

		if( !m_bChatScrollEnable)
		{
			if( g_pFramework->getChatBox()->GetCurrentBufferIndex() == 0)
			{
				BufferIdx = MAX_CHAT_BUFFER-1;
			}
			else
			{
				BufferIdx = g_pFramework->getChatBox()->GetCurrentBufferIndex() - 1;
			}
		}
		else
		{
			if( g_pFramework->getChatBox()->GetBackUpBufferIndex() == 0)
			{
				BufferIdx = MAX_CHAT_BUFFER-1;
			}
			else
			{
				BufferIdx = g_pFramework->getChatBox()->GetBackUpBufferIndex() - 1;
			}
		}

		for( i= BufferIdx; nLoopCount < GAME_CHAT_LINE_COUNT; i--)
		{
			if( nLoopCount > g_pFramework->getChatBox()->GetBufferCount() && !m_bChatScrollEnable)			
			{
				break;
			}

			if( i<0)
			{
				i = MAX_CHAT_BUFFER-1;
			}

 			if( m_nCurrentChatBufferType == CHATTING_TYPE_ALL || m_nCurrentChatBufferType == CHATTING_TYPE_LOBBY ||  m_nCurrentChatBufferType == CHATTING_TYPE_MATCH)
			{
				nLoopCount++;

				if( m_bChatScrollEnable)
				{
					i3String::Copy( szChatBoxText, g_pFramework->getChatBox()->GetBackUpBufferText(i), 1024 );
				}
				else
				{
					i3String::Copy( szChatBoxText, g_pFramework->getChatBox()->GetBufferText(i), 1024 );		
				}

				if( i3String::Length( szChatBoxText) > 0)
				{
					i3String::Concat( szChatBoxText, "\n");
					i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));		
				}
			}
			else if( m_nCurrentChatBufferType == CHATTING_TYPE_TEAM && ( g_pFramework->getChatBox()->getBufferType(i) == CHATTING_TYPE_TEAM || g_pFramework->getChatBox()->getBufferType(i) == CHATTING_TYPE_CLAN) )
			{
				nLoopCount++;

				if( m_bChatScrollEnable)
				{
					i3String::Copy( szChatBoxText, g_pFramework->getChatBox()->GetBackUpBufferText(i), 1024 );
				}
				else
				{
					i3String::Copy( szChatBoxText, g_pFramework->getChatBox()->GetBufferText(i), 1024 );		
				}

				if( i3String::Length( szChatBoxText) > 0)
				{
					i3String::Concat( szChatBoxText, "\n");
					i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));		
				}
			}
			else if( m_nCurrentChatBufferType == g_pFramework->getChatBox()->getBufferType(i) || g_pFramework->getChatBox()->getBufferType(i) == 0)
			{
				nLoopCount++;

				if( m_bChatScrollEnable)
				{
					i3String::Copy( szChatBoxText, g_pFramework->getChatBox()->GetBackUpBufferText(i), 1024 );
				}
				else
				{
					i3String::Copy( szChatBoxText, g_pFramework->getChatBox()->GetBufferText(i), 1024 );		
				}

				if( i3String::Length( szChatBoxText) > 0)
				{
					i3String::Concat( szChatBoxText, "\n");
					i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));		
				}
			}
		}

		// chat list를 출력합니다.
		m_pTextBox->SetText( szTotalText);
	}
}

void CBattleChatBox::_ProcessChat(REAL32 rDeltaSeconds)
{
	COLOR col;
	char szTemp[256] = {0,};
	char szTemp2[256]= {0,};
	char szTarget[256]= {0,};
	char szHistory[256]= {0,};

	CHAT_SYS_TYPE ChatType = CHAT_SYS_TYPE_NONE;
	i3InputMouse * pMouse = NULL;
	BOOL bWhisper = FALSE;
	BOOL bReply = FALSE;
	static INT32 WhisperLength = 0;
	INT32 nWhsiperTextCount = 0;
	INT32 nReplyTextCount = 0;
	static BOOL	bUseHistory = FALSE;

	if( ! m_bChatEnable)
	{
		g_pFramework->getChatBox()->SetEnable( FALSE);
		return;
	}

	// Mouse로 Mode변경키를 눌렀을경우의 처리입니다.
	{
		pMouse = g_pFramework->getMouse();

		VEC3D lt, lt2;
		VEC3D rb, rb2;
		m_pSpriteChat->GetLeftTop( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, &lt);
		m_pSpriteChat->GetRightBottom( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, &rb);
		m_pSpriteChat->GetLeftTop( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, &lt2);
		m_pSpriteChat->GetRightBottom( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, &rb2);

		UINT32 nState = pMouse->GetStrokeButtonState();

		if( nState & I3I_MOUSE_LBUTTON)
		{
			if( pMouse->GetX() >= i3Vector::GetX( &lt) && pMouse->GetX() <= i3Vector::GetX( &rb) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt) && pMouse->GetY() <= i3Vector::GetY( &rb) )
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;

					// 위쪽 변경키
					if( m_nChatType == CHATTING_TYPE_ALL) m_nChatType = CHATTING_TYPE_CLAN;
					else if( m_nChatType == CHATTING_TYPE_TEAM) m_nChatType = CHATTING_TYPE_ALL;
					else if( m_nChatType == CHATTING_TYPE_CLAN) m_nChatType = CHATTING_TYPE_TEAM;

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt2) && pMouse->GetX() <= i3Vector::GetX( &rb2) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt2) && pMouse->GetY() <= i3Vector::GetY( &rb2) )
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;

					// 아래쪽변경키
					if( m_nChatType == CHATTING_TYPE_ALL) m_nChatType = CHATTING_TYPE_TEAM;
					else if( m_nChatType == CHATTING_TYPE_TEAM) m_nChatType = CHATTING_TYPE_CLAN;
					else if( m_nChatType == CHATTING_TYPE_CLAN) m_nChatType = CHATTING_TYPE_ALL;

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
		}

		// Chat Channel
		if( nState & I3I_MOUSE_LBUTTON)
		{
			VEC3D lt, lt2, lt3, lt4;
			VEC3D rb, rb2, rb3, rb4;
			m_pSpriteChatChannel->GetLeftTop(0, &lt);
			m_pSpriteChatChannel->GetRightBottom(0, &rb);
			m_pSpriteChatChannel->GetLeftTop(1, &lt2);
			m_pSpriteChatChannel->GetRightBottom(1, &rb2);
			m_pSpriteChatChannel->GetLeftTop(2, &lt3);
			m_pSpriteChatChannel->GetRightBottom(2, &rb3);
			m_pSpriteChatChannel->GetLeftTop(3, &lt4);
			m_pSpriteChatChannel->GetRightBottom(3, &rb4);

			if( pMouse->GetX() >= i3Vector::GetX( &lt) && pMouse->GetX() <= i3Vector::GetX( &rb) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt) && pMouse->GetY() <= i3Vector::GetY( &rb) )
				{
					// 일반 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_ALL);
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt2) && pMouse->GetX() <= i3Vector::GetX( &rb2) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt2) && pMouse->GetY() <= i3Vector::GetY( &rb2) )
				{
					// 팀 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_TEAM);
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt3) && pMouse->GetX() <= i3Vector::GetX( &rb3) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt3) && pMouse->GetY() <= i3Vector::GetY( &rb3) )
				{
					// 클랜 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_CLAN);
				}
			}

			if( pMouse->GetX() >= i3Vector::GetX( &lt4) && pMouse->GetX() <= i3Vector::GetX( &rb4) )
			{
				if( pMouse->GetY() >= i3Vector::GetY( &lt4) && pMouse->GetY() <= i3Vector::GetY( &rb4) )
				{
					// 귓말 & 답변 채팅 채널
					_SetChatChannelType( CHAT_CHANNEL_TYPE_WHISPER);
				}
			}
		}
	}

#if defined ( LOCALE_NORTHAMERICA)
	m_pSpriteChat->SetEnable( HUD_SPRITE_CHAT_MODE, FALSE);		//	북미 요청으로 다국어, 영어 언어 마크 안보이게 처리
#else
	// IME모드에 따라서 컨트롤을 변경합니다.		
	if( m_pIME->GetIMEMode() == IME_MODE_NATIVE)
	{
		//다국어
		m_pSpriteChat->SetTextureCoord( HUD_SPRITE_CHAT_MODE, (556.0f), (26.0f), (578.0f), (48.0f));
	}
	else
	{
		// 영어
		m_pSpriteChat->SetTextureCoord( HUD_SPRITE_CHAT_MODE, (578.0f), (26.0f), (600.0f), (48.0f));
	}
#endif

	if( m_bOnceChatTypeAll)
	{			
		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
		m_pTextTarget->SetColor( &col);
		m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));	
	}
	else if( m_bOnceChatType)
	{
		i3Color::Set( &col, (UINT8) 98, 180, 253, 255);
		m_pTextTarget->SetColor( &col);
		m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));	
	}
	else
	{
		switch( m_nChatType)
		{
		case CHATTING_TYPE_ALL:		// 전체
			i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
			m_pTextTarget->SetColor( &col);
			m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));	

			// 임시용입니다. 하동익 어뷰징 테스트
			//g_nChatType = m_nChatType;
			break;	
		case CHATTING_TYPE_TEAM:		// 팀
			i3Color::Set( &col, (UINT8) 98, 180, 253, 255);
			m_pTextTarget->SetColor( &col);
			m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));	

			// 임시용입니다.
			//g_nChatType = m_nChatType;
			break;	
		case CHATTING_TYPE_CLAN:		// 클랜
			i3Color::Set( &col, (UINT8) 80, 255, 60, 255);
			m_pTextTarget->SetColor( &col);
			m_pTextTarget->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_CLAN"));	

			// 임시용입니다.
			//g_nChatType = m_nChatType;
			break;	
		case CHATTING_TYPE_WHISPER:		// 귓말
			i3Color::Set( &col, (UINT8) 237, 86, 250, 255);		
			break;
		case CHATTING_TYPE_REPLY:		// 답변
			i3Color::Set( &col, (UINT8) 237, 86, 250, 255);				
			break;
		default: break;
		}
	}

	{
		INT32 nCaretPos = m_pIME->getIME()->GetInsertPositionA(); 
		INT32 ret = 0;

		ret = m_pIME->getIME()->GetInput( szTarget, sizeof( szTarget));
		if( m_pIME->getIME()->IsComp())
		{
			ret += m_pIME->getIME()->GetComp( szTemp, sizeof( szTemp));

			i3String::Format(szTemp2, 255, "{Col:255,100,100,255}%s{/Col}", szTemp);

			//	최종 문자열의 크기가 인자로 들어온 버퍼 크기를 오버하지 않는지 검사
			I3ASSERT( (i3String::Length( szTarget) + i3String::Length( szTemp2)) < 256);

			//	조합문자를 버퍼 중간에 삽입
			i3String::InsertStr( szTarget, szTemp2, nCaretPos, i3String::Length( szTemp2));
		}

		szTarget[i3String::Length( szTarget)] = 0;
	}

	g_pFramework->getChatBox()->SetEnable( TRUE);
	m_pTextChat->SetColor( &col);
	m_pTextChat->SetText( szTarget);		

	if( m_nChatType != CHATTING_TYPE_WHISPER && m_nChatType != CHATTING_TYPE_REPLY)
	{
		ChatType = g_pFramework->getChatBox()->ParsingText( m_pTextChat->getText());
		{
			switch( ChatType)
			{
			case CHAT_SYS_TYPE_WHISPER:		
				{
					char szWhisper[128];
					if( !bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_WHISPER;

					i3String::Copy( szWhisper, "[To:", 128 );
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatTarget());
					i3String::Concat( szWhisper, "] ");
					szWhisper[i3String::Length( szWhisper)] = 0;
					WhisperLength = i3String::Length( szWhisper);
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetHistoryText());
					m_pIME->SetInput( szWhisper);
					m_pTextChat->SetText( szWhisper);

					m_bOnceChatType = FALSE;
				}
				break;
			case CHAT_SYS_TYPE_REPLY:	
				char szWhisper[128];
				if( !bUseHistory) m_nOldChatType = m_nChatType;
				m_nChatType = CHATTING_TYPE_REPLY;

				if( i3String::Length(g_pFramework->getChatBox()->GetChatReplyTarget()) > 0)
				{
					i3String::Copy( szWhisper, "[To:", 128 );
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatReplyTarget());
					i3String::Concat( szWhisper, "] ");
					szWhisper[i3String::Length( szWhisper)] = 0;
					WhisperLength = i3String::Length( szWhisper);
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetHistoryText());
					m_pIME->SetInput( szWhisper);
					m_pTextChat->SetText( szWhisper);

					m_bOnceChatType = FALSE;
				}
				else
				{
					m_nChatType = CHATTING_TYPE_ALL;
					m_nOldChatType = CHATTING_TYPE_ALL;
					m_pIME->SetInput("");
				}
				break;
			case CHAT_SYS_TYPE_ONETIME_TEAM:	// 일회성 팀 챗팅!!	
				m_bOnceChatType = TRUE;
				m_pIME->SetInput("");
				break;
			}
		}
	}

	// 귓속말 상태에서 아무런 텍스트가 입력되지 않은상태에서 글자가 지워지면 초기화합니다.
	if( m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
	{
		if( i3String::Length(m_pIME->getIMETextBuf()) < WhisperLength)
		{
			m_pIME->SetInput("");
			m_nChatType = m_nOldChatType;
			WhisperLength = 0;
		}

		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{				
			char szWhisper[128];
			i3String::Copy( szWhisper, "[To:", 128 );
			i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatTarget());
			i3String::Concat( szWhisper, "] ");

			const char* szContents = m_pIME->getIMETextBuf();

			if( i3String::Contain( szContents, szWhisper) == -1)
			{
				m_pIME->SetInput("");
				m_nChatType = m_nOldChatType;
				WhisperLength = 0;
			}
		}

		if( m_nChatType == CHATTING_TYPE_REPLY)
		{
			char szWhisper[128];
			i3String::Copy( szWhisper, "[To:", 128 );
			i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatReplyTarget());
			i3String::Concat( szWhisper, "] ");

			const char* szContents = m_pIME->getIMETextBuf();

			if( i3String::Contain( szContents, szWhisper) == -1)
			{
				m_pIME->SetInput("");
				m_nChatType = m_nOldChatType;
				WhisperLength = 0;
			}
		}
	}

	// Chat History기능
	if( g_pFramework->getChatBox()->GetHistoryBufferCount() > 0)
	{
		i3InputKeyboard* pKeyboard = g_pFramework->getKeyboard();

		char szText[256];
		szText[0]=0;
		INT32 nChatMode = 0;

		if (pKeyboard->GetStrokeState(I3I_KEY_UP))
		{	
			bUseHistory = TRUE;

			if( g_pFramework->getChatBox()->GetHistoryBufferOffset() == 0)
			{
				m_nOldChatType = m_nChatType;
			}

			g_pFramework->getChatBox()->GetHistoryBufferText(1, szText, 256, &nChatMode);
			if(szText[0] != NULL)
			{
				m_pIME->SetInput( szText);
				m_nChatType = nChatMode;
			}
			else
			{
				m_pIME->SetInput( "");
			}
		}
		else if (pKeyboard->GetStrokeState(I3I_KEY_DOWN))
		{	
			bUseHistory = TRUE;

			g_pFramework->getChatBox()->GetHistoryBufferText(-1, szText, 256, &nChatMode);
			if(szText[0] != NULL)	
			{
				m_pIME->SetInput( szText);
				m_nChatType = nChatMode;
			}
			else
			{
				bUseHistory = FALSE;
				m_pIME->SetInput( "");
				m_nChatType = m_nOldChatType;
			}
		}		
	}

	if (GetChatEnable())
	{
		//	빈 채팅창일 경우
		if( (g_pFramework->getSysKeyStroke() & GAME_KEY_CMD_CHAT) == GAME_KEY_CMD_CHAT)
		{
			if( i3String::Length(m_pIME->getIMETextBuf()) == 0)
			{
				SetChatEnable( FALSE);
			}
		}

		// IME 에디트 입력받는 부분은 받드시 상태를 체크해서 처리해야 됩니다. komet 09.06
		// IME를 처리하는 Windows API Proc은 메시지 큐잉 방식으로 처리하기 때문입니다.
		if(m_pIME->getIMEState() == EDIT_STATE_RETURN)
		{
			switch( ChatType)
			{
			case CHAT_SYS_TYPE_CLEAR:
				g_pFramework->getChatBox()->ResetChatBuffer();
				m_pIME->SetInput("");
				break;
			case CHAT_SYS_TYPE_HELP:
				g_pFramework->getChatBox()->SetHelpText();
				m_pIME->SetInput("");
				break;
			}

			if( i3String::Length(m_pIME->getIMETextBuf()) > 0) 
			{
				// 욕설필터
				I3TRACE( "IsRepeat : %f\n", rDeltaSeconds );
				if(g_pFramework->AddFilterMessage(m_pIME->getIMETextBuf()))
				{
					// 필터링 후 네트워크 전송
					g_pFramework->IsFilterChange( m_pIME->getIMETextBuf() );

					char szTemp[1024];
					szTemp[0] = 0;
					i3String::Copy( szTemp, m_pIME->getIMETextBuf(), 1024 );
					i3String::ToUpperBeforeN(  szTemp, 5);
					if( i3String::Contain( szTemp, COMMAND_KEYWORD) == 0)
					{
						// Command입니다.
#if defined( ENABLE_TEST_COMMAND )
						g_pFramework->getCommandSet()->Parsing( m_pIME->getIMETextBuf());
#endif
						m_pIME->SetInput("");
					}
					else if ( SetGMCommand(szTemp) )
					{
						// 아무것도 안함.
					}
					else
					{
						szTemp[0] = 0;

						// 네트워크에 메시지를 보냅니다.
						CHAT_INFO info;
						info._Type = (UINT16)m_nChatType;

						if( m_bOnceChatTypeAll)
						{
							if( IsGhostMessage())
							{
								i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT"), 1024 );/*(고스트) */
							}
							i3String::Concat( szTemp, m_pIME->getIMETextBuf());
						}
						else if( m_bOnceChatType)
						{
							if( IsGhostMessage())
							{	
								i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT"), 1024 );/*(고스트) */
							}
							i3String::Concat( szTemp, m_pIME->getIMETextBuf());
							info._Type = CHATTING_TYPE_TEAM;
						}
						else
						{
							switch( m_nChatType)
							{
							case CHATTING_TYPE_ALL:	// 전체
								if( IsGhostMessage())
								{
									i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT"), 1024 );/*(고스트) */
								}
								i3String::Concat( szTemp, m_pIME->getIMETextBuf());
								break;	
							case CHATTING_TYPE_TEAM:// 팀
								if( IsGhostMessage())
								{
									i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT"), 1024 );/*(고스트) */
								}
								i3String::Concat( szTemp, m_pIME->getIMETextBuf());
								break;	
							case CHATTING_TYPE_CLAN:// 클랜
								i3String::Concat( szTemp, m_pIME->getIMETextBuf());
								break;	
							case CHATTING_TYPE_WHISPER: // 귓말
								i3String::Copy( szTemp2, "{col:237, 86, 250, 255}", 256 );
								i3String::Concat( szTemp2, m_pIME->getIMETextBuf());
								i3String::Concat( szTemp2, "{/col}");

								i3String::Concat( szTemp, m_pIME->getIMETextBuf() + WhisperLength);

								nWhsiperTextCount = i3String::Length( m_pIME->getIMETextBuf() + WhisperLength);
								bWhisper = TRUE;
								m_nChatType = m_nOldChatType;	

								break;	
							case CHATTING_TYPE_REPLY: // 답변
								i3String::Copy( szTemp2, "{col:237, 86, 250, 255}", 256 );
								i3String::Concat( szTemp2, m_pIME->getIMETextBuf());
								i3String::Concat( szTemp2, "{/col}");

								i3String::Concat( szTemp, m_pIME->getIMETextBuf() + WhisperLength);

								nReplyTextCount = i3String::Length( m_pIME->getIMETextBuf() + WhisperLength);
								bReply = TRUE;
								m_nChatType = m_nOldChatType;
								break;	
							default: break;
							}
						}

						szTemp[i3String::Length( szTemp)] = 0;
						szTemp2[i3String::Length( szTemp2)] = 0;

						if( bWhisper)
						{
							if( nWhsiperTextCount > 0)
							{
								_ProcessChatBattleWhisper(g_pFramework->getChatBox()->GetChatTarget(), szTemp2, szTemp);

								i3String::Copy( szHistory, "/w ", 256 );
								strcat( szHistory, g_pFramework->getChatBox()->GetChatTarget());
								strcat( szHistory, " ");
								strcat( szHistory, (m_pIME->getIMETextBuf() + WhisperLength));
								szHistory[i3String::Length( szHistory)] = 0;
								g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
							}
						}
						else if( bReply)
						{
							if( nReplyTextCount > 0)
							{
								_ProcessChatBattleWhisper(g_pFramework->getChatBox()->GetChatReplyTarget(), szTemp2, szTemp);

								i3String::Copy( szHistory, "/r ", 256 );
								strcat( szHistory, " ");
								strcat( szHistory, m_pIME->getIMETextBuf() + WhisperLength);
								szHistory[i3String::Length( szHistory)] = 0;
								g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
							}
						}
						else 
						{
							USER_INFO_BASIC MyUserInfoBasic;
							g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

							info.setString( szTemp);

							if( info._Type == CHATTING_TYPE_CLAN)	// 클랜 채팅이라면..
							{
								// 클랜 미가입 상태입니다.
								if(MyUserInfoBasic._clanidx == 0 || MyUserInfoBasic._clanstate == 0)
								{
									g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_CHAT_NOTICE_NO_JOIN_CLAN"));/*<클랜에 미가입한 상태입니다.>*/
								}
								else
								{
									g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
									g_pFramework->getChatBox()->PutHistory(m_pIME->getIMETextBuf(), m_nChatType);
								}
							}
							else
							{
								// 커뮤니티 옵션에서의 채팅 제한이 걸려있다면, 그것에 대한 처리를 합니다.
								if( g_pEnvSet->m_nChatType != 0 && (m_nChatType != CHATTING_TYPE_TEAM && m_nChatType != CHATTING_TYPE_CLAN) )
								{
									i3String::Copy( szTemp2, "[", 256 );
									i3String::Concat( szTemp2, MyUserInfoBasic._nick);
									i3String::Concat( szTemp2, "] ");
									i3String::Concat( szTemp2, szTemp);

									g_pFramework->getChatBox()->PutSystemChatting(szTemp2);
									g_pFramework->getChatBox()->PutHistory(m_pIME->getIMETextBuf(), m_nChatType);
								}
								else
								{
									g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
									g_pFramework->getChatBox()->PutHistory(m_pIME->getIMETextBuf(), m_nChatType);
								}
							}
						}

						WhisperLength = 0;
					}
				}

				g_pFramework->getChatBox()->ResetHistoryBufferIndex();

				m_pIME->SetInput("");

				m_bOnceChatType = FALSE;
				m_bOnceChatTypeAll = FALSE;

				if( bUseHistory) m_nChatType = m_nOldChatType;
				bUseHistory = FALSE;

				//	IME 처리 종료
				m_pIME->InputDone();

				SetChatEnable( FALSE);
			}
			else
			{
				m_bOnceChatType = FALSE;
				m_bOnceChatTypeAll = FALSE;	

				SetChatEnable( FALSE);
			}
		}
	}


}

void CBattleChatBox::_ProcessChatBattleWhisper(char* nick, char* message, char* message2)
{
	// 리스폰 제한 모드에서는 어뷰징을 막기위해 참여중인 플레이어간 귓속말을 막습니다.
	BOOL canWhisper = TRUE;

	// 현재 폭파미션이 리소폰 제한 모드
	if (g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode())
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			// 배틀 중인 유저
			SLOT_INFO* slot = g_pGameContext->getSlotInfo(i);

			if (SLOT_STATE_LOAD <= slot->_State)
			{
				// 귓속말 보내려는 유저
				if (0 == i3String::Compare(nick, g_pGameContext->getNickForSlot(i)))
				{
					canWhisper = FALSE;
				}
			}
		}
	}
	
	// 귓속말 보낼 수 있다면 보낸다.
	if (canWhisper)
	{

		// 친구 목록에 있는 닉네임인 경우 해당 UID를 찾아서 UID로 보낸다.			


		// Send Event

		g_pFramework->getChatBox()->PutSystemChatting(message);
		g_pGameContext->SetEventOnce( EVENT_SEND_WHISPER, nick, message2);
	}
	else
	{
		g_pFramework->PutSystemChatting(GAME_STRING("STR_TBL_CHAT_NOTICE_WARNING_WHISPER"));/*리스폰이 되지 않는 모드의 경우 같은 방에서 play 중인 사용자에게 귓속말을 보낼 수 없습니다.*/
	}
}

void CBattleChatBox::_ProcessMacro(REAL32 rDeltaSeconds)
{
	// 채팅 상태설정.	
	// Macro를 이용한 입력처리

	CHAT_INFO info;
	char szTemp[1024];

	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ))
		{			
			if( i3String::Length( g_pEnvSet->m_szMacro[0]) > 0)
			{	
				// 욕설필터
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[0]))
				{
					g_pFramework->IsFilterChange( g_pEnvSet->m_szMacro[0] );

					if( g_pEnvSet->m_bMacroEnableFriend[0])
					{
						info._Type = CHATTING_TYPE_TEAM;	// 팀

						if( IsGhostMessage())
						{	
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR1"), 1024 );/*{col:98, 180, 253, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:98, 180, 253, 255}", 1024 );
						}
					}
					else
					{
						info._Type = CHATTING_TYPE_ALL;	// 전체

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR2"), 1024 );/*{col:255, 255, 255, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:255, 255, 255, 255}", 1024 );
						}
					}

					i3String::Concat( szTemp, g_pEnvSet->m_szMacro[0]);
					i3String::Concat( szTemp, "{/col}");
					info.setString( szTemp);
					g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[1]) > 0)
			{
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[1]))
				{
					g_pFramework->IsFilterChange( g_pEnvSet->m_szMacro[1] );

					if( g_pEnvSet->m_bMacroEnableFriend[1])
					{
						info._Type = CHATTING_TYPE_TEAM;	// 팀

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR1"), 1024 );/*{col:98, 180, 253, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:98, 180, 253, 255}", 1024 );
						}
					}
					else
					{
						info._Type = CHATTING_TYPE_ALL;	// 전체

						if( IsGhostMessage())
						{	
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR2"), 1024 );/*{col:255, 255, 255, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:255, 255, 255, 255}", 1024 );
						}
					}

					i3String::Concat( szTemp, g_pEnvSet->m_szMacro[1]);
					i3String::Concat( szTemp, "{/col}");
					info.setString( szTemp);
					g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[2]) > 0)
			{
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[2]))
				{
					g_pFramework->IsFilterChange( g_pEnvSet->m_szMacro[2] );

					if( g_pEnvSet->m_bMacroEnableFriend[2])
					{
						info._Type = CHATTING_TYPE_TEAM;	// 팀

						if( IsGhostMessage())
						{	
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR1"), 1024 );/*{col:98, 180, 253, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:98, 180, 253, 255}", 1024 );
						}
					}
					else
					{
						info._Type = CHATTING_TYPE_ALL;	// 전체

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR2"), 1024 );/*{col:255, 255, 255, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:255, 255, 255, 255}", 1024 );
						}
					}

					i3String::Concat( szTemp, g_pEnvSet->m_szMacro[2]);
					i3String::Concat( szTemp, "{/col}");
					info.setString( szTemp);
					g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[3]) > 0)
			{
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[3]))
				{
					g_pFramework->IsFilterChange( g_pEnvSet->m_szMacro[3] );

					if( g_pEnvSet->m_bMacroEnableFriend[3])
					{
						info._Type = CHATTING_TYPE_TEAM;	// 팀

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR1"), 1024 );/*{col:98, 180, 253, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:98, 180, 253, 255}", 1024 );
						}
					}
					else
					{
						info._Type = CHATTING_TYPE_ALL;	// 전체

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR2"), 1024 );/*{col:255, 255, 255, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:255, 255, 255, 255}", 1024 );
						}
					}

					i3String::Concat( szTemp, g_pEnvSet->m_szMacro[3]);
					i3String::Concat( szTemp, "{/col}");
					info.setString( szTemp);
					g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[4]) > 0)
			{
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[4]))
				{
					g_pFramework->IsFilterChange( g_pEnvSet->m_szMacro[4] );

					if( g_pEnvSet->m_bMacroEnableFriend[4])
					{
						info._Type = CHATTING_TYPE_TEAM;	// 팀

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR1"), 1024 );/*{col:98, 180, 253, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:98, 180, 253, 255}", 1024 );
						}
					}
					else
					{
						info._Type = CHATTING_TYPE_ALL;	// 전체

						if( IsGhostMessage())
						{
							i3String::Copy( szTemp, GAME_STRING("STR_TBL_CHAT_GHOST_TEXT_WITH_COLOR2"), 1024 );/*{col:255, 255, 255, 255}(고스트) */
						}
						else
						{
							i3String::Copy( szTemp, "{col:255, 255, 255, 255}", 1024 );
						}
					}

					i3String::Concat( szTemp, g_pEnvSet->m_szMacro[4]);
					i3String::Concat( szTemp, "{/col}");
					info.setString( szTemp);
					g_pGameContext->SetEventOnce(EVENT_CHATTING_N , &info ); 
				}
			}
		}
	}
	else if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
		{
			m_bOnceChatType = TRUE;
			if( !m_bChatEnable)	SetChatEnable( TRUE);
		}	
	}
	else 
	{
		if( g_pFramework->getGameKeyMode() == GAMEKEY_MODE_MENU)
		{
			if( !m_bChatEnableWithGUI)
			{
				if (FALSE == g_pFramework->IsMessageBoxEnabled())
				{
					if( (g_pFramework->getSysKeyStroke() & GAME_KEY_CMD_CHAT)==GAME_KEY_CMD_CHAT)
					{
						if( !m_bChatEnable)
						{
							SetChatEnable( TRUE);
						}
					}
				}
			}
			else
			{
				m_bChatEnableWithGUI = FALSE;
			}
			
			if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
			{
				if( m_nChatType != CHATTING_TYPE_ALL || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;
					m_nChatType = CHATTING_TYPE_ALL;
					g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
			else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
			{
				if( m_nChatType != CHATTING_TYPE_TEAM || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;
					m_nChatType = CHATTING_TYPE_TEAM;
					g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
			else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4)
			{
				if( m_nChatType != CHATTING_TYPE_CLAN || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;
					m_nChatType = CHATTING_TYPE_CLAN;
					g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
		}
		else
		{
			if (FALSE == g_pFramework->IsMessageBoxEnabled())
			{
				if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_CHAT)==GAME_KEY_CMD_CHAT)
				{
					if( !m_bChatEnable)
					{
						SetChatEnable( TRUE);
					}
				}
			}

			// 채팅 모드 설정
			if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
			{
				if( m_nChatType != CHATTING_TYPE_ALL || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;
					m_nChatType = CHATTING_TYPE_ALL;
					g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
			else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
			{
				if( m_nChatType != CHATTING_TYPE_TEAM || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;
					m_nChatType = CHATTING_TYPE_TEAM;
					g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
			else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4)
			{
				if( m_nChatType != CHATTING_TYPE_CLAN || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
				{
					m_bOnceChatType = FALSE;
					m_bOnceChatTypeAll = FALSE;
					m_nChatType = CHATTING_TYPE_CLAN;
					g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));

					// 임시용입니다.
					//g_nChatType = m_nChatType;
				}
			}
		}
	}
}

void CBattleChatBox::_UpdateChatPos(void)
{
	INT32 nScreenHeight = 0;
	int offset = 0;

	if(g_pGameContext->IsGM_Observer())
		offset = -85;

	for(UINT32 i = 0; i < ST_THUMB; i++)
	{
		I3RECT rect;

		// ! praptor - right/bottom을 width/height로 사용합니다.
		_GetSpriteRect(i, &rect);
		rect.top -= offset;
		m_pSpriteChatOutput->SetRect(i, rect.left, rect.top - m_nChatBoxOffsetHeight, rect.right, rect.bottom);
	}

	{
		INT32 nPosY = 0;
		if (g_pViewer != NULL)
		{
			nScreenHeight = g_pFramework->GetViewer()->GetViewHeight(); 

			nPosY = (INT32)(nScreenHeight - (38+offset)*i3GuiRoot::getHeightRatioBy768());
		}
		
		// Chat배경
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_INPUTBOX, 
			(10.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY - m_nChatBoxOffsetHeight, 
			(405.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(28.0f)*i3GuiRoot::getHeightRatioBy768());

		// Chat Mode
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_MODE, 
			(389.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY+4.0f - m_nChatBoxOffsetHeight, 
			(21.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(21.0f)*i3GuiRoot::getHeightRatioBy768());

		// Chat Target
		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_TARGET_CHANGE_PREV, 
			(50.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY+(4.0f*i3GuiRoot::getHeightRatioBy768()) - m_nChatBoxOffsetHeight, 
			(5.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(5.0f)*i3GuiRoot::getHeightRatioBy768());

		m_pSpriteChat->SetRect( HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT, 
			(50.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY+(18.0f*i3GuiRoot::getHeightRatioBy768()) - m_nChatBoxOffsetHeight, 
			(5.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(5.0f)*i3GuiRoot::getHeightRatioBy768());
	}

	

	{
		INT32 nPosY = 0;
		if (g_pViewer != NULL)
		{
			nScreenHeight = g_pFramework->GetViewer()->GetViewHeight();

			nPosY = (INT32)(nScreenHeight - (56+offset)*i3GuiRoot::getHeightRatioBy768());
		}
	
		// 일반
		m_pSpriteChatChannel->SetRect( 0, 
			(12.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY - m_nChatBoxOffsetHeight, 
			(41.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(18.0f)*i3GuiRoot::getHeightRatioBy768());
		// 팀
		m_pSpriteChatChannel->SetRect( 1, 
			(54.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY - m_nChatBoxOffsetHeight, 
			(41.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(18.0f)*i3GuiRoot::getHeightRatioBy768());
		// 클랜
		m_pSpriteChatChannel->SetRect( 2, 
			(96.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY - m_nChatBoxOffsetHeight, 
			(41.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(18.0f)*i3GuiRoot::getHeightRatioBy768());
		// 귓말 & 답변
		m_pSpriteChatChannel->SetRect( 3, 
			(138.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(REAL32)nPosY - m_nChatBoxOffsetHeight, 
			(41.0f)*i3GuiRoot::getHeightRatioBy768(), 
			(18.0f)*i3GuiRoot::getHeightRatioBy768());

		for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++)
		{ 
			m_pTextChatChannel[i]->setPos( (UINT32)((12 + (42*i))*i3GuiRoot::getHeightRatioBy768()), 
				(UINT32)(nPosY - m_nChatBoxOffsetHeight) );
		}
	}

	// HUD에서 사용하는 사용자 채팅 박스(다른 사용자들의 채팅 내용이 나오는 곳)
	{
		m_pTextBox->setPos( (UINT32)(12*i3GuiRoot::getHeightRatioBy768()), 
			(UINT32)(nScreenHeight - ((198+offset)*i3GuiRoot::getHeightRatioBy768()) - m_nChatBoxOffsetHeight) );
	}

	// HUD에서 사용하는 사용자 채팅창( 사용자가 입력하는곳)
	{
		m_pTextChat->setPos( (UINT32)(60*i3GuiRoot::getHeightRatioBy768()), 
			(UINT32)(nScreenHeight - ((29+offset)*i3GuiRoot::getHeightRatioBy768()) - m_nChatBoxOffsetHeight) );
		m_pTextTarget->setPos( (UINT32)(18*i3GuiRoot::getHeightRatioBy768()), 
			(UINT32)(nScreenHeight - ((31+offset)*i3GuiRoot::getHeightRatioBy768()) - m_nChatBoxOffsetHeight) );
	}
}

void CBattleChatBox::_UpdateChatCaret( REAL32 rDeltaSeconds)
{
	static REAL32 fOldCaretPosX = 0.0f;

	// Caret연산
	m_CaretLocalTime += rDeltaSeconds;
	if( m_CaretLocalTime > 0.5f)
	{
		m_pCaretSprite->SetEnable( 0, !(m_pCaretSprite->GetEnable(0)));
		m_CaretLocalTime = 0.0f;
	}

	INT32 nLength = 0;
	nLength = 0;
	if( m_pIME->getIME()->IsComp())
	{
		nLength+=1;
	}

	nLength += m_pIME->getIME()->GetCursorPosition();
	REAL32 fCaretPosX = (REAL32)m_pTextChat->getCaretPosX( nLength);

	if( fOldCaretPosX != fCaretPosX)
	{
		fOldCaretPosX = fCaretPosX;

		m_pCaretSprite->SetEnable( 0, TRUE);
		m_CaretLocalTime = 0.0f;
	}

	// Scroll 위치 조정
	{
		REAL32 x1, x2;

		x1 = 0.0f;
		x2 = (REAL32)m_pTextChat->getWidth();

		if( fCaretPosX < x1)
		{
			m_pTextChat->AdjustOffset( fCaretPosX - x1);		// minus로 들어가야 한다.
		}
		else if( fCaretPosX >= x2)
		{
			m_pTextChat->AdjustOffset( fCaretPosX - x2);		// plus
		}

		{
			REAL32 fWidth = 0.0f;
			INT32 nLength = 0;

			nLength = 0;
		
			//if( m_pIME->getIME()->IsComp())
			//{
			//	nLength+=1;
			//}

			nLength += m_pIME->getIME()->GetCursorPosition();
			fWidth = (REAL32)m_pTextChat->getCaretPosX( nLength);

			fCaretPosX = fWidth;
		}
	}

	if( m_pIME->getIME()->IsComp())
	{
		char szTemp[256] = {0,};
		if( m_pIME->getIME()->GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			m_pIME->m_pInputIME->GetComp( szTemp, 256);			
		}
		else
		{
			m_pIME->m_pInputIME->GetCompToIndex( szTemp, 256);
		}

		REAL32 fCharaWidth = (REAL32)m_pTextChat->getCharacterSizeX(szTemp);
		REAL32 fTextWidth = (REAL32)m_pTextChat->CalcTextWidth(szTemp);// - fCharaWidth;

		if( m_pIME->getIME()->GetCodePage() == I3_LANG_CODE_KOREAN)
		{
			m_pCaretSprite->SetSize(	0, m_CaretWidth + fCharaWidth, m_CaretHeight);
			m_pCaretSprite->SetCenter(	0, (m_pTextChat->getPosX()) + fCaretPosX + (m_CaretWidth * 0.5f) + fTextWidth - fCharaWidth * 0.5f ,
										(m_pTextChat->getPosY()) + (m_CaretHeight * 0.5f),
										0.0f);
		}
		else
		{
			m_pCaretSprite->SetSize(	0, m_CaretWidth, m_CaretHeight);
			m_pCaretSprite->SetCenter(	0, (m_pTextChat->getPosX()) + fCaretPosX + (m_CaretWidth * 0.5f) + fTextWidth,
										(m_pTextChat->getPosY()) + (m_CaretHeight * 0.5f),
										0.0f);
		}

		{
			VEC3D caretStart;
			m_pCaretSprite->GetLeftTop(0, &caretStart);

			m_pIME->SetWindowPosition((INT32)(caretStart.x - fTextWidth), (INT32)caretStart.y);
		}
	}
	else
	{

		m_pCaretSprite->SetSize(	0, m_CaretWidth, m_CaretHeight);
		m_pCaretSprite->SetCenter(	0, (m_pTextChat->getPosX()) + fCaretPosX + (m_CaretWidth * 0.5f),
									(m_pTextChat->getPosY()) + (m_CaretHeight * 0.5f) - 3.0f,
									0.0f);

		{
			VEC3D caretStart;
			m_pCaretSprite->GetLeftTop(0, &caretStart);

			m_pIME->SetWindowPosition((INT32)caretStart.x, (INT32)caretStart.y);
		}
	}

	
}

const char *	CBattleChatBox::_GetKeyboardValue(GAME_KEY_STRUCT *pGameKey)
{

	if( pGameKey->_nDiviceType == GAME_DEVICE_KEYBOARD)
	{
		switch( pGameKey->_nState)
		{
			case I3I_KEY_UNKNOWN:					return	GAME_STRING("STR_TBL_CHAT_KEY_UNKNOWN");/*[사용안함]*/

			case I3I_KEY_0:							return 	"0";
			case I3I_KEY_1:							return 	"1";
			case I3I_KEY_2:							return 	"2";
			case I3I_KEY_3:							return 	"3";
			case I3I_KEY_4:							return 	"4";
			case I3I_KEY_5:							return 	"5";
			case I3I_KEY_6:							return 	"6";
			case I3I_KEY_7:							return 	"7";
			case I3I_KEY_8:							return 	"8";
			case I3I_KEY_9:							return 	"9";

			case I3I_KEY_A:							return 	"A";
			case I3I_KEY_B:							return 	"B";
			case I3I_KEY_C:							return 	"C";
			case I3I_KEY_D:							return 	"D";
			case I3I_KEY_E:							return 	"E";
			case I3I_KEY_F:							return 	"F";
			case I3I_KEY_G:							return 	"G";
			case I3I_KEY_H:							return 	"H";
			case I3I_KEY_I:							return 	"I";
			case I3I_KEY_J:							return 	"J";
			case I3I_KEY_K:							return 	"K";
			case I3I_KEY_L:							return 	"L";
			case I3I_KEY_M:							return 	"M";
			case I3I_KEY_N:							return 	"N";
			case I3I_KEY_O:							return 	"O";
			case I3I_KEY_P:							return 	"P";
			case I3I_KEY_Q:							return 	"Q";
			case I3I_KEY_R:							return 	"R";
			case I3I_KEY_S:							return 	"S";
			case I3I_KEY_T:							return 	"T";
			case I3I_KEY_U:							return 	"U";
			case I3I_KEY_V:							return 	"V";
			case I3I_KEY_W:							return 	"W";
			case I3I_KEY_X:							return 	"X";
			case I3I_KEY_Y:							return 	"Y";
			case I3I_KEY_Z:							return 	"Z";

			case I3I_KEY_ESC:						return 	"ESC";
			case I3I_KEY_ENTER:						return 	"Enter";
			case I3I_KEY_LCTRL:						return 	"L-Ctrl";
			case I3I_KEY_RCTRL:						return 	"R-Ctrl";
			case I3I_KEY_LSHIFT:					return 	"L-Shift";
			case I3I_KEY_RSHIFT:					return 	"R-Shift";
			case I3I_KEY_LALT:						return 	"L-Alt";
			case I3I_KEY_RALT:						return 	"R-Alt";
			case I3I_KEY_SPACE:						return 	"Space";
			case I3I_KEY_LEFT:						return 	"Left";
			case I3I_KEY_RIGHT:						return 	"Right";
			case I3I_KEY_UP:						return 	"Up";
			case I3I_KEY_DOWN:						return 	"Down";
			case I3I_KEY_PGUP:						return 	"Page up";
			case I3I_KEY_PGDN:						return 	"Page down";
			case I3I_KEY_HOME:						return 	"Home";
			case I3I_KEY_END:						return 	"End";
			case I3I_KEY_DEL:						return 	"Del";
			case I3I_KEY_INS:						return 	"Insert";
			case I3I_KEY_TAB:						return 	"Tab";
			case I3I_KEY_NUMLOCK:					return 	"Num Lock";
			case I3I_KEY_CAPSLOCK:					return 	"Caps Loct";
			case I3I_KEY_SCROLLLOCK:				return 	"Scroll Lock";
			case I3I_KEY_BREAK:						return 	"Break";
			case I3I_KEY_PRTSC:						return 	"Print Screen";
			case I3I_KEY_WIN:						return 	"Window Key";
			case I3I_KEY_BACK:						return 	"Back Space";

			case I3I_KEY_F1:						return 	"F1";
			case I3I_KEY_F2:						return 	"F2";
			case I3I_KEY_F3:						return 	"F3";
			case I3I_KEY_F4:						return 	"F4";
			case I3I_KEY_F5:						return 	"F5";
			case I3I_KEY_F6:						return 	"F6";
			case I3I_KEY_F7:						return 	"F7";
			case I3I_KEY_F8:						return 	"F8";
			case I3I_KEY_F9:						return 	"F9";
			case I3I_KEY_F10:						return 	"F10";
			case I3I_KEY_F11:						return 	"F11";
			case I3I_KEY_F12:						return 	"F12";

			case I3I_KEY_NUMPAD0:					return 	"Numpad 0";
			case I3I_KEY_NUMPAD1:					return 	"Numpad 1";
			case I3I_KEY_NUMPAD2:					return 	"Numpad 2";
			case I3I_KEY_NUMPAD3:					return 	"Numpad 3";
			case I3I_KEY_NUMPAD4:					return 	"Numpad 4";
			case I3I_KEY_NUMPAD5:					return 	"Numpad 5";
			case I3I_KEY_NUMPAD6:					return 	"Numpad 6";
			case I3I_KEY_NUMPAD7:					return 	"Numpad 7";
			case I3I_KEY_NUMPAD8:					return 	"Numpad 8";
			case I3I_KEY_NUMPAD9:					return 	"Numpad 9";
			case I3I_KEY_NUMPADDIVIDE:				return 	"Numpad Div";
			case I3I_KEY_NUMPADMULTIPLY:			return 	"Numpad Mul";
			case I3I_KEY_NUMPADMINUS:				return 	"Numpad Minus";
			case I3I_KEY_NUMPADPLUS:				return 	"Numpad Plus";
			case I3I_KEY_NUMPADENTER:				return 	"Numpad Enter";
			case I3I_KEY_NUMPADDEL:					return 	"Numpad Del";
													
			case I3I_KEY_MINUS:						return 	"-";
			case I3I_KEY_EQUAL:						return 	"=";
			case I3I_KEY_BACKSLASH:					return 	"\\";
			case I3I_KEY_LBRACKET:					return 	"[";
			case I3I_KEY_RBRACKET:					return 	"]";
			case I3I_KEY_SEMICOLON:					return 	";";
			case I3I_KEY_APOSTROPHE:				return 	"'";
			case I3I_KEY_COMMA:						return 	",";
			case I3I_KEY_PERIOD:					return 	".";
			case I3I_KEY_SLASH:						return 	"/";
			case I3I_KEY_GRAVE:						return	"~";
		}
	}
	else if( pGameKey->_nDiviceType == GAME_DEVICE_MOUSE)
	{
		switch(pGameKey->_nState)
		{
		case I3I_MOUSE_LBUTTON:						return GAME_STRING("STR_TBL_CHAT_MOUSE_LBUTTON");/*마우스 좌측버튼*/
		case I3I_MOUSE_RBUTTON:						return GAME_STRING("STR_TBL_CHAT_MOUSE_RBUTTON");/*마우스 우측버튼*/
		case I3I_MOUSE_MBUTTON:						return GAME_STRING("STR_TBL_CHAT_MOUSE_MBUTTON");/*마우스 휠버튼*/
		case I3I_MOUSE_WHEELUP:						return GAME_STRING("STR_TBL_CHAT_MOUSE_WHEEL_UP");/*마우스휠 올리기*/
		case I3I_MOUSE_WHEELDOWN:					return GAME_STRING("STR_TBL_CHAT_MOUSE_WHEEL_DOWN");/*마우스휠 내리기*/
		}
	}

	return GAME_STRING("STR_TBL_CHAT_KEY_UNKNOWN");/*[사용안함]*/
}

void CBattleChatBox::UpdateWhisperBackupBuffer(void)
{
	INT32 i = 0;
	INT32 index = 0;
	
	for( i=0; i<g_pFramework->getChatBox()->GetWhisperBufferCount(); i++)
	{
		index = (g_pFramework->getChatBox()->GetCurrentWhisperBufferIndex() + i) % 5;
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatBox()->GetWhisperBufferText(index));
	}

	g_pFramework->getChatBox()->ResetWhisperBuffer();
}

void CBattleChatBox::_SetChatChannelType(CHAT_CHANNEL_TYPE type)
{
	COLOR col;
	INT32 nSelectedIndex = (INT32)type;

	// 초기화
	for(UINT32 i = 0; i < CHAT_CHANNEL_TYPE_COUNT; i++)
	{
		m_pSpriteChatChannel->SetTextureCoord( i, (861.0f), (38.0f), (902.0f), (56.0f));
		m_pTextChatChannel[i]->SetColor( 204, 204, 204, 255);
	}

	switch( type)
	{
	case CHAT_CHANNEL_TYPE_ALL:	// 일반
		m_nCurrentChatBufferType = CHATTING_TYPE_ALL;
		i3Color::Set( &col,(UINT8)  255, 255, 255, 255);
		break;
	case CHAT_CHANNEL_TYPE_TEAM:// 팀
		m_nCurrentChatBufferType = CHATTING_TYPE_TEAM;
		i3Color::Set( &col, (UINT8) 98, 180, 253, 255);
		break;
	case CHAT_CHANNEL_TYPE_CLAN:// 클랜
		m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
		i3Color::Set( &col, (UINT8) 80, 255, 60, 255);
		break;
	case CHAT_CHANNEL_TYPE_WHISPER:// 귓말 & 답변
		m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
		i3Color::Set( &col, (UINT8) 237, 86, 250, 255);
		break;
	}

	m_pSpriteChatChannel->SetTextureCoord( nSelectedIndex, (819.0f), (38.0f), (860.0f), (56.0f));
	m_pTextChatChannel[nSelectedIndex]->SetColor(&col);
}

bool CBattleChatBox::IsGhostMessage( void)
{
	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
		return true;
#endif

	CHud* pHud = CHud::i();
	I3ASSERT( pHud != NULL);

	if( ((g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode()) &&
		(g_pGameContext->getUserDeath( mySlotIdx) == true)) || pHud->getHudMode() == HUD_MODE_MISSION_OBSERVER)
		return true;

	return false;
}

#endif