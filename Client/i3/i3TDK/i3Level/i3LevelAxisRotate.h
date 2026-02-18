#if !defined( __I3_Level_AXIS_ROTATE_H)
#define __I3_Level_AXIS_ROTATE_H

#include "i3LevelAxis.h"
#include "i3LevelViewportControl.h"

#define		NUM_LINE_SEG		30

class I3_EXPORT_TDK i3LevelAxisRotate : public i3LevelAxis
{
	I3_EXPORT_CLASS_DEFINE( i3LevelAxisRotate, i3LevelAxis);

protected:
	i3Transform *		m_pRotateRoot = nullptr;
	i3Node *			m_pRotateLine = nullptr;
	i3Transform *		m_pTrans[3] = { nullptr, nullptr, nullptr };
	i3Transform *		m_pAxisTrans[3] = { nullptr, nullptr, nullptr };
	i3Node *			m_pLine[3] = { nullptr, nullptr, nullptr };

	VEC3D				m_LineVec[NUM_LINE_SEG];		// [initialize at constructor]
	VEC3D				m_PickedPos;

protected:

	bool		_HitTestWithLine( VEC3D * pStart, VEC3D * pEnd);

public:
	i3LevelAxisRotate(void);
	virtual ~i3LevelAxisRotate(void);

	BOOL	Create( void );

	VEC3D *	getPickedPos(void)				{ return &m_PickedPos; }

	void	RotateArc( I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pRotate );

	void	RotateLine( VEC3D * pRotate );
	BOOL	RotateLineX( REAL32 fDeg );
	BOOL	RotateLineY( REAL32 fDeg );
	BOOL	RotateLineZ( REAL32 fDeg );

	virtual bool	OnCollision(  i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx) override;
};

#endif