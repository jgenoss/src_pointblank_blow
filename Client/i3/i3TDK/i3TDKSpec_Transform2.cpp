// i3TDKSpec_Transform2.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Transform2.h"
#include "i3TDKSpec_ShapeSet.h"

// i3TDKSpec_Transform2 dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Transform2, i3TDKDialogBase)

i3TDKSpec_Transform2::~i3TDKSpec_Transform2()
{
	I3_SAFE_RELEASE( m_pTrans);
}

void i3TDKSpec_Transform2::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

void i3TDKSpec_Transform2::SetObject( i3ElementBase * pObj)
{
	I3_REF_CHANGE( m_pTrans,  (i3Transform2 *) pObj);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_Transform2, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_TDK_BTN_SHAPESET, &i3TDKSpec_Transform2::OnBnClickedTdkBtnShapeset)
END_MESSAGE_MAP()


// i3TDKSpec_Transform2 message handlers

void i3TDKSpec_Transform2::OnOK()
{
}

void i3TDKSpec_Transform2::OnCancel()
{
}

void i3TDKSpec_Transform2::OnBnClickedTdkBtnShapeset()
{
	if( m_pTrans == nullptr)
		return;

	i3TDKSpec_ShapeSet dlg;

	if( m_pTrans->getShapeSet() != nullptr)
	{
		dlg.Execute( m_pTrans->getShapeSet());
	}
	else
	{
		AfxMessageBox( _T("There is no shape set information on the node.") );
	}
}
