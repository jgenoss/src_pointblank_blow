#if !defined( __I3_COLLISION_Group_H)
#define __I3_COLLISION_Group_H

#include "i3Collidee.h"

class I3_EXPORT_SCENE i3CollideeGroup : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3CollideeGroup, i3ElementBase);
protected:
	i3::vector<void*>	m_List;
	INT32	m_Type = 0;

public:
	bool	Add(void* pCollidee);
	bool	Remove(void* pCollidee);
	bool	Clear(void);
	void	SetType(INT32 type)			{m_Type = type;}
	INT32	GetType(void)				{return m_Type;}
};

#endif
