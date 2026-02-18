#include "pch.h"
#include "TestAutomationInterface.h"

#if defined( TEST_AUTOMATION)

#include "KeyString.h"
#include "UI/UIMainFrame.h"
#include "BattleSlotContext.h"
#include "GameCharaBoneContext.h"

#define		TM_TXTDISPLAY			3.5f

static bool s_OldKeyPressed[256];

TestAutomationInterface::TestAutomationInterface(void)
{
	m_pTextTitle = nullptr;
	m_pTextStatus = nullptr;
	m_pTextMsg = nullptr;

	m_tmTextDisplay = 0.0f;

	m_pCapture = nullptr;

	m_pInd = nullptr;
	m_pInd_Text = nullptr;


	memset(&m_MouseBtnPressed, 0, sizeof(m_MouseBtnPressed));

	memset(&s_OldKeyPressed, 0, sizeof(s_OldKeyPressed));

	m_MousePos.x = 0;
	m_MousePos.y = 0;
	m_bValidMousePos = false;
}

TestAutomationInterface::~TestAutomationInterface(void)
{
	I3_SAFE_NODE_RELEASE(m_pTextTitle);
	I3_SAFE_NODE_RELEASE(m_pTextStatus);
	I3_SAFE_NODE_RELEASE(m_pTextMsg);

	I3_SAFE_NODE_RELEASE( m_pInd);
	I3_SAFE_NODE_RELEASE( m_pInd_Text);

	I3_SAFE_RELEASE( m_pCapture);
}

bool TestAutomationInterface::Create(CGameFramework * pFramework)
{
	m_pCapture = CaseRunCapture::new_object();
	m_pCapture->Create(g_pFramework, this);
	 
	m_pTextTitle = i3TextNodeDX2::new_object();
	m_pTextTitle->Create("Tahoma", 64, 11, FW_BOLD, false, false, DEFAULT_QUALITY, true);
	m_pTextTitle->setAutoWrapEnable(true);
	m_pTextTitle->setSize(1024, 42);
	m_pTextTitle->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pTextTitle->SetColor(255, 127, 39, 255);
	
	m_pTextStatus = i3TextNodeDX2::new_object();
	m_pTextStatus->Create("굴림체", 512, 9, 0, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_HIGH);
	m_pTextStatus->setAutoWrapEnable(true);
	m_pTextStatus->setSize(1024, 42);
	m_pTextStatus->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pTextStatus->SetColor(255, 210, 115, 255);

	m_pTextMsg = i3TextNodeDX2::new_object();
	m_pTextMsg->Create("굴림체", 512, 9, 0, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_HIGH);
	m_pTextMsg->setAutoWrapEnable(true);
	m_pTextMsg->setSize(1024, 42);
	m_pTextMsg->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pTextMsg->SetColor(255, 171, 115, 255);

	g_pFramework->Get2DRoot()->AddChild(m_pTextTitle);
	g_pFramework->Get2DRoot()->AddChild(m_pTextStatus);
	g_pFramework->Get2DRoot()->AddChild(m_pTextMsg);

	CalcLayout();
	
	CreateIndicator();

	return true;
}

CGameCharaBase * TestAutomationInterface::getPlayer(void)
{
	if( g_pCharaManager == nullptr)
		return nullptr;

	if( BattleSlotContext::i() == nullptr)
		return nullptr;

	return g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
}

bool TestAutomationInterface::OnSize()
{
	CalcLayout();

	return true;
}

void TestAutomationInterface::CalcLayout(void)
{
	INT32 cy = g_pFramework->GetViewer()->GetViewHeight();

	int fontSz = 18;

	m_pTextTitle->setPos(5, cy - (fontSz * 5));
	m_pTextStatus->setPos(8, cy - (fontSz * 4));
	m_pTextMsg->setPos(8, cy - (fontSz * 3));
}

i3UIControl * TestAutomationInterface::FindCtrl( const wchar_t * pszCtrlPath)
{
	i3UIManager	*	pUIManager = g_pFramework->getUIManager();

	i3::string path;

	i3::utf16_to_mb( pszCtrlPath, path);

	// Path로 Control을 찾는다.
	return pUIManager->TA_FindControlByPath(path.c_str());
}

