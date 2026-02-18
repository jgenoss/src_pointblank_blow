#include "stdafx.h"
#include "CompatiblePackScriptsCrawler.h"
#include <boost/filesystem.hpp>
#include "jsoncpp/json.h"
#include <fstream>
#include <boost/scoped_array.hpp>
#include "FolderFilesCrawler.h"
#include "ResourceController.h"
#include "PathHelper.h"

namespace rsc
{
	CompatiblePackScriptsCrawler::CompatiblePackScriptsCrawler(const Path& workdir, 
															   const Path& fileRelPath, 
															   const Path& packScriptWorkdir)
		: IFilePathable(workdir, fileRelPath), m_packScriptWorkdir(packScriptWorkdir)
	{
	}

	CompatiblePackScriptsCrawler::~CompatiblePackScriptsCrawler() 
	{
	}

	bool CompatiblePackScriptsCrawler::_Load()
	{
		const Path& fileAbsPath = IFilePathable::GetAbsolutePath();

		if (!fs::exists(fileAbsPath))
			return false;

		if (!i3::generic_is_iequal(fileAbsPath.filename().extension().string(), ".JSON"))
			return false;

		return true;
	}

	bool CompatiblePackScriptsCrawler::_Gather(std::vector<PathPtr>& out)
	{
		// json 파일을 읽어 메모리에 저장.
		std::ifstream file(IFilePathable::GetAbsolutePath().string());
		file.seekg(0, std::ifstream::end);

		const int fileSize = static_cast<int>(file.tellg());

		file.seekg(0, std::ifstream::beg);

		boost::scoped_array<char> buffer(new char[fileSize]);	// char* buffer = new char[fileSize];

		file.read(buffer.get(), fileSize);

		// json 파싱. 팩스크립트 목록을 긁어모은다.
		Json::Value root;
		Json::Reader reader;

		if (!reader.parse(buffer.get(), root))
			return false;

		const Json::Value array = root["ScriptList"];
		std::vector<Path> packScriptRelpathList;
		for (unsigned int i = 0; i < array.size(); i++)
		{
			const char* packScriptPath = array[i].asCString();
			packScriptRelpathList.push_back(packScriptPath);
		}

		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(m_packScriptWorkdir);
		folderCwl.SetFileSearchKeys( { ".i3PackScript" } );

		for (auto &packScriptRelPath : packScriptRelpathList)
		{
			if (!fs::exists(m_packScriptWorkdir / packScriptRelPath))
				continue;

			// 팩스크립트 파일일 경우 직접 저장하고 폴더일 경우에는 폴더 내 팩스크립트 파일을 긁어모아서 저장.
			if (i3::generic_is_iequal(packScriptRelPath.extension().string(), ".i3PackScript"))
			{
				out.push_back(std::make_shared<Path>(packScriptRelPath));
				continue;
			}

			folderCwl.SetRelativePath(packScriptRelPath);
			folderCwl.operator()(out);
		}

		/*for (auto &fileRelPath : outFileRelPathList)
			fileRelPath = m_packScriptWorkdir / fileRelPath;*/

		return true;
	}

	IFileCrawler* CompatiblePackScriptsCrawler::Creator_V1()
	{
		const CountryDataPtr countryData = CountrySettings::i()->FindData(ResourceController::i()->GetCurrCountryName());
		
		return new CompatiblePackScriptsCrawler(GetSettingsDir(), "CompatiblePackScripts/CompatiblePackScripts_V1.json", countryData->GetPackScriptWorkdir());
	}

	IFileCrawler* CompatiblePackScriptsCrawler::Creator_V2()
	{
		const CountryDataPtr countryData = CountrySettings::i()->FindData(ResourceController::i()->GetCurrCountryName());

		return new CompatiblePackScriptsCrawler(GetSettingsDir(), "CompatiblePackScripts/CompatiblePackScripts_V2.json", countryData->GetPackScriptWorkdir());
	}

	IFileCrawler* CompatiblePackScriptsCrawler::Creator_V3()
	{
		const CountryDataPtr countryData = CountrySettings::i()->FindData(ResourceController::i()->GetCurrCountryName());

		return new CompatiblePackScriptsCrawler(GetSettingsDir(), "CompatiblePackScripts/CompatiblePackScripts_V3.json", countryData->GetPackScriptWorkdir());
	}
}