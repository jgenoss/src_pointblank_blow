#if !defined( __I3_PVS_ATTR_H )
#define __I3_PVS_ATTR_H

#include "i3Base.h"
#include "i3Node.h"


class I3_EXPORT_SCENE i3PVSAttr : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3PVSAttr );
protected:
	i3List		m_PortentialList;

	void	_RemoveAllPortList(void)				{ m_PortentialList.Clear(); }

public:
	UINT32	getPortal( UINT32 idx )					{ return (UINT32) m_PortentialList.Items[ idx ]; }
	i3List * getPortList(void)						{ return &m_PortentialList; }
	void	setPortList( UINT32 idx, UINT32 value )	{ m_PortentialList.SetItem( idx, (void*) value ); }
	void	addPortList( UINT32 value )				{ m_PortentialList.Add( (void*) value ); }

public:
	i3PVSAttr();
	virtual ~i3PVSAttr();

	virtual	UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};


#endif // I3_PVS_ATTR_H
