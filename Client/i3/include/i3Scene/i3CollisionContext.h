// i3CollisionContext.h

#if !defined( __I3_COLLISION_ENGINE_H)
#define __I3_COLLISION_ENGINE_H

#include "i3Node.h"
#include "i3ColliderSet.h"

struct I3_COLLISION_UNIT
{
	UINT32			m_State = 0;
	UINT32			m_Style = 0;
	MATRIX			m_Matrix;
	i3ColliderSet *	m_pNode = nullptr;
} ;

#define		I3_COLLISION_GRP_STYLE_DO_TEST		0x00000001
#define		I3_COLLISION_GRP_STYLE_NOTIFY		0x00000002
#define		I3_COLLISION_GRP_STYLE_WITH_WORLD	0x00000004


class I3_EXPORT_SCENE i3CollisionContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3CollisionContext, i3ElementBase);

protected:
	I3_COLLISION_UNIT *		m_pColList = nullptr;
	
	UINT32					m_ColMax = 0;
	UINT32					m_ColCount = 0;
	i3Node *				m_pWorldRoot;
	i3::vector<struct I3_COLLISION_GROUP*>	m_GroupList;

	i3ColliderSet *			m_pCollider = nullptr;

	#if defined( I3_DEBUG)
	UINT32					m_ExceedCount = 0;
	#endif

//protected:
public:
	UINT32		_Calc( I3_COLLISION_UNIT * pUnit, i3Collider * pCollider, I3_COLLISION_RESULT_CONTEXT * pCtx);
	UINT32		_CalcWithWorld( i3Collidee * pCol, i3Node * pNode, I3_COLLISION_RESULT_CONTEXT * pCtx, UINT32 mask);

public:
	i3CollisionContext(void);
	virtual ~i3CollisionContext(void);

	bool	Create( INT32 numUnit, INT32 numResult);

	void	Add( MATRIX * pMatrix, i3ColliderSet * pCollSet);

	void	AddGroup( UINT32 GroupStyle, UINT32	SrcMask, UINT32 TargetMask);

	INT32	FindCollisionGroupIndexByName( const char * pszGroupName);
	void	RemoveAllGroup(void);

	void	AddWorld( i3Node * pNode);
	void	RemoveWorld( i3Node * pNode);
	void	RemoveAllWorld(void);

	I3_COLLISION_RESULT *	GetResult(void);
	void					reserveResult( UINT32 cnt);
	void					resetResult(void);

	void	Reset(void);
	void	SoftReset(void);

	void	Calculate(void);
	UINT32	CollisionTest( i3Collidee * pCollidee, UINT32 GroupIdx, UINT32 Style, i3EventReceiver * pEventRecv = nullptr);
	void	CollisionTest(void);
};

#endif
