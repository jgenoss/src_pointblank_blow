#include "stdafx.h"
#include "CountrySettings.h"
#include "MainFrm.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <array>
#include <cerrno>
#include <boost/system/system_error.hpp>
#include "PathHelper.h"

namespace rsc
{
	std::array<const char*, 3> g_arrVerLiteral = { "v1", "v2", "v3" };

	void AppendMessage(bool* validOK, std::wstring& wstrMessage, const wchar_t* wpszTopic, const std::wstring& wstrValue)
	{
		if (validOK)
			*validOK = false;

		wstrMessage += wpszTopic;	wstrMessage += L" : ";
		wstrMessage += wstrValue;	wstrMessage += L",";
	}

	void AppendMessage(bool* validOK, std::wstring& wstrMessage, const wchar_t* wpszTopic, const std::string& strValue)
	{
		if (validOK)
			*validOK = false;

		wstrMessage += wpszTopic;	wstrMessage += L" : ";

		std::wstring wstrValue;
		i3::mb_to_utf16(strValue, wstrValue);
		wstrMessage += wstrValue;	wstrMessage += L",";
	}

	std::array<const char*, 2> g_arrVCSLiteral = { "SVN", "GIT" };

	bool CheckVCS(const std::string& strVCS)
	{
		bool ok = false;

		for (auto &vcs : g_arrVCSLiteral)
		{
			if (vcs == strVCS)
			{
				ok = true;
				break;
			}
		}

		return ok;
	}

	// http://www.boost.org/doc/libs/1_39_0/libs/filesystem/test/operations_test.cpp

	using boost::system::error_code;
	using boost::system::system_category;
	using boost::system::system_error;

	void create_file(const fs::path& ph, const std::string& contents)
	{
		std::ofstream f(ph.string().c_str());

		if (!f)
			throw fs::filesystem_error("create_file", ph, error_code(errno, system_category()));
	}
}

namespace rsc
{
	const std::array<const char*, 3>& GetVersionStringList()
	{
		return g_arrVerLiteral;
	}

	const std::array<const char*, 2>& GetVCSList()
	{
		return g_arrVCSLiteral;
	}



	void CountryData::Reset()
	{
		m_strCountryName.clear();
		m_strVersion.clear();
		m_mediaWorkdir.clear();
		m_QAWorkdir.clear();
		m_liveWorkdir.clear();
		m_packScriptWorkdir.clear();
		m_PEFCountryIdx = -1;
	}

	bool CountryData::SetCountryName(const std::string& strCountryName)
	{
		if (strCountryName.empty())
			return false;

		m_strCountryName = strCountryName;

		return true;
	}

	const std::string& CountryData::GetCountryName() const
	{
		return m_strCountryName;
	}

	bool CountryData::SetVersion(const std::string& ver)
	{
		for (auto &verLiteral : g_arrVerLiteral)
		{
			if (ver.compare(verLiteral))
			{
				m_strVersion = ver;
				return true;
			}
		}

		return false;
	}

	const std::string& CountryData::GetVersion() const
	{
		return m_strVersion;
	}

	bool CountryData::SetMediaWorkdir(const Path& path)
	{
		if (!fs::exists(path))
			return false;

		m_mediaWorkdir = path;
		m_mediaWorkdir.make_preferred();

		return true;
	}

	const Path& CountryData::GetMediaWorkdir() const
	{
		return m_mediaWorkdir;
	}

	bool CountryData::SetQAWorkdir(const Path& path)
	{
		if (!fs::exists(path))
			return false;

		m_QAWorkdir = path;
		m_QAWorkdir.make_preferred();

		return true;
	}

	const Path& CountryData::GetQAWorkdir() const
	{
		return m_QAWorkdir;
	}

	bool CountryData::SetLiveWorkdir(const Path& path)
	{
		if (!fs::exists(path))
			return false;

		m_liveWorkdir = path;
		m_liveWorkdir.make_preferred();

		return true;
	}

