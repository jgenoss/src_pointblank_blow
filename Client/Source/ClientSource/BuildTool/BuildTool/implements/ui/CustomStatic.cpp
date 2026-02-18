#include "stdafx.h"
#include "CustomStatic.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	CustomStatic::CustomStatic() : m_callback(std::make_unique<CustomStaticCallback>())
	{}

	CustomStatic::~CustomStatic()
	{}

	void CustomStatic::SetText(const char* text)
	{
		{
			std::unique_lock<std::mutex> lock(m_mtxQue);

			std::wstring wstr;
			std::string str(text);
			if (!str.empty())
			{
				i3::mb_to_utf16(str, wstr);
				m_textQue.push(wstr);
			}

			PostMessage(WMU_CUSTOM_STATIC_TEXT);
		}
	}

	void CustomStatic::SetText(const wchar_t* text)
	{
		{
			std::unique_lock<std::mutex> lock(m_mtxQue);

			m_textQue.push(text);

			PostMessage(WMU_CUSTOM_STATIC_TEXT);
		}
	}

	IMPLEMENT_DYNAMIC(CustomStatic, CStatic)

	BEGIN_MESSAGE_MAP(CustomStatic, CStatic)
		ON_MESSAGE(WMU_CUSTOM_STATIC_TEXT, &CustomStatic::OnSetText)
	END_MESSAGE_MAP()

	LRESULT CustomStatic::OnSetText(WPARAM wparam, LPARAM lparam)
	{
		const std::wstring wstrText = m_textQue.front();

		m_textQue.pop();

		CStatic::SetWindowTextW(wstrText.c_str());

		return TRUE;
	}
}