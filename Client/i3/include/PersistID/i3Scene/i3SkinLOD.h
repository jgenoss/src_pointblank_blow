#if !defined( __I3_SKIN_LOD_H)
#define __I3_SKIN_LOD_H

#include "i3Skeleton.h"
#include "i3Node.h"

class I3_EXPORT_SCENE i3SkinLOD : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3SkinLOD);
protected:
	i3Skeleton *		m_pSkel;
	i3Node *			m_pShapeNode;
	INT32 *				m_pBoneMap;

public:
	i3SkinLOD(void);
	virtual ~i3SkinLOD(void);

	i3Skeleton *		getSkeleton(void)					{ return m_pSkel; }
	void				setSkeleton( i3Skeleton * pSkel)	{ I3_REF_CHANGE( m_pSkel, pSkel); }

	UINT32				getBoneCount(void)					{ return m_pSkel->getBoneCount(); }

	i3Node *			getShapeNode(void)					{ return m_pShapeNode; }
	void				setShapeNode( i3Node * pNode)		{ I3_REF_CHANGE( m_pShapeNode, pNode); }

	INT32 *				getBoneMap(void)					{ return m_pBoneMap; }

	// pTable can be NULL. In this case, the function just allocates memory for a room
	void				SetBoneMap( INT32 * pTable, UINT32 Count);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
