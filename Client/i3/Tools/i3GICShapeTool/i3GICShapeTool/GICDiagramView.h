#if !defined( __GIC_DIAGRAM_VIEW_H__)
#define __GIC_DIAGRAM_VIEW_H__

class CGICDiagramView : public i3GICDiagramView
{
	DECLARE_DYNAMIC( CGICDiagramView);

private:

protected:
	void			_DrawGrid( REAL32 x, REAL32 y);

public:
	CGICDiagramView();
	virtual ~CGICDiagramView();

	virtual void	OnSelectShape( i3GICShape * pShape, GIC_MOUSEMODE mode);
	virtual void	OnUnselectShape( void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnPaint();
	afx_msg void	OnSize( UINT nType, int cx, int cy);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp( UINT nFlags, CPoint point);
};

#endif
