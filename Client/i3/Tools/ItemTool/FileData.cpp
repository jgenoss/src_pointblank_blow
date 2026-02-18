#include "stdafx.h"
#include "FileData.h"
#include "FileManager.h"
#include <string>
#include <set>
#include <tchar.h>
#include <map>
#include <functional>
#include <algorithm>
#include <cctype>
#include "DlgFileLoader.h"

#define UM_UPDATE WM_USER

const std::string s_ignore::TYPE_STR[ MAX_IGNORE_TYPE ] = 
{
	_T("확장자"),
	_T("폴더 명"),
	_T("파일 명"),
};


bool s_sort::operator()( const CFileData* lhs, const CFileData* rhs ) const
{
	if( lhs->m_isDir && ! rhs->m_isDir ) return true;
	if( ! lhs->m_isDir && rhs->m_isDir ) return false;

	return lhs->m_name < rhs->m_name;
}

void _MakeDir( std::string& dir )
{
	if( dir.empty() )
	{
		return;
	}

	char wch = (*dir.rbegin());

	if( wch != _T('\\') && wch != _T('/') )
	{
		dir += _T("\\");
	}
}

std::deque< CFileData* >	CFileData::m_deads;

CFileData::CFileData() 
{
	ResetData();
	m_vExtensionType.resize(MAX_TYPE);
	m_vExtensionType[E_CHARAEDITOR] = "i3CharaEditor";
}

CFileData::~CFileData()
{	
	for each( std::pair< std::string, CFileData* > p in m_childs )
	{
		delete p.second;
	}
}

bool	CFileData::FindFile( mFileNameType& mFileNameCon,
							const std::string& rootDir,
							CString& strResName,
							const std::string& dir,
							CDialog* pDlg  )
{
	ClearChilds();

	std::string path = rootDir;
	
	_MakeDir( path );

	path += dir;
	_MakeDir( path );

	DWORD dwAtt = ::GetFileAttributes( path.c_str() );

	if( ((DWORD)-1) == dwAtt )
	{
		// invalid directory
		return false;
	}

	char prevPath[ MAX_PATH ];
	::GetCurrentDirectory( MAX_PATH, prevPath );
	::SetCurrentDirectory( path.c_str() );

	HANDLE hf = NULL; 
	WIN32_FIND_DATA findData;

	hf = ::FindFirstFile( _T("*.*"), &findData );

	if( INVALID_HANDLE_VALUE == hf )
	{
		return true;
	}

	do
	{
		std::string fileName = findData.cFileName;

		if( fileName == _T(".") || fileName == _T("..") || fileName == _T(".svn") )
		{
			continue;
		}

		CFileData* pChild = _AddChild( findData, dir );
		
		int nPos = pChild->m_name.rfind(".");
		if (nPos != -1)
		{
			std::string strExtenName	= pChild->m_name.substr(nPos+1,pChild->m_name.size());
			std::string strFileName		= pChild->m_name.substr(0, nPos);

			std::transform(strExtenName.begin(), strExtenName.end(), strExtenName.begin(), tolower);


			if (strExtenName.compare("i3charaeditor") == 0 || strExtenName.compare("i3s") == 0)
			{
				stFileInfo info;

				std::string strDir = pChild->GetMyDirectory();
				int nPos = strDir.find("\\");
				info.strRootName = strDir.substr(0,nPos);
				
				std::transform(strFileName.begin(), strFileName.end(), strFileName.begin(), tolower);

				if (info.strRootName.compare("Weapon") == 0)
				{
					info.strFileName = strDir;
					info.strFolderName = m_name;

					mFileNameType::iterator iter = mFileNameCon.find(strFileName);
					if (iter != mFileNameCon.end())
					{
						strFileName = GUTIL::InsertOverlapedFile(m_name.c_str(), strFileName.c_str());

						//I3TRACE("%s.i3charator 파일 이름이 겹칩니다. 파일이름을 임의로 변경합니다.\n", strFileName.c_str());
					}

					if (strExtenName.compare("i3s") == 0)
					{
						mFileNameCon.insert(make_pair(strFileName + "_I3S", info));
					}
					else
						mFileNameCon.insert(make_pair(strFileName, info));
				}
				else if (info.strRootName.compare("Chara") == 0)
				{
					//g_mFileNameChara[strFileName] = info;
				}

				strResName = pChild->GetMyDirectory().c_str();
				pDlg->PostMessage(UM_UPDATE);
			}
		}
		

		if( NULL != pChild )
		{
			if( false == pChild->m_isDir )
			{
				continue;
			}

			if( false == pChild->FindFile( mFileNameCon, rootDir, strResName, pChild->GetMyDirectory(),pDlg  ) )
			{
				pChild->ResetData();
				pChild->ClearChilds();
				m_deads.push_back( pChild );
				continue;
			}
		}

	} while( TRUE == ::FindNextFile( hf, &findData ) );

	::FindClose( hf );
	::SetCurrentDirectory( prevPath );
	return true;
}

