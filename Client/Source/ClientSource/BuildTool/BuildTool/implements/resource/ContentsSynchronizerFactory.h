#pragma once

#include "ContentsSynchronizer.h"

namespace rsc
{
	class ContentsSynchronizerFactory final : public i3::shared_ginst<ContentsSynchronizerFactory>
	{
	public:
		~ContentsSynchronizerFactory() {}

		ContentsSynchronizerPtr Request(ContentsSyncDataPtr data) const;
	};
}