#include "pch.h"

#if JuuL_0

#include "StageGUI.h"
#include "GlobalVariables.h"
#if defined (LOCALE_CHINA)	
#include "../SDOALink/SDOALink.h"
#endif

//I3_ABSTRACT_CLASS_INSTANCE( CStageGUI, i3Stage);
I3_CLASS_INSTANCE( CStageGUI);

CStageGUI::CStageGUI(void)
{
	m_pGUIRoot			= NULL;
	m_szUIFileName[0]	= 0;
	
	m_pCautionMessageBox	= NULL;
	
	m_rAwayInputTime = 0.0f;
	m_rCautionTime = 0.0f;
	m_bOffAwayInput = FALSE;
	m_bOffCaution = FALSE;
}

CStageGUI::~CStageGUI(void)
{
	if( m_pCautionMessageBox )
	{
		g_pFramework->CloseMessageBox(m_pCautionMessageBox);
		m_pCautionMessageBox = NULL;
	}

	Destroy();
}

void	CStageGUI::OnCreate()
{
	// GUI에 들어왔을경우에는 무조건 게임키모드는 Menu입니다.
	g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);

	i3Stage::OnCreate();
}
 
void	CStageGUI::Create()
{
	if( m_szUIFileName[0] != NULL)
	{
		if( LoadGUIFile( m_szUIFileName) == FALSE)
		{
			I3FATAL( "Could not load %s", m_szUIFileName);
			return;
		}
	}
}

void	CStageGUI::Destroy()
{
	if( m_pGUIRoot != NULL)
	{
		m_pGUIRoot->getSceneGraphNode()->RemoveFromParent();

		I3_SAFE_RELEASE( m_pGUIRoot);
	}

	g_pGUIRoot = NULL;
}

void	CStageGUI::SetRoot( i3GuiRoot * pRoot)
{
	if( pRoot != NULL)
	{
		pRoot->AddRef();
	}

	Destroy();

	m_pGUIRoot = pRoot;

	g_pGUIRoot = m_pGUIRoot;

	//	메시지박스 크기 재계산 (Gui에 따라 스크린 화면 비율이 변하기 때문에 여기서 호출합니다. komet)
	g_pFramework->ResizeMessageBox();

	m_pGUIRoot->OnUpdate( __RT_0);
}

BOOL CStageGUI::LoadGUIFile( const char * szGUIFileName)
{
	i3ResourceFile File;

	if( File.Load( szGUIFileName) == STREAM_ERR)
	{
		I3FATAL( "CStageGUI::LoadGUIFile()");
		return FALSE;
	}

	i3GuiRoot	* pRoot	= NULL;
	pRoot = (i3GuiRoot*)File.FindObjectByMeta(i3GuiRoot::static_meta());
	I3ASSERT(pRoot != NULL);

	const i3TextFontSetInfo * pFontTextInfo = GetDefaultFontName();
	pRoot->setTextNodeUsage(I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX);
	pRoot->Create( g_pFramework->getGuiLayer(), pFontTextInfo->GetFontName(), pFontTextInfo->GetCharset());
	pRoot->SetViewer( g_pViewer);
	pRoot->setWidth( g_pViewer->GetViewWidth());
	pRoot->setHeight( g_pViewer->GetViewHeight());
	pRoot->BuildSceneGraph();
	pRoot->setManualNotifyDispatch( true);

	i3PersistantElement * pObj;
	
	for( INT32 i = 0; i< File.GetObjectCount() ; ++i)
	{
		pObj = File.GetObject( i);

		// GUI중 EditBox Control및 EditBox의 상속을 받는 경우 Text를 생성해야 합니다.
		if( i3::same_of<i3GuiEditBox*>(pObj) //->IsExactTypeOf( i3GuiEditBox::static_meta()) 
			|| i3::same_of<i3GuiTextBox*>(pObj)) //->IsExactTypeOf( i3GuiTextBox::static_meta()) )
		{
			((i3GuiEditBox*)pObj)->CreateTextEx( GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_SIZE);
		}
	}

	SetRoot( pRoot);

	return TRUE;
}

void CStageGUI::OnLoadEnd()
{
	Create();
}

void CStageGUI::OnUpdate( REAL32 rDeltaSeconds)
{
	if( m_pGUIRoot != NULL)
	{
		m_pGUIRoot->OnUpdate( rDeltaSeconds);
			
		_ControlNotifyPump();	// OnControlNotify()를 호출한다
	}

	OnCheckAwayInput(rDeltaSeconds);

	i3Stage::OnUpdate( rDeltaSeconds);
}

void CStageGUI::OnControlNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	// NULL
	// CStageGUI까지 전임된 Notify는 처리하지 않고 버린다.
}

INT32 CStageGUI::_ControlNotifyPump(void)
{
	if( m_pGUIRoot == NULL) return 0;

	INT32 cPumpedNotify = 0;

	OnPreControlNotify();
	
	I3GUI_CONTROL_NOTIFY notify;
	while( m_pGUIRoot->PopControlNotify(&notify) )
	{
		cPumpedNotify++;
		OnControlNotify(&notify);		
	}

	OnPostControlNotify();

	return cPumpedNotify;
}

void CStageGUI::OnPreControlNotify(void)
{
	// NULL
}

void CStageGUI::OnPostControlNotify(void)
{
	// NULL
}

void CStageGUI::DriveAway(INT32 nReason)
{
	switch(nReason)
	{
	case ACCOUNT_KICK_LOGIN:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_LOGIN_E"), EXIT_CALLBACK(CbExitNormal, true), this);
		break;
	case ACCOUNT_KICK_GM_TOOL:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_TOOL_E"), EXIT_CALLBACK(CbExitNormal,true), this);
		break;
	case ACCOUNT_KICK_OTHER:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), EXIT_CALLBACK(CbExitNormal,true), this);
		break;
	default:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), EXIT_CALLBACK(CbExitNormal,true), this);
		break;
	}

	g_pGameContext->SetEvent(EVENT_DISCONNECT);
}

void CStageGUI::CbExitNormal(void* pThis,INT32 nParam)
{
}

void CStageGUI::CbEmptyCautionBox(void* pThis,INT32 nParam)
{
	((CStageGUI*)pThis)->m_pCautionMessageBox = NULL;
}

void CStageGUI::OnExitKey(void)
{	
	_ExitKey();	
}

void CStageGUI::OnCheckAwayInput(REAL32 rDeltaSeconds)
{
#if defined	USE_AWAY_INPUT
	if( m_rAwayInputTime > 0.0f)
	{
		m_rAwayInputTime -= rDeltaSeconds;
		m_rCautionTime -= rDeltaSeconds;
	}
#endif
}

BOOL CStageGUI::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				OnExitKey();

				return TRUE;
			}
		}
	}

	return FALSE;
}

void CStageGUI::SetAwayInputTime(REAL32 rAwayInputTime,REAL32 rCautionTime)
{
	I3ASSERT(rAwayInputTime >= rCautionTime);
	
	m_rAwayInputTime = rAwayInputTime;
	m_rCautionTime = rCautionTime;
	m_bOffAwayInput = FALSE;
	m_bOffCaution = FALSE;
}

void CStageGUI::_ExitKey(void)
{
	SYSTEM_POPUP_Q(GAME_STRING("STBL_IDX_EP_GAME_EXIT_NORMAL_QE"), EXIT_CALLBACK(CbExitNormal,true), this);
}
#endif