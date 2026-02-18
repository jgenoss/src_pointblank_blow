#include "stdafx.h"
#include "WeaponFileCrawlerImpl_V3.h"
#include "WeaponFileCrawlerImpl_V2.h"
#include "Settings.h"
#include "util/PathTokenizer.h"

namespace rsc
{
	WeaponFileCrawlerImpl_V3::WeaponFileCrawlerImpl_V3(ContentsDataPtr contentsData,
													   const Path& workdir,
													   const Path& PEFRelPath,
													   const std::string& strCountryName)
		: WeaponFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName),
		m_V2Impl(std::make_unique<WeaponFileCrawlerImpl_V2>(contentsData, workdir, PEFRelPath, strCountryName))
	{}

	WeaponFileCrawlerImpl_V3::~WeaponFileCrawlerImpl_V3()
	{}

	bool WeaponFileCrawlerImpl_V3::LoadPef()
	{
		return m_V2Impl->LoadPef();
	}

	void WeaponFileCrawlerImpl_V3::GatherPefFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherPefFiles(out);
	}

	void WeaponFileCrawlerImpl_V3::GatherMeshFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherMeshFiles(out);
	}

	void WeaponFileCrawlerImpl_V3::GatherEffectFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherEffectFiles(out);
	}

	void WeaponFileCrawlerImpl_V3::GatherSoundFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherSoundFiles(out);
	}

	void WeaponFileCrawlerImpl_V3::GatherUIFiles(std::vector<PathPtr>& out)
	{
		// UIRe 폴더에 있는 이미지 경로를 바탕으로 V3 UI 각 버전별로 경로명을 수정하여 모은다.
		std::vector<PathPtr> V2UIFileRelPathList;
		m_V2Impl->GatherUIFiles(V2UIFileRelPathList);

		struct ReplaceRootFolderNamePred
		{
			const char* m_folderName;
			std::vector<PathPtr>& m_outList;

			ReplaceRootFolderNamePred(const char* folderName, std::vector<PathPtr>& outList)
				: m_folderName(folderName), m_outList(outList)
			{}

			void operator()(PathPtr& src)
			{
				std::string strTok;
				GetPathToken(*src, 0, strTok);
				if (i3::generic_is_iequal(strTok, "UIRe"))
				{
					std::string strRelPath = src->string();
					strRelPath.replace(0, strTok.size(), m_folderName);

					m_outList.push_back(std::make_shared<Path>(strRelPath));
				}
			}
		};

		// V3 UI 버전별 경로 수정.
		std::vector<PathPtr> V3UIFileRelPathList;
		const CountryDataPtr countryData = CountrySettings::i()->FindData(GetCountryName());

		if (countryData->IsEnableV3UI_1_0())
			std::for_each(V2UIFileRelPathList.begin(), V2UIFileRelPathList.end(), ReplaceRootFolderNamePred("UI_V10", V3UIFileRelPathList));

		if (countryData->IsEnableV3UI_1_1())
			std::for_each(V2UIFileRelPathList.begin(), V2UIFileRelPathList.end(), ReplaceRootFolderNamePred("UI_V11", V3UIFileRelPathList));

		if (countryData->IsEnableV3UI_1_5())
			std::for_each(V2UIFileRelPathList.begin(), V2UIFileRelPathList.end(), ReplaceRootFolderNamePred("UI_V15", V3UIFileRelPathList));

		std::copy(V3UIFileRelPathList.begin(), V3UIFileRelPathList.end(), std::back_inserter(out));
	}

	void WeaponFileCrawlerImpl_V3::GatherCharaAssocFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherCharaAssocFiles(out);
	}

	void WeaponFileCrawlerImpl_V3::GatherRSCFiles(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherRSCFiles(out);
	}
	
	void WeaponFileCrawlerImpl_V3::GatherMiscs(std::vector<PathPtr>& out)
	{
		m_V2Impl->GatherMiscs(out);
	}

}