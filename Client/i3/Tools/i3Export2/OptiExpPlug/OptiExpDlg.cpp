
#include "stdafx.h"
#include "resource.h"

#include "OptiExpPlugHandlerImpl.h"
#include "OptiExpSetupOperDlg.h"
#include "IOptiExpSettings.h"

#include "DxRenderContext.h"
#include "OptiExpDlg.h"


class OptiExpDlg
{
public:
	OptiExpDlg(HWND hwnd, OptiExpPlugHandlerImpl* owner);
	~OptiExpDlg();

	enum ECTL
	{
		CB_EXPSETTINGS,
		LV_OPER,
		BT_OPER_SETUP,
		ST_OPER_DESC,
		CK_LOGGINGTOFILE,
		ST_PHYSXCOOKLIB,
		ST_VER,
		CTL_MAX
	};

	void OnInitDialog();
	BOOL OnCmd(WPARAM wParam, LPARAM lParam);
	BOOL OnNotify(WPARAM wParam, LPARAM lParam);

	HMODULE GetExporterDllMouduleInst();

	INT32 GetSelectedExpOperIndex();
	OptiExpTaskVec* GetSelectedExpOperationTask();

	INT32 GetSelectedExpSettingsIndex();
	IOptiExpSettings* GetSelectedExpSettings();

	bool GetExportOperationFileNamePath(INT32 operIndex, char* outPath, char* outName);
	bool RefreshPhysxCookLibInfo();

protected:
	bool Export();
	void SetupOperation();

private:
	void InitCtl(HWND hwnd);
	void OnDestroy();
	
	void ReloadOperListView();
 
private:
	OptiExpPlugHandlerImpl*			m_owner;

	HWND							m_dlg;
	HWND							m_ctl[CTL_MAX];

	static INT32					ms_expSettingsIndex;
	static bool						ms_loggingToFile;
	static bool						ms_notToWarnPhysXCookingLibErr;
};

OptiExpDlg*	gs_optiExpDlg = NULL;

INT32	OptiExpDlg::ms_expSettingsIndex = 0;
bool	OptiExpDlg::ms_loggingToFile = false;
bool	OptiExpDlg::ms_notToWarnPhysXCookingLibErr = false;


BOOL CALLBACK _OptiExpDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{			
			OptiExpPlugHandlerImpl* impl = (OptiExpPlugHandlerImpl*)lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)impl);

			if(gs_optiExpDlg)
			{
				I3ASSERT(0);
				delete gs_optiExpDlg;
			}

			gs_optiExpDlg = new OptiExpDlg(hwnd, impl);
			gs_optiExpDlg->OnInitDialog();
		}
		break;

		case WM_COMMAND:
		{
			if(gs_optiExpDlg)
				return gs_optiExpDlg->OnCmd(wParam, lParam);
		}
		break;

		case WM_NOTIFY:
		{
			if(gs_optiExpDlg)
				return gs_optiExpDlg->OnNotify(wParam, lParam);
		}
		break;

		case WM_DESTROY:
		{
			if(gs_optiExpDlg)
			{
				delete gs_optiExpDlg;
				gs_optiExpDlg = NULL;
			}

			return FALSE;
		}
		break;

		default:
			return FALSE;
	}

	return TRUE;
}

		
			
//
// OptiExpDlg
// 

OptiExpDlg::OptiExpDlg(HWND hwnd, OptiExpPlugHandlerImpl* owner)
{
	m_owner = owner;

	m_dlg = hwnd;
	memset(m_ctl, 0, sizeof(m_ctl));
}

OptiExpDlg::~OptiExpDlg()
{
	OnDestroy();
}

