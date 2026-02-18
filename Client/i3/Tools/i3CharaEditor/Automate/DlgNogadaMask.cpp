
#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgExport.h"
#include "OptionInfo.h"
#include "GlobalVar.h"
#include ".\dlgexport.h"
#include "i3Base/string/ext/extract_filename.h"


//////////////////////////////////////////////////////////////////////////
#ifdef _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////

#include "DlgNogadaMask.h"

#include "ToolAM.h"
#include "ToolAM_Util.h"
#include "ToolAM_Def_CharTool.h"

using namespace ToolAM;
using namespace ToolAM::CharTool;
using namespace ToolAM::Util;


IMPLEMENT_DYNAMIC(CDlgNogadaMask, CDialog)

BEGIN_MESSAGE_MAP(CDlgNogadaMask, CDialog)
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDOK, OnClickedOk)
	ON_BN_CLICKED(IDC_AUTOMAT_MASK_BT_GO, OnAutoGenMaskChr)
	ON_BN_CLICKED(IDC_AUTOMAT_OPERHERO_CHECK_ALL, OnBtn_HeroOper_AllCheck)
	ON_BN_CLICKED(IDC_AUTOMAT_OPERHERO_UNCHECKALL, OnBtn_HeroOper_AllUnCheck)
	ON_BN_CLICKED(IDC_AUTOMAT_MASK_BT_DEL, OnBtn_MaskPath_Del)
	ON_BN_CLICKED(IDC_AUTOMAT_MASK_BT_RESET, OnBtn_MaskPath_Reset)
	ON_BN_CLICKED(IDC_AUTOMAT_MASK_BT_LODFILENAME_EVAL, OnBtn_Mask_LodFileNameEval)
	ON_BN_CLICKED(IDC_AUTOMAT_MASK_BT_LODFILENAME_PRESET, OnBtn_Mask_LodFileNamePatternPreset)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


//
// Popup
//

INT32 ShowPopup_MaskPatternPreset(CWnd* parent, CPoint posPopup)
{
	CMenu menu;
	UINT32 cmd;
	char szBuf[MAX_PATH] = {0};

	menu.CreatePopupMenu();
	menu.AppendMenu( MF_STRING, 1, ToolAM::CharTool::MaskLODFileNamePattern[0]);

	sprintf_s(szBuf, MAX_PATH, "%s(마스크이름이 Mask_로시작할떄)", ToolAM::CharTool::MaskLODFileNamePattern[1]);
	menu.AppendMenu( MF_STRING, 2, szBuf);
	//sprintf_s(szBuf, MAX_PATH, "%s", ToolAM::CharTool::MaskLODFileNamePattern[2]);
	//menu.AppendMenu( MF_STRING, 3, szBuf);


//	OnQueryPopupMenu(&menu);

	cmd = menu.TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION 
		| TPM_VERTICAL | TPM_RETURNCMD, posPopup.x, posPopup.y, (CWnd *)parent, NULL);

	if(cmd == 0)
		return -1;
	
	return cmd-1;
}


//
// CDlgNogadaMask::MCtlFileDropTarget::CallBack
//

void CDlgNogadaMask::MCtlFileDropTarget_Reset(void* arg)
{
	
}

bool CDlgNogadaMask::MCtlFileDropTarget_OnIterateDropFiles(void* arg, 
	INT32 totalCnt, INT32 cur, bool isDirectory, const char* path)
{
	if(arg == m_maskDropTarget)
		return ProcessDropFileForMask(totalCnt, cur, isDirectory, path);

	return false;
}

void CDlgNogadaMask::MCtlFileDropTarget_OnIterateAfter(void* arg, INT32 num)
{

}

void CDlgNogadaMask::ToolAM_CmdQueue_Report(DWORD errFlag, ToolAM::CmdQueue::Data& cmdData, 
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

	m_log.AddString(buf);
}

