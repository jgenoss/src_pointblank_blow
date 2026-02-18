#if !defined( __I3_LOD_H)
#define __I3_LOD_H

#include "i3Skeleton.h"
#include "i3Node.h"

class I3_EXPORT_SCENE i3LOD : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3LOD);
protected:
	i3Skeleton *		m_pSkel;
	i3Node *			m_pShapeNode;
	i3MemoryBuffer *	m_pBoneMap;

	// юс╫ц
	i3Skeleton *		m_pSharedSkel;
public:
	i3LOD(void);
	virtual ~i3LOD(void);

	i3Skeleton *		getSkeleton(void)					{ return m_pSkel; }
	void				setSkeleton( i3Skeleton * pSkel)	{ I3_REF_CHANGE( m_pSkel, pSkel); }

	UINT32				getBoneCount(void)					{ return m_pSkel->getBoneCount(); }

	i3Node *			getShapeNode(void)					{ return m_pShapeNode; }
	void				setShapeNode( i3Node * pNode)		{ I3_REF_CHANGE( m_pShapeNode, pNode); }

	INT32 *				getBoneMap(void)					
	{ 
		if(m_pBoneMap == NULL) 
			return NULL;
		
		return (INT32 *) m_pBoneMap->getBuffer(); 
	}

	void				setSharedSkeleton( i3Skeleton * pSkel)	{ m_pSharedSkel = pSkel; }

	void				setBoneMap( i3MemoryBuffer * pBuf)	{ I3_REF_CHANGE( m_pBoneMap, pBuf); }

	// pTable can be NULL. In this case, the function just allocates memory for a room
	void				SetBoneMapTable( INT32 * pTable, UINT32 Count);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	void			OnInstancing( void * pUserDefault = NULL);
};

#endif
