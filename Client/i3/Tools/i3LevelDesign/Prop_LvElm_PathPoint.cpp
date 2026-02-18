// Prop_LvElm_PathPoint.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_PathPoint.h"
#include ".\prop_lvelm_pathpoint.h"


// CProp_LvElm_PathPoint dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_PathPoint, i3TDKDialogBase)
CProp_LvElm_PathPoint::CProp_LvElm_PathPoint(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_PathPoint::IDD, pParent)
{
	m_pPoint = NULL;
}

CProp_LvElm_PathPoint::~CProp_LvElm_PathPoint()
{
}

void CProp_LvElm_PathPoint::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_PRIORITY, m_ED_Priority);
	DDX_Control(pDX, IDC_COLOR_PLACEHOLDER, m_ColorPlaceHolder);
}

void CProp_LvElm_PathPoint::SetObject( i3ElementBase * pObj)
{
	char conv[256];

	if( pObj != NULL)
	{
		I3ASSERT( i3::kind_of<i3LevelPathPoint*>(pObj)); //->IsTypeOf( i3LevelPathPoint::static_meta()));

		m_pPoint = (i3LevelPathPoint *) pObj;

		// Priority
		sprintf( conv, "%d", m_pPoint->getPriority());
		m_ED_Priority.SetWindowText( conv);

		// Color
		m_ColorPreviewCtrl.setColor( m_pPoint->getMaterialDiffuse());
	}
	else
	{
		m_pPoint = NULL;

		m_ED_Priority.SetWindowText( "");
	}
}

BEGIN_MESSAGE_MAP(CProp_LvElm_PathPoint, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_ED_PRIORITY, OnEnKillfocusEdPriority)
	ON_BN_CLICKED(IDC_BTN_SEL_COLOR, OnBnClickedBtnSelColor)
END_MESSAGE_MAP()


// CProp_LvElm_PathPoint message handlers

void CProp_LvElm_PathPoint::OnEnKillfocusEdPriority()
{
	char conv[256];

	if( m_pPoint == NULL)
		return;

	m_ED_Priority.GetWindowText( conv, sizeof(conv) - 1);

	m_pPoint->setPriority( atoi( conv));
}

void CProp_LvElm_PathPoint::OnBnClickedBtnSelColor()
{
	i3TDKColorPicker dlg;

	if( dlg.Execute( m_pPoint->getMaterialDiffuse()))
	{
		m_pPoint->SetMaterialDiffuse( dlg.getSelectedColor());
		m_ColorPreviewCtrl.setColor( dlg.getSelectedColor());
	}
}

BOOL CProp_LvElm_PathPoint::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		m_ColorPlaceHolder.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_ColorPreviewCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_PathPoint::OnOK()
{
}

void CProp_LvElm_PathPoint::OnCancel()
{
}
