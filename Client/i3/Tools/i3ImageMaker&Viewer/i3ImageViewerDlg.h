// i3ImageViewerDlg.h : 헤더 파일
//

#pragma once
#include "i3ImageContext.h"
#include "i3ClutTableDlg.h"

#define CHANNEL_ALL		0
#define CHANNEL_RED		1
#define CHANNEL_GREEN	2
#define CHANNEL_BLUE	3
#define CHANNEL_ALPHA	4
#define CHANNEL_RGB		5

#define MIPMAP_VIEW_ALL		0
#define MIPMAP_VIEW_LEVEL	1
#define MIPMAP_VIEW_NONE	2

// Ci3ImageViewerDlg 대화 상자
class Ci3ImageViewerDlg : public CDialog
{
// 생성
public:
	Ci3ImageViewerDlg(CWnd* pParent = NULL);	// 표준 생성자

private:
	char				m_szFileName[ MAX_PATH];
	i3Texture		*	m_pTexture;
	i3ImageFile			m_pImageFile;

	UINT				m_nMipMapLevelCount;	//텍스쳐가 가지고있는 밉맵 레벨의 카운트.
	UINT				m_CurMipMap_Level;
	DWORD				m_MipMap_ViewMode;

	//////////////////////////////////////////////////////////////////////////////////////////
	//밉맵 레벨에따른 서로 다른크기의 이미지들을 각각 가지고있도록함
	HBITMAP				m_hBitmap[MAX_MIPMAP_LEVEL] , m_hOldBitmap[MAX_MIPMAP_LEVEL];
	HDC					m_MemDC[MAX_MIPMAP_LEVEL];

	INT32				m_nTotalWidth;
	INT32				m_nTotalHeight;

	/////////////////////////////////////////////////////////////////////////////////////////
	POINT				m_posScroll;
	POINT				m_posMouse;		//윈도우 상의 마우스 포지션
	CSize				m_WndSize;

	REAL32				m_Zoom;
	//RightDrag
	BOOL				m_RButtonDown;
	POINT				m_RDragStart;
	CPoint				m_OldPos;

	BOOL				m_LButtonDown;
	POINT				m_LDragStart;
	CRect				m_LDragRect;
	/////////////////////////////////////////////////////////////////////////////////////////
	DWORD				m_ViewMode_Channel;
	Ci3ImageContext		*m_pImgContext;
	i3ClutTableDlg		*m_pClutTableDlg;

	//
	BOOL				m_bDrawProperty;
	const char*			m_szCmdLineArg;

	bool				m_bPopMipOptionDlg;
	i3RenderContext *	m_pContext;
	/////////////////////////////////////////////////////////////////////////////////////////
	bool				m_bDrawGrid;
	UINT32				m_nGridVertBlockCount;
	UINT32				m_nGridHoriBlockCount;
	INT32				m_nPenIndex;		// StockObject Index

public:
	char				m_strFileName[1024];
	HDROP				m_hDropInfo;

private:
	//
	void				MoveClutWindow();
	BOOL				CheckFileName( const char	*	szFileName);
	void				SetImage(i3Texture	*	pTexture);				  //백버퍼에 텍스쳐를 비트맵화하여 얹는다.
	void				SetImagetoDC(INT32 nLevel , i3Texture * pTexture , INT32	WidthPosInDC , INT32 HeightPosInDC);//밉밉 레벨에따라 텍스쳐를 비트맵화
	void				AlphaBlendFunc(char * pRaster, INT32 width, INT32 height);

	void				SetContext( const char * szFileName , i3Texture * pTexture);
	void				SetMipMapToDC(i3Texture * pTexture , UINT32 nLevelCount );
	//
	void				CalculateTotalSize(i3Texture	*	pTexture);
	void				SetGridtoDC(HDC	hdc , INT32	origin_x , INT32	origin_y , INT32 width , INT32	height );//DC에 그리드를 만들어준다.//DC에 그리드를 만들어준다.
	void				GetRGBValue(INT32 R , INT32 G , INT32 B , COLORREF& DST);
	void				AlphaBlendFunc(HDC DestDC , BYTE * pSrc , INT32	Width , INT32 Height , INT32 DestPosX  , INT32 DestPosY);
	void				DeleteImage();									//사용된 dc and bitmap을 delete한다.

	void				OnUpdateCommand_Check(CMenu * pPopupMenu , UINT nIndex  , BOOL bCheck);
	void				OnUpdateCommandUI_Range_Check(CMenu * pPopupMenu , UINT nStartIndex , UINT Range , UINT nID);
	void				OnUpdateCommandUI_Range_Enable(CMenu * pPopupMenu , UINT nStartIndex , UINT Range , UINT nCount);

	//
	void				Scroll(int nBar , int ScrollAmount , LONG& pos);
	void				ResetScroll(int nBar , LONG& pos);
	void				SetScroll();

	//
	void				DrawGrid(CDC * pDC, CPoint StartPos);

	//
	void				TypeProperty(CPaintDC * hdc , Ci3ImageContext * ImgContext);
	// 대화 상자 데이터
	enum { IDD = IDD_I3IMAGEVIEWER_DIALOG };

	BOOL				OpenImageFile( const char *szFileName );
	BOOL				SaveImageFile( const char * pszPath);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;
	HACCEL m_hAccel;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileExit();
	afx_msg void OnFileOpen();
	afx_msg void OnChangeMode(UINT nID);
	afx_msg void OnMipMap(UINT nID);
	afx_msg void OnChangeLevel(UINT nID);
	afx_msg void OnViewProperty();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	void				SetCommandLineArg( const char *szCmdLineArg ){ m_szCmdLineArg = szCmdLineArg; }

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileExportmipmaps();
	afx_msg void OnFileGeneratemipmaps();
	afx_msg void OnFileLoadmipmaps();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnGridDrawgrid();
	afx_msg void OnGridGridsetting();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
