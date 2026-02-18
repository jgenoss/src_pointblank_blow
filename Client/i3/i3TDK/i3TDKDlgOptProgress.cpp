// i3TDKDlgOptProgress.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgOptProgress.h"
#include ".\i3tdkdlgoptprogress.h"


// i3TDKDlgOptProgress dialog

IMPLEMENT_DYNAMIC(i3TDKDlgOptProgress, CDialog)

void i3TDKDlgOptProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_NAME, m_ED_Name);
	DDX_Control(pDX, IDC_TDK_ST_PROGRESS, m_TXT_Progress);
	DDX_Control(pDX, IDC_TDK_PROGRESS1, m_ProgressCtrl);
	DDX_Control(pDX, IDC_TDK_ST_LOGPH, m_ST_LogPH);
	DDX_Control(pDX, IDOK, m_BTN_Close);
	DDX_Control(pDX, IDC_TDK_ST_PROGRESS_SUB, m_TXT_SubProgress);
	DDX_Control(pDX, IDC_TDK_PROGRESS_SUB, m_SubProgressCtrl);
}

void i3TDKDlgOptProgress::Execute( i3SceneOptimizer * pOpt)
{
	m_pOpt = pOpt;

	i3SceneOptimizer::SetOptimizeProgressProc( _MyProgressProc, this);

	i3TDK::SetResInstance();
	DoModal();
	i3SceneOptimizer::SetOptimizeProgressProc( nullptr, this);
	i3TDK::RestoreResInstance();
}

void i3TDKDlgOptProgress::Log( const TCHAR* pszStr)
{
	m_LogCtrl.AddText( pszStr);
}

void	i3TDKDlgOptProgress::_MyProgressProc( I3OPT_PROGRESS_REPORT report, INT32 param1, INT32 param2, const char * pszMsg, void * pUserData)
{
	i3TDKDlgOptProgress * pDlg = (i3TDKDlgOptProgress *) pUserData;
	if( !pDlg ) return ;
	if( !pDlg->m_ProgressCtrl ) return;

	TCHAR conv[256];

	switch( report)
	{
		case I3OPT_PROGRESS_REPORT_START :
			pDlg->m_Max = param1;
			pDlg->m_ProgressCtrl.SetRange32( 0, param1);

			if( pszMsg != nullptr)
			{
#ifdef _UNICODE
				i3::mb_to_utf16(pszMsg, pDlg->m_szProgress, _countof(pDlg->m_szProgress));
#else
				i3::safe_string_copy( pDlg->m_szProgress, pszMsg, 256);
#endif
			}
			else
			{
				i3::safe_string_copy( pDlg->m_szProgress, _T("Phase"), 256);
			}

			i3::snprintf( conv, _countof(conv), _T("%s : 0/%d"), pDlg->m_szProgress, pDlg->m_Max);
			pDlg->m_TXT_Progress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_SUBSTART :
			pDlg->m_SubMax = param1;
			pDlg->m_SubProgressCtrl.SetRange32( 0, param1);

			if( pszMsg != nullptr)
			{
#ifdef _UNICODE
				i3::mb_to_utf16( pszMsg, pDlg->m_szSubProgress, _countof(pDlg->m_szSubProgress) );
#else
				i3::safe_string_copy( pDlg->m_szSubProgress, pszMsg, _countof(pDlg->m_szSubProgress) );
#endif
			}
			else
				i3::safe_string_copy( pDlg->m_szSubProgress, _T("Step"), 256);

			i3::snprintf( conv, _countof(conv), _T("%s : 0/%d"), pDlg->m_szSubProgress, pDlg->m_SubMax);
			pDlg->m_TXT_SubProgress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_END :
			pDlg->m_ProgressCtrl.SetRange32( 0, pDlg->m_Max);
			pDlg->Log( _T("Finished...") );

			i3::snprintf( conv, _countof(conv), _T("%d/%d"), pDlg->m_Max, pDlg->m_Max);
			pDlg->m_TXT_Progress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_SUBEND :
			pDlg->m_SubProgressCtrl.SetRange32( 0, pDlg->m_SubMax);

			i3::snprintf( conv, _countof(conv), _T("%d/%d"), pDlg->m_SubMax, pDlg->m_SubMax);
			pDlg->m_TXT_SubProgress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_CANCEL :
			pDlg->m_ProgressCtrl.SetRange32( 0, pDlg->m_Max);
			pDlg->Log( _T("Canceled...") );
			break;

		case I3OPT_PROGRESS_REPORT_POS :
			if( pszMsg != nullptr)
			{
#ifdef _UNICODE
				i3::stack_wstring wstrLog;	i3::mb_to_utf16(pszMsg, wstrLog);
				const wchar_t* szLog = wstrLog.c_str();
#else
				const char* szLog = pszMsg;
#endif
				pDlg->Log( szLog );
			}

			pDlg->m_ProgressCtrl.SetPos( param1 + 1);

			i3::snprintf( conv, _countof(conv), _T("%s : %d/%d"), pDlg->m_szProgress, param1, pDlg->m_Max);
			pDlg->m_TXT_Progress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_SUBPOS :
			if( pszMsg != nullptr)
			{
#ifdef _UNICODE
				i3::stack_wstring wstrLog;	i3::mb_to_utf16(pszMsg, wstrLog);
				const wchar_t* szLog = wstrLog.c_str();
#else
				const char* szLog = pszMsg;
#endif
				pDlg->Log( szLog );
			}

			pDlg->m_SubProgressCtrl.SetPos( param1 + 1);

			i3::snprintf( conv, _countof(conv), _T("%s : %d/%d"), pDlg->m_szSubProgress, param1, pDlg->m_SubMax);
			pDlg->m_TXT_SubProgress.SetWindowText( conv);
			break;
	}
}

BEGIN_MESSAGE_MAP(i3TDKDlgOptProgress, CDialog)
END_MESSAGE_MAP()


// i3TDKDlgOptProgress message handlers

BOOL i3TDKDlgOptProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		m_ST_LogPH.GetWindowRect( &rt);
		ScreenToClient( &rt);

		m_LogCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	{
#ifdef _UNICODE
		i3::stack_wstring strMeta;	i3::mb_to_utf16(m_pOpt->meta()->class_name(), strMeta);
#else
		const i3::fixed_string& strMeta = m_pOpt->meta()->class_name();
#endif
		m_ED_Name.SetWindowText( strMeta.c_str() ) ;
	}

	{
		m_szProgress[0] = 0;
		m_szSubProgress[0] = 0;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