void CDlgNogadaMask::ToolAM_CmdQueue_Report2(const char* message)
{
	m_log.AddString(message);
}


//
// CDlgNogadaMask
//

CDlgNogadaMask::CDlgNogadaMask(CWnd* pParent)
: CDialog(CDlgNogadaMask::IDD, pParent)
{
	memset(m_ownerWorkDir, 0, sizeof(m_ownerWorkDir));
	m_maskDropTarget = NULL;
}

CDlgNogadaMask::~CDlgNogadaMask()
{
}

void CDlgNogadaMask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUTOMAT_EB_WORKDIR, m_ebWorkDir);
	DDX_Control(pDX, IDC_AUTOMAT_LB_LOG, m_log);

	const static INT32 ctls[ECHR_HERO_MAX] = 
	{
		IDC_AUTOMAT_OPERHERO_ACIDPOL, 
		IDC_AUTOMAT_OPERHERO_BELLA,
		IDC_AUTOMAT_OPERHERO_CHOU,
		IDC_AUTOMAT_OPERHERO_DFOX,
		IDC_AUTOMAT_OPERHERO_HIDE,
		IDC_AUTOMAT_OPERHERO_KEENEYES,
		IDC_AUTOMAT_OPERHERO_LEOPARD,
		IDC_AUTOMAT_OPERHERO_REDBULLS,
		IDC_AUTOMAT_OPERHERO_TARANTULA,
		IDC_AUTOMAT_OPERHERO_VIPERRED
	};

	for(INT32 i=0; i<ECHR_HERO_MAX; i++)
		DDX_Control(pDX, ctls[i], m_ckOperHero[i]);	
}

void CDlgNogadaMask::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}


// CDlgNogadaMask message handlers

