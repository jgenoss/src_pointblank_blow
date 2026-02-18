// DlgCreateNewWeapon.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DlgCreateNewWeapon.h"
#include "MainFrm.h"
#include "ClientWeaponData/1.1/CommonDef.h"
#include "i3Base/itl/range/generic_empty.h"

//#include "../../../PointBlank_15/Source_New/CommonSource/ItemID/ItemTypeWeaponClassDef.h"
//#include "../../../PointBlank_15/Source_New/CommonSource/ItemID/ItemTypeDef.h"

// enum WEAPON_CLASS_TYPE
// {
// 	WEAPON_CLASS_UNKNOWN	= 0,
// 
// 	WEAPON_CLASS_KNIFE,		//	나이프
// 	WEAPON_CLASS_HANDGUN,	//	권총
// 	WEAPON_CLASS_ASSAULT,	//	소총
// 	WEAPON_CLASS_SMG,		//	서브머신건
// 	WEAPON_CLASS_SNIPER,	//	저격총
// 	WEAPON_CLASS_SHOTGUN,	//	산탄총
// 	WEAPON_CLASS_THROWING1,	//	투척 무기
// 	WEAPON_CLASS_THROWING2,	//	특수무기(C4,연막)
// 	WEAPON_CLASS_MISSION,	//	미션 아이템
// 	WEAPON_CLASS_MG,		//	머신건
// 
// 	// 게임에서만 사용하는 클래스 ( 서버에는 없습니다.)
// 	WEAPON_CLASS_GRENADESHELL,		// 유탄
// 	WEAPON_CLASS_BOMBTRIGGER,		// 폭탄 스위치
// 
// 	// 새로 추가되는 무기 2008.07.09
// 	WEAPON_CLASS_CIC,
// 	WEAPON_CLASS_DUALHANDGUN,
// 	WEAPON_CLASS_DUALKNIFE,
// 
// 	WEAPON_CLASS_ROCKET_LAUNCHER,
// 	WEAPON_CLASS_OBJECT_GUN,		// 특정 오브젝트에 붙어 있는 총 ( M197....)
// 
// 	WEAPON_CLASS_DUALSMG,
// 
// 	WEAPON_CLASS_DINO,		
// 	WEAPON_CLASS_DINO_LEFT_SCRATCH,	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
// 	WEAPON_CLASS_TREX,				// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
// 	WEAPON_CLASS_STING,
// 	WEAPON_CLASS_KNUCKLE,			// 너클
// 	WEAPON_CLASS_ELITE_RIGHT_SCRATCH,
// 	WEAPON_CLASS_ELITE_LEFT_SCRATCH,
// 	WEAPON_CLASS_BOW,
// 	WEAPON_CLASS_COUNT,
// 
// 
// 	WEAPON_CLASS_WOODEN_ARROW = 99
// };

