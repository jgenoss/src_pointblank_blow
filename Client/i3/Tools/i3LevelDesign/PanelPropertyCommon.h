#pragma once

#include "Prop_LvElm_Common.h"

// CPanelPropertyCommon dialog

class CPanelPropertyCommon : public i3TDKDialogContainer
{
	DECLARE_DYNAMIC(CPanelPropertyCommon)
protected:
	CProp_LvElm_Common		m_Prop_Elm_Common;


public:
	CPanelPropertyCommon(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelPropertyCommon();

// Dialog Data
	enum { IDD = IDD_PROP_COMMON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
};
