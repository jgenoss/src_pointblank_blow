// SpecActionPath.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionPath.h"
#include "GlobalVar.h"
#include ".\specactionpath.h"

#include "i3Base/string/ext/make_relative_path.h"

// SpecActionPath dialog

IMPLEMENT_DYNAMIC(SpecActionPath, i3TDKDialogBase)
SpecActionPath::SpecActionPath(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(SpecActionPath::IDD, pParent)
{
	m_pAction = NULL;
}

SpecActionPath::~SpecActionPath()
{
}

void SpecActionPath::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_ANIMPATH, m_ED_AnimPath);
	DDX_Control(pDX, IDC_CB_PLAYMODE, m_CB_PlayMode);
}

void SpecActionPath::SetObject( i3ElementBase * pObj)
{
	m_pAction = (i3ActionPath *) pObj;

	i3Animation * pAnim = m_pAction->getAnim();

	if( pAnim != NULL)
	{
		m_ED_AnimPath.SetWindowText( pAnim->GetName());
	}
	else
	{
		m_ED_AnimPath.SetWindowText( "");
	}

	m_CB_PlayMode.SetCurSel( m_pAction->getPlayMode());
}

BEGIN_MESSAGE_MAP(SpecActionPath, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_ANIMPATH, OnBnClickedBtnAnimpath)
	ON_CBN_SELCHANGE(IDC_CB_PLAYMODE, OnCbnSelchangeCbPlaymode)
END_MESSAGE_MAP()


// SpecActionPath message handlers

void SpecActionPath::OnBnClickedBtnAnimpath()
{
	CFileDialog	Dlg( TRUE, "i3A", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Animation File(*.i3A)|*.i3A||", this);

	if( m_pAction == NULL)
		return;

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3Animation * pAnim;

	pAnim = i3Animation::LoadFromFile( LPCTSTR( Dlg.GetPathName()));

	if( pAnim == NULL)
	{
		NotifyBox( m_hWnd, "%s 파일을 읽을 수 없음", LPCTSTR( Dlg.GetPathName()));
		return;
	}

	char szRelPath[ MAX_PATH];

	i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR( Dlg.GetPathName()), szRelPath );
	i3::make_unix_path( szRelPath);

	pAnim->SetName( szRelPath);
	pAnim->SetExternRes( TRUE);

	m_pAction->setAnim( pAnim);

	m_ED_AnimPath.SetWindowText( szRelPath);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pAction);
}

void SpecActionPath::OnCbnSelchangeCbPlaymode()
{
	m_pAction->setPlayMode( (I3_ANIM_PLAY_MODE) m_CB_PlayMode.GetCurSel());
}

void SpecActionPath::OnOK(void)
{
}

void SpecActionPath::OnCancel(void)
{
}
