// i3AnimationKeyDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"


// Ci3AnimationKeyDlg 대화 상자
class Ci3AnimationKeyDlg : public CDialog
{
// 생성
public:
	Ci3AnimationKeyDlg(CWnd* pParent = NULL);	// 표준 생성자
	~Ci3AnimationKeyDlg();
// 대화 상자 데이터
	enum { IDD = IDD_I3ANIMATIONKEY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
	i3TransformSequence *	m_pCurSeq;
	char	m_szFileName[MAX_PATH];

protected:
	void	ReplaceControls( INT32 cx, INT32 cy);
	bool	LoadAnim( const char * pszPath);
	bool	SaveAnim( const char * pszPath);

	void	RemoveTrack( INT32 idx);
	void	SelectTrack( INT32 idx);
	void	UpdateAllTracks(void);
	INT32	GetSelectedTrack(void);

	void	UpdateKey( INT32 idx);
	void	UpdateAllKeys(void);
	void	RemoveKey( INT32 idx);
	void	GetSelectedKey( i3::vector<INT32>&  List);

	void	SetTitleName( const char * pszName);

	void	OnDelete(void);

public:
	i3Animation *			m_pAnim;

	void				SetAnimation( i3Animation * pAnim);

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
	CListCtrl m_TrackListCtrl;
	CListCtrl m_KeyListCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMClickTracklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTracklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnFileExit();
	afx_msg void OnNMRclickKeylist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileOptimize();
};
