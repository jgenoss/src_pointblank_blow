
#include "pch.h"

#include "PBConsoleCommon.h"
#include "PBConsoleClientCtx.h"
#include "PBConsoleLog.h"


namespace GameUI
{
	extern i3::string pre_macro_date_to_string();
	extern i3::string pre_macro_time_to_string();
}


//
// PBConsoleExecutor
//

class PBConsoleExecutor : public i3UIConsoleExecutor
{
	I3_CLASS_DEFINE(PBConsoleExecutor, i3UIConsoleExecutor);

public:
	PBConsoleExecutor() {}
	virtual ~PBConsoleExecutor();

	void AttachListener(i3UIConsoleExecutorListener* listener);
	void DetachListener(i3UIConsoleExecutorListener* listener);

private:
	i3UIConsoleExecutorListener * m_listener = nullptr;

};

I3_CLASS_INSTANCE(PBConsoleExecutor)

PBConsoleExecutor::~PBConsoleExecutor()
{
	I3_SAFE_RELEASE(m_listener);
}

void PBConsoleExecutor::AttachListener(i3UIConsoleExecutorListener* listener)
{
	I3_REF_CHANGE(m_listener, listener);
}

void PBConsoleExecutor::DetachListener(i3UIConsoleExecutorListener* listener)
{
	if(m_listener == listener)
	{
		I3_SAFE_RELEASE(m_listener);		
	}
}


//
// PBConsoleClientCtx
//

I3_CLASS_INSTANCE(PBConsoleClientCtx)

i3ClassMeta* GetPBConsoleClientCtxMeta() 
{
#ifdef __USE_PBCONSOLE__
	return PBConsoleClientCtx::static_meta(); 
#else
	return nullptr;
#endif
}

enum EConsoleUI
{
	CONSOLE_SIZ_W = 1000,
	CONSOLE_SIZ_H = 600,
};


PBConsoleClientCtx::PBConsoleClientCtx()
{	
#ifdef __USE_PBCONSOLE__
	// 위에함수를 호출하지 않으면 Static-Library 빌드중엔
	// PBConsoleCmdGlue.cpp 의 Glue함수코드가 실행파일빌드후에 사라진다.
	extern void PBConsoleCmdGlue_Init();	
	PBConsoleCmdGlue_Init();	
#endif

	m_logger = new PBConsoleLog();
	m_loggerTextRev = 0;

	m_consoleUIMain = nullptr;
	m_consoleUIFrame = nullptr;
	m_consoleUILog = nullptr;
	m_consoleUIInput = nullptr;

	m_attached = false;
}

PBConsoleClientCtx::~PBConsoleClientCtx()
{
#ifdef __USE_PBCONSOLE__
	extern void PBConsoleCmdGlue_Destroy();
	PBConsoleCmdGlue_Destroy();
#endif


	if(m_logger)
	{
		delete m_logger;
		m_logger = nullptr;
	}

	// OnDetach가 호출되었는가
	I3ASSERT(!m_attached);
	I3ASSERT(!m_consoleUIMain);
}

bool PBConsoleClientCtx::OnAttach()
{
	if(m_attached)
		return false;

	bool rst = false;
	rst = CreateUIScene();

	if(rst)
		m_attached = true;

	return rst;
}

void PBConsoleClientCtx::OnDetach()
{
	if(!m_attached)
		return;

	DestroyUIScene();

	m_attached = false;
}

void PBConsoleClientCtx::OnShow(bool isShow)
{
	i3UIManager* uiMan = GetUIManager();

	uiMan->resetOnMouseTarget();

	if(isShow)
	{
		m_logger->SetDirty(PBConsoleLog::EDF_DIRTY_LOGTEXT);
		m_logger->ClearDirty();
		uiMan->SetFocusControl(m_consoleUIInput);

		ReDrawConsoleLog();
	}
	else
	{
		uiMan->SetFocusControl(nullptr);
	}
}

void PBConsoleClientCtx::OnEnableConsoleScenes(bool enable)
{
	if(m_consoleUIMain)
	{
		m_consoleUIMain->EnableScene(enable);
		m_consoleUIMain->SetVisible(enable);
	}
}

