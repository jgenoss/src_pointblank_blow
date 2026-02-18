#pragma once

#include "IContentsCrawler.h"
#include "IPefable.h"

namespace rsc
{
	class WorldContentsCrawler final : public IContentsCrawler, public IPefable
	{
	public:
		WorldContentsCrawler(const Path& workdir, const Path& PEFRelPath);
		virtual ~WorldContentsCrawler();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}