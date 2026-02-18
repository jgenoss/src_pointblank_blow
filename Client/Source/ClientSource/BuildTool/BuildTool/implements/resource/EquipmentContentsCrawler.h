#pragma once

#include "IContentsCrawler.h"
#include "RscTool/i3RscPartialFile.h"
#include "IPefable.h"

namespace rsc
{
	class EquipmentContentsCrawler final : public IContentsCrawler, public IPefable
	{
	public:
		EquipmentContentsCrawler(const Path& pefWorkdir, const Path& pefRelativePath);
		virtual ~EquipmentContentsCrawler();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}