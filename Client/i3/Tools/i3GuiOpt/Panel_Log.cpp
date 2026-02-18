// Panel_Log.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "Panel_Log.h"
#include "GlobalVar.h"

// CPanel_Log dialog

IMPLEMENT_DYNAMIC(CPanel_Log, i3TDKDialogBase)

CPanel_Log::CPanel_Log(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanel_Log::IDD, pParent)
{

}

CPanel_Log::~CPanel_Log()
{
}

void CPanel_Log::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_PROGRESS, m_PRG_File);
	DDX_Control(pDX, IDC_OPT_PROGRESS, m_PRG_Opt);
}

void CPanel_Log::ReplaceControls( INT32 cx, INT32 cy)
{
	CRect rt;

	if( cx == -1)
	{
		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	m_PRG_File.GetWindowRect( &rt);
	ScreenToClient( &rt);

	m_PRG_File.SetWindowPos( NULL, 0, 0, cx - rt.left - 3, rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	m_PRG_Opt.SetWindowPos( NULL, 0, 0, cx - rt.left - 3, rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

void	CPanel_Log::StartProcessing(void)
{
	m_MaxFileProgress = (INT32)g_FileList.size() * g_pOptConfig->getCount();
	m_PRG_File.SetRange32( 0, m_MaxFileProgress);
	m_PRG_File.SetPos( 0);

	m_PRG_Opt.SetPos( 0);

	m_CurFile = 0;

	m_MaxOpt = 100;
	m_CurOpt = 0;

	m_MaxOptSub = 100;
	m_CurOptSub = 0;

	UpdateProgressOpt();
}

void	CPanel_Log::EndProcessing(void)
{
}

void CPanel_Log::UpdateProgressFile(void)
{
}

void CPanel_Log::UpdateProgressOpt(void)
{
	REAL32 rate, factor;

	factor = (1.0f / m_MaxOpt);
	rate = ((REAL32) m_CurOpt / m_MaxOpt) + ((REAL32) m_CurOptSub / m_MaxOptSub * factor);
	
	m_PRG_Opt.SetPos( (INT32) (rate * 1000.0f));
}

void	CPanel_Log::OnOptFile( INT32 idx)
{
	INT32 temp;

	m_CurFile = idx;
	temp = idx * (g_pOptConfig->getCount());

	m_PRG_File.SetPos( temp);
}

void	CPanel_Log::OnOptOpt( INT32 idx)
{
	INT32 temp;

	temp = (m_CurFile * g_pOptConfig->getCount()) + idx;

	m_PRG_File.SetPos( temp);
}

void	CPanel_Log::OnOptEnd(void)
{
	m_PRG_File.SetPos( m_MaxFileProgress);
}

void	CPanel_Log::OnOptCancel(void)
{
	//m_PRG_File.SetPos( 0);
}

BEGIN_MESSAGE_MAP(CPanel_Log, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanel_Log message handlers

BOOL CPanel_Log::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		i3SceneOptimizer::SetOptimizeProgressProc( _MyProgressProc, this);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Log::OnOK()
{
}

void CPanel_Log::OnCancel()
{
}

void CPanel_Log::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_PRG_File.m_hWnd))
	{
		ReplaceControls( cx, cy);
	}
}

void CPanel_Log::_MyProgressProc( I3OPT_PROGRESS_REPORT report, INT32 param1, INT32 param2, const char * pszMsg, void * pUserData)
{
	CPanel_Log * pDlg = (CPanel_Log *) pUserData;
	//char conv[256];

	switch( report)
	{
		case I3OPT_PROGRESS_REPORT_START :
			pDlg->m_MaxOpt = param1;
			pDlg->m_PRG_Opt.SetRange32( 0, 1000);

			/*
			if( pszMsg != NULL)
				i3::string_ncopy_nullpad( pDlg->m_szProgress, pszMsg, sizeof(pDlg->m_szProgress));
			else
				i3::string_ncopy_nullpad( pDlg->m_szProgress, "Phase", sizeof(pDlg->m_szProgress));

			i3::snprintf( conv, sizeof(conv), "%s : 0/%d", pDlg->m_szProgress, pDlg->m_Max);
			pDlg->m_TXT_Progress.SetWindowText( conv);
			*/
			break;

		case I3OPT_PROGRESS_REPORT_SUBSTART :
			pDlg->m_MaxOptSub = param1;

			/*
			if( pszMsg != NULL)
				i3::string_ncopy_nullpad( pDlg->m_szSubProgress, pszMsg, sizeof(pDlg->m_szSubProgress));
			else
				i3::string_ncopy_nullpad( pDlg->m_szSubProgress, "Step", sizeof(pDlg->m_szSubProgress));

			i3::snprintf( conv, sizeof(conv), "%s : 0/%d", pDlg->m_szSubProgress, pDlg->m_SubMax);
			pDlg->m_TXT_SubProgress.SetWindowText( conv);
			*/
			break;

		case I3OPT_PROGRESS_REPORT_END :
			//pDlg->m_PRG_Opt.SetRange32( 0, pDlg->m_Max);

			//i3::snprintf( conv, sizeof(conv), "%d/%d", pDlg->m_Max, pDlg->m_Max);
			//pDlg->m_TXT_Progress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_SUBEND :
			//pDlg->m_SubProgressCtrl.SetRange32( 0, pDlg->m_SubMax);

			//i3::snprintf( conv, sizeof(conv), "%d/%d", pDlg->m_SubMax, pDlg->m_SubMax);
			//pDlg->m_TXT_SubProgress.SetWindowText( conv);
			break;

		case I3OPT_PROGRESS_REPORT_CANCEL :
			//pDlg->Log( "Canceled...");
			break;

		case I3OPT_PROGRESS_REPORT_POS :
			/*
			if( pszMsg != NULL)
				pDlg->Log( pszMsg);
				*/

			pDlg->m_CurOpt = param1;
			
			pDlg->UpdateProgressOpt();

			/*
			i3::snprintf( conv, sizeof(conv), "%s : %d/%d", pDlg->m_szProgress, param1, pDlg->m_Max);
			pDlg->m_TXT_Progress.SetWindowText( conv);
			*/
			break;

		case I3OPT_PROGRESS_REPORT_SUBPOS :
			/*
			if( pszMsg != NULL)
				pDlg->Log( pszMsg);
				*/

			pDlg->m_CurOptSub = param1;
			pDlg->UpdateProgressOpt();

			/*
			i3::snprintf( conv, sizeof(conv), "%s : %d/%d", pDlg->m_szSubProgress, param1, pDlg->m_SubMax);
			pDlg->m_TXT_SubProgress.SetWindowText( conv);
			*/
			break;
	}
}