#pragma once

#include "WeaponFileCrawlerImpl.h"

namespace rsc
{
	class WeaponFileCrawlerImpl_V2 : public WeaponFileCrawlerImpl
	{
	public:
		WeaponFileCrawlerImpl_V2(ContentsDataPtr contentsData, 
								 const Path& pef_workdir,
								 const Path& pef_relpath, 
								 const std::string& countryName);

		virtual ~WeaponFileCrawlerImpl_V2();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshFiles(std::vector<PathPtr>& out) override;
		virtual void GatherEffectFiles(std::vector<PathPtr>& out) override;
		virtual void GatherSoundFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) override;
		virtual void GatherCharaAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;
	};
}