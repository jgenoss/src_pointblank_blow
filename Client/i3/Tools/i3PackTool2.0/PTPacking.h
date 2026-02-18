#ifndef _PTPACKING_H_
#define _PTPACKING_H_

class PTTreeworkBase;
class PTFile;

class PTPacking
{
public:
	PTPacking();
	~PTPacking();

	void Load(PTFile* file);
	void Save();
	void Refresh();
	void Add(PTFile* file, bool includeSubdir);
	void Delete(PTFile* file);
	void Export(const char* exportPath);
	void CheckSum(PTFile* file);
	void Encript(PTFile* file);

private:
	void AddRecur(CTreeCtrl* tree, HTREEITEM item);
	i3PackNode* PreparePath(const char* pszPath, bool bIsFile);
	void AddDirectory(const char* pszPath, i3PackNode* pNode, BOOL IncludeSubDir);


	i3PackNode* m_packnode;
	PTFile*		m_packfileSelected;

	PTPacking(const PTPacking& src);
	PTPacking& operator=(const PTPacking& src);

	// RAII·Î »ç¿ë
	static void* operator new(size_t size);
	static void operator delete(void* ptr);
};

namespace PackWork
{
	void Repack(PTFile* file);
	void MakePack();
	void Export(PTFile* file, const char* exportPath);
}

#endif