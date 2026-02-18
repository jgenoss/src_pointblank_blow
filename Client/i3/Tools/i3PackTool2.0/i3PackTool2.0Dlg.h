
// i3PackTool2.0Dlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
#include "LiveTreeCtrl.h"
#include "QATreeCtrl.h"
#include "MediaTreeCtrl.h"

#define MAX_MRU		5

class PTTreeworkLive;
class PTTreeworkQA;
class PTTreeworkMediator;

// Ci3PackTool20Dlg 대화 상자
class Ci3PackTool20Dlg : public CDialog
{
// 생성입니다.
public:
	Ci3PackTool20Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_I3PACKTOOL20_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExit();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnOpenLiveFolder();
	afx_msg void OnOpenQAFolder();
	afx_msg void OnOpenMediaFolder();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	DECLARE_MESSAGE_MAP()

private:
	CLiveTreeCtrl		m_liveTreeCtrl;
	CQATreeCtrl			m_qaTreeCtrl;
	CMediaTreeCtrl		m_mediaTreeCtrl;

	bool m_enableSizeModifyTreeCtrl;

	CImageList m_imageList;

	CEdit m_editLiveFolderpath;
	CEdit m_editQAFolderpath;
	CEdit m_editMediaFolderpath;

	struct SizeRatio
	{
		REAL32 w, h; SizeRatio() : w(1.0f), h(1.0f) {}
	};
	SizeRatio m_liveTreeSizeRatio;
	SizeRatio m_qaTreeCtrlSizeRatio;
	SizeRatio m_mediaTreeCtrlSizeRatio;
	SizeRatio m_liveEditSizeRatio;
	SizeRatio m_qaEditSizeRatio;
	SizeRatio m_mediaEditSizeRatio;
};

extern Ci3PackTool20Dlg g_dlg;