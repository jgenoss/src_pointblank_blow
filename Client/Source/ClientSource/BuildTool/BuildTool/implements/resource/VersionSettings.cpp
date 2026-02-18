#include "stdafx.h"
#include "VersionSettings.h"
#include "PathHelper.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/system_error.hpp>
#include <boost/format.hpp>

namespace rsc
{
	void VersionData::SetVersion(const std::string& strVer)
	{
		m_strVersion = strVer;
	}

	const std::string& VersionData::GetVersion() const
	{
		return m_strVersion;
	}

	void VersionData::SetPointBlankAbsPath(const Path& path)
	{
		m_PointBlankAbsPath = path;
	}

	const Path& VersionData::GetPointBlankAbsPath() const
	{
		return m_PointBlankAbsPath;
	}

	void VersionData::Seti3EngineAbsPath(const Path& path)
	{
		m_i3EngineAbsPath = path;
	}

	const Path& VersionData::Geti3EngineAbsPath() const
	{
		return m_i3EngineAbsPath;
	}


	VersionSettings::VersionSettings()
	{
		VerDataPtr v1 = std::make_shared<VersionData>();
		v1->SetVersion("v1");
		m_versionDataList.push_back(v1);

		VerDataPtr v2 = std::make_shared<VersionData>();
		v2->SetVersion("v2");
		m_versionDataList.push_back(v2);

		VerDataPtr v3 = std::make_shared<VersionData>();
		v3->SetVersion("v3");
		m_versionDataList.push_back(v3);
	}

	VersionSettings::~VersionSettings()
	{}

	void VersionSettings::Load()
	{
		Path path(GetSettingsDir());
		path /= "VersionDatas";
		path /= "*.*";

		CFileFind fd;
		BOOL ok = fd.FindFile(path.c_str());
		while (ok)
		{
			ok = fd.FindNextFile();

			if (fd.IsDots())
				continue;

			if (fd.IsDirectory())
				continue;

			std::wstring wstrFileName(fd.GetFilePath().GetString());

			std::string strFileName;
			i3::utf16_to_mb(wstrFileName, strFileName);

			boost::property_tree::ptree pt;
			try
			{
				boost::property_tree::ini_parser::read_ini(strFileName.c_str(), pt);
			}
			catch (boost::property_tree::ini_parser_error& error)
			{
				I3ASSERT(error.what());
			}

			const std::string& strVersion = pt.get<std::string>("VersionSettings.Version");
			VerDataPtr data = FindData(strVersion);
			if (data->GetVersion().empty())
				continue;

			const std::string& stri3EnginePath = pt.get<std::string>("VersionSettings.i3");
			data->Seti3EngineAbsPath(stri3EnginePath);

			const std::string& strPointBlankPath = pt.get<std::string>("VersionSettings.PointBlank");
			data->SetPointBlankAbsPath(strPointBlankPath);
		}
	}

	void VersionSettings::Save()
	{
		for (const auto& data : m_versionDataList)
		{
			boost::property_tree::ptree pt;

			pt.put("VersionSettings.Version", data->GetVersion());
			pt.put("VersionSettings.i3", data->Geti3EngineAbsPath().string());
			pt.put("VersionSettings.PointBlank", data->GetPointBlankAbsPath().string());

			const std::string strINIFileAbsPath = 
				boost::io::str(boost::format("%s/VersionDatas/%s.ini") % GetSettingsDir().string() % data->GetVersion());

			if (!fs::exists(strINIFileAbsPath))
			{
				Path INIFileAbsPath(strINIFileAbsPath);
				if (!fs::exists(INIFileAbsPath.parent_path()))
					fs::create_directories(INIFileAbsPath.parent_path());
			}

			boost::property_tree::write_ini(strINIFileAbsPath, pt);
		}
	}

	struct FindVerDataPred : std::binary_function<VerDataPtr, std::string, bool>
	{
		bool operator()(const VerDataPtr& srcCountryData, const std::string& strVer) const
		{
			return srcCountryData->GetVersion() == strVer;
		}
	};

	void VersionSettings::AddData(const VersionData& verData)
	{
		const std::string& strVersion = verData.GetVersion();
		if (strVersion.empty())
			return;

		const auto it = std::find_if(m_versionDataList.begin(), m_versionDataList.end(), std::bind2nd(FindVerDataPred(), strVersion));
		if (it == m_versionDataList.end())
		{
			m_versionDataList.push_back(std::make_shared<VersionData>(verData));
		}
	}

	void VersionSettings::SetData(const VersionData& verData)
	{
		const std::string& strVersion = verData.GetVersion();
		if (strVersion.empty())
			return;

		const auto it = std::find_if(m_versionDataList.begin(), m_versionDataList.end(), std::bind2nd(FindVerDataPred(), strVersion));
		if (it != m_versionDataList.end())
		{
			*(it->get()) = verData;
		}
	}

	VerDataPtr VersionSettings::FindData(const std::string& strVer) const
	{
		auto it = std::find_if(m_versionDataList.begin(), m_versionDataList.end(), std::bind2nd(FindVerDataPred(), strVer));
		if (it != m_versionDataList.end())
			return *it;

		return std::make_shared<VersionData>();
	}

	void VersionSettings::GetAllDataList(std::vector<VerDataPtr>& out) const
	{
		for (auto& data : m_versionDataList)
			out.push_back(data);
	}
}