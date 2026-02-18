#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSpecAnim dialog

class CSpecAnim : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecAnim)

protected:
	void	_UpdateTrack( INT32 idx, i3TransformSequence * pSeq);

	void	OnDelete(void);

public:
	CSpecAnim(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecAnim();

// Dialog Data
	enum { IDD = IDD_SPEC_ANIM };

	virtual void	SetObject( i3ElementBase * pObj);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_TrackCountCtrl;
	CEdit m_DurationCtrl;
	CListCtrl m_TrackListCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnKeydownTracklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTracklist(NMHDR *pNMHDR, LRESULT *pResult);
};