wstring TestAutomationInterface::GetAppName(void)
{
	return L"PointBlank";
}

HWND TestAutomationInterface::GetHWND(void)
{
	return g_pViewer->GetWindowHandle();
}

bool TestAutomationInterface::IsReady(void)
{
	i3Framework::STATE state = g_pFramework->GetState();

	if (state < i3Framework::STATE_SCENE)
		return false;

	i3Stage * pStage = g_pFramework->GetCurrentStage();

	if (pStage == nullptr)
		return false;


	if ( pStage->GetName()[0] == 0)
			return false;

	return true;
}

static const wchar_t *	getUIPhaseString(UIPHASE phase)
{
	switch (phase)
	{
		case UIPHASE_START:				return L"Start";
		case UIPHASE_SERVER_CHANNEL_LIST:	return L"Server Select";
		case UIPHASE_CREATE_CHARA:			return L"Chara Create";
		case UIPHASE_LOBBY:					return L"Lobby";
		case UIPHASE_CLANLOBBY:				return L"Clan Lobby";
		case UIPHASE_READY_ROOM:			return L"Ready Room";
	}

	return L"(Unknown Screen)";
}

static const wchar_t *	getUISubPhaseString(UISUBPHASE phase)
{
	switch (phase)
	{
	case USP_NONE:	return L"None";
	case USP_SHOP:	return L"Shop";
	case USP_INVENTORY: return L"Inventory";
	case USP_GACHA: return L"Gacha";
	case USP_GIFTSHOP: return L"GiftShop";
	case USP_PROFILE: return L"Profile";
	case USP_TEMP: return L"Temp";
	}

	return L"(Unknown Sub-Screen)";
}

void TestAutomationInterface::UpdateStatus(void)
{
	wstring str;

	if (g_pFramework->IsBattleStage() == false)
	{
		UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

		str = L"Screen-";

		if (pUIFrame != nullptr)
		{
			UIPHASE phase = pUIFrame->GetCurrentPhaseType();
			UISUBPHASE eSubPhase = pUIFrame->GetCurrentSubPhaseType();

			const wchar_t * pszPhaseStr = getUIPhaseString(phase);
			const wchar_t * pszSubStr = getUISubPhaseString(eSubPhase);

			str += pszPhaseStr;
			str += L" (";
			str += pszSubStr;
			str += L")";
			}
		else
		{
			str += L"Start";
		}
	}
	else
	{
		CGameCharaBase * pPlayer = getPlayer();

		if (pPlayer != nullptr)
		{
			//MATRIX * pTrans = pPlayer->GetMatrix();
			VEC3D * pPos = pPlayer->GetPos();
			
			CGameCharaBoneContext * pBoneCtx = pPlayer->getBoneContext();
			float rho = pBoneCtx->getRhoValue();
			float theta = pBoneCtx->getTheta();

			wchar_t conv[256];

			swprintf_s(conv, _countof(conv), L"Player pos( %.2f, %.2f, %.2f) ", pPos->x, pPos->y, pPos->z);
			str = conv;

			swprintf_s(conv, _countof(conv), L"lookat( %.2f, %.2f, 0) ", theta, rho);
			str += conv;
		}
		else
		{
			str = L"No Player";
		}
	}

	if (m_txtStatus != str)
	{
		m_txtStatus = str;
		m_pTextStatus->SetText(str.c_str());

		ShowText(TM_TXTDISPLAY);
	}
}

void TestAutomationInterface::UpdateText( const wchar_t * pszTitle, const wchar_t * pszMsg)
{
	if((m_pTextTitle != nullptr) && (pszTitle != nullptr))
		m_pTextTitle->SetText( pszTitle);

	if((m_pTextMsg != nullptr) && (pszMsg != nullptr))
	{
		m_pTextMsg->SetText( pszMsg);
	}

	ShowText(TM_TXTDISPLAY);
}

void TestAutomationInterface::ShowText(float duration)
{
	m_tmTextDisplay = duration;

	m_pTextTitle->SetEnable(true);
	m_pTextStatus->SetEnable(true);
	m_pTextMsg->SetEnable(true);
}

