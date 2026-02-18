#pragma once
#include "listsubctrl.h"

class CListSubCtrl_Impl : public CListSubCtrl
{
public:
	CListSubCtrl_Impl(void);
	~CListSubCtrl_Impl(void);

	virtual void NotifyHeaderColumn() override;
	virtual void EndSubControl(int nRow, int nCol, LPTSTR String) override;

	String GetNationName(size_t Index);
};
