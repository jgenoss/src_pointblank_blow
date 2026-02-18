#include "stdafx.h"
#include "ContentsStateChecker.h"
#include "MediaFileComparator.h"
#include "LiveFileComparator.h"
#include "LiveFileClassifier.h"
#include "IFileCrawler.h"
#include "ContentsStateCallback.h"
#include "FileCrawlerFactory.h"

namespace rsc
{
	ContentsStateChecker::ContentsStateChecker(ContentsDataPtr contentsData, CountryDataPtr countryData)
		: m_currContentsData(contentsData), m_currCountryData(countryData),
		m_callback(std::make_unique<ContentsStateCallback_Null>())
	{
	}

	ContentsStateChecker::~ContentsStateChecker()
	{
	}

	void ContentsStateChecker::operator()() const
	{
		auto key = std::make_pair(m_currCountryData->GetVersion(), m_currContentsData->strCategoryName);
		auto crawler = FileCrawlerFactory::i()->Create(
			key, m_currContentsData, m_currCountryData->GetMediaWorkdir(), m_currCountryData->GetCountryName());
		
		std::vector<PathPtr> fileRelPathList;
		crawler->operator()(fileRelPathList);

		MediaFileComparator	compM(m_currCountryData->GetMediaWorkdir());
		compM.AddFileRelPathList(fileRelPathList);

		LiveFileClassifiers::i()->Classify(m_currContentsData->strCategoryName, fileRelPathList);

		QAFileComparator compQ(m_currCountryData->GetQAWorkdir());
		compQ.AddFileRelPathList(fileRelPathList);

		LiveFileComparator compL(m_currCountryData->GetLiveWorkdir());
		compL.AddFileRelPathList(fileRelPathList);

		ContentsStatusFlag status;

		if (!compM.CheckFiles())
			status.set(ContentsStatus::NOT_EXISTS_IN_MEDIA);
		
		if (!compQ.CheckFiles())
			status.set(ContentsStatus::NOT_EXISTS_IN_QA);
		
		if (!compL.CheckFiles())
			status.set(ContentsStatus::NOT_EXISTS_IN_LIVE);

		if (status.any())
		{
			if (m_currContentsData->status != status)
				m_callback->OnUpdateState(m_currContentsData, m_currCountryData->GetCountryName(), status);
			return;
		}

		if (!compQ.IsEqualTo(compM))
			status.set(ContentsStatus::DIFF_MEDIA_QA);

		if (!compL.IsEqualTo(compQ))
			status.set(ContentsStatus::DIFF_QA_LIVE);

		if (!status.any())
			status.set(ContentsStatus::SYNC_OK);

		m_callback->OnUpdateState(m_currContentsData, m_currCountryData->GetCountryName(), status);
	}

	void ContentsStateChecker::SetCallback(ContentsStateCallbackPtr callback)
	{
		m_callback = std::move(callback);
	}
}