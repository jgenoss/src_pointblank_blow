#ifndef _PTSTORAGE_H_
#define _PTSTORAGE_H_

class PTFile;

enum LOAD_TYPE
{
	LOAD_WORKDIR_QA,	// QA 폴더(원본 로드)
	LOAD_WORKDIR_LIVE,	// LIVE 폴더(Pack 폴더의 팩 파일들만 로드, 언팩하여 로드)
};

class PTStorage
{
public:
	PTStorage(CTreeCtrl* treeCtrl, const char* workdir, LOAD_TYPE loadtype);
	virtual ~PTStorage();

	PTFile*	Add(const char* fullpath, PTFile* parent);
	HTREEITEM Add(const char* fullpath, HTREEITEM parent);

	PTFile*	Find(const char* fullpath);
	void	Delete(const char* fullpath);

	void	insert(const char* fullpath, PTFile* file);
	void	erase(const char* fullpath);

	PTFile*			TopLevelFile()			{ return m_topLevelFile; }
	const PTFile*	TopLevelFile() const	{ return m_topLevelFile; }

private:
	void LoadRecur(PTFile* f);
	void LoadUnpackRecur(i3PackNode* packnode, const char* parentpath);
	void LoadFolders(PTFile* f, i3::string& fullpath);
	void LoadFiles(PTFile* f, i3::string& fullpath);

private:
	CTreeCtrl*  m_treeCtrl;
	PTFile*		m_topLevelFile;
	LOAD_TYPE	m_loadType;

	typedef i3::unordered_map<i3::string, PTFile*> FileMap;
	FileMap		m_fileMap;

	PTStorage(const PTStorage& src);
	PTStorage& operator=(const PTStorage& src);
};

#endif