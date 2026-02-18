#include "stdafx.h"
#include "Settings.h"

namespace rsc
{
	SettingsContext::SettingsContext()
	{}

	SettingsContext::~SettingsContext()
	{}

	void SettingsContext::Load()
	{
		VersionSettings::i()->Load();
		CountrySettings::i()->Load();
	}
}