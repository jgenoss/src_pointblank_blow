#include "pch.h"

/*

#include "MessageBox.h"

#include "WebControl.h"
#include "GlobalVariables.h"
#include "MessageBoxManager.h"
#include "GameGUI.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif

I3_CLASS_INSTANCE(CMessageBox);

i3Texture* CMessageBox::m_pTexture = nullptr;

CMessageBox::CMessageBox()
{
	m_pSceneNode = i3Node::new_object();

	m_bInited = false;

	m_pWindow = nullptr;
	m_pSeparate = nullptr;

	i3mem::FillZero(m_pButton, sizeof(m_pButton));
	i3mem::FillZero(m_pText, sizeof(m_pText));

	m_nX = 0;
	m_nY = 0;
	
	m_nWidth = DefaultWidth;
	m_nHeight = DefaultHeight;

	m_style				= MBS_OK;
	m_pManager			= nullptr;
	m_nCapturedButton	= BT_MAX;
	m_bComplate			= false;
	m_nParam			= 0;
	m_pThis				= nullptr;
	m_bInputEnable		= true;
	m_bFirst			= true;
	m_CheckCRC			= 0;

	i3mem::FillZero( m_pBtnClickSound, sizeof( i3SoundPlayInfo*) * (BT_MAX + 1));

	bCreatedWebBilling	= false;
	bShowWebBilling		= false;
	m_WebScreenSize.w = m_WebScreenSize.h	= 0;

	m_hExplorerhWnd = nullptr;
}

CMessageBox::~CMessageBox()
{
	for(UINT32 i = 0; i < TT_MAX; i++)
	{
		I3_SAFE_RELEASE(m_pText[i]);
	}

	for(UINT32 i = 0; i < BT_MAX; i++)
	{
		I3_SAFE_RELEASE(m_pButton[i]);
	}

	I3_SAFE_RELEASE(m_pSeparate);
	I3_SAFE_RELEASE(m_pWindow);

	if (m_bInited)
	{
		I3_SAFE_RELEASE(m_pTexture);
	}

	for( INT32 i =0; i < BT_MAX + 1; ++i)
	{
		I3_SAFE_RELEASE( m_pBtnClickSound[i]);
	}

	if( bCreatedWebBilling)
	{
		::SetFocus(g_pViewer->GetWindowHandle());
		UninitWebCtrl();
		::DestroyWindow( m_hExplorerhWnd);
	}
}

bool CMessageBox::Create(i3Node* pParentNode, CMessageBoxManager* pManager, const MsgBoxOpenParam & Param)
{
	UINT32 style = Param.Style;

	if (false == i3GameObjBase::Create(pParentNode, true))
	{
		return false;
	}

	// 텍스처가 없다면 생성
	if (nullptr == m_pTexture)
	{
		m_pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Main/UI_Source01.i3i");
	}

	// 생성 실패
	if (nullptr == m_pTexture)
	{
		return false;
	}	

	// 생성에 성공하면 초기화되었다
	m_bInited = true;	
	m_pManager = pManager;

	if (TESTBIT(style, MBF_WAIT))
	{
		m_style = MBS_WAIT;
	}
	else if(TESTBIT(style, MBF_OKCANCEL))
	{
		m_style = MBS_OKCANCEL;
	}
	else if(TESTBIT(style, MBF_USER_OKCANCEL))
	{
		m_style = MBS_USER_OKCANCEL;
	}
	else if(style == MBF_WEB)
	{
		m_style = MBS_WEB;
		CreateWeb(Param.WebURL.c_str(), Param.WebScreenSize);
	}
	else
	{
		m_style = MBS_OK;
	}


	// 윈도우 스프라이트 생성
	m_pWindow = i3Sprite2D::new_object();
#if USE_VERTEX_BANK
	m_pWindow->Create(WS_MAX, true, false, true);
#else
	m_pWindow->Create(WS_MAX, true, false, false);
#endif
	m_pWindow->SetTexture(m_pTexture);
	m_pWindow->SetTextureCoord(WS_LEFT_TOP, 1, 84, 27, 142);
	m_pWindow->SetTextureCoord(WS_CENTER_TOP, 28, 84, 54, 142);
	m_pWindow->SetTextureCoord(WS_RIGHT_TOP, 55, 84, 81, 142);
	m_pWindow->SetTextureCoord(WS_LEFT_MIDDLE, 1, 143, 27, 169);
	m_pWindow->SetTextureCoord(WS_CENTER_MIDDLE, 28, 143, 54, 169);
	m_pWindow->SetTextureCoord(WS_RIGHT_MIDDLE, 55, 143, 81, 169);	
	m_pWindow->SetTextureCoord(WS_LEFT_BOTTOM, 1, 170, 27, 250);
	m_pWindow->SetTextureCoord(WS_CENTER_BOTTOM, 28, 170, 54, 250);
	m_pWindow->SetTextureCoord(WS_RIGHT_BOTTOM, 55, 170, 81, 250);

	m_pWindow->SetEnable(true);
	
	GetNode()->AddChild(m_pWindow);

	// 윈도우 가로선 스프라이트 생성
	m_pSeparate = i3Sprite2D::new_object();
#if USE_VERTEX_BANK
	m_pSeparate->Create(1, true, false, true);
#else
	m_pSeparate->Create(1, true, false, false);
#endif
	m_pSeparate->SetTexture(m_pTexture);
	m_pSeparate->SetTextureCoord(0, 158, 159, 184, 161);
	m_pSeparate->SetEnable(true);

	GetNode()->AddChild(m_pSeparate);


	// 버튼 스프라이트 생성
	for(UINT32 i = 0; i < BT_MAX; i++)
	{
		m_pButton[i] = i3Sprite2D::new_object();
	#if USE_VERTEX_BANK
		m_pButton[i]->Create(BS_MAX, true, false, true);
	#else
		m_pButton[i]->Create(BS_MAX, true, false, false);
	#endif
		m_pButton[i]->SetTexture(m_pTexture);
 		m_pButton[i]->SetTextureCoord(BS_NORMAL, 245, 172, 308, 198);
		m_pButton[i]->SetTextureCoord(BS_ON_MOUSE, 245, 197, 308, 223);
		m_pButton[i]->SetTextureCoord(BS_CLICKED, 245, 222, 308, 248);
		m_pButton[i]->SetTextureCoord(BS_DISABLE, 157, 297, 220, 323);

		GetNode()->AddChild(m_pButton[i]);
	}

	REAL32 fScreenRate = m_pManager->GetScreenRate();
	UINT32 TextWeight[TT_MAX];
	for ( INT32 i = 0 ; i < TT_MAX ; ++i )
		TextWeight[i] = FW_NORMAL;

#if defined(ENABLE_BOLD_CAPTION)
	TextWeight[TT_CAPTION] = FW_BOLD;
#endif

	// Text 생성
	for(UINT32 i = 0; i < TT_MAX; i++)
	{
		m_pText[i] = i3TextNodeDX2::new_object();
	#if USE_VERTEX_BANK
		m_pText[i]->Create( GetDefaultFontName(), MaxString, (INT32) (GAME_FONT_DEFAULT_SIZE * fScreenRate), TextWeight[i], 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, true);
	#else
		m_pText[i]->Create( GetDefaultFontName(), MaxString, (INT32) (GAME_FONT_DEFAULT_SIZE * fScreenRate), TextWeight[i], 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, false);
	#endif
		m_pText[i]->SetColor(GameGUI::GetColor(GCT_DEFAULT));
		m_pText[i]->setAutoWrapEnable(false);
		m_pText[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

		//m_pText[i]->setWidth((UINT32)(m_pText[i]->getWidth() * fScreenRate));
		//m_pText[i]->setHeight((UINT32)(m_pText[i]->getHeight() * fScreenRate));
		m_pText[i]->setWidth((UINT32)(1024 * fScreenRate));
		m_pText[i]->setHeight((UINT32)(1024 * fScreenRate));
		
		GetNode()->AddChild(m_pText[i]);
	}

	// Text 기본 설정
	m_pText[TT_CAPTION]->SetText( GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"));		//	"알림"
	m_pText[TT_MESSAGE]->SetText(L"");
	m_pText[TT_OK]->SetText( GAME_RCSTRING("STBL_IDX_BUTTON_OK"));			//	"확인"
	m_pText[TT_CANCEL]->SetText(GAME_RCSTRING("STBL_IDX_BUTTON_CANCEL"));	//	"취소"
	m_pText[TT_USER]->SetText( L"");

	m_CheckCRC = 0;

	// 스타일에 따른 버튼 설정
	for(UINT32 i = 0; i < BT_MAX; i++)
	{
		m_pButton[i]->SetEnable(false);
	}

	m_pText[TT_OK]->SetEnable(false);
	m_pText[TT_CANCEL]->SetEnable(false);
	m_pText[TT_USER]->SetEnable(false);

	switch(m_style)
	{
	case MBS_WEB:
		m_pButton[BT_OK]->SetEnable(BS_NORMAL, true);
		m_pText[TT_OK]->SetEnable(true);
		break;
	case MBS_USER_OKCANCEL:
		m_pButton[BT_USER]->SetEnable(BS_NORMAL, true);
		m_pText[TT_USER]->SetEnable(true);
		// fall through
	case MBS_OKCANCEL:
		m_pButton[BT_CANCEL]->SetEnable(BS_NORMAL, true);
		m_pText[TT_CANCEL]->SetEnable(true);
		// fall through
	case MBS_OK:
		m_pButton[BT_OK]->SetEnable(BS_NORMAL, true);
		m_pText[TT_OK]->SetEnable(true);
		break;
	default:
		break;
	}

	{
		static char * s_szclicksnd[] = 
		{
			"UI_Button_Click_Ok.wav",
			"UI_Button_Click_Cancel.wav",
			"UI_Button_Click_Ok.wav",
			"UI_Button_Click.wav",
		};

		for( INT32 i=0;i < BT_MAX + 1; ++i)
		{
			I3ASSERT( m_pBtnClickSound[i] == nullptr);

			m_pBtnClickSound[i] = i3SoundPlayInfo::new_object();

			i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( s_szclicksnd[i]);

			I3ASSERT( pRes != nullptr);

			i3SoundPlayInfo * pSndResInfo = pRes->getSound();
			if( pSndResInfo != nullptr)
			{
				pSndResInfo->CopyTo( m_pBtnClickSound[i], I3_COPY_INSTANCE);
			}
		}
	}

	return true;
}

void CMessageBox::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	// 키입력 중복을 막기위한, 1 frame 스킵
	if(m_bFirst)
	{
		m_bFirst = false;
		return;
	}


	// 처리가 완료되었으면 처리하지 않는다
	if (m_bComplate)
	{
		return;
	}


	// 키보드 입력처리가 막혀있다면 처리하지 않는다
	if( false == m_bInputEnable)
	{
		return;
	}


	// 버튼이 없다면 처리하지 않는다
	if (MBS_WAIT == m_style)
	{
		return;
	}


	// 마우스 상태를 가져온다
	INT32 msInputState = m_pManager->GetInputState();

	if (false == TESTBIT(msInputState, MBIS_LBUTTON_PRESS))
	{
		m_nCapturedButton = BT_MAX;
	}


	// 키보드 상태 확인
	if (TESTBIT(msInputState, MBIS_ENTER_DOWN))
	{
		_Complate(BT_OK);
		return;
	}
	else if (TESTBIT(msInputState, MBIS_ESC_DOWN))
	{
		if (MBS_OK == m_style)
		{
			_Complate(BT_OK);
		}
		else  
		{
			_Complate(BT_CANCEL);
		}

		return;
	}


	// 버튼 상태 확인
	for(UINT32 i = 0; i < BT_MAX; i++)
	{
		// 없는 버튼은 처리 하지 않는다.
		if (MBS_OK == m_style && BT_CANCEL == i)
		{
			continue;
		}

		// 클릭 처리
		if (_IsOnMouse((ButtonType)i) && TESTBIT(msInputState, MBIS_LBUTTON_DOWN))
		{
			m_nCapturedButton = i;
		}
		if (_IsOnMouse((ButtonType)i) && TESTBIT(msInputState, MBIS_LBUTTON_UP))
		{
			_Complate(i);
		}

		// 버튼 상태
		if (_IsOnMouse((ButtonType)i) && TESTBIT(msInputState, MBIS_LBUTTON_PRESS) && i == m_nCapturedButton)
		{
			if( !m_pButton[i]->GetEnable( BS_CLICKED))
			{
				i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
				if( pSndCtx != nullptr)
				{
					//if( MBS_OK == m_style )
					//	pSndCtx->Play( m_pBtnClickSound[BT_MAX]);
					//else
						pSndCtx->Play( m_pBtnClickSound[i]);
				}
			}

			// 마우스 왼버튼이 눌려있으면 클리키드
			m_pButton[i]->SetEnable(false);
			m_pButton[i]->SetEnable(BS_CLICKED, true);

			
		}
		else if (_IsOnMouse((ButtonType)i) && !TESTBIT(msInputState, MBIS_LBUTTON_PRESS) &&
				 (BT_MAX == m_nCapturedButton || i == m_nCapturedButton))
		{
			// 아니면 온마우스
			m_pButton[i]->SetEnable(false);
			m_pButton[i]->SetEnable(BS_ON_MOUSE, true);			
		}
		else
		{
			// 일반상태
			m_pButton[i]->SetEnable(false);
			m_pButton[i]->SetEnable(BS_NORMAL, true);
		}
	}
}

void CMessageBox::ShowEx(const char* strMessage,const char* strCaption, const CMsgBoxCallBack & fnEventCB,void*pThis, INT32 nAlignX, INT32 nAlignY)
{
	m_pCallBackFunc = fnEventCB;
	m_pThis = pThis;

	_SetText(strMessage, strCaption, nAlignX, nAlignY);

//	if (i3::generic_string_size(strMessage) < 100)
//	{
//		_SetSize(ERROR_POPUP_WIDTH, ERROR_POPUP_HEIGHT);
//	}
//	else
//	{
//		_SetSize((UINT32)(ERROR_POPUP_WIDTH * 1.2f), (UINT32)(ERROR_POPUP_HEIGHT * 1.2f));
//	}

	// Shape 크기 재계산
	_RecalcShape();
}

void CMessageBox::Hide(void)
{
	if( bCreatedWebBilling && bShowWebBilling) 
	{
		// 캐쉬 정보 얻는 이벤트를 	
		GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);

		::ShowWindow(m_hExplorerhWnd,SW_HIDE);
		bShowWebBilling = false;
	}

	SetEnable(false);
	
	GetNode()->RemoveFromParent();
	getParent()->RemoveChild(this);
}

bool CMessageBox::IsComplate(void) const
{
	return m_bComplate;
}

void CMessageBox::Notify(void) const
{
	if( m_pThis == nullptr)
		return;
	
	m_pCallBackFunc.OnCallback(m_pThis,m_nParam);
}

void CMessageBox::SetInputEnable(bool bEnable)
{
	m_bInputEnable = bEnable;
}

void CMessageBox::SetPosition(INT32 x,INT32 y)
{
	// 현재 위치와 같다면 처리하지 않는다
	if (x == m_nX && y == m_nY)
	{
		return;
	}

	// 값 설정
	m_nX = x;
	m_nY = y;

	_RecalcShape();
}

void CMessageBox::Resize(void)
{
	// Shape 크기 재계산
	_RecalcShape();
}

void CMessageBox::ChangeMessage(const char* strMessage,const char* strCaption)
{
	_SetText(strMessage, strCaption);

	// Shape 크기 재계산
	_RecalcShape();
}

void CMessageBox::ChangeButton(const i3::wliteral_range& wUser, const i3::wliteral_range& wOK, const i3::wliteral_range& wCancel)
{
	if( !wUser.empty())		m_pText[TT_USER]->SetText(wUser);
	if( !wOK.empty() )		m_pText[TT_OK]->SetText(wOK);
	if( !wCancel.empty() )	m_pText[TT_CANCEL]->SetText(wCancel);
}

void CMessageBox::_SetText(const char* strMessage,const char* strCaption, INT32 nAlignX, INT32 nAlignY)
{
	//	동일 메시지 체크용 CRC
	INT32 Length = i3::generic_string_size(strMessage);
	m_CheckCRC = CRC32(0xFFFFFFFF, (UINT8*) strMessage, Length);

	i3::wstring wstrMessage;
	i3::mb_to_utf16(strMessage, wstrMessage);

	m_pText[TT_MESSAGE]->SetText(wstrMessage);
	m_pText[TT_MESSAGE]->SetAlign(nAlignX, nAlignX);

	if (strCaption)
	{
		i3::wstring wstrCaption;
		i3::mb_to_utf16(strCaption, wstrCaption);
		m_pText[TT_CAPTION]->SetText(wstrCaption);
	}

	i3TextAttrDX2* pAttr = m_pText[TT_MESSAGE]->getTextAttrDX();
	m_nTextWidth = pAttr->getTextWidth();
	m_nTextHeight = pAttr->getTextHeight();

	const char * strLineCheck = strMessage;
	char * strColorRemovedMessage = nullptr;
	if ( Length > 0 )
	{
		strColorRemovedMessage = (char *)i3MemAlloc(sizeof(char) * Length);
		GameGUI::RemoveColorKeyword(strMessage, strColorRemovedMessage, Length);
		strLineCheck = strColorRemovedMessage;
	}

	//	멀티라인일 경우 처리
	{
		static char temp[4096] = "";
		static char szMost[2048] = "";

		INT32 line = 1, idx = 0;
		INT32 most = 0;

		const char * pszChar = strLineCheck;
		i3mem::FillZero( temp, sizeof( temp));
	
		while(*pszChar)
		{
			if( idx < 4096)
			{
				//	캐리지 리턴일 경우 한 문장으로 인식한다.
				if( *pszChar == '\n')
				{
					temp[idx] = 0;

					//	가장 긴 문장을 찾는다.
					if( most < idx)
					{
						most = idx;
					
						i3::string_ncopy_nullpad( szMost, temp, sizeof(szMost));
					}

					idx = 0;
					line++;
				}
				else
				{
					temp[idx] = *pszChar;

					idx++;
				}				
			}

			pszChar++;
		}

		//	가장 긴 문장을 찾는다.
		if( most < idx)
		{
			most = idx;

			i3::string_ncopy_nullpad( szMost, temp, sizeof(szMost));
		}

		if(line > 1)
		{
			//	가장 긴 문자을 기준으로 넓이를 계산한다.
			m_nTextWidth = pAttr->CalcTextWidth( szMost);

			//	멀티라인
			m_nTextHeight *= line;
		}
	}

	I3MEM_SAFE_FREE(strColorRemovedMessage);
}

void CMessageBox::_RecalcShape(void)
{
	REAL32 fScreenRate = m_pManager->GetScreenRate();

	const REAL32 fGapBetweenFrame = 10.0f;
	
	if( m_style == MBS_WEB)
	{
		// Text가 들어갈 공간 구하기

		INT32 nTextWidth = m_WebScreenSize.w - 20 + (INT32)( (fGapBetweenFrame * 2.0f));	//	끝 문자와 윈도우와의 사이 간격을 띄워줍니다.
		INT32 nTextHeight = m_WebScreenSize.h - 30 + (INT32)( fGapBetweenFrame);			//	끝 문자와 윈도우와의 사이 간격을 띄워줍니다.;

		// CenterMiddle Shape 크기 구하기
		REAL32 centerWidth = ((REAL32)MAX((REAL32) nTextWidth, (REAL32) DefaultWidth));
		REAL32 centerHeight = ((REAL32)MAX((REAL32) nTextHeight, (REAL32) DefaultHeight));

		// 윈도우 각 Shape의 Offset 좌표
		REAL32 offsetX[3];
		REAL32 offsetY[3];

		offsetX[0] = (REAL32)m_nX;
		offsetX[1] = offsetX[0] + (26.0f);
		offsetX[2] = offsetX[1] + centerWidth;  

		offsetY[0] = (REAL32)m_nY;
		offsetY[1] = offsetY[0] + (58.0f);
		offsetY[2] = offsetY[1] + centerHeight;

		// 윈도우 위치 지정
		m_pWindow->SetRect(WS_LEFT_TOP, offsetX[0], offsetY[0], 26.0f, 58.0f);
		m_pWindow->SetRect(WS_CENTER_TOP, offsetX[1], offsetY[0], centerWidth, 58.0f);
		m_pWindow->SetRect(WS_RIGHT_TOP, offsetX[2], offsetY[0], 26.0f, 58.0f);

		m_pWindow->SetRect(WS_LEFT_MIDDLE, offsetX[0], offsetY[1], 26.0f, centerHeight);
		m_pWindow->SetRect(WS_CENTER_MIDDLE, offsetX[1], offsetY[1], centerWidth, centerHeight);
		m_pWindow->SetRect(WS_RIGHT_MIDDLE, offsetX[2], offsetY[1], 26.0f, centerHeight);

		m_pWindow->SetRect(WS_LEFT_BOTTOM, offsetX[0], offsetY[2], 26.0f, 80.0f);
		m_pWindow->SetRect(WS_CENTER_BOTTOM, offsetX[1], offsetY[2], centerWidth, 80.0f);
		m_pWindow->SetRect(WS_RIGHT_BOTTOM, offsetX[2], offsetY[2], 26.0f, 80.0f);


		// 가로선 위치 지정
		REAL32 totalWidth = (26) + centerWidth + (26) - (18);
		m_pSeparate->SetRect(0, offsetX[0] + (9), offsetY[2] + (22), totalWidth, 2.0f);	

		// 버튼 위치 지정 - 위치 다시 잡을 것
		switch(m_style)
		{
		case MBS_WEB:
			for(UINT32 i = 0; i < BS_MAX; i++)
			{
				m_pButton[BT_OK]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) - (30), 
											offsetY[2] + (33) , 
											60.0f, 
											23.0f);
			}
			m_pText[TT_OK]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) - (30)), 
									(UINT32)(offsetY[2] + (33)));
			m_pText[TT_OK]->setSize((UINT32)(60), (UINT32)(23));
			break;		
		}
		
		// Text 위치 지정	
		m_pText[TT_CAPTION]->setPos((UINT32)offsetX[0], (UINT32)offsetY[0]);
		m_pText[TT_CAPTION]->setSize((UINT32)((26) + centerWidth + (26)), 
									(UINT32)(48));

		m_pText[TT_MESSAGE]->setPos((UINT32)(offsetX[1] - (8)), 
									(UINT32)(offsetY[1] - (19)));
		m_pText[TT_MESSAGE]->setSize((UINT32)(centerWidth + (16)), 
									(UINT32)(centerHeight + (38)));

		VEC2D * pvLeft = m_pWindow->GetSize( WS_LEFT_MIDDLE);
		VEC2D * pvCenter = m_pWindow->GetSize( WS_CENTER_MIDDLE);
		VEC2D * pvRight = m_pWindow->GetSize( WS_RIGHT_MIDDLE);
		VEC2D * pvTop = m_pWindow->GetSize( WS_CENTER_TOP);
		VEC2D * pvBottom = m_pWindow->GetSize( WS_CENTER_BOTTOM);

		m_nWidth = (UINT32) (pvLeft->x + pvCenter->x + pvRight->x);
		m_nHeight = (UINT32) (pvTop->y + pvCenter->y + pvBottom->y);
	}
	else
	{
		// Text가 들어갈 공간 구하기
		INT32 nTextWidth = m_nTextWidth + (INT32)( (fGapBetweenFrame * 2.0f)* fScreenRate);	//	끝 문자와 윈도우와의 사이 간격을 띄워줍니다.
		INT32 nTextHeight = m_nTextHeight + (INT32)( fGapBetweenFrame* fScreenRate);	//	끝 문자와 윈도우와의 사이 간격을 띄워줍니다.;

		// CenterMiddle Shape 크기 구하기
		REAL32 centerWidth = ((REAL32)MAX((REAL32) nTextWidth, (REAL32) DefaultWidth * fScreenRate));
		REAL32 centerHeight = ((REAL32)MAX((REAL32) nTextHeight, (REAL32) DefaultHeight * fScreenRate));

		// 윈도우 각 Shape의 Offset 좌표
		REAL32 offsetX[3];
		REAL32 offsetY[3];

		offsetX[0] = (REAL32)m_nX;
		offsetX[1] = offsetX[0] + (26.0f * fScreenRate);
		offsetX[2] = offsetX[1] + centerWidth;  

		offsetY[0] = (REAL32)m_nY;
		offsetY[1] = offsetY[0] + (58.0f * fScreenRate);
		offsetY[2] = offsetY[1] + centerHeight;

		// 윈도우 위치 지정
		m_pWindow->SetRect(WS_LEFT_TOP, offsetX[0], offsetY[0], 
										26.0f * fScreenRate, 58.0f * fScreenRate);
		m_pWindow->SetRect(WS_CENTER_TOP, offsetX[1], offsetY[0], 
										centerWidth, 58.0f * fScreenRate);
		m_pWindow->SetRect(WS_RIGHT_TOP, offsetX[2], offsetY[0], 
										26.0f * fScreenRate, 58.0f * fScreenRate);

		m_pWindow->SetRect(WS_LEFT_MIDDLE, offsetX[0], offsetY[1], 
											26.0f * fScreenRate, centerHeight);
		m_pWindow->SetRect(WS_CENTER_MIDDLE, offsetX[1], offsetY[1], 
											centerWidth, centerHeight);
		m_pWindow->SetRect(WS_RIGHT_MIDDLE, offsetX[2], offsetY[1], 
											26.0f * fScreenRate, centerHeight);

		m_pWindow->SetRect(WS_LEFT_BOTTOM, offsetX[0], offsetY[2], 
											26.0f * fScreenRate, 80.0f * fScreenRate);
		m_pWindow->SetRect(WS_CENTER_BOTTOM, offsetX[1], offsetY[2], 
											centerWidth, 80.0f * fScreenRate);
		m_pWindow->SetRect(WS_RIGHT_BOTTOM, offsetX[2], offsetY[2], 
											26.0f * fScreenRate, 80.0f * fScreenRate);


		// 가로선 위치 지정
		REAL32 totalWidth = (26 * fScreenRate) + centerWidth + (26 * fScreenRate) - (18 * fScreenRate);

		m_pSeparate->SetRect(0, offsetX[0] + (9 * fScreenRate), 
								offsetY[2] + (22 * fScreenRate), 
								totalWidth, 
								2.0f * fScreenRate);	


		// 버튼 위치 지정 - 위치 다시 잡을 것
		switch(m_style)
		{
		case MBS_OK:
			for(UINT32 i = 0; i < BS_MAX; i++)
			{
				m_pButton[BT_OK]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) - (30 * fScreenRate), 
											offsetY[2] + (33* fScreenRate) , 
											60.0f * fScreenRate, 
											23.0f * fScreenRate);
			}
			m_pText[TT_OK]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) - (30 * fScreenRate)), 
									(UINT32)(offsetY[2] + (33 * fScreenRate)));
			m_pText[TT_OK]->setSize((UINT32)(60 * fScreenRate), (UINT32)(23 * fScreenRate));
			break;
		case MBS_OKCANCEL:
			for(UINT32 i = 0; i < BS_MAX; i++)
			{
				m_pButton[BT_OK]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) - (65 * fScreenRate), 
											offsetY[2] + (33 * fScreenRate) , 
											60.0f * fScreenRate, 
											23.0f * fScreenRate);
				m_pButton[BT_CANCEL]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) + (5 * fScreenRate), 
												offsetY[2] + (33 * fScreenRate) , 
												60.0f * fScreenRate, 
												23.0f * fScreenRate);
			}
			m_pText[TT_OK]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) - (65 * fScreenRate)), 
									(UINT32)(offsetY[2] + (33 * fScreenRate)));
			m_pText[TT_OK]->setSize((UINT32)(60 * fScreenRate), (UINT32)(23 * fScreenRate));
			m_pText[TT_CANCEL]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) + (5 * fScreenRate)), 
										(UINT32)(offsetY[2] + (33 * fScreenRate)));
			m_pText[TT_CANCEL]->setSize((UINT32)(60 * fScreenRate), (UINT32)(23 * fScreenRate));
			break;
		case MBS_USER_OKCANCEL:
			for(UINT32 i = 0; i < BS_MAX; i++)
			{
				m_pButton[BT_USER]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) - (90 * fScreenRate), 
											offsetY[2] + (33 * fScreenRate) , 
											60.0f * fScreenRate, 
											23.0f * fScreenRate);
				m_pButton[BT_OK]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) - (25 * fScreenRate), 
											offsetY[2] + (33 * fScreenRate) , 
											60.0f * fScreenRate, 
											23.0f * fScreenRate);
				m_pButton[BT_CANCEL]->SetRect(i, offsetX[1] + (centerWidth * 0.5f) + (40 * fScreenRate), 
												offsetY[2] + (33 * fScreenRate) , 
												60.0f * fScreenRate, 
												23.0f * fScreenRate);
			}
			m_pText[TT_USER]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) - (90 * fScreenRate)), 
									(UINT32)(offsetY[2] + (33 * fScreenRate)));
			m_pText[TT_USER]->setSize((UINT32)(60 * fScreenRate), (UINT32)(23 * fScreenRate));

			m_pText[TT_OK]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) - (25 * fScreenRate)), 
									(UINT32)(offsetY[2] + (33 * fScreenRate)));
			m_pText[TT_OK]->setSize((UINT32)(60 * fScreenRate), (UINT32)(23 * fScreenRate));

			m_pText[TT_CANCEL]->setPos((UINT32)(offsetX[1] + (centerWidth * 0.5f) + (40 * fScreenRate)), 
										(UINT32)(offsetY[2] + (33 * fScreenRate)));
			m_pText[TT_CANCEL]->setSize((UINT32)(60 * fScreenRate), (UINT32)(23 * fScreenRate));
			break;
		}
		
		// Text 위치 지정	
		m_pText[TT_CAPTION]->setPos((UINT32)offsetX[0], (UINT32)offsetY[0]);
		m_pText[TT_CAPTION]->setSize((UINT32)((26 * fScreenRate) + centerWidth + (26 * fScreenRate)), 
									(UINT32)(48 * fScreenRate));

		m_pText[TT_MESSAGE]->setPos((UINT32)(offsetX[1] - (8 * fScreenRate)), 
									(UINT32)(offsetY[1] - (19 * fScreenRate)));
		m_pText[TT_MESSAGE]->setSize((UINT32)(centerWidth + (16 * fScreenRate)), 
									(UINT32)(centerHeight + (38 * fScreenRate)));

		VEC2D * pvLeft = m_pWindow->GetSize( WS_LEFT_MIDDLE);
		VEC2D * pvCenter = m_pWindow->GetSize( WS_CENTER_MIDDLE);
		VEC2D * pvRight = m_pWindow->GetSize( WS_RIGHT_MIDDLE);
		VEC2D * pvTop = m_pWindow->GetSize( WS_CENTER_TOP);
		VEC2D * pvBottom = m_pWindow->GetSize( WS_CENTER_BOTTOM);

		m_nWidth = (UINT32) (pvLeft->x + pvCenter->x + pvRight->x);
		m_nHeight = (UINT32) (pvTop->y + pvCenter->y + pvBottom->y);
	}
}

void CMessageBox::_Complate(INT32 nParam)
{
	m_bComplate = true;
	m_nParam = nParam;	
}

bool CMessageBox::_IsOnMouse(ButtonType type)
{
	// 마우스 위치
	REAL32 rMouseX = m_pManager->GetInputPositionX();
	REAL32 rMouseY = m_pManager->GetInputPositionY();


	// 버튼 크기
	VEC3D vec;

	m_pButton[type]->GetLeftTop(BS_START, &vec);

	REAL32 rX = i3Vector::GetX(&vec);
	REAL32 rY = i3Vector::GetY(&vec);
	REAL32 rWidth = i3Vector::GetX(m_pButton[type]->GetSize(BS_START));
	REAL32 rHeight = i3Vector::GetY(m_pButton[type]->GetSize(BS_START));


	// 마우스가 버튼 위에 있으면 처리
	if (rX < rMouseX && rX + rWidth > rMouseX && rY < rMouseY && rY + rHeight > rMouseY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CMessageBox::CreateWeb(const wchar_t* URL, SIZE32 Size)
{
	m_WebScreenSize = Size;

	INT32	Left	= static_cast<INT32>(g_pViewer->GetViewWidth() * 0.5f - Size.w * 0.5f);
	INT32	Top		= static_cast<INT32>(g_pViewer->GetViewHeight() * 0.5f - Size.h * 0.5f - 10.f);

	if( bCreatedWebBilling)
	{
		::ShowWindow(m_hExplorerhWnd,SW_SHOW);
		::MoveWindow(m_hExplorerhWnd, Left, Top,Size.w, Size.h, TRUE);
	}
	else
	{
		InitWebCtrl();

		m_hExplorerhWnd = ::CreateWebControl( URL, WS_CLIPCHILDREN | WS_CHILD,
			Left, Top, Size.w, Size.h, g_pViewer->GetWindowHandle(), NULL);

		bCreatedWebBilling = true;

		::ShowWindow(m_hExplorerhWnd, SW_SHOW);
	}

	bShowWebBilling = true;
}

*/