bool PBConsoleClientCtx::OnKeyInput(i3InputDeviceManager* inputDevMan)
{
	if(!inputDevMan)
		return false;

	i3InputKeyboard* pKeyboard = inputDevMan->GetKeyboard();

	if(pKeyboard->GetStrokeState(I3I_KEY_ESC))
	{
		i3UIManager::ShowConsole(false);
		return true;
	}

	if(pKeyboard->GetStrokeState(I3I_KEY_ENTER))
	{
		SubmitConsoleInput();
		return true;
	}

	return false;
}

bool PBConsoleClientCtx::CreateUIScene()
{
	i3UIManager* uiMan = GetUIManager();

	if(!uiMan)
		return false;

	// UI템플릿 라이브러리가 처음 사용되는시점에서 생성되기에 미래 생성해둡니다.
	uiMan->LoadTemplateLibrary("UIRe\\PointBlankRe.i3Uil");
	i3UILibrary* pLib = i3UI::getUILibrary();

	if(!pLib)
		return false;
	
	i3UIScene* consoleScene = i3UIScene::new_object();	
	
	i3UIFrameWnd* consoleFrame = i3UIFrameWnd::new_object_ref(); // consoleScene에 child로 붙을떄 ref+1

	//i3UICaptionControl* uiCaption = i3UICaptionControl::new_object();

	consoleScene->SetConsoleScene(true);
	consoleScene->SetName("ConsoleScene");
	consoleScene->setWidth(i3UIConsoleClientContext::EUIRES_WIDTH);
	consoleScene->setHeight(i3UIConsoleClientContext::EUIRES_HEIGHT);

	// console scene mainframe
	consoleFrame->SetTemplateByName("Frame_01");
	consoleFrame->SetName("ConsoleFrame");
	consoleFrame->setSize(CONSOLE_SIZ_W, CONSOLE_SIZ_H);
	consoleScene->AddChild(consoleFrame);
	
	i3UIEditBox* ebConsoleLog = i3UIEditBox::new_object_ref();
	ebConsoleLog->SetName("ebConsoleLog");
	ebConsoleLog->SetTemplateByName("EditBox_01");
	ebConsoleLog->setPos(8,10,0);
	ebConsoleLog->setSize(CONSOLE_SIZ_W-20, CONSOLE_SIZ_H-55);

	ebConsoleLog->SetReadOnly(true);
	ebConsoleLog->addEditBoxStyle(I3UI_EDITBOX_STYLE_CHAT);
	ebConsoleLog->setBoxProperty(i3UIEditBox::UIBOX_CHATVIEW);
	ebConsoleLog->SetMaxTextCount(100);
	ebConsoleLog->setScrollState((i3UIEditBox::UI_BOX_SCROLL_STATE)3);
	ebConsoleLog->setScrollMoveSpeed(3);

//		consoleFrame->AddControl(imgConsoleLog);
	consoleFrame->AddControl(ebConsoleLog);

	// console input editbox
	i3UIImageBox* imgConsoleInput = i3UIImageBox::new_object_ref();
	imgConsoleInput->SetTemplateByName("Img_chattingbox01");
	imgConsoleInput->setPos(5, CONSOLE_SIZ_H-35);
	imgConsoleInput->setSize(CONSOLE_SIZ_W-10, 30);
	
	i3UIEditBox* ebConsoleInput = i3UIEditBox::new_object_ref();
	ebConsoleInput->SetName("ebConsoleInput");
	ebConsoleInput->SetTemplateByName("EditBox_Null");
	ebConsoleInput->setPos(10, CONSOLE_SIZ_H-25);
	ebConsoleInput->setSize(CONSOLE_SIZ_W-15, 25);
	ebConsoleInput->SetLetterLimit(1024);
	ebConsoleInput->setUseCopyNPaste(true);

	ebConsoleInput->addEditBoxStyle(I3UI_EDITBOX_STYLE_CHAT);
	ebConsoleInput->setBoxProperty(i3UIEditBox::UIBOX_CHATINPUT);
	ebConsoleInput->enableScrollBar(false);

	m_consoleUIInput = ebConsoleInput;

	consoleFrame->AddControl(imgConsoleInput);
	consoleFrame->AddControl(ebConsoleInput);

	// attach
	consoleFrame->setTopFrameWndState(true); // 이게 안되어있으면 OnEnterMouse에서 최상위 탑프레임윈도우를 못찾음
	consoleFrame->addStyle(I3UI_STYLE_TOP_DRAW);
	consoleFrame->OnCompletedLoading();
	consoleFrame->setPos(0,50,0);//	uiFrame->CalcAccumulatedPos();

	consoleScene->Attach(false); // ref+1, i3UIManager
	consoleScene->EnableScene(false);
	consoleScene->SetVisible(false);
	
	m_consoleUILog = ebConsoleLog;
	m_consoleUIFrame = consoleFrame;
	m_consoleUIMain = consoleScene;

	ShowPrologueLog();
	ReDrawConsoleLog();

	return true;
}

