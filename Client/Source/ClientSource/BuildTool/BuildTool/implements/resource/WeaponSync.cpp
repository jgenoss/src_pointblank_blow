#include "stdafx.h"
#include "WeaponSync.h"
#include "WeaponFileCrawler.h"
#include "FileCopyFuncs.h"

namespace rsc
{
	//WeaponSync::WeaponSync(const ContentsData& data, const std::string& countryName,
	//	const fs::path& media, const fs::path& qa, const fs::path& live, const fs::path& pef)
	//	: ISync(data, countryName, media, qa, live)
	//{
	//}

	//WeaponSync::~WeaponSync() {}

	//bool WeaponSync::_CollectAssocFiles(std::vector<fs::path>& outFiles)
	//{
	//	WeaponFileCrawler crawler(GetContentsData(), GetMediaPath(), m_pef, GetCountryName());
	//	crawler(outFiles);
	//	
	//	return true;
	//}

	//bool WeaponSync::_Sync_MediaToQA(const std::vector<fs::path>& files)
	//{
	//	/*for (auto &file : files)
	//	{

	//		if (file.extension.string() == "i3chr")
	//		{
	//			IFileSync* fs = new FileSync_CopyPaste(new FileSync_RegistCHR);

	//		}
	//	}*/

	//	return true;
	//}

	//bool WeaponSync::_Sync_QAToLive(const std::vector<fs::path>& files)
	//{
	//	return true;
	//}
}