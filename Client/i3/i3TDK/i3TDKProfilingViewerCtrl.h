#if !defined( __I3_TDK_PROFILINGVIEWER_H)
#define __I3_TDK_PROFILINGVIEWER_H

//	=================================================================================
//	i3TDKProfilingViewerCtrl
//	Programed : komet
//	Build Date : 2006.12.20
//	Last Update : 2006.12.20
//	=================================================================================

#include "i3TDKListCtrl.h"


struct TreeItemNode
{
	REAL_PROFILE_NODE *		_pProfile = nullptr;
	HTREEITEM				_hItem = nullptr;
	HTREEITEM				_hParent = nullptr;

	BOOL					_bChildExpanded = FALSE;

} ;

struct HistoryGraphNode
{	
	const char *		_pszName = nullptr;
	COLORREF			_color = 0;
	INT32				_nUsage = 0;

} ;

// i3TDKProfilingViewerCtrl
#if !defined( I3_NO_PROFILE)

class I3_EXPORT_TDK i3TDKProfilingViewerCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKProfilingViewerCtrl)

public:
	i3TDKProfilingViewerCtrl() {}
	virtual ~i3TDKProfilingViewerCtrl();

	enum {
		PROFILING_TREE = 110,
		PROFILING_LIST = 111,
		PROFILING_VIEW = 112,
	};

protected:
	CTreeCtrl		m_ProfilingTreeCtrl;
	i3TDKListCtrl	m_ProfilingListCtrl;
	CWnd			m_ProfilingHistory;
	CEdit			m_UpdateTimeEdit;
	CButton			m_BtnApply;

	CRect			m_rtProfilingTreeCtrl;
	CRect			m_rtProfilingListCtrl;
	CRect			m_rtProfilingHistory;
	CFont			m_CaptionFont;

	i3::vector<REAL_PROFILE_NODE*>				m_ListNodeArray;
	i3::vector<i3::vector<HistoryGraphNode*> > m_HistoryGraphArray;

	DWORD		m_dwStyle = 0;

	HANDLE		m_hTimer = nullptr;
	INT32		m_TimerSec = 0;
	INT32		m_TotalNodeCount = 0;
	BOOL		m_bCreate = FALSE;

	HDC			m_hDrawDC = nullptr;
	HBITMAP		m_hbmpDraw = nullptr, m_hbmpOld = nullptr;

	UINT32		m_nTimerElapse = 0;
	INT32		m_nDrawWidth = 0;
	INT32		m_nDrawHeight = 0;
	INT32		m_nWidthGridCount = 20;
	INT32		m_nHeightGridCount = 10;
	INT32		m_nScrollPixel = 0;

	HTREEITEM				m_hRootItem = nullptr;
	REAL_PROFILE_NODE *		m_pRoot = nullptr;
	REAL_PROFILE_NODE *		m_pCurNode = nullptr;

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL	Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr);

	void	InitMain( void);
	void	InitProfilingTree( void);
	void	InitProfilingList( void);
	void	InitProfilingHistory( void);

	void	SetUpdateTime( UINT32 timeElapse = 1000);
	UINT32	GetUpdateTime( void)					{ return m_nTimerElapse;						}
	virtual void	UpdateReport( REAL_PROFILE_NODE * pRoot);
	
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void	_MakeProfilingTree( REAL_PROFILE_NODE * pRoot);
	void	_MakeProfilingListNode( REAL_PROFILE_NODE * pNode);
	void	_DeleteAllProfilingNode( void);
	INT32	_AddProfilingNode( REAL_PROFILE_NODE * pNode, HTREEITEM hParent);

	void	_MakeProfilingList( REAL_PROFILE_NODE * pRoot);
	void	_DeleteAllProfilingList( void);

	void	_MakeProfilingHistoryGraph( void);
	void	_DeleteAllProfilingHistoryGraph( void);

	void	_DrawHistoryBG( void);
	void	_DrawHistoryGraph( void);
	void	_DrawGroupBox( const TCHAR * pszCaption, INT32 x1, INT32 y1, INT32 x2, INT32 y2);
	void	_DrawGroupBox( const TCHAR * pszCaption, RECT * rt);

	INT32	_UpdateUsageList( REAL_PROFILE_NODE * pNode);
	void	_UpdateUsageHistory( REAL_PROFILE_NODE * pNode);

	void	_SelectProfilingNode( HTREEITEM hItem);
	void	_CalcTimeSec( void);
	void	_UpdateTimer();

	void	ProfilingTreeCtrl_SetSize( CRect * pRect);
	void	ProfilingListCtrl_SetSize( CRect * pRect);
	void	ProfilingHistory_SetSize( CRect * pRect);
		
public:
	//virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnNMLClickTree( NMHDR * pNMHDR, LRESULT * pResult);
};

#endif

#if defined( I3_DEBUG)
	#define __CHECKW( val, str)				{	if( !(val)) { MessageBox( str, _T("WARNING"), MB_OK); return;		}}
	#define __CHECKW_RT( val, str, ret)		{	if( !(val)) { MessageBox( str, _T("WARNING"), MB_OK); return ret;	}}
	#define __CHECKF( val, str)				{	if( !(val)) { MessageBox( str, _T("Error"), MB_OK); I3ASSERT(0);	return; }}
	#define __CHECKF_RT( val, str, ret)		{	if( !(val)) { MessageBox( str, _T("Error"), MB_OK); I3ASSERT(0);	return ret; }}
#else
	#define	__CHECKW( val, str)				__noop
	#define __CHECKW_RT( val, str, ret)		__noop
	#define __CHECKF( val, str)				__noop
	#define __CHECKF_RT( val, str, ret)		__noop
#endif

#endif