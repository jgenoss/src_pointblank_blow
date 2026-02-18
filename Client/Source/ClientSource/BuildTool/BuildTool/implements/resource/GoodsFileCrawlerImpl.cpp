#include "stdafx.h"
#include "GoodsFileCrawlerImpl.h"
#include "i3sCrawler.h"
#include <array>
#include <tuple>

namespace rsc
{
	GoodsFileCrawlerImpl::GoodsFileCrawlerImpl(ContentsDataPtr contentsData,
											   const Path& workdir,
											   const Path& PEFRelPath,
											   const std::string& strCountryName)
		: IPefable(workdir, PEFRelPath, strCountryName), m_contentsData(contentsData)
	{}

	GoodsFileCrawlerImpl::~GoodsFileCrawlerImpl()
	{}

	bool GoodsFileCrawlerImpl::LoadPef()
	{
		if (m_contentsData.get() == nullptr)
			return false;

		if (!OpenPef())
			return false;

		if (IPefable::GetPefNationIdx() == -1)
			return false;

		i3ResourceFileObjectInfo2* currRegistryKey = GetPef()->Findi3RegKeyRoot();
		if (currRegistryKey == nullptr)
			return false;

		const std::string& strSubCategoryName = m_contentsData->strSubcategoryName;
		currRegistryKey = GetPef()->FindChildi3TreeElement(strSubCategoryName.c_str());
		if (currRegistryKey == nullptr)
			return false;

		const std::string& strContentsName = m_contentsData->strContentsName;
		currRegistryKey = GetPef()->FindChildi3TreeElement(currRegistryKey, strContentsName.c_str());
		if (currRegistryKey == nullptr)
			return false;

		m_currRegistryKey = currRegistryKey;

		return true;
	}

	void GoodsFileCrawlerImpl::GatherPefFiles(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>(IPefable::GetRelativePath()));	// Goods.pef

		Path i3RegXMLRelPath(IPefable::GetRelativePath());
		i3RegXMLRelPath.replace_extension(".i3RegXML");

		out.push_back(std::make_shared<Path>(i3RegXMLRelPath));	// Goods.i3RegXML
	}
}