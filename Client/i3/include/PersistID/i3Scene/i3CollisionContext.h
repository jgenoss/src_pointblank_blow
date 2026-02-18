// i3CollisionContext.h

#if !defined( __I3_COLLISION_ENGINE_H)
#define __I3_COLLISION_ENGINE_H

#include "i3Node.h"
#include "i3ColliderSet.h"

typedef struct _tagi3CollisionUnit
{
	UINT32			m_State;
	UINT32			m_Style;
	MATRIX			m_Matrix;
	i3ColliderSet *	m_pNode;
} I3_COLLISION_UNIT;

#define		I3_COLLISION_GRP_STYLE_DO_TEST		0x00000001
#define		I3_COLLISION_GRP_STYLE_NOTIFY		0x00000002
#define		I3_COLLISION_GRP_STYLE_WITH_WORLD	0x00000004


class I3_EXPORT_SCENE i3CollisionContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3CollisionContext);

protected:
	I3_COLLISION_UNIT *		m_pColList;
	//INT32					m_ColListIdx; 
	
	UINT32					m_ColMax;
	UINT32					m_ColCount;
	i3Node *				m_pWorldRoot;
	i3List					m_GroupList;

	i3ColliderSet *			m_pCollider;

	#if defined( I3_DEBUG)
	UINT32					m_ExceedCount;
	#endif

//protected:
public:
	UINT32		_Calc( I3_COLLISION_UNIT * pUnit, i3Collider * pCollider, I3_COLLISION_RESULT_CONTEXT * pCtx);
	UINT32		_CalcWithWorld( i3Collidee * pCol, i3Node * pNode, I3_COLLISION_RESULT_CONTEXT * pCtx, UINT32 mask);

public:
	i3CollisionContext(void);
	virtual ~i3CollisionContext(void);

	BOOL	Create( INT32 numUnit, INT32 numResult);

	void	Add( MATRIX * pMatrix, i3ColliderSet * pCollSet);

	//void	AddGroup( const char * pszGroupName, UINT32 GroupStyle, UINT32	SrcMask, UINT32 TargetMask);
	void	AddGroup( UINT32 GroupStyle, UINT32	SrcMask, UINT32 TargetMask);

	INT32	FindCollisionGroupIndexByName( const char * pszGroupName);
	void	RemoveAllGroup(void);

	void	AddWorld( i3Node * pNode);
	void	RemoveWorld( i3Node * pNode);
	void	RemoveAllWorld(void);

	I3_COLLISION_RESULT *	GetResult(void);
	void					reserveResult( UINT32 cnt);
	void					resetResult(void);
	//I3_COLLISION_RESULT *	GetCurrentResult(void);

	void	Reset(void);
	void	SoftReset(void);

	void	Calculate(void);
	UINT32	CollisionTest( i3Collidee * pCollidee, UINT32 GroupIdx, UINT32 Style, i3EventReceiver * pEventRecv = NULL);
	void	CollisionTest(void);
};

#endif
