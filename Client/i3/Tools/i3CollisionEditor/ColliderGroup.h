#if !defined( __COLLIDER_GROUP_H)
#define __COLLIDER_GROUP_H

class ColliderGroup : public i3ElementBase
{
	I3_CLASS_DEFINE( ColliderGroup);
protected:
	COLOR		m_Color[I3_TERRAIN_TYPE_COUNT];
	INT32		m_Group;

public:
	ColliderGroup(void);
	virtual ~ColliderGroup(void);

	COLOR *		getColor(void)				{ return &m_Color[ m_Group]; }

	INT32		getGroup(void)				{ return m_Group; }
	void		setGroup( INT32 grp)		{ m_Group = grp; }
};

#endif
