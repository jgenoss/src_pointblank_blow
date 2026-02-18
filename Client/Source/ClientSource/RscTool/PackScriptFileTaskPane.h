#pragma once


// PackScriptFileTaskPane
#include "PackScriptContents.h"

#include "i3MFCListCtrl.h"
#include "i3MFCListCtrl_Custom.h"
#include "i3MFCVSListBox.h"


class PackScriptFileTaskPane : public CMFCTasksPane
{
	DECLARE_DYNAMIC(PackScriptFileTaskPane)

public:
	PackScriptFileTaskPane();
	virtual ~PackScriptFileTaskPane();

	void	LoadPackScriptFile(const i3::wstring& strFullPath);

	void	OnDragEndFromExplorer(CWnd* targetWnd, const CPoint& ptScreen, 
		const i3::vector<i3::wstring>& fullPathList);					
	void	OnDragMoveFromExplorer(CWnd* targetWnd, const CPoint& ptScreen);		

	// Internal..
	void		UpdatePackScriptUnit(PackScriptUnit* ItemData);
	void		RemoveAddRemoveEncGroup();
	
	PackScriptContents&		GetPackScriptContents() { return m_contents; }
	PackScriptUnit*			GetCurrentPackScriptUnit() const;
	PackScriptUnit*			GetCopyScriptUnit() const;

	bool		IsContentsModified() const;

	void DeleteItemFromListCtrl(i3MFCListCtrl* pCtrl, int target_idx);		
private:

	i3MFCListCtrl_Custom	m_listctrl_add;
	i3MFCListCtrl			m_listctrl_rem;
	i3MFCListCtrl			m_listctrl_enc;
	
	i3MFCVSListBox	m_vslistctrl_pack;
	
	PackScriptContents	m_contents;			// 우선 값으로 처리하고, 필요시 동적할당으로 변경..
	PackScriptContents	m_contents_orig;	//

	int			m_grp_idx_add, m_grp_idx_rem, m_grp_idx_enc;	// 그룹인덱스..
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	void OnContextMenu_Result(LONG result_cmd, i3MFCListCtrl* pCtrl, int target_idx);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


