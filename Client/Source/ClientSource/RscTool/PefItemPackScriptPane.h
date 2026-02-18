#pragma once


// PefItemPackScriptPane
#include "i3MFCListCtrl.h"
#include "PackMode.h"

typedef i3::shared_ptr<struct AssocFileSet>	AssocFileSetPtr;
struct AssocFile;
class PackDlg;

class PefItemPackScriptPane : public CDockablePane
{
	DECLARE_DYNAMIC(PefItemPackScriptPane)

public:
	PefItemPackScriptPane();
	virtual ~PefItemPackScriptPane();

	void					SetAssocFileSet(const AssocFileSetPtr& ptr);
	const AssocFileSetPtr&	GetAssocFileSet() const { return m_AssocFileSet; }
	
	void SelectAssocResultFiles(const i3::vector<const AssocFile*>&  file_list);

	void				DoPackScriptJobWithSelectedItems(PackMode mode);


private:
	AssocFileSetPtr	m_AssocFileSet;
	i3MFCListCtrl	m_listCtrl;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

};