BOOL CDlgNogadaMask::OnInitDialog()
{
	CDialog::OnInitDialog();

	CmdQueue* queue = ToolAM::CmdQueue::GetQ(ToolAM::CharTool::QNAME);
	queue->SetReportCallBack((ToolAM::CmdQueue::ReportCallBack*)this);

	m_ebWorkDir.SetWindowText(m_ownerWorkDir); 

	for(INT32 i=0; i<ECHR_HERO_MAX; i++)
		m_ckOperHero[i].SetCheck(TRUE);

	m_maskDropTarget = new ToolAM::MCtlFileDropTarget(this);
	m_maskDropTarget->Create(GetDlgItem(IDC_AUTOMAT_MASK_ST_DROPMASKDIR),
		WS_VISIBLE | WS_CHILD, "Drop Directory!",  this, IDC_AUTOMAT_MASK_IMPL_DROPMASKDIR);


	GetDlgItem(IDC_AUTOMAT_MASK_EB_LODFILENAMEPAT)->SetWindowText(
		ToolAM::CharTool::MaskLODFileNamePattern[0]);
	OnBtn_Mask_LodFileNameEval();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNogadaMask::OnClickedOk()
{
	DestroyWindow();
}

void CDlgNogadaMask::OnAutoGenMaskChr()
{
	m_log.ResetContent();

	CListBox* lb = (CListBox*)GetDlgItem(IDC_AUTOMAT_MASK_LB_PATH);

	if (lb->GetCount() == 0)
		return;

	i3TDK::SetPendingUpdateState(TRUE);

	CmdQueue* queue = ToolAM::CmdQueue::GetQ(ToolAM::CharTool::QNAME);

	std::vector<std::string> bufMaskPath;

	for (INT32 i = 0; i<lb->GetCount(); i++)
	{
		CString strName; lb->GetText(i, strName);
		bufMaskPath.push_back(std::string(strName.GetString()));
	}

	//char* bufMaskPath[] = 
	//{
	//	"Mask_Soccer_Canary",
	//	"Mask_Soccer_Eagle",
	//	"Mask_Soccer_Lion",
	//	"Mask_Soccer_Storm"
	//};

	INT32 numDoMask = bufMaskPath.size();
	const static char* subpath = "equip/mask";
	char workPath[MAX_PATH] = { 0 }; strcpy_s(workPath, MAX_PATH, m_ownerWorkDir);

	DoNormalizePath(workPath);

	std::vector<std::string> heroNames;
	GetCheckedHeroNames(heroNames);

	char bufMaskName[MAX_PATH];
	char bufRootName[MAX_PATH];
	char bufHeroName[MAX_PATH];
	char bufSavePath[MAX_PATH];

	for (INT32 k = 0; k<numDoMask; k++)
	{
		sprintf_s(bufMaskName, MAX_PATH, "%s", bufMaskPath[k].c_str());

		//////////////////////////////////////////////////////////////////////////
		for (INT32 i = 0; i<(INT32)heroNames.size(); i++)
		{
			queue->BeginTransaction(bufMaskName);

			sprintf_s(bufRootName, MAX_PATH, "%s_%s", heroNames[i].c_str(), bufMaskName);

			sprintf_s(bufHeroName, MAX_PATH, "%s", heroNames[i].c_str());
			sprintf_s(bufSavePath, MAX_PATH, "%s/%s/%s/%s_%s.I3CharaEditor", workPath, subpath,
				bufMaskName, bufHeroName, bufMaskName);

			DoNormalizePath(bufSavePath);

			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_NEW, CmdArg(bufRootName), CmdArgNull);

			CString maskPat;
			GetDlgItem(IDC_AUTOMAT_MASK_EB_LODFILENAMEPAT)->GetWindowText(maskPat);
			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_ADDLOD_WITH_HERO,
				CmdArg(workPath), CmdArg(subpath), CmdArg(maskPat.GetString()), CmdArg(bufMaskName), CmdArg(bufHeroName), CmdArgNull);

			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_SAVE, CmdArg(bufSavePath), CmdArgNull);

			char bufI3CHRPath[MAX_PATH] = { 0 };

			// mask네임이 Mask로 시작하지않을경우 Mask_ 를 마스크이름앞에 넣어준다

			bool startWithMaskStr = false;
			if (!strnicmp(bufMaskName, "Mask_", strlen("Mask_")))
				startWithMaskStr = true;

			if (startWithMaskStr)
				sprintf_s(bufI3CHRPath, MAX_PATH, "%s/%s/%s/%s_%s.I3CHR", workPath, subpath, bufMaskName, bufHeroName, bufMaskName);
			else
				sprintf_s(bufI3CHRPath, MAX_PATH, "%s/%s/%s/%s_Mask_%s.I3CHR", workPath, subpath, bufMaskName, bufHeroName, bufMaskName);

			DoNormalizePath(bufI3CHRPath);
			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_EXPORT_HERO_MASK, CmdArg(workPath), CmdArg(bufI3CHRPath), CmdArgNull);

			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_NEW, CmdArg(NULL), CmdArgNull);

			queue->EndTransaction();
		}
	}

	TOOLAM_CMDQUEUE_MFC_POST_COMMIT;

	i3TDK::SetPendingUpdateState(FALSE);

	SetForegroundWindow();
}

