// DlgRenameFolder.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgRenameFolder.h"
#include ".\dlgrenamefolder.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgRenameFolder dialog

IMPLEMENT_DYNAMIC(CDlgRenameFolder, CDialog)
CDlgRenameFolder::CDlgRenameFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRenameFolder::IDD, pParent)
{
	i3mem::FillZero( m_szFilter, MAX_PATH );
	i3mem::FillZero( m_szChangeName, MAX_PATH );
	m_nRenameType = RENAME_TYPE_ANIMATIONFOLDER;
	m_pAI = NULL;
}

CDlgRenameFolder::~CDlgRenameFolder()
{
}

void CDlgRenameFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FOLDER_FILTER, m_EdtFilterNameCtrl);
	DDX_Control(pDX, IDC_EDIT_FOLDER_RENAME, m_EdtChangeNameCtrl);
}


BEGIN_MESSAGE_MAP(CDlgRenameFolder, CDialog)
	ON_EN_CHANGE(IDC_EDIT_FOLDER_FILTER, OnEnChangeEditFolderFilter)
	ON_EN_CHANGE(IDC_EDIT_FOLDER_RENAME, OnEnChangeEditFolderRename)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgRenameFolder message handlers

void CDlgRenameFolder::OnEnChangeEditFolderFilter()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_EdtFilterNameCtrl.GetWindowText( m_szFilter, MAX_PATH - 1 );
}

void CDlgRenameFolder::OnEnChangeEditFolderRename()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_EdtChangeNameCtrl.GetWindowText( m_szChangeName, MAX_PATH - 1);
}

void CDlgRenameFolder::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

i3Animation * CDlgRenameFolder::FindAnimFromAnimList( const char * pszName)
{
	for( INT32 i = 0; i < g_pChara->getAnimCount(); i++)
	{
		i3Animation * pAnim = g_pChara->getAnim( i);
		if( i3::contain_string_head( (const char*) pAnim->GetName(), pszName) )
		{
			return pAnim;
		}
	}

	return NULL;
}

INT32 CDlgRenameFolder::FindStartPosSameName( const char * pszName, const char * pszCon)
{
	I3ASSERT( pszName != NULL);
	I3ASSERT( pszCon != NULL);

	INT32 iNameLen = i3::generic_string_size( pszName);
	INT32 iConLen = i3::generic_string_size( pszCon);

	INT32 iStart = -1;

	for( INT32 i = 0; i < iConLen; i++)
	{
		for( INT32 j = 0; j < iNameLen; j++)
		{
			if( pszName[ j] == pszCon[ i])
			{
				if( iStart == -1)
					iStart = j;
				i++;

				if( i >= iConLen)
					break;
			}
			else
			{
				iStart = -1;
				i = 0;
			}
		}

		if( iStart != -1)
			break;
	}

	return iStart;
}

bool CDlgRenameFolder::CheckTrueSameInclude( const char * pszPath, const char * pszName,  INT32 iStartPos)
{
	I3ASSERT( pszPath != NULL);
	I3ASSERT( pszName != NULL);

	INT32 iPathLen = i3::generic_string_size( pszPath);
	INT32 iNameLen = i3::generic_string_size( pszName);

	if( iStartPos == -1)
	{
		iStartPos = FindStartPosSameName( pszPath, pszName);
		I3ASSERT( iStartPos != -1);
	}

	bool bRv = true;
	for( INT32 i = iStartPos + iNameLen; i < iPathLen; ) //i++)  // 아랫줄 break때문에 증가문이 의미가 없음(경고때문에 증가문 제거)
	{
		if( (pszPath[ i] == '\\') || (pszPath[ i] == '/') )
		{
			break;
		}

		bRv = false;
		break;
	}

	return bRv;
}

i3Animation * CDlgRenameFolder::CheckAnimFileName( const char * pszPath, const char * pszName)
{
	return i3Animation::LoadFromFile( pszPath);
}

void CDlgRenameFolder::ChangeAnim(void)
{
	I3ASSERT( m_pAI != NULL);

	char szAnimName[ MAX_PATH];
	char szFileName[ MAX_PATH];
	char szDestAnimName[ MAX_PATH];
	char szLog[ 512];

	i3::to_upper( m_szFilter);
	i3::to_upper( m_szChangeName);
				
	for( INT32 i = 0; i < m_pAI->getAI()->getAIStateCount(); i++)
	{
		i3AIState * pAIState = m_pAI->getAI()->getAIState( i);
		if( pAIState->getAnim() == NULL)
			continue;

		i3::safe_string_copy( szAnimName, pAIState->getAnim()->GetName(), MAX_PATH );
		i3::to_upper( szAnimName);
		
		if( i3::contain_string( szAnimName, m_szFilter) < 0)
			continue;

		INT32 iAnimNameLen = FindStartPosSameName( szAnimName, m_szFilter);
		I3ASSERT( iAnimNameLen != -1);

//		i3String::SplitFileName( pAIState->getAnim()->GetName(), szFileName, FALSE);
		i3::extract_filetitle( pAIState->getAnim()->GetName(), szFileName);

		i3::to_upper( szFileName);
		for( INT32 j = 0; j < g_pChara->getAnimCount(); j++)
		{	
			i3Animation * pAnim = g_pChara->getAnim( j);
			i3::safe_string_copy( szDestAnimName, pAnim->GetName(), MAX_PATH );
			i3::to_upper( szDestAnimName);

			if( i3::contain_string( (const char*) szDestAnimName, szFileName) < 0)
				continue;
			
			INT32 iRv = i3::contain_string( szDestAnimName, m_szChangeName);
			if( iRv < 0)
				continue;

			INT32 iAnimDestLen = FindStartPosSameName( szDestAnimName, m_szChangeName);
			I3ASSERT( iAnimDestLen != -1);

			if( CheckTrueSameInclude( szDestAnimName, m_szChangeName, iAnimDestLen) == false)
				continue;

			i3Animation * pNewAnim = CheckAnimFileName( szDestAnimName, m_szChangeName);
			if( pNewAnim == NULL)
				continue;
			
			pAIState->SetAnim( pNewAnim);

			char szPrintName[ MAX_PATH];
			i3mem::Copy( szPrintName, &szAnimName[ iAnimNameLen], i3::generic_string_size( szAnimName) - iAnimNameLen);
			szPrintName[ i3::generic_string_size( szAnimName) - iAnimNameLen] = '\0';

			char szPrintDestName[ MAX_PATH];
			i3mem::Copy( szPrintDestName, &szDestAnimName[ iAnimDestLen], i3::generic_string_size( szDestAnimName) - iAnimDestLen);
			szPrintDestName[ i3::generic_string_size( szDestAnimName) - iAnimDestLen] = '\0';

			sprintf( szLog, "Change Anim %s -> %s", szPrintName, szPrintDestName);
			I3PRINTLOG(I3LOG_NOTICE, szLog);
		}
	}
}

void CDlgRenameFolder::OnOK()
{
	if( (i3::generic_string_size( m_szFilter ) > 0) && (i3::generic_string_size( m_szChangeName) > 0) )
	{
		switch( m_nRenameType)
		{
		case RENAME_TYPE_ANIMATIONFOLDER :	g_pChara->RenameFolder( m_szFilter, m_szChangeName );	break;
		case RENAME_TYPE_AUTOAISTATEANIM :		ChangeAnim();		break;
		}
	}

	CDialog::OnOK();
}