void PBConsoleClientCtx::DestroyUIScene()
{
	I3_SAFE_RELEASE(m_consoleUIMain);
}

bool PBConsoleClientCtx::OnExecBefore(const wchar_t* execTxt, const wchar_t** rstReason)
{
	if(execTxt)
	{
		m_logger->AddLineFeed();
		m_logger->AddLog(L">> %s", execTxt);		
	}

	return true;
}

void PBConsoleClientCtx::OnExecAfter(const i3UICSCmd* cmd, EI3UIConsoleRst rst,
	i3UICStrAry* rstStr)
{
	m_logger->SaveAttrib();

	i3::wstring str = cmd->GetCmdText();

	if(GetExecutor()->GetFlag() & i3UIConsoleExecutor::EFL_SHOW_PARSE_TOKEN)
	{
		INT32 numToken = cmd->GetNumToken();

		if(!numToken)
			m_logger->AddLog(255, 255, 0, L"!! No token parsed..");
		else
		{
			for(INT32 i=0; i<numToken; i++)
				m_logger->AddLog(255, 255, 0, L"[token desc %d] %s", i, cmd->GetToken(i));
		}

		m_logger->AddLog(255, 255, 0, L"Type 'show token 0' to hide this description..");
	}

	if(rst == EI3UICSRST_OK)
	{
		m_logger->AddLog(0, 255, 0, L"successfully executed [%s]", str.c_str());
	}
	else
	{
		switch(rst)
		{
		case EI3UICSRST_FAIL_EXEC_PARSE_ERR:
			m_logger->AddLog(255, 0, 0, L"failed to parse command [%s]", str.c_str());
			break;

		default:
			m_logger->AddLog(255, 0, 0, L"error in execution [%s]", str.c_str());
			break;
		}
	}

	m_logger->AddLineFeed();
	m_logger->RestoreAttrib();

	ReDrawConsoleLog();
}

i3UIConsoleExecutor* PBConsoleClientCtx::OnCreateExecutor()
{	
	PBConsoleExecutor* executor = PBConsoleExecutor::new_object();
	return executor;
}

EI3UIConsoleRst PBConsoleClientCtx::SubmitConsoleInput()
{
	if(!m_consoleUIInput->isFocused())
	{
		if(GetUIManager()->GetFocusFrameWnd() != m_consoleUIFrame)
			GetUIManager()->SetFocusSceneFrameWnd(m_consoleUIMain);

		GetUIManager()->SetFocusControl(m_consoleUIInput);
		return EI3UICSRST_OK;
	}

	// editbox에 포커스없을시 오류
	const wchar_t* inputText = m_consoleUIInput->getIMETextBuf();

	if(!inputText || !(*inputText))
		return EI3UICSRST_FAIL_EXEC_INVALID_CALL;

	std::wstring execText = inputText;
	m_consoleUIInput->SetIMEText(L""); // nullptr쓰면 오류남

	EI3UIConsoleRst execRst = Exec(execText.c_str());

	return execRst;
}

