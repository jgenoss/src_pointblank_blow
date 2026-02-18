// SpecSoundNode.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "SpecSoundNode.h"
#include ".\specsoundnode.h"

#include "i3Base/string/ext/ftoa.h"


// CSpecSoundNode dialog

IMPLEMENT_DYNAMIC(CSpecSoundNode, i3TDKDialogBase)
CSpecSoundNode::CSpecSoundNode(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecSoundNode::IDD, pParent)
{
}

CSpecSoundNode::~CSpecSoundNode()
{
}

void CSpecSoundNode::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_LOOP, m_LoopCtrl);
	DDX_Control(pDX, IDC_ED_LOOPCOUNT, m_LoopCountCtrl);
	DDX_Control(pDX, IDC_CHK_POSITIONAL, m_PositionalCtrl);
	DDX_Control(pDX, IDC_ED_PRIORITY, m_PriorityCtrl);
	DDX_Control(pDX, IDC_SLIDER_VOL, m_VolCtrl);
	DDX_Control(pDX, IDC_EDIT_SOUNDNODE_NAME, m_edSoundNodeName);
	DDX_Control(pDX, IDC_CHK_MUTEATMAX, m_chkMuteAtMaxdistance);
	DDX_Control(pDX, IDC_CHK_STOP_ON_RESET, m_chkStopOnReset);
	DDX_Control(pDX, IDC_ED_MAXDIST, m_edMaxDistance);
	DDX_Control(pDX, IDC_ED_MINDIST, m_edMinDistance);
}

void CSpecSoundNode::SetObject( i3ElementBase * pObj)
{
	char conv[256];

	I3ASSERT( i3::kind_of<i3SoundNode*>(pObj)); //->IsTypeOf( i3SoundNode::static_meta()));

	i3SoundNode * pNode = (i3SoundNode *) pObj;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();
	
	I3_REF_CHANGE(m_pObject, pObj);
	
	//
	sprintf( conv, "%s", pNode->GetName());
	m_edSoundNodeName.SetWindowText( conv);

	// Loop
	if( pPlayInfo->GetLoopMode() == I3SND_RT_LOOPMODE_LOOPING)
		m_LoopCtrl.SetCheck( BST_CHECKED);
	else
		m_LoopCtrl.SetCheck( BST_UNCHECKED);

	// Loop Count
	sprintf( conv, "%d", pNode->getLoopCount());
	m_LoopCountCtrl.SetWindowText( conv);

	// Positional Sound
	if( pPlayInfo->Get3DMode() == I3SND_RT_3DMODE_DISABLE)
		m_PositionalCtrl.SetCheck( BST_CHECKED);
	else
		m_PositionalCtrl.SetCheck( BST_UNCHECKED);

	// Default Volume
	m_VolCtrl.SetPos( pPlayInfo->GetDefaultVolume());

	// Priority
	sprintf( conv, "%d", pPlayInfo->GetPriorityLevel());
	m_PriorityCtrl.SetWindowText( conv);

	if( pPlayInfo->isMuteAtMaxDistance())
	{
		m_chkMuteAtMaxdistance.SetCheck( BST_CHECKED);
	}
	else
	{
		m_chkMuteAtMaxdistance.SetCheck( BST_UNCHECKED);
	}

	if( pPlayInfo->isStopOnReset())
	{
		m_chkStopOnReset.SetCheck( BST_CHECKED);
	}
	else
	{
		m_chkStopOnReset.SetCheck( BST_UNCHECKED);
	}

	i3::string str;
	i3::ftoa( pPlayInfo->getMaxDistance(), str);
	m_edMaxDistance.SetWindowText( str.c_str());

	i3::ftoa( pPlayInfo->getMinDistance(), str);
	m_edMinDistance.SetWindowText( str.c_str());
}

