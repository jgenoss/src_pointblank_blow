#pragma once


// CPaneTrack dialog

class CPaneTrack : public CDialog
{
	DECLARE_DYNAMIC(CPaneTrack)

public:
	CPaneTrack(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneTrack();

// Dialog Data
	enum { IDD = IDD_PANE_TRACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
