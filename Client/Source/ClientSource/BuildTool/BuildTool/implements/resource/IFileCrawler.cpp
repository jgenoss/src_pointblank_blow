#include "stdafx.h"
#include "IFileCrawler.h"
#include "../util/CustomUtil.h"

namespace rsc
{
	IFileCrawler::~IFileCrawler() {}

	bool IFileCrawler::operator()(std::vector<PathPtr>& out)
	{
		if (!_Load())
			return false;

		std::vector<PathPtr> gatheredList;
		if (!_Gather(gatheredList))
			return false;

		MakeUnique(gatheredList);
		std::copy(gatheredList.begin(), gatheredList.end(), std::back_inserter(out));

		return true;
	}

	void MakeUnique(std::vector<PathPtr>& out)
	{
		// ../../xx.ext => ..\\..\\xx.ext	(윈도우 기준).
		for (auto &curr : out)
			curr->make_preferred();

		RemoveDuplicates(out);
	}

	FileCrawlerWrapper::FileCrawlerWrapper(IFileCrawler* p) : m_crawler(p)
	{}

	FileCrawlerWrapper::~FileCrawlerWrapper()
	{}

	bool FileCrawlerWrapper::operator()(std::vector<PathPtr>& out)
	{
		return m_crawler->operator()(out);
	}
}