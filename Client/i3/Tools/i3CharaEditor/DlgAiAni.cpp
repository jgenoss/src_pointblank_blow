// DlgAiAni.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "afxcview.h"

#include "i3CharaEditor.h"

#include "DlgAiAni.h"
#include "GlobalVar.h"
// CDlgAiAni 대화 상자입니다.

//#include <iostream>
//#include <vector.h>
#ifdef _USE_TOOLAM_
#include "ToolAM.h"
#include "ToolAM_Util.h"
#include "ToolAM_Def_CharTool.h"

using namespace ToolAM;
using namespace ToolAM::CharTool;
using namespace ToolAM::Util;
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CDlgAiAni, CDialog)

CDlgAiAni* CDlgAiAni::ms_cur = NULL;

int CDlgAiAni::NotifyBox(HWND hWnd, const char* fmt, ...)
{
	if(ms_cur)
	{
		char tempstr[2048];

		va_list marker;
		va_start(marker, fmt);

		vsprintf(tempstr, fmt, marker);
		va_end(marker);

		CDlgAiAni::ms_cur->AddMsg(tempstr);
	}

	return 0;
}

void CDlgAiAni::AddMsg(const char* msg)
{
	if(!msg)
		return;

	CListBox* lb = (CListBox*)GetDlgItem(IDC_AIANI_LB_LOGMSG);

	lb->AddString(msg);
	lb->SetCurSel(lb->GetCount());
}


CDlgAiAni::CDlgAiAni(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAiAni::IDD, pParent)
{
	m_oldNotifyBoxPFN = SetChraEditor_NotifyBox_PFN(&CDlgAiAni::NotifyBox);
	CDlgAiAni::ms_cur = this;
}

CDlgAiAni::~CDlgAiAni()
{
	SetChraEditor_NotifyBox_PFN(m_oldNotifyBoxPFN);
}

void CDlgAiAni::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ED_PATH_1, m_Edit_1);
	DDX_Control(pDX, IDC_ED_PATH_2, m_Edit_2);
	DDX_Control(pDX, IDC_ED_PATH_4, m_Edit_4);
	DDX_Control(pDX, IDC_AI_LIST, m_AI_List);

	DDX_Control(pDX, IDC_CHK_ENABLE , m_B_Trans_1);
	DDX_Control(pDX, IDC_CHK_ENABLE2, m_B_Trans_2);
	DDX_Control(pDX, IDC_CHK_ENABLE3, m_B_UI_Idle);
}

BOOL CDlgAiAni::OnInitDialog()
{
	CDialog::OnInitDialog();

	//리스트 컨트롤 컬럼 삽입
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertColumn(0, "CUSTOM UI_IDLE 캐릭터명", LVCFMT_LEFT, 188);

	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, "REBEL_Male");
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, "REBEL_Female");
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, "REBEL_Female(ViperRed)");
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, "Leopard");
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, "Hide");
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, "D-Fox");


	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->InsertColumn(0, "PATH", LVCFMT_LEFT, 385);
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->InsertColumn(0, "NAME", LVCFMT_LEFT, 120);
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->InsertColumn(0, "UI_IDLE", LVCFMT_LEFT, 55);
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->InsertColumn(0, "TRANS", LVCFMT_LEFT, 45);
	
	m_B_Trans_1.SetCheck(1);


	m_AI_List.ResetContent();
	CString ItemStr = "";
	
	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		i3AI* AI_Driver = g_pChara->getAI(i)->getAI();
		char FullAIPath[256] = {0,};
		char AINamePath[256] = {0,};
		g_pChara->getAI(i)->GetFullAIPath(FullAIPath, true);
		int Count = 0;
		for(int j = strlen(FullAIPath)-1; j >= 0;--j)
		{
			if(FullAIPath[j] == '/')
			{
				++Count;
				if(Count >= 2)
				{
					for(int k = j+1; k < (int) strlen(FullAIPath);++k)
					{
						AINamePath[k-(j+1)] = FullAIPath[k];
					}
					break;
				}
			}
		}
		bool Flag = false;
		if
		(
			AI_Driver->GetNameString() != "Lower" &&
			AI_Driver->GetNameString() != "Upper" &&
			AI_Driver->GetNameString() != "1PV" &&
			AI_Driver->GetNameString() != "3PV" &&
			AI_Driver->GetNameString() != "ShotGun" &&
			AI_Driver->GetNameString() != "AssaultRifle" &&
			AI_Driver->GetNameString() != "Throwing" &&
			AI_Driver->GetNameString() != "Knife" &&
			AI_Driver->GetNameString() != "SMG" &&
			AI_Driver->GetNameString() != "HandGun" &&
			AI_Driver->GetNameString() != "SniperRifle" && 
			AI_Driver->GetNameString() != "MG" &&
			AI_Driver->GetNameString() != "CIC" &&
			AI_Driver->GetNameString() != "DualHandGun" &&
			AI_Driver->GetNameString() != "DualKnife" &&
			AI_Driver->GetNameString() != "RoketLauncher" &&
			AI_Driver->GetNameString() != "DualSMG" &&
			AI_Driver->GetNameString() != "Mission" &&
			AI_Driver->GetNameString() != "Bow" &&
			AI_Driver->GetNameString() != "AttackIdle" &&
			AI_Driver->GetNameString() != "AttackIdle_Left" &&
			AI_Driver->GetNameString() != "Zoom" &&
			AI_Driver->GetNameString() != "Grenade" &&
			AI_Driver->GetNameString() != "CrossIdle" &&
			AI_Driver->GetNameString() != "CrossIdle_Left"
			
		)
		{
			for(int j = 0;j < m_AI_List.GetCount(); ++j)
			{
				m_AI_List.GetLBText(j,ItemStr);
				
				

				if(ItemStr == AINamePath)
				{
					Flag = true;
					break;
				}
			}
		}
		else
			Flag = true;

		if(!Flag)
			m_AI_List.AddString(AINamePath);
	}

	tagRECT rect;
	m_AI_List.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_AI_List.SetWindowPos(NULL,rect.left,rect.top,rect.right-rect.left,500,NULL);
	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BEGIN_MESSAGE_MAP(CDlgAiAni, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDlgAiAni::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnClickedBtn_OK)
	ON_EN_CHANGE(IDC_ED_PATH_1, &CDlgAiAni::OnEnChangeEdI3sname)
	ON_EN_CHANGE(IDC_ED_PATH_2, &CDlgAiAni::OnEnChangeEdI3sname2)
	ON_BN_CLICKED(IDOK4, &CDlgAiAni::OnBnClickedOk4)
	ON_BN_CLICKED(IDOK5, &CDlgAiAni::OnBnClickedOk5)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_EXCEPT_LIST, &CDlgAiAni::OnLvnItemchangedExceptList)
	ON_BN_CLICKED(IDOK6, &CDlgAiAni::OnBnClickedOk6)
	ON_BN_CLICKED(IDOK7, &CDlgAiAni::OnBnClickedOk7)
	ON_BN_CLICKED(IDOK8, &CDlgAiAni::OnBnClickedOk8)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgAiAni::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_AI_LIST, &CDlgAiAni::OnCbnSelchangeAiList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_EXCEPT_LIST2, &CDlgAiAni::OnLvnItemchangedExceptList2)
	ON_EN_CHANGE(IDC_ED_PATH_4, &CDlgAiAni::OnEnChangeEdPath4)
	ON_BN_CLICKED(IDC_CHK_ENABLE3, &CDlgAiAni::OnBnClickedChkEnable3)
	ON_BN_CLICKED(IDC_CHK_ENABLE, &CDlgAiAni::OnBnClickedChkEnable)
	ON_BN_CLICKED(IDC_CHK_ENABLE2, &CDlgAiAni::OnBnClickedChkEnable2)
	ON_BN_CLICKED(IDOK9, &CDlgAiAni::OnBnClickedOk9)
	ON_EN_CHANGE(IDC_ED_PATH_3, &CDlgAiAni::OnEnChangeEdPath3)
	ON_BN_CLICKED(IDOK11, &CDlgAiAni::OnBnClickedOk11)
	ON_BN_CLICKED(IDC_AIANI_BT_INS_FROM_PRESET, &CDlgAiAni::InsertV1CharaListFromPreset)

	ON_BN_CLICKED(IDOK10, &CDlgAiAni::OnBnClickedOk10)
END_MESSAGE_MAP()


BEGIN_DHTML_EVENT_MAP(CDlgAiAni)
END_DHTML_EVENT_MAP()

void CDlgAiAni::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}