const static CString STR_WEAPON_CLASS[WEAPON_CLASS_COUNT] = 
{
	"",//WEAPON_CLASS_UNKNOWN	= 0,

	"Knife",//WEAPON_CLASS_KNIFE,		//	나이프
	"HandGun",//WEAPON_CLASS_HANDGUN,	//	권총
	"Assert",//WEAPON_CLASS_ASSAULT,	//	소총
	"Sub Machine Gun",//WEAPON_CLASS_SMG,		//	서브머신건
	"Sniper",//WEAPON_CLASS_SNIPER,	//	저격총
	"ShotGun",//WEAPON_CLASS_SHOTGUN,	//	산탄총
	"Throwing",//WEAPON_CLASS_THROWING1,	//	투척 무기
	"Throwing ETC",//WEAPON_CLASS_THROWING2,	//	특수무기(C4,연막)
	"Mission",//WEAPON_CLASS_MISSION,	//	미션 아이템
	"Machine Gun",//WEAPON_CLASS_MG,		//	머신건

	//// 게임에서만 사용하는 클래스 ( 서버에는 없습니다.)
	"GrenadeShell",//WEAPON_CLASS_GRENADESHELL,		// 유탄
	"BombTrigger",//WEAPON_CLASS_BOMBTRIGGER,		// 폭탄 스위치

	//// 새로 추가되는 무기 2008.07.09
	"CIC",//WEAPON_CLASS_CIC,
	"Dual HandGun",//WEAPON_CLASS_DUALHANDGUN,
	"Dual Knife",//WEAPON_CLASS_DUALKNIFE,

	"Rocket Launcher",//WEAPON_CLASS_ROCKET_LAUNCHER,
	"Object Gun",//WEAPON_CLASS_OBJECT_GUN,		// 특정 오브젝트에 붙어 있는 총 ( M197....)

	"Dual Sub Machine Gun",//WEAPON_CLASS_DUALSMG,

	"",//WEAPON_CLASS_DINO,		
	"",//WEAPON_CLASS_DINO_LEFT_SCRATCH,	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
	"",//WEAPON_CLASS_TREX,				// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
	"",//WEAPON_CLASS_STING,
	"Knuckle",//WEAPON_CLASS_KNUCKLE,			// 너클
	"",//WEAPON_CLASS_ELITE_RIGHT_SCRATCH,
	"",//WEAPON_CLASS_ELITE_LEFT_SCRATCH,
	"Bow",//WEAPON_CLASS_BOW,
	//WEAPON_CLASS_COUNT,


	//WEAPON_CLASS_WOODEN_ARROW = 99
};

IMPLEMENT_DYNAMIC(cDlgCreateNewWeapon, CDialog)

cDlgCreateNewWeapon::cDlgCreateNewWeapon(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgCreateNewWeapon::IDD, pParent)
	, m_str_selected_weapon_name(_T(""))
	, m_edit_new_weapon_name(_T(""))
	, m_edit_new_explain(_T(""))
{

}

cDlgCreateNewWeapon::~cDlgCreateNewWeapon()
{
}

void cDlgCreateNewWeapon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_NEW_WEAPON_NAME, m_edit_new_weapon_name);
	DDX_Control(pDX, IDC_COMBO_WEAPON_LIST, m_combo_weapon_list);
	DDX_Control(pDX, IDC_LIST_CURR_WEAPONS, m_list_curr_weapons);
	DDX_Control(pDX, IDC_EDIT_SEARCH_WEAPON, m_edit_search_weapon);
	DDX_Text(pDX, IDC_SELECTED_WEAPON_NAME, m_str_selected_weapon_name);
	DDX_Text(pDX, IDC_EDIT_NEW_WEAPON_NAME, m_edit_new_weapon_name);
	//DDX_Text(pDX, IDC_NEW_EXPLAIN, m_edit_new_explain);
	DDX_Text(pDX, IDC_NEW_EXPLAIN, m_edit_new_explain);
}


BEGIN_MESSAGE_MAP(cDlgCreateNewWeapon, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DO_CLONE, &cDlgCreateNewWeapon::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO_WEAPON_LIST, &cDlgCreateNewWeapon::OnCbnSelchangeComboWeaponList)
	ON_CBN_EDITCHANGE(IDC_COMBO_WEAPON_LIST, &cDlgCreateNewWeapon::OnCbnEditchangeComboWeaponList)
	ON_NOTIFY(NM_THEMECHANGED, IDC_COMBO_WEAPON_LIST, &cDlgCreateNewWeapon::OnNMThemeChangedComboWeaponList)
	ON_CBN_EDITUPDATE(IDC_COMBO_WEAPON_LIST, &cDlgCreateNewWeapon::OnCbnEditupdateComboWeaponList)
	ON_EN_CHANGE(IDC_EDIT_SEARCH_WEAPON, &cDlgCreateNewWeapon::OnEnChangeEditSearchWeapon)
	
	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	//ON_UPDATE_COMMAND_UI(IDC_LIST_CURR_WEAPONS, &cDlgCreateNewWeapon::OnUpdate)
	//ON_COMMAND(IDC_LIST_CURR_WEAPONS, &cDlgCreateNewWeapon::OnUpdateEvent)
	//ON_NOTIFY_REFLECT(IDC_LIST_CURR_WEAPONS, &cDlgCreateNewWeapon::OnUpdateEvent)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CURR_WEAPONS, &cDlgCreateNewWeapon::OnLvnItemchangedListCurrWeapons)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_NEW_WEAPON, &cDlgCreateNewWeapon::OnBnClickedButtonCreateNewWeapon)
