#include "pch.h"
#include "UIPhaseNation.h"
#include "UIDefine.h"
#include "UILogin.h"
#include "UIUtil.h"
#include "../TimeUtil.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
UIPhaseNation * GetThis( void)
{
	i3Stage * pStage = g_pFramework->GetCurrentStage();
	I3ASSERT( i3::same_of<UILogin*>(pStage));

	return (UIPhaseNation*)(((UILogin*)pStage)->GetCurrentPhase());
}

extern "C" {
	int UIPhaseNation_ClickLoginBtn( lua_State* L)
	{
		GetThis()->ClickLogin();
		return 0;
	}

	int UIPhaseNation_ClickExitBtn(lua_State * L)
	{
		GetThis()->ClickExit();
		return 0;
	}

	int UIPhaseNation_ClickNation(lua_State * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->ClickBtn(idx);
		return 0;
	}
}	// extern "C"

LuaFuncReg PhaseNation_Glue[] = 
{
	{ "ClickLoginBtn",			UIPhaseNation_ClickLoginBtn },	// 비번찾기 
	{ "Glue_Exit",				UIPhaseNation_ClickExitBtn },	// ID 찾기
	{ "ClickNation",			UIPhaseNation_ClickNation },	// ID 찾기

	{nullptr, nullptr}
};



I3_CLASS_INSTANCE( UIPhaseNation);//, UIPhaseBase);

UIPhaseNation::UIPhaseNation()
{

}

UIPhaseNation::~UIPhaseNation()
{
}

/*virtual*/ void UIPhaseNation::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);
	
	bool bCount = true;
	INT32 nCount = 1;
	i3::string strBuf;
	m_pButton.clear();
	while(bCount)
	{
		i3::sprintf(strBuf, "Btn_Nation%d", nCount);
		i3UIButtonImageSet * pImg = (i3UIButtonImageSet *)pScene->FindChildByName(strBuf.c_str());
		if (pImg != nullptr)
		{
			m_pButton.push_back(pImg);
			nCount++;
		}
		else
			bCount = false;
			
	}

	InitButtonState();
	ClickBtn(g_pServerInfo->GetCurrentIPIdx());

	i3UIFrameWnd* pLoginBackGround = (i3UIFrameWnd *) pScene->FindChildByName( "i3UIFrameWnd_Main");
	I3ASSERT( pLoginBackGround != nullptr);

	//이미지 원본을 사용하여 텍스처 변경을 용이하게 처리합니다. 
	i3Texture * pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/SelectNational.i3i");
	I3ASSERT(pTexture);

	m_pBG = i3UIImageBoxEx::new_object();
	I3ASSERT( m_pBG != nullptr);
	m_pBG->AddTexture( pTexture, 1024.f, 576.f);

	I3_MUST_RELEASE(pTexture);
	m_pBG->CreateBaseSceneObjects();
	pLoginBackGround->AddChildToHead(m_pBG);
	m_pBG->SetCurrentImage(0);

	_Resize();
}

/*virtual*/ void UIPhaseNation::_ClearAtUnload( i3UIScene * pScene)
{
	UIPhaseBase::_ClearAtUnload( pScene);

}



bool UIPhaseNation::OnRevive( i3RenderContext * pCtx)
{
	if( UIPhaseBase::OnRevive( pCtx) == false)
		return false;

	_Resize();
	return true;
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

/*virtual*/ void UIPhaseNation::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);
	AddScene("Scene/Main/PointBlankRe_Login_SelectNational.i3UIs", PhaseNation_Glue);
}

/*virtual*/ void UIPhaseNation::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);
	if (m_bExit_Popup == true)
	{
		if (m_pExitMessageBox && m_pExitMessageBox->isEnable() == true)
		{
			//ExitMessageBox 를 활성화 하면서 인풋을 막았기에 여기서 자체 검사를 해야 된다.
			if (GameUI::IsKeyboardStroked(I3I_KEY_ENTER) == true)
			{
				m_pExitMessageBox->OnKey_Enter();
			}
		}
		else
		{
			m_bExit_Popup = false;
			UIBase::SetInputDisable(false);
			ClickBtn(g_pServerInfo->GetCurrentIPIdx());
		}
	}
}

/*virtual*/ bool UIPhaseNation::OnEntranceStart( void * pArg1, void * pArg2)
{
	UIBase::OnEntranceStart(pArg1, pArg2);

	return true;
}

/*virtual*/ bool UIPhaseNation::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIPhaseBase::OnKeyInput( pInputMng))
		return true;

	return false;
}

/*virtual*/ bool UIPhaseNation::OnKey_Enter( void)
{
	return true;
}

bool UIPhaseNation::OnKey_Escape()
{
	//UIPhaseBase::OnKey_Escape() //이 함수를 호출 하면 안된다. ClickExit() 함수가 호출하며 중복 호출이 된다.
	ClickExit();

	return false;
}

void UIPhaseNation::InitButtonState()
{
	for (size_t i = 0; i < m_pButton.size(); i++)
	{
		if (i3::generic_is_iequal(g_pServerInfo->GetLogInServerIPStr(i), ""))
		{
			m_pButton[i]->addState(I3UI_STATE_DEACTIVATED);
		}
		else
		{
			m_pButton[i]->removeState(I3UI_STATE_DEACTIVATED);
		}
	}
}
void UIPhaseNation::ClickBtn(INT32 btidx)
{
	for (size_t i = 0; i < m_pButton.size(); i++)
	{
		if ((INT32)i != btidx)
		{
			m_pButton[i]->SetPushFlag(false);
			m_pButton[i]->setInputDisable(false);
		}
		else
		{
			m_pButton[i]->setInputDisable(true);
			m_pButton[i]->SetPushFlag(true);
		}
		m_pButton[i]->setModifyState(true);
	}

	// Server IP 설정
	g_pServerInfo->SetCurrentIPIdx(btidx);
	g_pFramework->PrintVersion();
}

void UIPhaseNation::ClickExit(void)
{
	OnEscapeKey();

	m_bExit_Popup = g_pFramework->GetMessageBoxManager()->IsTopMsgBox(m_pExitMessageBox);
	if (m_bExit_Popup == true)
	{
		SetInputDisable(true);
	}
}

void UIPhaseNation::ClickLogin(void)
{
	i3Stage * pStage = g_pFramework->GetCurrentStage();
	I3ASSERT(i3::same_of<UILogin*>(pStage));
	((UILogin*)pStage)->MovePhase(LOGIN_UIPHASE_LOGIN);
}