void CDlgAiAni::OnClickedBtn_OK()
{

	CString str;

	GetDlgItemText(IDC_ED_PATH_1, str);
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->InsertItem(0, str);//리스트 컨트롤에 이름 삽입

	//i3TDK::SetPendingUpdateState( TRUE);
	//{

	//	i3ResourceFile file;
	//	char szPath[ MAX_PATH] = "./SWAT_Female.i3CharaEditor";

	//	if( file.Load( szPath) == STREAM_ERR)
	//	{
	//		I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", szPath);
	//		return;
	//	}

	//	{
	//		CChara * pChara;

	//		pChara = (CChara *) file.getKeyObject();
	//		if( !i3::kind_of<CChara*>(pChara)) //->IsTypeOf( CChara::static_meta()) == FALSE)
	//		{
	//			I3PRINTLOG(I3LOG_WARN, "%s 파일은 올바른 i3CharaEditor 파일이 아닙니다.\n", szPath);
	//			return;
	//		}

	//		g_pChara = pChara;
	//		g_pChara->AddRef();

	//		g_pFramework->AddChild( g_pChara->geti3Chara());

	//		g_pChara->geti3Chara()->Instantiate();

	//		i3TDK::SetPendingUpdateState( FALSE);

	//		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, g_pChara);
	//	}
	//}


	//{	
	//	i3ResourceFile file2;
	//	char szPath[ MAX_PATH] = "./SWAT_Male.i3CharaEditor";

	//	if( file2.Load( szPath) == STREAM_ERR)
	//	{
	//		I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", szPath);
	//		return;
	//	}

	//	CChara *pChara = NULL;

	//	pChara = (CChara *) file2.getKeyObject();
	//	if( !i3::kind_of<CChara*>(pChara)) //->IsTypeOf( CChara::static_meta()) == FALSE)
	//	{
	//		I3PRINTLOG(I3LOG_WARN, "%s 파일은 올바른 i3CharaEditor 파일이 아닙니다.\n", szPath);
	//		return;
	//	}
	//	//pChara->LoadAnim("./SWAT_Female.i3CharaEditor");

	//	g_pChara->RemoveAllAnim();
	//	pChara->getAI(0)->getAI();
	//	for(int i = 0;i < pChara->getAnimCount();++i)
	//	{
	//		g_pChara->AddAnim(pChara->getAnim(i));
	//		char Path[128] = {0,};
	//		strcpy(Path, g_pChara->getAnim(i)->GetName());
	//		//strcpy(Path, g_pChara->getAnim(i)->GetName()));
	//		Path[0] = 0;
	//		//g_pChara->getAnim(i)->SetResourcePath();
	//	}
	//	
	//	if(pChara)
	//		I3_SAFE_RELEASE( pChara);
	//}
	////g_pChara->AddAI()

	////pChara->getAI
	//
	////pChara->Rel
	////file.Release();
	////file.clo


	////Chara.Load
	////g_pChara->Load

}
void CDlgAiAni::OnEnChangeEdI3sname()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgAiAni::OnEnChangeEdI3sname2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void CDlgAiAni::OnBnClickedOk4()
{
	CString OriginCharaName = g_pChara->GetFilePath();
	int FlagCount = 0;
	for(int i = OriginCharaName.GetLength()-1;i >= 0;--i)
	{
		if(OriginCharaName.GetAt(i) == '\\')
			++FlagCount;
		if(FlagCount >= 1)
		{
			char Path[256] = {0,};
			for(int m = i+1;m < OriginCharaName.GetLength()-14;++m)
			{
				Path[m-(i+1)] = OriginCharaName.GetAt(m);
			}
			OriginCharaName = Path;
			break;
		}
	}

	CString LogFilePath = OriginCharaName;
	LogFilePath += "_Repire.log";
	FILE* LogFile = fopen(LogFilePath.GetString(),"wt");
	if(!LogFile)
	{
		CharaEditorNotifyBox(GetSafeHwnd(),"로그파일 생성 실패");
		return ;
	}
	i3::string LogContext = "#####################################################################################\n\n\n";


	i3::string MsgContext = "I3 ANIMATION\n\n";
	int AniErrorCount = 0;
	int ErrorCount = 0;
	for(int i = 0;i < g_pChara->getAnimCount();++i)
	{
		char CheckAniName[7] = "CHARA/";
		char Path[128] = {0,};
		i3::string RepairPath = "Chara/";
		CString EditStr;

		if(ListAutoRepierCharaName.GetLength() > 0)
		{
			EditStr = ListAutoRepierCharaName;

		}
		else
			m_Edit_2.GetWindowText(EditStr);

		RepairPath += EditStr;
		strcpy(Path, g_pChara->getAnim(i)->GetName());
		int CheckCount = 0;
		int SaveCount = 0;
		bool RepairFlag = false;
		
		int ExpListCount = 0;
		bool ExpFlag = false;
		ExpListCount = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemCount();
		for(int j = 0;j < ExpListCount; ++j)
		{
			CString ItemText = "Chara/";
			ItemText += ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemText(j,0);
			ItemText += "/";

			ExpFlag = false;
			if(int(strlen(Path)) >= ItemText.GetLength())
			{
				for(int k = 0;k < ItemText.GetLength(); ++k)
				{
					char C = Path[k];
					if(C >= 'a' && C <= 'z')
					{
						C -= 32;
					}
					char C2 = ItemText.GetAt(k);
					if(C2 >= 'a' && C2 <= 'z')
					{
						C2 -= 32;
					}
					if(C != C2)
					{
						ExpFlag = true;
						break;
					}
				}
				if(!ExpFlag)
					break;
			}
			else
				ExpFlag = true;
		}
		if(ExpFlag)
		{
			for(int j = 0;j < int(strlen(Path)); ++ j)
			{
				char C = Path[j];
				if(C >= 'a' && C <= 'z')
				{
					C -= 32;
				}
				if(CheckCount >= 6 || C == CheckAniName[CheckCount])
				{
					CheckCount++;

					if(CheckCount == 6)
					{
						SaveCount = j;
					}
					else if(CheckCount > 6 && Path[j] == '/')
					{
						if(EditStr.GetLength() == j - SaveCount - 1)
						{
							for(int k = SaveCount+1;k < j;++k)
							{
								char C1 = EditStr.GetAt(k - (SaveCount+1));
								char C2 = Path[k];
								if(C1 >= 'a' && C1 <= 'z')
								{
									C1 -= 32;
								}
								if(C2 >= 'a' && C2 <= 'z')
								{
									C2 -= 32;
								}
								if(C1 != C2)
								{
									RepairFlag = true;
									MsgContext+= "\n Error - ";
									MsgContext+= Path;
									break;
								}
							}
						}
						else
						{
							RepairFlag = true;
							MsgContext+= "\n Error - ";
							MsgContext+= Path;
						}
						if(RepairFlag)
						{
							ErrorCount++;
							RepairPath += (Path+j);
						}
						break;
					}
				}
				else
					CheckCount = 0;
			}
		}
		if(RepairFlag)
		{
			LogContext += "I3 ANIMATION : ";
			LogContext += Path;
			LogContext += " -> ";
			LogContext += RepairPath;
			LogContext += "\n";
			g_pChara->getAnim(i)->SetResourcePath(RepairPath.c_str());
		}
	}

	LogContext += "\n\n#####################################################################################\n\n";

	AniErrorCount = ErrorCount;
	LogContext += "\n";
	MsgContext += "\nI3 AI\n";

	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		i3AI* AI_Driver = g_pChara->getAI(i)->getAI();
		for(int j = 0;j < AI_Driver->getAIStateCount();++j)
		{
			char CheckAIName[7] = "CHARA/";
			char AI_Path[256] = {0,};
			strcpy(AI_Path, AI_Driver->getAIState(j)->getAnimName());

			i3::string RepairPath = "Chara/";
			CString EditStr;
			if(ListAutoRepierCharaName.GetLength() > 0)
			{
				EditStr = ListAutoRepierCharaName;

			}
			else
				m_Edit_2.GetWindowText(EditStr);
			RepairPath += EditStr;


			int CheckCount = 0;
			int SaveCount = 0;
			bool RepairFlag = false;
			
			int ExpListCount = 0;
			bool ExpFlag = false;
			ExpListCount = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemCount();

			for(int k = 0;k < ExpListCount; ++k)
			{
				CString ItemText = "Chara/";
				ItemText += ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemText(k,0);
				ItemText += "/";

				ExpFlag = false;

				if(int(strlen(AI_Path)) >= ItemText.GetLength())
				{

					for(int m = 0;m < ItemText.GetLength(); ++m)
					{
						char C = AI_Path[m];
						if(C >= 'a' && C <= 'z')
						{
							C -= 32;
						}
						char C2 = ItemText.GetAt(m);
						if(C2 >= 'a' && C2 <= 'z')
						{
							C2 -= 32;
						}
						if(C != C2)
						{
							ExpFlag = true;
							break;
						}
					}
					if(!ExpFlag)
						break;
				}
				else
					ExpFlag = true;
			}

			CString PrevPath = AI_Driver->getAIState(j)->getAnimName();
			if(ExpFlag)
			{
				for(int k = 0;k < int(strlen(AI_Path)); ++ k)
				{
					char C = AI_Path[k];
					if(C >= 'a' && C <= 'z')
					{
						C -= 32;
					}
					if(CheckCount >= 6 || C == CheckAIName[CheckCount])
					{
						CheckCount++;

						if(CheckCount == 6)
						{
							SaveCount = k;
						}
						else if(CheckCount > 6 && AI_Path[k] == '/')
						{
							if(EditStr.GetLength() == k - SaveCount - 1)
							{
								for(int n = SaveCount+1;n < k;++n)
								{
									char C1 = EditStr.GetAt(n - (SaveCount+1));
									char C2 = AI_Path[n];
									if(C1 >= 'a' && C1 <= 'z')
									{
										C1 -= 32;
									}
									if(C2 >= 'a' && C2 <= 'z')
									{
										C2 -= 32;
									}
									if(C1 != C2)
									{
										RepairFlag = true;
										MsgContext+= "\n Error - ";
										MsgContext+= AI_Path;
										break;
									}
								}
							}
							else
							{
								RepairFlag = true;
								MsgContext+= "\n Error - ";
								MsgContext+= AI_Path;
							}
							if(RepairFlag)
							{
								ErrorCount++;
								RepairPath += (AI_Path+k);
							}
							break;
						}
					}
					else
						CheckCount = 0;
				}
			}
			if(RepairFlag)
			{

				//i3::string a = AI_Driver->getAIState(j)->GetName();
				bool CheckFlag = false;
				CString CheckPath;
				for(int t = 0;t < g_pChara->getAnimCount();++t)
				{
					char Path[128] = {0,};
					strcpy(Path, g_pChara->getAnim(t)->GetName());
					CheckPath = g_pChara->getAnim(t)->GetName();
					if(strlen(Path) == RepairPath.length())
					{
						bool CheckFlag2 = false;
						for(int m = 0;m < int(strlen(Path)); ++ m)
						{
							char C = Path[m];
							if(C >= 'a' && C <= 'z')
							{
								C-=32;
							}
							char C2 = RepairPath.at(m);
							if(C2 >= 'a' && C2 <= 'z')
							{
								C2-=32;
							}
							if(C != C2)
							{
								CheckFlag2 = true;
								break;
							}
						}
						if(!CheckFlag2)
						{
							CheckFlag = true;
							char FullAIPath[256] = {0,};
							g_pChara->getAI(i)->GetFullAIPath(FullAIPath);

							LogContext += "I3 AI : ";
							LogContext += FullAIPath;
							LogContext += "\nPATH ";
							LogContext += PrevPath.GetString();
							LogContext += " -> ";
							LogContext += RepairPath.c_str();
							LogContext += "\n\n";
							AI_Driver->getAIState(j)->SetAnim(g_pChara->getAnim(t));
							AI_Driver->getAIState(j)->setAnimName(RepairPath.c_str());
							break;
						}
					}
					
				}
				if(!CheckFlag)
				{

					char FullAIPath[256] = {0,};
					g_pChara->getAI(i)->GetFullAIPath(FullAIPath);

					LogContext += "I3 AI : ";
					LogContext += FullAIPath;
					LogContext += "\nPATH ";
					LogContext += PrevPath;
					LogContext += " -> ";
					LogContext += RepairPath.c_str();
					LogContext += "\n(애니메이션 파일이 존재하지 않아 PATH 만변경 ANI정보는 NULL 상태)\n\n";

					AI_Driver->getAIState(j)->SetAnim(NULL);
					AI_Driver->getAIState(j)->setAnimName(RepairPath.c_str());
				}

				
				
			}

			/*AI_Driver->getAIState(j)->setAnimName("Test");
			strcpy(AI_Path, AI_Driver->getAIState(j)->GetName());
			AI_Path[0] = 0;*/
		}
	}

	LogContext += "\n#####################################################################################\n\n\n";

	int AI_NULL_ERROR_COUNT = 0;


	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		AI* AI_Driver = g_pChara->getAI(i);

		for(int j = 0;j < AI_Driver->getAI()->getAIStateCount();++j)
		{
			if(!AI_Driver->getAI()->getAIState(j)->getAnim())
			{
				if(AI_Driver->getAI()->getAIState(j)->getAnimNameString().length() > 0)
				{
					i3AnimationResManager * pManager = (i3AnimationResManager *) i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta());
					i3Animation * pAnim = (i3Animation*) pManager->GetResource( NULL, NULL, AI_Driver->getAI()->getAIState(j)->getAnimName());
					
					if(!pAnim)
					{
						CString OrginPath = g_pChara->GetFilePath();
						int FlagCount2 = 0;
						for(int k = OrginPath.GetLength()-1;k >= 0;--k)
						{
							if(OrginPath.GetAt(k) == '\\')
								++FlagCount2;
							if(FlagCount2 >= 3)
							{
								char Path[256] = {0,};
								for(int m = 0;m < k;++m)
								{
									Path[m] = OrginPath.GetAt(m);
								}
								OrginPath = Path;
								break;
							}
						}
						OrginPath += "\\";
						OrginPath += AI_Driver->getAI()->getAIState(j)->getAnimName();

						bool LoadCheck = g_pChara->LoadAnim(OrginPath.GetString());

						if(LoadCheck)
						{
							for(int k = 0;k < g_pChara->getAnimCount();++k)
							{
								if(g_pChara->getAnim(k)->GetNameString() == AI_Driver->getAI()->getAIState(j)->getAnimName())
								{
									pAnim = g_pChara->getAnim(k);
									break;
								}
							}
						}
					}

					if(pAnim)
					{
						AI_NULL_ERROR_COUNT++;

						

						char FullAIPath[256] = {0,};
						g_pChara->getAI(i)->GetFullAIPath(FullAIPath);

						LogContext += "I3 ANIMATION ( ANI NULL ) : ";
						LogContext += FullAIPath;
						LogContext += "\nPATH ";
						LogContext += AI_Driver->getAI()->getAIState(j)->getAnimName();
						LogContext += "\\( ANI 정보 복구 완료 ) \n\n";

						AI_Driver->getAI()->getAIState(j)->SetAnim( pAnim);
					}
					else
					{
						AI_NULL_ERROR_COUNT++;
						char FullAIPath[256] = {0,};
						g_pChara->getAI(i)->GetFullAIPath(FullAIPath);


						LogContext += "I3 ANIMATION ( ANI NULL ) : ";
						LogContext += FullAIPath;
						LogContext += "\nPATH ";
						LogContext += AI_Driver->getAI()->getAIState(j)->getAnimName();
						LogContext += "\\( ANI 정보 복구불가능 PATH 경로 제거 ) \n\n";


						AI_Driver->getAI()->getAIState(j)->setAnimName( "");
					}
				}
			}
		}
	}

	if(ErrorCount > 0)
	{
		MsgContext = "Animation Error :  ";
		char Number[32] = {0};
		itoa(AniErrorCount,Number,10);
		MsgContext += Number; 
		MsgContext += " 개\n";
		MsgContext += "AI Path Error :  ";
		itoa(ErrorCount - AniErrorCount,Number,10);
		MsgContext += Number; 
		MsgContext += " 개\n";

		MsgContext += "AI Path( ANI NULL ) Error :  ";
		itoa(AI_NULL_ERROR_COUNT,Number,10);
		MsgContext += Number; 
		MsgContext += " 개\n수정 완료 되었습니다.";
		CharaEditorNotifyBox(GetSafeHwnd(),MsgContext.c_str());

		LogContext += "#####################################################################################\n";

		fprintf(LogFile,"%s",LogContext.c_str());
		fclose(LogFile);
		CString Cmd = "start ./";
		Cmd += LogFilePath.GetString();
		system(Cmd);
	}
	else
		CharaEditorNotifyBox(GetSafeHwnd(),"에러가 발견되지 않았습니다.");
}

