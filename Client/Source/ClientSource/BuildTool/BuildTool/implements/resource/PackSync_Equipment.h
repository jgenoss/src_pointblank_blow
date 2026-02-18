#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_Equipment : public IPackSync
	{
	public:
		virtual ~PackSync_Equipment() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};

	class PackSync_Avatar : public IPackSync
	{
	public:
		virtual ~PackSync_Avatar() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};
}