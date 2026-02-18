#pragma once

#include "WeaponFileCrawlerImpl.h"

namespace rsc
{
	class WeaponFileCrawlerImpl_V2;

	class WeaponFileCrawlerImpl_V3 : public WeaponFileCrawlerImpl
	{
	public:
		WeaponFileCrawlerImpl_V3(ContentsDataPtr contentsData,
								 const Path& workdir,
								 const Path& PEFRelPath,
								 const std::string& strCountryName);

		virtual ~WeaponFileCrawlerImpl_V3();

		virtual bool LoadPef() override;
		virtual void GatherPefFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMeshFiles(std::vector<PathPtr>& out) override;
		virtual void GatherEffectFiles(std::vector<PathPtr>& out) override;
		virtual void GatherSoundFiles(std::vector<PathPtr>& out) override;
		virtual void GatherUIFiles(std::vector<PathPtr>& out) override;
		virtual void GatherCharaAssocFiles(std::vector<PathPtr>& out) override;
		virtual void GatherRSCFiles(std::vector<PathPtr>& out) override;
		virtual void GatherMiscs(std::vector<PathPtr>& out) override;

	private:
		using V2ImplPtr = std::unique_ptr<WeaponFileCrawlerImpl_V2>;
		V2ImplPtr m_V2Impl;
	};
}