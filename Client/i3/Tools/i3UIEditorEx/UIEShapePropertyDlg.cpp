// UIEShapePropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "UIEShapePropertyDlg.h"


// CUIEShapePropertyDlg dialog

IMPLEMENT_DYNAMIC(CUIEShapePropertyDlg, CDialog)

CUIEShapePropertyDlg::CUIEShapePropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUIEShapePropertyDlg::IDD, pParent)
{
	m_pInfo = NULL;

	m_X = m_Y			=	0.0f;
	m_Width				=	100.0f;
	m_Height			=	100.0f;
	m_shapeCount		=	1;

	m_bOnlyRectMode		= false;
}

CUIEShapePropertyDlg::~CUIEShapePropertyDlg()
{
}

void CUIEShapePropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHAPEINFO_STRING, m_edInfoString);
	DDX_Control(pDX, IDC_EDIT_SHAPEINFO_WIDTH, m_edWidth);
	DDX_Control(pDX, IDC_EDIT_SHAPEINFO_HEIGHT, m_edHeight);
	DDX_Control(pDX, IDC_EDIT_SHAPEINFO_POSX, m_edPosX);
	DDX_Control(pDX, IDC_EDIT_SHAPEINFO_POSY, m_edPosY);
	DDX_Control(pDX, IDC_EDIT_SHAPE_COUNT,	  m_edCount);
}


BEGIN_MESSAGE_MAP(CUIEShapePropertyDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUIEShapePropertyDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUIEShapePropertyDlg::OnBnClickedCancel)
	ON_EN_KILLFOCUS(IDC_EDIT_SHAPEINFO_STRING, &CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoString)
	ON_EN_KILLFOCUS(IDC_EDIT_SHAPEINFO_WIDTH, &CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_SHAPEINFO_HEIGHT, &CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_SHAPEINFO_POSX, &CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoPosx)
	ON_EN_KILLFOCUS(IDC_EDIT_SHAPEINFO_POSY, &CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoPosy)
	ON_EN_KILLFOCUS(IDC_EDIT_SHAPE_COUNT, &CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoPosy)
END_MESSAGE_MAP()


// CUIEShapePropertyDlg message handlers
void CUIEShapePropertyDlg::_UpdateInfo(void)
{
	if( m_pInfo != NULL)
	{

		m_strInfoString = m_pInfo->m_strInfoString;
		
		m_edInfoString.SetWindowText(m_strInfoString.c_str());
		if( m_bOnlyRectMode)
		{
			m_edInfoString.EnableWindow( false);
		}

		m_X			= m_pInfo->m_pShape->getPosX();
		m_Y			= m_pInfo->m_pShape->getPosY();
		m_Width		= m_pInfo->m_pShape->getWidth();
		m_Height	= m_pInfo->m_pShape->getHeight();
	}

	char conv[MAX_PATH];
	
	i3::snprintf( conv, MAX_PATH, "%f", m_X);
	m_edPosX.SetWindowText((LPCTSTR)conv);

	i3::snprintf( conv, MAX_PATH, "%f", m_Y);
	m_edPosY.SetWindowText((LPCTSTR)conv);

	i3::snprintf( conv, MAX_PATH, "%d", 1);
	m_edCount.SetWindowText((LPCTSTR)conv);

	i3::snprintf( conv, MAX_PATH -1, "%f", m_Width);
	m_edWidth.SetWindowText((LPCTSTR)conv);

	i3::snprintf( conv, MAX_PATH -1, "%f", m_Height);
	m_edHeight.SetWindowText((LPCTSTR)conv);
}

bool CUIEShapePropertyDlg::Execute( REAL32 width, REAL32 height)
{
	m_Width = width;
	m_Height = height;

	if( DoModal() == IDOK)
		return true;

	return false;
}

void CUIEShapePropertyDlg::GetShapeInfo( I3UI_SHAPE_INFO * pInfo)
{
	if( pInfo == NULL)
		return;

	pInfo->m_strInfoString = m_strInfoString;

	pInfo->m_pShape->setPosX( m_X);
	pInfo->m_pShape->setPosY( m_Y);
	pInfo->m_pShape->setWidth( m_Width);
	pInfo->m_pShape->setHeight( m_Height);
}

void CUIEShapePropertyDlg::OnBnClickedOk()
{
	OnEnKillfocusEditShapeinfoString();
	OnEnKillfocusEditShapeinfoWidth();
	OnEnKillfocusEditShapeinfoHeight();
	OnEnKillfocusEditShapeinfoPosx();
	OnEnKillfocusEditShapeinfoPosy();
	OnEnKillfocusEditShapeCount();

	if( m_pInfo != NULL)
		GetShapeInfo( m_pInfo);

	OnOK();
}

void CUIEShapePropertyDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CUIEShapePropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_UpdateInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoString()
{
	// TODO: Add your control notification handler code here
	if( m_edInfoString.GetModify())
	{
		char conv[MAX_PATH];
		m_edInfoString.GetWindowText(conv, MAX_PATH -1);
		
		m_strInfoString = conv;

	}
}

void CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoWidth()
{
	// TODO: Add your control notification handler code here
	if( m_edWidth.GetModify())
	{
		char conv[MAX_PATH];

		m_edWidth.GetWindowText(conv, MAX_PATH -1);

		m_Width = (REAL32)atof( conv);
	}
}

void CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoHeight()
{
	// TODO: Add your control notification handler code here
	if( m_edHeight.GetModify())
	{
		char conv[MAX_PATH];

		m_edHeight.GetWindowText(conv, MAX_PATH -1);
		
		m_Height = (REAL32)atof( conv);
	}
}

void CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoPosx()
{
	// TODO: Add your control notification handler code here
	if( m_edPosX.GetModify())
	{
		char conv[MAX_PATH];

		m_edPosX.GetWindowText(conv, MAX_PATH -1);
		
		m_X = (REAL32)atof( conv);
	}
}

void CUIEShapePropertyDlg::OnEnKillfocusEditShapeinfoPosy()
{
	// TODO: Add your control notification handler code here
	if( m_edPosY.GetModify())
	{
		char conv[MAX_PATH];

		m_edPosY.GetWindowText(conv, MAX_PATH -1);
		
		m_Y = (REAL32)atof( conv);
	}
}

void CUIEShapePropertyDlg::OnEnKillfocusEditShapeCount()
{
	if( m_edCount.GetModify())
	{
		char conv[MAX_PATH];

		m_edCount.GetWindowText(conv, MAX_PATH -1);

		m_shapeCount = (INT32)atoi( conv);
	}
}