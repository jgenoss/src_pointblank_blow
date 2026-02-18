#include "pch.h"
#include "DomiStageProperties.h"
#include "Domination_Defines.h"

namespace domi
{
	Locations::Locations(size_t size)
	{
		m_locations.reserve(size);
	}

	Locations::~Locations()
	{
		m_locations.clear();
	}

	void Locations::AddLocation(const LOCATION_TYPE& src)
	{
		m_locations.push_back(src);
	}

	const LOCATION_TYPE& Locations::GetLocation(size_t idx) const
	{
		return m_locations.at(idx);
	}

	size_t Locations::size() const
	{
		return m_locations.size();
	}

	bool Locations::empty() const
	{
		return m_locations.empty();
	}


	StageProp::StageProp() {}
	StageProp::~StageProp()
	{
		Clear();
	}

	void StageProp::LoadFromFile()
	{
		i3IniParser parser;

		char filepath[MAX_PATH] = {0,};
		const char* name = g_pFramework->getStageInfo(g_pFramework->GetCurrentStageIndex())->GetName();
		i3::snprintf(filepath, MAX_PATH, "World/%s/%s_Common.ini", name, name);

		if (!parser.OpenFromFile(filepath))
			I3ASSERT_0;

		if (!m_coll.empty())
			m_coll.clear();

		INT32 commonCount = 0;
		char commonStr[64] = {0, };

		if (!parser.ReadSection(_T("Common"))) I3ASSERT_0;

		parser.GetValue("SentryGun_Count", &commonCount);
		Locations* sentryLocsPtr = new Locations(commonCount);
		MEMDUMP_NEW( sentryLocsPtr, sizeof( Locations));

		for (INT32 i=0; i<commonCount; i++)
		{
			LOCATION_TYPE location;
			i3::snprintf(commonStr, 64, "SentryGun%02d_X", i); parser.GetValue(commonStr, &location.position.x);
			i3::snprintf(commonStr, 64, "SentryGun%02d_Y", i); parser.GetValue(commonStr, &location.position.y);
			i3::snprintf(commonStr, 64, "SentryGun%02d_Z", i); parser.GetValue(commonStr, &location.position.z);

			sentryLocsPtr->AddLocation(location);
		}
		m_coll.insert(MapType::value_type(DOMI_TYPE_SENTRYGUN, sentryLocsPtr));

		parser.GetValue("Dummy_Count", &commonCount);
		Locations* dummyLocsPtr = new Locations(commonCount);
		MEMDUMP_NEW( dummyLocsPtr, sizeof( Locations));

		for (INT32 i=0; i<commonCount; i++)
		{
			LOCATION_TYPE location;
			i3::snprintf(commonStr, 64, "Dummy%02d_X", i); parser.GetValue(commonStr, &location.position.x);
			i3::snprintf(commonStr, 64, "Dummy%02d_Y", i); parser.GetValue(commonStr, &location.position.y);
			i3::snprintf(commonStr, 64, "Dummy%02d_Z", i); parser.GetValue(commonStr, &location.position.z);
			i3::snprintf(commonStr, 64, "Dummy%02d_Angle", i); parser.GetValue(commonStr, &location.rotation);

			dummyLocsPtr->AddLocation(location);
		}
		m_coll.insert(MapType::value_type(DOMI_TYPE_DUMMY, dummyLocsPtr));

		parser.GetValue("HPSupply_Count", &commonCount);
		Locations* hpsupplyLocsPtr = new Locations(commonCount);
		MEMDUMP_NEW( hpsupplyLocsPtr, sizeof( Locations));

		for (INT32 i=0; i<commonCount; i++)
		{
			LOCATION_TYPE location;
			i3::snprintf(commonStr, 64, "HPSupply%02d_X", i); parser.GetValue(commonStr, &location.position.x);
			i3::snprintf(commonStr, 64, "HPSupply%02d_Y", i); parser.GetValue(commonStr, &location.position.y);
			i3::snprintf(commonStr, 64, "HPSupply%02d_Z", i); parser.GetValue(commonStr, &location.position.z);
			i3::snprintf(commonStr, 64, "HPSupply%02d_Angle", i); parser.GetValue(commonStr, &location.rotation);

			hpsupplyLocsPtr->AddLocation(location);
		}
		m_coll.insert(MapType::value_type(DOMI_TYPE_HPSUPPLY, hpsupplyLocsPtr));

		parser.GetValue("AmmoSupply_Count", &commonCount);
		Locations* ammosupplyLocsPtr = new Locations(commonCount);
		MEMDUMP_NEW( ammosupplyLocsPtr, sizeof( Locations));

		for (INT32 i=0; i<commonCount; i++)
		{
			LOCATION_TYPE location;
			i3::snprintf(commonStr, 64, "AmmoSupply%02d_X", i); parser.GetValue(commonStr, &location.position.x);
			i3::snprintf(commonStr, 64, "AmmoSupply%02d_Y", i); parser.GetValue(commonStr, &location.position.y);
			i3::snprintf(commonStr, 64, "AmmoSupply%02d_Z", i); parser.GetValue(commonStr, &location.position.z);
			i3::snprintf(commonStr, 64, "AmmoSupply%02d_Angle", i); parser.GetValue(commonStr, &location.rotation);

			ammosupplyLocsPtr->AddLocation(location);
		}
		m_coll.insert(MapType::value_type(DOMI_TYPE_AMMOSUPPLY, ammosupplyLocsPtr));

		parser.GetValue("SkillShop_Count", &commonCount);
		Locations* skillshopLocsPtr = new Locations(commonCount);
		MEMDUMP_NEW( skillshopLocsPtr, sizeof( Locations));

		for (INT32 i=0; i<commonCount; i++)
		{
			LOCATION_TYPE location;
			i3::snprintf(commonStr, 64, "SkillShop%02d_X", i); parser.GetValue(commonStr, &location.position.x);
			i3::snprintf(commonStr, 64, "SkillShop%02d_Y", i); parser.GetValue(commonStr, &location.position.y);
			i3::snprintf(commonStr, 64, "SkillShop%02d_Z", i); parser.GetValue(commonStr, &location.position.z);
			i3::snprintf(commonStr, 64, "SkillShop%02d_Angle", i); parser.GetValue(commonStr, &location.rotation);

			skillshopLocsPtr->AddLocation(location);
		}
		m_coll.insert(MapType::value_type(DOMI_TYPE_SKILLSHOP, skillshopLocsPtr));

		parser.Close();
	}

	void StageProp::Clear()
	{
		for (MapType::iterator it(m_coll.begin()); it != m_coll.end(); ++it)
		{
			I3_SAFE_DELETE( it->second);
		}
		m_coll.clear();
	}

	const Locations* StageProp::GetLocations(DOMI_TYPE type) const
	{
		MapType::const_iterator it = m_coll.find(type);
		if (it != m_coll.end())
			return it->second;

		return nullptr;
	}
}