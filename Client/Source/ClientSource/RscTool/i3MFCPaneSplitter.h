#pragma once

//
//  XDockPane 소스를 참고했다.   (http://www.codeproject.com/Members/huzifa30)
//

class i3MFCPaneSplitter : public CSplitterWndEx
{
DECLARE_DYNAMIC(i3MFCPaneSplitter)
public:

	BOOL AddWindow(int row, int col, CWnd* pWnd, const CString& className, DWORD dwStyle, DWORD dwStyleEx, const CSize& sizeInit);
};