void CDlgAiAni::OnBnClickedOk5()
{
	i3::string MsgContext = "#####################################################################################\n\nI3 ANIMATION\n\n";
	int ErrorCount = 0;

	CString OriginCharaName = g_pChara->GetFilePath();
	int FlagCount = 0;
	for(int i = OriginCharaName.GetLength()-1;i >= 0;--i)
	{
		if(OriginCharaName.GetAt(i) == '\\')
			++FlagCount;
		if(FlagCount >= 1)
		{
			char Path[256] = {0,};
			for(int m = i+1;m < OriginCharaName.GetLength()-14;++m)
			{
				Path[m-(i+1)] = OriginCharaName.GetAt(m);
			}
			OriginCharaName = Path;
			break;
		}
	}

	CString LogFilePath = OriginCharaName;
	LogFilePath += ".log";
	FILE* LogFile = fopen(LogFilePath.GetString(),"wt");
	if(!LogFile)
	{
		CharaEditorNotifyBox(GetSafeHwnd(), "로그파일 생성 실패");
		return ;
	}
	for(int i = 0;i < g_pChara->getAnimCount();++i)
	{
		char CheckAniName[7] = "CHARA/";
		char Path[128] = {0,};
		i3::string RepairPath = "Chara/";
		CString EditStr;

		if(ListAutoRepierCharaName.GetLength() > 0)
		{
			EditStr = ListAutoRepierCharaName;

		}
		else
			m_Edit_2.GetWindowText(EditStr);


		strcpy(Path, g_pChara->getAnim(i)->GetName());
		int CheckCount = 0;
		int SaveCount = 0;

		int ExpListCount = 0;
		bool ExpFlag = false;
		ExpListCount = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemCount();
		for(int j = 0;j < ExpListCount; ++j)
		{
			CString ItemText = "Chara/";
			ItemText += ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemText(j,0);
			ItemText += "/";

			ExpFlag = false;
			if(int(strlen(Path)) >= ItemText.GetLength())
			{

				for(int k = 0;k < ItemText.GetLength(); ++k)
				{
					char C = Path[k];
					if(C >= 'a' && C <= 'z')
					{
						C -= 32;
					}
					char C2 = ItemText.GetAt(k);
					if(C2 >= 'a' && C2 <= 'z')
					{
						C2 -= 32;
					}
					if(C != C2)
					{
						ExpFlag = true;
						break;
					}
				}
				if(!ExpFlag)
					break;
			}
			else
				ExpFlag = true;
		}
		if(ExpFlag)
		{

			for(int j = 0;j < int(strlen(Path)); ++ j)
			{
				char C = Path[j];
				if(C >= 'a' && C <= 'z')
				{
					C -= 32;
				}
				if(CheckCount >= 6 || C == CheckAniName[CheckCount])
				{
					CheckCount++;

					if(CheckCount == 6)
					{
						SaveCount = j;
					}
					else if(CheckCount > 6 && Path[j] == '/')
					{
						if(EditStr.GetLength() == j - SaveCount - 1)
						{
							for(int k = SaveCount+1;k < j;++k)
							{
								char C1 = EditStr.GetAt(k - (SaveCount+1));
								char C2 = Path[k];
								if(C1 >= 'a' && C1 <= 'z')
								{
									C1 -= 32;
								}
								if(C2 >= 'a' && C2 <= 'z')
								{
									C2 -= 32;
								}
								if(C1 != C2)
								{
									ErrorCount++;
									MsgContext+= "\n Error - ";
									MsgContext+= Path;
									break;
								}
							}
						}
						else
						{
							
							ErrorCount++;
							MsgContext+= "\n Error - ";
							MsgContext+= Path;
							break;
						}
						break;
					}
				}
				else
					CheckCount = 0;
			}
		}
	}

	MsgContext += "\n Animation Error : ";
	char Number[32] = {0};
	itoa(ErrorCount,Number,10);
	MsgContext += Number; 
	MsgContext += " 개\n";
	ErrorCount = 0;
	MsgContext += "\n#####################################################################################\n\nI3 AI Path\n";

	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		i3AI* AI_Driver = g_pChara->getAI(i)->getAI();
		for(int j = 0;j < AI_Driver->getAIStateCount();++j)
		{
			char CheckAIName[7] = "CHARA/";
			char AI_Path[256] = {0,};
			strcpy(AI_Path, AI_Driver->getAIState(j)->getAnimName());

			i3::string RepairPath = "Chara/";
			CString EditStr;
			if(ListAutoRepierCharaName.GetLength() > 0)
			{
				EditStr = ListAutoRepierCharaName;

			}
			else
				m_Edit_2.GetWindowText(EditStr);
			RepairPath += EditStr;


			int CheckCount = 0;
			int SaveCount = 0;

			int ExpListCount = 0;
			bool ExpFlag = false;
			ExpListCount = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemCount();

			for(int k = 0;k < ExpListCount; ++k)
			{
				CString ItemText = "Chara/";
				ItemText += ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST))->GetItemText(k,0);
				ItemText += "/";

				ExpFlag = false;

				if(int(strlen(AI_Path)) >= ItemText.GetLength())
				{

					for(int m = 0;m < ItemText.GetLength(); ++m)
					{
						char C = AI_Path[m];
						if(C >= 'a' && C <= 'z')
						{
							C -= 32;
						}
						char C2 = ItemText.GetAt(m);
						if(C2 >= 'a' && C2 <= 'z')
						{
							C2 -= 32;
						}
						if(C != C2)
						{
							ExpFlag = true;
							break;
						}
					}
					if(!ExpFlag)
						break;
				}
				else
					ExpFlag = true;
			}
			if(ExpFlag)
			{

				for(int k = 0;k < int(strlen(AI_Path)); ++ k)
				{
					char C = AI_Path[k];
					if(C >= 'a' && C <= 'z')
					{
						C -= 32;
					}
					if(CheckCount >= 6 || C == CheckAIName[CheckCount])
					{
						CheckCount++;

						if(CheckCount == 6)
						{
							SaveCount = k;
						}
						else if(CheckCount > 6 && AI_Path[k] == '/')
						{
							if(EditStr.GetLength() == k - SaveCount - 1)
							{
								for(int n = SaveCount+1;n < k;++n)
								{
									char C1 = EditStr.GetAt(n - (SaveCount+1));
									char C2 = AI_Path[n];
									if(C1 >= 'a' && C1 <= 'z')
									{
										C1 -= 32;
									}
									if(C2 >= 'a' && C2 <= 'z')
									{
										C2 -= 32;
									}
									if(C1 != C2)
									{
										ErrorCount++;
										char FullAIPath[256] = {0,};
								g_pChara->getAI(i)->GetFullAIPath(FullAIPath);

								MsgContext+= "\n AI Name( ";
								MsgContext+= FullAIPath;
								MsgContext+= " : ";
								MsgContext+= AI_Driver->getAIState(j)->GetName();
								MsgContext+= " )\n Error - ";
								MsgContext+= AI_Path;
								MsgContext+= "\n";
										
										
										break;
									}
								}
							}
							else
							{
								ErrorCount++;
								char FullAIPath[256] = {0,};
								g_pChara->getAI(i)->GetFullAIPath(FullAIPath);

								MsgContext+= "\n AI Name( ";
								MsgContext+= FullAIPath;
								MsgContext+= " : ";
								MsgContext+= AI_Driver->getAIState(j)->GetName();
								MsgContext+= " )\n Error - ";
								MsgContext+= AI_Path;
								MsgContext+= "\n";
							}
							break;
						}
					}
					else
						CheckCount = 0;
				}
			}
		}
	}

	MsgContext += "\nAI Path Error : ";
	memset(Number,0,sizeof(Number));
	itoa(ErrorCount,Number,10);
	MsgContext += Number; 
	MsgContext += " 개\n";
	ErrorCount = 0;

	MsgContext += "\n#####################################################################################\n\nI3 AI Path( ANI NULL )\n";

	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		AI* AI_Driver = g_pChara->getAI(i);

		for(int j = 0;j < AI_Driver->getAI()->getAIStateCount();++j)
		{
			if(!AI_Driver->getAI()->getAIState(j)->getAnim())
			{
				if(AI_Driver->getAI()->getAIState(j)->getAnimNameString().length() > 0)
				{
					ErrorCount++;

					i3AnimationResManager * pManager = (i3AnimationResManager *) i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta());
					i3Animation * pAnim = (i3Animation*) pManager->GetResource( NULL, NULL, AI_Driver->getAI()->getAIState(j)->getAnimName());
					bool FileCheck = false;
					if(!pAnim)
					{
						CString OrginPath = g_pChara->GetFilePath();
						int FlagCount2 = 0;
						for(int k = OrginPath.GetLength()-1;k >= 0;--k)
						{
							if(OrginPath.GetAt(k) == '\\')
								++FlagCount2;
							if(FlagCount2 >= 3)
							{
								char Path[256] = {0,};
								for(int m = 0;m < k;++m)
								{
									Path[m] = OrginPath.GetAt(m);
								}
								OrginPath = Path;
								break;
							}
						}
						OrginPath += "\\";
						OrginPath += AI_Driver->getAI()->getAIState(j)->getAnimName();

						FILE* File = fopen(OrginPath.GetString(),"r");
						if(File)
						{
							FileCheck = true;
							fclose(File);
						}
					}
					if(pAnim || FileCheck)
					{
						
						char FullAIPath[256] = {0,};
						g_pChara->getAI(i)->GetFullAIPath(FullAIPath);

						
						MsgContext+= "\n AI Name( ";
						MsgContext+= FullAIPath;
						MsgContext+= " : ";
						MsgContext+= AI_Driver->getAI()->getAIState(j)->GetName();
						MsgContext+= " )\n Error - ";
						MsgContext+= AI_Driver->getAI()->getAIState(j)->getAnimName();
						MsgContext+= " ( PATH 유효 복구가능 )\n";
					}
					else
					{
						
						char FullAIPath[256] = {0,};
						g_pChara->getAI(i)->GetFullAIPath(FullAIPath);


						MsgContext+= "\n AI Name( ";
						MsgContext+= FullAIPath;
						MsgContext+= " : ";
						MsgContext+= AI_Driver->getAI()->getAIState(j)->GetName();
						MsgContext+= " )\n Error - ";
						MsgContext+= AI_Driver->getAI()->getAIState(j)->getAnimName();
						MsgContext+= " ( PATH NULL 복구 불가능 Repier 시 PATH 제거됨 )\n";
					}
				}
			}
		}
	}
	
	if(ErrorCount > 0)
	{
		MsgContext += "\n\nAI Path( ANI NULL ) Error : ";
		char Number2[32] = {0};
		itoa(ErrorCount,Number2,10);
		MsgContext += Number2; 
		MsgContext += " 개\n\n#####################################################################################\n";
		ErrorCount = 0;
		fprintf(LogFile,"%s",MsgContext.c_str());
		fclose(LogFile);
		Sleep(2000);
		CString Cmd = "start ./";
		Cmd += LogFilePath.GetString();
		system(Cmd);
	}
	else
		CharaEditorNotifyBox(GetSafeHwnd(), "에러가 발견되지 않았습니다.");
}



