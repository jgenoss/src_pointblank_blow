// PBConfigDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CPBConfigDlg 대화 상자
class CPBConfigDlg : public CDialog
{
// 생성
public:
	CPBConfigDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_PBCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV 지원

protected:
	i3RenderContext *	m_pCtx;
	
	// 국가 ID
	// MSDN Language identifier
	LANGID				m_nLanguageID;
	INT32				m_nUIVersion;

	BOOL				m_bFullScreen;
	INT32				m_ScreenWidth;
	INT32				m_ScreenHeight;
	INT32				m_ScreenDepth;
	INT32				m_ScreenHz;
	BOOL				m_bVSync;
	BOOL				m_bTriLinearFilter;
	BOOL				m_b16BitsDepth;
	BOOL				m_bDynamicLight;
	BOOL				m_bNormalMap;
	BOOL				m_bSpecularMap;
	INT32				m_TexQuality;
	INT32				m_ShadowQuality;
	INT32				m_AA;
	INT32				m_AAQ;
	REAL32				m_Gamma;
	INT32				m_SpeakerType;
	BOOL				m_bBulletTrailer;
	BOOL				m_bTerrainEffect;
	CString m_EnvFileName;

	INT32 m_InterFace; //new 가 0이다.
	INT32 m_TeamBand;
	INT32 m_Parts, m_Chara;

protected:
	void		_UpdateResList( I3G_IMAGE_FORMAT fmt);
	void		_RemoveAllResList(void);
	void		_SetWindowText(void);

private:
	void LoadEnv(); //env 파일에 저장 값을 읽어 온다.
	void SetControlValue();
	void SaveEnv();
	void GetControlValue();
	

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_ResListCtrl;
	CComboBox m_ColorBitsCtrl;
	CButton m_DynamicLightCtrl;
	CButton m_NormalMapCtrl;
	CButton m_SpecularMapCtrl;
	CButton m_TriLinearCtrl;
	CButton m_VSyncCtrl;
	CComboBox m_TexQualityCtrl;
	CComboBox m_ShadowQualityCtrl;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	CButton m_CHK_WindowMode;

	CButton m_Btn_TeamBand;
	CButton m_Btn_InterFace[2];
	CButton m_Btn_Parts[2];
	CButton m_Btn_Chara[2];
	CComboBox m_UIVersion;
};
