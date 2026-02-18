#if !defined( I3OPT_COMPACT_MESH_H)
#define I3OPT_COMPACT_MESH_H

#include "i3SceneOptimizer.h"
#include "i3OptGeometryBase.h"

class I3_EXPORT_OPT i3OptCompactMesh : public i3OptGeometryBase
{
	I3_EXPORT_CLASS_DEFINE( i3OptCompactMesh, i3OptGeometryBase);

protected:
	void	_SearchSameFormat( VAINFO * pStart, i3::vector<VAINFO*>& Group);
	void	_MergeGroup( i3::vector<VAINFO*>& Group);

	INT32	_MergeVA( i3VertexArray * pOldVA, i3VertexArray * pNewVA, INT32 newVACount, INT32 * pIndexTable);

	INT32	_FindVertex( INT32 idx, i3VertexArray * pOldVA, i3VertexArray * pNewVA, INT32 newVACount);

	void	_ReplaceVA( i3GeometryAttr * pAttr, VAINFO * pInfo);

public:
	i3OptCompactMesh(void);

public:
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;
};

#endif