void TestAutomationInterface::HideText(void)
{
	m_tmTextDisplay = 0.0f;

	m_pTextTitle->SetEnable(false);
	m_pTextStatus->SetEnable(false);
	m_pTextMsg->SetEnable(false);
}

void TestAutomationInterface::_GetCtrlLocation(i3UIControl * pCtrl, RECT * pRect)
{
	pCtrl->CalcAccumulatedPos();

	VEC3D * pPos = pCtrl->getAccumulatedPos();

	pRect->left = (int)pPos->x;
	pRect->top = (int)pPos->y;
	pRect->right = pRect->left + (INT32)pCtrl->getWidth();
	pRect->bottom = pRect->top + (INT32)pCtrl->getHeight();
}

bool TestAutomationInterface::GetCtrlRect( const wchar_t * pszCtrlPath, RECT * pRect)
{
	i3UIControl * pCtrl = FindCtrl( pszCtrlPath);

	if( pCtrl == nullptr)
		return false;

	_GetCtrlLocation(pCtrl, pRect);

	return true;
}

bool TestAutomationInterface::FindCtrlText_GetRect(const wchar_t * pszCtrlPath, const wchar_t * pszText, bool must_be_same, RECT * pRect)
{
	i3UIControl * pCtrl = nullptr;
	i3UIControl * pFind = nullptr;

	if (pszCtrlPath != nullptr)
	{
		pCtrl = FindCtrl(pszCtrlPath);
	}

	// 우선 Control을 찾아야지...
	if (pCtrl != nullptr)
	{
		// pCtrl의 Child Control들 중에, pszText를 가지고 있는 Control을 찾는다.
		pFind = pCtrl->TA_ScanText( pszText, must_be_same, true);
	}
	else
	{
		// 특정 Control이 지정되지 않았다면, 전역적으로 검색.
		i3UIManager * pManager = g_pFramework->getUIManager();

		pFind = pManager->TA_ScanText( pszText, must_be_same);
	}

	if (pFind == nullptr)
		return false;

	// pFind가 입력 불가능한 Control일 수 있으니...
	pFind = FindClickable(pFind);

	if (pFind == nullptr)
		return false;

	_GetCtrlLocation(pFind, pRect);

	return true;
}

bool TestAutomationInterface::IsCtrlReady( const wchar_t * pszCtrlPath)
{
	i3UIControl * pCtrl = FindCtrl( pszCtrlPath);

	if( pCtrl == nullptr)
	{
		return false;
	}

	//return pCtrl->isActivated() && (pCtrl->isState(I3UI_STATE_HIDED) == false);
	return pCtrl->isActivated() && pCtrl->isVisible();
}

bool TestAutomationInterface::GetCtrlText( const wchar_t * pszCtrlPath, wstring & str)
{
	i3UIControl * pCtrl = FindCtrl( pszCtrlPath);

	if( pCtrl == nullptr)
		return false;

	if( pCtrl->kind_of( i3UICaptionControl::static_meta()))
	{
		i3UICaptionControl * pCaption = (i3UICaptionControl *) pCtrl;

		if( pCaption->getText() != nullptr)
		{
			str = pCaption->getText();
		}
		else
		{
			str = L"";
		}

		return true;
	}

	return false;
}

void TestAutomationInterface::OnTest( NodeBase * pNode)
{
	wstring str;

	pNode->getInfoString( str);

	UpdateText( nullptr, str.c_str());
}

TESTRESULT	TestAutomationInterface::OnExit(void)
{
	GameEventSender::i()->SetEvent(EVENT_GAME_END);
	g_pFramework->GotoNextStage("Exit");

	return TESTRESULT_PASS;
}

bool TestAutomationInterface::ToggleCapture(void)
{
	if (m_pCapture->isCapturing())
		m_pCapture->End();
	else
		m_pCapture->Start();

	return true;
}

static int _GetDepth( i3GameNode * pNode)
{
	INT32 depth = 0;

	while( pNode != nullptr)
	{
#if defined( _DEBUG)
		i3::class_meta * pMeta = pNode->meta();

		I3TRACE( L"Depth(%d) : %s(%s)\n", depth, pMeta->class_name().c_str(), pNode->GetName());
#endif

		depth++;
		pNode = pNode->getParent();
	}

	return depth - 2;
}

