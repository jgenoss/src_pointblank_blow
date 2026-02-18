// DlgSelectSound.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgSelectSound.h"
#include "GlobalVar.h"
#include ".\dlgselectsound.h"

#include "i3Base/string/ext/extract_filename.h"

// CDlgSelectSound dialog

IMPLEMENT_DYNAMIC(CDlgSelectSound, CDialog)
CDlgSelectSound::CDlgSelectSound(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectSound::IDD, pParent)
{
}

CDlgSelectSound::~CDlgSelectSound()
{
}

void CDlgSelectSound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOUND_LIST, m_SoundListCtrl);
}

i3Sound * CDlgSelectSound::Execute(void)
{
	m_pSelectedSound = NULL;

	if( DoModal() == IDCANCEL)
		return NULL;

	return m_pSelectedSound;
}

BEGIN_MESSAGE_MAP(CDlgSelectSound, CDialog)
END_MESSAGE_MAP()


// CDlgSelectSound message handlers

BOOL CDlgSelectSound::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_SoundListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 120);
		m_SoundListCtrl.InsertColumn( 1, "Length", LVCFMT_LEFT, 80);

		m_SoundListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		INT32 i, idx;
		i3Sound * pSound;
		char szName[MAX_PATH];

		for( i = 0; i < g_pChara->getSoundCount(); i++)
		{
			pSound = g_pChara->getSound( i);

		//	i3String::SplitFileName( pSound->GetName(), szName, FALSE);
			i3::extract_filetitle(pSound->GetName(), szName);
			idx = m_SoundListCtrl.InsertItem( i, szName, 0);

			// Set Sound data;
			m_SoundListCtrl.SetItemData( idx, (DWORD_PTR) pSound);

			sprintf( szName, "%.1f", pSound->GetDuration());
			m_SoundListCtrl.SetItemText( idx, 1, szName);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectSound::OnOK()
{
	POSITION pos = m_SoundListCtrl.GetFirstSelectedItemPosition();

	if( pos == NULL)
	{
		AfxMessageBox( "선택된 Sound가 없습니다.");
		return;
	}

	INT32 idx = m_SoundListCtrl.GetNextSelectedItem( pos);

	m_pSelectedSound = (i3Sound *) m_SoundListCtrl.GetItemData( idx);

	CDialog::OnOK();
}
