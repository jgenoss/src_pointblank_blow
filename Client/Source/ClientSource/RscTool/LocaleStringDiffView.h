#pragma once


// LocaleStringDiffView 뷰입니다.
#include "i3MFCListCtrl.h"
struct PreprocessMessageCallback;

class LocaleStringDiffView : public CView
{
	DECLARE_DYNCREATE(LocaleStringDiffView)
public:
	void	UpdateLocaleTextFiles();
	void	OpenTxt( int list_idx );
	void	OpenPefTxtDiffDlg(int list_idx, enum PathMode m);



	void	Reset();
	
protected:
	LocaleStringDiffView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LocaleStringDiffView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:

	void		LoadLocalScripti3Pack(PreprocessMessageCallback* cb);
	void		LoadSVNScripti3Pack(PreprocessMessageCallback* cb);


	i3MFCListCtrl	m_listCtrl;
	i3::string		m_Stringi3PackPath;
	i3::string		m_Expi3PackPath;
	int				m_exported_revision;
	i3PackVolume*	m_exp_volume;
	i3PackVolume*	m_local_volume;

	i3::vector<i3::rc_string>	m_rel_path_list;
	i3::vector<i3::rc_wstring>	m_filename_list;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


