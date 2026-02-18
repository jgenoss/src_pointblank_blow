#pragma once
#include "i3LevelPathLink.h"

class i3LevelHitPathLink : public i3LevelPathLink
{
	I3_EXPORT_CLASS_DEFINE(i3LevelHitPathLink, i3LevelPathLink);

protected:
	VEC3D					m_Offset;

public:
	i3LevelHitPathLink();

	void				SetOffsetPos(VEC3D offset) { m_Offset = offset; }

	virtual void		UpdateLink(void) override;
	virtual void		OnSelected(BOOL bSelected) override;
};

