// i3RSCGenView.h : icRSCGen 클래스의 인터페이스
//
#pragma once

#include <set>
#include "RSCNode.h"
#include "RSCGenDefine.h"

class cRSCGen : public i3ElementBase
{
public:
	
	I3_CLASS_DEFINE( cRSCGen, i3ElementBase);

	cRSCGen();
	virtual ~cRSCGen();

public:
	void				AddFileToMap( const char * );
	bool				CheckSameName( const char * );

	// 2011-04-27 김재진
	bool				m_bFirst;
	bool				m_bFile;
	char				m_szWorkDir[MAX_PATH];
	char				m_szWork[MAX_PATH];
	////
//	set<const char *, Comp>	m_FileList;
	RSCNode			*	m_pRoot;
	char				m_szFileName[MAX_PATH];

	i3::vector_set<RSCNode*, RSCNode_NodeComp> m_NodeList;						// 중복노드가 없는 것으로 보임..


	RSCNode			*	m_pTypeRoot[I3RSC_TYPE_COUNT];

	void				_ClearAll();
	i3::vector<i3::rc_string>	m_ExceptList;

	RSCNode			*	_GetTypeRoot( INT32 nRscType);
	void				CreateRootScene();
	void				SetRoot( RSCNode * pRoot);

	RSCNode *			FindByRelPath( const char * pszRelPath);

	void				ScanAllNodes( RSCNode * pRoot);


	void				AddFile( const char * szFileName);
	void				AddDirectory( const char * szFileName);
	BOOL				LookForFiles( const char * pszPath, RSCNode * pParent);

	void				OpenRSCFile( const char * pszFileName);
	void				_ReadRSCFile( i3FileStream * pStream);
	void				_ReadRSCBin( i3FileStream * pStream);

	void				BuildRSCFile( const char * szRSCFileName);
	void				ExportRSCBin( char * pszFileName);
	void				_Rec_BuildRSCFile( i3IniParser * pParser, RSCNode * pNode);
	void				_Rec_AddExportList( RSCNode * pParent, i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& ExportList);
	
	void				MakeExceptList( void);
	BOOL				IsAcceptable( char * pszStr);

	void				CreateNodeInfo( RSCNode * pNode, INT32 nStyle);

protected:
	bool m_UpStageFile;

public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	void OnDropFiles(HDROP hDropInfo);
	void OnInitialUpdate();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnFileSaveAs();
	void OnFileSave();
	void OnFileOpen();
	void OnFileNew();
	// 2011-04-27 김재진
	bool AutoWorkPath(const char* pszFileName, bool bPrsvDir = false);
	bool WorkingFolderYN(const char* pszNewFolder);
	/////
};

