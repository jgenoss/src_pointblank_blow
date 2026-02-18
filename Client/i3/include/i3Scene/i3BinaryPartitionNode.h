#if !defined( __I3_BSP_H)
#define __I3_BSP_H

#include "i3Node.h"

/** \brief
	\desc leaf class
	*/

class I3_EXPORT_SCENE i3BinaryPartitionNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3BinaryPartitionNode, i3Node);
protected:
	VEC4D		m_Plane = { 1.0f, 0.0f, 0.0f, 0.0f };

	i3BinaryPartitionNode	*	m_pFront = nullptr;
	i3BinaryPartitionNode	*	m_pBack = nullptr;

protected:
	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo);
	virtual void		CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled);
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

	void				GetPlaceMatrix( MATRIX * pMtx);

	virtual		void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual		void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual		UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual		UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
