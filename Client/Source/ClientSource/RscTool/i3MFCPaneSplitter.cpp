#include "stdafx.h"
#include "i3MFCPaneSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(i3MFCPaneSplitter, CSplitterWndEx)

BOOL i3MFCPaneSplitter::AddWindow(int row, int col, CWnd* pWnd , const CString& className ,
	DWORD dwStyle,DWORD dwStyleEx, const CSize& sizeInit)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(row >= 0 && row < m_nRows);
	ASSERT(col >= 0 && col < m_nCols);
	ASSERT(pWnd);

	if (GetDlgItem(IdFromRowCol(row, col)) != NULL)
	{
		TRACE(traceAppMsg, 0, "Error: CreateView - pane already exists for row %d, col %d.\n",
			row, col);
		ASSERT(FALSE);
		return FALSE;
	}
#endif

	// set the initial size for that pane
	m_pColInfo[col].nIdealSize = sizeInit.cx;
	m_pRowInfo[row].nIdealSize = sizeInit.cy;
	ASSERT(pWnd->m_hWnd == NULL);       // not yet created
	// Create with the right size (wrong position)
	CRect rect(CPoint(0,0), sizeInit);
	if(!pWnd->CreateEx(dwStyleEx,className,NULL,dwStyle,rect,this,IdFromRowCol(row, col)))
	{
		TRACE(traceAppMsg, 0, "Warning: couldn't create client pane for splitter.\n");
		return FALSE;
	}
	return TRUE;
}