END_MESSAGE_MAP()

void cDlgCreateNewWeapon::InsertWeaponList(const std::vector<std::string >& vWeaponList)
{
	
}
// cDlgCreateNewWeapon 메시지 처리기입니다.

bool cDlgCreateNewWeapon::ReadFiles(std::vector<CString>& vFolders, std::vector<CString>& vFiles, CString strRootPath, CString strSubPath)
{
	CString strCurrPath = strRootPath;
	
	if (!strSubPath.IsEmpty())
		strCurrPath += strSubPath;

	char prevPath[ MAX_PATH ];
	::GetCurrentDirectory( MAX_PATH, prevPath );
	::SetCurrentDirectory( strCurrPath );

	WIN32_FIND_DATA fileData;

	HANDLE handle = ::FindFirstFile( _T("*.*"), &fileData );

	if( INVALID_HANDLE_VALUE == handle )
	{
		return true;
	}

	do
	{
		CString fileName = fileData.cFileName;

		if( fileName == _T(".") || fileName == _T("..") || fileName == _T(".svn") )
		{
			continue;
		}
		CString strSub = strSubPath + "\\" + fileName;
		//디렉토리만 제외
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			vFolders.push_back(strSub);
			ReadFiles(vFolders, vFiles, strRootPath, strSub);
		}
		else
		{
			vFiles.push_back(strSub);
		}

	} while( TRUE == ::FindNextFile( handle, &fileData ) );
	
	::FindClose( handle );
	::SetCurrentDirectory( prevPath );

	return true;
}

