#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_Weapon_V1 : public IPackSync
	{
	public:
		virtual ~PackSync_Weapon_V1() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};

	class PackSync_Weapon_V2 : public IPackSync
	{
	public:
		virtual ~PackSync_Weapon_V2() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};

	class PackSync_Weapon_V3 : public IPackSync
	{
	public:
		virtual ~PackSync_Weapon_V3() {}

	protected:
		virtual void _RegisterRuleCreators() override;
	};
}