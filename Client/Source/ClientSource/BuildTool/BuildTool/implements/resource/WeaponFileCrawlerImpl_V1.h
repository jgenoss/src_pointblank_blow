#pragma once

#include "WeaponFileCrawlerImpl.h"

namespace rsc
{
	class WeaponFileCrawlerImpl_V1 : public WeaponFileCrawlerImpl
	{
	public:
		WeaponFileCrawlerImpl_V1(ContentsDataPtr contentsData, 
								 const Path& pefWorkdir,
								 const Path& pefRelativePath, 
								 const std::string& strCountryName);

		virtual ~WeaponFileCrawlerImpl_V1();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshFiles(std::vector<PathPtr>& out) override;
		virtual void GatherEffectFiles(std::vector<PathPtr>& out) override;
		virtual void GatherSoundFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) override;
		virtual void GatherCharaAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCFiles(std::vector<PathPtr>& out) override;
	};
}