
#include "ToolAMPCH.h"
#include "ctl/MCtlFileDropTarget.h"


using namespace ToolAM;

//
// MCtlFileDropTarget
//

IMPLEMENT_DYNAMIC(MCtlFileDropTarget, CWnd)

BEGIN_MESSAGE_MAP(MCtlFileDropTarget, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


MCtlFileDropTarget::MCtlFileDropTarget(MCtlFileDropTarget::CallBack* callBack)
{
	m_callBack = callBack;
	m_wndName[0] = NULL;

	strcpy_s(m_wndName, MAX_PATH, "Drop File!");
}

MCtlFileDropTarget::~MCtlFileDropTarget()
{
	
}

BOOL MCtlFileDropTarget::Create(CWnd* hAreaWnd, DWORD dwStyle, const char* wndName, CWnd * pParentWnd, UINT id)
{
	HCURSOR hCursor = NULL;
	HBRUSH hBrush = NULL;
	HICON	hIcon = NULL;

	RECT rcDropTarget;
	hAreaWnd->GetWindowRect(&rcDropTarget);
	pParentWnd->ScreenToClient(&rcDropTarget);

	hCursor = LoadCursor( NULL, IDC_ARROW);
	hBrush = (HBRUSH) ::GetStockObject( NULL_BRUSH);

	//lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_HREDRAW | CS_DBLCLKS, hCursor, hBrush, hIcon);
	//I3ASSERT(lpszClass != NULL);

	if(wndName)
		strcpy_s(m_wndName, MAX_PATH, wndName);

	dwStyle |= SS_SUNKEN | SS_CENTER | SS_CENTERIMAGE;

	BOOL rst = CStatic::Create(m_wndName, dwStyle, rcDropTarget, pParentWnd, id);

	DragAcceptFiles(TRUE);

	return rst;
}

LRESULT MCtlFileDropTarget::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_DROPFILES)
	{
		PerformOnDropFiles((HDROP)wParam);
		DragFinish((HDROP)wParam);

		return 0;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void MCtlFileDropTarget::PerformOnDropFiles(HDROP dropInfo)
{
	if(m_callBack)
	{
		m_callBack->MCtlFileDropTarget_Reset(this);
		m_callBack->m_dropInfo = dropInfo;

		char fileName[MAX_PATH];

		INT32 numFile = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);
		for(INT32 i=0; i<numFile; i++)
		{
			DragQueryFile(dropInfo, i, fileName, sizeof(fileName) - 1);

			DWORD dwAttr = GetFileAttributes(fileName);

			bool isDirecotry =
				(dwAttr != INVALID_FILE_ATTRIBUTES) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) ;

			if(!m_callBack->MCtlFileDropTarget_OnIterateDropFiles(this, numFile, i, isDirecotry, fileName))
				break;
		}

		m_callBack->MCtlFileDropTarget_OnIterateAfter(this, numFile);
	}
}

//
// afx message handler
//

int MCtlFileDropTarget::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