void cDlgCreateNewWeapon::OnBnClickedButton1()
{
	UpdateData(TRUE);

	if (m_edit_new_weapon_name.IsEmpty() || m_str_selected_weapon_name.IsEmpty())
		return;

	CString strResName = m_mWeaponTargetResName[m_str_selected_weapon_name];
	if (strResName.IsEmpty())
	{
		CString strTmpLog;
		strTmpLog.Format("Not Found Target Resource : %s, Name : \n", strResName, m_str_selected_weapon_name);
		I3ASSERT(strTmpLog);
		return;
	}
	
	strResName.MakeLower();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	cDlgFileLoader* pLoader = pFrame->GetFileLoader();
	CString strCharaEditorName = pLoader->GetFileFolderName((LPTSTR)(LPCTSTR)strResName).c_str();

	CString strNewFolder	= STR_WEAPON_PATH + "\\" + m_edit_new_weapon_name;
	CString strFolder		= STR_WEAPON_PATH + "\\" + strCharaEditorName;
	BOOL br = ::PathFileExists(strNewFolder);

	//복제될 기본 폴더를 생성한다..
	if (FALSE == br)
	{
		::CreateDirectory(strNewFolder, NULL);
	}

	br = ::PathFileExists(strFolder);

	if (FALSE == br)
		ASSERT(0);

	WIN32_FIND_DATA fileData;
	HANDLE hSearch = FindFirstFile("*.*", &fileData);

	std::vector<CString > vFolder;
	std::vector<CString > vFileList;

	//복사할 파일이름과 폴더이름을 모두 뽑아낸다.
	ReadFiles(vFolder, vFileList, (LPTSTR)(LPCTSTR)strFolder, "");

	//복제될 하위폴더를 생성한다.
	for (size_t ui = 0; ui < vFolder.size(); ++ui)
	{
		CString strSrcFolder = strNewFolder + vFolder[ui];
		BOOL br = ::PathFileExists(strSrcFolder);

		if (FALSE == br)
		{
			::CreateDirectory(strSrcFolder, NULL);
		}
	}
	
	//CString strCharaEditorName;
	//파일을 복사
	std::vector<CString> vResoucePath;
	for (size_t ui = 0; ui < vFileList.size(); ++ui)
	{
		CString strFilePath = vFileList[ui];
		CString strSrc = strFolder + strFilePath;
		CString strDest = strNewFolder + strFilePath;

		strFilePath.MakeLower();
		if (strFilePath.Find("i3charaeditor") >= 0)
		{
			strDest = strNewFolder + "\\" + m_edit_new_weapon_name + ".i3CharaEditor";
			strCharaEditorName = strDest;
		}
		else if (strFilePath.Find("resource") >= 0 &&
				 strFilePath.Find("i3s") >= 0)
		{
			strDest = strNewFolder + vFileList[ui];
			vResoucePath.push_back(strDest);
		}

		if (FALSE == ::CopyFile( strSrc, strDest, FALSE ))
			I3ASSERT(0);
	}

	::FindClose( hSearch );

	I3_TDK_UPDATE_INFO Data;
	Data.m_Event = I3_TDK_UPDATE_SELECT;
	strCharaEditorName.Format("Weapon\\%s\\%s.i3CharaEditor", m_edit_new_weapon_name, m_edit_new_weapon_name);

	//캐릭터에디터 파일을 복사한 파일로 오픈한다..
	::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM) &Data, (LPARAM)(LPTSTR)(LPCTSTR)strCharaEditorName);

	//기존 LOD삭제후 복사한 파일로 다시 로딩한다
	g_pChara->RemoveAllLOD();
	g_pChara->RemoveAllSceneGraph();
	for (size_t ui = 0; ui < vResoucePath.size() ;++ui)
	{
		g_pChara->LoadSceneGraph( vResoucePath[ui]);
	}

	CString szFinalPath =  g_strRootPath + "\\" + strCharaEditorName;

	i3ResourceFile file;
	if( g_pChara == NULL)
		return;

	file.setKeyObject( g_pChara);
	if( file.Save( szFinalPath) == STREAM_ERR)
	{
		I3WARN( "%s 파일을 저장할 수 없습니다.", szFinalPath.GetString());
		return;
	}
	
	Data.m_Event = I3_TDK_UPDATE_EDIT;
	::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM)&Data, (LPARAM)(LPTSTR)(LPCTSTR)strCharaEditorName);

	CString strSend = m_str_selected_weapon_name +"/"+m_edit_new_weapon_name;
	
	//PEF파일에 추가한다.
	Data.m_Event = I3_TDK_UPDATE_ADD;
	::SendMessage(g_HwndDockPropertyWnd, WM_TDK_UPDATE, (WPARAM)&Data, (LPARAM)(LPTSTR)(LPCTSTR)strSend);

	//해당 pef아이템을 선택
	Data.m_Event = I3_TDK_UPDATE_SELECT;
	::SendMessage(g_HwndDockPropertyWnd, WM_TDK_UPDATE, (WPARAM)&Data, (LPARAM)(LPTSTR)(LPCTSTR)m_edit_new_weapon_name);

	MessageBox("복사 완료~",MB_OK);

	OnOK();
	

	//g_pChara->RemoveAllSceneGraph();
}


