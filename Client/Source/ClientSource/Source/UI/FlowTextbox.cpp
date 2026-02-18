#include "pch.h"
#include "FlowTextBox.h"
#include "i3Base/string/ext/utf16_to_mb.h"

//#include "i3Base/string/ext/ismb.h"


namespace
{
	inline void SetEditboxText(const i3::rc_wstring& wstrText, i3UIEditBox* editbox)
	{
		editbox->SetText(wstrText);
	}
}

FlowTextBox::FlowTextBox() : m_Edit(0), m_PF_Update(&FlowTextBox::Update_Pre)
{
}

FlowTextBox::~FlowTextBox()
{
}

/*****************************//*****************************/
void FlowTextBox::SetControl(i3UIScene * pScene, const char* edit)
{
	i3GameNode* node = pScene->FindChildByName( edit );

	if( !node )
	{
		I3PRINTLOG(I3LOG_FATAL, "Edit Control 이 없음.. i3Uie 파일 확인");
		return;
	}

	m_Edit = (i3UIEditBox*)node;
	m_Edit->addEditBoxStyle(I3UI_EDITBOX_STYLE_ANNOUNCE);
}

void FlowTextBox::SetControl(i3UIControl* edit)
{
	m_Edit = (i3UIEditBox*)edit;
	m_Edit->addEditBoxStyle(I3UI_EDITBOX_STYLE_ANNOUNCE);
}

void FlowTextBox::SetText(const i3::rc_wstring & wstr_text)
{
	if(!m_Edit) return;

	if (m_Edit->getScrollBar() == nullptr)
		I3ASSERT("scroll bar is needed to use announce style editbox. Click \"cmd_ST_Nt\" control of PBRe_HUD.i3UIe and check \"Use Scroll\" to true. w\n");

	m_wstr_text = wstr_text;
}

void FlowTextBox::Start(INT32 LetterPerSecond, bool finish_text_show, REAL32 finish_show_time)
{
	m_acc_time = 0.f;
	m_letterPersecond = LetterPerSecond;
	m_finish_text_show = finish_text_show;
	m_finish_show_time = finish_show_time;

	m_PF_Update = &FlowTextBox::Update_On;
}

void FlowTextBox::Finish(bool finish_text_show, REAL32 finish_show_time)
{
	//m_Edit->SetTextMB( m_text.c_str() );
	SetEditboxText(m_wstr_text, m_Edit);

	m_acc_time = 0.0f;
	m_finish_text_show = finish_text_show;
	m_finish_show_time = finish_show_time;

	m_PF_Update = &FlowTextBox::Update_Post;
}

bool FlowTextBox::Update_On(REAL32 rDeltaSeconds)
{
	m_acc_time += rDeltaSeconds;

	INT32 letterCount = (INT32)(m_acc_time * m_letterPersecond);

	i3::rc_wstring wstr_outputText;
	letter_copy(wstr_outputText, m_wstr_text, letterCount);

	I3TRACE("%s \n", wstr_outputText );

	if( m_wstr_text == wstr_outputText )
	{
		m_acc_time = 0.0f;
		m_PF_Update = &FlowTextBox::Update_Post;
	}

	//m_Edit->SetTextMB(outputText);
	SetEditboxText(wstr_outputText, m_Edit);

	return true;
}

bool FlowTextBox::Update_Post(REAL32 rDeltaSeconds)
{
	if( !m_finish_text_show )	//텍스트를 보여주지 않는다.
	{
		m_PF_Update = &FlowTextBox::Update_Pre;
		//m_Edit->SetTextMB("");
		SetEditboxText(L"", m_Edit);
		return false;
	}

	m_acc_time += rDeltaSeconds;
	if( m_acc_time>m_finish_show_time )	//텍스트를 한정 시간 동안 보여준다.
	{
		m_PF_Update = &FlowTextBox::Update_Pre;
		//m_Edit->SetText("");
		SetEditboxText(L"", m_Edit);
		return false;
	}

	return true;
}

bool FlowTextBox::OnUpdate(REAL32 rDeltaSeconds)
{
	if(!m_Edit) return false;

	return (this->*m_PF_Update)(rDeltaSeconds);
}

namespace
{
	inline bool is_1st_surrogate(wchar_t c)
	{
		return 0xD800 <= c && c <= 0xDBFF;
	}
}

INT32 FlowTextBox::letter_copy(i3::rc_wstring& dest, const i3::rc_wstring& src, INT32 letterCount)
{

	const INT32 src_length = src.size();
	INT32 result_length = letterCount;
	
	for (INT32 i = 0 ; i < result_length && i < src_length; ++i )
	{
		if ( is_1st_surrogate(src[i]) )
			++result_length;			// 간단이 레터카운트를 증가시킴..
	}

	if (result_length < src_length)
	{
		i3::generic_string_ncopy(dest, src, result_length);
	}
	else
	{
		result_length = src_length;
		dest = src;
	}
			
	return result_length;
}

/*
INT32 FlowTextBox::letter_copy(char* dest, const char* src, INT32 letterCount)
{
	INT32 length = 0;
	
	for(INT32 i = 0; i < letterCount && *src; i++)
	{		
	//	INT32 code = i3String::WhatIsCode((char*)src, 0);
		
		code = i3::curr_mbb_byte(src[0], code);

		// 한글 첫바이트면 한바이트 더 복사한다.
		if (code == 1)
		{
			*dest++ = *src++;
			length++;
		}

		*dest++ = *src++;
		length++;		
	}

	*dest = '\0';

	return length;
}
*/


UINT32 FlowTextBox::GetLetterCount(const i3::wliteral_range& wTextRng)
{

	const size_t num = wTextRng.size();
	UINT32 result_len = 0;

	for (size_t i = 0 ; i < num ; ++i )
	{
		if ( is_1st_surrogate(wTextRng[i]) == false )
			++result_len;
	}
	
	return result_len;
}


/*
UINT32 FlowTextBox::GetLetterCount(const char* text)
{
	INT32 ret = 0;
	INT32 code = 0;
	
	while(*text)
	{
		// 한글 첫바이트면 뒷바이트는 뛰어넘는다.
	//	INT32 code = i3String::WhatIsCode((char*)text, 0);
		code = i3::curr_mbb_byte(*text, code);
		if (code == 1) text ++;

		text++;
		ret++;
	}

	return ret;
}
*/