void OptiExpDlg::OnInitDialog()
{
	InitCtl(m_dlg);
	// optiSettings = pExport->GetOptiExpSettings();		

	// oper listview
	const char* lvOperHdrColStr[]			= { "Operation Name", "Desc" };
	const static INT widthLvOperHdrCol[]	= { 300, 600 };
	const static INT32 numLVOperHdrColStr	= sizeof(lvOperHdrColStr) / sizeof(lvOperHdrColStr[0]);

	LVCOLUMN lvOperCol;
	lvOperCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvOperCol.fmt = LVCFMT_LEFT;

//	m_lvOperImgList = ImageList_LoadBitmap(OptiExpPlug_HInstance, 
//		MAKEINTRESOURCE(IDB_OPTIEXP_OPERLV), 16,1, RGB(255,255,255));
//	ListView_SetImageList(m_ctl[LV_OPER], m_lvOperImgList, LVSIL_SMALL);

	for(INT32 i=0; i<numLVOperHdrColStr; i++)
	{
		lvOperCol.cx = widthLvOperHdrCol[i];
		lvOperCol.pszText = (char*)(lvOperHdrColStr[i]);
		lvOperCol.iSubItem = i;

		ListView_InsertColumn(m_ctl[LV_OPER], i, &lvOperCol);
	}

	ListView_SetExtendedListViewStyle(m_ctl[LV_OPER], LVS_EX_FULLROWSELECT);
	ReloadOperListView();

	OptiExpPlug_CenterWindow(m_dlg, GetParent(m_dlg));
	RefreshPhysxCookLibInfo();
}

void OptiExpDlg::ReloadOperListView()
{
	ListView_DeleteAllItems(m_ctl[LV_OPER]);

	LVITEM item;
	memset(&item, 0, sizeof(item));

	item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	item.stateMask = LVIS_STATEIMAGEMASK | LVIS_FOCUSED | LVIS_SELECTED;
	item.state = 0; //INDEXTOSTATEIMAGEMASK(0);

	IOptiExpSettings* expSettings 
		= IOptiExpSettings::GetExpSettings(GetSelectedExpSettingsIndex());
	
	INT32 numWorldExpOperation = expSettings->GetNumExportOperation();
	for(INT32 i=0; i<numWorldExpOperation; i++)
	{
		const char* operTitle = expSettings->GetExportOperationTitle(i);
		INT32 operEnabled = 1;

		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = NULL;
		item.iImage = operEnabled;

		if(i==0)
			item.state |= LVIS_FOCUSED | LVIS_SELECTED;
		else
			item.state = 0;
	 
		ListView_InsertItem(m_ctl[LV_OPER], &item);
		ListView_SetItemText(m_ctl[LV_OPER], i, 0, (LPSTR)operTitle);
	}


} 

INT32 OptiExpDlg::GetSelectedExpOperIndex()
{
	return ListView_GetNextItem(m_ctl[LV_OPER], -1, LVNI_ALL | LVNI_SELECTED);
}

OptiExpTaskVec* OptiExpDlg::GetSelectedExpOperationTask()
{
	INT32 operRow = GetSelectedExpOperIndex();

	if(operRow == -1)
		return NULL;

	INT32 index = GetSelectedExpSettingsIndex();
	IOptiExpSettings* expSet = IOptiExpSettings::GetExpSettings(index);

	return expSet->GetExportOperTasks(operRow);
}

INT32 OptiExpDlg::GetSelectedExpSettingsIndex()
{
	return (INT32)SendMessage(m_ctl[CB_EXPSETTINGS], CB_GETCURSEL, 0, 0);
}

IOptiExpSettings* OptiExpDlg::GetSelectedExpSettings()
{
	return IOptiExpSettings::GetExpSettings(GetSelectedExpSettingsIndex());
}

bool OptiExpDlg::GetExportOperationFileNamePath(INT32 operIndex, char* path, char* name)
{
	char expPath[MAX_PATH] = {0};
	char expName[MAX_PATH] = {0};
	
	m_owner->GetCallBack()->OEPC_GetExporterFilePathName(expPath, expName);

	if(strlen(expName) == 0)
		return false;

	if(strlen(expPath))
		strcpy(path, expPath);
	else
		path[0] = NULL;

	strcpy(name, expName);

	IOptiExpSettings* expSet = GetSelectedExpSettings();
	expSet->ApplyFileName(operIndex, name);

	return true;
}

void OptiExpDlg::OnDestroy()
{
}

