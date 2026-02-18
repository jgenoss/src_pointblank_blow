#include "stdafx.h"
#include "SyncShaders.h"
#include <boost/filesystem.hpp>
#include "util/CustomUtil.h"
#include <boost/format.hpp>
#include <fstream>
#include <algorithm>

namespace
{
	bool FindIncludeQuotPos(const std::string& src, size_t& pos_lquot, size_t& pos_rquot)
	{
		const std::string strKey("#include ");
		const size_t pos_incl = src.find(strKey);
		if (pos_incl != std::string::npos)
		{
			// 주석 처리되어 있으면 무효.
			if (src.rfind("//", pos_incl) != std::string::npos)
				return false;

			// " " 범위 검색.
			pos_lquot = src.find("\"", pos_incl + strKey.size());
			if (pos_lquot == std::string::npos)
				return false;

			pos_rquot = src.find("\"", pos_lquot + 1);
			if (pos_rquot == std::string::npos)
				return false;

			if (pos_lquot == pos_rquot - 1)
				return false;

			return true;
		}

		return false;
	}

	std::string ExtractIncludeFileName(const std::string& src)
	{
		size_t pos_lquot, pos_rquot;
		if (FindIncludeQuotPos(src, pos_lquot, pos_rquot))
		{
			if (pos_lquot == pos_rquot - 1)
				return std::string();

			const size_t length = pos_rquot - pos_lquot - 1;
			return src.substr(pos_lquot + 1, length);
		}

		return std::string();
	}

	bool ReplaceFormatString(std::vector<std::string>& texts, std::vector<std::string>& outIncludes)
	{
		std::string strInclName;
		for (auto &line : texts)
		{
			strInclName = ExtractIncludeFileName(line);
			if (!strInclName.empty())
			{
				if (strInclName == "i3Decl.hlsl")
				{
					line.clear();	// i3Decl.hlsl은 엔진이 자동으로 생성하므로 제외시킨다.
				}
				else
				{
					outIncludes.push_back(strInclName);

					line = "%s";	// #include "xxx.hlsl" => %s

					static const char* fmt = R"(
// end of %s )";
					line += boost::io::str(boost::format(fmt) % strInclName);
				}
			}
		}

		return !outIncludes.empty();
	}

	struct ShaderCodeEncFmtRes
	{
		std::string strFileName;				// 셰이더 파일명(xxx.hlsl, ...)
		std::vector<BYTE> codes;				// 암호화된 16진수 코드 데이터. 포맷 문자열로 변환된다(include 처리 위해).
		std::vector<std::string> includes;		// 추가된 셰이더 파일명들.
	};
	using ShaderCodeEncFmtResPtr = std::shared_ptr<ShaderCodeEncFmtRes>;

	bool EncryptShaderCodeFormatted(const fs::path& shaderFileAbsPath, ShaderCodeEncFmtRes& out)
	{
		std::vector<BYTE> codes;
		if (rsc::ReadFile(shaderFileAbsPath.string(), codes))
		{
			out.strFileName = shaderFileAbsPath.filename().string();

			// 소스 코드를 포맷 문자열로 변환 : #include "" -> %s, 헤더 처리 위해.
			rsc::ConvertHLSLCodeToFormat(codes, out.codes, out.includes);

			// 암호화.
			BitRotateEncript((UINT8*)(&out.codes[0]), out.codes.size(), 3);

			return true;
		}

		return false;
	}

	void EncryptShaderCodesFormatted(const std::vector<Path>& HLSLAbsPathList, std::vector<ShaderCodeEncFmtResPtr>& out)
	{
		ShaderCodeEncFmtResPtr res;
		for (const auto& path : HLSLAbsPathList)
		{
			res = std::make_shared<ShaderCodeEncFmtRes>();

			if (EncryptShaderCodeFormatted(path, *res))
				out.push_back(res);
		}
	}

	// i3ShaderMemCode.cpp 파일 생성.
	void Exporti3EngineSourceFilesWithFmtCodes(const std::vector<ShaderCodeEncFmtResPtr>& resList, const Path& i3EngineTargetAbsPath)
	{
		std::string strSourceCode;

		strSourceCode = R"(#include "i3GfxType.h"
#include "i3ShaderMemoryCode.h"
#include "i3Base/itl/map.h"
#include "i3Base/itl/algorithm/transform.h"	
#include "i3Base/smart_ptr/shared_ptr.h"
#include "i3Base/itl/algorithm/find_if.h"
	)";

