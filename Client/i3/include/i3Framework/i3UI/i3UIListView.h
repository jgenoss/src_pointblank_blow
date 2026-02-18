#ifndef __I3UI_LISTVIEW_H__
#define __I3UI_LISTVIEW_H__

#include "i3UIControl.h"		
#include "i3UIScrollBar.h"

typedef INT32 (*ITEMSETPROC)(void * pItem, void*pItemTemplate);

class I3_EXPORT_FRAMEWORK i3UIListView : public i3UIControl	
{
	I3_CLASS_DEFINE( i3UIListView);

protected:
	INT32			m_nColCount;
	INT32			m_nMaxRow;

	i3UIScrollBar	*m_pUIScrollBar;
	REAL32			m_fWidthScrollBar;

	i3UITemplate	*m_LVItemTemplate;
	i3List			m_ItemList;

	void			_UpdateScrollBarRect( void);		//Rect만 Update
	void			_UpdateScrollBarValue( void);		//Page Size등을 Update 후에 _UpdateScrollBarRect를 call

	// Click했을 때 Block처리 하기 위함
	INT32			_GetItemIndexByPos( INT32 x, INT32 y);
	I3RECT			_GetItemRectByIndex( INT32 idx);
	void			_SetBlock( I3RECT* pRt);

	virtual void	_UpdateShapeState(void);	// 현재 state에 따른 visible state를 update한다.

public:
	i3UIListView();
	virtual ~i3UIListView();

	virtual void	OnEnabled( bool bEnable);
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);

	virtual void	SetItemSetCallBack( ITEMSETPROC * pProc);

	virtual void	OnSize( REAL32 cx, REAL32 cy);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void	OnNotify( i3UIControl * pCtrl, UINT32 event, INT32 param1, INT32 param2);

	virtual void	OnBindTemplate(void);

	void			GetItem( INT32 idx, char* pBuf, INT32 nBufSize);

	virtual void	UpdateRenderObjects();

	//
	virtual void	Init( void);
	virtual void	OnInitAfterLoad();

	virtual void	GetTDKGridProperties(i3List * pList);
	virtual void	ApplyChangesFromPropertyGrid( void);
};

#endif //__I3UI_LISTBOX_H__