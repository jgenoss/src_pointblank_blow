#if !defined( __I3_OPT_BSP_BUILDER_H)
#define __I3_OPT_BSP_BUILDER_H

#include "i3SceneOptimizer.h"
#include "i3OptBSPGeometry.h"
#include "i3OptBSPNode.h"

class I3_EXPORT_OPT i3OptBSPBuilder : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBSPBuilder);
protected:
	i3AttrStackManager * m_pAttrStack;
	i3List				m_GeoList;
	i3List				m_TransGeoList;

	I3OPT_BSP_CONTEXT	m_Ctx;

protected:
	void	_SearchAllGeometry( i3Node * pNode);
	void	_FlushDifferAttrs( i3OptBSPGeometry * pGeo);
	void	_RemoveAllGeo(void);
	void	_ConvertToTriList(void);
	bool	_isTransparentNode( i3Node * pNode);

public:
	i3OptBSPBuilder(void);
	virtual ~i3OptBSPBuilder(void);

	UINT32			getConditionFlag(void)				{ return m_Ctx.m_Condition; }
	void			addConditionFlag( UINT32 mask)		{ m_Ctx.m_Condition |= mask; }
	void			removeConditionFlag( UINT32 mask)	{ m_Ctx.m_Condition &= ~mask; }
	bool			isConditionFlag( UINT32 mask)		{ return (m_Ctx.m_Condition & mask) != 0; }

	INT32			getDepthLimit(void)					{ return m_Ctx.m_LimitDepth; }
	void			setDepthLimit( INT32 depth)			{ m_Ctx.m_LimitDepth = depth; }

	INT32			getMinPolygonCount(void)			{ return m_Ctx.m_LimitCount; }
	void			setMinPolygonCount( INT32 cnt)		{ m_Ctx.m_LimitCount = cnt; }

	SPLIT_MODE		getSplitMode(void)					{ return m_Ctx.m_Mode; }
	void			setSplitMode( SPLIT_MODE mode)		{ m_Ctx.m_Mode = mode; }

	bool			getSplitPolygonEnable(void)			{ return m_Ctx.m_bSplitPoly; }
	void			setSplitPolygonEnable( bool bFlag)	{ m_Ctx.m_bSplitPoly = bFlag; }

	virtual BOOL	OnPreNode( i3Node * pNode);
	virtual BOOL	OnPostNode( i3Node * pNode);
	virtual void	Optimize( i3SceneGraphInfo * pSgInfo);

};

#endif
