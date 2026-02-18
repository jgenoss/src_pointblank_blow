#include "stdafx.h"
#include "PackScriptTree.h"

#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/itl/range/algorithm/find_if.h"

#include "i3Base/string/ext/generic_find_last_of.h"


void CollectPackScriptFiles(const PackScriptFolder* folder, i3::vector<PackScriptFile*>& inout)
{
	const i3::vector<PackScriptFile*>& file_list = folder->GetFileList();		
	const size_t num_file = file_list.size();

	for (size_t i = 0 ; i < num_file ; ++i)
		inout.push_back( file_list[i]);

	const i3::vector<PackScriptFolder*>& child_list = folder->GetChildList();

	const size_t num_child = child_list.size();

	for (size_t i = 0 ; i < num_child ; ++i)
		CollectPackScriptFiles(child_list[i], inout);
}


PackScriptFolder::~PackScriptFolder()
{
	i3::cu::for_each_delete(m_FileList);
	i3::cu::for_each_delete(m_ChildList);
}

PackScriptFolder::PackScriptFolder(const PackScriptFolder& rhs) : m_Name(rhs.m_Name), 
	m_Parent(NULL), m_ChildList(rhs.m_ChildList.size()), m_FileList(rhs.m_FileList.size())
{
	const size_t num_child = m_ChildList.size();
	for (size_t i = 0 ; i < num_child ; ++i)
	{
		PackScriptFolder* rhs_child_folder = rhs.m_ChildList[i];
		m_ChildList[i] = new PackScriptFolder(*rhs_child_folder);
		m_ChildList[i]->m_Parent = this;
	}
	
	const size_t num_File = m_FileList.size();
	for (size_t i = 0 ; i < num_child ; ++i)
	{
		PackScriptFile* rhs_child_file = rhs.m_FileList[i];
		m_FileList[i] = new PackScriptFile(*rhs_child_file);
		m_FileList[i]->m_Parent = this;
	}
	
}


void PackScriptFolder::AddChild( PackScriptFolder* child) 
{ 
	m_ChildList.push_back(child);  child->m_Parent = this; 
}	

bool PackScriptFolder::DeleteChild( PackScriptFolder* child)
{
	if ( i3::vu::remove(m_ChildList, child) )
	{
		delete child;
		return true;
	}
	return false;
}

PackScriptFolder* PackScriptFolder::FindChild(const i3::wstring& str)
{
	struct fn 
	{ 
		const i3::wstring& str; fn(const i3::wstring& str) : str(str) {}   
		bool operator()(const PackScriptFolder* f) const { return f->GetFolderName() == str ; }
	};
	i3::vector<PackScriptFolder*>::iterator it = i3::range::find_if(m_ChildList, fn(str) );	
	return (it != m_ChildList.end() ) ? *it : NULL;
}

PackScriptFile* PackScriptFolder::FindChildFile(const i3::wstring& str)
{
	struct fn 
	{ 
		const i3::wstring& str; fn(const i3::wstring& str) : str(str) {}   
		bool operator()(const PackScriptFile* f) const { return f->GetFileTitle() == str ; }
	};

	i3::vector<PackScriptFile*>::iterator it = i3::range::find_if(m_FileList, fn(str) );	
	return (it != m_FileList.end() ) ? *it : NULL;
}

void PackScriptFolder::AddFile(PackScriptFile* file)
{
	m_FileList.push_back(file);	file->m_Parent = this;
}

bool PackScriptFolder::DeleteFile(PackScriptFile* file)
{
	if ( i3::vu::remove(m_FileList, file) )
	{
		delete file;
		return true;
	}
	return false;
}

PackScriptFolder*	PackScriptFolder::FindFolder(const i3::const_wchar_range& strRelPath)
{
	i3::const_wchar_range::iterator it = i3::generic_find_last_of(strRelPath, L"./\\");
	
	if ( it != strRelPath.end() )
	{
		if (*it == L'.')
		{
			i3::const_wchar_range::iterator it2 = i3::generic_find_last_of( strRelPath.begin(), it, L"/\\" );	
			it = (it2 == it) ? strRelPath.begin() : it2;
		}
		else
		{
			it = strRelPath.end();
		}
	}

	i3::const_wchar_range strRelDir(strRelPath.begin(), it);
	
	if (strRelDir.empty() )
		return this;				// 폴더 경로가 아예 없으면 그냥 여기서 this리턴..

	return PackScriptFolder::FindFolderRercursive( strRelDir );
}