		strSourceCode += R"(

namespace
{
	/*template<typename ... Args>
	i3::string format_str(const std::string& format, Args ... args)
	{
		const size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buff(new char[size]); 
		snprintf(buff.get(), size, format.c_str(), args ...);
		return i3::string(buff.get(), buff.get() + size - 1);
	}*/

	// 가변 포맷 스트링(boost format, i3::sprintf(V2 버전 이상 사용 가능)).
	// c++11 로 구현하면 위 주석 처리된 format_str 과 같다(가변인자템플릿을 사용하기 때문에 안정적이다).
	i3::string format_str(const int count, const char* fmt, ...)
	{
		if (count <= 0)
			return i3::string(fmt);

		va_list args;
		va_start(args, fmt);

		size_t size = strlen(fmt);

		for (int i = 0; i < count; i++)
			size += strlen(va_arg(args, char*));

		size += 1; // null-terminate.

		i3::vector<char> buff(size, 0);

		va_start(args, fmt);
		vsprintf_s(&buff[0], buff.size(), fmt, args);
		va_end(args);

		return i3::string(buff.begin(), buff.end()-1);
	}

	void DecryptCode(const unsigned char* encryptedCode, const size_t size, i3::vector<BYTE>& out)
	{
		out.assign(encryptedCode, encryptedCode + size);
		
		BitRotateDecript(&out[0], out.size(), 3);
		
		if (out.back() != '\0')
			out.push_back('\0');	// null-terminate.
	}
}
)";

		char* format = R"(
namespace %s
{
	static const BYTE g_encryptedCode[] = {%s};
	static i3::string Decrypt();
}
)";

		std::string strShaderName;
		std::string strBytes;
		char temp[32] = { 0, };
		for (const auto& res : resList)
		{
			strShaderName = fs::path(res->strFileName).replace_extension("").string();

			::memset(&temp, 0, sizeof(temp));
			for (const auto& byte : res->codes)
			{
				sprintf_s(temp, 32, "0x%02X", (byte & 0xFF));
				strBytes += temp;
				strBytes += ", ";
			}

			// strBytes 의 마지막 ',' 제거
			if (!strBytes.empty())
				strBytes.erase(strBytes.end() - 2);

			strSourceCode += boost::io::str(boost::format(format) % strShaderName % strBytes);

			strBytes.clear();
		}

		for (const auto& res : resList)
		{
			strShaderName = fs::path(res->strFileName).replace_extension("").string();

			if (res->includes.empty())
			{
				format = R"(
i3::string %s::Decrypt()
{
	i3::vector<BYTE> decryptedCode;
	DecryptCode(%s::g_encryptedCode, sizeof(%s::g_encryptedCode), decryptedCode);

	return i3::string(decryptedCode.begin(), decryptedCode.end()-1);
}
)";
				strSourceCode += boost::io::str(boost::format(format) % strShaderName % strShaderName % strShaderName);
			}
			else
			{
				std::string strArgs;
				for (const auto &include : res->includes)
				{
					strArgs += ", ";
					strArgs += fs::path(include).replace_extension("").string();
					strArgs += "::Decrypt().c_str()";
				}

				format = R"(
i3::string %s::Decrypt()
{
	i3::vector<BYTE> fmt_decryptedCode;
	DecryptCode(%s::g_encryptedCode, sizeof(%s::g_encryptedCode), fmt_decryptedCode);

	return format_str(%d, (const char*)&fmt_decryptedCode[0]%s);
}
)";
				strSourceCode += boost::io::str(boost::format(format) % strShaderName % strShaderName % strShaderName % res->includes.size() % strArgs);
			}
		}

		format = R"(_Register("%s", &%s::Decrypt);)";
		std::string strRegistDecryptFuncs;
		for (const auto& res : resList)
		{
			strShaderName = fs::path(res->strFileName).replace_extension("").string();
			strRegistDecryptFuncs += boost::io::str(boost::format(format) % res->strFileName % strShaderName);
			strRegistDecryptFuncs += "\n\t\t\t";
		}

		format = R"(
