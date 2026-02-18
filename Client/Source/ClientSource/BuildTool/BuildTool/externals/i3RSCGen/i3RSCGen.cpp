// i3RSCGenView.cpp : Ci3RSCGenView 클래스의 구현
//

#include "stdafx.h"
#include "i3RSCGen.h"

#include "i3Base/i3StringListBuffer.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/compare/generic_is_niequal.h"

#include "i3Base/string/ext/utf16_to_mb.h"

using namespace std;

i3RSCGen::i3RSCGen()
{
	m_pRoot = NULL;

	ZeroMemory(m_pTypeRoot, sizeof(RSCNode *) * I3RSC_TYPE_COUNT);

	m_szFileName[0] = 0;
}

i3RSCGen::~i3RSCGen()
{
	I3_SAFE_RELEASE(m_pRoot);
}

RSCNode * i3RSCGen::_GetTypeRoot(INT32 nRscType)
{
	I3ASSERT(nRscType < I3RSC_TYPE_COUNT);

	if (m_pTypeRoot[nRscType] == NULL)
	{
		const char * szRootName[I3RSC_TYPE_COUNT] = { "Scene", "Charactor", "World", "Animation", "Texture", "Sound", "Binary", "AnimationPack", "Object", "UI" };

		m_pTypeRoot[nRscType] = RSCNode::new_object_ref();
		m_pTypeRoot[nRscType]->SetName(szRootName[nRscType]);
		m_pTypeRoot[nRscType]->SetStyle(I3RSC_ATTR_FOLDER | nRscType);

		m_pRoot->AddChild(m_pTypeRoot[nRscType]);
	}

	return m_pTypeRoot[nRscType];
}

void i3RSCGen::CreateRootScene()
{
	SetRoot(NULL);
	{
		RSCNode * pNewRoot;

		//SetRoot를 통해서 root가 되기때문에 new_object_ref()로
		pNewRoot = RSCNode::new_object_ref();
		pNewRoot->SetName("/");
		pNewRoot->SetStyle(I3RSC_ATTR_FOLDER);

		SetRoot(pNewRoot);

		i3mem::FillZero(m_pTypeRoot, sizeof(RSCNode*) * I3RSC_TYPE_COUNT);
	}
}

void i3RSCGen::SetRoot(RSCNode * pRoot)
{
	if (pRoot != NULL)
	{
		pRoot->AddRef();
	}

	I3_SAFE_RELEASE(m_pRoot);
	m_NodeMap.clear();

	m_pRoot = pRoot;

	if (m_pRoot != NULL)
	{
		m_pRoot->GatherAllNodeToPathMap(m_NodeMap);
	}
}

void i3RSCGen::AddFile(const char * pszFileName)
{
	i3::rc_string	strRelPath;
	char szExt[32] = { 0, };

	const char * pszName;

	i3::vector<i3::rc_string> StrList;

	UINT32	nStyle = 0;
	RSCNode * pNode, *pChild;

	i3::make_relative_path(m_szWorkDir, pszFileName, strRelPath);


	// 2011-04-27 김재진 : Working Directory 변수 변경으로 인한 주석처리
	//	i3String::MakeRelativePath( m_vecWorkingDir[m_iIndex - 1], pszFileName, szRelPath);

	i3::resolve_path_token(strRelPath, StrList);

	//	i3String::SplitFileExt( pszFileName, szExt, sizeof( szExt) -1);
	i3::extract_fileext(pszFileName, szExt);

	//File Attr
	nStyle = Geti3RscTypeWithFileExt(szExt);

	pNode = _GetTypeRoot(nStyle);

	pszName = StrList.back().c_str();		// StrList.Items[ StrList.GetCount() -1];	//StrList의 Last가 실제 File Name이다.

	pChild = FindByRelPath(strRelPath.c_str());
	if (pChild == NULL)
	{
		pChild = RSCNode::new_object_ref();

		pChild->SetName(pszName);
		pChild->SetRelativePath(strRelPath);

		CreateNodeInfo(pChild, nStyle);

		pNode->AddChild(pChild);

		pChild->AddThisToPathMap(m_NodeMap);
	}
}

