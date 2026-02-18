#if !defined( __I3_BSP_NODE_H )
#define __I3_BSP_NODE_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3Base/itl/vector_util.h"

class I3_EXPORT_SCENE i3BSPNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3BSPNode, i3Node );
protected:
	VEC4D 	m_Plane;		// 분할할 플래너

	bool	m_bLeaf = false;	// true : 차일드노드가 없다.( m_pParts를 검사한다 ) false : 차일드노드가 있다( 폴리곤이 있으나 Leaf가 아니다)
	i3::vector<void*>	m_Parts;		// 해당 노드에 들어가는 폴리곤리스트 (인덱스값)
							// 컬리젼 박스 리스트( Geometry )

public:
	INT32	m_nCount = 0;
	INT32	m_nDivideCount = 0;

	bool	IsLeaf( void )					{ return m_bLeaf; }
	void	setLeaf( bool bValue )			{ m_bLeaf = bValue; }
	
	VEC4D *	getPlane( void )				{ return &m_Plane; }
	void	setPlane( VEC4D * pVec )		{ i3Vector::Copy( &m_Plane, pVec ); }
	void	setPlane( REAL32 x, REAL32 y, REAL32 z, REAL32 w )	{ i3Vector::Set( &m_Plane, x, y, z, w ); }

	INT32	getPartCount( void )			{ return (INT32)m_Parts.size(); }
	void	setPartCount( INT32 nCount )	{ m_Parts.resize( nCount ); }

	void *	getPart( INT32 idx )			{ return m_Parts[ idx ]; }
	void	setPart( INT32 idx, void * value )	{ i3::vu::set_value_force(m_Parts, idx, value ); }
	void	addPart( void * part )			{ m_Parts.push_back( part ); }
	void	removePart( void * part )		{ i3::vu::remove(m_Parts, part ); }

public:
	virtual ~ i3BSPNode(void);

	void	Create( void );
	void	ReleaseParts( void );

	i3::vector<void*>* CollisionTest( VEC3D * pVec );
	
	virtual UINT32 OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile ) override;
	
};


#endif	// __I3_BSP_NODE_H
