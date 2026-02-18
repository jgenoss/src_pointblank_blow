
#ifndef _I3TDKOPTSPECRENATTR_H_
#define _I3TDKOPTSPECRENATTR_H_

#include "afxwin.h"


class i3TDKOptSpecRenAttr : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecRenAttr)

public:
	i3TDKOptSpecRenAttr(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecRenAttr::IDD, pParent) {}
	virtual ~i3TDKOptSpecRenAttr() {}

	enum { IDD = IDD_OPTSPEC_RENATTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();

};


#endif
