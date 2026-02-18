#pragma once
#include "resource.h"

// Panel_Spec dialog
class i3TDKSpec_Body;

class Panel_Spec : public i3TDKSpecDialogContainer
{
	DECLARE_DYNAMIC(Panel_Spec)
public:
	Panel_Spec(CWnd* pParent = NULL);   // standard constructor
	virtual ~Panel_Spec();

// Dialog Data
	enum { IDD = IDD_PANEL_SPEC };

protected:
	i3GameObjPropertyDialog* m_pSpec_Body;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	i3GameObjPropertyDialog* GetBody(){ return m_pSpec_Body; }
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
