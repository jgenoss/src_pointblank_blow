#ifndef _PTFILE_H_
#define _PTFILE_H_

class PTStorage;

class PTFile
{
public:
	PTFile(const char* fullpath, PTStorage* owner, CTreeCtrl* treeCtrl, HTREEITEM parentItem);
	~PTFile();

	const char* GetFullpath() const	{ return m_path->fullpath.c_str();	}
	const char* GetFileName() const	{ return m_path->name.c_str();		}
	const char* GetUnpackPath() const { return m_path->fullpathUnpacked.c_str(); }

	bool		IsFolder() const		{ return m_path->isFolder;	 }
	bool		IsPackFile() const		{ return m_path->isPackFile; }
	bool		IsUnpackFile() const	{ return m_path->isUnpackFile; }

	CTreeCtrl*  GetTreeCtrl() const	{ return m_treeCtrl; }
	HTREEITEM	GetTreeItem() const	{ return m_treeItem; }

	const char*	GetTopLevelFolderPath() const;

	bool operator==(const PTFile& f);

private:
	struct PATH_INFO
	{
		i3::string fullpath;				// 풀 패스
		i3::string name;					// 폴더명 또는 파일명
		i3::string fullpathUnpacked;	

		bool isFolder;
		bool isPackFile;
		bool isUnpackFile;

		PATH_INFO(const char* fullpath);
	};

	PATH_INFO*	m_path;
	PTStorage*	m_owner;
	CTreeCtrl*	m_treeCtrl;
	HTREEITEM	m_treeItem;

	PTFile(const PTFile& src);
	PTFile& operator=(const PTFile& src);
};

#endif