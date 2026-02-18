#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_Null : public IPackSync
	{
	public:
		virtual ~PackSync_Null() {}

	protected:
		virtual void _RegisterRuleCreators() override {}
	};
}