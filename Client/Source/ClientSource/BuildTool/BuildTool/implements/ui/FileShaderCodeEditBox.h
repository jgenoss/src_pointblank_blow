#pragma once

#include "CustomEditBox.h"

namespace ui
{
	using FileShaderCodeConvFunc = std::function<void(const std::string&, const std::vector<BYTE>&, std::vector<BYTE>&)>;

	class FileShaderCodeEditBox final
	{
	public:
		FileShaderCodeEditBox();
		~FileShaderCodeEditBox();

		void SetCustomEditCtrl(CustomEditBox* ctrl);
		void SetShaderCode(const std::string& strShaderFileAbsPath);
		void SetCodeConvFunc(const FileShaderCodeConvFunc& f);

	private:
		CustomEditBox* m_ctrl = nullptr;
		FileShaderCodeConvFunc m_fnConv;
		std::string m_strCurrShaderFileAbsPath;
	};

	void ConvNull(const std::string& strHLSLAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out);
	void ConvCombinedShaderCode(const std::string& strHLSLAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out);
}
