#include "stdafx.h"
#include "resource.h"
#include "DlgList.h"

BEGIN_MESSAGE_MAP(CMediaPathDlg, CDialog)
	ON_COMMAND(IDC_BT_V1MEDIA, LoadV1Media)
	ON_COMMAND(IDC_BT_V2MEDIA, LoadV2Media)

END_MESSAGE_MAP()


CMediaPathDlg::CMediaPathDlg() : CDialog(CMediaPathDlg::IDD)
{
	strV1Path = strV2Path = "";

	FILE* pSaveSetting = fopen("./MediaPath.ini","r");
	if(pSaveSetting)
	{
		char V1_MediaPath[512] = {0,};
		char V2_MediaPath[512] = {0,};
		fscanf(pSaveSetting,"%s\n%s",V1_MediaPath,V2_MediaPath);
		strV1Path = V1_MediaPath;
		strV2Path = V2_MediaPath;

		fclose(pSaveSetting);
	}


	bState = false;
}

void CMediaPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	GetDlgItem(IDC_EDIT_V1MEDIA)->SetWindowText(strV1Path);
	GetDlgItem(IDC_EDIT_V2MEDIA)->SetWindowText(strV2Path);
}

void CMediaPathDlg::LoadV1Media()
{
	ITEMIDLIST*  pildBrowse;
	TCHAR   pszPathname[MAX_PATH];
	BROWSEINFO  bInfo;
	memset(&bInfo, 0, sizeof(bInfo));
	bInfo.hwndOwner   = GetSafeHwnd();
	bInfo.pidlRoot   = NULL;
	bInfo.pszDisplayName = pszPathname;
	bInfo.lpszTitle   = _T("1.0 디렉토리를 선택하세요");
	bInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn    = NULL;
	bInfo.lParam  = (LPARAM)(LPCTSTR)"C:\\";
	bInfo.lParam  = (LPARAM)NULL;
	pildBrowse    = ::SHBrowseForFolder(&bInfo);

	if(pildBrowse)
	{
		SHGetPathFromIDList(pildBrowse, pszPathname);
		GetDlgItem(IDC_EDIT_V1MEDIA)->SetWindowText(pszPathname);
	}
}

void CMediaPathDlg::LoadV2Media()
{
	ITEMIDLIST*  pildBrowse;
	TCHAR   pszPathname[MAX_PATH];
	BROWSEINFO  bInfo;
	memset(&bInfo, 0, sizeof(bInfo));
	bInfo.hwndOwner   = GetSafeHwnd();
	bInfo.pidlRoot   = NULL;
	bInfo.pszDisplayName = pszPathname;
	bInfo.lpszTitle   = _T("1.5 디렉토리를 선택하세요");
	bInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn    = NULL;
	bInfo.lParam  = (LPARAM)(LPCTSTR)"C:\\";
	bInfo.lParam  = (LPARAM)NULL;
	pildBrowse    = ::SHBrowseForFolder(&bInfo);

	if(pildBrowse)
	{
		SHGetPathFromIDList(pildBrowse, pszPathname);
		GetDlgItem(IDC_EDIT_V2MEDIA)->SetWindowText(pszPathname);
	}
}


void CMediaPathDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	GetDlgItem(IDC_EDIT_V1MEDIA)->GetWindowText(strV1Path);
	GetDlgItem(IDC_EDIT_V2MEDIA)->GetWindowText(strV2Path);

	bState = true;

	CDialog::OnOK();
}
