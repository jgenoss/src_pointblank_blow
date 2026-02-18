#if !defined( __USERMAP_WORLD_H)
#define __USERMAP_WORLD_H

#include "UserMap_Define.h"

class CUserMap_World : public i3GameObj
{
	I3_CLASS_DEFINE( CUserMap_World);
protected:
	i3AttrSet *				m_pAttrSet;
	i3TextureBindAttr *		m_pTexBindAttr;
	i3GeometryAttr *		m_pGeoAttr;
	i3VertexArray *			m_pVA;

	i3PhysixShapeSet *		m_pGroundCvx;

	i3Node *				m_pSky;
	USMAP::SKY::TYPE		m_SkyType;

	i3Light *				m_pLight;

public:
	CUserMap_World(void);
	virtual ~CUserMap_World(void);

	void	Create(void);
	
	void	SetSize( REAL32 cx, REAL32 cz);
	void	SetSky( USMAP::SKY::TYPE type);
};

#endif
