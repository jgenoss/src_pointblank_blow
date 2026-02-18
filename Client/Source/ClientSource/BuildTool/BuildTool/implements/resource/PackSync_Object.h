#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_Object : public IPackSync
	{
	public:
		virtual ~PackSync_Object() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};
}
