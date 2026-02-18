// EditDialog.cpp : implementation file
//

#include "stdafx.h"

#include "ListSubDialog.h"


// CListSubDialog dialog

IMPLEMENT_DYNAMIC(CListSubDialog, CDialog)

CListSubDialog::CListSubDialog(int nItem, int nSubItem, CWnd* pParent /*=NULL*/)
	: CDialog(CListSubDialog::IDD, pParent), m_nItem(nItem), m_nSubItem(nSubItem)
{
	m_pParent = pParent;
}
CListSubDialog::CListSubDialog(CWnd* pParent /*=NULL*/): 
								CDialog(CListSubDialog::IDD, pParent)
{
	m_pParent = pParent;
	SetTitle( "Width", "Width" );
	InitDialogText( "Width:, ", ", 0" );
}

CListSubDialog::~CListSubDialog()
{
}

void CListSubDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC1, m_StaticCtrl[0]);
	DDX_Control(pDX, IDC_STATIC2, m_StaticCtrl[1]);
	DDX_Control(pDX, IDC_STATIC3, m_StaticCtrl[2]);
	DDX_Control(pDX, IDC_STATIC4, m_StaticCtrl[3]);

	DDX_Control(pDX, IDC_EDIT1, m_EditCtrl[0]);
	DDX_Control(pDX, IDC_EDIT2, m_EditCtrl[1]);
	DDX_Control(pDX, IDC_EDIT3, m_EditCtrl[2]);
	DDX_Control(pDX, IDC_EDIT4, m_EditCtrl[3]);
}


BEGIN_MESSAGE_MAP(CListSubDialog, CDialog)
	ON_WM_CREATE()   
	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_BUTTON1, &CListSubDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CListSubDialog::OnBnClickedCancel)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CListSubDialog message handlers
BOOL CListSubDialog::PreTranslateMessage(MSG* pMsg)
{   
	if(pMsg->message == WM_KEYDOWN)   
	{   
		if(pMsg->wParam == VK_RETURN)   
		{   
			return TRUE;
		}   
	}   

	return CDialog::PreTranslateMessage(pMsg);   
}   

int CListSubDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)   
{   
	if (CDialog::OnCreate(lpCreateStruct) == -1)   
		return -1;   

	CFont* pFont = GetParent()->GetFont();   
	SetFont(pFont);   

	SetFocus();   

	return 0;   
}   

BOOL CListSubDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	HideEditCtrl();

	CString resTokenName, resTokenValue;
	int curPosName = 0, curPosValue = 0;
	int Index = 0;

	resTokenName= m_NameString.Tokenize(m_SeparationString, curPosName);
	resTokenValue= m_ValueString.Tokenize(m_SeparationString, curPosValue);
	while (resTokenName != _T("") && resTokenValue != _T(""))
	{
		SetTextEditCtrl( Index, resTokenName, resTokenValue );

		++Index;

		resTokenName= m_NameString.Tokenize(m_SeparationString, curPosName);
		resTokenValue= m_ValueString.Tokenize(m_SeparationString, curPosValue);
	};

	this->SetWindowText( m_TitleString );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CListSubDialog::HideEditCtrl()
{
	for(int i = 0; i < MAX_CONTROL; ++i)
	{
		m_StaticCtrl[ i ].ShowWindow( SW_HIDE );
		m_EditCtrl[ i ].ShowWindow( SW_HIDE );
	}
}

void CListSubDialog::SetTextEditCtrl( int Index, LPCTSTR name, LPCTSTR value )
{
	m_StaticCtrl[ Index ].SetWindowText(name);
	m_EditCtrl[ Index ].SetWindowText(value);
	
	m_StaticCtrl[ Index ].ShowWindow( SW_SHOW );
	m_EditCtrl[ Index ].ShowWindow(SW_SHOW );
}

void CListSubDialog::InitDialogText(LPCTSTR Name, LPCTSTR Value, LPCTSTR Separation)
{
	m_NameString = Name;
	m_ValueString = Value;
	m_SeparationString = Separation;
}

CString CListSubDialog::GetTextEdit()
{
	return m_ReturnString;
}

void CListSubDialog::SetTitle(LPCTSTR Nation, LPCTSTR Id)
{
	m_TitleString.Format( _T("%s - %s"), Nation, Id );
}

void CListSubDialog::OnBnClickedOk()
{
	m_ReturnString = _T("");
	for(int i = 0; i < MAX_CONTROL; ++i)
	{
		if( m_EditCtrl[ i ].IsWindowVisible() )
		{
			CString Text;
			m_EditCtrl[ i ].GetWindowText( Text );

			m_ReturnString.Append( Text );
			m_ReturnString.Append( m_SeparationString );
		}
	}

	int Size = m_ReturnString.GetLength();
	int Index = m_ReturnString.ReverseFind( m_SeparationString[0] );

	m_ReturnString.Delete( Index, Size - Index );

	OnOK();
}

void CListSubDialog::OnBnClickedCancel()
{
	OnCancel();
}
void CListSubDialog::OnDestroy()
{
	CDialog::OnDestroy();

	m_pParent->SetFocus();
}
