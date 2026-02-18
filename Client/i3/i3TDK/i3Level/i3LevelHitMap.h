#pragma once
#include "i3LevelElement3D.h"
#include "i3LevelHitPoint.h"
#include "i3LevelHitPathLink.h"

struct HITMAPINFO {
	i3LevelHitPoint* _pKillerOj = nullptr;
	i3LevelHitPoint* _pTargetOj = nullptr;
	i3LevelHitPathLink* _pPathLink = nullptr;
	UINT64 _uiIUserID = 0;
};
class I3_EXPORT_TDK i3LevelHitMap : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE(i3LevelHitMap, i3LevelElement3D);
	i3::vector<HITMAPINFO*>	m_HitMapList;

public:
	i3LevelHitMap();
	~i3LevelHitMap();

	void		AddHitPoint(UINT64 uiUserID, char* killerName, char* deathName, VEC3D killerPos, VEC3D deathPos);
	void		Update();
	void		SetColorChange(UINT64 uiUserID);
	void		DeleteAllPoint();
};

