#pragma once

#include "ISync.h"
#include "IPefable.h"

namespace rsc
{
	/*class WeaponSync : public ISync
	{
	public:
		WeaponSync(const ContentsData& data, const std::string& countryName, 
			const fs::path& media, const fs::path& qa, const fs::path& live, const fs::path& pef);

		virtual ~WeaponSync();

	private:
		virtual bool _CollectAssocFiles(std::vector<fs::path>& outFiles) override;
		virtual bool _Sync_MediaToQA(const std::vector<fs::path>& files) override;
		virtual bool _Sync_QAToLive(const std::vector<fs::path>& files) override;

	private:
		fs::path m_pef;
	};*/
}