void i3RSCGen::DeleteFiles(const char* pszFileName)
{
	i3::vector<RSCNode*> i3RSCNodeList;
	if (FindByFileName(pszFileName, i3RSCNodeList))
	{
		RSCNode* i3RSCParentNode = nullptr;
		for (auto &curri3RSCNode : i3RSCNodeList)
		{
			i3RSCParentNode = curri3RSCNode->GetParent();
			if (i3RSCParentNode)
				i3RSCParentNode->RemoveChild(curri3RSCNode);
		}
	}

	Path currFileRelPath;
	for (auto it = m_NodeMap.begin(); it != m_NodeMap.end();)
	{
		currFileRelPath = it->first;

		if (i3::generic_is_iequal(pszFileName, currFileRelPath.filename().string()))
			m_NodeMap.erase(it++);
		else
			++it;
	}
}

RSCNode* i3RSCGen::FindByRelPath(const char* pszRelPath)
{
	Path fileRelPathWindow(pszRelPath);	fileRelPathWindow.make_preferred();
	std::string strRelPathUpperWindow(fileRelPathWindow.string()); i3::to_upper(strRelPathUpperWindow);

	auto it = m_NodeMap.find(strRelPathUpperWindow);

	return (it == m_NodeMap.end()) ? nullptr : it->second;
}

bool i3RSCGen::FindByFileName(const char* pszFileName, i3::vector<RSCNode*>& out)
{
	Path currFileRelPath;

	for (auto it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it)
	{
		currFileRelPath = it->first;

		if (i3::generic_is_iequal(pszFileName, currFileRelPath.filename().string()))
			out.push_back(it->second);
	}

	return !out.empty();
}

void i3RSCGen::_ReadRSCFile(i3FileStream * pStream)//const char * szRSCFileName)
{
	i3IniParser	Parser;

	if (!Parser.Open(pStream))
	{
		I3PRINTLOG(I3LOG_FATAL, "Could not Open Stream", pStream->GetFileName());
		return;
	}

	strcpy(m_szFileName, pStream->GetFileName());

	INT32 i;
	INT32 nCount = Parser.GetSectionCount();
	for (i = 0; i< nCount; ++i)
	{
		if (Parser.ReadSectionByIndex(i))
		{
			i3::rc_string strFileName;

			RSCNode * pNode = RSCNode::new_object_ref();
			RSC_DATA * pData = pNode->GetData();

			i3IniSectionInfo * pSection = Parser.GetSection(i);

			pData->m_strFilePath = pSection->Name;

			i3::extract_filename(pData->m_strFilePath, strFileName);

			pNode->SetName(strFileName);

			CFieldPack * pPack = new CFieldPack;
			pPack->Load(&Parser, pSection);

			pData->m_spField.reset(pPack);

			i3::rc_string* any_val = i3::unsafe_any_cast<i3::rc_string>(&pPack->GetField(0)->m_FieldData);

			for (INT32 j = 0; j < I3RSC_TYPE_COUNT; ++j)
			{
				if (*any_val == g_RSCStyleDef[j].m_strFieldPackName)
				{
					pData->m_Attr = j;
					break;
				}
			}

			pNode->GatherAllNodeToPathMap(m_NodeMap);

			_GetTypeRoot(pData->m_Attr & I3RSC_TYPE_MASK)->AddChild(pNode);

			I3PRINTLOG(I3LOG_NOTICE, "Added File: %s", pData->m_strFilePath.c_str());
		}
	}

	Parser.Close();
}

void i3RSCGen::BuildRSCFile(const char * szRSCFileName)
{
	i3IniParser	Parser;

	if (!Parser.Create(szRSCFileName))
	{
		I3PRINTLOG(I3LOG_FATAL, "Could not Open File", (char*)szRSCFileName);
		return;
	}

	strcpy(m_szFileName, szRSCFileName);

	_Rec_BuildRSCFile(&Parser, m_pRoot);

	Parser.Close();
}

void i3RSCGen::_Rec_BuildRSCFile(i3IniParser * pParser, RSCNode * pNode)
{
	if (pNode == NULL)	return;

	CFieldPack * pField = pNode->GetData()->m_spField.get();

	if ((pNode->GetStyle() & I3RSC_ATTR_FOLDER) == 0)
	{
		if (pField != NULL)
		{
			pField->Save(pParser);
		}
	}

	INT32 nCount = pNode->GetChildCount();
	RSCNode * pChildNode = NULL;
	for (INT32 i = 0; i< nCount; ++i)
	{
		pChildNode = pNode->GetChild(i);

		_Rec_BuildRSCFile(pParser, pChildNode);
	}
}

