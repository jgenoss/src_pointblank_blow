
#ifndef _OPTIEXPSETUPOPERDLG_H_
#define _OPTIEXPSETUPOPERDLG_H_

#include "OptiExpTask.h"

class OptiExpSetupOperDlg
{
public:
	OptiExpSetupOperDlg();
	~OptiExpSetupOperDlg();

	bool DoModal(OptiExpTaskVec* tasks, HWND parent);
	void SetExporterCallBack(OptiExpPlugCallBack* callBack);

protected:
	void OnInitDialog();
	BOOL OnCmd(WPARAM wParam, LPARAM lParam);

	void GenerateTaskDescComp();

	static BOOL __stdcall DlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, 
		LPARAM lParam);

	void GenerateTaskSetupControl(INT32 row, INT32 subIndex, 
		INT32& left, INT32& top, HWND hWndParent, OptiExpTask* task);
	
protected:
	HWND									m_dlg;

	OptiExpTaskShareInfo*					m_taskShInfo;
	OptiExpTaskVec*							m_tasks;
};


#endif
