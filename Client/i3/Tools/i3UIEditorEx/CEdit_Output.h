//#ifndef __CEDIT_OUTPUT_H__
//#define __CEDIT_OUTPUT_H__

#pragma once
#include "afxwin.h"

class COutputEdit : public CEdit
{
	DECLARE_DYNAMIC( COutputEdit);		//??

protected:
	DECLARE_MESSAGE_MAP()

	void _ClearAll( void);

	//virtual BOOL	OnCommand( WPARAM wParam, LPARAM lParam);
	afx_msg void	OnContextMenu( CWnd* pWnd, CPoint pos);

public:
	COutputEdit()	{}
	virtual ~COutputEdit()	{}

};

//#endif	//__CEDIT_OUTPUT_H__