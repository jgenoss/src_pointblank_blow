#pragma once
#include "DB/MapPlayInfo.h"
#include "afxwin.h"

class CDlgHitMapTimeSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgHitMapTimeSet)

private:
	bool		m_bState;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.
	virtual void OnOK() override;
	DECLARE_MESSAGE_MAP()

public:
	CDlgHitMapTimeSet(CWnd* pParent = NULL);
	virtual ~CDlgHitMapTimeSet();

	virtual BOOL OnInitDialog() override;

	// 대화 상자 데이터입니다.
	enum {
		IDD = IDD_DLG_HITMAP_TIMESET
	};

	
	afx_msg void OnMcnSelchangeMonthcalendarStart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMcnSelchangeMonthcalendarEnd(NMHDR *pNMHDR, LRESULT *pResult);

	bool GetState() {return m_bState;	}

	COleDateTime m_tmStart;
	COleDateTime m_tmEnd;
};