i3UIControl * TestAutomationInterface::PopupPickedCtrls( std::vector<i3UIControl *> & list)
{
	HMENU hMenu = ::CreatePopupMenu();
	wstring str;
	i3UIControl * pPicked = nullptr;

	for( size_t i = 0; i < list.size(); i++)
	{
		i3UIControl * pCtrl = list[i];

		{
			int depth = _GetDepth( pCtrl);

			str = L"";

			for( int k = 0; k < depth; k++)
			{
				str += L"+---";
			}
		}

		if( pCtrl->GetName() != nullptr)
		{
			string aname( pCtrl->GetName());
			wstring wname( aname.begin(), aname.end());

			str += wname;
		}
		else
		{
			str += L"(Nonamed)";
		}

		i3::class_meta * pMeta = pCtrl->meta();
		string ameta( pMeta->class_name().c_str());
		wstring wmeta( ameta.begin(), ameta.end());

		str += L"    <";
		str += wmeta;
		str += L"> \t\"";

		if( pCtrl->kind_of( i3UICaptionControl::static_meta()))
		{
			i3UICaptionControl * pCaption = (i3UICaptionControl *) pCtrl;
			i3UIText * pUIText = pCaption->getUIText();

			if( pUIText != nullptr)
			{
				//str += pUIText->getRefinedText().c_str();
				str += pUIText->getText();
			}
		}

		str += L"\"    ";

		if (pCtrl->isActivated())
			str += L"Act  ";
		else
			str += L"Deact  ";

		if (pCtrl->isState(I3UI_STATE_HIDED))
			str += L"Hide  ";
		else
			str += L"Vis  ";

		::AppendMenuW( hMenu, MF_STRING, i + 1, str.c_str());
	}

	{
		POINT pt;

		::GetCursorPos( &pt);

		UINT id = ::TrackPopupMenuEx( hMenu, TPM_RETURNCMD, pt.x, pt.y, g_pViewer->GetWindowHandle(), nullptr);

		if( id != 0)
		{
			// 선택 했다.
			pPicked = list[ id - 1];
		}
	}

	::DestroyMenu( hMenu);

	return pPicked;
}

void TestAutomationInterface::PickCtrl(void)
{
	i3UIManager				*	pUIManager = g_pFramework->getUIManager();
	i3InputMouse * pMouse = g_pFramework->getMouse();
	//UIMainFrame * pMainFrm = g_pFramework->GetUIMainframe();
	vector<i3UIControl *>	list;

	wstring text;

	if (pUIManager != nullptr)
	{
		POINT pt;

		pt.x = (INT32) pMouse->GetX();
		pt.y = (INT32) pMouse->GetY();

		I3TRACE( L"Picked : %d, %d\n", pt.x, pt.y);

		bool rv = pUIManager->TA_GetPickedCtrl( pt, list);

		if( rv == false)
			return;

		i3UIControl * pCtrl = PopupPickedCtrls( list);

		if( pCtrl != nullptr)
		{
			char path[512];

			path[0] = 0;

			pCtrl->TA_GetCtrlPath( path, _countof(path));

			string a_path( path);
			wstring w_path( a_path.begin(), a_path.end());

			text = L"Picked:";
			text += w_path;

			if( pCtrl->kind_of( i3UICaptionControl::static_meta()))
			{
				i3UICaptionControl * pTextCtrl = (i3UICaptionControl *) pCtrl;

				if( pTextCtrl->getText() != nullptr)
				{
					text += L"\nText:";
					text += pTextCtrl->getText();
				}
			}
		}
	}

	if( text.size() > 0)
	{
		I3TRACE( "--%s\n", text.c_str());
		UpdateText( nullptr, text.c_str());
	}

	// Clipboard로 복사
	{
		HANDLE hMem;
		UINT32 Size;

		// Node들을 JSON 형식으로 저장한다.
		{
			Size = (text.length() + 1) * sizeof(wchar_t);

			hMem = GlobalAlloc(GMEM_DDESHARE, Size);

			wchar_t * pBuf = (wchar_t *)GlobalLock(hMem);

			memset(pBuf, 0, Size);

			wcscpy_s( pBuf, text.length() + 1, text.c_str());

			GlobalUnlock(hMem);
		}

		if (OpenClipboard(nullptr) == false)
		{
			GlobalFree(hMem);
			return;
		}

		//EmptyClipboard();

		SetClipboardData( CF_UNICODETEXT, hMem);

		CloseClipboard();
	}
}

