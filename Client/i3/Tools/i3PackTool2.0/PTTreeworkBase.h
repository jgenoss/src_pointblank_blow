#ifndef _PTTREEWORK_BASE_H_
#define _PTTREEWORK_BASE_H_

#include "PTCollegue.h"

class CDragDropTreeCtrl;
class PTFile;
class PTStorage;

class PTTreeworkBase : public PTCollegue
{
public:
	PTTreeworkBase();
	virtual ~PTTreeworkBase() {}

	virtual void Create(const char* workdir, CDragDropTreeCtrl* treeCtrl) = 0;
	virtual void Destroy() = 0;

	virtual void SetMediator(PTMediator* m) { m_mediator = m; }

	const PTStorage*	GetStorage() const	{ return m_storage; }
	PTStorage*			GetStorage()		{ return m_storage; }

	const CDragDropTreeCtrl*	GetTreeCtrl() const { return m_treeCtrl; }
	CDragDropTreeCtrl*			GetTreeCtrl()		{ return m_treeCtrl; }

	const char*			GetWorkDir() const	{ return m_workdir.c_str(); }

protected:
	i3::string			m_workdir;
	CDragDropTreeCtrl*	m_treeCtrl;
	PTStorage*			m_storage;
	PTMediator*			m_mediator;
};

namespace TreeUtil
{
	PTFile* FindPackFolder(PTTreeworkBase* twbase);
	void	CheckItemAll(CDragDropTreeCtrl* tree, HTREEITEM item, bool ok);
	PTFile* FindSameFile(const char* file, PTTreeworkBase* from, PTTreeworkBase* to);
	PTFile* FindBestFitFolder(PTFile* pack);
	i3::string PathExceptWorkdir(const char* fullpath, const char* workdir);
	const char* ReplaceWorkdir(PTFile* fileSrc);
	void	SortTreeItem(CDragDropTreeCtrl* treeCtrl, HTREEITEM item);
}
#endif