BOOL OptiExpDlg::OnCmd(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	WORD evt = HIWORD(wParam);

	switch(id)
	{
	case IDC_OPTIEXP_CB_EXPSETTINGS:
		{
			if(evt == CBN_SELCHANGE)
			{
				ms_expSettingsIndex = GetSelectedExpSettingsIndex();
				ReloadOperListView();
			}
		}
		break;

	case IDC_OPTIEXP_BT_OPER_SETUP:
		{
			SetupOperation();
		}
		break;

	case IDC_OPTIEXP_BT_EXPORT:
		{
			Export();
		}
		break;

	case IDC_OPTIEXP_CK_LOGGINGTOFILE:
		{
			ms_loggingToFile = SendMessage(m_ctl[CK_LOGGINGTOFILE], BM_GETCHECK, 0, 0) 
				== BST_CHECKED ? true : false;
		}
		break;

	case IDOK:
	case IDCANCEL:
		EndDialog(m_dlg, id);
		return TRUE;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL OptiExpDlg::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR hdr=(LPNMHDR)lParam;

	switch(hdr->idFrom)
	{
	case IDC_OPTIEXP_LV_OPER:
		{
			LPNMLISTVIEW nlv=(LPNMLISTVIEW)lParam;

			switch(hdr->code)
			{
			case NM_DBLCLK:
				{
					LVITEM item;
					memset(&item, 0, sizeof(item));

					item.mask = LVIF_IMAGE;					
					item.iItem = nlv->iItem;
					item.iSubItem = 0;
					
					ListView_GetItem(hdr->hwndFrom, &item);
					item.iImage = item.iImage ? 0 : 1;
					ListView_SetItem(hdr->hwndFrom, &item);
				}
				break;
			default:
				break;
			}			
		}
		break;

	default:
		break;
	}

	return TRUE;	
}

void OptiExpDlg::InitCtl(HWND hwnd)
{
	m_ctl[CB_EXPSETTINGS]		= GetDlgItem(m_dlg, IDC_OPTIEXP_CB_EXPSETTINGS);
	m_ctl[LV_OPER]				= GetDlgItem(m_dlg, IDC_OPTIEXP_LV_OPER);
	m_ctl[BT_OPER_SETUP]		= GetDlgItem(m_dlg, IDC_OPTIEXP_BT_OPER_SETUP);
	m_ctl[ST_OPER_DESC]			= GetDlgItem(m_dlg, IDC_OPTIEXP_ST_OPER_DESC);
	m_ctl[CK_LOGGINGTOFILE]     = GetDlgItem(m_dlg, IDC_OPTIEXP_CK_LOGGINGTOFILE);
	m_ctl[ST_PHYSXCOOKLIB]		= GetDlgItem(m_dlg, IDC_OPTIEXP_ST_PHYSX_COOKLIB);
	m_ctl[ST_VER]				= GetDlgItem(m_dlg, IDC_OPTIEXP_ST_VER);

	for(INT32 i=0; i<IOptiExpSettings::GetNumExpSettings(); i++)
		SendMessage(m_ctl[CB_EXPSETTINGS], CB_ADDSTRING, 0, (LPARAM)IOptiExpSettings::GetExpSettings(i)->GetTitle());

	SendMessage(m_ctl[CB_EXPSETTINGS], CB_SETCURSEL, ms_expSettingsIndex, 0);
	SendMessage(m_ctl[CK_LOGGINGTOFILE], BM_SETCHECK, ms_loggingToFile ? TRUE : FALSE, 0);

	char path[MAX_PATH] = {0};
	GetModuleFileName(m_owner->GetCallBack()->OEPC_GetOwnerModuleInst(), path, MAX_PATH);
	DWORD infoSiz = GetFileVersionInfoSize(path, 0);

	if(infoSiz > 0)
	{
		char* buf = new char[infoSiz];

		if(GetFileVersionInfo(path, 0, infoSiz, buf))
		{
			VS_FIXEDFILEINFO* fileInfo = NULL;
			UINT bufLen = 0;

			if(VerQueryValue(buf,"\\",(LPVOID*)&fileInfo, &bufLen) !=0)
			{    
				char verInfo[MAX_PATH] = {0};

				WORD major, minor, build, rev;
				major = HIWORD(fileInfo->dwFileVersionMS);
				minor = LOWORD(fileInfo->dwFileVersionMS);
				build = HIWORD(fileInfo->dwFileVersionLS);
				rev = LOWORD(fileInfo->dwFileVersionLS);

				sprintf_s(verInfo, MAX_PATH, "%d,%d,%d,%d",major, minor, build, rev);
				SetWindowText(m_ctl[ST_VER], verInfo);
			}
		}

		delete [] buf;
	}
}

bool OptiExpDlg::Export()
{
	char path[MAX_PATH] = {0};
	char name[MAX_PATH] = {0};

	IOptiExpSettings* expSet = GetSelectedExpSettings();

	if(!expSet)
		return false;

	INT32 operIndex = GetSelectedExpOperIndex();
	if(operIndex == -1)
	{
		MessageBox(m_dlg, "Select export operation!", "ERR!", MB_ICONERROR);
		return false;
	}

	OptiExpTaskVec* tasks = expSet->GetExportOperTasks(operIndex);

	if(!tasks)
		return false;

	if(!GetExportOperationFileNamePath(operIndex, path, name))
		return false;

	bool loggingToi3Export 
		= SendMessage(m_ctl[CK_LOGGINGTOFILE], BM_GETCHECK, 0, 0) == BST_CHECKED ? true : false;

	if(!RefreshPhysxCookLibInfo())
		return false;

	m_owner->GetCallBack()->OEPC_ClearLog();

	bool prevLoggingStatus;
	m_owner->GetCallBack()->OEPC_SetLoggingToFile(loggingToi3Export, &prevLoggingStatus);

	OptiExpTaskProcessor taskProcessor;
	OptiExpTaskShareInfo taskShInfo;

	taskShInfo.m_ownerWnd = m_dlg;
	taskShInfo.m_expCallBack = m_owner->GetCallBack();
	taskShInfo.SetFilePathName(path, name);
	if(!taskProcessor.DoWithStatus(&taskShInfo, *tasks, m_dlg))
	{
		MessageBox(m_dlg, "An Error is occured during exporting.. ", "Error", MB_ICONERROR);
	}

	taskShInfo.Reset();
	m_owner->GetCallBack()->OEPC_SetLoggingToFile(prevLoggingStatus);

	RefreshPhysxCookLibInfo(); // 가끔씩 PhysCookingLibrary가 작동안할때가 있는거 같다 체크용.

	return true;
}

void OptiExpDlg::SetupOperation()
{
	OptiExpTaskVec* tasks = GetSelectedExpOperationTask();

	OptiExpSetupOperDlg setup;

	setup.SetExporterCallBack(m_owner->GetCallBack());
	setup.DoModal(tasks, m_dlg);
}

bool OptiExpDlg::RefreshPhysxCookLibInfo()
{
	char str[MAX_PATH] = {0};

	DWORD ver;
	bool avail;
	bool rst = false;

	avail = m_owner->GetCallBack()->OEPC_GetPhysXCookingLibInfo(&ver);

	if(avail)
	{
								//" PhysX Cooking Library(VER=%08p) is avaiable"
		sprintf_s(str, MAX_PATH, " PhysX Cooking Library is avaiable");
		rst = true;
	}
	else
	{
		sprintf_s(str, MAX_PATH, " ERR! - PhysX Cooking Library is not available!!");
		if(!ms_notToWarnPhysXCookingLibErr && 
			MessageBox(m_dlg, "ERR! - PhysX Cooking Library is not avaialble..\n"
				"You need to restart 3dsmax to use physX cooking library..\n\n", 
				"PhysX Cooking Library Err!", MB_ICONERROR))
		{
//			ms_notToWarnPhysXCookingLibErr = true;
			rst = false;
		}
	}

	SetWindowText(m_ctl[ST_PHYSXCOOKLIB], str);

	return rst;
}
