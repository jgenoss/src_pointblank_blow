// DlgWeaponEditor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "..\i3Base\i3ResourceFile.h"
#include "DlgWeaponEditor.h"
#include "DlgAIDriver.h"
#include "DlgCreateNewWeapon.h"
#include "DlgAIEnumList.h"
#include "RSCGen.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/itl/range/generic_empty.h"


#include <list>
using namespace std;		

// cDlgWeaponEditor 대화 상자입니다.

IMPLEMENT_DYNAMIC(cDlgWeaponEditor, CSplitDialog)

bool compare_nocase (string first, string second)
{
	unsigned int i=0;
	while ( (i<first.length()) && (i<second.length()) )
	{
		if (tolower(first[i])<tolower(second[i]))
			return true;
		else if (tolower(first[i])>tolower(second[i]))
			return false;
		++i;
	}

	if (first.length()<second.length())
		return true;
	else
		return false;
}


cDlgWeaponEditor::cDlgWeaponEditor(CWnd* pParent /*=NULL*/)
:	CSplitDialog(cDlgWeaponEditor::IDD, pParent)
,	m_pDlgTimeTrack(NULL)
,	m_pDlgResourceControl(NULL)
,	m_bOnceReset(true)
,	m_bIsVisible(false)
,	m_pAIDriverMng(NULL)
{

}

cDlgWeaponEditor::~cDlgWeaponEditor()
{
	I3_SAFE_RELEASE(m_pAIDriverMng);	// I3_SAFE_DELETE(m_pAIDriverMng);
	I3_SAFE_DELETE(m_pDlgTimeTrack);
	I3_SAFE_DELETE(m_pDlgResourceControl);
}

void cDlgWeaponEditor::DoDataExchange(CDataExchange* pDX)
{
	CSplitDialog::DoDataExchange(pDX);
	
 	DDX_Control(pDX, IDC_WEAPON_RESLIST, m_WeaponResList);
	DDX_Control(pDX, IDC_WEAPON_ANI, m_WeaponAni);
 	DDX_Control(pDX, IDC_WEAPON_RENDERING, m_WeaponRendering);
 	//DDX_Control(pDX, IDC_WEAPON_TRACK, m_WeaponTrack);
	DDX_Control(pDX, IDC_WEAPON_PROP, m_WeaponProp);
	
}


BEGIN_MESSAGE_MAP(cDlgWeaponEditor, CSplitDialog)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_OPEN_RESDLG, &cDlgWeaponEditor::OnBnClickedBtnOpenResdlg)
	ON_BN_CLICKED(IDC_BTN_OPEN_AIDRIVER, &cDlgWeaponEditor::OnBnClickedBtnOpenAidriver)
	ON_BN_CLICKED(IDC_BTN_CREATE, &cDlgWeaponEditor::OnBnClickedBtnCreate)
	ON_BN_CLICKED(IDC_BTN_SAVE, &cDlgWeaponEditor::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_ADD_AI_ENUM, &cDlgWeaponEditor::OnBnClickedBtnAddAiEnum)
END_MESSAGE_MAP()



int cDlgWeaponEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSplitDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(TRUE);

	g_HwndDlgWeaponEditor = GetSafeHwnd();
	return 0;
}

