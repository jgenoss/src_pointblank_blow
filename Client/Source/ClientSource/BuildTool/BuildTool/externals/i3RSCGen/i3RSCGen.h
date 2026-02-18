// i3RSCGenView.h : iCi3RSCGenView 클래스의 인터페이스
//
#pragma once

#include <set>
#include "RSCNode.h"

// --------------------------------------------------
// i3RSCGen 툴 소스 그대로 가져와서 필요한 부분만 남김. 실행에만 문제 없도록.
// --------------------------------------------------
class i3RSCGen
{
public:
	i3RSCGen();
	virtual ~i3RSCGen();

	bool				OpenRSCFile(const char * pszFileName);
	void				AddFile(const char * szFileName);
	void				DeleteFiles(const char * pszFileName);
	void				BuildRSCFile(const char * szRSCFileName);
	void				ExportRSCBin(char * pszFileName);
	RSCNode *			FindByRelPath(const char * pszRelPath);
	bool				FindByFileName(const char* pszFileName, i3::vector<RSCNode*>& out);

protected:
	// 2011-04-27 김재진
	char				m_szWorkDir[MAX_PATH];
	////
	RSCNode			*	m_pRoot = nullptr;
	char				m_szFileName[MAX_PATH];

	std::unordered_map< std::string, RSCNode*> m_NodeMap;	// 중복노드가 없는 것으로 보임..

	RSCNode			*	m_pTypeRoot[I3RSC_TYPE_COUNT];

	RSCNode			*	_GetTypeRoot(INT32 nRscType);	//
	void				CreateRootScene();				//
	void				SetRoot(RSCNode * pRoot);		//

	void				_ReadRSCFile(i3FileStream * pStream);

	void				_Rec_BuildRSCFile(i3IniParser * pParser, RSCNode * pNode);
	void				_Rec_AddExportList(RSCNode * pParent, i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& ExportList);

	void				CreateNodeInfo(RSCNode * pNode, INT32 nStyle);
};
