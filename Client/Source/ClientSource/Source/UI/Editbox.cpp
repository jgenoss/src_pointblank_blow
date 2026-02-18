#include "pch.h"

#include "Editbox.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/ftoa.h"


Editbox::Editbox() : m_Edit(0)
{
}

Editbox::~Editbox()
{

}

/*****************************//*****************************/
void Editbox::SetControl(i3UIScene * pScene, const char* edit)
{
	i3GameNode* node = pScene->FindChildByName( edit );

	if( !node )
	{
		I3PRINTLOG(I3LOG_FATAL, "Edit Control 이 없음.. i3Uie 파일 확인");
	}

	m_Edit = (i3UIEditBox*)node;
}

void Editbox::SetControl(i3UIControl* edit)
{
	m_Edit = (i3UIEditBox*)edit;
}

void Editbox::Clear()
{
	SetText(L"");
}

void Editbox::SetText(const i3::wstring & text)
{
	if(!m_Edit) return;
	m_Edit->SetText( text );
	m_wTextRng = m_Edit->getTextRange(); 
}

void Editbox::SetText(const i3::rc_wstring& text)
{
	if(!m_Edit) return;
	m_Edit->SetText( text );
	m_wTextRng = m_Edit->getTextRange(); 
}

void Editbox::SetText(const i3::wliteral_range& text)
{
	if(!m_Edit) return;
	m_Edit->SetText( text);
	m_wTextRng = m_Edit->getTextRange(); 
}

void Editbox::SetText(const i3::stack_wstring& text)
{
	if(!m_Edit) return;
	m_Edit->SetText( text);
	m_wTextRng = m_Edit->getTextRange(); 
}


void Editbox::SetText(const wchar_t* text)
{
	if(!m_Edit) return;
	m_Edit->SetText( text );
	m_wTextRng = m_Edit->getTextRange(); 
}

void Editbox::SetText(int value)
{
	i3::wstring wstr;	i3::itoa(value, wstr);
	SetText( wstr );
}

void Editbox::SetText(REAL32 value)
{
	i3::wstring wstr;
	i3::ftoa(value, wstr, 3);
	SetText( wstr );
}

i3::wliteral_range Editbox::ExportText() const
{
	if(!m_Edit) return i3::wliteral_range(0,0);

	g_pFramework->SetFocus( m_Edit );
	const i3::wstring& wstr = m_Edit->getIMETextBufString();
	g_pFramework->SetFocus( 0 );

	return i3::wliteral_range(wstr);
}

void Editbox::SetAlphaRealNumericOnly()
{
	m_Edit->setAlphaNumericOnly(true);
	m_Edit->addEditBoxStyle(I3UI_EDITBOX_STYLE_REAL_NUMERIC);
}
void Editbox::SetAlphaNumericOnly()
{
	m_Edit->setAlphaNumericOnly(true);
//	m_Edit->addEditBoxStyle(IME_STYLE_NUMERIC_ONLY);
}
void Editbox::SetTextOnly()
{
	m_Edit->setTextReadOnly(true);
}

void Editbox::EnableReadOnly(bool enable)
{
	m_Edit->SetReadOnly(enable);
}

bool Editbox::IsAlphaNumericOnly() const
{
	return !m_Edit->isTextReadOnly();
}
bool Editbox::IsTextOnly() const
{
	return m_Edit->isTextReadOnly();
}
bool Editbox::IsReadOnly() const
{
	return m_Edit->isReadOnly();
}

void Editbox::Hide()
{
	m_Edit->EnableCtrl(false);
}
void Editbox::Show()
{
	m_Edit->EnableCtrl(true);
}
void Editbox::Enable(bool enable)
{
	m_Edit->setInputDisable(!enable);
}


bool Editbox::OnKey_Enter()
{
	if(!m_Edit) return false;
	i3UIControl* pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());

	if( m_Edit == pCurCtrl )
	{
		g_pFramework->SetFocus( 0 );
		m_wTextRng = ExportText();
		return true;
	}

	return false;
}

bool Editbox::OnKey_Escape()
{
	if(!m_Edit) return false;
	i3UIControl* pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());

	if( m_Edit == pCurCtrl )
	{
		g_pFramework->SetFocus( 0 );
		SetText(m_wTextRng);
		return true;
	}

	return false;
}

void Editbox::SetFocus() const
{
	g_pFramework->SetFocus( m_Edit );
}

bool Editbox::IsFocused() const
{
	return (g_pFramework->getFocus() == m_Edit);
}