void	CFileData::ClearChilds()
{
	for each( std::pair< std::string, CFileData* > p in m_childs )
	{
		p.second->ResetData();
		p.second->ClearChilds();
		CFileData::m_deads.push_back( p.second );
	}

	m_childs.clear();
	m_setchilds.clear();
}

void	CFileData::Init()
{
	const int nMax = 10000;
	for( int i = 0; i < nMax; ++i )
	{
		m_deads.push_back( new CFileData );
	}
}

void	CFileData::Release()
{
	for each( CFileData* p in m_deads )
	{
		delete p;
	}

	m_deads.clear();
}

void	CFileData::SetTreeItem( CTreeCtrl* pCtrl, HTREEITEM hParent )
{
	if( pCtrl )
		m_pTreeCtrl = pCtrl;

	if( m_pTreeCtrl )
	{
		m_hItem = m_pTreeCtrl->InsertItem( m_name.c_str(), 0, 0, hParent );
		
		for each( CFileData* ptr in m_setchilds )
		{
			ptr->SetTreeItem( pCtrl, m_hItem );
		}

		m_pTreeCtrl->SetItemData( m_hItem, (DWORD_PTR) this );

		//theApp.m_mapFileData.insert( std::make_pair( m_hItem, this ) );

		if( NULL == hParent )
			m_pTreeCtrl->Expand( m_hItem, TVE_EXPAND );

		_SetItemImage();
	}
}

void	CFileData::ResetData()
{
	m_name		= _T("");
	m_dir		= _T("");
	m_ext		= _T("");
	m_isIgnore	= false;
	m_isDir		= false;
	m_pParent	= NULL;
	m_pTreeCtrl	= NULL;
	m_hItem		= NULL;
}

