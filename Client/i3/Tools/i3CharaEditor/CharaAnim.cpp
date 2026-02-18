#include "stdafx.h"
#include "Chara.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////////////////

class AniPackFileProgress : public i3AniPackFileProgress
{
public:
	enum ECTL
	{
		ECTL_PB_PHASE,
		ECTL_ST_PHASE_REMAIN,
		ECTL_ST_PHASE_DESC,

		ECTL_PB_TASK,
		ECTL_ST_TASK_REMAIN,
		ECTL_ST_TASK_DESC,
		
		ECTL_MAX
	};

	AniPackFileProgress(HWND parent=NULL)
	{
		m_wnd = NULL;
		m_wndParent = parent;

		memset(m_ctl, 0, sizeof(m_ctl));
		m_bContinueTask = true;

		Show(false);
	}

	~AniPackFileProgress()
	{
		OnDestroyWnd();
	}

	virtual bool OnReport(EACT act)
	{
		switch(m_phase)
		{
		case EPHASE_SAVE_START:
			{
				if(act == EACT_BEGIN)
				{
					SetProgressInfo(ECTL_PB_PHASE, m_arg0- EPHASE_SAVE_START, m_arg1- EPHASE_SAVE_START);
					SetText(ECTL_ST_PHASE_DESC, "%d/%d", m_arg0- EPHASE_SAVE_START, m_arg1- EPHASE_SAVE_START);
					SetProgress(ECTL_PB_PHASE, m_arg0);

					Show(true);
				}
			}
			break;

		case EPHASE_SAVE_CONV_I3ANIM_TO_I3ANIM2:
			{
				if(act == EACT_BEGIN)
				{
					SetProgress(ECTL_PB_PHASE, m_phase - EPHASE_SAVE_START);
					SetText(ECTL_ST_PHASE_REMAIN, "%d/%d", m_phase - EPHASE_SAVE_START, m_arg1- EPHASE_SAVE_START);
					SetProgressInfo(ECTL_PB_TASK, 0, m_arg0);
				}
				else
				{
					SetText(ECTL_ST_TASK_REMAIN, "%d/%d", m_arg1, m_arg0);
					SetText(ECTL_ST_TASK_DESC, "%s", m_argText0.c_str());
					SetProgress(ECTL_PB_TASK, m_arg1);
				}
			}
			break;

		case EPHASE_SAVE_I3ANIM2_TABLE:
			{
				if(act == EACT_BEGIN)
				{
					SetProgress(ECTL_PB_PHASE, m_phase - EPHASE_SAVE_START);
					SetText(ECTL_ST_PHASE_REMAIN, "%d/%d", m_phase - EPHASE_SAVE_START, m_arg1- EPHASE_SAVE_START);
					SetText(ECTL_ST_PHASE_DESC, " ");

					SetText(ECTL_ST_TASK_REMAIN, "  ", m_arg0);
					SetText(ECTL_ST_TASK_DESC, "writing i3animation2 (%d)..", m_arg0);
					SetProgressInfo(ECTL_PB_TASK, 0, 0);
				}
			}
			break;

		case EPHASE_SAVE_I3ANIM2:
			{
				if(act == EACT_BEGIN)
				{
					SetProgress(ECTL_PB_PHASE, m_phase - EPHASE_SAVE_START);
					SetText(ECTL_ST_PHASE_DESC, "%d/%d", m_phase - EPHASE_SAVE_START, m_arg1- EPHASE_SAVE_START);

					SetText(ECTL_ST_TASK_DESC, "update and storing i3animation2 data (%d)..", m_arg0);
					SetProgressInfo(ECTL_PB_TASK, 0, m_arg0);
				}
				else
				{
					SetProgress(ECTL_PB_TASK, m_arg1);
				}
			}
			break;

		case EPHASE_SAVE_I3ANIM2_HEADER_AND_TABLE:
			{
				SetProgress(ECTL_PB_PHASE, m_phase - EPHASE_SAVE_START);
				SetText(ECTL_ST_PHASE_DESC, "%d/%d", m_phase - EPHASE_SAVE_START, m_arg1- EPHASE_SAVE_START);

				SetText(ECTL_ST_TASK_DESC, "writing i3Animation2 header and table..");
				SetProgressInfo(ECTL_PB_TASK, 0, 0);				
			}
			break;

		case EPHASE_SAVE_END:
			{
				Show(false);
			}
			break;

		default:
			return false;
		}

		PumpMsg();

		return true; // true for continue;
	}

