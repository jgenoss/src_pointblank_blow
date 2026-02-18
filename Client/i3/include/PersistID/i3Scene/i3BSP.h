#if !defined( __I3_BSP_H )
#define __I3_BSP_H

#include "i3Base.h"
#include "i3Geometry.h"

class I3_EXPORT_SCENE i3BSP : public i3NamedElement
{
	I3_CLASS_DEFINE( i3BSP );
protected:
	INT32		m_ID;
	VEC3D		m_MinSize;
	VEC3D		m_MaxSize;

	VEC3D		m_MidPoint;

	i3List		m_BSPList;
	i3List *	m_pPrimOrder;


	void		_BuildPrimOrder( i3VertexArray * pVA, i3IndexArray * pIA );
	void		_CheckWorldSize( i3VertexArray * pVA, UINT32 idx );
	BOOL		_BuildConvexPoly( i3VertexArray * pVA, i3BSPNode * pNode );
	BOOL		_CheckPartitionPlane( i3VertexArray * pVA, i3BSPNode * pNode , VEC4D * pOutPlane );

	void		_CalcMidpoint(void)
	{
		VEC3D	Vec;
		i3Vector::Add( &Vec, &m_MinSize, &m_MaxSize );
		i3Vector::Scale( &m_MidPoint, &Vec, 0.5f );
	}

	REAL32		_getMinX( void )							{ return i3Vector::GetX( &m_MinSize ); }
	void		_setMinX( REAL32 value )					{ i3Vector::SetX( &m_MinSize, value ); }
	REAL32		_getMaxX( void )							{ return i3Vector::GetX( &m_MaxSize ); }
	void		_setMaxX( REAL32 value )					{ i3Vector::SetX( &m_MaxSize, value ); }

	REAL32		_getMinY( void )							{ return i3Vector::GetY( &m_MinSize ); }
	void		_setMinY( REAL32 value )					{ i3Vector::SetY( &m_MinSize, value ); }
	REAL32		_getMaxY( void )							{ return i3Vector::GetY( &m_MaxSize ); }
	void		_setMaxY( REAL32 value )					{ i3Vector::SetY( &m_MaxSize, value ); }

	REAL32		_getMinZ( void )							{ return i3Vector::GetZ( &m_MinSize ); }
	void		_setMinZ( REAL32 value )					{ i3Vector::SetZ( &m_MinSize, value ); }
	REAL32		_getMaxZ( void )							{ return i3Vector::GetZ( &m_MaxSize ); }
	void		_setMaxZ( REAL32 value )					{ i3Vector::SetZ( &m_MaxSize, value ); }

public:

	i3BSPNode * getNode( UINT32 idx )						{ return (i3BSPNode *) m_BSPList.Items[ idx ]; }
	void		setNode( UINT32 idx, i3BSPNode * pNode )	{ m_BSPList.Items[ idx ] = (void*) pNode; }
	void		addNode( i3BSPNode * pNode )				{ m_BSPList.Add( pNode ); }

	UINT32		getPrimOrder( UINT32 idx )					{ return (UINT32) m_pPrimOrder->Items[ idx ]; }
	INT32		getPrimOrderCount( void )					{ return m_pPrimOrder->GetCount(); }
	
public:
	i3BSP(void);
	virtual ~i3BSP(void);

	void		Build( void );

	void		CreateNode( i3VertexArray * pVA, i3BSPNode * pParent );

	void		Compile( i3Geometry * pGeo );

	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif

