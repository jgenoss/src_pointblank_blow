#include "stdafx.h"
#include "PTPacking.h"
#include "PTTreeworkLive.h"
#include "PTTreeworkQA.h"
#include "PTFile.h"
#include "DragDropTreeCtrl.h"
#include "PTTreeworkMediator.h"
#include "NewPackDlg.h"

#include "../../include/i3Base/string/ext/contain_string.h"
#include "../../include/i3Base/string/ext/make_relative_path.h"
#include "../../include/i3Base/string/ext/make_relative_path.h"

namespace PackWork
{
	void Repack(PTFile* file)
	{
		PTPacking p;
		p.Load(file);
		p.Refresh();
		p.Save();
	}

	void MakePack()
	{
		CNewPackDlg dlg;
		dlg.DoModal();
	}

	void Export(PTFile* file, const char* exportPath)
	{
		PTPacking p;
		p.Load(file);
		p.Export(exportPath);
	}
}

PTPacking::PTPacking() 
: m_packnode(0), m_packfileSelected(0)
{
}

PTPacking::~PTPacking() 
{
	I3_SAFE_RELEASE(m_packnode);
}

void PTPacking::Load(PTFile* file)
{
	if (file)
	{
		i3PackVolume pv;
		if (pv.LoadFromFile(file->GetFullpath()))
		{
			i3PackNode* existingPack = (i3PackNode*)pv.getRoot();
			if (i3::same_of<i3PackNode*>(existingPack))
			{
				I3_REF_CHANGE(m_packnode, existingPack);
			}
		}
		else
		{
			i3PackNode* newpack = i3PackNode::new_object();
			newpack->SetName( "/");
			I3_REF_CHANGE(m_packnode, newpack);
		}

		m_packfileSelected = file;
	}
	else
	{
		i3PackNode* newpack = i3PackNode::new_object();
		I3_REF_CHANGE(m_packnode, newpack);
	}	
}

void DeleteAll(i3PackNode* pnode)
{
	for (INT32 i=0; i<pnode->getFileCount(); i++)
	{
		I3_PACK_FILE_REF* file = pnode->getFile(i);
		pnode->RemoveFile(file->m_szName);
	}

	for (INT32 i=0; i<pnode->getChildCount(); i++)
	{
		i3PackNode* child = (i3PackNode*)pnode->getChild(i);
		DeleteAll(child);
	}
}

void PTPacking::Refresh()
{
	if (m_packnode == NULL) return;
	if (m_packfileSelected == NULL) return;

	DeleteAll(m_packnode);

	AddRecur(m_packfileSelected->GetTreeCtrl(), m_packfileSelected->GetTreeItem());
}

void PTPacking::AddRecur(CTreeCtrl* tree, HTREEITEM item)
{
	if (tree->ItemHasChildren(item))
	{
		item = tree->GetNextItem(item, TVGN_CHILD);

		while (item)
		{
			PTFile* file = reinterpret_cast<PTFile*>(tree->GetItemData(item));
			const char* fullpath = file->IsUnpackFile() ? file->GetUnpackPath() : file->GetFullpath();
			PTFile* target = TreeUtil::FindSameFile(fullpath, 
				TheMediator().GetTreeworkLive(), TheMediator().GetTreeworkQA());

			if (target) 
				Add(target, !target->IsFolder());

			AddRecur(tree, item);
			item = tree->GetNextItem(item, TVGN_NEXT);
		}
	}
}

void PTPacking::Add(PTFile* file, bool includeSubdir)
{
	i3PackNode* packnode = 0;

	const char* fullpath = file->GetFullpath();

	if (i3System::IsDirectory(fullpath))
	{
		packnode = PreparePath(fullpath, false);
		AddDirectory(fullpath, packnode, includeSubdir);
		return;
	}

	CFileFind find;
	BOOL fileExist = find.FindFile(fullpath);
	if (fileExist == FALSE)
	{
		//PRINTF( "이름이 잘못되었거나 없는 파일(폴더)입니다.");
		return;
	}

	packnode = PreparePath(fullpath, true);
	I3ASSERT(packnode != NULL);

	//같은목록을 추가하려고 하면 기존의 리스트를 삭제하고 다시 추가합니다. 
	for(INT32 i=0; i<packnode->getFileCount(); i++)
	{
		const char* fileName = packnode->getFile(i)->m_szName;
		INT32 result = i3::contain_string(fullpath, fileName);
		if (result != -1)
			packnode->RemoveFile(packnode->getFile(i)->m_szName);
	}

	while (fileExist)
	{
		fileExist = find.FindNextFile();

		if (find.IsHidden() || find.IsSystem()) continue;
		if (find.IsDirectory()) continue;

		packnode->AddFile(find.GetFileName(), 0, static_cast<UINT32>(find.GetLength()), 0);
		Encript(file);
	}
}

i3PackNode* PTPacking::PreparePath(const char* fullpath, bool isFile)
{
	char szPath[MAX_PATH] = {0,};

	// 상대 Path로 만든다.
	i3::make_relative_path(TheMediator().GetTreeworkQA()->GetWorkDir(), fullpath, szPath);

	i3::vector<i3::rc_string> token;
	i3::resolve_path_token(szPath, token);

	// 만약 주어진 경로가 폴더가 아니라 파일에 대한 것이라면
	// 가장 마지막의 Token은 File명이기 때문에 고려하지 않는다.
	size_t cnt = token.size();
	if ((token.empty() == false) && isFile)
		cnt --;

	i3PackNode* pParent = m_packnode;
	i3PackNode* pNode = 0;

	for (size_t i=0; i<cnt; i++)
	{
		const char* name = token[i].c_str();

		pNode = static_cast<i3PackNode*>(pParent->FindChildByName(name));
		if (pNode == NULL)
		{
			// 없던 Path... 추가해 준다.
			pNode = i3PackNode::new_object();
			pNode->SetName(name);
			pParent->AddChild(pNode);
		}

		pParent = pNode;
	}

	return pParent;
}

