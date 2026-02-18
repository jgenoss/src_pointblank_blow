
#include "stdafx.h"
#include "resource.h"
#include "OptiExpSetupOperDlg.h"


OptiExpSetupOperDlg::OptiExpSetupOperDlg()
{
	m_dlg = NULL;
	m_taskShInfo = new OptiExpTaskShareInfo;
}

OptiExpSetupOperDlg::~OptiExpSetupOperDlg()
{
	delete m_taskShInfo;
}

bool OptiExpSetupOperDlg::DoModal(OptiExpTaskVec* tasks, HWND parent)
{
	m_tasks = tasks;

	BOOL rst = DialogBoxParam(OptiExpPlug_HInstance, MAKEINTRESOURCE(IDD_DLG_OPTIEXP_SETUPOPER), 
		parent, OptiExpSetupOperDlg::DlgProc, (LPARAM)this);

	m_tasks = NULL;

	return rst ? true : false;
}

void OptiExpSetupOperDlg::SetExporterCallBack(OptiExpPlugCallBack* callBack)
{
	if(callBack)
		m_taskShInfo->m_expCallBack = callBack;
}

void OptiExpSetupOperDlg::OnInitDialog()
{
	GenerateTaskDescComp();	
	OptiExpPlug_CenterWindow(m_dlg, GetParent(m_dlg));
}

BOOL OptiExpSetupOperDlg::OnCmd(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	WORD evt = HIWORD(wParam);

	if((id & 0xf000))
	{
		OptiExpTaskVec& taskVec = (*m_tasks);

		bool enableBtn = (id & 0xf000) == 0xa000 ? true : false;
		bool setupBtn = (id & 0xf000) == 0xb000 ? true : false;

		OptiExpTaskVec::size_type rowIndex = id & 0x00ff;
		OptiExpTaskVec::size_type subIndex = (id & 0x0f00) >> 8;
		
		bool hasSubTask = false;

		if(subIndex != 0xf)
			hasSubTask = true;

		OptiExpTaskVec::size_type numTasks = taskVec.size();

		if(rowIndex > numTasks)
		{
			I3ASSERT( 0 && "ERR! - Invalid opti task index..");
			return FALSE;
		}
 
		OptiExpTask* task = taskVec[rowIndex];

		if(hasSubTask)
			task = task->GetSubTask(subIndex);

		I3ASSERT(task);

		if(!task)
			return FALSE;

		if(enableBtn && evt == BN_CLICKED)
		{
			if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
				task->SetEnable(false);
			else
				task->SetEnable(true);
		}
		else if(setupBtn && evt == BN_CLICKED)
		{
			task->ShowSetupBox(m_dlg, m_taskShInfo);
		}

		return TRUE;
	}

	switch(id)
	{
	case IDOK:
	case IDCANCEL:
		::EndDialog(m_dlg, id);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL OptiExpSetupOperDlg::DlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	OptiExpSetupOperDlg* setupOperDlg 
		= (OptiExpSetupOperDlg*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(uiMsg)
	{
		case WM_INITDIALOG:
		{
			setupOperDlg = (OptiExpSetupOperDlg*)lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)setupOperDlg);

			setupOperDlg->m_dlg = hWnd;
			setupOperDlg->OnInitDialog();
		}
		break;

		case WM_COMMAND:
		{
			if(setupOperDlg)
				return setupOperDlg->OnCmd(wParam, lParam);
		}
		break;

		case WM_DESTROY:
			{
				if(setupOperDlg)
				{
					setupOperDlg->m_dlg = NULL;
					SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)NULL);
				}

				return FALSE;
			}

		default:
			return FALSE;
	}

	return TRUE;
}

void OptiExpSetupOperDlg::GenerateTaskDescComp()
{		
	HWND parent = m_dlg;
	INT32 left = 30;
	INT32 top = 30;

	for(OptiExpTaskVec::size_type i=0; i<m_tasks->size(); i++)
	{
		OptiExpTask* task = (*m_tasks)[i];
		GenerateTaskSetupControl(i, -1, left, top, parent, task);

		if(task->GetNumSubTask())
		{
			INT32 subTaskLeft = left + 60;
			INT32 subTaskTop  = top;

			for(OptiExpTaskVec::size_type k=0; k<task->GetNumSubTask(); k++)
				GenerateTaskSetupControl(i, k, subTaskLeft, subTaskTop, parent, task->GetSubTask(k));

			top = subTaskTop;
		}
	}
}

void OptiExpSetupOperDlg::GenerateTaskSetupControl(INT32 row, INT32 subIndex, INT32& left, INT32& top, 
	HWND hWndParent, OptiExpTask* task)
{
	INT32 sizRowItemH = 32;
	INT32 spcRowItmeW = 10;
	HMENU id; 
	HINSTANCE hInst = OptiExpPlug_HInstance;		

	// enable/disable btn
	INT32 chkX = left, chkY = top;
	INT32 chkW = 32,   chkH = sizRowItemH - 10;

	id = (HMENU)(0x0000a000 | (row & 0xff) | (subIndex == -1 ? 0x0f00 : (0x0f00 & (subIndex << 8)) )); 
	HWND enableTask = CreateWindow("button", NULL, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
		chkX, chkY, chkW, chkH, hWndParent, id, hInst, NULL);
	SendMessage(enableTask, BM_SETCHECK, task->GetEnable() ? BST_CHECKED : BST_UNCHECKED, 0);

	// setup btn
	id = (HMENU)(0x0000b000 | (row & 0xff) | (subIndex == -1 ? 0x0f00 : (0x0f00 & (subIndex << 8)) )); 
	INT32 sbtX = chkX + chkW + spcRowItmeW, sbtY = top;
	INT32 sbtW = 70, sbtH = sizRowItemH - 10;
	HWND setupTask = CreateWindow("button", "Setup", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		sbtX, sbtY, sbtW, sbtH, hWndParent, id, hInst, NULL);

	if(!task->HasSetupBox())
		EnableWindow(setupTask, FALSE);

	// Desc
	INT32 descX = sbtX + sbtW + spcRowItmeW, descY = top;
	INT32 descW = 600, descH = sizRowItemH;
	HWND desc = CreateWindow("static", NULL, WS_CHILD | WS_VISIBLE,
		descX, descY, descW, descH, hWndParent, (HMENU)-1, hInst, NULL);
	SetWindowText(desc, task->GetDesc());

	top += 30;
}
