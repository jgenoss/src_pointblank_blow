#include "stdafx.h"
#include "EquipmentFileCrawlerImpl.h"
#include "i3sCrawler.h"

namespace rsc
{
	EquipmentFileCrawlerImpl::EquipmentFileCrawlerImpl(ContentsDataPtr contentsData, 
													   const Path& workdir,
													   const Path& PEFRelPath,
													   const std::string& strCountryName)
		: IPefable(workdir, PEFRelPath, strCountryName), m_contentsData(contentsData), m_workdir(workdir)
	{}

	EquipmentFileCrawlerImpl::~EquipmentFileCrawlerImpl() 
	{}

	bool EquipmentFileCrawlerImpl::LoadPef()
	{
		if (m_contentsData.get() == nullptr)
			return false;

		if (!OpenPef())
			return false;

		if (IPefable::GetPefNationIdx() == -1)
			return false;

		i3ResourceFileObjectInfo2* registryKey = GetPef()->Findi3RegKeyRoot();
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement("Equipment");
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strSubcategoryName.c_str());
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strContentsName.c_str());
		if (registryKey == nullptr)
			return false;

		m_currRegistryKey = registryKey;

		return true;
	}

	void EquipmentFileCrawlerImpl::GatherPefFiles(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>(IPefable::GetRelativePath()));	// Equipment.pef 파일.
	
		Path i3RegXMLRelPath(IPefable::GetRelativePath());	
		i3RegXMLRelPath.replace_extension(".i3RegXML");
		out.push_back(std::make_shared<Path>(i3RegXMLRelPath));	// Equipment.i3RegXML 파일.
	}

	bool GatherEquipMeshSharingAssocFiles(const Path& workdir, const Path& i3sRelPath, const char* pszPefRelPath, std::vector<PathPtr>& out)
	{
		const i3SGTexChangeInfo* texChangeInfo = i3SGTexChangeManager::i()->FindTexChangeInfo(i3sRelPath.string().c_str());
		if (texChangeInfo)
		{
			i3::rc_string strOrigi3sRelPath = i3SGTexChangeManager::i()->GetRefi3sFilename(texChangeInfo);	// 원본 메시 파일.
			out.push_back(std::make_shared<Path>(strOrigi3sRelPath.c_str()));

			i3::vector<i3::rc_string> strTexRelPathList;
			i3SGTexChangeManager::GatherAllTargetTexturePaths(texChangeInfo, strTexRelPathList);

			for (auto &strTexRelPath : strTexRelPathList)
				out.push_back(std::make_shared<Path>(strTexRelPath.c_str()));

			// 원본 i3s 파일 대상 재검색.
			i3sCrawler i3sCwl;	
			i3sCwl.SetWorkdir(workdir);
			i3sCwl.SetRelativePath(strOrigi3sRelPath.c_str());
			i3sCwl.operator()(out);

			// pef 파일.
			Path pefRelPath(pszPefRelPath);
			out.push_back(std::make_shared<Path>(pefRelPath));
			out.push_back(std::make_shared<Path>(pefRelPath.replace_extension(".i3RegXML")));

			return true;
		}

		return false;
	}
}