// SetChannelDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimationKey.h"
#include "SetChannelDialog.h"
#include ".\setchanneldialog.h"


// CSetChannelDialog dialog

IMPLEMENT_DYNAMIC(CSetChannelDialog, CDialog)
CSetChannelDialog::CSetChannelDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetChannelDialog::IDD, pParent)
{
	m_pAnim = NULL;
	m_pSeq = NULL;
}

CSetChannelDialog::~CSetChannelDialog()
{
}

void CSetChannelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_TRANSLATE, m_TranslateCtrl);
	DDX_Control(pDX, IDC_CHK_ROTATION, m_RotateCtrl);
	DDX_Control(pDX, IDC_CHK_SCALE, m_ScaleCtrl);
	DDX_Control(pDX, IDC_CHK_TIME, m_TimeCtrl);
	DDX_Control(pDX, IDC_CK_FIXED_TRANSLATE, m_FixedTranslateCtrl);
	DDX_Control(pDX, IDC_CHK_FIXED_ROTATION, m_FixedRotateCtrl);
	DDX_Control(pDX, IDC_CHK_FIXED_SCALE, m_FixedScaleCtrl);
}

i3TransformSequence *	CSetChannelDialog::Execute( i3Animation * pAnim, i3TransformSequence * pSeq)
{
	m_pAnim = pAnim;
	m_pSeq = pSeq;

	DoModal();

	return m_pSeq;
}

BEGIN_MESSAGE_MAP(CSetChannelDialog, CDialog)
END_MESSAGE_MAP()


// CSetChannelDialog message handlers

BOOL CSetChannelDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		if( m_pSeq->HasTranslation())
			m_TranslateCtrl.SetCheck( BST_CHECKED);

		if( m_pSeq->HasRotation())
			m_RotateCtrl.SetCheck( BST_CHECKED);

		if( m_pSeq->HasScale())
			m_ScaleCtrl.SetCheck( BST_CHECKED);

		if( m_pSeq->HasTime())
			m_TimeCtrl.SetCheck( BST_CHECKED);

		if( m_pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE)
			m_FixedTranslateCtrl.SetCheck( BST_CHECKED);

		if( m_pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE)
			m_FixedRotateCtrl.SetCheck( BST_CHECKED);

		if( m_pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE)
			m_FixedScaleCtrl.SetCheck( BST_CHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetChannelDialog::CopyTransSeq( i3TransformSequence * pDest, i3TransformSequence * pSrc)
{
	UINT32 i;
	VEC3D vec;
	QUATERNION q;

	for( i = 0; i < pSrc->GetKeyframeCount(); i++)
	{
		if( pSrc->HasTranslation())
		{
			pSrc->GetTranslation( i, &vec);
			pDest->SetTranslation( i, &vec);
		}
		else
		{
			i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);
			pDest->SetTranslation( i, &vec);
		}

		if( pSrc->HasRotation())
		{
			pSrc->GetRotation( i, &q);
			pDest->SetRotation( i, &q);
		}
		else
		{
			i3Quat::Identity( &q);
			pDest->SetRotation( i, &q);
		}

		if( pSrc->HasScale())
		{
			pSrc->GetScale( i, &vec);
			pDest->SetScale( i, &vec);
		}
		else
		{
			i3Vector::Set( &vec, 1.0f,1.0f, 1.0f);
			pDest->SetScale( i, &vec);
		}

		if( pSrc->HasTime())
		{
			REAL32 tm;

			tm = pSrc->GetTime( i);
			pDest->SetTime( i, tm);
		}
	}

	pDest->SetKeyframeInterval( pSrc->GetKeyframeInterval());
}

void CSetChannelDialog::OnOK()
{
	UINT32 newChannel = 0;
	//INT32 i;

	if( m_TranslateCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::TRANSLATE;

	if( m_RotateCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::ROTATE;

	if( m_ScaleCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::SCALE;

	if( m_FixedTranslateCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::FIXED_TRANSLATE;

	if( m_FixedRotateCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::FIXED_ROTATE;

	if( m_FixedScaleCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::FIXED_SCALE;

	if( m_TimeCtrl.GetCheck() == BST_CHECKED)
		newChannel |= (UINT32) i3TransformSequence::TIME;


	if( (UINT32) m_pSeq->GetKeyframeChannels() != newChannel)
	{
		i3TransformSequence * pNewSeq;
		UINT32 i;

		pNewSeq = i3TransformSequence::new_object_ref();

		pNewSeq->Create( m_pSeq->GetKeyframeCount(), m_pSeq->GetDuration(), (i3TransformSequence::I3_KEY_CHANNEL) newChannel);

		CopyTransSeq( pNewSeq, m_pSeq);

		for( i = 0; i < (UINT32) m_pAnim->GetTrackCount(); i++)
		{
			i3TransformSequence * pTemp = m_pAnim->GetTrack( i);

			if( pTemp == m_pSeq)
			{
				m_pAnim->SetTrack( i, pNewSeq, m_pAnim->GetTrackBoneName( i));

				m_pSeq = pNewSeq;
			}
		}
	}

	CDialog::OnOK();
}