void	CFileData::_SetFindFileData( const WIN32_FIND_DATA& data )
{
	m_name	= data.cFileName;
	m_isDir	= (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;

	if( false == m_isDir )
	{
		int _pos = m_name.rfind( _T('.') );
		m_ext		= m_name.substr( _pos +1 );		
		m_filename	= m_name.substr( 0, _pos );

		std::transform( m_ext.begin(), m_ext.end(), m_ext.begin(), std::tolower );
		std::transform( m_filename.begin(), m_filename.end(), m_filename.begin(), std::tolower );
	}
	else
	{
		m_ext = _T("");
	}
}


CFileData*	CFileData::_AddChild( const WIN32_FIND_DATA& data, const std::string& dir )
{
	CFileData* pNew = NULL;

	map_childs::iterator itr = m_childs.find( data.cFileName );

	if( itr != m_childs.end() )
	{
		ASSERT( FALSE );
		return itr->second;
	}

	if( false == m_deads.empty() )
	{	
		pNew = *m_deads.rbegin();
		m_deads.pop_back();
	}
	else
	{
		pNew = new CFileData;
	}

	pNew->ResetData();
	pNew->_SetFindFileData( data );

	pNew->m_dir = dir;

	AddChild( pNew );
	return pNew;
}

void	CFileData::SetIgnore( bool bIgnore, bool bRefresh )
{
	m_isIgnore = bIgnore;

	if( bRefresh )
		ResetImageStage();
}

void	CFileData::_SetItemImage()
{
// 	if( NULL == m_pTreeCtrl )
// 	{
// 		return;
// 	}
// 
// 	int nStateDisable = 0;
// 	int nImage = m_isDir ? 0 : 2;
// 
// 	
// 
// 	if( _IsFolderNameIgnore() || _IsExtensionIgnore() || _IsFileNameIgnore() )
// 	{
// 		nImage = 4;
// 		nStateDisable = TVIS_EX_DISABLED;
// 	}
// 
// 	else if( m_isIgnore )
// 	{
// 		nImage = 3;
// 		nStateDisable = TVIS_EX_DISABLED;
// 	}
// 
// 	else if( IsParentIgnore() || _IsParentFolderNameIgnore() )
// 	{		
// 		nStateDisable = TVIS_EX_DISABLED;
// 	}
// 
// 	else
// 	{
// 	}
// 
// 	m_pTreeCtrl->SetItemStateEx( m_hItem, nStateDisable );
// 	m_pTreeCtrl->SetItemImage( m_hItem, nImage, nImage );
}

bool	CFileData::IsParentIgnore() const
{
	if( m_pParent )
	{
		if( m_pParent->m_isIgnore )
		{
			return true;
		}
		else
		{
			return m_pParent->IsParentIgnore();
		}
	}

	return false;
}

CFileData*	CFileData::GetParentIgnore()
{
	if( m_pParent )
	{
		if( m_pParent->m_isIgnore )
		{
			return m_pParent;
		}
		else
		{
			return m_pParent->GetParentIgnore();
		}
	}

	return NULL;
}

void	CFileData::ResetImageStage( bool bRecursive )
{
	_SetItemImage();

	if( false == bRecursive )
	{
		return;
	}

	for each( CFileData* ptr in m_setchilds )
	{
		ptr->ResetImageStage();
	}
}

CFileData*	CFileData::GetFileData( const std::string& path )
{
	if( path.empty() )
	{
		return NULL;
	}

	std::string realPath = path;

	char wch = (*realPath.rbegin());

	if( wch == _T('\\') || wch == _T('/') )
	{
		realPath.erase( realPath.begin() + (realPath.length() -1) );
	}

	return _GetFileData( realPath, 0 );
}

CFileData*	CFileData::_GetFileData( const std::string& path, int prevPos )
{
	std::string name = path;
	UINT32 _pos = name.find( _T("\\"), prevPos );

	if( std::string::npos != _pos )
	{
		name = name.substr( prevPos, (_pos - prevPos) );

		map_childs::iterator itr = m_childs.find( name );

		if( itr != m_childs.end() )
		{
			return (itr->second)->_GetFileData( path, _pos +1 );
		}
	}
	else
	{
		name = name.substr( prevPos, (name.length() - prevPos) );

		map_childs::iterator itr = m_childs.find( name );

		if( itr != m_childs.end() )
		{
			return (itr->second);
		}
	}

	return NULL;
}

void	CFileData::GetFileCopyCount( int& nCopyCount ) const
{
	nCopyCount = 0;

	for each( CFileData* ptr in m_setchilds )
	{
		ptr->_GetFileCopyCount( nCopyCount );
	}
}


void	CFileData::_GetFileCopyCount( int& nCopyCount ) const
{
	if( m_isIgnore )
	{
		return;
	}

	if( _IsFolderNameIgnore() )
	{
		return;
	}

	for each( CFileData* ptr in m_setchilds )
	{
		ptr->_GetFileCopyCount( nCopyCount );
	}

	/*if( false == m_isDir )
	{
		if( theApp.m_setIgnoreExt.find( m_ext ) != theApp.m_setIgnoreExt.end() )
		{
			return;
		}

		++nCopyCount;
	}*/

	if( _IsFileNameIgnore() || _IsExtensionIgnore() )
	{
		return;
	}

	++nCopyCount;
}


bool	CFileData::GetFileCopyList( vec_copy& vec, const std::string& rootDir, const std::string& pasteDir )
{
	int nMaxCopyCount = 0;
	GetFileCopyCount( nMaxCopyCount );

	vec.clear();
	vec.reserve( nMaxCopyCount );

	for each( CFileData* ptr in m_setchilds )
	{
		ptr->_GetFileCopyList( vec, rootDir, pasteDir );
	}
	return true;
}

void	CFileData::_GetFileCopyList( vec_copy& vec, const std::string& rootDir, const std::string& pasteDir )
{
	if( m_isIgnore )
	{
		return;
	}

	if( _IsFolderNameIgnore() )
	{
		return;
	}


	/*if( false == m_isDir )
	{
		if( theApp.m_setIgnoreExt.end() != theApp.m_setIgnoreExt.find( m_ext ) )
		{
			return;
		}
	}*/

	if( false == _IsFileNameIgnore() &&
		false == _IsExtensionIgnore() )
	{
		s_copy cp;

		cp.copyType		= m_isDir ? s_copy::COPY_DIR : s_copy::COPY_FILE;
		cp.copyFile		= rootDir;
		cp.pasteFile	= pasteDir;

		_MakeDir( cp.copyFile );
		_MakeDir( cp.pasteFile );

		cp.copyFile += GetMyDirectory();
		cp.pasteFile += GetMyDirectory();

		vec.push_back( cp );
	}

	for each( CFileData* ptr in m_setchilds )
	{
		ptr->_GetFileCopyList( vec, rootDir, pasteDir );
	}



	/*if( false == m_isDir )
	{
		if( theApp.m_setIgnoreExt.end() != theApp.m_setIgnoreExt.find( m_ext ) )
		{
			return;
		}

		std::string copyPath = rootDir;
		std::string pastePath = pasteDir;

		_MakeDir( copyPath );		
		copyPath += GetMyDirectory();

		_MakeDir( pastePath );
		pastePath += GetMyDirectory();

		vec.push_back( s_copy( copyPath, pastePath ) );
	}
	else
	{
		for each( CFileData* ptr in m_setchilds )
		{
			ptr->_GetFileCopyList( vec, rootDir, pasteDir );
		}
	}*/
}

//void	CFileData::CreateDirectory( const std::string& pasteDir )
//{
//	if( m_isIgnore )
//	{
//		return;
//	}
//
//	if( true == m_isDir )
//	{
//		std::string dir = pasteDir;
//
//		_MakeDir( dir );
//		dir += GetMyDirectory();
//
//		::CreateDirectory( dir.c_str(), NULL );
//	}
//
//	for each( CFileData* ptr in m_setchilds )
//	{
//		ptr->CreateDirectory( pasteDir );
//	}
//}

void	CFileData::AddChild( CFileData* pNew )
{
	pNew->m_pParent = this;	

	m_childs.insert( std::make_pair( pNew->m_name, pNew ) );
	m_setchilds.insert( pNew );
}

bool	CFileData::_IsFileNameIgnore() const
{
// 	if( false == m_isDir )
// 	{
// 		// file name
// 		{
// 			std::set< std::string >& setIgnore = theApp.m_setIgnores[ s_ignore::FILE_NAME ];
// 			std::string name = m_name;
// 
// 			int _p = name.find( L'.' );
// 			
// 			if( std::string::npos != _p )
// 			{
// 				name = name.substr( 0, _p );
// 			}
// 
// 			std::transform( name.begin(), name.end(), name.begin(), std::tolower );
// 
// 			if( setIgnore.end() != setIgnore.find( name ) )
// 			{
// 				return true;
// 			}
// 		}		
// 	}

	return false;
}

bool	CFileData::_IsFolderNameIgnore() const
{
// 	if( true == m_isDir )
// 	{
// 		// floder name
// 
// 		std::set< std::string >& setIgnore = theApp.m_setIgnores[ s_ignore::FOLDER_NAME ];
// 		std::string name = m_name;
// 		std::transform( name.begin(), name.end(), name.begin(), std::tolower );
// 
// 		if( setIgnore.end() != setIgnore.find( name ) )
// 		{
// 			return true;
// 		}
// 	}

	return false;
}

bool	CFileData::_IsExtensionIgnore() const
{
// 	if( false == m_isDir )
// 	{
// 		// extnsion
// 		{
// 			std::set< std::string >& setIgnore = theApp.m_setIgnores[ s_ignore::EXTENSION ];
// 			std::string name = m_ext;
// 			std::transform( name.begin(), name.end(), name.begin(), std::tolower );
// 
// 			if( setIgnore.end() != setIgnore.find( name ) )
// 			{
// 				return true;
// 			}
// 		}	
// 	}

	return false;
}

bool	CFileData::_IsParentFolderNameIgnore() const
{
	if( m_pParent )
	{
		if( m_pParent->_IsFolderNameIgnore() )
		{
			return true;
		}
		else
		{
			return m_pParent->_IsParentFolderNameIgnore();
		}
	}

	return false;
}
