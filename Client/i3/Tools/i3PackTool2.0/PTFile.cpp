#include "stdafx.h"
#include "PTFile.h"
#include "PTStorage.h"
#include "PTTreeworkBase.h"
#include "../../include/i3Base/string/ext/contain_string.h"

PTFile::PATH_INFO::PATH_INFO(const char* fullpath_) 
	: fullpath(fullpath_), isFolder(false), isPackFile(false), isUnpackFile(false)
{
	size_t pos = fullpath.find_last_of("\\");
	name = fullpath.substr(pos+1);

	if (i3::contain_string(name, ".") == -1)
		isFolder = true;

	if ((i3::contain_string(name, ".i3Pack") != -1) &&
		(i3::contain_string(name, ".i3PackScript") == -1))
	{
		isPackFile = true;

		fullpathUnpacked = fullpath;
		size_t end = fullpathUnpacked.find("\\Pack");
		fullpathUnpacked = fullpathUnpacked.substr(0, end);
	}
	else
	{
		if (i3::contain_string(fullpath.c_str(), "\\Pack") != -1 &&
			i3::contain_string(fullpath.c_str(), ".i3Pack") != -1)
		{
			isUnpackFile = true;

			fullpathUnpacked = fullpath;
			size_t start = fullpathUnpacked.find("\\Pack");
			size_t end = fullpathUnpacked.find(".i3Pack"); end += 7;
			size_t length = end - start;

			fullpathUnpacked.erase(start, length);
		}
	}
}

PTFile::PTFile(const char* fullpath, PTStorage* owner, CTreeCtrl* treeCtrl, HTREEITEM parentItem) 
: m_path(new PATH_INFO(fullpath)), m_owner(owner), m_treeCtrl(treeCtrl), m_treeItem(0)
{
	HTREEITEM item = treeCtrl->InsertItem(m_path->name.c_str(), 0, 0, parentItem);
	treeCtrl->SetItemData(item, (DWORD_PTR)this);
	m_treeItem = item;

	owner->insert(fullpath, this);
}

PTFile::~PTFile()
{
	m_owner->erase(m_path->fullpath.c_str());

	delete m_path; m_path = 0;
}

const char* PTFile::GetTopLevelFolderPath() const
{
	 return m_owner->TopLevelFile()->GetFullpath();
}

bool PTFile::operator==(const PTFile& f)
{
	return true;
}