BOOL cDlgCreateNewWeapon::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::vector<int > vWeaponClass;
	vWeaponClass.resize(WEAPON_CLASS_COUNT);

	int nComboCnt = 0;
	for (int i = 0; i < WEAPON_CLASS_COUNT; ++i)
	{
		if (i3::generic_empty((const char*)STR_WEAPON_CLASS[i]) )
			continue;

		m_combo_weapon_list.InsertString(nComboCnt, STR_WEAPON_CLASS[i]);
		vWeaponClass[i] = nComboCnt++;
	}

	m_vvStrList.clear();
	m_vvStrList.resize(WEAPON_CLASS_COUNT);

	for (size_t i = 0; i < g_pPefList->GetCurrKeyList()->size(); ++i)
	{
		char szFolderName[MAX_PATH];
		g_pPefList->GetItemResName(i, szFolderName, MAX_PATH);
	}

	//char szFolderName[MAX_PATH];
	//UINT32 uiSize = g_pPefList->GetCurrKeyList()->size();

	for (size_t i = 0; i < g_pPefList->GetCurrKeyList()->size(); ++i)
	{
		char szResName[MAX_PATH];
		g_pPefList->GetItemResName(i, szResName, MAX_PATH);

		const char* szFolderName = g_pPefList->GetItemNodeName(i);

		if (i3::generic_empty(szFolderName) )
			continue;
		
		int nClassIdx = 0;
		int nNumber = 0;
			
		//1.5
		//g_pPefList->GetItemIndex(i, "ITEMID", nClassIdx);

		//1.0
		g_pPefList->GetItemIndex(i, "_ClassType", nClassIdx);
		g_pPefList->GetItemIndex(i, "_Number", nNumber);

// 		ITEM_TYPE itemType			= (ITEM_TYPE)GET_ITEM_FLAG_TYPE(nClassIdx);
// 		
// 		if (itemType < ITEM_TYPE_PRIMARY && itemType > ITEM_TYPE_THROWING2 )
// 			continue;

		//MAKE_ITEM_FLAG()

		WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE) nClassIdx;

		//UnKnown 클래스
		if (classType == 0)
			continue;

		if (i3::generic_empty((const char*)STR_WEAPON_CLASS[classType]) )
			continue;

		int nTmpIdx = vWeaponClass[classType];
		m_vvStrList[nTmpIdx].push_back(szFolderName);

		m_mWeaponTargetResName[szFolderName] = szResName;
	}

	m_list_curr_weapons.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	m_list_curr_weapons.InsertColumn (0,_T(""),LVCFMT_CENTER);

	CRect rt;
	m_list_curr_weapons.GetClientRect(rt);
	m_list_curr_weapons.SetColumnWidth(0, rt.right - 17);


	//m_edit_new_weapon_name.
	m_edit_new_explain =	"새로운 무기를 만들면 PEF를 수동으로 추가해야 한다.\n"\
							"<기존 무기 복사를 추천합니다.>\n\n"\
							"무기 복제는 기존 무기와 동일하나 메쉬, 텍스쳐및 성능이 약간 틀린 무기를 추가할수 있습니다.\n"\
							"기본적으로 리소스는 미리 만들어져 있어야 합니다.\n"\
							"폴더는(ex:C:/Media/Weapon/무기이름)형식으로 되어 있어야 하며\n"\
							"무기파일 이름및 폴더명은 동일해야 합니다.";

	
	//m_edit_new_explain.Format()

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void cDlgCreateNewWeapon::OnCbnSelchangeComboWeaponList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_combo_weapon_list.GetCurSel();
	
	m_list_curr_weapons.DeleteAllItems();

	for (size_t ui = 0; ui < m_vvStrList[nSel].size(); ++ui)
	{
		m_list_curr_weapons.InsertItem(ui, m_vvStrList[nSel][ui]);
	}
}

