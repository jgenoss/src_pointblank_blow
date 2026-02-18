#pragma once



#include "i3MFCListCtrl.h"

struct PackScriptUnit;

class PackScriptRealFileListPane : public CDockablePane
{
	DECLARE_DYNAMIC(PackScriptRealFileListPane)

public:
	PackScriptRealFileListPane();
	virtual ~PackScriptRealFileListPane();

	void SetPackScriptUnit(PackScriptUnit* unit);
//	void UpdateUnitData();
	
	PackScriptUnit*			GetPackScriptUnit() const { return m_unit; }
	const i3::vector<i3::string>&	GetResultFileList() const { return m_resultFileList; }
	

protected:
	DECLARE_MESSAGE_MAP()

private:
	i3MFCListCtrl			m_listCtrl;
	PackScriptUnit*			m_unit;
	
	i3::vector<i3::string>	m_resultAddList;
	i3::vector<i3::string>	m_resultRemoveList;
	i3::vector<i3::string>	m_resultEncList;

	i3::vector<i3::string>	m_resultFileList;
	i3::vector<bool>		m_encMappingList;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


