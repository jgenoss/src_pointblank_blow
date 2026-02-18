#pragma once

namespace rsc
{
	void GatherHLSLAbsPaths(const Path& HLSLFolderAbsPath, std::vector<Path>& out);
	bool ReadFile(const std::string& strAbsPath, std::vector<BYTE>& out);
	void ConvertHLSLCodeToFormat(const std::vector<BYTE>& in, std::vector<BYTE>& outCodes, std::vector<std::string>& outIncludes);
	void GetCombinedHLSLCodes(const std::string& strShaderFolderAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out);
	void GetCombinedHLSLMemoryCodes(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out);
	void DecryptHLSLMemoryCodes(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out);
	void GatherGlobalVarsOfEncShaderCode(const std::string& stri3ShaderMemoryCodeCppAbsPath, std::vector<std::wstring>& out);
	void ReadGlobalVarOfi3ShaderMemoryCodeCpp(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, std::vector<BYTE>& out);
	bool SyncShaders_Formatted(const Path& HLSLFolderAbsPath, const Path& exportAbsPath);
	bool SyncShaders_NonFormatted(const Path& HLSLFolderAbsPath, const Path& exportAbsPath);
}
