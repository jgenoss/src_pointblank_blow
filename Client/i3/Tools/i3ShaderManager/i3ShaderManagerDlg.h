// i3ShaderManagerDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// Ci3ShaderManagerDlg 대화 상자
class Ci3ShaderManagerDlg : public CDialog
{
// 생성
public:
	Ci3ShaderManagerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3SHADERMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV 지원

protected:
	i3TDKLogCtrl		m_LogCtrl;
	i3RenderContext *	m_pGfx;

	i3ShaderCache *		m_pCache;
	char				m_szFileName[MAX_PATH];

protected:
	void		_PlaceControls( int cx, int cy);
	void		_NewCache(void);
	void		_OpenCache( const char * pszPath);
	void		_MergeCache( const char * pszPath);
	void		_SaveCache( const char * pszPath);

	void		_UpdateAllShader(void);
	void		_UpdateShader( INT32 idx, i3Shader * pShader);

	INT32		_FindShaderByName( char * pszName);

	INT32		_getSelected(void);
	void		_getAllSelected( i3::vector<INT32>& List);

	void		_Delete(void);
	void		_Regenerate( INT32 idx = -1);
	void		_RegenerateAll(void);
	void		_GenerateHWSpecificShaders(void);
	void		_GenerateCloneShader( i3Shader * pSrcShader, I3G_GPU_VENDOR target);

	INT32		_FindShaderNameAndTarget( const char * pszName, I3G_GPU_VENDOR target);

	void		_UnselectAll(void);
	void		_SelectAllPPLShaders(void);
	void		_SelectAllPVLShaders(void);
	void		_SelectAllRedundantShaders(void);

	void		_Sort(void);

public:

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ShaderListCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	CEdit m_ShaderCountCtrl;
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnFileMerge();
	afx_msg void OnBnClickedBtnIssversion();
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileExportxml();
	afx_msg void OnFileImportxml();
};