void cDlgCreateNewWeapon::OnCbnEditchangeComboWeaponList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void cDlgCreateNewWeapon::OnNMThemeChangedComboWeaponList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void cDlgCreateNewWeapon::OnCbnEditupdateComboWeaponList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void cDlgCreateNewWeapon::OnEnChangeEditSearchWeapon()
{
	CString strText;
	m_edit_search_weapon.GetWindowText(strText);
	strText.MakeLower();

	vStrListCon vFindList;
	for each(vStrListCon p in m_vvStrList)
	{
		for each(CString strName in p)
		{
			CString strTmp = strName;
			strTmp.MakeLower();
			if (strTmp.Find(strText) >= 0)
				vFindList.push_back(strName);
		}
	}

	std::sort(vFindList.begin(), vFindList.end());

	m_list_curr_weapons.DeleteAllItems();
	for (size_t ui = 0; ui < vFindList.size(); ++ui)
	{
		m_list_curr_weapons.InsertItem(ui, vFindList[ui]);
	}

	if(strText.IsEmpty())
	{
		int nSel = m_combo_weapon_list.GetCurSel();

		m_list_curr_weapons.DeleteAllItems();

		for (size_t ui = 0; ui < m_vvStrList[nSel].size(); ++ui)
		{
			m_list_curr_weapons.InsertItem(ui, m_vvStrList[nSel][ui]);
		}	
	}
}

