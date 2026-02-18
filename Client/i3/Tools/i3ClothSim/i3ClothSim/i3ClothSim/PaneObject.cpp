// PaneObject.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "PaneObject.h"
#include ".\paneobject.h"


// PaneObject dialog

IMPLEMENT_DYNAMIC(PaneObject, i3TDKDialogBase)
PaneObject::PaneObject(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(PaneObject::IDD, pParent)
{
}

PaneObject::~PaneObject()
{
}

void PaneObject::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_ED_OBJECTNAME, m_ObjectNameCtrl );
	DDX_Control( pDX, IDC_ED_TOTAL_VERTEX, m_TotalVertexCtrl );
	DDX_Control( pDX, IDC_ED_SIM_VERTEX_COUNT, m_SimVertexCtrl );
	DDX_Control( pDX, IDC_ED_TOTAL_SPRING, m_TotalSpringCtrl );

	DDX_Control( pDX, IDC_ED_TOTAL_MASS, m_TotalMassCtrl);
	DDX_Check( pDX, IDC_CHK_MASSDIFF, m_bMassDifferential );
}


BEGIN_MESSAGE_MAP(PaneObject, i3TDKDialogBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// PaneObject message handlers

int PaneObject::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText( "Cloth Info.");

	return 0;
}
