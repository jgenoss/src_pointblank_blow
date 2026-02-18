#if !defined( __I3_TDK_UTIL_CTRL_H)
#define __I3_TDK_UTIL_CTRL_H

typedef BOOL	(*I3_TREECTRL_TRAVERSE_PROC)( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData);
typedef BOOL	(*I3_ENABLE_CHILDWND_PROC)( void * pData );

namespace i3TDK
{
	// Tree Control
	I3_EXPORT_TDK	HTREEITEM	MoveTreeItem( CTreeCtrl * pCtrl, HTREEITEM hNewParent, HTREEITEM hMoveItem);
	I3_EXPORT_TDK	HTREEITEM	FindTreeItemByData( CTreeCtrl * pCtrl, void * pData, HTREEITEM hParent = TVI_ROOT, BOOL bRecurse = TRUE);
	I3_EXPORT_TDK	void		TreeCtrlTraverse( CTreeCtrl * pCtrl, I3_TREECTRL_TRAVERSE_PROC pProc, void * pData, HTREEITEM hRoot = TVI_ROOT);
	I3_EXPORT_TDK	void		TreeCtrlTraverse( HWND hwnd, I3_TREECTRL_TRAVERSE_PROC pProc, void * pData, HTREEITEM hRoot = TVI_ROOT);
	I3_EXPORT_TDK	void		DeleteAllChildItem( CTreeCtrl * pCtrl, HTREEITEM hParent);
	I3_EXPORT_TDK	void		EnableChildCtrl( HWND hWnd, BOOL bEnable );
	I3_EXPORT_TDK	INT32		FindItemByData( CComboBox * pCtrl, DWORD_PTR data);
	I3_EXPORT_TDK	void		SetCursor( HWND hwnd, HCURSOR hCursor);

	enum EBatchAlignCtlFlag
	{
		EBATCH_ALGNCTL_NONE			= 0,
		EBATCH_ALGNCTL_LEFT			= 0x00000001,
		EBATCH_ALGNCTL_TOP			= 0x00000002,
		EBATCH_ALGNCTL_RIGHT		= 0x00000004,
		EBATCH_ALGNCTL_BOTTOM		= 0x00000008,
		EBATCH_ALGNCTL_MAX		
	};

	I3_EXPORT_TDK void BatchAlignCtrls(DWORD alignFlag/*EBatchAlignCtlFlag*/, INT32 paddX, INT32 paddY/*0*/, 
		CWnd* parent, ... /* ,wnd0, wnd1, nullptr */);
};


#endif
