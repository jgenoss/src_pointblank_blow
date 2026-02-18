
#pragma once


#include "i3XFileLexer.h"
#include "i3XFileTemplate.h"

class Ci3GraphDlg;

// 파일 헤더의 자료의 바이트 크기
enum
{
	MAGIC_NUMBER_SIZE	= 4,
	VERSION_NUMBER_SIZE	= 4,
	FORMAT_TYPE_SIZE	= 4,
	FLOAT_SIZE			= 4,

	FILE_HEADER_SIZE	= (MAGIC_NUMBER_SIZE+VERSION_NUMBER_SIZE+FORMAT_TYPE_SIZE+FLOAT_SIZE),
};

struct	XFileHeader
{
	char MagicNumber[MAGIC_NUMBER_SIZE];
	char VersionNumber[VERSION_NUMBER_SIZE];
	char FormatType[FORMAT_TYPE_SIZE];
	char FloatSize[FLOAT_SIZE];
};


class i3XFileParser
{
public:
	i3XFileParser();
	~i3XFileParser();

public:
	BOOL	Open(const char* pFileName);
	BOOL	Open(i3FileStream* pStream);

	i3SceneGraphInfo *	GetSceneGraphInfo()					{return m_pSgInfo;}
	void				SetLogWnd(Ci3GraphDlg * pLogWnd)	{m_pLogWnd = pLogWnd;}
	void				Log(INT32 LogLevel, const char *format, ...);


private:
	void	Clear();
	void	EndLogMessage();


public:
	i3XFileTemplate *	GetDataObjectByName(const char* pName);
	void				StartParseTemplate(i3XFileTemplate * pTemplate);
	void				EndParseTemplate(i3XFileTemplate * pTemplateClass);

	const char *		GetFilePath()	{return m_FilePath;}
	const char *		GetFullPath()	{return m_FileFullPath;}


private:
	// 파일 헤더를 체크합니다.
	BOOL	CheckFileHeader(i3FileStream* pStream);

	// 데이터를 분석합니다.
	BOOL	BeginParsing();
	BOOL	EndParsing();


	// 수집된 데이터로 실제 변환을 수행합니다.
	BOOL	BuildFrame();
	BOOL	BuildMesh();
	BOOL	BuildAnimationSet();

	BOOL	CalcuBoneIndex();
	BOOL	CalcuSkinBoneIndex();


	i3XFileFrame *			FindFrameByName(const char * pName);
	i3XFileSkinWeights *	FindSkinWeightInfoByName(const char * pName);

private:
	char			m_FileFullPath[MAX_PATH];
	char			m_FilePath[MAX_PATH];		// 파일이 포함되어 있는 디렉토리 경로
	char			m_FileName[MAX_PATH];

	i3XFileLexer*	m_pLexer;
	i3FileStream *	m_pStream;

	BOOL			m_CreatedRoot;

	XFileHeader			m_Header;
	i3SceneGraphInfo *	m_pSgInfo;
	i3Node *	m_pRootNode;
	i3AttrSet *	m_pRootAttrSet;
	BOOL		m_TextureEnable;

	i3List		m_DataObjectList;

	i3List		m_MeshList;
	i3List		m_FrameList;
	i3List		m_SkinWeightList;
	i3List		m_AnimationSetList;

	Ci3GraphDlg *	m_pLogWnd;
};



BOOL	IsIgnoreTemplate(UINT32 IdenNumber);