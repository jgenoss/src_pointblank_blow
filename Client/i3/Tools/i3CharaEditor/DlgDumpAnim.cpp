// DlgDumpAnim.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgDumpAnim.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_filename.h"


// CDlgDumpAnim 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDumpAnim, CDialog)

CDlgDumpAnim::CDlgDumpAnim(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDumpAnim::IDD, pParent)
{

}

CDlgDumpAnim::~CDlgDumpAnim()
{
}

void CDlgDumpAnim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DUMP_ANIM_PATH, m_edPath);
}


BEGIN_MESSAGE_MAP(CDlgDumpAnim, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgDumpAnim::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CDlgDumpAnim::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDumpAnim 메시지 처리기입니다.

void CDlgDumpAnim::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Open Folder"))
	{
		m_edPath.SetWindowText( dlg.GetSelectedFolderPath());
	}
}

void searchi3CHRFileList( const char * pszPath, std::map< UINT128, CDlgDumpAnim::CHRInfo> &list)
{
	CString strPath( pszPath);

	WIN32_FIND_DATA findFileData;

	strPath += "/*.*";
	HANDLE f = ::FindFirstFile( strPath.GetString(), &findFileData);

	if( f != INVALID_HANDLE_VALUE)
	{
		do 
		{
			std::string fileName = findFileData.cFileName;

			// 1. 자신/상위 폴더명은 제외
			if( (strcmp(findFileData.cFileName, "." ) == 0) ||
				(strcmp(findFileData.cFileName, ".." ) == 0) ||
				(strcmp(findFileData.cFileName, ".svn" )== 0) )
			{
				continue;
			}

			if( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString childFolder( pszPath);

				childFolder += "/";
				childFolder += findFileData.cFileName;
				searchi3CHRFileList( childFolder.GetString(), list);
			}
			else
			{
				char conv[ 32];
				i3::extract_fileext( findFileData.cFileName, conv);

				i3::to_upper( conv);
				if( i3::generic_is_equal( conv, "I3CHR"))
				{
					CDlgDumpAnim::CHRInfo info;
					UINT128 hash;

					info._Path = pszPath;
					info._Path += "/";
					info._Path += findFileData.cFileName;
					i3MD5::Gen( (char*)info._Path.c_str(), info._Path.length(), (char *) &hash);

					list.insert( std::map<UINT128, CDlgDumpAnim::CHRInfo>::value_type( hash, info));
				}
			}
			// 3. 다음 File Handle로 접근
		} while( ::FindNextFile( f, &findFileData) );
	}

	::FindClose( f);
}

void CDlgDumpAnim::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char path[ MAX_PATH];
	m_edPath.GetWindowTextA( path, MAX_PATH);

	// 하위 폴더에서 .i3Chr 파일을 찾는다.
	searchi3CHRFileList( path, m_CharaPathList);

	// CHR 파일에서 애니메이션 리스트를 읽는다.
	LoadAnimFromI3CHR();

	DumpFile( "E:\\AnimDump.txt");

	OnOK();
}


static void searchAIContext( i3GameNode * pChara, std::vector< i3AIContext*> &list)
{
	i3GameNode * pChild = pChara->getFirstChild();

	while( pChild != NULL)
	{
		if( i3::same_of<i3AIContext*>(pChild) )
		{
			list.push_back( (i3AIContext*)pChild);
		}

		searchAIContext( pChild, list);

		pChild = pChild->getNext();
	}
}

