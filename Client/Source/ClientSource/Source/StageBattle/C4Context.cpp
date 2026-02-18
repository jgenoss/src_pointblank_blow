#include "pch.h"
#include "C4Context.h"

#include "../StageBattle.h"

BombInstallAreaProp::BombInstallAreaProp()
{
	pos.x = pos.y = pos.z = std::numeric_limits<REAL32>::quiet_NaN();
	floor = std::numeric_limits<INT32>::quiet_NaN();
}

BombInstallAreaProp::BombInstallAreaProp(const VEC3D& pos_, const INT32 floor_)
{
	i3Vector::Copy(&pos, const_cast<VEC3D*>(&pos_));
	floor = floor_;
}

bool BombInstallAreaProp::IsValid() const
{
	if (floor == std::numeric_limits<REAL32>::quiet_NaN()) return false;

	INT32 NaN = std::numeric_limits<INT32>::quiet_NaN();
	if (pos.x == NaN || pos.y == NaN || pos.z == NaN) return false;

	return true;
}

C4Context::~C4Context()
{
	ClearAreaProps();
}

void C4Context::Reset()
{
	m_bInstallMissionStart	= true;
	m_fInstallMissionTime	= 0.0f;
}

//VEC3D* C4Context::getBombAreaPos( BOMB_AREA area)
//{
//	return &m_vPos[area];
//}
//
//void C4Context::setBombAreaPos( BOMB_AREA area, VEC3D * pVec)
//{ 
//	i3Vector::Copy( &m_vPos[area], pVec);
//}
//
//INT32 C4Context::getBombFloorID( BOMB_AREA area)
//{ 
//	return m_nBombFloorIdx[area];
//}
//void C4Context::setBombFloorID( BOMB_AREA area, INT32 id)
//{ 
//	m_nBombFloorIdx[area] = id;
//}
void C4Context::InsertAreaProp(const BOMB_AREA key, const BombInstallAreaProp& val)
{
	m_installAreaProps.insert(
		i3::map<BOMB_AREA, BombInstallAreaProp*>::value_type(key, new BombInstallAreaProp(val.pos, val.floor)));
}

BombInstallAreaProp* C4Context::FindAreaProp(const BOMB_AREA area) const
{
	i3::map<BOMB_AREA, BombInstallAreaProp*>::const_iterator cit;
	cit = m_installAreaProps.find(area);

	return (cit != m_installAreaProps.end()) ? cit->second : nullptr; 
}

void C4Context::ClearAreaProps()
{
	i3::map<BOMB_AREA, BombInstallAreaProp*>::const_iterator cit;
	for (cit = m_installAreaProps.begin(); cit != m_installAreaProps.end(); ++cit)
	{
		delete cit->second;
	}
	m_installAreaProps.clear();
}