void TestAutomationInterface::OnTick( float tm)
{
	i3InputKeyboard * pKey = g_pViewer->GetInputDeviceManager()->GetKeyboard();
	i3InputMouse * pMouse = g_pViewer->GetInputDeviceManager()->GetMouse();

	if (pKey->GetPressState(I3I_KEY_LCTRL) && pKey->GetPressState(I3I_KEY_LSHIFT))
	{
		ShowText(TM_TXTDISPLAY);

		if (pKey->GetStrokeState(I3I_KEY_SLASH))
		{
			ToggleCapture();
		}

		if (pKey->GetStrokeState(I3I_KEY_COMMA))
		{
			// Crash 발생 시, 데이터 유실 등의 QA 확인을 위해
			// 강제로 크래쉬 시키도록 합니다.
			// 삭제하지 말아주세요.
			int * p = nullptr;

			*p = 0;
		}

		if( pMouse->GetStrokeButtonState() & I3I_MOUSE_MBUTTON)
		{
			// Capture로 간주하지 않고, Ctrl 정보를 얻어내기 위한 동작으로 인식
			PickCtrl();
			return;			// Capture 하지 않도록...
		}

#if defined( _DEBUG)
		if (pKey->GetStrokeState(I3I_KEY_K))
		{
			wstring t = L"BombA";
			OnMoveTo(0.0f, CaseRun::LOC_OBJECT, t);
		}

		if (pKey->GetStrokeState(I3I_KEY_M))
		{
			wstring t = L"BombA";
			OnLookAt(0.0f, CaseRun::DIR_OBJECT, t);
		}
#endif
	}

	/*
#if defined( _DEBUG)
	if (pKey->GetStrokeState(I3I_KEY_B))
	{
		BuyGoods(L"XM2010");
	}
#endif
	*/

	UpdateStatus();

	if(m_tmTextDisplay > 0.0f)
	{
		m_tmTextDisplay -= tm;
		if (m_tmTextDisplay < 0.0f)
		{
			m_tmTextDisplay = 0.0f;
		}

		// Text Transparency 조정
		UINT8 a = (UINT8)(i3Math::Min(1.0f, m_tmTextDisplay / 1.0f) * 255.0f);

		m_pTextTitle->SetAlpha(a);
		m_pTextStatus->SetAlpha(a);
		m_pTextMsg->SetAlpha(a);

		if (m_tmTextDisplay == 0.0f)
		{
			m_pTextTitle->SetEnable(false);
			m_pTextStatus->SetEnable(false);
			m_pTextMsg->SetEnable(false);
		}
	}

	m_pCapture->OnInput( tm);
}

void TestAutomationInterface::OnKeyPress(float tmLocal, const wchar_t * pszKeyName, bool bPress)
{
	i3Viewer * pViewer = g_pFramework->GetViewer();
	i3InputKeyboard * pKeyboard = pViewer->GetInputDeviceManager()->GetKeyboard();

	I3I_KEY key = GetKey( pszKeyName);

	pKeyboard->SetPressState( key, bPress);

	if( tmLocal == 0.0f)
		pKeyboard->SetStrokeState( key, bPress);
	
}

