#include "stdafx.h"
#include "CharacterFileCrawler.h"
#include "CharacterFileCrawlerImpl.h"
#include "CharacterFileCrawlerImpl_V1.h"
#include "CharacterFileCrawlerImpl_V2.h"
#include "CharacterFileCrawlerImpl_V3.h"

namespace rsc
{
	CharacterFileCrawler::CharacterFileCrawler(CharacterFileCrawlerImpl* impl)
		: m_impl(impl)
	{}

	CharacterFileCrawler::~CharacterFileCrawler()
	{}

	bool CharacterFileCrawler::_Load()
	{
		return m_impl->LoadPef();
	}

	bool CharacterFileCrawler::_Gather(std::vector<PathPtr>& out)
	{
		m_impl->GatherPefFiles(out);
		m_impl->GatherMeshAssocFiles(out);
		m_impl->GatherEquipmentAssocFiles(out);
		m_impl->GatherRSCAssocFiles(out);
		m_impl->GatherMiscs(out);

		return true;
	}

	IFileCrawler* CharacterFileCrawler::Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v1 =
			new CharacterFileCrawlerImpl_V1(contentsData, workdir, "config/Character.pef", "config/Equipment.pef", strCountryName);

		return new CharacterFileCrawler(impl_v1);
	}

	IFileCrawler* CharacterFileCrawler::Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v2 =
			new CharacterFileCrawlerImpl_V2(contentsData, workdir, "config/Character.pef", "config/Equipment_Backup.pef", strCountryName);	// V3 작업 때문에 Equipment_Backup.pef를 사용. 양승천에게 문의.

		return new CharacterFileCrawler(impl_v2);
	}

	IFileCrawler* CharacterFileCrawler::Creator_V3(ContentsDataPtr data, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v3 =
			new CharacterFileCrawlerImpl_V3(data, workdir, "config/Character.pef", "config/Equipment.pef", strCountryName);

		return new CharacterFileCrawler(impl_v3);
	}
}