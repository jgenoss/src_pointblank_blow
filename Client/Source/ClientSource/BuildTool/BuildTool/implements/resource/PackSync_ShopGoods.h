#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_ShopGoods : public IPackSync
	{
	public:
		virtual ~PackSync_ShopGoods() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};
}