namespace
{
	i3::string NullDecrypt() { return i3::string(); }

	void ToUpper(i3::string& inout) 
	{ 
		i3::transform(inout.begin(), inout.end(), inout.begin(), toupper); 
	}

	class Context
	{
	private:
		// v1 버전 : i3::function 없음. std::tr1::function 사용가능.
		// v2 버전 : i3::function, std::tr1::function 사용가능.
		// v3 버전 : c++11 std::function, i3::function 사용가능.
		// 보기 안 좋아도 함수포인터로 통일하자 ㅡㅡ;
		typedef i3::string (*DecryptFuncType)();

		struct DecryptInfo
		{
			i3::string strShaderFileName;
			DecryptFuncType fnDecrypt;

			DecryptInfo(const char* name, const DecryptFuncType& f) 
				: strShaderFileName(name), fnDecrypt(f) {}
		};
		typedef i3::shared_ptr<DecryptInfo> DecryptInfoPtr;

		// map 보다 검색 효율이 더 좋을것임.
		i3::vector<DecryptInfoPtr> m_decryptInfoList;

	public:
		Context()
		{
			%s
		}

		i3::string GetDecryptedCode(const char* shaderFileName)
		{
			return _FindDecryptInfo(shaderFileName)->fnDecrypt();
		}

	private:
		void _Register(const char* shaderFileName, const DecryptFuncType& f)
		{
			i3::string strShaderFileName(shaderFileName);
			ToUpper(strShaderFileName);

			m_decryptInfoList.push_back(i3::make_shared<DecryptInfo>(strShaderFileName.c_str(), f));
		}

		const DecryptInfoPtr _FindDecryptInfo(const char* shaderFileName)
		{
			struct fn
			{
				i3::string str;		fn(const char* s) : str(s) { ToUpper(str); }
				bool operator()(const DecryptInfoPtr src) const { return (src->strShaderFileName == str); }
			};

			i3::vector<DecryptInfoPtr>::iterator it = 
				i3::find_if(m_decryptInfoList.begin(), m_decryptInfoList.end(), fn(shaderFileName));

			if (it != m_decryptInfoList.end())
				return *it;

			return i3::make_shared<DecryptInfo>("", &NullDecrypt);
		}
	} g_ctx;
}
)";

		strSourceCode += boost::io::str(boost::format(format) % strRegistDecryptFuncs);

		strSourceCode += R"(
I3_EXPORT_GFX
i3::string GetShaderMemoryCode(const char* shaderFileName)
{
	return g_ctx.GetDecryptedCode(shaderFileName);
}
)";

		{
			const Path cpp_path(i3EngineTargetAbsPath / "src/i3Gfx/i3ShaderMemoryCode.cpp");
			std::ofstream out(cpp_path.string().c_str(), std::ios::out | std::ios::trunc);
			if (out.is_open())
			{
				out.write(strSourceCode.c_str(), strSourceCode.size());
			}
		}

		{
			strSourceCode = R"(#if !defined( __I3_SHADER_MEMORY_CODE_H)
#define __I3_SHADER_MEMORY_CODE_H

// 이 파일과 i3ShaderMemoryCode.cpp 파일은 빌드툴을 이용하여 추출한다. 
// 빌드툴 이용 추출 방법
// 1. command : BuildTool.exe sync_shaders 셰이더폴더경로 추출경로.
// 2. 컨텐츠 동기화 : "Shader Sync All" 컨텐츠 검색 후 동기화.
I3_EXPORT_GFX i3::string GetShaderMemoryCode(const char* fileName);

#endif
)";

			const Path h_path(i3EngineTargetAbsPath / "include/i3Gfx/i3ShaderMemoryCode.h");
			std::ofstream out(h_path.string().c_str(), std::ios::out | std::ios::trunc);
			if (out.is_open())
			{
				out.write(strSourceCode.c_str(), strSourceCode.size());
			}
		}
	}

	struct ShaderCodeEncNonFmtRes
	{
		std::string strFileName;				// 셰이더 파일명(xxx.hlsl, ...)
		std::vector<BYTE> codes;				// 암호화된 16진수 코드 데이터.
	};
	using ShaderCodeEncNonFmtResPtr = std::shared_ptr<ShaderCodeEncNonFmtRes>;

	bool EncryptShaderCodeNonFormatted(const fs::path& shaderFileAbsPath, ShaderCodeEncNonFmtRes& out)
	{
		if (rsc::ReadFile(shaderFileAbsPath.string(), out.codes))
		{
			RemoveNullTerminates(out.codes);

			out.strFileName = shaderFileAbsPath.filename().string();
			BitRotateEncript((UINT8*)(&out.codes[0]), out.codes.size(), 3);

			return true;
		}

		return false;
	}

	void EncryptShaderCodesNonFormatted(const std::vector<Path>& HLSLAbsPathList, std::vector<ShaderCodeEncNonFmtResPtr>& out)
	{
		ShaderCodeEncNonFmtResPtr res;
		for (const auto& path : HLSLAbsPathList)
		{
			res = std::make_shared<ShaderCodeEncNonFmtRes>();

			if (EncryptShaderCodeNonFormatted(path, *res))
				out.push_back(res);
		}
	}

	void Exporti3EngineSourceFilesWithNonFmtCodes(const std::vector<ShaderCodeEncNonFmtResPtr>& resList, const Path& i3EngineTargetAbsPath)
	{
		std::string strSourceCode;

		strSourceCode = R"(#include "i3GfxType.h"
#include "i3ShaderMemoryCode.h"
#include "i3Base/itl/map.h"
#include "i3Base/itl/algorithm/transform.h"	
#include "i3Base/smart_ptr/shared_ptr.h"
#include "i3Base/itl/algorithm/find_if.h"

#if !defined REALTIME_SHADER_COMPILE_PACK
	)";

		char* format = R"(