void CDlgDumpAnim::SearchAnimFromAICtx( std::map< UINT128, CHRInfo>::iterator &chrIt, const std::vector<i3AIContext*> &AIList)
{
	for( UINT32 i = 0; i < AIList.size(); i++)
	{
		i3AIContext * pAICtx = AIList.at(i);
		for( INT32 j = 0; j < pAICtx->getAIStateCount(); j++)
		{
			i3AIState * pState = pAICtx->getAIState( j);

			std::string path;

			if( pState->getAnimNameString().length() > 0)
			{
				path = pState->getAnimName();
			}
			else
			{
				if( pState->getAnim() != NULL)
				{
					path = pState->getAnim()->GetName();
				}
			}

			if( path.length() > 0)
			{
				UINT128 hash;
				i3MD5::Gen( (char*)path.c_str(), path.length(), (char *) &hash);

				std::map<UINT128, AnimInfo>::iterator it = m_AnimationPathList.find( hash);
				if( it != m_AnimationPathList.end())
				{
					bool bFind = false;
					for( UINT32 k = 0; k < it->second._ChrFileIndexList.size(); k++)
					{
						UINT128 chrIdx = it->second._ChrFileIndexList.at(k);
						if( chrIt->first == chrIdx)
						{
							bFind = true;
							break;
						}
					}
					if( bFind == false)
						it->second._ChrFileIndexList.push_back( chrIt->first);
				}
				else
				{
					AnimInfo info( path, chrIt->first);
					m_AnimationPathList.insert( std::map<UINT128,AnimInfo>::value_type(hash, info));
				}

				bool bFind = false;
				for( UINT32 k = 0; k < chrIt->second._AnimFileIndexList.size(); k++)
				{
					UINT128 animIdx = chrIt->second._AnimFileIndexList.at(k);
					if( hash == animIdx)
					{
						bFind = true;
						break;
					}
				}

				if( bFind == false)
					chrIt->second._AnimFileIndexList.push_back( hash);
			}
		}
	}
}

void CDlgDumpAnim::LoadAnimFromI3CHR( void)
{
	std::map<UINT128, CHRInfo>::iterator it = m_CharaPathList.begin();
	while( it != m_CharaPathList.end())
	{
		i3ResourceFile file;
		
		if( file.Load( it->second._Path.c_str()) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", it->second._Path.c_str());
			it++;
			continue;
		}

		std::vector<i3AIContext*> AICtxList;
		i3Chara * pChara = static_cast<i3Chara*>(file.getKeyObject());
		searchAIContext( pChara, AICtxList);

		SearchAnimFromAICtx( it, AICtxList);

		it++;
	}
}

void CDlgDumpAnim::DumpFile( const char * pszPath)
{
	char Temp[MAX_PATH];

	HANDLE hFile = ::CreateFile( pszPath, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
	{			
		::CloseHandle( hFile );
		return;
	}

	std::map<UINT128, CHRInfo>::iterator it = m_CharaPathList.begin();
	while( it != m_CharaPathList.end())
	{
		sprintf( Temp, "[%s]\r", it->second._Path.c_str());
		INT32 Len = i3::generic_string_size(Temp); 
		::WriteFile( hFile, Temp, Len, (LPDWORD)&Len, NULL );

		for( UINT32 i = 0; i < it->second._AnimFileIndexList.size(); i++)
		{
			UINT128 animIdx = it->second._AnimFileIndexList.at(i);
			std::map< UINT128, AnimInfo>::iterator animIt = m_AnimationPathList.find( animIdx);
			if( animIt != m_AnimationPathList.end())
			{
				sprintf( Temp, "  - %s\r", animIt->second._Path.c_str());
				Len = i3::generic_string_size(Temp);
				::WriteFile( hFile, Temp, Len, (LPDWORD)&Len, NULL );

				for( UINT32 j = 0; j < animIt->second._ChrFileIndexList.size(); j++)
				{
					UINT128 chrIdx = animIt->second._ChrFileIndexList.at(j);
					if( it->first != chrIdx)
					{	// 다른 CHR 파일
						std::map<UINT128, CHRInfo>::iterator chrIt = m_CharaPathList.find( chrIdx);
						if( chrIt != m_CharaPathList.end())
						{
							char conv[ MAX_PATH];
							i3::extract_filetitle( chrIt->second._Path.c_str(), conv);
							sprintf( Temp, "     -> Shared : %s\r", conv);
							Len = i3::generic_string_size(Temp);
							::WriteFile( hFile, Temp, Len, (LPDWORD)&Len, NULL );
						}
					}
				}
			}
		}

		it++;
	}

	::CloseHandle( hFile);
}
