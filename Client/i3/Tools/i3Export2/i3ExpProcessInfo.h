#if !defined( __I3EXP_PROCESS_INFO_H)
#define __I3EXP_PROCESS_INFO_H

#define		EPI_STATE_EXCLUDED		0x00000001
#define		EPI_STATE_LINKED		0x00000002

class i3ExpProcessInfo
{
protected:
	UINT32			m_State;
	INode *			m_pParentINode;
	INode *			m_pINode;
	
	i3PointerStack	m_NodeStack;
	INT32			m_Bookmark;
	i3Node *		m_pi3Node;

	i3Node *		m_pAttachNode;

public:
	i3ExpProcessInfo(void);
	virtual ~i3ExpProcessInfo(void);

	void			setState( UINT32 state)		{ m_State = state; }
	UINT32			getState(void)				{ return m_State; }
	void			addState( UINT32 mask)		{ m_State |= mask; }
	void			removeState( UINT32 mask)	{ m_State &= ~mask; }
	bool			isState( UINT32 mask)		{ return (m_State & mask) == mask; }

	bool			isExcluded(void)			{ return isState(EPI_STATE_EXCLUDED); }
	void			setExcluded( bool bFlag)
	{ 
		if( bFlag)
			addState( EPI_STATE_EXCLUDED);
		else
			removeState( EPI_STATE_EXCLUDED);
	}

	bool			isLinked(void)			{ return isState(EPI_STATE_LINKED); }
	void			setLinked( bool bFlag)
	{ 
		if( bFlag)
			addState( EPI_STATE_LINKED);
		else
			removeState( EPI_STATE_LINKED);
	}


	INode *			getParentINode(void)		{ return m_pParentINode; }
	void			setParentINode( INode * pParent)
	{
		m_pParentINode = pParent;
	}

	INode *			getINode(void)				{ return m_pINode; }
	void			setINode( INode * pNode)	{ m_pINode = pNode; }

	i3Node *		geti3Node(void)				{ return m_pi3Node; }
	void			pushi3Node( i3Node * pNode, bool bToHead = false);
	void			popi3Node(void);
	void			replaceTop( i3Node * pNode);
	i3Node *		getTop(void);

	i3Node *		getAttachNode(void)				{ return m_pAttachNode; }
	void			setAttachNode( i3Node * pNode)	{ m_pAttachNode = pNode; }

	INT32			getBookmark(void)			{ return m_NodeStack.getCount(); }
	void			setBookmark( INT32 idx);

	void			pushAttr( i3RenderAttr * pAttr);
	i3RenderAttr *	findLastAttr( i3ClassMeta * pMeta);
};

#endif
