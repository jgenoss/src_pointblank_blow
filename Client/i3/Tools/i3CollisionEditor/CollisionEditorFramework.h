#if !defined( _COLLISION_EDITOR_FRAMEWORK_H)
#define _COLLISION_EDITOR_FRAMEWORK_H

class CollisionEditorFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( CollisionEditorFramework);
protected:
	bool	m_bMultiSelect;

	CRect	m_OldDragRect;

protected:
	void	_DrawSelectRect(void);
	
public:
	CollisionEditorFramework(void);
	virtual ~CollisionEditorFramework(void);

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
};

#endif