void CDlgAiAni::OnLvnItemchangedExceptList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}



void CDlgAiAni::OnBnClickedOk6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOpenChara("./SWAT_Female.i3CharaEditor");
}


void CDlgAiAni::OnOpenChara( const char * pszPath)
{
	OnCloseChara();
	i3TDK::SetPendingUpdateState( TRUE);


	i3ResourceFile file;
	//char szPath[ MAX_PATH];

	//NormalizeCmdArg( szPath, pszPath);

	if( file.Load( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", pszPath);
		return;
	}

	{
		CChara * pChara;

		pChara = (CChara *)file.getKeyObject();
		if( !i3::kind_of<CChara*>(pChara)) //->IsTypeOf( CChara::static_meta()) == FALSE)
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일은 올바른 i3CharaEditor 파일이 아닙니다.\n", pszPath);
			return;
		}

		g_pChara = pChara;
		g_pChara->AddRef();

		g_pFramework->AddChild( g_pChara->geti3Chara());

		g_pChara->geti3Chara()->Instantiate();

		i3TDK::SetPendingUpdateState( FALSE);


		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, g_pChara);
	}

	i3::safe_string_copy( m_szFilePath, pszPath, MAX_PATH );
	g_pChara->SetFilePath(m_szFilePath);
}

void CDlgAiAni::OnSaveChara( const char * pszPath)
{
	i3ResourceFile file;

	if( g_pChara == NULL)
		return;

	file.setKeyObject( g_pChara);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "%s 파일을 저장할 수 없습니다.", pszPath);
		return;

	}

#ifdef _USE_TOOLAM_

	i3TDK::SetPendingUpdateState(TRUE);
	CmdQueue* queue = ToolAM::CmdQueue::GetQ(ToolAM::CharTool::QNAME);

	queue->SetReportCallBack((ToolAM::CmdQueue::ReportCallBack*)this);
	queue->BeginTransaction("CDlgAiAniOnSaveChara");

	char bufI3CHRPath[MAX_PATH] = {0};

	char tempName[MAX_PATH] = {0};
	char charaDirName[MAX_PATH] = {0};
	char charaFileNameOnly[MAX_PATH] = {0};

	strcpy_s(tempName, MAX_PATH, pszPath);

	char* p = strrchr(tempName, '.');

	if(p)
		*p = NULL;

	p = strrchr(tempName, '\\');
	if(!p)
		p = strrchr(tempName, '/');

	if(!p)
	{
		CharaEditorNotifyBox(GetSafeHwnd(), "ERR! - Failed to export i3Chr- %s", pszPath);
		return;
	}

	strcpy_s(charaFileNameOnly, MAX_PATH, p+1);

	*p = NULL;

	p = strrchr(tempName, '\\');
	if(!p)
		p = strrchr(tempName, '/');

	if(!p)
	{
		CharaEditorNotifyBox(GetSafeHwnd(), "ERR! - Failed to export i3Chr- %s", pszPath);
		return;
	}

	strcpy_s(charaDirName, MAX_PATH, p+1);		

	char* workPath = g_pOption->m_szWorkPath;
	INT32 length = strlen(workPath);

	if (workPath[length - 1] == '\\' || workPath[length - 1] == '/')
		workPath[length - 1] = NULL;

	sprintf_s(bufI3CHRPath, MAX_PATH, "%s\\Chara\\%s\\%s.I3CHR", workPath, charaDirName, charaFileNameOnly);
	DoNormalizePath(bufI3CHRPath);

	INT32 saveI3S = 0; // i3S는 저장할필요없음
	INT32 saveI3CHR = 1;
	queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_EXPORT_CHARA, CmdArg(workPath), CmdArg(bufI3CHRPath), CmdArg(saveI3S), CmdArg(saveI3CHR), CmdArgNull);

	char bufAIDriverCPPPath[MAX_PATH] = {0};
	sprintf_s(bufAIDriverCPPPath, MAX_PATH, "%s\\Chara\\%s", workPath, charaDirName);
	DoNormalizePath(bufAIDriverCPPPath, '\\');

	queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_DELETE_AIDRIVERCPP, CmdArg(bufAIDriverCPPPath), CmdArgNull);

	queue->EndTransaction();
	TOOLAM_CMDQUEUE_MFC_COMMIT(ToolAM::CharTool::QNAME);

	i3TDK::SetPendingUpdateState(FALSE);
	queue->SetReportCallBack(NULL);

	return;

#endif

}

void CDlgAiAni::OnCloseChara(void)
{
	if( g_pChara != NULL)
	{
		g_pFramework->RemoveChild( g_pChara->geti3Chara());
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, g_pChara);
		I3_SAFE_RELEASE( g_pChara);
	}
}


#ifdef _USE_TOOLAM_

void CDlgAiAni::ToolAM_CmdQueue_Report(DWORD errFlag, ToolAM::CmdQueue::Data& cmdData, 
	const char* message)
{
	char buf[1024] = {0};

	if(errFlag & 0x80000000)
		strcat_s(buf, 1024, "[ERR!]");
	else
		strcat_s(buf, 1024, "[INFO!]");

	INT32 lenPrefix = strlen(buf);

	const char* transactionDesc = cmdData.m_transactionDesc.c_str();
	if(!transactionDesc)
		transactionDesc = "[NoTransactionDesc]";

	const char* cmdStr = ToolAM::CharTool::ConvertCmdStr(cmdData.m_cmd);

	std::string argsStr; 
	ToolAM::GenerateCmdArgStr(cmdData.m_args, argsStr);

	sprintf_s(buf+lenPrefix, 1024 - lenPrefix, "[%s],[%s],[%s]", transactionDesc, cmdStr, argsStr.c_str());
	AddMsg(buf);
}

void CDlgAiAni::ToolAM_CmdQueue_Report2(const char* message)
{
	AddMsg(message);
}

#endif


