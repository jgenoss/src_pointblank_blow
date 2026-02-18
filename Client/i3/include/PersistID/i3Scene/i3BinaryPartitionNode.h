#if !defined( __I3_BSP_H)
#define __I3_BSP_H

#include "i3Node.h"

class I3_EXPORT_SCENE i3BinaryPartitionNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3BinaryPartitionNode);
protected:
	VEC4D		m_Plane;

	i3BinaryPartitionNode	*	m_pFront;
	i3BinaryPartitionNode	*	m_pBack;

protected:
	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3BinaryPartitionNode(void);
	virtual ~i3BinaryPartitionNode(void);

	bool		isLeaf(void)						{ return GetChildCount() == 0; }

	VEC4D *		getPlane(void)						{ return & m_Plane; }
	void		setPlane( VEC4D * pPlane)			{ i3Vector::Copy( &m_Plane, pPlane); }

	i3BinaryPartitionNode *	getFront(void)			{ return m_pFront; }
	void					setFront( i3BinaryPartitionNode * pFront)
	{
		I3_REF_CHANGE( m_pFront, pFront);
	}

	i3BinaryPartitionNode *	getBack(void)			{ return m_pBack; }
	void					setBack( i3BinaryPartitionNode * pBack)
	{
		I3_REF_CHANGE( m_pBack, pBack);
	}

	virtual		void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual		void	OnBuildObjectList( i3List * pList);
	virtual		UINT32	OnSave( i3ResourceFile * pResFile);
	virtual		UINT32	OnLoad( i3ResourceFile * pResFile);	
};

#endif