bool i3RSCGen::OpenRSCFile(const char * pszFileName)
{
	i3FileStream File;
	if (!File.Open(pszFileName, STREAM_READ))
	{
		I3PRINTLOG(I3LOG_FATAL, "Couldn't Open File: %s", (char*)pszFileName);
		return false;
	}

	CreateRootScene();

	_ReadRSCFile(&File);

	File.Close();

	return true;
}


static void _ExportRSC(const i3::vector<RSCNode*>& List, char * pszFileName)
{
	I3ASSERT(pszFileName != NULL);

	i3FileStream FileStream;
	char szFileName[MAX_PATH];
	i3::change_file_ext_copy(szFileName, pszFileName, "RSC");

	UINT32	nResult = 0;
	UINT8	nID[4];
	INT32	nCount = (INT32)List.size();

	if (FileStream.Create(szFileName, STREAM_WRITE | STREAM_SHAREWRITE) == STREAM_ERR)
	{
		return;
	}

	i3mem::Copy(nID, "RSCF", sizeof(UINT8) * 4);

	// File ID
	FileStream.Write(nID, sizeof(UINT8) * 4);

	// list Count
	FileStream.Write(&nCount, sizeof(INT32));

	for (INT32 i = 0; i < nCount; ++i)
	{
		RSCNode * pNode = List[i];
		nResult = pNode->Export(&FileStream);
	}

	return;
}

void i3RSCGen::_Rec_AddExportList(RSCNode * pParent, i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& ExportList)
{
	if (pParent == NULL)
		return;

	if (i3::same_of<RSCNode*>(pParent)) //->IsExactTypeOf( RSCNode::static_meta()))
	{
		RSC_DATA * pData = pParent->GetData();
		if (pParent != m_pRoot)
		{
			if ((pData->m_Attr & I3RSC_ATTR_FOLDER) == 0)
			{
				ExportList.insert(pParent);
			}
		}
	}

	for (INT32 i = 0; i < pParent->GetChildCount(); ++i)
	{
		RSCNode * pChild = (RSCNode *)pParent->GetChild(i);
		if (pChild != NULL)
		{
			_Rec_AddExportList(pChild, ExportList);
		}
	}
}

void i3RSCGen::ExportRSCBin(char * pszFileName)
{
	if (pszFileName == NULL)
		return;

	if (m_pRoot == NULL)
		return;

	i3::vector<RSCNode*> ExportList;
	m_pRoot->GatherAllNode_NonFolder(ExportList);
	i3::sort(ExportList.begin(), ExportList.end(), RSCNode_ExportComp());

	_ExportRSC(ExportList, pszFileName);
}

void i3RSCGen::CreateNodeInfo(RSCNode * pNode, INT32 nStyle)
{
	I3ASSERT(pNode != NULL);

	CFieldPack * pPack = new CFieldPack;


	pPack->Create(nStyle);

	pNode->GetData()->m_Attr = nStyle;
	pNode->GetData()->m_spField.reset(pPack);

	{//Working Path를 기준으로한 Relative Path를 설정한다.
		RSC_FIELD_INFO * pNameField = pPack->GetField(1);

		if (pNameField != NULL)
		{
			if (i3::generic_is_iequal(pNameField->m_strFieldName, "Path") ||
				i3::generic_is_iequal(pNameField->m_strFieldName, "Scene"))
			{
				pNameField->m_FieldData = pNode->GetRelativePath();
			}
		}
	}

	if (nStyle == I3RSC_TYPE_SOUND)
	{
		RSC_FIELD_INFO * pInfo = (RSC_FIELD_INFO*)pPack->GetField(4);
		pInfo->m_FieldData = 100;

		pInfo = (RSC_FIELD_INFO*)pPack->GetField(5);
		pInfo->m_FieldData = 100;

		pInfo = (RSC_FIELD_INFO*)pPack->GetField(3);
		pInfo->m_FieldData = 50.0f;
	}
}