bool _AIANI_BuildAIList( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	//	List * pList = (List *) pUserData;
	i3::vector<i3PersistantElement*>& List = *reinterpret_cast<i3::vector<i3PersistantElement*>*>(pUserData);

	if( i3::kind_of<i3AIContext*>(pNode)) //->IsTypeOf( i3AIContext::static_meta()))
	{
		INT32 idx;

		// AI context에 대응하는 AI Class를 찾아 등록한다.
		idx = g_pChara->FindAIByAIContext( (i3AIContext *) pNode);
		if( idx != -1)
		{
			List.push_back( g_pChara->getAI( idx));
		}
	}

	return true;
}
static void _MakeName( char * pszStr)
{
	while( *pszStr != 0)
	{
		if( (*pszStr == ':') || (*pszStr == '/'))
			*pszStr = '_';

		pszStr++;
	}
}

void CDlgAiAni::OnBnClickedOk7()
{
	CComboBox* swatmaleAI = (CComboBox*)GetDlgItem(IDC_AI_LIST);
	if(swatmaleAI->GetCurSel() == CB_ERR)
	{
		AfxMessageBox("다른 Chara에 머징할 SWAT_MALE AI 콤보박스에서 골라주세요!");
		return;
	}

	int Count = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemCount();
	if(Count <= 0)
		return;

	CString AI_Name;

	GetDlgItemText(IDC_AI_LIST, AI_Name);

	char PerAIDriverName[256] = {0};

	//bool SearchFlag = false;
	char AI_P_Name[64] = {0,};
	std::vector<CString> PV1_Path;
	std::vector<CString> PV3_Path;
	i3::vector<i3::string> Remove_Path;
	char CurFullPath[256] = { 0, };
	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		if(PV1_Path.size() > 0 && PV3_Path.size() > 0)
			break;
		AI* AI_Driver = g_pChara->getAI(i);
		char FullPath[256] = {0,};
		char Trans[4] = {0,};
		
		g_pChara->getAI(i)->GetFullAIPath(FullPath, true);

		char AI_Temp_Name[256] = {0,};
		char AI_IF_Name[256] = {0,};
		AI_Driver->GetFullAIPath(AI_Temp_Name, true);

		int TempCount = 0;
		for(int j = strlen(AI_Temp_Name)-1; j >= 0;--j)
		{
			if(AI_Temp_Name[j] == '/')
			{
				++TempCount;
				if(TempCount >= 2)
				{
					for(int k = j+1; k < int(strlen(AI_Temp_Name));++k)
					{
						AI_IF_Name[k-(j+1)] = AI_Temp_Name[k];
					}
					break;
				}
			}
		}

		if( !strcmp(AI_Name.GetString() , AI_IF_Name ))
		{
			Remove_Path.push_back(FullPath);
			
			int Index = 0;
			for(int j = strlen(FullPath)-AI_Driver->GetNameString().length()-2;j >= 0;--j)
			{	
				if(FullPath[j] == '/')
				{
					Index = j;
					break;
				}
			}
			for(int j = Index+1;j < int(strlen(FullPath));++j)
			{
				if(FullPath[j] == '/')
				{
					break;
				}
				AI_P_Name[j - (Index+1)] = FullPath[j];
			}
			for(int j = Index-1;j >= Index-3;--j)
			{
				Trans[2-((Index-1)-j)] = FullPath[j];
			}
			if(!strcmp(Trans,"1PV"))
			{

				PV1_Path.push_back(FullPath);
					
				i3ResourceFile file;
				i3::vector<i3PersistantElement*> list;


				{
					i3GameNodeTraversal tr;

					tr.setUserData( &list);
					tr.setUserProc( _AIANI_BuildAIList);
					tr.Traverse( AI_Driver->getAIContext());
				}

				file.setKeyObject( AI_Driver);
				file.SetObjects( list);

				for(size_t j = 0;j < list.size();++j)
				{
					i3AI* AI_Driver2 = ((AI*)list[j])->getAI();
					for(int k = 0;k < AI_Driver2->getAIStateCount();++k)
					{
						CString AniName = AI_Driver2->getAIState(k)->getAnimName();
						if(AniName.GetLength() > 0)
						{
							bool Flag = false;
							for(size_t m = 1;m < PV1_Path.size();++m)
							{
								if(PV1_Path[m] == AniName)
								{
									Flag = true;
									break;
								}
							}
							if(!Flag)
								PV1_Path.push_back(AniName);
						}
						
					}
				}

				char CheckPath[256] = {0,};
				for(size_t j = 1;j < PV1_Path.size();++j)
				{
					int FlagCount = 0;
					int Count2 = 0;
					for(int k = 0; k < PV1_Path[j].GetLength();++k)
					{
						if(FlagCount >= 3)
						{
							CheckPath[Count2] = PV1_Path[j].GetAt(k);
							++Count2;
						}
						if(PV1_Path[j].GetAt(k) == '/')
							++FlagCount;
					}
					PV1_Path[j] = CheckPath;
					memset(CheckPath,0,sizeof(CheckPath));
				}

				CString AI_Temp_Path = "C:\\1.i3AI";
				if( file.Save( LPCTSTR( AI_Temp_Path) ) == STREAM_ERR)
				{
					return;
				}
			}
			else if(!strcmp(Trans,"3PV"))
			{


				PV3_Path.push_back(FullPath);
				i3ResourceFile file;
				i3::vector<i3PersistantElement*> list;


				{
					i3GameNodeTraversal tr;

					tr.setUserData( &list);
					tr.setUserProc( _AIANI_BuildAIList);
					tr.Traverse( AI_Driver->getAIContext());
				}

				file.setKeyObject( AI_Driver);
				file.SetObjects( list);

				for(size_t j = 0;j < list.size();++j)
				{
					i3AI* AI_Driver2 = ((AI*)list[j])->getAI();
					for(int k = 0;k < AI_Driver2->getAIStateCount();++k)
					{
						CString AniName = AI_Driver2->getAIState(k)->getAnimName();
						if(AniName.GetLength() > 0)
						{
							bool Flag = false;
							for(size_t m = 1;m < PV3_Path.size();++m)
							{
								if(PV3_Path[m] == AniName)
								{
									Flag = true;
									break;
								}
							}
							if(!Flag)
								PV3_Path.push_back(AniName);
						}

					}
				}
				char CheckPath[256] = {0,};
				for(size_t j = 1;j < PV3_Path.size();++j)
				{
					int FlagCount = 0;
					int Count2 = 0;
					for(int k = 0; k < PV3_Path[j].GetLength();++k)
					{
						if(FlagCount >= 3)
						{
							CheckPath[Count2] = PV3_Path[j].GetAt(k);
							++Count2;
						}
						if(PV3_Path[j].GetAt(k) == '/')
							++FlagCount;
					}
					PV3_Path[j] = CheckPath;
					memset(CheckPath,0,sizeof(CheckPath));
				}
				CString AI_Temp_Path = "C:\\3.i3AI";
				if( file.Save( LPCTSTR(AI_Temp_Path)) == STREAM_ERR)
				{
					return;
				}
			}
			else//??이경우는 잘못됨 4차원 이신가
			{

			
			}
		}
		/*if(PV1_Path.size() > 0 && PV3_Path.size() > 0)
		{
			for(int j = 0;j < g_pChara->getAICount();++j)
			{
				AI* AI_Driver = g_pChara->getAI(j);
				char FullPath[256] = {0,};
				char Trans[4] = {0,};

				g_pChara->getAI(j)->GetFullAIPath(FullPath);
				_MakeName( FullPath);
				if( AI_Driver->GetNameString() != AI_Name.GetString() )
				{
					bool PV3Flag = false;
					if(PV1_Path[0].GetLength() <= strlen(FullPath))
					{
						
						for(int k = 0;k < PV1_Path[0].GetLength();++k)
						{
							if(PV1_Path[0].GetAt(k) != FullPath[k])
							{
								PV3Flag = true;
								break;
							}
						}
					}
					if(PV3Flag)
					{
						PV3Flag = false;
						if(PV3_Path[0].GetLength() <= strlen(FullPath))
						{

							for(int k = 0;k < PV3_Path[0].GetLength();++k)
							{
								if(PV3_Path[0].GetAt(k) != FullPath[k])
								{
									PV3Flag = true;
									break;
								}
							}
						}
						if(PV3Flag)
						{
							
						}
					}
					else
					{

					}
				}
			}
		}*/
		
	}
	for(int i = 0;i < g_pChara->getAICount();++i)
	{
		AI* AI_Driver = g_pChara->getAI(i);
		if(AI_Driver->GetNameString() == AI_P_Name)
		{

			strcpy(PerAIDriverName, AI_Driver->getDriverName());
			break;

		
		}
	}
	//Base Chara Path 
	CString OrginCharaEditPath = g_pChara->GetFilePath();
	CString OrginPath = g_pChara->GetFilePath();
	int FlagCount = 0;
	for(int j = OrginPath.GetLength()-1;j >= 0;--j)
	{
		if(OrginPath.GetAt(j) == '\\')
			++FlagCount;
		if(FlagCount >= 2)
		{
			char Path[256] = {0,};
			for(int k = 0;k < j;++k)
			{
				Path[k] = OrginPath.GetAt(k);
			}
			OrginPath = Path;
			break;
		}
	}

	
	std::vector<CString> CharaFilePath;
	std::vector<CString> CharaName;
	std::vector<CString> CharaIdleFlag;
	std::vector<CString> CharaTrans;

	for(int i = 0;i < Count; ++i)
	{
		CharaTrans.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,0));
		CharaIdleFlag.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,1));
		CharaName.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,2));
		CharaFilePath.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,3));
		
		OnSaveChara(g_pChara->GetFilePath());
		OnOpenChara(CharaFilePath[i]);
		CString CharaFolderPath = CharaFilePath[i];
		//int CutIndex = 0;
		for(int j = CharaFolderPath.GetLength()-1;j >= 0;--j)
		{
			if(CharaFolderPath.GetAt(j) == '\\')
			{
				char Path[256] = {0};
				for(int k = 0;k < j;++k)
				{
					Path[k] = CharaFolderPath.GetAt(k);
				}
				CharaFolderPath = Path;
				break;
			}
		}

		if(CharaIdleFlag[i] == "O")
		{
			for(size_t j = 1;j < PV1_Path.size();++j)
			{
				char LoadPath[256] = {0,};
				strcpy(LoadPath, OrginPath.GetString());
				
				if(CharaTrans[i] == "남자")
				{
					strcat(LoadPath, "\\SWAT_Male\\1PV\\");
				}
				else
				{
					strcat(LoadPath, "\\SWAT_Female\\1PV\\");
				}

				strcat(LoadPath, PV1_Path[j].GetString());

				for(size_t k = 0;k < strlen(LoadPath);++k)
				{
					if(LoadPath[k] == '/')
						LoadPath[k] = '\\';
				}

				g_pChara->LoadAnim(LoadPath);
			}
			for(size_t j = 1;j < PV3_Path.size();++j)
			{
				char LoadPath[256] = {0,};
				strcpy(LoadPath, OrginPath.GetString());

				bool UIIdleFlag = true;
				for(int k = PV3_Path[j].GetLength()-1;k >= 0;--k)
				{
					if(PV3_Path[j].GetAt(k) == '/')
					{
						if(PV3_Path[j].GetLength() - k == 13)
						{
							char UIIdle_Path[14] = "/UI_IDLEA.I3A";
							for(int m = k;m < PV3_Path[j].GetLength();++m)
							{
								if(m - k != 8)
								{
									char c = PV3_Path[j].GetAt(m);
									if(c >= 'a' && c <= 'z')
									{
										c -= 32;
									}
									if(UIIdle_Path[m - k] != c)
									{
										UIIdleFlag = false;
										break;
									}
								}
							}
						}
						else
							UIIdleFlag = false;
						break;
					}
				}
				if(UIIdleFlag)
				{
					strcat(LoadPath, "\\");
					strcat(LoadPath, CharaName[i].GetString());
					strcat(LoadPath, "\\3PV\\");
				}
				else
				{
					if(CharaTrans[i] == "남자")
					{
						strcat(LoadPath, "\\SWAT_Male\\3PV\\");
					}
					else
					{
						strcat(LoadPath, "\\SWAT_Female\\3PV\\");
					}
				}
				strcat(LoadPath, PV3_Path[j].GetString());

				for(size_t k = 0;k < strlen(LoadPath);++k)
				{
					if(LoadPath[k] == '/')
						LoadPath[k] = '\\';
				}

				g_pChara->LoadAnim(LoadPath);
			}
		}
		else
	 	{
			for(size_t j = 1;j < PV1_Path.size();++j)
			{
				char LoadPath[256] = {0,};
				strcpy(LoadPath, OrginPath.GetString());
				if(CharaTrans[i] == "남자")
				{
					strcat(LoadPath, "\\SWAT_Male\\1PV\\");
				}
				else
				{
					strcat(LoadPath, "\\SWAT_Female\\1PV\\");
				}
				strcat(LoadPath, PV1_Path[j].GetString());

				for(size_t k = 0;k < strlen(LoadPath);++k)
				{
					if(LoadPath[k] == '/')
						LoadPath[k] = '\\';
				}

				g_pChara->LoadAnim(LoadPath);
			}
			for(size_t j = 1;j < PV3_Path.size();++j)
			{
				char LoadPath[256] = {0,};
				strcpy(LoadPath, OrginPath.GetString());
				if(CharaTrans[i] == "남자")
				{
					strcat(LoadPath, "\\SWAT_Male\\3PV\\");
				}
				else
				{
					strcat(LoadPath, "\\SWAT_Female\\3PV\\");
				}
				strcat(LoadPath, PV3_Path[j].GetString());

				for(size_t k = 0;k < strlen(LoadPath);++k)
				{
					if(LoadPath[k] == '/')
						LoadPath[k] = '\\';
				}

				g_pChara->LoadAnim(LoadPath);
			}
			
		}
		
		
		//AI변경 셋팅

		AI* AI_1PV = NULL;
		AI* AI_3PV = NULL;

		for(int j = 0;j < g_pChara->getAICount();++j)
		{
			if(AI_1PV && AI_3PV)
				break;
			AI* AI_Driver = g_pChara->getAI(j);
			char FullPath[256] = {0,};
			char Trans[4] = {0,};

			g_pChara->getAI(j)->GetFullAIPath(FullPath, true);


			CString PathTest = AI_Driver->getAI()->GetName();
			if( AI_Driver->getAI()->GetNameString() == AI_P_Name )
			{

				int Index = 0;
				for(int k = strlen(FullPath)-strlen(AI_P_Name)-2;k >= 0;--k)
				{	
					if(FullPath[k] == '/')
					{
						Index = k;
						break;
					}
				}

				for(int k = Index+1;k < Index+4;++k)
				{
					Trans[k-(Index+1)] = FullPath[k];
				}
				if(!strcmp(Trans,"1PV"))
				{
					AI_1PV = AI_Driver;
					
				}
				else if(!strcmp(Trans,"3PV"))
				{
					AI_3PV = AI_Driver;
				}
				else//??이경우는 잘못됨 4차원 이신가
				{


				}
			}
			
		}
		if(!AI_1PV)
		{
			AI * AddPerAI = AI::new_object_ref();
			AddPerAI->setDriverName(PerAIDriverName);
			AddPerAI->SetName(AI_P_Name);

			for(int Ser = 0;Ser < g_pChara->getAICount();++Ser)
			{
				AI* AI_Driver = g_pChara->getAI(Ser);
				if(AI_Driver->GetNameString() == "1PV")
				{
					g_pChara->AddAI( AI_Driver, AddPerAI, true);
					break;
				}
			}
			AI_1PV = AddPerAI;
		}
		if(!AI_3PV)
		{
			AI * AddPerAI = AI::new_object_ref();
			AddPerAI->setDriverName(PerAIDriverName);
			AddPerAI->SetName(AI_P_Name);

			for(int Ser = 0;Ser < g_pChara->getAICount();++Ser)
			{
				AI* AI_Driver = g_pChara->getAI(Ser);
				if(AI_Driver->GetNameString() == "3PV")
				{
					g_pChara->AddAI( AI_Driver, AddPerAI, true);
					break;
				}
			}
			AI_3PV = AddPerAI;
		}
		{
			i3ResourceFile file;

			CString AI_Temp_Path = "C:\\1.i3AI";
			if( file.Load( LPCTSTR( AI_Temp_Path)) == STREAM_ERR)
			{
				return;
			}


			i3PersistantElement * pPersistObj;
			AI * pAI = NULL;

			//for( int k = 0; k < file.GetObjectCount(); k++)
			//{
			//	pPersistObj = file.getKeyObject();
			//	if( i3::kind_of<AI*>(pPersistObj)) //->IsTypeOf( AI::static_meta()))
			//	{
			//		pAI = (AI *) pPersistObj;
			//		g_pChara->CMP_Remove(pAI);
			//	}
			//}
			for (size_t m = 0; m < Remove_Path.size(); ++m)
			{
				g_pChara->FullPath_Remove(Remove_Path[m].c_str());
			}

			for( int k = 0; k < file.GetObjectCount(); k++)
			{
				pPersistObj = file.GetObject( k);

				if( i3::kind_of<AI*>(pPersistObj)) //->IsTypeOf( AI::static_meta()))
				{
					pAI = (AI *) pPersistObj;
		
					i3AI* pI3AI = pAI->getAI();
					for(int m = 0;m < pI3AI->getAIStateCount();++m)
					{
						char AniName[256] = {0,};
						strcpy(AniName, pI3AI->getAIState(m)->getAnimName());
						if(strlen(AniName) > 0)
						{

							
							CString ConvertAniName = "CHARA/";
							int CountFlag = 0;
							for(size_t n = 0;n < strlen(AniName);++n)
							{
								if(AniName[n] == '/')
									++CountFlag;
								if(CountFlag == 2)
								{

									if(CharaTrans[i] == "남자")
									{
										ConvertAniName += "SWAT_MALE";
									}
									else
									{
										ConvertAniName += "SWAT_FEMALE";
									}
									ConvertAniName += (AniName+n);

									
									break;
								}
							}
							bool CheckFlag = false;
							for(int t = 0;t < g_pChara->getAnimCount();++t)
							{
								char Path[128] = {0,};
								strcpy(Path, g_pChara->getAnim(t)->GetName());
								if(int(strlen(Path)) == ConvertAniName.GetLength())
								{
									bool CheckFlag2 = false;
									for(size_t n = 0;n < strlen(Path); ++ n)
									{
										char C = Path[n];
										if(C >= 'a' && C <= 'z')
										{
											C-=32;
										}
										char C2 = ConvertAniName.GetAt(n);
										if(C2 >= 'a' && C2 <= 'z')
										{
											C2-=32;
										}
										if(C != C2)
										{
											CheckFlag2 = true;
											break;
										}
									}
									if(!CheckFlag2)
									{
										CheckFlag = true;
										pI3AI->getAIState(m)->SetAnim(g_pChara->getAnim(t));
										pI3AI->getAIState(m)->setAnimName(ConvertAniName.GetString());
										break;
									}
								}

							}
							if(!CheckFlag)
							{
								CString Context = "해당 애니메이션을 찾을수 없습니다\n";
								Context += ConvertAniName;
								pI3AI->getAIState(m)->SetAnim(NULL);
								pI3AI->getAIState(m)->setAnimName(ConvertAniName.GetString());
								CharaEditorNotifyBox(GetSafeHwnd(), Context);
							}
						}
					}

					//pAI->getAI()->getAIStateCount()
					if( file.getKeyObject() == pPersistObj)
					{
						g_pChara->AddAI( AI_1PV, pAI, true);
					}
					else
					{
						g_pChara->AddAI( NULL, pAI, false);	
					}
				}
			}

			pAI = (AI *) file.getKeyObject();
		}
		{
			i3ResourceFile file;

			CString AI_Temp_Path = "C:\\3.i3AI";
			if( file.Load( LPCTSTR( AI_Temp_Path)) == STREAM_ERR)
			{
				return;
			}


			i3PersistantElement * pPersistObj;
			AI * pAI = NULL;

			for( int k = 0; k < file.GetObjectCount(); k++)
			{
				pPersistObj = file.GetObject( k);

				if( i3::kind_of<AI*>(pPersistObj)) //->IsTypeOf( AI::static_meta()))
				{
					pAI = (AI *) pPersistObj;

					i3AI* pI3AI = pAI->getAI();
					for(int m = 0;m < pI3AI->getAIStateCount();++m)
					{
						char AniName[256] = {0,};
						strcpy(AniName, pI3AI->getAIState(m)->getAnimName());
						if(strlen(AniName) > 0)
						{


							CString ConvertAniName = "CHARA/";
							int CountFlag = 0;
							for(size_t n = 0;n < strlen(AniName);++n)
							{
								if(AniName[n] == '/')
									++CountFlag;
								if(CountFlag == 2)
								{
									if(CharaIdleFlag[i] == "O")
									{
										bool UIIdleFlag = true;
										for(int t = strlen(AniName)-1;t >= 0;--t)
										{
											if(AniName[t] == '/')
											{
												if(strlen(AniName) - t == 13)
												{
													char UIIdle_Path[14] = "/UI_IDLEA.I3A";
													for(size_t q = t;q < strlen(AniName);++q)
													{
														if(q - t != 8)
														{
															char c = AniName[q];
															if(c >= 'a' && c <= 'z')
															{
																c -= 32;
															}
															if(UIIdle_Path[q - t] != c)
															{
																UIIdleFlag = false;
																break;
															}
														}
													}
												}
												else
													UIIdleFlag = false;
												break;
											}
										}
										if(UIIdleFlag)
										{
											ConvertAniName += CharaName[i];

											ConvertAniName += (AniName+n);

										}
										else
										{
											if(CharaTrans[i] == "남자")
											{
												ConvertAniName += "SWAT_MALE";
											}
											else
											{
												ConvertAniName += "SWAT_FEMALE";
											}
											ConvertAniName += (AniName+n);
										}
									}
									else
									{
										if(CharaTrans[i] == "남자")
										{
											ConvertAniName += "SWAT_MALE";
										}
										else
										{
											ConvertAniName += "SWAT_FEMALE";
										}
										ConvertAniName += (AniName+n);

									}
									break;
								}
							}
							bool CheckFlag = false;
							for(int t = 0;t < g_pChara->getAnimCount();++t)
							{
								char Path[128] = {0,};
								strcpy(Path, g_pChara->getAnim(t)->GetName());
								if(int(strlen(Path)) == ConvertAniName.GetLength())
								{
									bool CheckFlag2 = false;
									for(int n = 0;n < int(strlen(Path)); ++ n)
									{
										char C = Path[n];
										if(C >= 'a' && C <= 'z')
										{
											C-=32;
										}
										char C2 = ConvertAniName.GetAt(n);
										if(C2 >= 'a' && C2 <= 'z')
										{
											C2-=32;
										}
										if(C != C2)
										{
											CheckFlag2 = true;
											break;
										}
									}
									if(!CheckFlag2)
									{
										CheckFlag = true;
										pI3AI->getAIState(m)->SetAnim(g_pChara->getAnim(t));
										pI3AI->getAIState(m)->setAnimName(ConvertAniName.GetString());
										break;
									}
								}

							}
							if(!CheckFlag)
							{
								CString Context = "해당 애니메이션을 찾을수 없습니다\n";
								Context += ConvertAniName;
								pI3AI->getAIState(m)->SetAnim(NULL);
								pI3AI->getAIState(m)->setAnimName(ConvertAniName.GetString());
								CharaEditorNotifyBox(GetSafeHwnd(), Context);
							}
						}
					}

					if( file.getKeyObject() == pPersistObj)
					{
						g_pChara->AddAI( AI_3PV, pAI, true);
					}
					else
					{
						g_pChara->AddAI( NULL, pAI, false);
					}
				}
			}

			pAI = (AI *) file.getKeyObject();
		}
	}

	OnSaveChara(g_pChara->GetFilePath());
	OnOpenChara(OrginCharaEditPath);
}

