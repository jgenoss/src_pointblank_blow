// EditDlgAction_PlaySound.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "EditDlgAction_PlaySound.h"
#include "GlobalVariable.h"

// CEditDlgAction_PlaySound dialog

IMPLEMENT_DYNAMIC(CEditDlgAction_PlaySound, CDialog)
CEditDlgAction_PlaySound::CEditDlgAction_PlaySound(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlgAction_PlaySound::IDD, pParent)
{
	m_pNode = NULL;
}

CEditDlgAction_PlaySound::~CEditDlgAction_PlaySound()
{
}

void CEditDlgAction_PlaySound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITACTION_SOUND_SELFILE, m_edSoundFile);
	DDX_Control(pDX, IDC_EDITACTION_SOUNDVOLUME, m_edVolume);
	DDX_Control(pDX, IDC_EDITACTION_SOUNDSTARTTIME, m_edStartTime);
	DDX_Control(pDX, IDC_CKB_LOOPPLAY, m_cbLoopPlay);
	DDX_Control(pDX, IDC_EDITACTION_LOOPCOUNT, m_edLoopCount);
	DDX_Control(pDX, IDC_CB_SOUNDTYPE, m_cbSoundType);
	DDX_Control(pDX, IDC_CB_3DMODE, m_cb3DMode);
	DDX_Control(pDX, IDC_ED_SOUND_FILESELECT, m_cbSelectSoundFile);
}


BEGIN_MESSAGE_MAP(CEditDlgAction_PlaySound, CDialog)
	ON_BN_CLICKED(IDC_ED_SOUND_FILESELECT, OnBnClickedEdSoundFileselect)
END_MESSAGE_MAP()

void CEditDlgAction_PlaySound::SetSoundEventData( i3SoundNode * pNode)
{
	m_pNode = pNode;
}

// CEditDlgAction_PlaySound message handlers
BOOL CEditDlgAction_PlaySound::OnInitDialog()
{
	CDialog::OnInitDialog();

	I3ASSERT( m_pNode != NULL);

	char conv[MAX_PATH];
	conv[0] = 0;


	i3SoundPlayInfo * pInfo = m_pNode->getSoundPlayInfo();
	// TODO:  Add extra initialization here
	{
		if( pInfo->getSound() != NULL)
		{
			i3String::Copy( conv, pInfo->getSound()->GetName());
		}
		
		m_edSoundFile.SetWindowText( conv);
	}

	{
		m_cbSoundType.AddString( "BGM");
		m_cbSoundType.AddString( "VOICE");
		m_cbSoundType.AddString( "EFFECT");

		INT32 i = 0;
		for( i = 0; i < 3; ++i)
		{
			UINT32 n = I3SND_PROP_TYPE_BGM << i;
			if( pInfo->GetSoundType() == n)
			{
				m_cbSoundType.SetCurSel( i);
			}
		}
	}

	{
		m_cb3DMode.AddString( "3DMODE_DISABLE");
		m_cb3DMode.AddString( "3DMODE_CAMERA_SPACE");
		m_cb3DMode.AddString( "3DMODE_WORLD_SPACE");
		
		INT32 n3DMode = pInfo->Get3DMode();
		n3DMode -= I3SND_RT_3DMODE_DISABLE;
		m_cb3DMode.SetCurSel( n3DMode);
	}

	{
		sprintf( conv, "%d", pInfo->GetDefaultVolume());
		m_edVolume.SetWindowText( conv);
	}

	{
		sprintf( conv, "%f", m_pNode->getStartTime());
		m_edStartTime.SetWindowText( conv);
	}

	{
		if( pInfo->GetLoopMode() == I3SND_RT_LOOPMODE_LOOPING)
		{
			m_cbLoopPlay.SetCheck( BST_CHECKED);
		}
		else
		{
			m_cbLoopPlay.SetCheck( BST_UNCHECKED);
		}
	}

	{
		sprintf( conv, "%d", m_pNode->getLoopCount());
		m_edLoopCount.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlgAction_PlaySound::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	I3ASSERT( m_pNode != NULL);

	UpdateData( TRUE);
	
	char conv[MAX_PATH];
	conv[0] = 0;
	
	{
		i3SoundPlayInfo * pInfo = m_pNode->getSoundPlayInfo();

		INT32 nStyle = 0;

		//Type
		nStyle |= I3SND_PROP_TYPE_BGM << m_cbSoundType.GetCurSel();

		//3DMode
		nStyle |= I3SND_RT_3DMODE_DISABLE + m_cb3DMode.GetCurSel();


		if( m_cbLoopPlay.GetCheck() == BST_CHECKED)
		{
			nStyle |= I3SND_RT_LOOPMODE_LOOPING;
		}

		pInfo->SetStyle( nStyle);

		m_edVolume.GetWindowText( conv, MAX_PATH -1);
		pInfo->SetDefaultVolume( atoi( conv));

		m_edStartTime.GetWindowText( conv, MAX_PATH - 1);
		m_pNode->setStartTime( (RT_REAL32)atof( conv));

		m_edLoopCount.GetWindowText( conv, MAX_PATH -1);
		m_pNode->setLoopCount( atoi( conv));
	}

	CDialog::OnOK();
}

void CEditDlgAction_PlaySound::OnBnClickedEdSoundFileselect()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "WAV", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"PCM Wave File(*.wav)| *.wav||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3SoundPlayInfo * pInfo = m_pNode->getSoundPlayInfo();

	i3SoundFile File;

	i3Sound * pSound = File.Load( (LPCSTR)Dlg.GetFileName());
	if( pSound != NULL)
	{
		pInfo->setSound( pSound);
	}
	else
	{
		I3WARN( "File을 읽어들이지 못하였습니다");
		return ;
	}

	char szFilePath[MAX_PATH];
	char szFullPath[MAX_PATH];

	GetCurrentDirectory( MAX_PATH - 1, szFilePath);
	sprintf( szFullPath, "%s/%s", szFilePath, (LPCSTR)Dlg.GetFileName());

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, szFullPath, szFilePath);

	pSound->SetExternRes();
	pSound->SetName( szFilePath);

	m_edSoundFile.SetWindowText( pSound->GetName());
}
