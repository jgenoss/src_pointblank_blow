#pragma once
#include "afxwin.h"


// CModalObjectType dialog

class CModalObjectType : public CDialog
{
	DECLARE_DYNAMIC(CModalObjectType)

protected:
	INT32		m_Selected;

public:
	CModalObjectType(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModalObjectType();

	i3ClassMeta *		Execute(void);

// Dialog Data
	enum { IDD = IDD_MODAL_OBJECTTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ObjectTypeCtrl;
protected:
	virtual void OnOK() override;
};
