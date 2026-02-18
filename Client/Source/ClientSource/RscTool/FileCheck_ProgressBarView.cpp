// FileCheck_ProgressBarView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_ProgressBarView.h"

#include "FileCheck_ProgressMD5View.h"
#include "DiffResult_PackScriptResult.h"

namespace
{
	struct StartButtonCB : i3MFCButtonCallback
	{
		FileCheck_ProgressBarView* owner;
		StartButtonCB( FileCheck_ProgressBarView* owner) : owner(owner) {}
		
		virtual void OnClick(i3MFCButton* ctrl) 
		{
			FileCheck_ProgressMD5View* MD5View = owner->GetProgressMD5View();
			MD5View->Start();
		}
	};

	struct PauseResumeButtonCB : i3MFCButtonCallback
	{
		FileCheck_ProgressBarView*	owner;
		bool						bPauseText;
		PauseResumeButtonCB( FileCheck_ProgressBarView* owner) : owner(owner), bPauseText(true) {}

		virtual void OnClick(i3MFCButton* ctrl) 
		{
			FileCheck_ProgressMD5View* MD5View = owner->GetProgressMD5View();

			if (bPauseText)
			{
				ctrl->SetWindowText(L"Resume");
				MD5View->Pause();
			}
			else
			{
				ctrl->SetWindowText(L"Pause");
				MD5View->Resume();
			}
			
			bPauseText = !bPauseText;
		}
	};

	struct StopButtonCB : i3MFCButtonCallback
	{
		FileCheck_ProgressBarView* owner;
		StopButtonCB( FileCheck_ProgressBarView* owner) : owner(owner) {}

		virtual void OnClick(i3MFCButton* ctrl) 
		{
			FileCheck_ProgressMD5View* MD5View = owner->GetProgressMD5View();
			MD5View->Stop();
		}
	};
	
}


// FileCheck_ProgressBarView

IMPLEMENT_DYNCREATE(FileCheck_ProgressBarView, CView)

FileCheck_ProgressBarView::FileCheck_ProgressBarView() : m_MD5View(NULL), 
m_str_status(L"Idle")
{
	m_button_Start.SetCallback(new StartButtonCB(this));
	m_button_PauseResume.SetCallback(new PauseResumeButtonCB(this));
	m_button_Stop.SetCallback(new StopButtonCB(this));
}

FileCheck_ProgressBarView::~FileCheck_ProgressBarView()
{
	delete m_button_Stop.GetCallback();
	delete m_button_PauseResume.GetCallback();
	delete m_button_Start.GetCallback();
}

BEGIN_MESSAGE_MAP(FileCheck_ProgressBarView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_ProgressBarView 그리기입니다.

void FileCheck_ProgressBarView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_ProgressBarView 진단입니다.

#ifdef _DEBUG
void FileCheck_ProgressBarView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_ProgressBarView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_ProgressBarView 메시지 처리기입니다.

int FileCheck_ProgressBarView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	m_progress_ctrl.Create(WS_VISIBLE|WS_CHILD, CRect(0,0,0,0), this, 0);
	
	m_button_Start.Create(L"Start", WS_VISIBLE|WS_CHILD, CRect(0,0,0,0), this, 0);
	m_button_PauseResume.Create(L"Pause", WS_VISIBLE|WS_CHILD, CRect(0,0,0,0), this, 1);
	m_button_Stop.Create(L"Stop", WS_VISIBLE|WS_CHILD, CRect(0,0,0,0), this, 2);

	m_button_PauseResume.EnableWindow(FALSE);

	return 0;
}

void FileCheck_ProgressBarView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int pos_x = 0;
	int width = cx * 2 / 3;
	m_progress_ctrl.MoveWindow(pos_x,0, width, cy);
	
	pos_x += width;
	width = cx / 9;
	m_button_Start.MoveWindow(pos_x, 0, width, cy);
	pos_x += width;
	m_button_PauseResume.MoveWindow(pos_x, 0, width, cy);
	pos_x += width;
	m_button_Stop.MoveWindow(pos_x, 0, width, cy);

}

