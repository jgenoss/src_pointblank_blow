#if !defined( __C4_CONTEXT_H)
#define __C4_CONTEXT_H

#include "i3Base/itl/map.h"

struct BombInstallAreaProp
{ 
	BombInstallAreaProp();
	BombInstallAreaProp(const VEC3D& pos_, const INT32 floor_);
	bool IsValid() const;

	VEC3D pos; 
	INT32 floor;
};

class C4Context : public i3::shared_ginst<C4Context>
{
public:
	C4Context() { Reset(); i3Vector::Zero( &m_vInstalledPos);}
	~C4Context();

	void		Reset();

	bool		getInstallStart( void) const { return m_bInstallMissionStart;}
	void		setInstallStart( bool bTrue)	{ m_bInstallMissionStart = bTrue;}

	REAL32		getInstallStartTime( void) const { return m_fInstallMissionTime;}
	void		setInstallStartTime( REAL32 tm)	{ m_fInstallMissionTime = tm;}

	VEC3D*		getInstalledPos( void) { return &m_vInstalledPos;}
	void		setInstalledPos( REAL32 x, REAL32 y, REAL32 z) { i3Vector::Set( &m_vInstalledPos, x, y, z);}
	
	//VEC3D*		getBombAreaPos( BOMB_AREA area);
	//void		setBombAreaPos( BOMB_AREA area, VEC3D * pVec);

	//INT32		getBombFloorID( BOMB_AREA area);
	//void		setBombFloorID( BOMB_AREA area, INT32 id);
	void InsertAreaProp(const BOMB_AREA key, const BombInstallAreaProp& val);
	BombInstallAreaProp* FindAreaProp(const BOMB_AREA area) const;
	void ClearAreaProps();

private:
	bool		m_bInstallMissionStart;
	REAL32		m_fInstallMissionTime;

	VEC3D		m_vInstalledPos;								//실제 설치된 C4 위치.
	//VEC3D		m_vPos[MAX_BOMB_AREA_COUNT];					//UI 위치.
	//UINT32		m_nBombFloorIdx[MAX_BOMB_AREA_COUNT];			//C4 층.

	i3::map<BOMB_AREA, BombInstallAreaProp*> m_installAreaProps;
};

#endif