void CDlgAiAni::OnBnClickedOk8()
{

	//int Count = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemCount();
	//if(Count <= 0)
	//	return;

	//std::vector<CString> CharaFilePath;
	//std::vector<CString> CharaName;
	//std::vector<CString> CharaIdleFlag;
	//std::vector<CString> CharaTrans;

	//for(int i = 0;i < Count; ++i)
	//{
	//	CharaTrans.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,0));
	//	CharaIdleFlag.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,1));
	//	CharaName.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,2));
	//	CharaFilePath.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,3));

	//	OnOpenChara(CharaFilePath[i]);
	//	m_Edit_2.GetWindowText(CharaName[i]);
	//	OnBnClickedOk5();
	//	OnBnClickedOk4();
	//	OnSaveChara(g_pChara->GetFilePath());
	//}
	//OnSaveChara(g_pChara->GetFilePath());
	
}

void CDlgAiAni::OnBnClickedButton1()
{
	CFileDialog	Dlg( TRUE, "i3CharaEditor", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3CharaEditor Project File(*.i3CharaEditor)|*.i3CharaEditor||", this);

	if( Dlg.DoModal() != IDOK) return;


	m_Edit_4.SetWindowText( Dlg.GetPathName());
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgAiAni::OnCbnSelchangeAiList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgAiAni::OnLvnItemchangedExceptList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CDlgAiAni::OnEnChangeEdPath4()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgAiAni::OnBnClickedChkEnable3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgAiAni::OnBnClickedChkEnable()
{
	m_B_Trans_1.SetCheck(1);
	m_B_Trans_2.SetCheck(0);
}

void CDlgAiAni::OnBnClickedChkEnable2()
{
	m_B_Trans_1.SetCheck(0);
	m_B_Trans_2.SetCheck(1);
}

void CDlgAiAni::OnBnClickedOk9()
{
	CString str;
	int Index = 0;
	if(	m_B_Trans_1.GetCheck() != 0 )
		Index = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->InsertItem(0, "남자");
	else
		Index = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->InsertItem(0, "여자");

	if(	m_B_UI_Idle.GetCheck() != 0 )
		((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->SetItemText(Index, 1, "O");
	else
		((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->SetItemText(Index, 1, "X");
	
	GetDlgItemText(IDC_ED_PATH_3, str);
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->SetItemText(Index, 2, str);

	GetDlgItemText(IDC_ED_PATH_4, str);
	((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->SetItemText(Index, 3, str);
}

void CDlgAiAni::OnEnChangeEdPath3()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void CDlgAiAni::OnBnClickedOk11()
{
	
	int Count = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemCount();
	if(Count <= 0)
		return;

	CString OrginCharaEditPath = g_pChara->GetFilePath();
	std::vector<CString> CharaFilePath;
	std::vector<CString> CharaName;
	std::vector<CString> CharaIdleFlag;
	std::vector<CString> CharaTrans;


	for(int i = 0;i < Count; ++i)
	{
		CharaTrans.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,0));
		CharaIdleFlag.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,1));
		CharaName.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,2));
		CharaFilePath.push_back( ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,3));

		
		OnOpenChara(CharaFilePath[i]);
		ListAutoRepierCharaName = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2))->GetItemText(i,2);

		OnBnClickedOk5();
		OnBnClickedOk4();

		OnSaveChara(g_pChara->GetFilePath());
		
		
	}
	ListAutoRepierCharaName = "";
	OnOpenChara(OrginCharaEditPath);

}