	const Path& CountryData::GetLiveWorkdir() const
	{
		return m_liveWorkdir;
	}

	bool CountryData::SetPackScriptWorkdir(const Path& path)
	{
		if (!fs::exists(path))
			return false;

		m_packScriptWorkdir = path;
		m_packScriptWorkdir.make_preferred();

		return true;
	}

	const Path& CountryData::GetPackScriptWorkdir() const
	{
		return m_packScriptWorkdir;
	}

	bool CountryData::SetPEFCountryIdx(const int idx)
	{
		if (idx < 0)
			return false;

		m_PEFCountryIdx = idx;
		return true;
	}

	int CountryData::GetPEFCountryIdx() const
	{
		return m_PEFCountryIdx;
	}

	bool CountryData::SetMediaVCS(const std::string& strVCS)
	{
		if (CheckVCS(strVCS))
		{
			m_strMediaVCS = strVCS;
			return true;
		}

		return false;
	}

	const std::string& CountryData::GetMediaVCS() const
	{
		return m_strMediaVCS;
	}

	bool CountryData::SetQAVCS(const std::string& strVCS)
	{
		if (CheckVCS(strVCS))
		{
			m_strQAVCS = strVCS;
			return true;
		}

		return false;
	}

	const std::string& CountryData::GetQAVCS() const
	{
		return m_strQAVCS;
	}

	bool CountryData::SetLiveVCS(const std::string& strVCS)
	{
		if (CheckVCS(strVCS))
		{
			m_strLiveVCS = strVCS;
			return true;
		}

		return false;
	}

	const std::string& CountryData::GetLiveVCS() const
	{
		return m_strLiveVCS;
	}

	bool CountryData::SetPackScriptVCS(const std::string& strVCS)
	{
		if (CheckVCS(strVCS))
		{
			m_strPackScriptVCS = strVCS;
			return true;
		}

		return false;
	}

	const std::string& CountryData::GetPackScriptVCS() const
	{
		return m_strPackScriptVCS;
	}

	void CountryData::SetEnableV3UI_1_0(const bool ok)
	{
		m_enable_V3UI_1_0 = ok;
	}

	bool CountryData::IsEnableV3UI_1_0() const
	{
		return m_enable_V3UI_1_0;
	}

	void CountryData::SetEnableV3UI_1_1(const bool ok)
	{
		m_enable_V3UI_1_1 = ok;
	}

	bool CountryData::IsEnableV3UI_1_1() const
	{
		return m_enable_V3UI_1_1;
	}

	void CountryData::SetEnableV3UI_1_5(const bool ok)
	{
		m_enable_V3UI_1_5 = ok;
	}

	bool CountryData::IsEnableV3UI_1_5() const
	{
		return m_enable_V3UI_1_5;
	}

	bool CountryData::SetMediaLocaleFolderRelPath(const Path& relPath)
	{
		if (!fs::exists(m_mediaWorkdir / relPath))
			return false;

		m_mediaLocaleFolderRelPath = relPath;
		m_mediaLocaleFolderRelPath.make_preferred();

		return true;
	}

	const Path& CountryData::GetMediaLocaleFolderRelPath() const
	{
		return m_mediaLocaleFolderRelPath;
	}

