#pragma once

#include "IContentsCrawler.h"

namespace rsc
{
	class ShaderContentsCrawler final : public IContentsCrawler
	{
	public:
		ShaderContentsCrawler();
		virtual ~ShaderContentsCrawler();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}
