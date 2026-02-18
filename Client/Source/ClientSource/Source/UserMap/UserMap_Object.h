#if !defined( __USERMAP_OBJECT_H)
#define __USERMAP_OBJECT_H

#include "UserMap_Define.h"

class CUserMap_Object : public i3Object
{
	I3_CLASS_DEFINE( CUserMap_Object);
protected:
	USMAP::OBJECT::INFO		m_Info;
	USMAP::BLOCK::INFO *	m_pBlock;
	i3BoundBox				m_BoundBox;

protected:
	void					_Rec_ResizeGeo( i3Node * pNode, VEC3D * pSz);
	void					_getScaleOption( char * pszName, VEC3D * pOrg, VEC3D * pNew);

public:
	CUserMap_Object(void);
	virtual ~CUserMap_Object(void);

	USMAP::OBJECT::INFO	*	getInfo(void)			{ return &m_Info; }
	i3BoundBox *			getBound(void)			{ return &m_BoundBox; }

	void					SetSize( REAL32 cx, REAL32 cy, REAL32 cz, bool bConfirm = true);

	void					SetSnappedPos( VEC3D * pPos);

	void					Create( UINT16 blockID);
};

#endif