void PBConsoleClientCtx::ReDrawConsoleLog()
{
	DWORD textRev = m_logger->GetFullLogTextRev();

	if(m_loggerTextRev != textRev)
	{
		m_consoleUILog->SetText(m_logger->GetFullLogText());
		m_consoleUILog->ScrollToEnd();

		m_loggerTextRev = textRev;
	}
}

void PBConsoleClientCtx::AddLog(UINT32 flag, const wchar_t* fmt, ...)
{
	wchar_t buf[1024] = { 0 };
	
	va_list arg; va_start(arg, fmt);

	vswprintf_s(buf, 1024, fmt, arg); va_end(arg);

	m_logger->SaveAttrib();
	m_logger->AddLog(buf);
	m_logger->RestoreAttrib();
}


void PBConsoleClientCtx::AddLogStr(UINT32 flag, const wchar_t* str)
{
	m_logger->SaveAttrib();
	m_logger->AddLog(str);
	m_logger->RestoreAttrib();
}

bool PBConsoleClientCtx::SaveLogToFile(const wchar_t* path)
{
	wchar_t consoleSavePath[512] = {0};

	if(!path)
	{
		wchar_t modulePath[MAX_PATH] = {0};		

		::GetModuleFileNameW(NULL, modulePath, MAX_PATH);
		wchar_t* p = wcsrchr(modulePath, L'\\');
		if(*p) *p = 0;

		SYSTEMTIME st;
		GetLocalTime(&st);

		wchar_t userID[MAX_PATH]={ L"Unknown" };
		wchar_t* additional = nullptr;

		extern const std::string& __get_user_id();
		std::string uid = __get_user_id();

		if(uid.c_str())
		{
			i3String::MultiByteToWideChar(CP_ACP, NULL, uid.c_str(), -1, userID, MAX_PATH);
			additional = userID;
		}

		swprintf_s(consoleSavePath, 512, L"%s\\PBCSLOG_%s_%02d%02d-%02d%02d%02d.log",
			modulePath, additional ? additional : L"none", 
			st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		path = consoleSavePath;
	}

	m_logger->AddLog(L"Saving console log to file %s  ...", path);
	bool rst = m_logger->SaveLogToFile(path);
	
	if(rst)
	{
		m_logger->SetLogColorAttr(255,255,0);
		m_logger->AddLog(L"Successfully saved console log to file %s", path);
	}
	else
	{
		m_logger->SetLogColorAttr(255,0,0);
		m_logger->AddLog(L"Failed to save console log to file %s", path);
	}

	m_logger->ResetLogAttrib();
	ReDrawConsoleLog();

	return rst;
}

void PBConsoleClientCtx::ShowPrologueLog()
{
	m_logger->SetLogColorAttr(rand() % 255,rand() % 255, rand() % 255);
	m_logger->AddLog(L"GameVer(Month,Day)%d,%d - (VER,BUILD,SC,SS) %d.%d.%d.%d", GAME_VER_MONTH, GAME_VER_DAY, VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL);
	m_logger->SetLogColorAttr(rand() % 255,rand() % 255, rand() % 255);

	{		
		wchar_t dateStr[MAX_PATH], timeStr[MAX_PATH];

		i3String::MultiByteToWideChar(CP_ACP, NULL, GameUI::pre_macro_date_to_string().c_str(), -1, dateStr, MAX_PATH);
		i3String::MultiByteToWideChar(CP_ACP, NULL, GameUI::pre_macro_time_to_string().c_str(), -1, timeStr, MAX_PATH);

		m_logger->AddLog(L"BuildLog(%s,%s)", dateStr, timeStr);
	}

	m_logger->AddLog(L"/SAVELOG : 콘솔로그 파일 저장.");
	m_logger->SetLogColorAttr(rand() % 255,rand() % 255, rand() % 255);
	m_logger->AddLog(L"F2 : 마지막명령 재생");
	
	m_logger->ResetLogAttrib();
}

void PBConsoleClientCtx::ClearLog()
{
	m_logger->Reset();
	ReDrawConsoleLog();
}

