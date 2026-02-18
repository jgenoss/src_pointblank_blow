#include "pch.h"

#include "ProgressEdit.h"
#include "UIMath.h"
#include "UIMainFrame.h"
#include "UIBattleFrame.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/itoa.h"

/*****************************//*****************************/
/*****************************//*****************************/
namespace
{
	void move_pos(i3UIProgressBar* progressbar, i3UIButton* button, REAL32 interpolated_factor)
	{
		i3::pack::RECT rect;
		progressbar->getRect(&rect);

		REAL32 pos_x = uiMath::interpolate(interpolated_factor, 0.f, rect.Width() ) + rect.left;
		button->setPos(pos_x - (button->getWidth()/2), button->getPos()->y);

		progressbar->setProgress(interpolated_factor);
	}

	UIMainFrame* g_main_frame = 0;
	UIBattleFrame* g_battle_frame = 0;
	i3InputMouse* g_mouse_mgr =0;
}

iProgressEdit::iProgressEdit() : m_ProgressBar(0), m_Button(0)
{
	m_Enable = true;
	m_IsMouseMove = 0;

	g_main_frame = g_pFramework->GetUIMainframe();

	if( !g_main_frame ) g_battle_frame = UIBattleFrame::i();
	else g_battle_frame = 0;

	i3InputDeviceManager * pInputManager = g_pFramework->GetViewer()->GetInputDeviceManager();
	g_mouse_mgr = pInputManager->GetMouse();
}

iProgressEdit::~iProgressEdit()
{

}

void iProgressEdit::SetControl(UIPopupBase* parent, i3UIScene * pScene, const char* progressbar, const char* button, const char* edit)
{
	m_Parent = parent;
	i3GameNode* node1 = pScene->FindChildByName( progressbar );
	i3GameNode* node2 = pScene->FindChildByName( button );
	if( !node1 || !node2)
	{
		I3PRINTLOG(I3LOG_FATAL, "Edit Control 이 없음.. i3Uie 파일 확인");
	}
	m_ProgressBar = (i3UIProgressBar*)node1;
	m_Button = (i3UIButton*)node2;

	m_Edit.SetControl(pScene, edit);
	after_set_control();
}

void iProgressEdit::SetControl(UIPopupBase* parent, i3UIControl* progressbar, i3UIControl* button, i3UIControl* edit)
{
	m_Parent = parent;
	m_ProgressBar = (i3UIProgressBar*)progressbar;
	m_Button = (i3UIButton*)button;
	m_Edit.SetControl(edit);
	after_set_control();
}

/*****************************//*****************************/
/*virtual*/ void iProgressEdit::OnUpdate( REAL32 rDeltaSeconds)
{
	if( !m_Enable ) return;

	if( g_main_frame && !g_main_frame->IsFocusedTopPopup(m_Parent) ) return;
	if( g_battle_frame && !g_battle_frame->IsFocusedTopPopup(m_Parent) ) return;

	UINT32 state = g_mouse_mgr->GetButtonState();

	if( (state & I3I_MOUSE_LBUTTON) != I3I_MOUSE_LBUTTON )	m_IsMouseMove = false;

	if( m_IsMouseMove )
	{
		POINT pt = i3UI::getManager()->getCurrentMousePosition();
		i3::pack::RECT rect;
		m_ProgressBar->getAccumulatedRect(&rect);
		REAL32 x = uiMath::limited( (REAL32)pt.x, (REAL32)rect.left, (REAL32)rect.right );

		REAL32 factor = uiMath::inverse_interpolate((REAL32)x, (REAL32)rect.left, (REAL32)rect.right);

		// 110등분에 맞춰 소수점 절삭 처리 합니다.
		REAL32 m = (factor<1.f)?i3Math::fmod( factor, 0.0091f) : 0.f;

		adjust( factor-m, true);
	}
	else
	{
		if( (g_mouse_mgr->GetStrokeButtonState() & I3I_MOUSE_LBUTTON) == I3I_MOUSE_LBUTTON )
		{
			POINT pt = i3UI::getManager()->getCurrentMousePosition();
			
			I3UI_OVERLAP_INFO info;

			info.x = (REAL32)pt.x;
			info.y = (REAL32)pt.y;

			if( m_Button->isOnMouse( &info ))
			{
				m_IsMouseMove = true;
			}
			else if(m_ProgressBar->isOnMouse( &info ))
			{
				i3::pack::RECT rect;
				m_ProgressBar->getAccumulatedRect(&rect);
				REAL32 factor = uiMath::inverse_interpolate((REAL32)pt.x, (REAL32)rect.left, (REAL32)rect.right);
				adjust(factor, true);
			}
		}
	}
}

