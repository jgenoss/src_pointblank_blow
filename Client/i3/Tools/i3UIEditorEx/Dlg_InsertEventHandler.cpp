// Dlg_InsertEventHandler.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_InsertEventHandler.h"

#include "ScriptManager.h"

#include "i3Base/string/compare/generic_is_iequal.h"

// CDlg_InsertEventHandler dialog

//#define UM_MESSAGE     (WM_USER+1)

IMPLEMENT_DYNAMIC(CDlg_InsertEventHandler, CDialog)

CDlg_InsertEventHandler::CDlg_InsertEventHandler(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_InsertEventHandler::IDD, pParent)
	, m_strDesc(_T(""))
{
	m_bScreen = false;
	m_pCtrl = NULL;
	m_pTemplate = NULL;
	m_szGenCode[0] = 0;
	m_szBlock[0] = 0;
	m_szFunctionName[0] = 0;
	m_bEventHandler = false;
}

CDlg_InsertEventHandler::~CDlg_InsertEventHandler()
{
}

void CDlg_InsertEventHandler::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LB_IEH_CLASSES, m_lbClasses);
	DDX_Control(pDX, IDC_TAB_IEH_CATEGORY, m_tabCategory);
	DDX_Control(pDX, IDC_LB_IEH_HANDLERS, m_lbHandlers);
	DDX_Text(pDX, IDC_ED_IEH_DESC, m_strDesc);
	DDX_Control(pDX, IDOK, m_btnOK);
}


BEGIN_MESSAGE_MAP(CDlg_InsertEventHandler, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_IEH_CATEGORY, &CDlg_InsertEventHandler::OnTcnSelchange_Category)
	ON_LBN_SELCHANGE(IDC_LB_IEH_CLASSES, &CDlg_InsertEventHandler::OnLbnSelchange_Classes)
	ON_LBN_SELCHANGE(IDC_LB_IEH_HANDLERS, &CDlg_InsertEventHandler::OnLbnSelchange_Handlers)
	ON_LBN_DBLCLK(IDC_LB_IEH_HANDLERS, &CDlg_InsertEventHandler::OnLbnDblclk_Handlers)
	//ON_MESSAGE( UM_MESSAGE, &CDlg_InsertEventHandler::OnOK )
END_MESSAGE_MAP()


// CDlg_InsertEventHandler message handlers

BOOL CDlg_InsertEventHandler::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
		
	if( m_bScreen)	//screen
	{
		i3::vector<i3GameNode*> list;
		ScriptManager::GetLuaControls( list);

		for( size_t i=0; i< list.size(); i++)
		{
			i3UIControl* pCtrl = (i3UIControl*)list[i];
			m_lbClasses.AddString( pCtrl->GetName());
		}
	}
	else			//library
	{
		i3::vector<i3UITemplate*> list;
		ScriptManager::GetLuaTemplates( list);

		for( size_t i=0; i< list.size(); i++)
		{
			i3UITemplate* pTemplate = list[i];

			if( i3::same_of<i3UITemplate_User*>(pTemplate)) //->IsExactTypeOf( i3UITemplate_User::static_meta()))
			{
				m_lbClasses.AddString( pTemplate->GetName());
			}
		}
	}

	if( m_lbClasses.GetCount() > 0)
	{
		m_tabCategory.InsertItem( 0, "Event Handlers");
		m_tabCategory.InsertItem( 1, "Member Functions");
		m_tabCategory.EnableWindow( TRUE);

		_SetInitialClass();
			
		m_btnOK.EnableWindow( TRUE);
	}
	else
	{
		m_tabCategory.EnableWindow( FALSE);
		
		m_btnOK.EnableWindow( FALSE);

		m_strDesc.SetString( "No classes to insert event handler\n");
		UpdateData( FALSE);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_InsertEventHandler::OnTcnSelchange_Category(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	_OnTBSelChange_Category();

	*pResult = 0;
}

void CDlg_InsertEventHandler::OnLbnSelchange_Classes()
{
	// TODO: Add your control notification handler code here
	_OnLBSelChange_Classes();
}

void CDlg_InsertEventHandler::OnLbnSelchange_Handlers()
{
	// TODO: Add your control notification handler code here
	_OnLBSelChange_Handlers();
}


void CDlg_InsertEventHandler::GetGeneratedCodeBlock( char* pBuf, INT32 nBufSize)
{
	i3::string_ncopy_nullpad( pBuf, m_szGenCode, nBufSize);
}

void  CDlg_InsertEventHandler::GetFunctionName( char* pBuf, INT32 nBufSize)
{
	i3::string_ncopy_nullpad( pBuf, m_szFunctionName, nBufSize);
}

void CDlg_InsertEventHandler::GetBlockName( char* pBuf, INT32 nBufSize)
{
	i3::string_ncopy_nullpad( pBuf, m_szBlock, nBufSize);
}

void CDlg_InsertEventHandler::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	// 선택된 class명과 handler명을 조합하여 삽입할 code block을 만듭니다.
	INT32 nClass = m_lbClasses.GetCurSel();
	I3ASSERT( nClass > -1);
	m_lbClasses.GetText( nClass, m_szBlock);

	INT32 nFunc = m_lbHandlers.GetCurSel();
	I3ASSERT( nFunc > -1);
	m_lbHandlers.GetText( nFunc, m_szFunctionName);

	if( m_tabCategory.GetCurSel() == 0)		//event handler
	{
		//user wants to override this function
		//enableUIEvent도 같이 호출해 줘야한다.
		i3::snprintf(	m_szGenCode, sizeof( m_szGenCode), 
							"\n%s:enableUIEvent(%s)\n\nfunction %s:%s()\n\nend\n", 
							m_szBlock, ScriptManager::GetLuaEventCode( m_szFunctionName), m_szBlock, m_szFunctionName);

		m_bEventHandler = true;
	}
	else if( m_tabCategory.GetCurSel() == 1)
	{
		//user wants to know the signature of this function 
		//parameter는 어떻게 할것인가..
		char pParams[MAX_PATH];
		ScriptManager::GetLuaMemberParams( m_szFunctionName, pParams);
		i3::snprintf( m_szGenCode, sizeof( m_szGenCode), "\n%s:%s(%s)", m_szBlock, m_szFunctionName, pParams);

		m_bEventHandler = false;
	}

	CDialog::OnOK();
}

