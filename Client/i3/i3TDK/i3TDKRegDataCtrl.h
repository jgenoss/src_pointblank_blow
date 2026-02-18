#pragma once

#include "i3TDKListCtrl.h"
#include "i3TDKUpdateSystem.h"

// i3RegDataCtrl
class I3_EXPORT_TDK i3TDKRegDataCtrl : public i3TDKListCtrl
{
	DECLARE_DYNAMIC(i3TDKRegDataCtrl)
protected:
	i3RegData				*	m_pData = nullptr;

public:
	i3TDKRegDataCtrl() {}
	virtual ~i3TDKRegDataCtrl();

	void	SetData( i3RegData * pData);

protected:
	DECLARE_MESSAGE_MAP()

	void	_InitData( void);
		
	void	_InitINT32Data();
	void	_InitREAL32Data();
	void	_InitSTRINGData();
	void	_InitVEC2DData();
	void	_InitVEC3DData();
	void	_InitVEC4DData();
	void	_InitCOLORData();
	void	_InitMATRIXData();
	void	_InitTimeData();

	void					__InitVEC4D( const TCHAR* szTitle,  INT32 nRow, INT32 nFirstID, VEC4D * pVec);
	i3TDKPropertySubItem *	_CreateNewItem( INT32 nRow, INT32 nCol, i3ClassMeta * pMeta, const TCHAR* pszName, INT32 nID);

	
	void	_GetINT32Data( i3TDKPropertySubItem * pItem);	
	void	_GetREAL32Data( i3TDKPropertySubItem * pItem);	
	void	_GetSTRINGData( i3TDKPropertySubItem * pItem);	
	void	_GetVEC2DData( i3TDKPropertySubItem * pItem);	
	void	_GetVEC3DData( i3TDKPropertySubItem * pItem);	
	void	_GetVEC4DData( i3TDKPropertySubItem * pItem);	
	void	_GetCOLORData( i3TDKPropertySubItem * pItem);	
	void	_GetMATRIXData( i3TDKPropertySubItem * pItem);
	
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


