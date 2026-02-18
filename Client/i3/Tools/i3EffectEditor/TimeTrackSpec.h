#pragma once
#include "afxwin.h"
#include "resource.h"

// CTimeTrackSpec dialog

class CTimeTrackSpec : public CDialog
{
	DECLARE_DYNAMIC(CTimeTrackSpec)

public:
	CTimeTrackSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimeTrackSpec();

// Dialog Data
	enum { IDD = IDD_TIMETRACKSPEC };

protected:
	i3TimeSequenceInfo *		m_pInfo;

public:
	void		SetTimeSequence( i3TimeSequenceInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_StartTime;
	float m_Duration;
	int m_Type;
	CButton m_BtnSceneGraph;
};