namespace %s
{
	static const BYTE g_encryptedCode[] = {%s};
}
)";

		std::string strShaderName;
		std::string strBytes;
		char temp[32] = { 0, };
		for (const auto& res : resList)
		{
			strShaderName = fs::path(res->strFileName).replace_extension("").string();

			::memset(&temp, 0, sizeof(temp));
			for (const auto& byte : res->codes)
			{
				sprintf_s(temp, 32, "0x%02X", (byte & 0xFF));
				strBytes += temp;
				strBytes += ", ";
			}

			// strBytes 의 마지막 ',' 제거
			if (!strBytes.empty())
				strBytes.erase(strBytes.end() - 2);

			strSourceCode += boost::io::str(boost::format(format) % strShaderName % strBytes);

			strBytes.clear();
		}

		format = R"(
namespace
{
	void ToUpper(i3::string& inout)
	{
		i3::transform(inout.begin(), inout.end(), inout.begin(), toupper); 
	}

	class Context
	{
	private:
		struct ShaderInfo
		{
			i3::string strShaderFileName;
			const BYTE* encryptedCodes;
			size_t codeSize;

			ShaderInfo(const char* name, const BYTE* encCodes, const size_t size) 
				: strShaderFileName(name), encryptedCodes(encCodes), codeSize(size)
			{}
		};
		typedef i3::shared_ptr<ShaderInfo> ShaderInfoPtr;

		i3::vector<ShaderInfoPtr> m_infoList;

	public:
		Context()
		{
			%s
		}

		i3::string GetDecryptedCode(const char* shaderFileName)
		{
			ShaderInfoPtr info = _FindShaderInfo(shaderFileName);
			if (info->encryptedCodes != NULL)
			{
				i3::vector<BYTE> buff;

				const size_t size = sizeof(info->encryptedCodes);
				buff.assign(&info->encryptedCodes[0], info->encryptedCodes + info->codeSize);

				BitRotateDecript(&buff[0], buff.size(), 3);

				return i3::string(buff.begin(), buff.end());
			}

			return i3::string();
		}

	private:
		void _Register(const char* shaderFileName, const BYTE* encryptedCodes, const size_t size)
		{
			i3::string strShaderFileName(shaderFileName);
			ToUpper(strShaderFileName);

			m_infoList.push_back(i3::make_shared<ShaderInfo>(strShaderFileName.c_str(), encryptedCodes, size));
		}

