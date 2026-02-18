// TimeTrackSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "TimeTrackSpec.h"


// CTimeTrackSpec dialog

IMPLEMENT_DYNAMIC(CTimeTrackSpec, CDialog)
CTimeTrackSpec::CTimeTrackSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeTrackSpec::IDD, pParent)
	, m_StartTime(0)
	, m_Duration(0)
	, m_Type(0)
{
}

CTimeTrackSpec::~CTimeTrackSpec()
{
}

void CTimeTrackSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_STARTTIME, m_StartTime);
	DDX_Text(pDX, IDC_ED_DURATION, m_Duration);
	DDX_CBIndex(pDX, IDC_CB_TYPE, m_Type);
	DDX_Control(pDX, IDC_BTN_SCENEGRAPH, m_BtnSceneGraph);
}


BEGIN_MESSAGE_MAP(CTimeTrackSpec, CDialog)
END_MESSAGE_MAP()


// CTimeTrackSpec message handlers