void CDlg_InsertEventHandler::_SetInitialClass( void)
{
	INT32 nSel = -1;

	if( m_szBlock[0] != 0)
	{
		for( INT32 i=0; i< m_lbClasses.GetCount(); i++)
		{
			char pBuf[MAX_PATH];
			m_lbClasses.GetText( i, pBuf);
			
			if( i3::generic_is_iequal( pBuf, m_szBlock) )
			{
				nSel = i;
				break;
			}
		}
	}
	else
		nSel = 0;

	I3ASSERT( nSel > -1);
	
	m_lbClasses.SetCurSel( nSel);

	_OnLBSelChange_Classes();
}

void CDlg_InsertEventHandler::_OnLBSelChange_Classes( void)
{
	// ListBox(Classes)의 Selected Item이 바뀌면 ListBox(Handlers)를 업데이트해준다.
	char szClass[64];
	INT32 nClass = m_lbClasses.GetCurSel();
	I3ASSERT( nClass > -1);
	m_lbClasses.GetText( nClass, szClass);	//현재 선택된 Class



	if( m_bScreen)	//screen
	{
		//m_pCtrl = i3UI::FindControlByName( szClass);
		//preview Mode에서만 되는듯.. OpenLuaContext 해볼까..
		//PreviewMode랑 OpenLuaContext랑 다른가..
		i3::vector<i3GameNode*> list;
		ScriptManager::GetLuaControls( list);

		for( size_t i=0; i< list.size(); i++)
		{
			i3UIControl* pCtrl = (i3UIControl*)list[i];
			if( i3::generic_is_iequal( pCtrl->GetName(), szClass) )
			{
				m_pCtrl = pCtrl;
				break;
			}
		}
		
		I3ASSERT( m_pCtrl != NULL);
	}
	else
	{
		i3::vector<i3UITemplate*> list;

		ScriptManager::GetLuaTemplates( list);

		for( size_t i=0; i< list.size(); i++)
		{
			i3UITemplate* pTemplate = list[i];
			if( i3::generic_is_iequal( pTemplate->GetName(), szClass) )
			{
				m_pTemplate = pTemplate;
				break;
			}
		}

		I3ASSERT( m_pTemplate != NULL);
	}

	_UpdateListBox_Handlers();
}

void CDlg_InsertEventHandler::_OnLBSelChange_Handlers( void)
{
	_UpdateEditBox_Desc();
}

void CDlg_InsertEventHandler::_OnTBSelChange_Category( void)
{
	_UpdateListBox_Handlers();
}

void CDlg_InsertEventHandler::_UpdateListBox_Handlers( void)
{
	m_lbHandlers.ResetContent();

	i3::vector<i3::rc_string> list;

	if( m_bScreen)	//Control
	{
		if( m_tabCategory.GetCurSel() == 0)	//Event Handlers	
			ScriptManager::GetLuaEventHandlers( list, m_pCtrl);
		else if( m_tabCategory.GetCurSel() == 1)	//Member Functions
			ScriptManager::GetLuaMemberFunctions( list, m_pCtrl);
	}
	else	//Template
	{
		if( m_tabCategory.GetCurSel() == 0)	//Event Handlers	
			ScriptManager::GetLuaEventHandlers( list, NULL);
		//else if( m_tabCategory.GetCurSel() == 1)	//Member Functions
		//	m_lbHandlers.AddString( "Not Ready");
	}

	for( size_t i=0; i< list.size(); i++)
	{
		if( m_lbHandlers.FindStringExact( -1, list[i].c_str() ) == LB_ERR)
			m_lbHandlers.AddString( list[i].c_str() );
	}

	if( m_lbHandlers.GetCount() > 0)
	{
		m_btnOK.EnableWindow( TRUE);
		m_lbHandlers.SetCurSel( 0);
	}
	else
	{
		m_btnOK.EnableWindow( FALSE);
	}

	_OnLBSelChange_Handlers();
}

void CDlg_InsertEventHandler::_UpdateEditBox_Desc( void)
{
	INT32 nSel = m_lbHandlers.GetCurSel();

	if( nSel > -1)
	{
		char pBuf[MAX_PATH];
		m_lbHandlers.GetText( nSel, pBuf);

		if( m_tabCategory.GetCurSel() == 0)
		{
			const char* pszDesc = ScriptManager::GetLuaEventDescription( pBuf);
			m_strDesc.SetString( pszDesc);
		}
		else if( m_tabCategory.GetCurSel() == 1)
		{
			char pDesc[MAX_PATH];
			ScriptManager::GetLuaMemberDescription( pBuf, pDesc);
			m_strDesc.SetString( pDesc);
		}
	}
	else
		m_strDesc.SetString( "");

	UpdateData( FALSE);	
}

void CDlg_InsertEventHandler::OnLbnDblclk_Handlers()
{
	// TODO: Add your control notification handler code here

	if( m_lbClasses.GetCurSel() > -1 && m_lbHandlers.GetCurSel() > -1)
	{
		//PostMessage( UM_MESSAGE, (WPARAM)0, (LPARAM)0);
		OnOK();
	}
}
