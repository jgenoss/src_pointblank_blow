#include "pch.h"

#if legacy_gui_function
#include "GuiNotifyReceiver.h"
#include "GlobalVariables.h"

#include "UI/UIMainFrame.h"

//I3_ABSTRACT_CLASS_INSTANCE(CGuiNotifyReceiver, i3GameNode);

I3_CLASS_INSTANCE(CGuiNotifyReceiver);


static char * s_szGuiSoundFile[ GUI_SND_MAX] = 
{
	"UI_Scroll_In.wav",
	"UI_Scroll_Out.wav",
	"UI_PopUp_Open.wav",
	"UI_PopUp_Close.wav",
	"UI_Button_Click_Select.wav",
	"UI_Item_Equip.wav",
};

CGuiNotifyReceiver::CGuiNotifyReceiver()
{
	m_slideTime = 0.0f;
	m_slideState = SS_OUT;
	m_slideX = 0.0f;
	m_slideY = 0.0f;

	i3mem::FillZero( m_pGuiSound, sizeof( m_pGuiSound));
}

CGuiNotifyReceiver::~CGuiNotifyReceiver()
{
	for( INT32 i=0;i < GUI_SND_MAX; ++i)
	{
		I3_SAFE_RELEASE( m_pGuiSound[i]);
	}
}

void CGuiNotifyReceiver::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate(rDeltaSeconds);

	// GUI 윈도우 등장/퇴장 처리
	OnSliding(rDeltaSeconds);
}

void CGuiNotifyReceiver::OnGameEvent(INT32 event,INT32 arg)
{
	// Empty
}

void CGuiNotifyReceiver::OnInitControl()
{
	//	GUI 사운드 로딩
	for( INT32 i=0;i < GUI_SND_MAX; ++i)
	{
		if( m_pGuiSound[i] == nullptr)
		{
			//	중복 로딩을 막기위해 komet
			m_pGuiSound[i] = i3SoundPlayInfo::new_object();

			i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( s_szGuiSoundFile[i]);

			I3ASSERT( pRes != nullptr);

			i3SoundPlayInfo * pSndResInfo = pRes->getSound();
			if( pSndResInfo != nullptr)
			{
				pSndResInfo->CopyTo( m_pGuiSound[i], I3_COPY_INSTANCE);
			}
		}
	}
}

void CGuiNotifyReceiver::OnExitKey(void)
{
	OnSubExitKey();
}

void CGuiNotifyReceiver::OnSubExitKey(void)
{
	SYSTEM_POPUP_Q(GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_NORMAL_QE"), EXIT_CALLBACK(CbExitNormal,true), this);

}

void CGuiNotifyReceiver::OnWheelUpdate(INT32 scrollby)
{
	// Empty
}

void CGuiNotifyReceiver::SetInputDisable(bool bDisable)
{
	// Empty
}

bool CGuiNotifyReceiver::GetInputDisable(void)
{
	return false;
}

bool CGuiNotifyReceiver::InputKeyboardEnter(void)
{ 
	if( GetInputDisable() == false) 
		return g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER );

	return false;
	}

void CGuiNotifyReceiver::CbExitNormal(void* pThis,INT32 nParam)
{
}

bool CGuiNotifyReceiver::PlaySound(GuiSound type)
{
//	bool bNeedPlay = true;

	I3_BOUNDCHK((INT32) type, GUI_SND_MAX);

	if( g_pSndMng->tellPlaySound( m_pGuiSound[type]) == nullptr)
		return false;

	return true;
}

void CGuiNotifyReceiver::ExclusionPopup(i3GuiPopupList* pPopup)
{
	if (pPopup)
	{
		pPopup->SetFocus(!pPopup->getIsFocus());
	}
}

void CGuiNotifyReceiver::SlideIn(void)
{	
	if (SS_IN != m_slideState)
	{
		m_slideState = SS_SLIDE_IN;
		m_slideTime = 0.0f;
	}
}

void CGuiNotifyReceiver::SlideOut(void)
{
	if (SS_OUT != m_slideState)
	{
		m_slideState = SS_SLIDE_OUT;
		m_slideTime = 0.0f;
	}
}

bool CGuiNotifyReceiver::OnSliding(REAL32 rDeltaSeconds)
{
	// 등장/퇴장이 완료되었다면 더이상 처리할 필요 없다.
	if (SS_IN == m_slideState || SS_OUT == m_slideState)
	{
		return false;
	}


	// 기준시간이 지나면 이동을 완료한다.
	// 완료순간까지는 이동을 처리해야한다.
	if (SS_SLIDE_IN == m_slideState)
	{
		m_slideTime += rDeltaSeconds * 2.0f;
	}
	else if (SS_SLIDE_OUT == m_slideState)
	{
		m_slideTime += rDeltaSeconds * 3.0f;
	}

	m_slideTime = MIN(1.0f, m_slideTime);

	if (1.0f <= m_slideTime)
	{
		switch(m_slideState)
		{
		case SS_SLIDE_IN:	m_slideState = SS_IN;	break;
		case SS_SLIDE_OUT:	m_slideState = SS_OUT;	break;
		default:			/* Do nothing */		break;
		}
	}


	// 최종 상태에 따른 기준 위치
	switch(m_slideState)
	{
	case SS_IN:
		m_slideX = 0.0f;
		m_slideY = 0.0f;
		break;
	case SS_OUT:
		m_slideX = (REAL32)g_pViewer->GetViewWidth();
		m_slideY = (REAL32)g_pViewer->GetViewHeight();
		break;
	case SS_SLIDE_IN:
		m_slideX = (REAL32)g_pViewer->GetViewWidth() * i3Math::cos(I3_PI2 * m_slideTime);
		m_slideY = (REAL32)g_pViewer->GetViewHeight() * i3Math::cos(I3_PI2 * m_slideTime);
		break;
	case SS_SLIDE_OUT:
		m_slideX = (REAL32)g_pViewer->GetViewWidth() * i3Math::sin(I3_PI2 * m_slideTime);
		m_slideY = (REAL32)g_pViewer->GetViewHeight() * i3Math::sin(I3_PI2 * m_slideTime);
		break;
	default:
		// Do nothing
		break;
	}

	return true;
}

REAL32 CGuiNotifyReceiver::GetSlideTime(void) const
{
	return m_slideTime;
}

SlideState CGuiNotifyReceiver::GetSlideState(void) const
{
	return m_slideState;
}

REAL32 CGuiNotifyReceiver::GetSlideX(void) const
{
	return m_slideX;
}

REAL32 CGuiNotifyReceiver::GetSlideY(void) const
{
	return m_slideY;
}


#endif