#pragma once

#include "IContentsCrawler.h"
#include "RscTool/i3RscPartialFile.h"
#include "IPefable.h"

namespace rsc
{
	class WeaponContentsCrawler final : public IContentsCrawler, public IPefable
	{
	public:
		WeaponContentsCrawler(const Path& PEFWorkdir, const Path& PEFRelPath);
		virtual ~WeaponContentsCrawler();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}