// cDlgWeaponEditor 메시지 처리기입니다.
void cDlgWeaponEditor::OnSize(UINT nType, int cx, int cy)
{
	CSplitDialog::OnSize(nType, cx, cy);

	
	INT32 nDelta = 0;
	RECT rt;
	rt.left		= nDelta;
	rt.right	= cx-nDelta;
	rt.top		= nDelta;
	rt.bottom	= cy-nDelta;
	//g_pViewer->SetWeaponRect(rt);

	RECT rtTmp;
	if (m_WeaponRendering.GetSafeHwnd())
	{
		m_WeaponRendering.GetClientRect(&rtTmp);
		g_pViewer->SetWeaponRect(rtTmp, E_SWAPCHAIN_WEAPON);
	}
	if (m_pDlgTimeTrack)
	{
		RECT rcParent, rcThis;
		GetWindowRect(&rcParent);
		m_pDlgTimeTrack->GetWindowRect(&rcThis);
		m_pDlgTimeTrack->MoveWindow(rcThis.left,
			rcThis.top,
			rcParent.right-rcParent.left,
			rcThis.bottom-rcThis.top);
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void cDlgWeaponEditor::OnSizing(UINT fwSide, LPRECT pRect)
{
	if (g_pViewer->GetMaxW() < pRect->right- pRect->left)
		pRect->right = g_pViewer->GetMaxW() + pRect->left;
	if (g_pViewer->GetMaxH() < pRect->bottom- pRect->top)
		pRect->bottom = g_pViewer->GetMaxH() + pRect->top;

	CSplitDialog::OnSizing(fwSide, pRect);

	m_pDlgTimeTrack->OnMove(GetSafeHwnd());

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void cDlgWeaponEditor::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	list<string>	FileList;
	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);
		FileList.push_back(TempName);
	}

	FileList.sort(compare_nocase);

	for (list<string>::iterator iter = FileList.begin() ; iter != FileList.end() ; ++iter)
	{
		I3TRACE2("%d - %s\n\n", ++i, iter->c_str());
		const char * pName = iter->c_str();

	//	i3String::SplitFileExt( pName, Ext, sizeof(Ext) - 1);
		i3::extract_fileext(pName, Ext);

		if( i3::generic_is_iequal( Ext, "I3S") )
		{
			g_pChara->LoadSceneGraph( pName);
		}
		else if( i3::generic_is_iequal( Ext, "i3A") )
		{
			g_pChara->LoadAnim( pName);
		}
		else if(	i3::generic_is_iequal( Ext, "WAV")  ||
			i3::generic_is_iequal( Ext, "I3SND") )
		{
			g_pChara->LoadSound( pName);
		}
		else if(i3::generic_is_iequal( Ext, "i3CharaEditor"))
		{
			OnLoadCharaEditor(TempName);

		}
	}
 	

	DragFinish( hDropInfo);

}

void cDlgWeaponEditor::OnLoadCharaEditor(const CString& strFolderName)
{
	if (false == m_bIsVisible)
		return;

	if( g_pChara != NULL)
		g_pWeaponSCFramework->RemoveChild(g_pChara->geti3Chara());

	if (false == OnOpenChara( strFolderName))
		return;

	if( g_pChara != NULL)
	{
		i3SceneObject * pSceneObj = g_pChara->geti3Chara()->getSceneObject();
		g_pWeaponSCFramework->SetSceneGraph( pSceneObj);
		g_pWeaponSCFramework->AddChild(g_pChara->geti3Chara());
	}

	//한번만 리셋하는곳~
	if (m_bOnceReset)
	{
		//카메라 리셋은 한번만 한다
		g_pWeaponSCFramework->SetResetCamera(false);
		//g_pItemSCFramework->SetResetCamera(false);
		m_bOnceReset = false;
	}
}

void cDlgWeaponEditor::UpdateDlgList()
{
	m_WeaponResList.UpdateAllRes();
	m_WeaponAni.UpdateAllRes();
}

void cDlgWeaponEditor::SetVisible(const bool& bVisible)
{
	m_bIsVisible = bVisible;

	//SW_SHOW; // 5 	//SW_HIDE; // 0
	int nVisible = bVisible == true ? 5 : 0;
	this->ShowWindow(nVisible);
	m_pDlgTimeTrack->ShowWindow(nVisible);
	//m_pDlgResourceControl->ShowWindow(nVisible);
}