void TestAutomationInterface::OnMouseAction(float tmLocal, MOUSE_ACTION action, POINT pt, BUTTON btn)
{
	i3Viewer * pViewer = g_pFramework->GetViewer();
	i3InputMouse * pMouse = pViewer->GetInputDeviceManager()->GetMouse();
	UINT32 btnMask = 0;

	switch (action)
	{
	case Interface::MOUSE_MOVE:
		break;

	case Interface::MOUSE_BUTTON_DOWN:
		switch (btn)
		{
		case BTN_L:	m_MouseBtnPressed[0] = true;	btnMask = I3I_MOUSE_LBUTTON;	break;
		case BTN_M:	m_MouseBtnPressed[1] = true;	btnMask = I3I_MOUSE_MBUTTON;	break;
		case BTN_R:	m_MouseBtnPressed[2] = true;	btnMask = I3I_MOUSE_RBUTTON; break;
		}
		break;

	case Interface::MOUSE_BUTTON_UP:
		switch (btn)
		{
		case BTN_L:	m_MouseBtnPressed[0] = false;	btnMask = I3I_MOUSE_LBUTTON;	break;
		case BTN_M:	m_MouseBtnPressed[1] = false;	btnMask = I3I_MOUSE_MBUTTON;	break;
		case BTN_R:	m_MouseBtnPressed[2] = false;	btnMask = I3I_MOUSE_RBUTTON;	break;
		}
		break;
	}

	pMouse->SetButtonState(btnMask, m_MouseBtnPressed[0]);
	__super::OnMouseAction(tmLocal, action, pt, btn);
}

TESTRESULT	TestAutomationInterface::OnUISetText(float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText)
{
	i3UIControl * pCtrl = nullptr;

	if (pszCtrlPath != nullptr)
	{
		pCtrl = FindCtrl(pszCtrlPath);
	}

	// 우선 Control을 찾아야지...
	if (pCtrl != nullptr)
	{
		i3UICaptionControl * pControl = (i3UICaptionControl *)pCtrl;

		pControl->SetText(pszText);

		return TESTRESULT_PASS;
	}

	return TESTRESULT_FAIL;
}

i3UIControl *	TestAutomationInterface::FindClickable(i3UIControl * pCtrl)
{
	while (pCtrl != nullptr)
	{
		//if( i3::kind_of<i3UIStaticText*>( pCtrl))
		if (pCtrl->isIgnoreInput() == false)
			break;

		i3GameNode * pParent = pCtrl->getParent();

		if (i3::kind_of<i3UIControl *>(pParent) == false)
			break;

		pCtrl = (i3UIControl *)pParent;
	}

	return pCtrl;
}

TESTRESULT	TestAutomationInterface::OnUISelectItem( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText, bool bExact, bool dblClick)
{
	i3UIControl * pCtrl = nullptr;
	
	if( (pszCtrlPath != nullptr) && (pszCtrlPath[0] != 0))
		pCtrl = FindCtrl( pszCtrlPath);

		// 처음 시작하는 경우. TA_SelectItem을 이용해, 선택을 시도한다.
		if((pCtrl != nullptr) && ( pCtrl->TA_Select( pszText, bExact) == true))
		{
			return TESTRESULT_PASS;			// 정상적으로 선택되었다.
		}

				return TESTRESULT_FAIL;
}

TESTRESULT	TestAutomationInterface::OnUIScanText( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText, bool bExact, bool bSubCtrl)
{
	i3UIControl * pCtrl = FindCtrl( pszCtrlPath);
	if( pCtrl == nullptr)
		return TESTRESULT_ERROR;

	if (pCtrl->TA_ScanText( pszText, bExact, bSubCtrl))
		return TESTRESULT_PASS;

	return TESTRESULT_FAIL;
}

TESTRESULT	TestAutomationInterface::OnUICheckState( float tmLocal, const wchar_t * pszCtrlPath, CTRL_STATE state)
{
	i3UIControl * pCtrl = FindCtrl( pszCtrlPath);
	if( pCtrl == nullptr)
		return TESTRESULT_ERROR;

	I3UI_STATE stateMask = 0;

	switch ( state)
	{
		case CTRL_STATE_DISABLED:	stateMask = I3UI_STATE_DEACTIVATED; break;
		case CTRL_STATE_SELECTED:	stateMask = I3UI_STATE_SELECTED;	break;
		case CTRL_STATE_FOCUSED:	stateMask = I3UI_STATE_FOCUSED;		break;
	}

	if (pCtrl->isState(stateMask))
		return TESTRESULT_PASS;

	return TESTRESULT_FAIL;
}

TESTRESULT	TestAutomationInterface::OnUICheckTopWindow( float tmLocal, const wchar_t * pszCtrlPath)
{
	i3UIControl * pCtrl = FindCtrl( pszCtrlPath);
	if( pCtrl == nullptr)
		return TESTRESULT_ERROR;

	return TESTRESULT_PASS;
}


#endif
