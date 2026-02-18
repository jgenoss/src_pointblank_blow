#if !defined( __I3_Level_AXIS_MOVE_H)
#define __I3_Level_AXIS_MOVE_H

#include "i3LevelAxis.h"

class I3_EXPORT_TDK i3LevelAxisMove : public i3LevelAxis
{
	I3_EXPORT_CLASS_DEFINE( i3LevelAxisMove, i3LevelAxis);

protected:
	i3Node *		m_pRect[3] = { nullptr, nullptr, nullptr };
	i3Transform *	m_pTrans[3] = { nullptr, nullptr, nullptr };
	i3Node *		m_pLine[3] = { nullptr, nullptr, nullptr };

public:
	BOOL	Create();

	void	ChangeLineColorX( COLORREAL * pColor );
	void	ChangeLineColorY( COLORREAL * pColor );
	void	ChangeLineColorZ( COLORREAL * pColor );

	void	ChangeRectColorAll( COLORREAL * pColor );
	void	ChangeRectColorX( COLORREAL * pColor );
	void	ChangeRectColorY( COLORREAL * pColor );
	void	ChangeRectColorZ( COLORREAL * pColor );

	virtual bool	OnCollision(  i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx) override;
};

#endif