LRESULT cDlgWeaponEditor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		I3_TDK_UPDATE_INFO* pInfo = (I3_TDK_UPDATE_INFO*)wParam;
		if (pInfo->m_Event == I3_TDK_UPDATE_SELECT)
		{
			CString szFolderName = (char*)lParam;

			//CString szFinalPath =  g_strRootPath + "\\Weapon\\" + szFolderName + "\\" + szFolderName + ".i3CharaEditor";
			CString szFinalPath =  g_strRootPath + "\\" + szFolderName;
		
			m_WeaponResList.DeleteAllItems();
			m_WeaponAni.DeleteAllItems();

			OnLoadCharaEditor(szFinalPath);
			UpdateDlgList();
		}
		else if (pInfo->m_Event == I3_TDK_UPDATE_EDIT)
		{
 			UpdateDlgList();
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void cDlgWeaponEditor::OnSetFocus(CWnd* pOldWnd)
{
	CSplitDialog::OnSetFocus(pOldWnd);

 	OnManualSetFocus();

	I3TRACE("cDlgWeaponEditor SetFocus\n");

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void cDlgWeaponEditor::OnMove(int x, int y)
{
	CSplitDialog::OnMove(x, y);
	if (m_pDlgTimeTrack)
		m_pDlgTimeTrack->OnMove(GetSafeHwnd());
}


void cDlgWeaponEditor::OnManualSetFocus()
{
	if (g_pWeaponSCFramework)
	{
		g_pViewer->SetFramework(g_pWeaponSCFramework);
		g_pViewer->SetWndProc(GetRenderHwnd(), false);
	}
}

bool cDlgWeaponEditor::OnOpenChara(const char* pszPath)
{
	OnCloseChara();
	i3NamedResourceManager::Reset();

	i3ResourceFile file;
	char szPath[ MAX_PATH];

	GUTIL::NormalizeCmdArg( szPath, pszPath);

	//char* pszTmp = i3ResourceFile::GetWorkingFolder();
	
	if (i3::generic_empty(pszPath) )
		return false;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		//I3WARN( "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	m_strFilePath = pszPath;

	INT32 nCount = file.GetObjectCount();
 	for( INT32 i = 0;i < nCount; ++i)
 	{
 		i3PersistantElement* pElem = file.GetObject(i);
 
 		//노드의 정보는 모두 삭제해도 될듯하다..
 		if ( i3::kind_of<i3Node*>(pElem) //->IsTypeOf(i3Node::GetClassMeta()) 
			|| i3::kind_of<i3TransformSourceCombiner*>(pElem)) //->IsTypeOf(i3TransformSourceCombiner::GetClassMeta()) )
 		{
 			file.DeleteObject(i);
 			i--;
 		}
 	}

	{
		CChara * pChara;

		pChara = (CChara *) file.getKeyObject();
		if((pChara == NULL) || (!i3::kind_of<CChara*>(pChara))) //->IsTypeOf( CChara::GetClassMeta()) == FALSE))
		{
			I3WARN( "%s 파일은 올바른 i3CharaEditor 파일이 아닙니다.\n", pszPath);
			return false;
		}

		I3_SAFE_RELEASE(g_pChara);
		g_pChara = pChara;
  		g_pChara->AddRef();
  		g_pChara->geti3Chara()->Instantiate();

		//Listener 볼륨이 0인 경우가 있다.. 강제로 100으로 셋팅
		if (0 == g_pChara->getAICount())
			return true;

		AI* pAI = g_pChara->getAI(0);

  		GICDiagramAI* pDigram = (GICDiagramAI*)pAI->getDiagram();
  		for (int i = 0; i < pDigram->getShapeCount(); ++i)
  		{
  			GICShapeAI* pGICShapeAI		= (GICShapeAI*)pDigram->getShape(i);
  			i3AIState* pAIState			= pGICShapeAI->getAIState();
  			i3TimeSequence * pTimeSeq	= pAIState->getTimeSeq();
  			if (pTimeSeq)
  			{
  				for (int nSeqCnt = 0; nSeqCnt < pTimeSeq->GetSequenceCount(); ++nSeqCnt)
  				{
  					i3TimeSequenceInfo* pTimeSeqInfo = pTimeSeq->GetSequence(nSeqCnt);
  					if (i3::kind_of<i3SoundNode*>(pTimeSeqInfo->GetNode())) //->IsTypeOf(i3SoundNode::GetClassMeta()))
  					{
  						i3SoundNode* pSoundNode = (i3SoundNode*)pTimeSeqInfo->GetNode();
  						i3SoundPlayInfo* pSoundInfo = pSoundNode->getSoundPlayInfo();
  						pSoundInfo->setListenerVolume(100);
  					}
  				}
  			}
  		}
	}

	return true;
}



void cDlgWeaponEditor::OnCloseChara()
{
	if( g_pChara != NULL)
	{
		g_pWeaponSCFramework->RemoveChild( g_pChara->geti3Chara());
		I3_SAFE_RELEASE( g_pChara);
	}
}

void cDlgWeaponEditor::OnManualOnSize(const int& cx, const int& cy)
{
	INT32 nDelta = 0;
	RECT rt;
	rt.left		= nDelta;
	rt.right	= cx-nDelta;
	rt.top		= nDelta;
	rt.bottom	= cy-nDelta;
	//g_pViewer->SetWeaponRect(rt);
}

BOOL cDlgWeaponEditor::OnInitDialog()
{
	CSplitDialog::OnInitDialog();

	m_WeaponResList.Create();
	m_WeaponAni.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );	
	m_WeaponAni.Init();
//	m_WeaponAni.SetSelectionMark()
	m_WeaponProp.Init();
	
	//create the panes
	m_PaneMain = CreatePane(CMyPane::ST_VERTICAL);
	m_PaneLeft = CreatePane(CMyPane::ST_HORIZONTAL,m_PaneMain,1);
	m_PaneRight = CreatePane(CMyPane::ST_VERTICAL,m_PaneMain,2);

	//assign the controls
	SetPaneWnd(m_PaneLeft,&m_WeaponResList,1,100,100,300,300);
	SetPaneWnd(m_PaneLeft,&m_WeaponAni,2,100,100,300,300);
	SetPaneWnd(m_PaneRight,&m_WeaponProp,1,100,100,300,300);
	SetPaneWnd(m_PaneRight,&m_WeaponRendering,2,100,100,300,300);
	
	
	m_WeaponResList.SetEventWnd(this);
	m_WeaponTrack.SetEventWnd(this);
	m_WeaponAni.SetEventWnd(this);
	m_WeaponRendering.SetEventWnd(this);

	SetTopOffset(30);
	//SetBottomOffset(50);

	Initialize();

	g_pViewer->SetWeaponHwnd(m_WeaponRendering.GetSafeHwnd());

	RECT rtRender;
	if (m_WeaponRendering.GetSafeHwnd())
	{
		m_WeaponRendering.GetClientRect(&rtRender);
		g_pViewer->SetWeaponRect(rtRender);
	}

	m_pDlgTimeTrack = new cDlgTimeTrack();
	m_pDlgTimeTrack->Create(IDD_WEAPON_TIME_TRACK, this);
	m_pDlgTimeTrack->ShowWindow(SW_HIDE);
	m_pDlgTimeTrack->OnMove(GetSafeHwnd());

 	RECT rcParent, rcThis;
 	GetWindowRect(&rcParent);
 	m_pDlgTimeTrack->GetWindowRect(&rcThis);
 	m_pDlgTimeTrack->MoveWindow(rcThis.left,
 								rcThis.top,
 								rcParent.right-rcParent.left,
 								rcThis.bottom-rcThis.top);

	m_pDlgResourceControl = new cDlgResourceControl();
	m_pDlgResourceControl->Create(IDD_DLG_ADD_RESOURCE, this);
	m_pDlgResourceControl->ShowWindow(SW_HIDE);
	m_pDlgResourceControl->GetClientRect(&rcThis);
	m_pDlgResourceControl->MoveWindow(	rcParent.right,
										rcParent.top,
										rcThis.right-rcThis.left,
										rcParent.bottom-rcParent.top);

	m_pAIDriverMng = cAIDriverManager::new_object(); 
	m_pAIDriverMng->Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void cDlgWeaponEditor::UpdateWeaponProp()
{
	//m_WeaponProp.UpdateAll();
}
BOOL cDlgWeaponEditor::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CSplitDialog::DestroyWindow();
}

void cDlgWeaponEditor::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSplitDialog::OnOK();
}

