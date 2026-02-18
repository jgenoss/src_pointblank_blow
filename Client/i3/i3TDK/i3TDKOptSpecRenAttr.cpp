
#include "stdafx.h"
#include "i3TDK.h"
#include "i3Opt/i3OptRenAttr.h"

#include "i3TDKOptSpecRenAttr.h"


IMPLEMENT_DYNAMIC(i3TDKOptSpecRenAttr, i3TDKOptSpecBase)

BEGIN_MESSAGE_MAP(i3TDKOptSpecRenAttr, i3TDKOptSpecBase)
END_MESSAGE_MAP()

void i3TDKOptSpecRenAttr::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
}

BOOL i3TDKOptSpecRenAttr::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if(m_pOpt != nullptr)
	{
//		char conv[256];
//		i3OptRenAttr * pOpt = (i3OptRenAttr *) m_pOpt;	
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecRenAttr::OnOK()
{
	if(m_pOpt != nullptr)
	{
//		i3OptRenAttr * pOpt = (i3OptRenAttr *) m_pOpt;
	}

	i3TDKOptSpecBase::OnOK();
}
