#if !defined( __I3_TREE_NODE_H)
#define __I3_TREE_NODE_H

#include "i3NamedElement.h"

class i3TreeNode;

typedef BOOL (* I3_TREE_TRAVERSE_PROC)( INT32 depth, i3TreeNode * pNode, void * pUserData);

#define	I3_TREENODE_STYLE_VOLATILE		0x00000001

#define	I3_TREENODE_STATE_DISABLE		0x00000001

class I3_EXPORT_BASE i3TreeNode : public i3NamedElement
{
	I3_CLASS_DEFINE( i3TreeNode);
protected:
	UINT32				m_NodeStyle;
	UINT32				m_NodeState;
	i3List				m_ChildList;
	i3TreeNode *		m_pParent;

public:
	i3TreeNode(void);
	virtual ~i3TreeNode(void);

	UINT32				getNodeStyle(void)				{ return m_NodeStyle; }
	void				setNodeStyle( UINT32 style)		{ m_NodeStyle = style; }
	void				addNodeStyle( UINT32 mask)		{ setNodeStyle( getNodeStyle() | mask); }
	void				removeNodeStyle( UINT32 mask)	{ setNodeStyle( getNodeStyle() & ~mask); }
	bool				isNodeStyle( UINT32 mask)		{ return (getNodeStyle() & mask) == mask; }

	UINT32				getNodeState(void)				{ return m_NodeState; }
	void				setNodeState( UINT32 state)		{ m_NodeState = state; }
	void				addNodeState( UINT32 mask)		{ setNodeState( getNodeState() | mask); }
	void				removeNodeState( UINT32 mask)	{ setNodeState( getNodeState() & ~mask); }
	bool				isNodeState( UINT32 mask)		{ return (getNodeState() & mask) == mask; }

	INT32				getChildCount(void)							{ return m_ChildList.GetCount(); }
	i3TreeNode *		getChild( INT32 idx)						{ return (i3TreeNode *) m_ChildList.Items[idx]; }
	i3List *			getChildList(void)							{ return & m_ChildList; }
	void				setChild( INT32 idx, i3TreeNode * pNode)	{ m_ChildList.Items[idx] = pNode; }
	i3TreeNode *		FindChildByName( const char * pszName);

	void				AddChild( i3TreeNode * pNode);
	void				InsertChild( INT32 idx, i3TreeNode * pNode);
	void				RemoveChild( i3TreeNode * pNode);
	void				RemoveAllChild(void);
	void				RemoveFromParent(void);
	void				DetachChild( i3TreeNode * pNode);
	BOOL				IsChild( i3TreeNode * pNode);

	i3TreeNode *		getParent(void)								{ return m_pParent; }
	void				SetParent( i3TreeNode * pParent);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	static i3TreeNode *	FindNodeByType( i3TreeNode * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);
	static void			FindNodeByType( i3TreeNode * pRoot, i3ClassMeta * pMeta, i3List * pList, bool bRecursive = true);

	static i3TreeNode *	FindNodeByExactType( i3TreeNode * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);
	static void			FindNodeByExactType( i3TreeNode * pRoot, i3ClassMeta * pMeta, i3List * pList, bool bRecursive = true);

	static i3TreeNode *	FindNodeByName( i3TreeNode * pRoot, const char * pszName, bool bRecursive = true);
	static void			FindNodeByName( i3TreeNode * pRoot, const char * pszName, i3List * pList, bool bRecursive = true);

	static i3TreeNode *	CreateClone( i3TreeNode * pRoot, I3_COPY_METHOD method);
	static bool			Traverse( i3TreeNode * pRoot, I3_TREE_TRAVERSE_PROC pProc, void * pUserData);
};

#endif