		const ShaderInfoPtr _FindShaderInfo(const char* shaderFileName)
		{
			struct fn
			{
				i3::string str;		fn(const char* s) : str(s) { ToUpper(str); }
				bool operator()(const ShaderInfoPtr src) const { return (src->strShaderFileName == str); }
			};

			i3::vector<ShaderInfoPtr>::iterator it = 
				i3::find_if(m_infoList.begin(), m_infoList.end(), fn(shaderFileName));

			if (it != m_infoList.end())
				return *it;

			return i3::make_shared<ShaderInfo>("", (BYTE*)NULL, 0);
		}
	} g_ctx;
}
)";

		std::string strRegister;
		for (const auto& res : resList)
		{
			strShaderName = fs::path(res->strFileName).replace_extension("").string();
			strRegister += boost::io::str(boost::format(R"(_Register("%s", %s::g_encryptedCode, sizeof(%s::g_encryptedCode));)") % res->strFileName % strShaderName % strShaderName);
			strRegister += "\n\t\t\t";
		}

		strSourceCode += boost::io::str(boost::format(format) % strRegister);

		strSourceCode += R"(
I3_EXPORT_GFX
i3::string GetShaderMemoryCode(const char* shaderFileName)
{
	return g_ctx.GetDecryptedCode(shaderFileName);
}

#endif
)";
		{
			const Path cpp_path(i3EngineTargetAbsPath / "src/i3Gfx/i3ShaderMemoryCode.cpp");
			std::ofstream out(cpp_path.string().c_str(), std::ios::out | std::ios::trunc);
			if (out.is_open())
			{
				out.write(strSourceCode.c_str(), strSourceCode.size());
			}
		}

		{
			strSourceCode = R"(#if !defined( __I3_SHADER_MEMORY_CODE_H)
#define __I3_SHADER_MEMORY_CODE_H

// 이 파일과 i3ShaderMemoryCode.cpp 파일은 빌드툴을 이용하여 추출한다. 
// 빌드툴 이용 추출 방법
// 1. command : BuildTool.exe sync_shaders 셰이더폴더경로 추출경로.
// 2. 컨텐츠 동기화 : "Shader Sync All" 컨텐츠 검색 후 동기화.

#if !defined REALTIME_SHADER_COMPILE_PACK
I3_EXPORT_GFX i3::string GetShaderMemoryCode(const char* fileName);
#endif

#endif
)";

			const Path h_path(i3EngineTargetAbsPath / "include/i3Gfx/i3ShaderMemoryCode.h");
			std::ofstream out(h_path.string().c_str(), std::ios::out | std::ios::trunc);
			if (out.is_open())
			{
				out.write(strSourceCode.c_str(), strSourceCode.size());
			}
		}
	}
}

namespace rsc
{
	void GatherHLSLAbsPaths(const Path& HLSLFolderAbsPath, std::vector<Path>& out)
	{
		fs::directory_iterator it(HLSLFolderAbsPath);
		for (; it != fs::directory_iterator(); ++it)
		{
			if (fs::is_directory(it->status()))
				continue;

			if ((*it).path().extension().string() == ".hlsl")
			{
				if ((*it).path().filename() == "i3Decl.hlsl")	// 디버그 모드에서 실시간으로 생성되는 전처리 관련 파일로서 릴리즈에서는 i3ShaderGen::Generate() 함수에서 코드가 자동 생성된다.
					continue;

				out.push_back(*it);
			}
		}
	}

	bool ReadFile(const std::string& strAbsPath, std::vector<BYTE>& out)
	{
		std::ifstream in(strAbsPath.c_str(), std::ios::in);
		if (in.is_open())
		{
			in.seekg(0, std::ifstream::end);
			const size_t size = static_cast<size_t>(in.tellg());

			out.resize(size + 1, 0);	// +1 for null-terminate.

			in.seekg(0, std::ifstream::beg);
			in.read((char*)&out[0], size);

			out.push_back('\0');

			return true;
		}

		return false;
	}

	void ConvertHLSLCodeToFormat(const std::vector<BYTE>& in, std::vector<BYTE>& outCodes, std::vector<std::string>& outIncludes)
	{
		std::vector<std::string> texts;
		ConvertBufferToLineTexts(in, texts);

		if (!ReplaceFormatString(texts, outIncludes))
		{
			outCodes = std::move(in);
			return;
		}

		std::string str;
		for (const auto& line : texts)
		{
			str += line;
			str += "\n";
		}

		outCodes.assign(str.begin(), str.end());
		outCodes.push_back('\0');
	}