//void CDlgNogadaMask::GenerateMask(char* pchDirName)
//{
//	CmdQueue* queue = ToolAM::CmdQueue::GetQ(ToolAM::CharTool::QNAME);
//
//	INT32 numDoMask = MASK_NUM_LODFILENAME_PATTERN;
//	const static char* subpath = "equip/mask";
//	char workPath[MAX_PATH] = {0}; strcpy_s(workPath, MAX_PATH, m_ownerWorkDir);
//
//	DoNormalizePath(workPath);
//
//	std::vector<std::string> heroNames;
//	GetCheckedHeroNames(heroNames);
//
//	char bufMaskName[MAX_PATH];
//	char bufRootName[MAX_PATH];
//	char bufHeroName[MAX_PATH];
//	char bufSavePath[MAX_PATH];
//
//	for(INT32 k=0; k<numDoMask; k++)
//	{
//		sprintf_s(bufMaskName, MAX_PATH, "%s", pchDirName);
//
//		//////////////////////////////////////////////////////////////////////////
//		for(INT32 i=0; i<(INT32)heroNames.size(); i++)
//		{
//			queue->BeginTransaction(bufMaskName);
//
//			sprintf_s(bufRootName, MAX_PATH, "%s_%s", heroNames[i].c_str(), bufMaskName);
//
//			sprintf_s(bufHeroName, MAX_PATH, "%s", heroNames[i].c_str());
//			sprintf_s(bufSavePath, MAX_PATH, "%s/%s/%s/%s_%s.I3CharaEditor", workPath, subpath, 
//				bufMaskName, bufHeroName, bufMaskName);
//
//			DoNormalizePath(bufSavePath);
//
//			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_NEW, CmdArg(bufRootName), CmdArgNull);
//
//			CString maskPat;
//			maskPat= ToolAM::CharTool::MaskLODFileNamePattern[k];
//
//			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_ADDLOD_WITH_HERO, 
//				CmdArg(workPath), CmdArg(subpath), CmdArg(maskPat.GetString()), CmdArg(bufMaskName), CmdArg(bufHeroName), CmdArgNull);
//
//			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_SAVE, CmdArg(bufSavePath), CmdArgNull);
//
//			char bufI3CHRPath[MAX_PATH] = {0};
//
//			// mask네임이 Mask로 시작하지않을경우 Mask_ 를 마스크이름앞에 넣어준다
//
//			bool startWithMaskStr = false;
//			if(!strnicmp(bufMaskName, "Mask_", strlen("Mask_")))
//				startWithMaskStr = true;
//
//			if(startWithMaskStr)
//				sprintf_s(bufI3CHRPath, MAX_PATH, "%s/%s/%s/%s_%s.I3CHR", workPath, subpath, bufMaskName, bufHeroName, bufMaskName);
//			else
//				sprintf_s(bufI3CHRPath, MAX_PATH, "%s/%s/%s/%s_Mask_%s.I3CHR", workPath, subpath, bufMaskName, bufHeroName, bufMaskName);
//
//			DoNormalizePath(bufI3CHRPath);
//			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_EXPORT_HERO_MASK, CmdArg(workPath), CmdArg(bufI3CHRPath), CmdArgNull);
//
//			queue->PostCmd(ECMD_CHARTOOL_MF_I3CHR_NEW, CmdArg(NULL), CmdArgNull);
//
//			queue->EndTransaction();
//		}
//	}
//
//	TOOLAM_CMDQUEUE_MFC_POST_COMMIT;
//
//	i3TDK::SetPendingUpdateState( FALSE);
//
//	SetForegroundWindow();
//}


void CDlgNogadaMask::OnDropFiles(HDROP hDropInfo)
{
	POINT ptHit;
	DragQueryPoint(hDropInfo, &ptHit);

	{
		RECT rc[100];

		GetDlgItem(IDC_AUTOMAT_MASK_IMPL_DROPMASKDIR)->GetWindowRect(&rc[0]); ScreenToClient(&rc[0]);

		if(PtInRect(&rc[0], ptHit))
			GetDlgItem(IDC_AUTOMAT_MASK_IMPL_DROPMASKDIR)->SendMessage(WM_DROPFILES, (WPARAM)hDropInfo, 0);
	}
}

void CDlgNogadaMask::OnBtn_HeroOper_AllCheck()
{
	for(INT32 i=0; i<ToolAM::ECHR_HERO_MAX; i++)
		m_ckOperHero[i].SetCheck(1);
}

void CDlgNogadaMask::OnBtn_HeroOper_AllUnCheck()
{
	for(INT32 i=0; i<ToolAM::ECHR_HERO_MAX; i++)
		m_ckOperHero[i].SetCheck(0);	
}

