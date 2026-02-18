#include "stdafx.h"
#include "MemoryShaderCodeEditBox.h"
#include "CustomUGCtrl.h"
#include "ShaderSyncDlg.h"
#include <fstream>
#include "resource/SyncShaders.h"
#include "util/CustomUtil.h"

namespace ui
{
	MemoryShaderCodeEditBox::MemoryShaderCodeEditBox()
		: m_fnConv(std::bind(&ConvMemoryCodeNull, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))
	{}

	MemoryShaderCodeEditBox::~MemoryShaderCodeEditBox()
	{}

	void MemoryShaderCodeEditBox::SetCustomEditCtrl(CustomEditBox* ctrl)
	{
		m_ctrl = ctrl;
	}

	void MemoryShaderCodeEditBox::SetShaderCode(const std::string& stri3ShaderMemCodeCppAbsPath, const std::string& strShaderName)
	{
		if (m_ctrl == nullptr)
			return;

		std::vector<BYTE> codes;
		rsc::ReadGlobalVarOfi3ShaderMemoryCodeCpp(stri3ShaderMemCodeCppAbsPath, strShaderName, codes);

		if (!codes.empty())
		{
			std::vector<BYTE> out;
			m_fnConv(stri3ShaderMemCodeCppAbsPath, strShaderName, codes, out);

			CString str(&out[0]);

			// CEdit 의 줄바꾸기 처리를 위해 아래와 같은 번거러운 작업이 필요함.
			str.Replace(L"\n", L"\r\n");
			str.Replace(L"\r\r", L"\r");

			m_ctrl->SetWindowText(str.GetString());

			m_strCurri3ShaderMemCodeCppAbsPath = stri3ShaderMemCodeCppAbsPath;
			m_strShaderName = strShaderName;
		}
	}

	void MemoryShaderCodeEditBox::SetCodeConvFunc(const MemoryShaderCodeConvFunc& f)
	{
		m_fnConv = f;

		if (!m_strCurri3ShaderMemCodeCppAbsPath.empty() && !m_strShaderName.empty())
			SetShaderCode(m_strCurri3ShaderMemCodeCppAbsPath, m_strShaderName);
	}

	void ConvMemoryCodeNull(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		out = std::move(in);
	}
}