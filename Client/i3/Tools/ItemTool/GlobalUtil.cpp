#include "stdafx.h"
#include "GlobalUtil.h"

cItemToolMainFrameWork *	g_pMainSCFramework		= NULL;
cItemToolSubFrameWork *		g_pWeaponSCFramework	= NULL;
cItemToolSubFrameWork*		g_pItemSCFramework		= NULL;
cItemToolViewer*			g_pViewer				= NULL;
CChara *					g_pChara				= NULL;
cPefDataList *				g_pPefList				= NULL;

HWND	g_HwndWeaponProp;
HWND	g_HwndDlgTimeTrack;
HWND	g_HwndWeaponResList;
HWND	g_HwndDlgWeaponEditor;
HWND	g_HwndDlgAIDriver;
HWND	g_HwndDlgAIEnumList;
HWND	g_HwndDockPropertyWnd;

CString g_strRootPath = "";
CString g_strCurrentDir = "";

mStrVecCon g_mAIDrive;
mStrStrCon g_mOverlapedEditorFile;