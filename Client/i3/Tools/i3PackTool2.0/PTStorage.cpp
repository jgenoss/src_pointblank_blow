#include "stdafx.h"
#include "PTStorage.h"
#include "PTFile.h"
#include "../../include/i3Base/string/ext/contain_string.h"

PTStorage::PTStorage(CTreeCtrl* treeCtrl, const char* workdir, LOAD_TYPE loadtype) 
: m_treeCtrl(treeCtrl), m_topLevelFile(0), m_loadType(loadtype)
{
	m_topLevelFile = Add(workdir, (PTFile*)NULL);

	LoadRecur(m_topLevelFile);
}

PTStorage::~PTStorage()
{
	FileMap::iterator it = m_fileMap.begin();
	for (; it != m_fileMap.end(); it++)
	{
		i3::string str = it->first;
		delete it->second;
	}

	m_fileMap.clear();
}

void PTStorage::LoadRecur(PTFile* f)
{
	HANDLE fileHandle;
	WIN32_FIND_DATA fileInfo;

	i3::string targetPath(f->GetFullpath());
	targetPath += "\\*";
	fileHandle = ::FindFirstFile(targetPath.c_str(), &fileInfo);

	if (fileHandle == INVALID_HANDLE_VALUE) 
	{
		::FindClose(fileHandle);
		return;
	}

	do
	{
		// 모든 폴더마다 존재하는 경로인 "."과 ".."은 건너 뜀.
		if (::strcmp(fileInfo.cFileName, ".") == 0) continue;
		if (::strcmp(fileInfo.cFileName, "..") == 0) continue;
		if (::strcmp(fileInfo.cFileName, ".svn") == 0) continue;

		i3::string fullpath;
		fullpath += f->GetFullpath();
		fullpath += "\\";
		fullpath += fileInfo.cFileName;

		if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 폴더만
			LoadFolders(f, fullpath);
		else
			LoadFiles(f, fullpath);

	}while(::FindNextFile(fileHandle, &fileInfo));

	::FindClose(fileHandle);
}

void PTStorage::LoadUnpackRecur(i3PackNode* packnode, const char* parentpath)
{
	PTFile* fparent = Find(parentpath);
	HTREEITEM parentItem = fparent->GetTreeItem();

	if (packnode->getChildCount() != 0)
	{
		// 현재 폴더의 파일 추가.
		for (INT32 i=0; i<packnode->getFileCount(); i++)
		{
			i3::string fullpath(parentpath);
			fullpath += "\\";
			fullpath += packnode->getFile(i)->m_szName;

			Add(fullpath.c_str(), parentItem);
		}

		// 서브 폴더 추가
		for (INT32 i=0; i<packnode->getChildCount(); i++)
		{
			i3::string path(parentpath);
			path += "\\";
			path += packnode->getChild(i)->GetName();

			Add(path.c_str(), parentItem);
			LoadUnpackRecur((i3PackNode*)packnode->getChild(i), path.c_str());
		}
	}
	else
	{
		// 현재 폴더에 파일만 있음. 파일 추가.
		for (INT32 i=0; i<packnode->getFileCount(); i++)
		{
			i3::string fullpath(parentpath);
			fullpath += "\\";
			fullpath += packnode->getFile(i)->m_szName;

			Add(fullpath.c_str(), parentItem);
		}
	}
}

void PTStorage::LoadFolders(PTFile* fparent, i3::string& fullpath)
{
	LoadRecur(Add(fullpath.c_str(), fparent));
}

void PTStorage::LoadFiles(PTFile* fparent, i3::string& fullpath)
{
	// 팩스크립트 파일은 읽지 않는다. 이 툴 제작 목적이 팩스크립트 사용하지 않기 위함이므로.
	if (fullpath.find(".i3PackScript") != i3::string::npos) return;
	if (fullpath.find(".bat") != i3::string::npos) return;

	if (m_loadType == LOAD_WORKDIR_QA)
	{
		if (i3::contain_string(fullpath.c_str(), ".i3Pack") == -1)
		{
			Add(fullpath.c_str(), fparent);
		}
	}
	else if (m_loadType == LOAD_WORKDIR_LIVE)
	{
		if (i3::contain_string(fullpath.c_str(), ".i3Pack") != -1)
		{
			i3PackVolume packfile;

			if (packfile.LoadFromFile(fullpath.c_str()))
			{
				i3PackNode* packnode = (i3PackNode*)packfile.getRoot();
				if (!i3::same_of<i3PackNode*>(packnode)) return;

				Add(fullpath.c_str(), fparent);
				LoadUnpackRecur(packnode, fullpath.c_str());
			}
		}
		else
		{
			Add(fullpath.c_str(), fparent);
		}
	}
}

PTFile* PTStorage::Add(const char* fullpath, PTFile* fparent)
{
	HTREEITEM parentItem = (fparent) ? fparent->GetTreeItem() : 0;

	return new PTFile(fullpath, this, m_treeCtrl, parentItem); 
}

HTREEITEM PTStorage::Add(const char* fullpath, HTREEITEM parent)
{
	PTFile* f = new PTFile(fullpath, this, m_treeCtrl, parent);
	return f ? f->GetTreeItem() : 0;
}

PTFile* PTStorage::Find(const char* fullpath)
{
	FileMap::const_iterator it = m_fileMap.find(fullpath);
	return (it != m_fileMap.end()) ? it->second : 0;
}

void PTStorage::Delete(const char* fullpath)
{
	PTFile* file = Find(fullpath);
	if (file)
	{
		delete file; file = 0;
		erase(fullpath);
	}
}

void PTStorage::insert(const char* fullpath, PTFile* f)
{
	m_fileMap.insert(FileMap::value_type(fullpath, f));
}

void PTStorage::erase(const char* fullpath)
{
	m_fileMap.erase(fullpath);
}