void CDlgAiAni::InsertV1CharaListFromPreset()
{
	CDlgAiAni_V1Preset dlg;

	const char* workdir = g_pOption->m_szWorkPath;
	strcpy(dlg.m_workDir, workdir);

	if(!dlg.DoModal())
		return;

	CListCtrl* lv = ((CListCtrl*)GetDlgItem(IDC_EXCEPT_LIST2));

	lv->DeleteAllItems();

	INT32 row=0;
	for(size_t i=0; i<dlg.m_rst.size(); i++)
	{
		CDlgAiAni_V1Preset::Dat& cur = dlg.m_rst[i];
	
		lv->InsertItem(row, cur.m_str[0]); // 남자/여자
		lv->SetItemText(row, 1, cur.m_str[1]); // O/X : UI_IDLE
		lv->SetItemText(row, 2, cur.m_str[2]); // BaseCharaName
		lv->SetItemText(row, 3, cur.m_str[3]); // Path
	}

}


//
// CDlgAiAni_V1Preset
// 

IMPLEMENT_DYNAMIC(CDlgAiAni_V1Preset, CDialog)

BEGIN_MESSAGE_MAP(CDlgAiAni_V1Preset, CDialog)

ON_BN_CLICKED(IDCANCEL,						&CDlgAiAni_V1Preset::OnBtnCancel)
ON_BN_CLICKED(IDOK,							&CDlgAiAni_V1Preset::OnBtnOK)
ON_BN_CLICKED(IDC_AIANI_V1P_BT_SET,			&CDlgAiAni_V1Preset::OnBtn_Set)
ON_BN_CLICKED(IDC_AIANI_V1P_BT_UNSET,		&CDlgAiAni_V1Preset::OnBtn_UnSet)
ON_CBN_SELCHANGE(IDC_AIANI_V1P_CB_PRESET,	&CDlgAiAni_V1Preset::OnCB_ChangePreset)

END_MESSAGE_MAP()


CDlgAiAni_V1Preset::CDlgAiAni_V1Preset(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAiAni_V1Preset::IDD, pParent)
{
	m_cbPreset = NULL;
	m_lbPreset = NULL;
}

CDlgAiAni_V1Preset::~CDlgAiAni_V1Preset()
{
	for(INT32 i=0; i<(INT32)m_columns.size(); i++)
		delete m_columns[i];
}

void CDlgAiAni_V1Preset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

#define V1CHARA_PRESET_COL								7

#define V1CHARA_PRESET_CHARANAME						1
#define V1CHARA_PRESET_CUSTUIIDLE						3
#define V1CHARA_PRESET_BASECHARANAME_COL_IDX			4
#define V1CHARA_PRESET_BASE_COL_IDX						5
#define V1CHARA_PRESET_FILENAME_COL_IDX					6

