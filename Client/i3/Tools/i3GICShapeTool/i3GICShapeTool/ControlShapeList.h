#pragma once

class CControlShapeList : public CListCtrl
{
	DECLARE_DYNAMIC(CControlShapeList)

protected:
	i3List			m_ShapeList;

protected:
	void			_AddIcon( INT32 id);
	const char *	_GetShapeTypeName( i3GICShape * pShape);
	INT32			_GetIconIndex( i3GICShape * pShape);
	void			_SetShapeInfo( LVITEM * pInfo, i3GICShape * pShape);

public:
	CControlShapeList();
	virtual ~CControlShapeList();

	INT32			AddShape( i3GICShape * pShape);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnMouseMove( UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp( UINT nFlags, CPoint point);
};