BOOL FileCheck_ProgressBarView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

namespace
{
	void SetProgressPosText(CTextProgressCtrl* ctrl, int Pos, int Size, const i3::wstring& status)
	{
		i3::stack_wstring wstrTemp;
		i3::sprintf(wstrTemp, L" ( %d / %d )  [ %s ]", Pos, Size, status);
		ctrl->SetWindowText(wstrTemp.c_str());
	
	}


}


void	FileCheck_ProgressBarView::InitProgressBar(const DiffResult_PackScriptResult& diff_result)
{
	const size_t num_file = diff_result.in_out_alist[DIFFRESULT_PS_FOUND_LEFT].size();
		//in_out_found_lhs_res.size();
	m_progress_ctrl.SetRange( 0, (short)num_file );
	m_progress_ctrl.SetPos(0);

	SetProgressPosText(&m_progress_ctrl, 0, num_file, m_str_status);
}

void	FileCheck_ProgressBarView::StartProgress(size_t num_list)
{
	m_progress_ctrl.SetRange(0, (short)num_list);
	m_progress_ctrl.SetPos(0);
	
	m_str_status = L"In Progress";
	
	SetProgressPosText(&m_progress_ctrl, 0, num_list, m_str_status);

	m_progress_ctrl.RedrawWindow();

	m_button_PauseResume.EnableWindow(TRUE);
}

void	FileCheck_ProgressBarView::PauseProgress(size_t next_index)
{
	m_str_status = L"Paused";
	int rng_min, rng_max;
	m_progress_ctrl.GetRange(rng_min, rng_max);
	SetProgressPosText(&m_progress_ctrl, m_progress_ctrl.GetPos(), rng_max, m_str_status);

	m_progress_ctrl.RedrawWindow();
}

void	FileCheck_ProgressBarView::ResumeProgress(size_t next_index)
{
	m_str_status = L"In Progress";
	int rng_min, rng_max;
	m_progress_ctrl.GetRange(rng_min, rng_max);
	SetProgressPosText(&m_progress_ctrl, m_progress_ctrl.GetPos(), rng_max, m_str_status);	

	m_progress_ctrl.RedrawWindow();
}

void	FileCheck_ProgressBarView::StopProgress(size_t next_index)
{
	m_str_status = L"Stopped";
	int rng_min, rng_max;
	m_progress_ctrl.GetRange(rng_min, rng_max);
	SetProgressPosText(&m_progress_ctrl, m_progress_ctrl.GetPos(), rng_max, m_str_status);	
	
	m_progress_ctrl.RedrawWindow();
	
	PauseResumeButtonCB* cb = (PauseResumeButtonCB*)m_button_PauseResume.GetCallback();
	cb->bPauseText = true;
	m_button_PauseResume.SetWindowText(L"Pause");

	m_button_PauseResume.EnableWindow(FALSE);
}

void	FileCheck_ProgressBarView::EndProgress()
{
	m_str_status = L"Done!";
	int rng_min, rng_max;
	m_progress_ctrl.GetRange(rng_min, rng_max);
	SetProgressPosText(&m_progress_ctrl, m_progress_ctrl.GetPos(), rng_max, m_str_status);	

	m_progress_ctrl.RedrawWindow();

	PauseResumeButtonCB* cb = (PauseResumeButtonCB*)m_button_PauseResume.GetCallback();
	cb->bPauseText = true;
	m_button_PauseResume.SetWindowText(L"Pause");

	m_button_PauseResume.EnableWindow(FALSE);

}
void	FileCheck_ProgressBarView::SetProgressPos(size_t next_index)
{
	
	m_progress_ctrl.SetPos(next_index);
	int rng_min, rng_max;
	m_progress_ctrl.GetRange(rng_min, rng_max);

	SetProgressPosText(&m_progress_ctrl, next_index, rng_max, m_str_status);	
	
}

void FileCheck_ProgressBarView::SetRedrawProgressCtrl(BOOL bTrue)
{
	m_progress_ctrl.SetRedraw(bTrue);

	if (bTrue)
	{
		m_progress_ctrl.RedrawWindow();
	}
}