	void Show(bool show)
	{
		if(!m_wnd)
		{
			m_wnd = CreateDialogParam((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ANIPACKFILE_PROGRESS),
				m_wndParent, AniPackFileProgress::DlgProc, (LPARAM)this);
		}

		if(m_wnd)
		{
			::ShowWindow(m_wnd, show ? SW_SHOW : SW_HIDE);

			if(show)
				::SetFocus(m_wnd);
		}
	}

	bool PumpMsg()
	{
		if(!m_wnd)
			return false;

		bool exitWnd = false;
		MSG msg;
		while(PeekMessage(&msg, m_wnd, 0, 0, PM_NOREMOVE))
		{
			if(!GetMessage(&msg, m_wnd, 0, 0))
			{
				// exit
				exitWnd = true;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return !exitWnd;
	}

	void SetText(ECTL item, const char* fmt, ...)
	{
		char tempstr[2048];

		if(item < ECTL_PB_PHASE || item >= ECTL_MAX)
			return;

		va_list marker;
		va_start(marker, fmt);

		vsprintf(tempstr, fmt, marker);
		va_end(marker);

		SetWindowText(m_ctl[item], tempstr);
	}

	void SetProgressInfo(ECTL ctl, INT32 minVal, INT32 maxVal)
	{
		SendMessage(m_ctl[ctl], PBM_SETRANGE, 0, MAKELONG(minVal, maxVal));
	}

	void SetProgress(ECTL ctl, INT32 val)
	{
		SendMessage(m_ctl[ctl], PBM_SETPOS, val, 0);
	}

private:

	void OnCancel()
	{
		m_bContinueTask = false;
		MessageBoxA(m_wnd, "지금 작업을 취소하시면 저장하려던 AnimPack파일에 일부쓰기작업이되어서"
			"\n손상되었을수있으니 해당파일을  복구하시기바랍니다.", "ERR!", MB_ICONEXCLAMATION);
	}

	void OnInitDialog(HWND hWnd)
	{
		m_wnd = hWnd;

		m_ctl[ECTL_PB_PHASE]		= GetDlgItem(m_wnd, IDC_I3ANIPAKPG_PB_PHASE);
		m_ctl[ECTL_ST_PHASE_REMAIN]	= GetDlgItem(m_wnd, IDC_I3ANIPAKPG_ST_PHASE_REMAIN);
		m_ctl[ECTL_ST_PHASE_DESC]	= GetDlgItem(m_wnd, IDC_I3ANIPAKPG_ST_PHASE_DESC);
		m_ctl[ECTL_PB_TASK]			= GetDlgItem(m_wnd, IDC_I3ANIPAKPG_PB_TASK);
		m_ctl[ECTL_ST_TASK_REMAIN]	= GetDlgItem(m_wnd, IDC_I3ANIPAKPG_ST_TASK_REMAIN);
		m_ctl[ECTL_ST_TASK_DESC]	= GetDlgItem(m_wnd, IDC_I3ANIPAKPG_ST_TASK_DESC);
	}

	void OnDestroyWnd()
	{
		if(m_wnd)
		{
			SetWindowLongPtr(m_wnd, DWL_USER, NULL);
			DestroyWindow(m_wnd);
			m_wnd = NULL;
		}
	}
	
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		AniPackFileProgress* inst = (AniPackFileProgress*)GetWindowLongPtr(hWnd, DWL_USER);

		switch(msg) 
		{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hWnd, DWL_USER, (LONG_PTR)lParam);
				inst = (AniPackFileProgress*)lParam;
				inst->OnInitDialog(hWnd);
			}
			return TRUE;

		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
				case IDCANCEL:
					{					
						if(inst)
						{
							inst->OnCancel();
							inst->OnDestroyWnd();
						}
						else
							DestroyWindow(hWnd);
					}
					return TRUE;

				default:
					return FALSE;
				}
			}
			break;

		case WM_DESTROY:
			{
				if(inst)
					inst->OnDestroyWnd();
			}
			break;
		}

		return FALSE;
	}

	////////////////////////////////////////////////////////////

	HWND							m_wnd;
	HWND							m_wndParent;

	HWND							m_ctl[ECTL_MAX];
	bool							m_bContinueTask;
};



