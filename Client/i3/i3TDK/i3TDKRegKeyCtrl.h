#pragma once

// i3RegKeyCtrl
class I3_EXPORT_TDK i3TDKRegKeyCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(i3TDKRegKeyCtrl)

protected:
	i3RegKey * m_pKey = nullptr;

	CFont		m_Font;
public:
	i3TDKRegKeyCtrl() {}
	virtual ~i3TDKRegKeyCtrl();

	void	SetRegKey( i3RegKey * pKey);
	i3RegKey	*	getRegKey()	{	return m_pKey;}

	void	AddNewData( INT32 nType);
	void	AddTimeStampData( const TCHAR* pszName, const TCHAR* pszTime, const TCHAR* pszTimeString );		//
	void	UpdateTimeStampData( const TCHAR* pszName, const TCHAR* pszTime, const TCHAR* pszTimeString );		//
	void	DeleteData( INT32 nIdx);
	void	RenameData( INT32 nIdx);
	void	PasteData( i3RegData * pData);

	void UpdateItem( INT32 nRow);
protected:
	void _UpdateKey();
	INT32 _AddData( i3RegData * pData);
	INT32 _AddData( i3RegData * pData, const TCHAR* pszDefault );

	void _Convert2String( i3::stack_string& conv, i3RegData * pData);		// ¾ðÁ¨°¡ char¹öÀü Æó±â
	void _Convert2String( i3::stack_wstring& conv, i3RegData * pData);
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};

