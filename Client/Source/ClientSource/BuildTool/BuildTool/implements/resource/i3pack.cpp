#include "stdafx.h"
#include "i3pack.h"
#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/remove_all_blank.h"
#include "i3Base/string/ext/is_matched_wildcard.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	bool IsExceptFile(const char* path)
	{
		char fileName[MAX_PATH] = { 0, };

		const char* exceptions[] = 
		{
			"*.sif",
			"*.log",
			"*.i3CharaEditor",
			"*.i3LevelDesign",
			"*.dump",
			"*.svn",
			"*.RSG",
			"*.exe",
			"*.i3Reg",
			"*.xml",
			"*.i3RegXML",
			"*.i3ObjectEditor",
			"*.cpp",
			"*.h"
		};

		const size_t exceptCount = sizeof(exceptions) / sizeof(exceptions[0]);

		for (size_t i = 0; i<exceptCount; i++)
		{
			i3::safe_string_copy(fileName, exceptions[i], MAX_PATH);
			i3::remove_all_blank(fileName);

			if (i3::is_matched_wildcard(path, fileName))
				return true;
		}

		return false;
	}

	i3pack::i3pack() 
	{}

	i3pack::i3pack(const char* pszSrcWorkdir) : m_strPackSrcWorkdir(pszSrcWorkdir), m_strPackTgtPath(pszSrcWorkdir)
	{
		_CreatePack();
	}

	i3pack::i3pack(const char* pszSrcWorkdir, const char* pszTgtWorkdir) : m_strPackSrcWorkdir(pszSrcWorkdir), m_strPackTgtPath(pszTgtWorkdir)
	{
		_CreatePack();
	}

	i3pack::~i3pack()
	{
		I3_SAFE_RELEASE(m_node);
	}

	void i3pack::_CreatePack()
	{
		m_node = i3PackNode::new_object();
		m_node->SetName("/");
	}

	bool i3pack::LoadPack(const char* pszi3PackAbsPath)
	{
		const Path i3PackAbsPath(pszi3PackAbsPath);
		const std::string ext = i3PackAbsPath.extension().string();

		if (fs::exists(i3PackAbsPath) && (i3::generic_is_iequal(ext, ".i3Pack")))
		{
			// 팩 파일 로드.
			i3PackVolume vol;

			if (!vol.LoadFromFile(pszi3PackAbsPath))
				return false;

			i3PackNode* node = (i3PackNode*)vol.getRoot();
			if (i3::same_of<i3PackNode*>(node))
			{
				I3_REF_CHANGE(m_node, node);
				return true;
			}
		}

		return false;
	}

	bool i3pack::SavePack(const char* targetDir, const char* fileName)
	{
		if (m_node == nullptr)
			return false;

		const Path tgtDir(targetDir);

		if (!fs::exists(tgtDir))
			return false;

		if (!fs::is_directory(tgtDir))
			return false;

		wchar_t prevDir[MAX_PATH] = { 0, };

		GetCurrentDirectory(sizeof(prevDir), prevDir);

		// 반드시 패킹할 원본 파일이 있는 경로로 설정되어 있어야 함.
		const std::string strCurrDir(m_strPackSrcWorkdir);
		const std::wstring wstrCurrDir(strCurrDir.begin(), strCurrDir.end());
		SetCurrentDirectory(wstrCurrDir.c_str());

		i3PackVolume vol;
		
		vol.setRoot(m_node);

		if (!i3::generic_is_iequal(m_node->GetName(), "/"))	// 루트노드가 절대경로로 저장되어 있는 경우가 있다. 상대경로로 바꾼다.
			m_node->SetName("/");

		Path fileAbsPathWindow(targetDir);	
		fileAbsPathWindow /= fileName;	
		fileAbsPathWindow.make_preferred();

		vol.SaveToFile(fileAbsPathWindow.string().c_str());

		SetCurrentDirectory(prevDir);

		return true;
	}

	bool i3pack::SavePack(const char* packName)
	{
		return SavePack(m_strPackTgtPath.c_str(), packName);
	}

	bool i3pack::AddFileToPack(const char* pszFileRelPath, const bool includeSubdirOK)
	{
		if (i3::contain_string(pszFileRelPath, ".i3Pack") != -1)
			return false;

		Path fileAbsPath(m_strPackSrcWorkdir);
		fileAbsPath /= pszFileRelPath;

		const std::string& strFileAbsPath = fileAbsPath.string();

		i3PackNode* node = nullptr;

		if (i3System::IsDirectory(strFileAbsPath.c_str()))
		{
			node = _AddPathsToPackNode(strFileAbsPath.c_str(), true);

			_AddDirectory(strFileAbsPath.c_str(), node, includeSubdirOK);

			return true;
		}

		CFileFind fd;

		const std::wstring wFullpath(strFileAbsPath.begin(), strFileAbsPath.end());

		if (!fd.FindFile(wFullpath.c_str()))
			return false;

		node = _AddPathsToPackNode(strFileAbsPath.c_str(), false);
		
		if (!_CheckPathValid(strFileAbsPath.c_str()))
			return false;

		// 같은목록을 추가하려고 하면 기존의 리스트를 삭제하고 다시 추가합니다. 
		for (INT32 i=0; i<node->getFileCount(); i++)
		{
			const char* ch = node->getFile(i)->m_szName;
			INT32 result = i3::contain_string(strFileAbsPath.c_str(), ch);
			if (result != -1)
				node->RemoveFile(node->getFile(i)->m_szName);
		}

		BOOL find = true;
		while (find)
		{
			find = fd.FindNextFile();

			if (fd.IsHidden())
				continue;

			if (fd.IsSystem())
				continue;

			if (fd.IsDirectory())
				continue;

			i3::string strFileName;	i3::utf16_to_mb(fd.GetFileName().GetString(), strFileName);

			node->AddFile(strFileName.c_str(), 0, static_cast<UINT32>(fd.GetLength()), 0);
		}

		return true;
	}

	bool i3pack::AddFileToPack(const char* pszFileRelPath)
	{
		return AddFileToPack(pszFileRelPath, true);	// 하위 디렉터리 포함.
	}

	bool i3pack::_CheckPathValid(const char* path) const
	{
		i3::vector<i3::rc_string> workdirTokens;
		i3::vector<i3::rc_string> pathTokens;

		i3::resolve_path_token(m_strPackSrcWorkdir.c_str(), workdirTokens);
		i3::resolve_path_token(path, pathTokens);

		for (size_t i=0; i<workdirTokens.size(); i++)
		{
			if (workdirTokens[i] != pathTokens[i])
				return false;
		}

		return true;
	}

	i3PackNode* i3pack::_AddPathsToPackNode(const char* pszFileAbsPath, const bool isDir)
	{
		char relpath[MAX_PATH] = {0,};

		if (!_CheckPathValid(pszFileAbsPath))
			return nullptr;

		i3::make_relative_path(m_strPackSrcWorkdir.c_str(), pszFileAbsPath, relpath);

		i3::vector<i3::rc_string> tokenList;
		i3::resolve_path_token(relpath, tokenList);
		
		i3PackNode* parentNode = nullptr;
		i3PackNode* node = nullptr;

		parentNode = m_node;

		size_t cnt = tokenList.size();

		// 만약 주어진 경로가 폴더가 아니라 파일에 대한 것이라면
		// 가장 마지막의 Token은 File명이기 때문에 고려하지 않는다.
		if (!tokenList.empty() && !isDir)
			cnt--;

		for (size_t i = 0; i < cnt; i++)
		{
			const char* name = tokenList[i].c_str();

			node = (i3PackNode*)parentNode->FindChildByName(name);

			if (node == nullptr)
			{
				// 없던 Path... 추가해 준다.
				node = i3PackNode::new_object_ref();
				node->SetName(name);

				parentNode->AddChild(node);
			}

			parentNode = node;
		}

		return parentNode;
	}

	void i3pack::_AddDirectory(const char* pszFileAbsPath, i3PackNode* node, const bool includeSubdirOK)
	{
		_BuildupFileList(pszFileAbsPath, node);	// 현재 디렉터리만.

		if (!includeSubdirOK)
			return;

		// 모든 하위 디렉터리 포함.
		std::string strFileAbsPath(pszFileAbsPath);
		strFileAbsPath += "\\*.*";

		const std::wstring wstrPath(strFileAbsPath.begin(), strFileAbsPath.end());

		CFileFind fd;
		
		BOOL findOK = fd.FindFile(wstrPath.c_str());

		while (findOK)
		{
			findOK = fd.FindNextFile();

			if (fd.IsHidden())
				continue;

			if (fd.IsSystem())
				continue;

			if (fd.IsDots())
				continue;

			if (!fd.IsDirectory())
				continue;

			const std::wstring wstrFileName(fd.GetFileName());
			const std::string strFileName(wstrFileName.begin(), wstrFileName.end());

			i3PackNode* newNode = 
				(i3PackNode*)node->FindChildByName(strFileName.c_str());
			
			if (newNode == nullptr)
			{
				newNode = i3PackNode::new_object_ref();

				node->AddChild(newNode);
				newNode->SetName(strFileName.c_str());
			}
			
			std::string strDirAbsPath(pszFileAbsPath);
			strDirAbsPath += "/";
			strDirAbsPath += strFileName;

			_AddDirectory(strDirAbsPath.c_str(), newNode, includeSubdirOK);
		}
	}

	bool i3pack::_BuildupFileList(const char* pszFileAbsPath, i3PackNode* parentNode)
	{	
		parentNode->SetFileCount(0);

		char targetPath[MAX_PATH] = { 0, };
		sprintf(targetPath, "%s\\*.*", pszFileAbsPath);

		i3::wstring wstrTargetPath;	i3::mb_to_utf16(targetPath, wstrTargetPath);

		WIN32_FIND_DATA fd;
		HANDLE handle = FindFirstFile(wstrTargetPath.c_str(), &fd);

		const UINT32 mask = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY;

		while (handle != INVALID_HANDLE_VALUE)
		{
			if ((fd.dwFileAttributes & mask) == 0)
			{
				i3::string strFileName;	i3::utf16_to_mb(fd.cFileName, strFileName);

				if (!IsExceptFile(strFileName.c_str()))
				{
					parentNode->AddFile(strFileName.c_str(), 0, fd.nFileSizeLow, 0);
				}
			}

			if (!FindNextFile(handle, &fd))
				break;
		}

		FindClose(handle);

		return true;
	}

	bool i3pack::RemoveFileFromPack(const char* pszFileRelPath, const bool includeSubdirOK)
	{
		CFileFind find;

		Path fileAbsPath(m_strPackSrcWorkdir);	fileAbsPath /= pszFileRelPath;

		const std::string& strFileAbsPath = fileAbsPath.string();

		const std::wstring wstrFileAbsPath(strFileAbsPath.begin(), strFileAbsPath.end());

		BOOL findOK = find.FindFile(wstrFileAbsPath.c_str());
		while (findOK)
		{
			findOK = find.FindNextFile();

			if (find.IsDots())
				continue;

			if (find.IsSystem())
				continue;

			if (!includeSubdirOK)
			{
				if (find.IsDirectory())
					continue;
			}

			// 상대 Path로 만든다.
			char szFileRelPath[512] = {0,};

			const std::wstring wstrFileAbsPath(find.GetFilePath());
			const std::string strFileAbsPath(wstrFileAbsPath.begin(), wstrFileAbsPath.end());
			
			i3::make_relative_path(m_strPackSrcWorkdir.c_str(), strFileAbsPath.c_str(), szFileRelPath);

			I3_PACK_FILE_REF* info = nullptr;
			i3PackNode* node = m_node->FindPath(szFileRelPath, &info);
			if (node == nullptr)
				continue;

			if (info != nullptr) // 파일 제거
			{
				const std::wstring wstrFileName(find.GetFileName());
				const std::string strFileName(wstrFileName.begin(), wstrFileName.end());
				
				node->RemoveFile(strFileName.c_str());
			}
			else // Directory 제거.
			{
				i3PackNode* parentNode = (i3PackNode*)node->getParent();
				parentNode->RemoveChild(node);
			}
		}

		return true;
	}

	bool i3pack::RemoveFileFromPack(const char* fileRelPath)
	{
		return RemoveFileFromPack(fileRelPath, true);
	}

	void i3pack::EncryptFileInPack(const char* pszFileRelPath, const bool enc)
	{
		if (m_node == nullptr)
			return;

		Path fileAbsPath(m_strPackSrcWorkdir);	fileAbsPath /= pszFileRelPath;

		const std::string& strFileAbsPath = fileAbsPath.string();
		const std::wstring wstrFileAbsPath(strFileAbsPath.begin(), strFileAbsPath.end());

		CFileFind fd;
		BOOL findOK = fd.FindFile(wstrFileAbsPath.c_str());

		char szEncTargetRelPath[512] = { 0, };
		std::wstring wstrEncTargetAbsPath;
		std::string strEncTargetAbsPath;

		while (findOK)
		{
			findOK = fd.FindNextFile();

			wstrEncTargetAbsPath = fd.GetFilePath();

			strEncTargetAbsPath.assign(wstrEncTargetAbsPath.begin(), wstrEncTargetAbsPath.end());

			::memset(szEncTargetRelPath, 0, 512);	
			i3::make_relative_path(m_strPackSrcWorkdir.c_str(), strEncTargetAbsPath.c_str(), szEncTargetRelPath);

			I3_PACK_FILE_REF* info;
			i3PackNode* node = m_node->FindPath(szEncTargetRelPath, &info);

			if ((node != nullptr) && (info != nullptr))
			{
				if (enc)
					info->m_Attr |= I3_PACK_FILE_ATTR_SECURE;
				else
					info->m_Attr &= ~I3_PACK_FILE_ATTR_SECURE;
			}
		}
	}

	bool i3pack::SetChecksumToPack(const char* i3packName)
	{
		Path i3PackAbsPathWindow(m_strPackTgtPath);
		i3PackAbsPathWindow /= i3packName;
		i3PackAbsPathWindow.make_preferred();

		if (!fs::exists(i3PackAbsPathWindow))
			return false;

		i3ResourceFile::SetProtectionCode(i3PackAbsPathWindow.string().c_str(), "Zepetto Inc.", 12);

		return true;
	}

	bool i3pack::SetChecksumToPack(const char* pszPackDir, const char* i3PackName)
	{
		Path i3PackAbsPathWindow(pszPackDir);	
		i3PackAbsPathWindow /= i3PackName;
		i3PackAbsPathWindow.make_preferred();

		if (!fs::exists(i3PackAbsPathWindow))
			return false;

		i3ResourceFile::SetProtectionCode(i3PackAbsPathWindow.string().c_str() , "Zepetto Inc.", 12);
		
		return true;
	}

	void i3pack::ExportFiles(const char* pszExportAbsDirPath)
	{
		if (fs::is_directory(pszExportAbsDirPath))
		{
			i3FileStream fs;

			_Rec_Export(&fs, m_node, pszExportAbsDirPath);
		}
	}

	void i3pack::_Rec_Export(i3FileStream* fs, i3PackNode* node, const char* path)
	{
		char szBase[MAX_PATH] = {0,};

		if (i3::generic_is_iequal(node->GetName(), "/") == false)
		{
			sprintf(szBase, "%s/%s", path, node->GetName());
			i3System::PrepareDirectory(szBase);
		}
		else
		{
			strcpy(szBase, path);
		}

		// 파일 저장
		for (INT32 i=0; i<node->getFileCount(); i++)
		{
			char szPath[MAX_PATH] = { 0, };

			I3_PACK_FILE_REF* info = node->getFile(i);

			i3FileStream fs_dst;

			sprintf(szPath, "%s/%s", szBase, info->m_szName);

			if (!fs_dst.Create(szPath, STREAM_WRITE))
				continue;

			fs->SetPosition(info->m_Offset, STREAM_BEGIN);

			fs_dst.CopyFrom(fs, info->m_Size);
		}

		for (INT32 i=0; i<node->getChildCount(); i++)
		{
			i3PackNode* childNode = (i3PackNode*)node->getChild(i);

			_Rec_Export(fs, childNode, (const char*)szBase);
		}
	}

	void i3pack::CopyFrom(const i3pack& src)
	{
		I3_SAFE_RELEASE(m_node);
		m_node = src.m_node;
		m_node->AddRef();

		m_strPackSrcWorkdir = src.m_strPackSrcWorkdir;
		m_strPackTgtPath = src.m_strPackTgtPath;
	}

	namespace i3packUtil
	{
		bool DisableChecksum(const char* pszi3packAbsPath)
		{
			i3FileStream fs;
			I3_RESOURCE_FILE_HEADER2 head2;
			UINT32 rc;

			if (fs.Open(pszi3packAbsPath, STREAM_SHAREREAD|STREAM_SHAREWRITE))
			{
				fs.SetPosition(4, STREAM_BEGIN);			// ID 분만큼 스킵
				rc = fs.Read(&head2, sizeof(head2));
				if (rc != STREAM_ERR)
				{
					head2.m_ChecksumCode[0] = 0;
					head2.m_ChecksumCode[1] = 0;
					head2.m_ChecksumCode[2] = 0;
					head2.m_ChecksumCode[3] = 0;

					fs.SetPosition(4, STREAM_BEGIN);		// ID분 만큼 스킵
					rc = fs.Write(&head2, sizeof(head2));

					if (rc != STREAM_ERR)
						return true;
				}

				fs.Close();
			}

			return false;
		}

		void GetChecksumCode(const char* pszi3packAbsPath, std::vector<UINT32>& out)
		{
			i3FileStream fs;
			I3_RESOURCE_FILE_HEADER2 head2;
			UINT32 rc;

			if (fs.Open(pszi3packAbsPath, STREAM_READ | STREAM_SHAREREAD))
			{
				fs.SetPosition(4, STREAM_BEGIN);			// ID 분만큼 스킵
				rc = fs.Read(&head2, sizeof(head2));
				if (rc != STREAM_ERR)
				{
					out.push_back(head2.m_ChecksumCode[0]);
					out.push_back(head2.m_ChecksumCode[1]);
					out.push_back(head2.m_ChecksumCode[2]);
					out.push_back(head2.m_ChecksumCode[3]);
				}

				fs.Close();
			}
		}

		void _Rec_OffsetCheck(i3FileStream* fs, i3PackNode* node)
		{
			for (INT32 i = 0; i < node->getFileCount(); i++)
			{
				I3_PACK_FILE_REF* info = node->getFile(i);
				
				if (info->m_Size > 0 && info->m_Offset <= 0)	// 파일 사이즈가 있는데 offset이 0 일 수 없음.
				{
					std::string stri3PackName(info->m_szName);
					std::wstring wstri3PackName(stri3PackName.begin(), stri3PackName.end());

					std::wstring wstrMessage(L"패킹 중 유효하지 않은 offset 이 들어갔습니다. 다시 패킹하시기 바랍니다 : ");
					wstrMessage += wstri3PackName;

					::MessageBox(NULL, wstrMessage.c_str(), L"i3Pack invalid offset", MB_OK);
				}
			}

			for (INT32 i = 0; i < node->getChildCount(); i++)
			{
				i3PackNode* childNode = (i3PackNode*)node->getChild(i);
				_Rec_OffsetCheck(fs, childNode);
			}
		}

		void CheckOffsetValidity(const char* pszi3PackAbsPath)
		{
			const Path i3PackAbsPath(pszi3PackAbsPath);
			const std::string ext = i3PackAbsPath.extension().string();

			if (fs::exists(i3PackAbsPath) && (i3::generic_is_iequal(ext, ".i3Pack")))
			{
				// 팩 파일 로드.
				i3PackVolume vol;

				if (!vol.LoadFromFile(pszi3PackAbsPath))
					return;

				i3PackNode* rootNode = (i3PackNode*)vol.getRoot();
				if (i3::same_of<i3PackNode*>(rootNode))
				{
					i3FileStream fs;
					_Rec_OffsetCheck(&fs, rootNode);
				}
			}
		}
	}
}