LRESULT cDlgCreateNewWeapon::OnKickIdle()
{
	UpdateDialogControls(this, FALSE);
	return (LRESULT)0;
}
void cDlgCreateNewWeapon::OnLvnItemchangedListCurrWeapons(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	char szWeaponName[256];
	m_list_curr_weapons.GetItemText(pNMLV->iItem, pNMLV->iSubItem, szWeaponName, 256);
	m_str_selected_weapon_name = szWeaponName;
	
	UpdateData(FALSE);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

bool cDlgCreateNewWeapon::CheckResourceFiles(const CString& strSelectPath)
{
	CString strWeaponResource = strSelectPath + "\\Resource";

	int nPos = strSelectPath.ReverseFind('\\');
	CString strWeaponName = strSelectPath.Mid(nPos+1,strSelectPath.GetLength());

	char prevPath[ MAX_PATH ];
	::GetCurrentDirectory( MAX_PATH, prevPath );
	::SetCurrentDirectory(strWeaponResource);

	m_vNewResourceLOD.clear();
	m_vNewAnimation.clear();

	HANDLE hf = NULL; 
	WIN32_FIND_DATA findData;

	hf = ::FindFirstFile( _T("*.*"), &findData );

	if( INVALID_HANDLE_VALUE == hf )
		return true;

	do
	{
		CString fileName = findData.cFileName;

		if( fileName == _T(".") || fileName == _T("..") || fileName == _T(".svn") )
		{
			continue;
		}

		int nPos = fileName.ReverseFind('.');
		if (nPos != -1)
		{
			CString strExt = fileName.Mid(nPos+1, fileName.GetLength());
			strExt.MakeLower();

			if (strExt == "i3s")
			{
				m_vNewResourceLOD.push_back(fileName);
			}
			else if(strExt == "i3a")	//애니메이션 파일은 없어도 된다.
			{
				m_vNewAnimation.push_back(fileName);
			}
		}

	}while( TRUE == ::FindNextFile( hf, &findData ) );

	::SetCurrentDirectory(prevPath);

	if (m_vNewResourceLOD.empty())
		return false;

	//LOD0, LOD1, LOD2
	int nMaxLOD[3];

	memset(&nMaxLOD,-1,sizeof(int)*3);

	int nLODCnt = 0;

	for (size_t ui = 0; ui < m_vNewResourceLOD.size(); ++ui)
	{
		int nPos = m_vNewResourceLOD[ui].Find('.');
		CString strExt = m_vNewResourceLOD[ui].Mid(nPos+1, m_vNewResourceLOD[ui].GetLength());
		if (nPos != -1 && strExt == "i3s")
		{
			CString strFileName = m_vNewResourceLOD[ui].Mid(0, nPos);
			strFileName.MakeLower();

			CString strLODName;
			strLODName.Format("%s_lod%d",strWeaponName,ui);
			strLODName.MakeLower();

			if (nLODCnt >= 3)
			{
				AfxMessageBox("LOD갯수가 3개가 넘습니다.\n lod.i3s파일의 갯수를 확인하세요",MB_OK);
				return false;
			}
			if (strFileName == strLODName)
				nMaxLOD[nLODCnt++] = 0;
			else
				nMaxLOD[nLODCnt++] = -1;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		if (nMaxLOD[i] == -1)
		{
			CString strOutMessage;
			strOutMessage.Format("%s/Resource 폴더에\n%s_LOD%d.i3s파일이 있는지 확인하세요.",strWeaponName,strWeaponName,i);
			AfxMessageBox(strOutMessage,MB_OK);
		}
	}

	m_strCreatedWeaponName = strWeaponName;

	return true;
}

void cDlgCreateNewWeapon::OnBnClickedButtonCreateNewWeapon()
{
	CString strOutMessage;
	strOutMessage.Format("새로 생성하기전에 무기폴더를 지정하세요. \n또한 Weapon/무기이름/Resource 폴더에\n무기이름_LOD0.i3s\n무기이름_LOD1.i3s\n무기이름_LOD2.i3s\n파일이 있어야 합니다.");
	AfxMessageBox(strOutMessage,MB_OK);

	CString strPrevPath = g_strRootPath;
	CString strCurrPath = g_strRootPath;//+"\\Weapon";
	
	GUTIL::SetWorkingFolder(false, (LPTSTR)(LPCTSTR)strCurrPath);
	
	CString strSelectPath = g_strRootPath;
	g_strRootPath = strPrevPath;
	if (false == CheckResourceFiles(strSelectPath))
	{
		AfxMessageBox("무기 생성에 실패 하였습니다",MB_OK);
		return;
	}
	
	if( g_pChara != NULL)
		g_pWeaponSCFramework->RemoveChild(g_pChara->geti3Chara());

	I3_SAFE_RELEASE(g_pChara);

	g_pChara = CChara::new_object();

	g_pChara->SetName((const char*)m_strCreatedWeaponName);
	g_pChara->geti3Chara()->SetName((const char*)m_strCreatedWeaponName);

	for (size_t i = 0; i < m_vNewResourceLOD.size(); ++i)
	{
		CString strLODFullPath = strSelectPath + "\\Resource\\" + m_vNewResourceLOD[i];
		g_pChara->LoadSceneGraph(strLODFullPath);
	}

	for (size_t i = 0; i < m_vNewAnimation.size(); ++i)
	{
		CString strAnimFullPath = strSelectPath + "\\Resource\\" + m_vNewAnimation[i];
		g_pChara->LoadAnim(strAnimFullPath);
	}

	CMainFrame*		pFrame	= AfxGetMainFrame();
	cDlgWeaponEditor* pDlg = pFrame->GetDlgWeaponEditor();

	CString strCharEditorName = strSelectPath + "\\" + m_strCreatedWeaponName + ".i3CharaEditor";
	pDlg->SetFilePath(strCharEditorName);

 	if( g_pChara != NULL)
 	{
 		i3SceneObject * pSceneObj = g_pChara->geti3Chara()->getSceneObject();
 		g_pWeaponSCFramework->SetSceneGraph( pSceneObj);
 		g_pWeaponSCFramework->AddChild(g_pChara->geti3Chara());
 	}

	I3_TDK_UPDATE_INFO Data;
	memset(&Data,0,sizeof(I3_TDK_UPDATE_INFO));

	Data.m_Event = I3_TDK_UPDATE_EDIT;
	::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM) &Data, NULL);

	strOutMessage.Format("새로운 무기가 생성 되었습니다.\n이름:%s",m_strCreatedWeaponName);
	AfxMessageBox(strOutMessage,MB_OK);
	OnOK();
}