	bool CountryData::CheckValid()
	{
		bool validOK = true;
		std::wstring wstrMessage(L"유효하지 않은 값이 있습니다. 아래 정보를 확인하여 수정 바랍니다. \n");

		std::wstring wstrCountryName;
		i3::mb_to_utf16(m_strCountryName, wstrCountryName);

		if (m_strCountryName.empty())
			AppendMessage(&validOK, wstrMessage, L"CountryName", L"Empty");

		bool versionOK = false;
		for (auto &ver : g_arrVerLiteral)
		{
			if (ver == m_strVersion)
				versionOK = true;
		}

		if (versionOK == false)
			AppendMessage(&validOK, wstrMessage, L"Version", m_strVersion);

		if (!fs::exists(m_mediaWorkdir))
			AppendMessage(&validOK, wstrMessage, L"MediaWorkdir", m_mediaWorkdir.wstring());

		if (!fs::exists(m_QAWorkdir))
			AppendMessage(&validOK, wstrMessage, L"QAWorkdir", m_QAWorkdir.wstring());

		if (!fs::exists(m_liveWorkdir))
			AppendMessage(&validOK, wstrMessage, L"LiveWorkdir", m_liveWorkdir.wstring());

		if (m_PEFCountryIdx < 0)
			AppendMessage(&validOK, wstrMessage, L"MediaWorkdir", std::to_wstring(m_PEFCountryIdx));

		if (!fs::exists(m_packScriptWorkdir))
			AppendMessage(&validOK, wstrMessage, L"PackScriptWorkdir", m_packScriptWorkdir.wstring());

		if (!CheckVCS(m_strMediaVCS))
			AppendMessage(&validOK, wstrMessage, L"Version Control System", m_strMediaVCS);

		if (!CheckVCS(m_strQAVCS))
			AppendMessage(&validOK, wstrMessage, L"Version Control System", m_strQAVCS);

		if (!CheckVCS(m_strLiveVCS))
			AppendMessage(&validOK, wstrMessage, L"Version Control System", m_strLiveVCS);

		if (!CheckVCS(m_strPackScriptVCS))
			AppendMessage(&validOK, wstrMessage, L"Version Control System", m_strPackScriptVCS);

		return validOK;
	}


	CountrySettings::CountrySettings()
	{
		Load();
	}

	CountrySettings::~CountrySettings() {}

	void CountrySettings::Load()
	{
		m_countryDataList.clear();

		CFileFind fd;

		Path path(GetSettingsDir());
		path /= "CountryDatas";
		path /= "*.*";

		BOOL ok = fd.FindFile(path.c_str());

		while (ok)
		{
			ok = fd.FindNextFile();

			if (fd.IsDots())
				continue;

			if (fd.IsDirectory())
			{
				std::wstring wstrCountryFolderPath(fd.GetFilePath().GetString());

				if (i3::generic_is_iequal(wstrCountryFolderPath, L"default"))
					continue;

				std::string strCountryFolderPath;
				i3::utf16_to_mb(wstrCountryFolderPath, strCountryFolderPath);

				std::string strConfigFilePath(strCountryFolderPath);
				strConfigFilePath += "/config.ini";

				// INI 파일 이용. boost INI 관련 참조 : http://boost-extras.sourceforge.net/ini_file/ini_file.html

				boost::property_tree::ptree pt;

				try
				{
					boost::property_tree::ini_parser::read_ini(strConfigFilePath.c_str(), pt);
				}
				catch (boost::property_tree::ini_parser_error& error)
				{
					I3ASSERT(error.what());
				}

				CountryData data;

				const std::string& strCountryName = Path(strCountryFolderPath).filename().string();
				if (!data.SetCountryName(strCountryName))
					continue;

				const std::string& strVersion = pt.get<std::string>("CountrySettings.Version");
				if (!data.SetVersion(strVersion))
					continue;

				if (!data.SetMediaWorkdir(pt.get<std::string>("CountrySettings.MediaWorkdir")))
					continue;

				if (!data.SetMediaVCS(pt.get<std::string>("CountrySettings.MediaVersionControlSystem")))
					continue;

				if (!data.SetMediaLocaleFolderRelPath(pt.get<std::string>("CountrySettings.MediaLocaleFolderRelPath")))
					continue;

				if (!data.SetQAWorkdir(pt.get<std::string>("CountrySettings.QAWorkdir")))
					continue;

				if (!data.SetQAVCS(pt.get<std::string>("CountrySettings.QAVersionControlSystem")))
					continue;

				if (!data.SetLiveWorkdir(pt.get<std::string>("CountrySettings.LiveWorkdir")))
					continue;

				if (!data.SetLiveVCS(pt.get<std::string>("CountrySettings.LiveVersionControlSystem")))
					continue;

				if (!data.SetPEFCountryIdx(pt.get<int>("CountrySettings.PEFCountryIndex")))
					continue;

				if (!data.SetPackScriptWorkdir(pt.get<std::string>("CountrySettings.PackScriptWorkdir")))
					continue;

				if (!data.SetPackScriptVCS(pt.get<std::string>("CountrySettings.PackScriptVersionControlSystem")))
					continue;

				if (i3::generic_is_iequal("v3", strVersion))
				{
					data.SetEnableV3UI_1_0(pt.get<bool>("CountrySettings.Enable_V3_UI_1_0"));
					data.SetEnableV3UI_1_1(pt.get<bool>("CountrySettings.Enable_V3_UI_1_1"));
					data.SetEnableV3UI_1_5(pt.get<bool>("CountrySettings.Enable_V3_UI_1_5"));
				}

				if (!IsDataExist(strCountryName))
					m_countryDataList.push_back(std::make_unique<CountryData>(data));
			}
		}

		fd.Close();
	}

