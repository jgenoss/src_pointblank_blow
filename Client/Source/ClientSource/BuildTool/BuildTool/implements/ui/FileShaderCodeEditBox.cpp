#include "stdafx.h"
#include "FileShaderCodeEditBox.h"
#include "CustomUGCtrl.h"
#include "ShaderSyncDlg.h"
#include <fstream>
#include "resource/SyncShaders.h"
#include "util/CustomUtil.h"

namespace ui
{
	FileShaderCodeEditBox::FileShaderCodeEditBox()
		: m_fnConv(std::bind(&ConvNull, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	{}

	FileShaderCodeEditBox::~FileShaderCodeEditBox()
	{}

	void FileShaderCodeEditBox::SetCustomEditCtrl(CustomEditBox* ctrl)
	{
		m_ctrl = ctrl;
	}

	void FileShaderCodeEditBox::SetShaderCode(const std::string& strShaderFileAbsPath)
	{
		if (m_ctrl == nullptr)
			return;

		std::vector<BYTE> codes;
		if (rsc::ReadFile(strShaderFileAbsPath.c_str(), codes))
		{
			std::vector<BYTE> out;
			m_fnConv(strShaderFileAbsPath, codes, out);

			CString str(&out[0]);

			// CEdit 의 줄바꾸기 처리를 위해 아래와 같은 번거러운 작업이 필요함.
			str.Replace(L"\n", L"\r\n");
			str.Replace(L"\r\r", L"\r");

			m_ctrl->SetWindowText(str.GetString());

			m_strCurrShaderFileAbsPath = strShaderFileAbsPath;
		}
	}

	void FileShaderCodeEditBox::SetCodeConvFunc(const FileShaderCodeConvFunc& f)
	{
		m_fnConv = f;

		if (!m_strCurrShaderFileAbsPath.empty())
			SetShaderCode(m_strCurrShaderFileAbsPath);
	}

	void ConvNull(const std::string& strHLSLAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		out = std::move(in);	
	}

	void ConvCombinedShaderCode(const std::string& strHLSLAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		std::vector<BYTE> combinedCodes;

		Path HLSLAbsPath(strHLSLAbsPath);

		rsc::GetCombinedHLSLCodes(HLSLAbsPath.parent_path().string(), in, out);
		out.push_back('\0');
	}
}