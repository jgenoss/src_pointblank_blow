#if !defined( __I3_PVS_H )
#define __I3_PVS_H


#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextureEnableAttr.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3AlphaTestEnableAttr.h"
#include "i3PVSAttr.h"
#include "i3Geometry.h"


class I3_EXPORT_SCENE i3PVS : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3PVS );
protected:
	INT32	m_Count;

	i3List	m_LeafList;
	i3List	m_PortalList;

	i3Geometry	*	m_pGeometry;



	void	_RemoveAllList(void);

	void	_AddLeaf( i3PVSAttr * pLeaf );
	void	_SetLeaf( UINT32 idx, i3PVSAttr * pLeaf );
	void	_RemoveLeaf( i3PVSAttr * pLeaf );

public:
	void	setGeometry( i3Geometry * pGeo )	{ m_pGeometry = pGeo; }
	i3Geometry * getGeometry( void )			{ return m_pGeometry; }

public:
	i3PVS();
	virtual ~i3PVS();

	void	Create();

	void	Build();

	void	BuildLeaf();

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};


#endif // __I3_PVS_H