	void GetINIFileAbsPathList(std::vector<std::string>& out)
	{
		CFileFind fd;

		Path path(GetSettingsDir());
		path /= "CountryDatas";
		path /= "*.*";

		BOOL ok = fd.FindFile(path.c_str());
		while (ok)
		{
			ok = fd.FindNextFile();

			if (fd.IsDots())
				continue;

			if (fd.IsDirectory())
			{
				std::wstring wstrINIFileAbsPath(fd.GetFilePath().GetString());
				wstrINIFileAbsPath += L"\\config.ini";

				std::string strINIFileAbsPath;
				i3::utf16_to_mb(wstrINIFileAbsPath, strINIFileAbsPath);

				out.push_back(strINIFileAbsPath);
			}
		}
	}

	void CountrySettings::Save()
	{
		for (auto &currCountryData : m_countryDataList)
		{
			if (!currCountryData->CheckValid())
				continue;

			// 파일 저장.
			boost::property_tree::ptree pt;

			pt.put("CountrySettings.Version", currCountryData->GetVersion());
			pt.put("CountrySettings.MediaWorkdir", currCountryData->GetMediaWorkdir().string());
			pt.put("CountrySettings.MediaVersionControlSystem", currCountryData->GetMediaVCS());
			pt.put("CountrySettings.MediaLocaleFolderRelPath", currCountryData->GetMediaLocaleFolderRelPath().string());
			pt.put("CountrySettings.QAWorkdir", currCountryData->GetQAWorkdir().string());
			pt.put("CountrySettings.QAVersionControlSystem", currCountryData->GetQAVCS());
			pt.put("CountrySettings.LiveWorkdir", currCountryData->GetLiveWorkdir().string());
			pt.put("CountrySettings.LiveVersionControlSystem", currCountryData->GetLiveVCS());
			pt.put("CountrySettings.PEFCountryIndex", std::to_string(currCountryData->GetPEFCountryIdx()));
			pt.put("CountrySettings.PackScriptWorkdir", currCountryData->GetPackScriptWorkdir().string());
			pt.put("CountrySettings.PackScriptVersionControlSystem", currCountryData->GetPackScriptVCS());

			if (i3::generic_is_iequal("v3", currCountryData->GetVersion()))
			{
				pt.put("CountrySettings.Enable_V3_UI_1_0", currCountryData->IsEnableV3UI_1_0());
				pt.put("CountrySettings.Enable_V3_UI_1_1", currCountryData->IsEnableV3UI_1_1());
				pt.put("CountrySettings.Enable_V3_UI_1_5", currCountryData->IsEnableV3UI_1_5());
			}

			Path INIFileAbsPath(GetSettingsDir());
			INIFileAbsPath /= "CountryDatas";
			INIFileAbsPath /= currCountryData->GetCountryName();
			INIFileAbsPath /= "config.ini";

			if (!fs::exists(INIFileAbsPath))
			{
				Path defaultINIFileAbsPath(GetSettingsDir());
				defaultINIFileAbsPath /= "CountryDatas";
				defaultINIFileAbsPath /= "default";
				defaultINIFileAbsPath /= "config.ini";

				if (!fs::exists(INIFileAbsPath))
				{
					Path dstAbsDir;

					if (fs::is_directory(INIFileAbsPath))
						dstAbsDir = INIFileAbsPath;
					else
						dstAbsDir = INIFileAbsPath.parent_path();	// 파일이면 상위 폴더를 대상으로 생성.

					if (!fs::exists(dstAbsDir))
						fs::create_directories(dstAbsDir);
				}

				try
				{
					if (fs::exists(defaultINIFileAbsPath))
						fs::copy_file(defaultINIFileAbsPath, INIFileAbsPath, fs::copy_option::overwrite_if_exists);
				}
				catch (const boost::system::system_error &err)
				{
					err.what();
				}
			}

			boost::property_tree::write_ini(INIFileAbsPath.string().c_str(), pt);
		}
	}

