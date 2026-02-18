#pragma once

#include "util/ObjectFactory.h"
#include <map>

namespace rsc
{
	class IPathHelper;

	class PathHelperFactory : public FlyweightFactory<IPathHelper, std::string>, public i3::shared_ginst < PathHelperFactory >
	{
	public:
		PathHelperFactory();

		virtual ~PathHelperFactory() {}
	};

	class IPathHelper
	{
	public:
		virtual ~IPathHelper() {}

		virtual const char* GetUIPath() const { return ""; }
	};

	class PathHelper_V1 : public IPathHelper
	{
	public:
		virtual ~PathHelper_V1() {}

		virtual const char* GetUIPath() const override { return "Gui"; }
	};

	class PathHelper_V2 : public IPathHelper
	{
	public:
		virtual ~PathHelper_V2() {}

		virtual const char* GetUIPath() const override { return "UIRe/Image/Weapon_detail"; }
	};

	const Path GetSettingsDir();
	const Path GetExeDir();
}