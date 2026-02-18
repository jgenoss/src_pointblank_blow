#pragma once


// CSceneGraphTreeCtrl

class CSceneGraphTreeCtrl : public i3TDKSceneGraphTreeCtrl
{
	DECLARE_DYNAMIC(CSceneGraphTreeCtrl)

protected:
	i3Node *		m_pLinkNode;

	i3Node *		m_pAddPoint;

public:
	CSceneGraphTreeCtrl();
	virtual ~CSceneGraphTreeCtrl();

	void	BuildRenderableColliderMesh(void);

	i3Node *		getAddPoint(void)					{ return m_pAddPoint; }

	virtual void OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew);
	virtual void OnSetSceneGraph(void);

protected:
	DECLARE_MESSAGE_MAP()
};


