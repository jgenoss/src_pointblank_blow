// EditEffectSound.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "EditEffectSound.h"
#include ".\editeffectsound.h"


// CEditEffectSound dialog

IMPLEMENT_DYNAMIC(CEditEffectSound, i3TDKDialogBase)
CEditEffectSound::CEditEffectSound(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CEditEffectSound::IDD, pParent)
{
	m_pSoundNode = NULL;
}

CEditEffectSound::~CEditEffectSound()
{
}

void CEditEffectSound::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_cbLoopEnable);
	DDX_Control(pDX, IDC_EDIT_SND_LOOPCNT, m_edLoopCount);
}


BEGIN_MESSAGE_MAP(CEditEffectSound, i3TDKDialogBase)
	ON_EN_CHANGE(IDC_EDIT_SND_LOOPCNT, OnEnChangeEditSndLoopcnt)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()


// CEditEffectSound message handlers
void CEditEffectSound::SetObject( i3ElementBase * pObj)
{
	m_pSoundNode = NULL;
	if( !i3::same_of<i3SoundNode*>(pObj)) //->IsExactTypeOf( i3SoundNode::static_meta())==FALSE)
		return;

	m_pSoundNode = (i3SoundNode*)pObj;
}

BOOL CEditEffectSound::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	i3SoundPlayInfo * pInfo = m_pSoundNode->getSoundPlayInfo();
	if( pInfo == NULL)
		return FALSE;

	if( pInfo->GetLoopMode() == I3SND_RT_LOOPMODE_ONESHOT)
	{
		m_cbLoopEnable.SetCheck( BST_UNCHECKED);

		m_edLoopCount.EnableWindow( FALSE);
	}
	else
	{
		m_cbLoopEnable.SetCheck( BST_CHECKED);
		m_edLoopCount.EnableWindow( TRUE);
	}

	char conv[MAX_PATH];

	sprintf( conv, "%d", m_pSoundNode->getLoopCount());
	m_edLoopCount.SetWindowText( conv);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditEffectSound::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	i3SoundPlayInfo * pInfo = m_pSoundNode->getSoundPlayInfo();
	if( pInfo == NULL)
		return ;

	if( m_cbLoopEnable.GetCheck() == BST_CHECKED)
	{
		pInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);

		char conv[MAX_PATH];

		m_edLoopCount.GetWindowText( conv, MAX_PATH -1);

		m_pSoundNode->setLoopCount((UINT32)atoi( conv));
	}
	else
	{
		pInfo->setLoopMode( I3SND_RT_LOOPMODE_ONESHOT);

		m_pSoundNode->setLoopCount(1);
	}

	i3TDKDialogBase::OnOK();
}

void CEditEffectSound::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	i3TDKDialogBase::OnCancel();
}

void CEditEffectSound::OnEnChangeEditSndLoopcnt()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the i3TDKDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*
	if( m_edLoopCount.GetModify())
	{
		char conv[MAX_PATH];

		m_edLoopCount.GetWindowText( conv, MAX_PATH -1);

		m_pSoundNode->setLoopCount((UINT32)atoi( conv));
	}*/
}

void CEditEffectSound::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	if( m_cbLoopEnable.GetCheck() == BST_CHECKED)
	{
		m_edLoopCount.EnableWindow( TRUE);
	}
	else
	{
		m_edLoopCount.EnableWindow( FALSE);
	}
}
