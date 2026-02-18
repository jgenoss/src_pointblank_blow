#if !defined( __I3_BSP_NODE_H )
#define __I3_BSP_NODE_H

#include "i3Base.h"
#include "i3Node.h"

typedef struct ALIGN4 _tagI3PERSIST_BSPNODE
{	
	PERSIST_VEC4D	m_Plane;				// 나눠줄 플랜
	BOOL			m_bLeaf;				// 차일드가 없다
	INT32			m_PartCount;			// 오브젝트 갯수
	OBJREF			m_Parts;				// 오브젝트
	
} I3PERSIST_BSPNODE;

class I3_EXPORT_SCENE i3BSPNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3BSPNode );
protected:
	VEC4D 	m_Plane;		// 분할할 플래너

	BOOL	m_bLeaf;		// TRUE : 차일드노드가 없다.( m_pParts를 검사한다 ) FALSE : 차일드노드가 있다
	i3List	m_Parts;		// 해당 노드에 들어가는 폴리곤리스트 (인덱스값)
							// 컬리젼 박스 리스트( Geometry )

public:
	INT32	m_nCount;
	INT32	m_nDivideCount;

	BOOL	IsLeaf( void )					{ return m_bLeaf; }
	void	setLeaf( BOOL bValue )			{ m_bLeaf = bValue; }
	
	VEC4D *	getPlane( void )				{ return &m_Plane; }
	void	setPlane( VEC4D * pVec )		{ i3Vector::Copy( &m_Plane, pVec ); }
	void	setPlane( REAL32 x, REAL32 y, REAL32 z, REAL32 w )	{ i3Vector::Set( &m_Plane, x, y, z, w ); }

	INT32	getPartCount( void )			{ return m_Parts.GetCount(); }
	void	setPartCount( INT32 nCount )	{ m_Parts.SetCount( nCount ); }

	void *	getPart( INT32 idx )			{ return m_Parts.Items[ idx ]; }
	void	setPart( INT32 idx, void * value )	{ m_Parts.SetItem( idx, value ); }
	void	addPart( void * part )			{ m_Parts.Add( part ); }
	void	removePart( void * part )		{ m_Parts.Remove( part ); }

public:
	i3BSPNode(void);
	virtual ~ i3BSPNode(void);

	void	Create( void );
	void	ReleaseParts( void );

	i3List * CollisionTest( VEC3D * pVec );
	
	virtual UINT32 OnSave( i3ResourceFile * pResFile );
	virtual UINT32 OnLoad( i3ResourceFile * pResFile );
	
};


#endif	// __I3_BSP_NODE_H
