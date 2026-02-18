#ifndef _DOMI_STAGE_PROPERTIES_H_
#define _DOMI_STAGE_PROPERTIES_H_

#include "i3Base/itl/map.h"

//----------------------------------------------
// 스테이지 별 센트리건/더미/서플라이(ammo, hp, shop) 위치/회전값
//----------------------------------------------
namespace domi
{
	struct LOCATION_TYPE
	{
		REAL32 rotation = 0.0f;
		VEC3D  position;
	};

	class Locations
	{
	public:
		explicit Locations(size_t size);
		~Locations();

		void AddLocation(const LOCATION_TYPE& src);
		const LOCATION_TYPE& GetLocation(size_t idx) const;

		size_t size() const;
		bool empty() const;

	private:
		i3::vector<LOCATION_TYPE> m_locations;

		Locations(const Locations& rhs);
		Locations& operator=(const Locations& rhs);
	};

	class StageProp : public i3::shared_ginst<StageProp>
	{
	public:
		StageProp();
		~StageProp();

		void LoadFromFile();
		void Clear();

		const Locations* GetLocations(DOMI_TYPE type) const;

	private:
		typedef i3::vector_map<DOMI_TYPE, Locations*> MapType;
		MapType m_coll;
	};
}

#endif