	struct FindCountryDataPred : std::binary_function<CountryDataPtr, std::string, bool>
	{
		bool operator()(const CountryDataPtr& srcCountryData, const std::string& strCountryName) const
		{
			return srcCountryData->GetCountryName() == strCountryName;
		}
	};

	void CountrySettings::AddData(const CountryData& data)
	{
		if (!IsDataExist(data.GetCountryName()))
			m_countryDataList.push_back(std::make_shared<CountryData>(data));
	}

	void CountrySettings::SetData(const CountryData& data)
	{
		if (!IsDataExist(data.GetCountryName()))
			return;

		auto countryData = FindData(data.GetCountryName());
		*(countryData.get()) = data;	// 깊은 복사 요소가 없으므로 대입처리.
	}

	void CountrySettings::DelData(const std::string& strCountryName)
	{
		auto it = std::find_if(m_countryDataList.begin(), m_countryDataList.end(), std::bind2nd(FindCountryDataPred(), strCountryName));
		if (it != m_countryDataList.end())
		{
			// Settings 하위 폴더의 해당 국가 폴더 삭제.
			std::vector<std::string> INIAbsPathList;
			GetINIFileAbsPathList(INIAbsPathList);

			struct fn : std::binary_function<std::string, std::string, bool>
			{
				bool operator()(const std::string& src, const std::string& strCountryName) const
				{
					const Path srcPath(src);
					const std::string strCountryFolderName = srcPath.parent_path().filename().string();

					return i3::generic_is_iequal(strCountryFolderName, strCountryName);
				}
			};

			auto it_ = std::find_if(INIAbsPathList.begin(), INIAbsPathList.end(), std::bind2nd(fn(), (*it)->GetCountryName()));
			if (it_ != INIAbsPathList.end())
			{
				Path INIFileAbsPath(*it_);
				fs::remove_all(INIFileAbsPath.parent_path());	// 폴더 삭제.
			}

			m_countryDataList.erase(it);
		}
	}

	CountryDataPtr CountrySettings::FindData(const std::string& strCountryName)
	{
		auto it =
			std::find_if(m_countryDataList.begin(), m_countryDataList.end(), std::bind2nd(FindCountryDataPred(), strCountryName));

		if (it != m_countryDataList.end())
			return *it;

		return std::make_shared<CountryData>();
	}

	bool CountrySettings::IsDataExist(const std::string& strCountryName) const
	{
		auto it =
			std::find_if(m_countryDataList.begin(), m_countryDataList.end(), std::bind2nd(FindCountryDataPred(), strCountryName));

		return it != m_countryDataList.end();
	}

	void CountrySettings::GetDataList(std::vector<CountryDataPtr>& outList) const
	{
		std::copy(m_countryDataList.begin(), m_countryDataList.end(), std::back_inserter(outList));
	}
}