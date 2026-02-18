#if !defined( __I3OPT_BUILD_OCTREE_H_)
#define __I3OPT_BUILD_OCTREE_H_

#include "i3SceneOptimizer.h"


class I3_EXPORT_OPT i3OptBuildOcTree : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildOcTree, i3SceneOptimizer);

	friend class i3OptOctreeNode;

public:
	enum CENTER_METHOD
	{
		CFM_MAX_SPAN = 0,
		CFM_FIXED,
	};

protected:
	i3OptOctreeNode *	m_pTreeRoot = nullptr;

protected:
	// for progress report
	INT32			m_MaxProgress = 0;
	INT32			m_CurProgress = 0;
	char			m_szProgress[512] = { 0 };

	bool			m_bSplit = false;
	bool			m_bLimitDepth = false;
	INT32			m_MaxDepth = 10;
	bool			m_bLimitTriCount = false;
	INT32			m_MinTriCount = 2000;
	CENTER_METHOD	m_CenterFindMethod = CFM_FIXED;

	REAL32			m_CellSize = 10.0f;
	REAL32			m_K = 2.0f;
	REAL32			m_EPS = 0.009f;

protected:
	void			_ProgressSet( INT32 maxCount);
	void			_ProgressInc( const char * pszStr);
	void			_ProgressMsg( const char * pszStr);
	void			_ProgressEnd(void);

public:
	i3OptBuildOcTree( void);
	virtual ~i3OptBuildOcTree(void);

	CENTER_METHOD	getMethodOfCenter(void)				{ return m_CenterFindMethod; }
	void			setMethodOfCenter( CENTER_METHOD m)	{ m_CenterFindMethod = m; }

	bool			getSplitState(void)					{ return m_bSplit; }
	void			setSplitState( bool bFlag)			{ m_bSplit = bFlag; }

	bool			getLimitDepthState(void)			{ return m_bLimitDepth; }
	void			setLimitDepthState( bool bFlag)		{ m_bLimitDepth = bFlag; }

	INT32			getMaxDepth(void)					{ return m_MaxDepth; }
	void			setMaxDepth( INT32 depth)			{ m_MaxDepth = depth; }

	bool			getLimitTriangleCountState(void)	{ return m_bLimitTriCount; }
	void			setLimitTriangleCountState( bool bFlag)
	{
		m_bLimitTriCount = bFlag;
	}

	INT32			getMinTriangleCount(void)			{ return m_MinTriCount; }
	void			setMinTriangleCount( INT32 cnt)		{ m_MinTriCount = cnt; }

	REAL32			getCellSize(void)					{ return m_CellSize; }
	void			setCellSize( REAL32 val)			{ m_CellSize = val; }

	REAL32			getEPS(void)						{ return m_EPS; }
	void			setEPS( REAL32 val)					{ m_EPS = val; }

	REAL32			getExtent(void)						{ return m_K; }
	void			setExtent( REAL32 k)				{ m_K = k; }


	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void Trace( i3Node * pRoot) override;
	virtual bool		isSupportProgress(void) override;

	virtual const char *	getDesc(void) override;
};

#endif //__I3OPT_BUILD_OCTREE_H_