void PTPacking::AddDirectory(const char* fullpath, i3PackNode* packnode, BOOL includeSubdir)
{
	if (includeSubdir == FALSE) return;

	CString	path;
	path.Format("%s\\*.*", fullpath);

	CFileFind find;
	BOOL fileExist = find.FindFile(path);
	while (fileExist)
	{
		fileExist = find.FindNextFile();

		if (find.IsHidden() || find.IsSystem()) continue;
		if (find.IsDots()) continue;
		if (find.IsDirectory() == FALSE) continue;

		CString	fileName = find.GetFileName();
		i3PackNode* newPacknode = 
			static_cast<i3PackNode*>(packnode->FindChildByName(fileName.GetString()));

		if (newPacknode == NULL)
		{
			newPacknode = i3PackNode::new_object();
			packnode->AddChild(newPacknode);
			newPacknode->SetName(fileName.GetString());
		}

		path.Format("%s/%s", fullpath, fileName.GetString());
		AddDirectory(path, newPacknode, includeSubdir);
	}
}

void PTPacking::Delete(PTFile* file)
{
	CFileFind find;

	BOOL fileExist = find.FindFile(file->GetFullpath());
	while (fileExist)
	{
		fileExist = find.FindNextFile();

		// 상대 Path로 만든다.
		char relativepath[MAX_PATH] = {0,};
		i3::make_relative_path(file->GetTopLevelFolderPath(), find.GetFilePath().GetString(), relativepath);

		I3_PACK_FILE_REF* fileRef = 0;
		i3PackNode* packnode = m_packnode->FindPath(relativepath, &fileRef);
		if (packnode == NULL)
		{
			//PRINTF( "%s 파일을 찾을 수 없어, 제거하지 못했음.\n", pszPath);
			return;
		}

		if (fileRef)
		{
			CString	fileName = find.GetFileName();
			packnode->RemoveFile(fileName.GetString());
		}
		else
		{
			i3PackNode* parent = static_cast<i3PackNode*>(packnode->getParent());
			parent->RemoveChild(packnode);
		}
	}
}

void PTPacking::Save()
{
	if (m_packnode == 0) return;
	if (m_packfileSelected == 0) return;

	const char* workdir = TheMediator().GetTreeworkQA()->GetWorkDir();
	if (::strcmp(workdir, "") == 0) 
	{
		AfxMessageBox("파일을 찾을 수 없습니다. QA폴더 설정하시고 다시 시도해 주세요!");
		return;
	}

	i3PackVolume vol;
	vol.setRoot(m_packnode);

	// QA 폴더 경로 설정 : 이 경로에 패킹할 파일들이 있음.
	SetCurrentDirectory(workdir);

	if (vol.SaveToFile(m_packfileSelected->GetFullpath()) == false)
		I3ASSERT("파일 저장 실패!");

	CheckSum(m_packfileSelected);
}

namespace
{
	void ExportRecur(i3FileStream* fileSrc, i3PackNode* pnode, const char* fullpath)
	{
		i3System::PrepareDirectory(const_cast<char*>(fullpath));

		for (INT32 i=0; i<pnode->getFileCount(); i++)
		{
			I3_PACK_FILE_REF* file = pnode->getFile(i);

			i3::string path(fullpath);
			path += "\\"; 
			path += file->m_szName;

			i3FileStream fileDest;
			fileDest.Create(path.c_str(), STREAM_WRITE);
			fileSrc->SetPosition(file->m_Offset, STREAM_BEGIN);
			fileDest.CopyFrom(fileSrc, file->m_Size);
		}

		for (INT32 i=0; i<pnode->getChildCount(); i++)
		{
			i3PackNode* child = static_cast<i3PackNode*>(pnode->getChild(i));

			char path[MAX_PATH] = {0,}; child->GetFullPath(path);

			i3::string folder(path);
			size_t pos = folder.find_last_of("\\");
			if (pos != i3::string::npos)
				folder = folder.substr(pos+1);

			i3::string fullpath(fullpath);
			fullpath += "\\";
			fullpath += folder.c_str();

			ExportRecur(fileSrc, child, fullpath.c_str());
		}
	}
}

void PTPacking::Export(const char* exportPath)
{
	i3FileStream fstream;
	if (fstream.Open(m_packfileSelected->GetFullpath(), STREAM_READ|STREAM_SHAREREAD) == FALSE)
	{
		AfxMessageBox("팩 파일 열기 실패.");
		return;
	}

	ExportRecur(&fstream, m_packnode, exportPath);
}

void PTPacking::CheckSum(PTFile* file)
{
	i3ResourceFile::SetProtectionCode(file->GetFullpath(), "Zepetto Inc.", 12);
}

void PTPacking::Encript(PTFile* file)
{
	if (m_packnode == 0) return;

	CFileFind find;

	BOOL fileExist = find.FindFile(file->GetFullpath());
	while (fileExist)
	{
		fileExist = find.FindNextFile();

		// 상대 Path로 만든다.
		char relativepath[MAX_PATH] = {0,};
		
		i3::make_relative_path(file->GetTopLevelFolderPath(), find.GetFilePath().GetString(), relativepath);

		I3_PACK_FILE_REF* file = 0;
		i3PackNode* packnode = m_packnode->FindPath(relativepath, &file);

		if (packnode && file)
			file->m_Attr |= I3_PACK_FILE_ATTR_SECURE;
	}
}