const char* gs_V1CharaPresetData[][V1CHARA_PRESET_COL] =
{
	//Merge	// CharaName						AlterName			Custom UI_IDLE	  		BaseCharaName					Base							i3CharaEditor FileName						
//	"X",	"SWAT_Male",						"ACIDPol",				"O",				"SWAT_Male",					"SWAT_Male",					"SWAT_Male"
	"O",	"SWAT_Female",						"KeenEyes",				"O",				"SWAT_Female",					"SWAT_Female",					"SWAT_Female",
	"O",	"SWAT_Male_GRS",					"  ",					"X",				"SWAT_Male",					"SWAT_Male",					"SWAT_Male_GRS",
	"O",	"SWAT_Male_Madness",				"  ",					"X",				"SWAT_Male",					"SWAT_Male",					"SWAT_Male_Madness",
	"O",	"SWAT_Female_GRS",					"  ",					"X",				"SWAT_FEMALE",					"SWAT_Female",					"SWAT_Female_GRS",
	"O",	"SWAT_Female_Madness",				"  ",					"X",				"SWAT_FEMALE",					"SWAT_Female",					"SWAT_Female_Madness",
	"O",	"REBEL_Female",						"TaranTula",			"O",				"REBEL_Female",					"SWAT_Female",					"REBEL_Female",
	"O",	"REBEL_Female(Tarantula_Soccer)",	"  ",					"X",				"REBEL_Female",					"SWAT_Female",					"REBEL_Soccer",
	"O",	"REBEL_Female(ViperRed)",			"ViperRed",				"O",				"REBEL_FEMALE(VIPERRED)",		"SWAT_Female",					"REBEL_ViperRed",
	"O",	"REBEL_Female(ViperRed_Shadow)",	"  ",					"X",				"REBEL_FEMALE(VIPERRED)",		"SWAT_Female",					"REBEL_ViperRed_Shadow",
	"O",	"REBEL_Female_GRS",					"  ",					"X",				"REBEL_Female",					"SWAT_Female",					"REBEL_Female_GRS",
	"O",	"REBEL_Female(ViperRed_Kopassus)",	"  ",					"X",				"REBEL_Female",					"SWAT_Female",					"REBEL_ViperRed_Kopassus",
	"O",	"Bella",							"  ",					"X",				"REBEL_Female",					"SWAT_Female",					"Bella",
	"O",	"REBEL_Male",						"RedBulls",				"O",				"REBEL_Male",					"SWAT_Male",					"REBEL_Male",
	"O",	"REBEL_Male_GRS",					"  ",					"X",				"REBEL_Male",					"SWAT_Male",					"REBEL_Male_GRS",
	"O",	"Leopard",							"  ",					"O",				"Leopard",						"SWAT_Male",					"Leopard",
	"O",	"Leopard_Bope",						"  ",					"X",				"Leopard",						"SWAT_Male",					"Leopard_Bope",
	"O",	"Leopard_Madness",					"  ",					"X",				"Leopard",						"SWAT_Male",					"Leopard_Madness",
	"O",	"Hide",								"  ",					"O",				"Hide",							"SWAT_Female",					"Hide",
	"O",	"Hide_Kopassus",					"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_Kopassus",
	"O",	"Hide_Recon",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_Recon",
	"O",	"Hide_Soccer",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_Soccer",
	"O",	"Hide_Black",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_Black",
	"O",	"Hide_Madness",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_Madness",
	"O",	"Chou",								"  ",					"X",				"Hide",							"SWAT_Female",					"Chou",
	"O",	"D-Fox",							"  ",					"O",				"D-Fox",						"SWAT_Male",					"D-Fox",
	"O",	"Chou_FBI",							"  ",					"X",				"Hide",							"SWAT_Female",					"Chou_FBI",
	"O",	"Bella_Belly",						"  ",					"X",				"REBEL_Female",					"SWAT_Female",					"Bella_Belly",
	"O",	"Hide_General",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_General",
	"O",	"REBEL_Female(ViperRed_General)",	"  ",					"X",				"REBEL_FEMALE(VIPERRED)",		"SWAT_Female",					"REBEL_ViperRed_General",
	"O",	"Hide_Gign",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_Gign",
	"O",	"REBEL_Female(ViperRed_Matilda)",	"  ",					"X",				"REBEL_FEMALE(VIPERRED)",		"SWAT_Female",					"REBEL_ViperRed_Matilda",
	"O",	"REBEL_Female(Tarantula_Somalian-Pirate)","  ",				"X",				"REBEL_Female",					"SWAT_Female",					"Tarantula_Somalian-Pirate",
	"O",	"SWAT_Female_SWAT-Sniper",			"  ",					"X",				"SWAT_FEMALE",					"SWAT_Female",					"SWAT_Female_SWAT-Sniper",
	"O",	"REBEL_Female(ViperRed_NBgirl)",	"  ",					"X",				"REBEL_Female",					"SWAT_Female",					"REBEL_ViperRed_NBgirl",
	"O",	"Hide_NBgirl",						"  ",					"X",				"Hide",							"SWAT_Female",					"Hide_NBgirl",

};

const INT32 g_numV1CharaPresetData = sizeof(gs_V1CharaPresetData) / sizeof(char*) / V1CHARA_PRESET_COL;



BOOL CDlgAiAni_V1Preset::OnInitDialog()
{
	CDialog::OnInitDialog();

	//IDC_AIANI_V1P_LB_PRESET
	//IDC_AIANI_V1P_CB_PRESET

	GetDlgItem(IDC_AIANI_V1P_ST_WORKPATH)->SetWindowText(m_workDir);

	m_cbPreset = (CComboBox*)GetDlgItem(IDC_AIANI_V1P_CB_PRESET);
	m_lbPreset = &(((CListView*)GetDlgItem(IDC_AIANI_V1P_LB_PRESET))->GetListCtrl());

	m_lbPreset->SetExtendedStyle(LVS_EX_FULLROWSELECT);
//	ListView_SetExtendedListViewStyle(m_lbPreset, LVS_EX_FULLROWSELECT);

	m_cbPreset->ResetContent();
	m_cbPreset->AddString("All");
	m_cbPreset->AddString("SWAT_Male");
	m_cbPreset->AddString("SWAT_Female");
	m_cbPreset->SetCurSel(0);

	// listview	
	const char* labels[V1CHARA_PRESET_COL] = { "Merge", "CharaName", "AlterName", "Custom UI_IDLE", "Base CharaName", "Base", "CharaFileName" };
	const INT32 labelWidth[] = { 50, 200, 60, 60, 160, 100, 150 };
	const static INT32 numLabel = V1CHARA_PRESET_COL; // sizeof(labels) / sizeof(labels[0]);

	for(INT32 i=0; i<numLabel; i++)
	{
		LVCOLUMN* ins = new LVCOLUMN;
		memset(ins, 0, sizeof(LVCOLUMN));

		ins->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		ins->fmt = LVCFMT_LEFT;
		ins->cx = labelWidth[i];
		ins->pszText = (LPSTR)labels[i];
		ins->cchTextMax = strlen(labels[i]);
		ins->iSubItem = i;

		m_columns.push_back(ins);
		m_lbPreset->InsertColumn(i, m_columns[i]);
//		m_lbPreset->SetColumn(i, m_columns[i]);
	}

	SetFilter();
	return TRUE;
}

void CDlgAiAni_V1Preset::RefreshRow(i3::vector<INT32>* rows)
{
	m_lbPreset->DeleteAllItems();

	INT32 curSel = m_cbPreset->GetCurSel();
	INT32 insRow = 0;

	if(curSel != CB_ERR)
	{
		// fill data
		for(INT32 i=0; i<g_numV1CharaPresetData; i++)
		{
			if(rows)
			{
				bool found = false;

				for(size_t k=0; k<rows->size(); k++)
				{
					if( (*rows)[k] == i)
					{
						found = true;
						break;
					}
				}

				if(!found)
					continue;				
			}

			LV_ITEM lvItem;

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = insRow;

			lvItem.iSubItem = 0;
			lvItem.pszText = (LPSTR)gs_V1CharaPresetData[i][0];
			m_lbPreset->InsertItem(&lvItem);

			m_lbPreset->SetItemText(insRow, 1,  gs_V1CharaPresetData[i][1]);
			m_lbPreset->SetItemText(insRow, 2,  gs_V1CharaPresetData[i][2]);
			m_lbPreset->SetItemText(insRow, 3,  gs_V1CharaPresetData[i][3]);
			m_lbPreset->SetItemText(insRow, 4,  gs_V1CharaPresetData[i][4]);
			m_lbPreset->SetItemText(insRow, 5,  gs_V1CharaPresetData[i][5]);
			m_lbPreset->SetItemText(insRow, 6,  gs_V1CharaPresetData[i][6]);

			insRow++;
		}
	}
}

void CDlgAiAni_V1Preset::OnBtnCancel()
{
	OnCancel();
}

void CDlgAiAni_V1Preset::OnBtnOK()
{
	for(INT32 i=0; i<m_lbPreset->GetItemCount(); i++)
	{
		if(stricmp(m_lbPreset->GetItemText(i, 0), "O"))
			continue;

		Dat d;
		strcpy_s(d.m_str[0], MAX_PATH, m_lbPreset->GetItemText(i, V1CHARA_PRESET_BASE_COL_IDX));
		
		if(!stricmp(d.m_str[0], "SWAT_Male"))
			strcpy_s(d.m_str[0], MAX_PATH, "남자");
		else
			strcpy_s(d.m_str[0], MAX_PATH, "여자");

		strcpy_s(d.m_str[1], MAX_PATH, m_lbPreset->GetItemText(i, V1CHARA_PRESET_CUSTUIIDLE));

		strcpy_s(d.m_str[2], MAX_PATH, m_lbPreset->GetItemText(i, V1CHARA_PRESET_BASECHARANAME_COL_IDX));

		char path[MAX_PATH] = {0};
		CString charaDirName = m_lbPreset->GetItemText(i, V1CHARA_PRESET_CHARANAME);
		CString charaFileName = m_lbPreset->GetItemText(i, V1CHARA_PRESET_FILENAME_COL_IDX);

		sprintf_s(path, MAX_PATH, "%s\\Chara\\%s\\%s.i3CharaEditor", m_workDir, (const char*)charaDirName, (const char*)charaFileName);
		strcpy_s(d.m_str[3], MAX_PATH, path);

		m_rst.push_back(d);
	}

	OnOK();
}

void CDlgAiAni_V1Preset::OnBtn_Set()
{
	POSITION pos = m_lbPreset->GetFirstSelectedItemPosition();
	INT32 row;

	while(pos)
	{
		row = m_lbPreset->GetNextSelectedItem(pos);
		m_lbPreset->SetItemText(row, 0, "O");
	}
}

void CDlgAiAni_V1Preset::OnBtn_UnSet()
{
	POSITION pos = m_lbPreset->GetFirstSelectedItemPosition();
	INT32 row;

	while(pos)
	{
		row = m_lbPreset->GetNextSelectedItem(pos);
		m_lbPreset->SetItemText(row, 0, "X");
	}
}

void CDlgAiAni_V1Preset::OnCB_ChangePreset()
{
	INT32 cur = m_cbPreset->GetCurSel();
	
	if(cur == CB_ERR)
		return;

	char curName[MAX_PATH] = {0};
	m_cbPreset->GetLBText(cur, curName);

	SetFilter(curName);
}

void CDlgAiAni_V1Preset::SetFilter(const char* filterName)
{
	i3::vector<INT32> rows;

	for(INT32 i=0; i<g_numV1CharaPresetData; i++)
	{
		if(filterName && stricmp(filterName, "All"))
		{
			if(!stricmp(gs_V1CharaPresetData[i][V1CHARA_PRESET_BASE_COL_IDX], filterName))
				rows.push_back(i);
		}
		else
		{
			rows.push_back(i);
		}
	}

	RefreshRow(&rows);
}


void CDlgAiAni::OnBnClickedOk10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
