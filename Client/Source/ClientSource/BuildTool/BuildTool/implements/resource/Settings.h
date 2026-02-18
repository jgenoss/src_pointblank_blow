#pragma once

#include "CountrySettings.h"
#include "VersionSettings.h"

namespace rsc
{
	class SettingsContext final : public i3::shared_ginst<SettingsContext>
	{
	public:
		SettingsContext();
		~SettingsContext();

		void Load();
	};
}