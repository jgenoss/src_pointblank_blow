#include "stdafx.h"
#include "UIFileCrawlerImpl.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	UIFileCrawlerImpl::UIFileCrawlerImpl(ContentsDataPtr contentsData, 
										 const Path& workdir, 
										 const Path& folderRelPath,
										 const std::string& strCountryName)
		: IFilePathable(workdir, folderRelPath), m_contentsData(contentsData)
	{}

	UIFileCrawlerImpl::~UIFileCrawlerImpl()
	{}
}