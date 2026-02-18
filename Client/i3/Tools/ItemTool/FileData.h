#pragma once

#include <deque>
#include <set>
class CFileData;

struct s_sort
{
	bool operator () ( const CFileData* lhs, const CFileData* rhs ) const;
};

struct s_copy
{
	enum TYPE
	{
		COPY_FILE,
		COPY_DIR,
	};

	TYPE	copyType;

	std::string	copyFile;
	std::string	pasteFile;
};

struct s_ignore
{
	int				nType;
	std::string	name;

public:
	enum TYPE
	{
		EXTENSION,
		FOLDER_NAME,
		FILE_NAME,

		MAX_IGNORE_TYPE,
	};

	s_ignore( int _t = EXTENSION, const std::string& sz = "" )
		: nType( _t), name ( sz )
	{
	}

	bool operator < ( const s_ignore& rhs )
	{
		if( nType < rhs.nType ) return true;
		if( nType > rhs.nType ) return false;

		return name < rhs.name;
	}

	static const std::string TYPE_STR[ MAX_IGNORE_TYPE ];
	static const TCHAR*	GetTypeName( int nType )
	{
		return s_ignore::TYPE_STR[ nType ].c_str();
	}
};

typedef std::vector< s_copy >	vec_copy;
typedef std::vector<std::string> vString;


enum eExtensionType
{
	E_CHARAEDITOR,
	MAX_TYPE
};

struct stFileInfo
{
	std::string strRootName;
	std::string strFileName;
	std::string strFolderName;
};


typedef std::vector<std::string> vFileName;
typedef std::map<std::string, stFileInfo> mFileNameType;

class CFileData
{
public:
	CFileData();
	~CFileData();

	std::string	GetMyDirectory() const
	{
		return m_isDir ? m_dir + m_name + _T("\\") : m_dir + m_name;
	}
	
	CFileData*		GetFileData( const std::string& path );
	CFileData*		GetParentIgnore();

	void	GetFileCopyCount( int& nCopyCount ) const;
	
	void	ResetData();
	void	ResetImageStage( bool bRecursive = true );
	void	SetIgnore( bool bIgnore, bool bRefresh = true );
	void	SetTreeItem( CTreeCtrl* pCtrl, HTREEITEM hParent = NULL );	

public:
	bool	IsParentIgnore() const;

	bool	GetFileCopyList( vec_copy& vec, const std::string& rootDir, const std::string& pasteDir );
	bool	FindFile( mFileNameType& mFileNameCon, const std::string& rootDir, CString& strResName, const std::string& dir = _T(""),CDialog* pDlg = NULL);

public:
	//void	CreateDirectory( const std::string& pasteDir );
	
	void	AddChild( CFileData* pNew );
	void	ClearChilds();

private:
	CFileData*		_GetFileData( const std::string& path, int prevPos );
	CFileData*		_AddChild( const WIN32_FIND_DATA& data, const std::string& dir );
	
	bool	_IsFolderNameIgnore() const;
	bool	_IsExtensionIgnore() const;	
	bool	_IsFileNameIgnore() const;
	bool	_IsParentFolderNameIgnore() const;

	void	_SetFindFileData( const WIN32_FIND_DATA& data );
	void	_GetFileCopyCount( int& nCopyCount ) const;
	void	_GetFileCopyList( vec_copy& vec, const std::string& rootDir, const std::string& pasteDir );
	void	_SetItemImage();

public:
	CTreeCtrl*		m_pTreeCtrl;
	HTREEITEM		m_hItem;

public:
	typedef std::map< std::string, CFileData* >	map_childs;
	typedef std::set< CFileData*, s_sort >			set_childs;
	
	map_childs		m_childs;
	set_childs		m_setchilds;

public:
	CFileData*	m_pParent;

	std::string		m_name;
	std::string		m_dir;
	std::string		m_ext;
	std::string		m_filename;
	
	bool	m_isDir;
	bool	m_isIgnore;


	vString	m_vExtensionType;

public:
	static void		Init();
	static void		Release();

	
private:
	static	std::deque< CFileData* >	m_deads;
};