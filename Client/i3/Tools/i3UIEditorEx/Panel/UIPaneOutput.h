#pragma once

//class COutputEdit;
#include "..\CEdit_Output.h"

#include "UIPaneBase.h"

enum MSG_TYPE
{
	MSG_NORMAL = 0,
	MSG_COMPILE,
	
	MSG_COUNT
};

// CUIPaneOutput

class CUIPaneOutput : public CUIPaneBase
{
	DECLARE_DYNAMIC(CUIPaneOutput)

protected:
	CFont			m_Font;

	COutputEdit			m_Edit;
	char			m_Msg[4096];

protected:
	DECLARE_MESSAGE_MAP()

	void		_OutputString( MSG_TYPE type, const char* pszString, bool bConcat);
public:
	CUIPaneOutput();
	virtual ~CUIPaneOutput();

	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	void		OutputCompileResult( const char* pszString, bool bConcat = false);
	void		OutputNotice( const char* pszString, bool bConcat = false);

};