void CDlgNogadaMask::OnBtn_MaskPath_Del()
{
	if(!m_log.GetSelCount())
		return;

	INT32 index = m_log.GetSel(0);
	m_log.DeleteString(index);
}

void CDlgNogadaMask::OnBtn_MaskPath_Reset()
{
	CListBox* lb = (CListBox*)GetDlgItem(IDC_AUTOMAT_MASK_LB_PATH);
	if(lb)
		lb->ResetContent();
	
	m_log.ResetContent();
}

void CDlgNogadaMask::OnBtn_Mask_LodFileNameEval()
{
	CString evalStr;
	GetDlgItem(IDC_AUTOMAT_MASK_EB_LODFILENAMEPAT)->GetWindowText(evalStr);

	std::string rst = EvaluateLODFileName(evalStr.GetString(), "AcidPol", "TestMask", 0);

	GetDlgItem(IDC_AUTOMAT_MASK_ST_LODFILENAME_EVAL)->SetWindowText(rst.c_str());
}

void CDlgNogadaMask::OnBtn_Mask_LodFileNamePatternPreset()
{
	RECT rcRect;
	GetDlgItem(IDC_AUTOMAT_MASK_BT_LODFILENAME_PRESET)->GetWindowRect(&rcRect);

	CPoint pos; pos.x = rcRect.left; pos.y = rcRect.top;
	INT32 index = ShowPopup_MaskPatternPreset(this, pos);

	if(index == -1)
		return;

	GetDlgItem(IDC_AUTOMAT_MASK_EB_LODFILENAMEPAT)->SetWindowText(ToolAM::CharTool::MaskLODFileNamePattern[index]);
	OnBtn_Mask_LodFileNameEval();
}

void CDlgNogadaMask::OnRButtonUp(UINT vKeyStatus, CPoint pos)
{	
}

const char* CDlgNogadaMask::GetWorkDir()
{
	CString str;
	static char workDir[MAX_PATH] = {0};

	m_ebWorkDir.GetWindowText(str);

	if(str.IsEmpty())
		workDir[0] = NULL;
	else
		strcpy_s(workDir, MAX_PATH, str.GetString());

	return workDir;
}

void CDlgNogadaMask::GetCheckedHeroNames(std::vector<std::string>& heroNames)
{
	for(INT32 i=0; i<ECHR_HERO_MAX; i++)
	{
		if(m_ckOperHero[i].GetCheck())
			heroNames.push_back(ToolAM::GetHeroName(i));
	}
}

//
// FileDrop
//

bool CDlgNogadaMask::ProcessDropFileForMask(INT32 totalCnt, INT32 cur, bool isDir, const char* path)
{
	if(totalCnt == 0)
		return false;

	CListBox* lb = (CListBox*)GetDlgItem(IDC_AUTOMAT_MASK_LB_PATH);

	if(cur == 0)
	{
		lb->ResetContent();

		if(!strlen(GetWorkDir()))
		{
			AfxMessageBox("ERR!- Working Directory가 비어있습니다!", MB_ICONERROR);
			SetForegroundWindow();
			return false;
		}
	}

	if(!isDir)
		return false;

	if(!path)
		return false;
	
	std::string workDir = ToolAM::Util::NormalizePath(GetWorkDir());
	workDir += "/equip/mask";

	std::string curPath = ToolAM::Util::NormalizePath(path);

	if(!strnicmp(curPath.c_str(), workDir.c_str(), workDir.length()))
	{
		const char* cp = curPath.c_str() + workDir.size() + 1;
		lb->AddString(cp);
	}
	else
	{
		std::string workDirNotMatchText = std::string("[ERR!] ") + curPath.c_str() + " is not in workdir..";
		lb->AddString(workDirNotMatchText.c_str());
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
#endif // end of if _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////