void cDlgWeaponEditor::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	SetVisible(false);
// 	CSplitDialog::OnCancel();
// 
// 	m_pDlgTimeTrack->EndDialog(0);
// 	m_pDlgResourceControl->EndDialog(0);
}

void cDlgWeaponEditor::OnBnClickedBtnOpenResdlg()
{
	static bool bOnResourceDlg = false;
	bOnResourceDlg = !bOnResourceDlg;
	
	int nID = SW_SHOW;
	bOnResourceDlg == true ? nID = SW_SHOW : nID = SW_HIDE;

	m_pDlgResourceControl->ShowWindow(nID);
}

// cDlgAIDriver 메시지 처리기입니다.

void cDlgWeaponEditor::OnBnClickedBtnOpenAidriver()
{
	cDlgAIDriver dlg;
	dlg.SetAIDriverData(m_pAIDriverMng->GetAIDriverCon());
	dlg.SetAIEnum(m_pAIDriverMng->GetAIEnum());

	dlg.DoModal();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void cDlgWeaponEditor::OnBnClickedBtnCreate()
{
	cDlgCreateNewWeapon dlg;
	dlg.DoModal();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void cDlgWeaponEditor::OnBnClickedBtnSave()
{
	i3ResourceFile file;

	if( g_pChara == NULL)
		return;

	file.setKeyObject( g_pChara);

	if( file.Save( m_strFilePath) == STREAM_ERR)
	{
		I3WARN( "%s 파일을 저장할 수 없습니다.", m_strFilePath.GetString());
		return;
	}
	CString strFileName = m_strFilePath;
	strFileName.Delete(0, strFileName.ReverseFind('\\')+1);
	strFileName.Delete(strFileName.ReverseFind('.'), strFileName.GetLength());

	CString strCHRPath	= g_strRootPath + "\\Weapon" + "\\Weapon" + "\\" +strFileName + ".I3CHR";
	CString strI3S		= g_strRootPath + "\\Weapon" + "\\" + strFileName + "\\" + strFileName + ".i3s";

	g_pChara->Export(strCHRPath, strI3S, strFileName);

	CString strRSCPath = g_strRootPath + "\\Weapon" + "\\Weapon.RSG";
	cRSCGen m_pRSCGen;
	m_pRSCGen.OpenRSCFile(strRSCPath);

	//int nStyle = GetResType( (const char *) strCHRPath);

	RSCNode * pNode = m_pRSCGen._GetTypeRoot(1);

	bool bExist = false;
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		RSCNode* pChild = pNode->GetChild(i);
		CString strCHRFile = strFileName + ".I3CHR";
		if (i3::generic_is_iequal(pChild->GetName(), (const char*)strCHRFile) )
		{
			bExist = true;
			break;
		}
	}

	if (false == bExist)
	{
		m_pRSCGen.AddFile(strCHRPath);
	}

	m_pRSCGen.OnFileSave();

	AfxMessageBox( "파일 저장 완료!");
}

void cDlgWeaponEditor::OnBnClickedBtnAddAiEnum()
{
	cDlgAIEnumList dlg;
	dlg.SetAIDriverData(m_pAIDriverMng->GetAIDriverCon());
	dlg.SetAIEnum(m_pAIDriverMng->GetAIEnum());
	dlg.DoModal();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
