#if !defined( __I3_Level_AXIS_SCALE_H)
#define __I3_Level_AXIS_SCALE_H

#include "i3LevelAxis.h"

class I3_EXPORT_TDK i3LevelAxisScale : public i3LevelAxis
{
	I3_EXPORT_CLASS_DEFINE( i3LevelAxisScale, i3LevelAxis);

protected:
	i3Transform *		m_pTrans[3] = { nullptr, nullptr, nullptr };
	i3Node *			m_pCap[3] = { nullptr, nullptr, nullptr };
	i3Node *			m_pLine[3] = { nullptr, nullptr, nullptr };

	i3Transform *		m_pGrip_Trans[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	i3AttrSet *			m_pGrip_AttrSet[6] = { nullptr, nullptr, nullptr,nullptr, nullptr, nullptr };
	i3ColorAttr *		m_pGrip_Color[6] = { nullptr, nullptr, nullptr,nullptr, nullptr, nullptr };

public:
	BOOL	Create( void );
	void	Update( void );

	virtual void	OnUpdate( REAL32 tm) override;
	virtual void	OnUpdateTransform(void) override;
	virtual bool	OnCollision( i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx) override;
};

#endif