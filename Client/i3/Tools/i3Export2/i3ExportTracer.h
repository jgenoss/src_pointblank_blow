#if !defined( __I3_EXPORT_TRACER_H)
#define __I3_EXPORT_TRACER_H

typedef bool (*INODE_TRAVERSE_PROC)( INT32 Level, INode * pNode, void * pUserData);

class i3ExportTracer
{
protected:
	INODE_TRAVERSE_PROC		m_pUserProc;
	void *					m_pUserData;

	INode *					m_pRoot;

	bool					m_bSelectedOnly;

protected:
	bool _Rec_Traverse( INT32 Level, INode * pNode);

public:
	i3ExportTracer(void);
	virtual ~i3ExportTracer(void);

	bool		getSelectedOnlyState(void)				{ return m_bSelectedOnly; }
	void		setSelectedOnlyState( bool bFlag)		{ m_bSelectedOnly = bFlag; }

	void	Trace( INode * pRoot, INODE_TRAVERSE_PROC pProc, void * pData);
};

#endif
