// Dlg_Find.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Dlg_Find.h"


// Dlg_Find dialog

IMPLEMENT_DYNAMIC(Dlg_Find, CDialog)

Dlg_Find::Dlg_Find(CWnd* pParent /*=NULL*/)
	: CDialog(Dlg_Find::IDD, pParent)
	, m_bMatchCase(false)
	, m_bMatchWholeWord(false)
	, m_bSearchUp(false)
{

}

Dlg_Find::~Dlg_Find()
{
}

void Dlg_Find::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cb_FindWhat);
	DDX_Control(pDX, IDC_CHECK1, m_chk_MatchCase);
	DDX_Control(pDX, IDC_CHECK2, m_chk_MatchWholeWord);
	DDX_Control(pDX, IDC_CHECK3, m_chk_SearchUp);
}


BEGIN_MESSAGE_MAP(Dlg_Find, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Dlg_Find message handlers

BOOL Dlg_Find::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	//UpdateData(TRUE);
	if( m_bMatchCase)
		m_chk_MatchCase.SetCheck( BST_CHECKED);
	else
		m_chk_MatchCase.SetCheck( BST_UNCHECKED);

	if( m_bMatchWholeWord)
		m_chk_MatchWholeWord.SetCheck( BST_CHECKED);
	else
		m_chk_MatchWholeWord.SetCheck( BST_UNCHECKED);

	if( m_bSearchUp)
		m_chk_SearchUp.SetCheck( BST_CHECKED);
	else
		m_chk_SearchUp.SetCheck( BST_UNCHECKED);

	m_cb_FindWhat.SetWindowTextA( m_pFindWhat);
	m_cb_FindWhat.SetEditSel( 0, i3::generic_string_size( m_pFindWhat));
	::SetFocus( m_cb_FindWhat.m_hWnd);

	// Update StringList
	for( size_t i=0; i< m_FoundList.size(); i++)
	{
		m_cb_FindWhat.AddString( m_FoundList[i].c_str() );
	}


	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//"Find" Button을 누름
void Dlg_Find::OnOK()	
{
	// TODO: Add your specialized code here and/or call the base class
	
	//UpdateData(FALSE);
	if( m_chk_MatchCase.GetCheck() == BST_CHECKED)
		m_bMatchCase = true;
	else
		m_bMatchCase = false;

	if( m_chk_MatchWholeWord.GetCheck() == BST_CHECKED)
		m_bMatchWholeWord = true;
	else
		m_bMatchWholeWord = false;

	if( m_chk_SearchUp.GetCheck() == BST_CHECKED)
		m_bSearchUp = true;
	else
		m_bSearchUp = false;

	char pBuf[MAX_PATH];
	m_cb_FindWhat.GetWindowTextA( pBuf, sizeof( pBuf));
	i3::string_ncopy_nullpad( m_pFindWhat, pBuf, sizeof( m_pFindWhat));


	//Update StringList
	INT32 idx = -1;
	size_t nCount = m_FoundList.size();

	for( size_t i=0; i< nCount; i++)
	{
	//	const char* pStr = (const char*)m_FoundList.GetItem(i);
	//	if( i3String::NCompare( pStr, m_pFindWhat, sizeof(m_pFindWhat)) == 0)
		if (m_FoundList[i] == m_pFindWhat )
		{
			idx = i;
			break;
		}
	}

	if( idx == -1)	// add
	{
		if( nCount >= 10)
		{
			//m_FoundList.Delete( 0);
			i3::vu::erase_index(m_FoundList, 0);
		}
			
		m_FoundList.push_back( m_pFindWhat);
	}
	else	// reorder
	{
	}

	CDialog::OnOK();
}

//검색할 문자열을 "InitDialog" 하기 전에 설정(Combo에 표시하기 위함)
void Dlg_Find::SetFindWhat( const char* pszFindWhat)	//
{
	i3::string_ncopy_nullpad( m_pFindWhat, pszFindWhat, sizeof( m_pFindWhat));
}

const char* Dlg_Find::GetFindWhat( void)
{
	return (const char*)m_pFindWhat;
}
