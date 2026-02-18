#if !defined( __I3_COLLISION_Group_H)
#define __I3_COLLISION_Group_H

#include "i3Collidee.h"

class I3_EXPORT_SCENE i3CollideeGroup : public i3ElementBase
{
	I3_CLASS_DEFINE( i3CollideeGroup);
public:
	i3CollideeGroup(void);
	virtual ~i3CollideeGroup(void);

protected:
	i3List	m_List;
	INT32	m_Type;

public:
	BOOL	Add(void* pCollidee);
	BOOL	Remove(void* pCollidee);
	BOOL	Clear(void);
	void	SetType(INT32 type)			{m_Type = type;}
	INT32	GetType(void)				{return m_Type;}
};

#endif
