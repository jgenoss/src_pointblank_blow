#pragma once


// CSceneGraphTreeCtrl

class CSceneGraphTreeCtrl : public i3TDKSceneGraphTreeCtrl
{
	DECLARE_DYNAMIC(CSceneGraphTreeCtrl)

protected:
public:
	CSceneGraphTreeCtrl();
	virtual ~CSceneGraphTreeCtrl();

	virtual void OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew) override;
	virtual void OnSetSceneGraph(void) override;

protected:
	DECLARE_MESSAGE_MAP()
};


