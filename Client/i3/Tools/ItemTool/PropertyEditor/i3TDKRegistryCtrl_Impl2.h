#pragma once

#include "../../PropertyEditor/i3TDKRegistryCtrl_Impl.h"



class i3TDKRegistryCtrl_Impl2 :
	public i3TDKRegistryCtrl_Impl
{

protected:
	DECLARE_MESSAGE_MAP()

public:
	i3TDKRegistryCtrl_Impl2(void);
	virtual ~i3TDKRegistryCtrl_Impl2(void);

	virtual void		_AddNewKey(CTreeCtrl & TreeCtrl, HTREEITEM hParentItem);
	virtual void		_DeleteKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	virtual void		_RenameKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	virtual void		_CopyKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	virtual void		_PasteKey(CTreeCtrl & TreeCtrl,  HTREEITEM hItem, const  CString& strClonedName = "", INT32 nNumber = -1);
	virtual void		_PasteKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	
	HTREEITEM	GetHItem(const std::string& str) { return m_mHitemList[str].hItem; }
	void		GetHitemFullPath(CString& resPath, HTREEITEM item);
	
	INT32		InsertItemList(INT32 nClassIndex,const CString& strCloneName);

	void		OnSelectItem(const char* szName);
	void		OnCloneItem(const char* szName, const char* szClonedName);
	

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