/////////////////////////////////////////////////////////////////////////////////////////
//
void	CChara::AddAnim( i3Animation * pAnim)
{
	I3ASSERT( pAnim != NULL);

	if( FindAnimByPath( pAnim->GetName()) > -1)
	{
		CharaEditorNotifyBox( NULL, "The animation file is already exist (%s)\n", pAnim->GetName());
		return;
	}

	pAnim->AddRef();

	m_AnimList.push_back( pAnim);

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pAnim);
}

void	CChara::RemoveAnim( i3Animation * pAnim)
{
	INT32 idx;

	idx = FindAnim( pAnim);
	I3ASSERT( idx != -1);

	RemoveAnim( idx);
}

void	CChara::RemoveAnim( INT32 idx)
{
	I3ASSERT( idx < (INT32)m_AnimList.size());

	i3Animation * pAnim = (i3Animation *) getAnim( idx);

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pAnim);

	pAnim->Release();
	m_AnimList.erase(m_AnimList.begin() + idx);	
}

void	CChara::RemoveAllAnim(void)
{
	INT32 i;
	i3Animation * pAnim;

	for( i = 0; i < getAnimCount(); i++)
	{
		pAnim = getAnim( i);

		pAnim->Release();
	}

	m_AnimList.clear();

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, i3Animation::static_meta());
}

INT32	CChara::FindAnim( i3Animation * pAnim)
{
	size_t idx = i3::vu::index_of(m_AnimList, pAnim);
	return	(idx != m_AnimList.size()) ? (INT32)idx : -1; 
}

INT32	CChara::FindAnimByPath( const char * pszPath)
{
	INT32 i;
	i3Animation * pAnim;

	for( i = 0; i < getAnimCount(); i++)
	{
		pAnim = getAnim( i);

		if( i3::generic_is_iequal( pszPath, pAnim->GetName()) )
			return i;
	}

	return -1;
}

bool CChara::LoadAnim( const char * pszPath)
{
	i3Animation * pAnim;

	pAnim = i3Animation::LoadFromFile( pszPath);
	if( pAnim == NULL)
		return false;

	// 이름을 지정...
	// 항상 Working Folder를 기준으로 한, Relative Path가
	// 이름으로 지정되어야 한다.
	{
		char szRelPath[ MAX_PATH];

		i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szRelPath );
		i3::make_unix_path( szRelPath);

		pAnim->SetName( szRelPath);
	}

	pAnim->SetExternRes( TRUE);

	AddAnim( pAnim);

	return true;
}

void CChara::OptimizeAnim(void)
{
	INT32 i;
	i3Animation * pAnim;
	i3OptAnimation opt;
	char szFull[MAX_PATH];

	for( i = 0; i < getAnimCount(); i++)
	{
		pAnim = getAnim( i);

		opt.OptimizeAnim( pAnim);


		sprintf( szFull, "%s/%s", g_pOption->m_szWorkPath, pAnim->GetName());

		pAnim->SetExternRes( FALSE);
		pAnim->SaveToFile( szFull);
		pAnim->SetExternRes( TRUE);
	}
}

void CChara::BuildAnimPackFile( char * pszPath)
{
	i3AnimationPackFile file;
	AniPackFileProgress progress(NULL);

	file.SaveToFile( pszPath, m_AnimList, &progress);
}

void CChara::RenamePath( char * pszPath, const INT32 nPathMax, const char * pszTargetName, const char * pszNewName)
{	
	INT32	nSamePos = 0;
	BOOL	bSame = FALSE;

	I3ASSERT( pszTargetName != NULL);
	I3ASSERT( pszNewName != NULL);

	INT32	iTargetNameLen = i3::generic_string_size( pszTargetName );
	if( iTargetNameLen == 0 )	return;
	INT32	iNewNameLen = i3::generic_string_size( pszNewName );
	if( iNewNameLen == 0 )	return;

	if( i3::contain_string_head( (const char*) pszPath, pszTargetName)  )
	{
		INT32 length = i3::generic_string_size( pszPath );
		for( INT32 j = 0; j < length; j++ )
		{
			for( INT32 k = 0; k < iTargetNameLen; k++ )
			{
				if( j + k < length )
				{
					if( pszPath[j + k] == pszTargetName[k] )
					{
						bSame = TRUE;
					}
					else
					{
						bSame = FALSE;
						break;
					}
				}
				else
				{
					bSame = FALSE;
					break;
				}
			}

			if( bSame )
			{
				nSamePos = j;
				break;
			}
		}

		if( bSame )
		{// 같은 단어가 있습니다.
			// 스플릿해줍니다.
			char	szName[ MAX_PATH ] = {0,};
			i3mem::Copy( szName, (void*) pszPath, nSamePos );
			i3::generic_string_cat( szName, pszNewName );

			char szTemp[MAX_PATH] = {0,};
			i3mem::Copy( szTemp, (void*) &pszPath[ nSamePos + iTargetNameLen], length - nSamePos - iTargetNameLen );
			i3::generic_string_cat( szName, szTemp );

			i3::safe_string_copy( pszPath, szName, nPathMax );
		}
	}
}

