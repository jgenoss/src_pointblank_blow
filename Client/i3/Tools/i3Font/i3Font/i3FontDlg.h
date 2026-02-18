// i3FontDlg.h : 헤더 파일
//

#pragma once

#define CHARSETNUM			65535

#pragma pack( push, 1)
typedef struct
{
	UINT32	LabelPos;
	UINT32	StringPos;
} STRING_INFO;
#pragma pack( pop)

typedef struct
{
	INT32	nIndex;//인덱스
	INT32	nCount;//빈도(Count)
} INDEXCOUNT_DATA;

enum
{
	IMAGE_FMT_24BPP_TGA,
	IMAGE_FMT_32BPP_TGA,
	IMAGE_FMT_WHITE_32BPP_TGA,
	IMAGE_FMT_GRAY_4BPP_PSP,
	IMAGE_FMT_GRAY_4BPP,
};

#include "TexUtil.h"

// Ci3FontDlg 대화 상자
class Ci3FontDlg : public CDialog
{
// 생성
public:
	Ci3FontDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3FONT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


private: 
	CString		m_strError;
	CString		m_strProjectFile;

	LOGFONT		m_LogFont; 

	BOOL		m_bRelativePath;

	INT32		m_nNumChar;
	UINT8		m_DefaultGlyphWidth;
	UINT8		m_DefaultGlyphHeight;
	UINT8		m_GlyphWidth[CHARSETNUM];
	BOOL		m_CharList[CHARSETNUM];
	wchar_t		m_Dictionary[CHARSETNUM];
	wchar_t		m_RevDictionary[CHARSETNUM];

	INT32		m_NumSavedImage;

	BOOL		SaveImages();

	BOOL		Savei3FontTextFile();

	void		SaveBMP( LPCTSTR pszFileName, char * pImageBuf); 
	void		ConvertImage24To32( RGBQuad *pOut, RGBTripple *pIn, BOOL bWhiteRGB );
	void		ConvertImage24ToIndexed8( RGBIndexed * pOut , RGBTripple *pIn );
	void		ConvertIndexed8ToIndexed4( RGBIndexed * pOut , RGBIndexed * pOutClut , RGBIndexed *pIn );
	BOOL		ExportImage4Gray( const char *szFileName , RGBIndexed *pIn, RGBIndexed * pOutClut);
	void		SetClut4FromIndexed8( RGBIndexed * pIn , RGBIndexed * pOutClut );
	UINT8		GetIndexAtClut4( RGBIndexed * pOutClut , UINT8	nIndexAt8 );
	BOOL		ExportImage( const char *szFileName, void *pImageBuf );
//	BOOL		Test( const char * szFileName );
	BOOL		MakeCharacterList(void); 

	BOOL		LoadProject( const char *szFileName ); 
	BOOL		SaveProject( const char *szFileName );

	BOOL		LoadDictionary(void);

	BOOL		SaveSTR( BOOL bCheckDictionary );

	void		UpdateControls( BOOL bUpdate = TRUE );
	CString		m_strImageFileName;
	CString		m_strDictionaryFile;

	CString		m_strTextDir;
	CString		m_strTextFile;
	CString		m_strLabelFile;
	CString		m_strTableFile;
	CString		m_stri3FntFile;
	CString		m_stri3FontFile;
	CString		m_strSTRFile;
	CString		m_strOutDir;

	CString m_strName;
	CString m_strFontName;

	// For Test
	BOOL		ConvertSTRtoLabelnUniText();
	CString		m_strDumpLabelFile;
	CString		m_strDumpTextFile;

	BOOL		OpenFromBinary( const char *szPath );
	void		CloseSTR();
	BOOL		NewBuffers();

	STRING_INFO	*m_pTable;
	char		*m_pLabelBuf;
	wchar_t		*m_pStringBuf;

	UINT32		m_nNumString;
	UINT32		m_nTotalLabelSize;
	UINT32		m_nTotalStringSize;

	static BOOL m_bTest;

	BOOL		m_bNeedSave;
	void		SetNeedSave( BOOL bTrue = TRUE )	{ m_bNeedSave = bTrue; }
	BOOL		IsNeedSave(){ return m_bNeedSave; }

	void		SetProjectName( const char *szName );
	const char*	GetProjectName();

	void		MakeAllToAbsolutePath();
	void		MakeAllToRelativePath();

	void		SetOutputFileNames();

	// 구현
protected:
	HICON m_hIcon;

	BOOL m_bDumpDebugFiles;
	int m_nFontSize;
	int m_nImageWidth;
	int m_nImageHeight;
	int m_nSpaceX;
	int m_nSpaceY;
	int m_nFontQuality;
	CString m_strMediaDir;
	int m_nImageFormat;
	BOOL m_bBatchMode;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTblfile();
	
	afx_msg void OnBnClickedButtonTxtfile();
	afx_msg void OnBnClickedButtonImagefilename();	
	afx_msg void OnBnClickedButtonMakeimage();
	BOOL OnMakeImageCB( LPCTSTR szDir, LPCTSTR szFileName );

	afx_msg void OnBnClickedButtonGenSTR();
	BOOL OnMakeStrCB( LPCTSTR szDir, LPCTSTR szFileName );

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonOutdir();
	afx_msg void OnEnUpdateEditTxtfile();
	afx_msg void OnBnClickedButtonOpenoutdir();
	afx_msg void OnEnChangeEditOutdir();
	afx_msg void OnBnClickedButtonAutoOutdir();
	afx_msg void OnBnClickedButSelectFont();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();

	afx_msg void OnBnClickedButtonMediaDir();
	afx_msg void OnEnChangeEditMediaDir();

	afx_msg void OnBnClickedButtonMakeI3font();
	BOOL OnMakei3FontCB( LPCTSTR szDir, LPCTSTR szFileName );

	afx_msg void OnBnClickedButtonTxtDir();
	afx_msg void OnBnClickedRadioTxt2();
	afx_msg void OnBnClickedRadioTxt1();
};