	void GetCombinedHLSLCodesRec(const std::string& strShaderFolderAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		std::vector<BYTE> fmtCodes;
		std::vector<std::string> includes;
		ConvertHLSLCodeToFormat(in, fmtCodes, includes);

		std::vector<std::string> texts;
		ConvertBufferToLineTexts(fmtCodes, texts);

		std::string strHLSLAbsPath;
		std::vector<BYTE> origCodes;
		std::vector<BYTE> combinedCodes;
		size_t idx_incl = 0;
		for (size_t i = 0; i < texts.size(); i++)
		{
			const std::string& line = texts[i];
			if (line == "%s")
			{
				origCodes.clear();
				combinedCodes.clear();

				strHLSLAbsPath = strShaderFolderAbsPath; strHLSLAbsPath += "\\" + includes[idx_incl++];
				ReadFile(strHLSLAbsPath, origCodes);

				GetCombinedHLSLCodesRec(strShaderFolderAbsPath, origCodes, combinedCodes);

				std::copy(combinedCodes.begin(), combinedCodes.end(), std::back_inserter(out));
				out.push_back('\n');
			}
			else
			{
				std::copy(line.begin(), line.end(), std::back_inserter(out));
				out.push_back('\n');
			}
		}
	}

	void GetCombinedHLSLCodes(const std::string& strShaderFolderAbsPath, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		GetCombinedHLSLCodesRec(strShaderFolderAbsPath, in, out);
		RemoveNullTerminates(out);
		out.push_back('\0');
	}

	void DecryptHLSLMemoryCodes(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		std::vector<BYTE> buff;
		rsc::ReadGlobalVarOfi3ShaderMemoryCodeCpp(stri3ShaderMemoryCodeCppAbsPath, strShaderName, buff);

		if (!buff.empty())
		{
			std::string strEncryptedCodes(buff.begin(), buff.end());

			std::string strHexByte;
			size_t pos = 0;
			int decimal;
			while ((pos = strEncryptedCodes.find(", ", pos + 1)) != std::string::npos)
			{
				strHexByte = strEncryptedCodes.substr(pos - 2, 2);
				decimal = strtol(strHexByte.c_str(), NULL, 16);

				out.push_back((char)decimal);
			}

			pos = strEncryptedCodes.find("};", pos + 1);
			if (pos != std::string::npos)
			{
				strHexByte = strEncryptedCodes.substr(pos - 2, 2);
				decimal = strtol(strHexByte.c_str(), NULL, 16);

				out.push_back((char)decimal);
			}

			BitRotateDecript(&out[0], out.size(), 3);
			out.push_back('\0');
		}
	}

	void GetCombinedHLSLMemoryCodesRec(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		std::vector<BYTE> buff;
		DecryptHLSLMemoryCodes(stri3ShaderMemoryCodeCppAbsPath, strShaderName, in, buff);

		std::vector<std::string> texts;
		ConvertBufferToLineTexts(buff, texts);

		std::vector<BYTE> origCodes;
		std::vector<BYTE> combinedCodes;
		size_t idx_incl = 0;
		std::string strIncludeName;
		std::string strShaderNameExceptExt;

		for (size_t i = 0; i < texts.size(); i++)
		{
			std::string& line = texts[i];

			strIncludeName = ExtractIncludeFileName(line);
			if (!strIncludeName.empty())
			{
				if (strIncludeName == "i3Decl.hlsl")
				{
					line.clear();
				}
				else
				{
					origCodes.clear();
					combinedCodes.clear();

					const size_t idx = strIncludeName.find(".hlsl");
					strShaderNameExceptExt = strIncludeName.substr(0, idx);

					GetCombinedHLSLMemoryCodesRec(stri3ShaderMemoryCodeCppAbsPath, strShaderNameExceptExt, origCodes, combinedCodes);

					std::copy(combinedCodes.begin(), combinedCodes.end(), std::back_inserter(out));
					out.push_back('\n');
				}
			}
			else
			{
				std::copy(line.begin(), line.end(), std::back_inserter(out));
				out.push_back('\n');
			}
		}
	}