PackScriptFolder*	PackScriptFolder::FindFolderRercursive(i3::const_wchar_range& strRelPath) 
{
	//
	if ( this->GetParent() )
	{
		if ( i3::icontain_string_head( strRelPath, m_Name) == false )
			return NULL;
	
		strRelPath.advance_begin(m_Name.size());
	
		if (strRelPath.empty() )
			return this;

		if (strRelPath[0] != L'/' && strRelPath[0] != L'\\')		// 이 경우에도 아니다..
			return NULL;

		strRelPath.advance_begin(1);		// '/' or '\\' skip...

		if (strRelPath.empty() )
			return this;
	}
	
	const size_t numChild = m_ChildList.size();
	for (size_t i = 0 ; i < numChild ; ++i )
	{
		PackScriptFolder* folder = m_ChildList[i]->FindFolderRercursive(strRelPath);		
		if ( folder )
			return folder;
	}
	
	return NULL;
}

PackScriptFile*		PackScriptFolder::FindFile(const i3::const_wchar_range& strRelPath_Arg)
{
	
	i3::const_wchar_range strRelPath(strRelPath_Arg);

	i3::const_wchar_range::iterator pos = i3::generic_find_last_of(strRelPath, L"\\/.");

	bool			  bFoundParent = false;

	if ( pos != strRelPath.end() )
	{
		if (  *pos == L'.')
		{
			strRelPath = i3::const_wchar_range(strRelPath.begin(), pos);
			pos = i3::generic_find_last_of(strRelPath, L"\\/");
			
			if (pos != strRelPath.end() )
				bFoundParent = true;
		}
		else
		{
			bFoundParent = true;
		}
	}

	PackScriptFolder* target_folder;

	if (bFoundParent == false)
	{
		target_folder = this;		
		pos = strRelPath.begin();
	}
	else
	{
		i3::const_wchar_range folder_path(strRelPath.begin(), pos );
		target_folder = PackScriptFolder::FindFolderRercursive(folder_path);

		if (target_folder == NULL) // 찾기 실패....그냥 널 값 리턴하자..
			return NULL;
		++pos;
	}

	i3::const_wchar_range file_title(pos, strRelPath.end());
		
	struct internal_fn 
	{
		const i3::const_wchar_range& m_rng;
		internal_fn(const i3::const_wchar_range& rng) : m_rng(rng) {}
		bool operator()( PackScriptFile* f ) const
		{
			return ( i3::generic_is_iequal(f->GetFileTitle(), m_rng ) );
		}
	};

	i3::vector<PackScriptFile*>::iterator it = i3::range::find_if(target_folder->m_FileList, internal_fn(file_title));
	
	return  ( it != target_folder->m_FileList.end() ) ? *it : NULL;
}



// 선행 재귀형식이어야한다..(parent쪽으로 상위 재귀할때, 가장 마지막 루트에서 가장 먼저 연산이 이루어지게)

namespace
{
	void		MakeRelPathImpl(const PackScriptFolder* folder, i3::wstring& outRelPath)
	{
		if ( folder->GetParent() == NULL )		
			return;
		MakeRelPathImpl(folder->GetParent(), outRelPath);
		outRelPath += folder->GetFolderName();
		outRelPath += L'/';
	}
}

void		PackScriptFolder::MakeRelPath(i3::wstring& outRelPath) const
{
	if ( this->GetParent() == NULL) 
		return;
	MakeRelPathImpl(this->GetParent(), outRelPath);		// 상위 재귀..
	outRelPath += this->GetFolderName();
}


void		PackScriptFolder::MakeAbsPath(const i3::const_wchar_range& absPath, i3::wstring& outAbsPath) const
{
	outAbsPath.assign(absPath.begin(), absPath.end());

	if ( this->GetParent() )
		outAbsPath += L'/';

	PackScriptFolder::MakeRelPath(outAbsPath);
}

void		PackScriptFile::MakeRelPath(i3::wstring& outRelPath) const
{
	m_Parent->MakeRelPath(outRelPath);
	outRelPath += L'/';
	outRelPath += this->GetFileTitle();
}

void		PackScriptFile::MakeAbsPath(const i3::const_wchar_range& absPath, i3::wstring& outAbsPath) const
{
	m_Parent->MakeAbsPath(absPath, outAbsPath);
	outAbsPath += L'/';
	outAbsPath += this->GetFileTitle();
}
