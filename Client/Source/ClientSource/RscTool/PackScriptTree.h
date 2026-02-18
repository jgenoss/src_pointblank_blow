#pragma once

//
// 이름 제외한 나머지 데이터는 여기서 처리하지 말것.. ParentPath는 상대경로이고, 자기 자신의 이름은 포함하지 않는다.
//
 

#include "i3Base/itl/range/iterator_range.h"
#include "i3Base/string/rc_string.h"

class PackScriptFile;
class PackScriptFolder;

void CollectPackScriptFiles(const PackScriptFolder* folder, i3::vector<PackScriptFile*>& inout);

PackScriptFolder*		ClonePackScriptFolder(const PackScriptFolder* folder);

class PackScriptFolder
{
public:
	PackScriptFolder(const wchar_t* name) : m_Name(name), m_Parent(NULL) {}
	PackScriptFolder(const i3::const_wchar_range& name) : m_Name(name.begin(), name.end()), m_Parent(NULL) {}
	PackScriptFolder(const PackScriptFolder& rhs);

	~PackScriptFolder();

	PackScriptFolder*					FindFolder(const i3::const_wchar_range& strRelPath);
	PackScriptFile*						FindFile(const i3::const_wchar_range& strRelPath);

	void								MakeRelPath(i3::wstring& outRelPath) const;
	void								MakeAbsPath(const i3::const_wchar_range& absPath, i3::wstring& outAbsPath) const;


	PackScriptFolder*					GetParent() const { return m_Parent; }
	const i3::wstring&					GetFolderName() const		{ return m_Name; }

	void								SetFolderName(const i3::wstring& name) { m_Name = name; }
	bool								HasChildFolder() const { return !m_ChildList.empty();  }

	void AddChild( PackScriptFolder* child);
	bool DeleteChild( PackScriptFolder* child);
	PackScriptFolder* FindChild(const i3::wstring& str);

	void AddFile(PackScriptFile* file);
	bool DeleteFile(PackScriptFile* file);
	PackScriptFile* FindChildFile(const i3::wstring& str);

	const i3::vector<PackScriptFolder*>&	GetChildList() const { return m_ChildList; }
	const i3::vector<PackScriptFile*>&		GetFileList() const { return m_FileList; }

private:
	PackScriptFolder*					FindFolderRercursive(i3::const_wchar_range& strRelPath);

	i3::wstring							m_Name;			// 경로 이름이 아닌 끝이름만...
	i3::vector<PackScriptFolder*>	m_ChildList;		// 
	i3::vector<PackScriptFile*>		m_FileList;			//

	PackScriptFolder*	m_Parent;			// 상식적으로 모든 부모는 File이 될수없다.
	
};

class PackScriptFile
{
public:

	PackScriptFile(const i3::const_wchar_range& fileTitle) : 
								m_FileTitle(fileTitle.begin(), fileTitle.end()), m_Parent(NULL) {}
	PackScriptFile(const PackScriptFile& rhs) : m_FileTitle(rhs.m_FileTitle), m_Parent(NULL) {}

	void							MakeRelPath(i3::wstring& outRelPath) const;
	void							MakeAbsPath(const i3::const_wchar_range& absPath, i3::wstring& outAbsPath) const;
	
	const i3::wstring&				GetFileTitle() const { return m_FileTitle; }
	void							SetFileTitle(const i3::wstring& title) { m_FileTitle = title; }

	PackScriptFolder*				GetParent() const { return m_Parent; }
	
private:
	i3::wstring						m_FileTitle;		
	PackScriptFolder*				m_Parent;
	friend class PackScriptFolder;
};

inline
PackScriptFolder*		ClonePackScriptFolder(const PackScriptFolder* folder) 
{
	return new PackScriptFolder(*folder);
}