	void GetCombinedHLSLMemoryCodes(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, const std::vector<BYTE>& in, std::vector<BYTE>& out)
	{
		GetCombinedHLSLMemoryCodesRec(stri3ShaderMemoryCodeCppAbsPath, strShaderName, in, out);
		RemoveNullTerminates(out);
		out.push_back('\0');
	}

	void GatherGlobalVarsOfEncShaderCode(const std::string& stri3ShaderMemoryCodeCppAbsPath, std::vector<std::wstring>& out)
	{
		std::vector<BYTE> buff;
		if (ReadFile(stri3ShaderMemoryCodeCppAbsPath, buff))
		{
			std::vector<std::wstring> texts;
			ConvertBufferToLineTexts(buff, texts);

			size_t pos = 0;
			std::wstring wstrShaderName;
			const std::wstring wstrKey(L"namespace ");
			for (const auto& line : texts)
			{
				pos = line.find(wstrKey);
				if (pos == std::wstring::npos)
					continue;

				wstrShaderName = line.substr(wstrKey.length());
				if (!wstrShaderName.empty())
				{
					out.push_back(wstrShaderName);
				}
			}
		}
	}

	void ReadGlobalVarOfi3ShaderMemoryCodeCpp(const std::string& stri3ShaderMemoryCodeCppAbsPath, const std::string& strShaderName, std::vector<BYTE>& out)
	{
		std::vector<BYTE> buff;
		if (ReadFile(stri3ShaderMemoryCodeCppAbsPath.c_str(), buff))
		{
			std::vector<std::string> texts;
			ConvertBufferToLineTexts(buff, texts);

			size_t pos = 0;
			std::string strKey("namespace ");	strKey += strShaderName;
			std::string strLine;
			size_t idxNamespaceLine = -1;
			for (size_t i = 0; i < texts.size(); i++)
			{
				strLine = texts[i];

				pos = strLine.find(strKey);
				if (pos != std::wstring::npos)
				{
					idxNamespaceLine = i;
					break;
				}
			}

			if (idxNamespaceLine == -1)
				return;

			const size_t idxGlobalArrayVar = idxNamespaceLine + 2;
			if (idxGlobalArrayVar >= texts.size())
				return;

			const std::string& str = texts[idxGlobalArrayVar];
			if (!str.empty())
			{
				out.assign(str.begin(), str.end());
				out.push_back('\0');
			}
		}
	}

	// 포맷 타입으로 변환.
	bool SyncShaders_Formatted(const Path& HLSLFolderAbsPath, const Path& exportAbsPath)
	{
		if (!fs::exists(HLSLFolderAbsPath))
			return false;

		if (!fs::exists(exportAbsPath))
			return false;

		// 셰이더 파일 경로 모으기.
		std::vector<Path> HLSLFileAbsPathList;
		GatherHLSLAbsPaths(HLSLFolderAbsPath, HLSLFileAbsPathList);

		// 원본 셰이더파일의 코드를 암호화된 16진수로 변환한다.
		// 코드 조합을 위해 원본 코드의 '#include ...' 구문을 포매팅 처리(%s 로 변환) 한다.
		std::vector<ShaderCodeEncFmtResPtr> resList;
		EncryptShaderCodesFormatted(HLSLFileAbsPathList, resList);

		// 암호화 결과를 엔진 소스 파일로 추출/저장. i3ShaderMemoryCode(.h, .cpp).
		Exporti3EngineSourceFilesWithFmtCodes(resList, exportAbsPath);

		return true;
	}

	bool SyncShaders_NonFormatted(const Path& HLSLFolderAbsPath, const Path& exportAbsPath)
	{
		if (!fs::exists(HLSLFolderAbsPath))
			return false;

		if (!fs::exists(exportAbsPath))
			return false;

		// 셰이더 파일 경로 모으기.
		std::vector<Path> HLSLFileAbsPathList;
		GatherHLSLAbsPaths(HLSLFolderAbsPath, HLSLFileAbsPathList);

		std::vector<ShaderCodeEncNonFmtResPtr> resList;
		EncryptShaderCodesNonFormatted(HLSLFileAbsPathList, resList);

		Exporti3EngineSourceFilesWithNonFmtCodes(resList, exportAbsPath);

		return true;
	}
}