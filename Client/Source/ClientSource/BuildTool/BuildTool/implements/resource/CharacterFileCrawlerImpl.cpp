#include "stdafx.h"
#include "CharacterFileCrawlerImpl.h"
#include "i3sCrawler.h"

namespace rsc
{
	CharacterFileCrawlerImpl::CharacterFileCrawlerImpl(ContentsDataPtr contentsData,
													   const Path& workdir,
													   const Path& pefRelPath,
													   const std::string& countryName)
		: IPefable(workdir, pefRelPath, countryName), m_contentsData(contentsData)
	{}

	CharacterFileCrawlerImpl::~CharacterFileCrawlerImpl()
	{}

	void CharacterFileCrawlerImpl::GatherPefFiles(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>(IPefable::GetRelativePath()));	// Character.pef 파일.

		Path i3RegXMLRelPath(IPefable::GetRelativePath());	
		i3RegXMLRelPath.replace_extension(".i3RegXML");
		out.push_back(std::make_shared<Path>(i3RegXMLRelPath));	// Character.i3RegXML 파일.
	}
}