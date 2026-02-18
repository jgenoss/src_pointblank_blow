
#ifndef _MCTLFILEDROPTARGET_H_
#define _MCTLFILEDROPTARGET_H_


namespace ToolAM
{

	/*
		void CDlgAutomation::OnDropFiles(HDROP hDropInfo)
		{
			POINT ptHit;
			if(DragQueryPoint(hDropInfo, &ptHit))
			{
				INT32 numTest = 1;
				RECT rc[100];
				GetDlgItem(IDC_AUTOMAT_MASK_IMPL_DROPMASKDIR)->GetWindowRect(&rc[0]); ScreenToClient(&rc[0]);
				if(PtInRect(&rc[0], ptHit))
					GetDlgItem(IDC_AUTOMAT_MASK_IMPL_DROPMASKDIR)->SendMessage(WM_DROPFILES, (WPARAM)hDropInfo, 0);
			}
		}
	*/

	class MCtlFileDropTarget : public CStatic
	{
		DECLARE_DYNAMIC(MCtlFileDropTarget)

	public:
		class CallBack 
		{
		public:			
			virtual void  MCtlFileDropTarget_Reset(void* arg=NULL) {} 
			virtual bool MCtlFileDropTarget_OnIterateDropFiles(void* arg,   // ret false: finish, true: continue
				INT32 totalCnt, INT32 cur, bool isDirectory, const char* path) = 0;
			virtual void  MCtlFileDropTarget_OnIterateAfter(void* arg, INT32 num) {}

			void*		m_dropInfo;
		};

		MCtlFileDropTarget(MCtlFileDropTarget::CallBack* callBack=NULL);
		virtual ~MCtlFileDropTarget();

		BOOL Create(CWnd* hAreaWnd, DWORD dwStyle, const char* wndName, CWnd * pParentWnd, UINT id);

	protected:
		void PerformOnDropFiles(HDROP dropInfo);
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		DECLARE_MESSAGE_MAP()

	public:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	protected:
		CallBack*						m_callBack;
		char							m_wndName[MAX_PATH];
	};

}

#endif