bool iProgressEdit::OnKey_Enter()
{
	if( m_Edit.OnKey_Enter() )
	{
		key_enter_impl();
		return true;
	}
	return false;
}

bool iProgressEdit::OnKey_Escape()
{
	if( m_Edit.OnKey_Escape() )
	{
		key_escape_impl();
		return true;
	}
	return false;
}


void iProgressEdit::Hide()
{
	m_Enable = false;
	m_ProgressBar->EnableCtrl(false);
	m_Button->EnableCtrl(false);
	m_Edit.Hide();
}
void iProgressEdit::Show()
{
	m_Enable = true;
	m_ProgressBar->EnableCtrl(true);
	m_Button->EnableCtrl(true);
	m_Edit.Show();
}
void iProgressEdit::Enable(bool enable)
{
	m_Enable = enable;
	m_Button->setInputDisable(!enable);
	m_Edit.Enable(enable);
}

void iProgressEdit::SetFocus() const
{
	m_Edit.SetFocus();
}

bool iProgressEdit::IsFocused() const
{
	i3EventReceiver* focus = g_pFramework->getFocus();

	if( focus == m_ProgressBar )	return true;
	if( focus == m_Button )	return true;

	return m_Edit.IsFocused();
}



/*****************************//*****************************/
/*****************************//*****************************/
namespace
{
	class Int_Progress_Callback_None : public iProgress_Callback
	{
	public:
		virtual void	OnChange( INT32 value ) {}
	} _int_none;
}


IntProgressEdit::IntProgressEdit() : iProgressEdit(), m_callback(&_int_none)
{
	m_Num = 0;	m_Min = INT_MIN;	m_Max = INT_MAX;
}

IntProgressEdit::~IntProgressEdit()
{
}


void IntProgressEdit::adjust(REAL32 factor, bool call_cb)
{
	move_pos(m_ProgressBar, m_Button, factor);

	m_Num = (int)ceil(( (m_Max - m_Min) * (factor) + m_Min));
	
	i3::stack_wstring wstrNum;	i3::itoa(m_Num, wstrNum);

	m_Edit.SetText( wstrNum );
	if(call_cb) m_callback->OnChange(m_Num);
}


void IntProgressEdit::key_enter_impl()
{
	int val = GetNumber();
	SetNumber(val, true);
}

void IntProgressEdit::key_escape_impl()
{
	SetNumber(m_Num, true);
}

void IntProgressEdit::after_set_control()
{
	m_Edit.SetAlphaNumericOnly();
}

void IntProgressEdit::SetNumber(int number, bool call_cb)
{
	int num = uiMath::limited( number, m_Min, m_Max );
	REAL32 inter = (num-m_Min) / (REAL32)(m_Max - m_Min);
	int factor = (int)( inter * 100);
	adjust( factor * 0.01f, call_cb );
}

int IntProgressEdit::GetNumber() const
{
	i3::wliteral_range rng = m_Edit.ExportText();
//	return StringUtil::ToParse<int>(str);
	return i3::atoi(rng);
}



/*****************************//*****************************/
/*****************************//*****************************/
namespace
{
	class Real_Progress_Callback_None : public rProgress_Callback
	{
	public:
		virtual void	OnChange( REAL32 value ) {}
	} _Real_none;
}


RealProgressEdit::RealProgressEdit() : iProgressEdit(), m_callback(&_Real_none)
{
	m_Num = 0.f;	m_Min = 0.0f;	m_Max = 0.0f;
}

RealProgressEdit::~RealProgressEdit()
{
}


void RealProgressEdit::adjust(REAL32 factor, bool call_cb)
{
	move_pos(m_ProgressBar, m_Button, factor);

	m_Num = (REAL32) uiMath::interpolate((REAL32)factor, (REAL32)m_Min, (REAL32)m_Max);
	
	i3::stack_wstring wstrNum;	i3::ftoa(m_Num, wstrNum, 2);

	m_Edit.SetText( wstrNum );
	if(call_cb) m_callback->OnChange(m_Num);
}

void RealProgressEdit::key_enter_impl()
{
	REAL32 val = GetNumber();
	SetNumber(val, true);
}

void RealProgressEdit::key_escape_impl()
{
	SetNumber(m_Num, true);
}

void RealProgressEdit::after_set_control()
{
	m_Edit.SetAlphaRealNumericOnly();
}

void RealProgressEdit::SetNumber(REAL32 number, bool call_cb)
{
	REAL32 num = uiMath::limited( number, m_Min, m_Max );
	REAL32 factor = ( (num-m_Min) / (m_Max - m_Min) );
	adjust( factor, call_cb);
}

REAL32 RealProgressEdit::GetNumber() const
{
	i3::wliteral_range rng = m_Edit.ExportText();

//	return StringUtil::ToParse<REAL32>(str);
	return i3::atof(rng);
}