void CChara::RenameFolder( char * pszFilterName, char * pszNewFolderName )
{
	INT32	i, j, k;
	BOOL	bSame;
	INT32	nSamePos;
	i3Animation * pAnim;
	char	szOriginName[ MAX_PATH];
	char	szAIName[ MAX_PATH];

	INT32	nFilterLength = i3::generic_string_size( pszFilterName );
	if( nFilterLength == 0 )	return;
	INT32	nFolderLength = i3::generic_string_size( pszNewFolderName );
	if( nFolderLength == 0 )	return;

	i3::to_upper( pszFilterName);
	i3::to_upper( pszNewFolderName);

	for( i = 0; i < getAnimCount(); i++ )
	{
		bSame = FALSE;
		nSamePos = 0;

		pAnim = getAnim( i );
		i3::safe_string_copy( szOriginName, pAnim->GetName(), MAX_PATH );
		i3::to_upper( szOriginName);

		if( i3::contain_string_head( szOriginName, pszFilterName)  )
		{
			INT32 length = i3::generic_string_size( szOriginName );
			for( j = 0; j < length; j++ )
			{
				for( k = 0; k < nFilterLength; k++ )
				{
					if( j + k < length )
					{
						if( szOriginName[j + k] == pszFilterName[k] )
						{
							bSame = TRUE;
						}
						else
						{
							bSame = FALSE;
							break;
						}
					}
					else
					{
						bSame = FALSE;
						break;
					}
				}

				if( bSame )
				{
					nSamePos = j;
					break;
				}
			}

			if( bSame )
			{// 같은 단어가 있습니다.
				// 스플릿해줍니다.
				char	szName[ MAX_PATH ] = {0,};
				i3mem::Copy( szName, (void*) szOriginName, nSamePos );
				i3::generic_string_cat( szName, pszNewFolderName );
				char szTemp[MAX_PATH] = {0,};
				i3mem::Copy( szTemp, (void*) &szOriginName[ nSamePos + nFilterLength], length - nSamePos - nFilterLength );
				i3::generic_string_cat( szName, szTemp );

				i3Animation * pNewAnim = FindAnimFile( (const char*) szName);
				if( pNewAnim == NULL)
					continue;

				AddAnim( pNewAnim);
				pNewAnim->SetExternRes();
				pNewAnim->SetName( szName );

				// AIState의 Animation을 찾아서 다시 세팅합니다.
				for( j = 0; j < getAICount(); j++)
				{
					i3AI * pAI = getAI( j)->getAI();
					for( k = 0; k < pAI->getAIStateCount(); k++)
					{
						i3AIState * pAIState = pAI->getAIState( k);
						if( pAIState->getAnim() == NULL)
							continue;

						i3::safe_string_copy( szAIName, pAIState->getAnim()->GetName(), MAX_PATH );
						i3::to_upper( szAIName);

						if( i3::generic_is_iequal( szAIName, szOriginName) )
							pAIState->SetAnim( pNewAnim);
					}
				}

				RemoveAnim( i);
				i--;
				i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, pNewAnim);
			}
		}
	}

}

i3Animation * CChara::FindAnimFile( const char * pszPath)
{
	char szRelPath[ MAX_PATH];

	sprintf( szRelPath, "%s\\%s", g_pOption->m_szWorkPath, pszPath);
	i3::make_unix_path( szRelPath);

	WIN32_FIND_DATA info;
	HANDLE hFind = FindFirstFile( szRelPath, &info);

	if( hFind == INVALID_HANDLE_VALUE)
		return NULL;

	FindClose( hFind);

	i3Animation * pAnim = i3Animation::LoadFromFile( szRelPath);
	return pAnim;
}