BEGIN_MESSAGE_MAP(CSpecSoundNode, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_CHK_LOOP, OnBnClickedChkLoop)
	ON_BN_CLICKED(IDC_CHK_POSITIONAL, OnBnClickedChkPositional)
	ON_EN_KILLFOCUS(IDC_ED_LOOPCOUNT, OnEnKillfocusEdLoopcount)
	ON_EN_KILLFOCUS(IDC_ED_PRIORITY, OnEnKillfocusEdPriority)
	ON_EN_KILLFOCUS(IDC_CHK_MUTEATMAX, OnBnClickedChkMuteAtmax)
	ON_BN_CLICKED(IDC_CHK_STOP_ON_RESET, OnBnClickedOnStopReset)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_ED_MAXDIST, &CSpecSoundNode::OnEnChangeEdMaxdist)
	ON_EN_CHANGE(IDC_ED_MINDIST, &CSpecSoundNode::OnEnChangeEdMindist)
END_MESSAGE_MAP()


// CSpecSoundNode message handlers

BOOL CSpecSoundNode::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_VolCtrl.SetRange( 0, 100);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSpecSoundNode::OnOK()
{
}

void CSpecSoundNode::OnBnClickedChkLoop()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	if( m_LoopCtrl.GetCheck() == BST_CHECKED)
		pPlayInfo->AddStyle( I3SND_RT_LOOPMODE_LOOPING);
	else
		pPlayInfo->RemoveStyle( I3SND_RT_LOOPMODE_LOOPING);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pNode);
}

void CSpecSoundNode::OnBnClickedChkPositional()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	if( m_PositionalCtrl.GetCheck() == BST_CHECKED)
		pPlayInfo->set3DMode(I3SND_RT_3DMODE_DISABLE);
	else
		pPlayInfo->set3DMode(I3SND_RT_3DMODE_WORLD_SPACE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pNode);
}

void CSpecSoundNode::OnEnKillfocusEdLoopcount()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	//i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();
	char conv[128];

	m_LoopCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pNode->setLoopCount( atoi( conv));

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pNode);
}

void CSpecSoundNode::OnEnKillfocusEdPriority()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();
	char conv[128];

	m_PriorityCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pPlayInfo->SetPriorityLevel( atoi( conv));

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pNode);
}

void CSpecSoundNode::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	if( pScrollBar == (CScrollBar *) &m_VolCtrl)
	{
		INT32 pos = MINMAX( 0, m_VolCtrl.GetPos(), 100);

		pPlayInfo->SetDefaultVolume( pos);
		pPlayInfo->setListenerVolume( pos);
	}

	i3TDKDialogBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSpecSoundNode::OnBnClickedChkMuteAtmax()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	if( m_chkMuteAtMaxdistance.GetCheck() == BST_CHECKED)
		pPlayInfo->setMuteAtMaxDistance(true);
	else
		pPlayInfo->setMuteAtMaxDistance(false);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pNode);
}

void CSpecSoundNode::OnBnClickedOnStopReset()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	if( m_chkStopOnReset.GetCheck() == BST_CHECKED)
		pPlayInfo->setStopOnReset(true);
	else
		pPlayInfo->setStopOnReset(false);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pNode);
}

void CSpecSoundNode::OnEnChangeEdMaxdist()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	char conv[ 260];
	m_edMaxDistance.GetWindowText( conv, sizeof(conv) - 1);
	pPlayInfo->SetDistanceInfo( (REAL32)atoi( conv), pPlayInfo->getMinDistance(), pPlayInfo->getVolumeType());
}

void CSpecSoundNode::OnEnChangeEdMindist()
{
	i3SoundNode * pNode = (i3SoundNode *) m_pObject;
	i3SoundPlayInfo * pPlayInfo = pNode->getSoundPlayInfo();

	char conv[ 260];
	m_edMinDistance.GetWindowText( conv, sizeof(conv) - 1);
	pPlayInfo->SetDistanceInfo( pPlayInfo->getMaxDistance(), (REAL32)atoi( conv), pPlayInfo->getVolumeType());
}
