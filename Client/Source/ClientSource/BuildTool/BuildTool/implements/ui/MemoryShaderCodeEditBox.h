#pragma once

#include "CustomEditBox.h"

namespace ui
{
	using MemoryShaderCodeConvFunc = std::function<void(const std::string&, const std::string&, const std::vector<BYTE>&, std::vector<BYTE>&)>;

	class MemoryShaderCodeEditBox final
	{
	public:
		MemoryShaderCodeEditBox();
		~MemoryShaderCodeEditBox();

		void SetCustomEditCtrl(CustomEditBox* ctrl);
		void SetShaderCode(const std::string& stri3ShaderMemCodeCppAbsPath, const std::string& strShaderName);
		void SetCodeConvFunc(const MemoryShaderCodeConvFunc& f);

	private:
		CustomEditBox* m_ctrl = nullptr;
		MemoryShaderCodeConvFunc m_fnConv;
		std::string m_strCurri3ShaderMemCodeCppAbsPath;
		std::string m_strShaderName;
	};

	void ConvMemoryCodeNull(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out);
}
