#pragma once


// cDlgTimeTrack 대화 상자입니다.
#include "TimeSeqCtrl.h"
#include "GICShapeAI.h"

class cDlgTimeTrack : public CDialog
{
	DECLARE_DYNAMIC(cDlgTimeTrack)

public:
	cDlgTimeTrack(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgTimeTrack();

	void SetAIState(GICShapeAI* pShape);
private:
	CTimeSeqCtrl	m_TimeSeqCtrl;
	GICShapeAI *	m_pAIState;
	HWND			m_HwndParent;
	int				m_nSelectedIndex;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_WEAPON_TIME_TRACK };

public:
	virtual BOOL OnInitDialog();
	virtual void OnMove(HWND hWndParent);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void UpdateAll(int nIdx);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

};
